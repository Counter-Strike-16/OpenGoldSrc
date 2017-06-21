//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#if !defined( _X360 )
#include "winheaders.h"
#endif
#include <stdio.h>
#include "UtlBuffer.h"
#include <vgui/VGUI2.h>
#include <vgui_controls/Controls.h>
#include "FileSystem.h"

#if defined( _X360 )
#include "xbox/xbox_win32stubs.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui2;

#define	TYP_LUMPY		64				// 64 + grab command number

typedef struct
{
	char		identification[ 4 ];		// should be WAD2 or 2DAW
	int			numlumps;
	int			infotableofs;
} wadinfo_t;

typedef struct
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[ 16 ];				// must be null terminated
} lumpinfo_t;

typedef struct
{
	char		name[ 16 ];
	unsigned	width, height;
	unsigned	offsets[ 4 ];		// four mip maps stored
} miptex_t;

unsigned char	pixdata[ 256 ];

float 	linearpalette[ 256 ][ 3 ];
float 	d_red, d_green, d_blue;
int		colors_used;
int		color_used[ 256 ];
float	maxdistortion;
unsigned char palLogo[ 768 ];

/*
=============
AveragePixels
=============
*/
unsigned char AveragePixels( int count )
{
	return pixdata[ 0 ];
}

/*
==============
GrabMip

filename MIP x y width height
must be multiples of sixteen
==============
*/
int GrabMip( HANDLE hdib, unsigned char *lump_p, char *lumpname, COLORREF crf, int *width, int *height )
{
	int             i, x, y, xl, yl, xh, yh, w, h;
	unsigned char   *screen_p, *source;
	miptex_t		*qtex;
	int				miplevel, mipstep;
	int				xx, yy;
	int				count;
	int				byteimagewidth, byteimageheight;
	unsigned char   *byteimage;
	LPBITMAPINFO	lpbmi;      // pointer to BITMAPINFO structure (Win3.0)

								/* get pointer to BITMAPINFO (Win 3.0) */
	lpbmi = ( LPBITMAPINFO )::GlobalLock( ( HGLOBAL ) hdib );
	unsigned char *lump_start = lump_p;

	xl = yl = 0;
	w = lpbmi->bmiHeader.biWidth;
	h = lpbmi->bmiHeader.biHeight;

	*width = w;
	*height = h;

	byteimage = ( unsigned char * ) ( ( LPSTR ) lpbmi + sizeof( BITMAPINFOHEADER ) + 256 * sizeof( RGBQUAD ) );

	if( ( w & 15 ) || ( h & 15 ) )
		return 0; //Error ("line %i: miptex sizes must be multiples of 16", scriptline);

	xh = xl + w;
	yh = yl + h;

	qtex = ( miptex_t * ) lump_p;
	qtex->width = ( unsigned ) ( w );
	qtex->height = ( unsigned ) ( h );
	Q_strncpy( qtex->name, lumpname, sizeof( qtex->name ) );

	lump_p = ( unsigned char * ) &qtex->offsets[ 4 ];

	byteimagewidth = w;
	byteimageheight = h;

	source = ( unsigned char * ) lump_p;
	qtex->offsets[ 0 ] = ( unsigned ) ( ( unsigned char * ) lump_p - ( unsigned char * ) qtex );

	// We're reading from a dib, so go bottom up
	screen_p = byteimage + ( h - 1 ) * w;
	for( y = yl; y<yh; y++ )
	{
		for( x = xl; x<xh; x++ )
			*lump_p++ = *screen_p++;

		screen_p -= 2 * w;
	}

	// calculate gamma corrected linear palette
	for( i = 0; i < 256; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			float f = ( float ) ( palLogo[ i * 3 + j ] / 255.0 );
			linearpalette[ i ][ j ] = f; //pow((double)f, 2); // assume textures are done at 2.2, we want to remap them at 1.0
		}
	}

	maxdistortion = 0;
	// assume palette full if it's a transparent texture
	colors_used = 256;
	for( i = 0; i < 256; i++ )
		color_used[ i ] = 1;


	//
	// subsample for greater mip levels
	//

	for( miplevel = 1; miplevel<4; miplevel++ )
	{
		d_red = d_green = d_blue = 0;	// no distortion yet
		qtex->offsets[ miplevel ] = ( unsigned ) ( lump_p - ( unsigned char * ) qtex );

		mipstep = 1 << miplevel;

		for( y = 0; y<h; y += mipstep )
		{
			for( x = 0; x<w; x += mipstep )
			{
				count = 0;
				for( yy = 0; yy<mipstep; yy++ )
				{
					for( xx = 0; xx<mipstep; xx++ )
						pixdata[ count++ ] = source[ ( y + yy )*w + x + xx ];
				}

				*lump_p++ = AveragePixels( count );
			}
		}
	}

	::GlobalUnlock( lpbmi );

	// Write out palette in 16bit mode
	*( unsigned short * ) lump_p = 256;	// palette size
	lump_p += sizeof( short );

	memcpy( lump_p, &palLogo[ 0 ], 765 );
	lump_p += 765;

	*lump_p++ = ( unsigned char ) ( crf & 0xFF );

	*lump_p++ = ( unsigned char ) ( ( crf >> 8 ) & 0xFF );

	*lump_p++ = ( unsigned char ) ( ( crf >> 16 ) & 0xFF );

	return lump_p - lump_start;
}

int GrabMipFromTGA( byte* byteimage, unsigned char *lump_p, char *lumpname, COLORREF crf, int width, int height )
{
	int             i, x, y, xl, yl, xh, yh;
	unsigned char   *screen_p, *source;
	miptex_t		*qtex;
	int				miplevel, mipstep;
	int				xx, yy;
	int				count;
	int				byteimagewidth, byteimageheight;

	if( ( width & 15 ) || ( height & 15 ) )
		return 0; //Error ("line %i: miptex sizes must be multiples of 16", scriptline);

	xl = yl = 0;
	unsigned char *lump_start = lump_p;

	xh = xl + width;
	yh = yl + height;

	qtex = ( miptex_t * ) lump_p;
	qtex->width = ( unsigned ) ( width );
	qtex->height = ( unsigned ) ( height );
	Q_strncpy( qtex->name, lumpname, sizeof( qtex->name ) );

	lump_p = ( unsigned char * ) &qtex->offsets[ 4 ];

	byteimagewidth = width;
	byteimageheight = height;

	source = ( unsigned char * ) lump_p;
	qtex->offsets[ 0 ] = ( unsigned ) ( ( unsigned char * ) lump_p - ( unsigned char * ) qtex );

	//TODO: needed for TGA? - Solokiller
	// We're reading from a dib, so go bottom up
	screen_p = byteimage + ( height - 1 ) * width;
	for( y = yl; y<yh; y++ )
	{
		for( x = xl; x<xh; x++ )
			*lump_p++ = *screen_p++;

		screen_p -= 2 * width;
	}

	// calculate gamma corrected linear palette
	for( i = 0; i < 256; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			float f = ( float ) ( palLogo[ i * 3 + j ] / 255.0 );
			linearpalette[ i ][ j ] = f; //pow((double)f, 2); // assume textures are done at 2.2, we want to remap them at 1.0
		}
	}

	maxdistortion = 0;
	// assume palette full if it's a transparent texture
	colors_used = 256;
	for( i = 0; i < 256; i++ )
		color_used[ i ] = 1;


	//
	// subsample for greater mip levels
	//

	for( miplevel = 1; miplevel<4; miplevel++ )
	{
		d_red = d_green = d_blue = 0;	// no distortion yet
		qtex->offsets[ miplevel ] = ( unsigned ) ( lump_p - ( unsigned char * ) qtex );

		mipstep = 1 << miplevel;

		for( y = 0; y<height; y += mipstep )
		{
			for( x = 0; x<width; x += mipstep )
			{
				count = 0;
				for( yy = 0; yy<mipstep; yy++ )
				{
					for( xx = 0; xx<mipstep; xx++ )
						pixdata[ count++ ] = source[ ( y + yy )*width + x + xx ];
				}

				*lump_p++ = AveragePixels( count );
			}
		}
	}

	// Write out palette in 16bit mode
	*( unsigned short * ) lump_p = 256;	// palette size
	lump_p += sizeof( short );

	memcpy( lump_p, &palLogo[ 0 ], 765 );
	lump_p += 765;

	*lump_p++ = ( unsigned char ) ( crf & 0xFF );

	*lump_p++ = ( unsigned char ) ( ( crf >> 8 ) & 0xFF );

	*lump_p++ = ( unsigned char ) ( ( crf >> 16 ) & 0xFF );

	return lump_p - lump_start;
}

void UpdateLogoWAD( void *phdib, int r, int g, int b )
{
	char logoname[ 32 ];
	char *pszName;
	Q_strncpy( logoname, "LOGO", sizeof( logoname ) );
	pszName = &logoname[ 0 ];

	HANDLE hdib = ( HANDLE ) phdib;
	COLORREF crf = RGB( r, g, b );

	if( ( !pszName ) || ( pszName[ 0 ] == 0 ) || ( hdib == NULL ) )
		return;
	// Generate lump

	unsigned char *buf = ( unsigned char * ) _alloca( 20480 );

	CUtlBuffer buffer( 0, 20480 );

	int width, height;

	int length = GrabMip( hdib, buf, pszName, crf, &width, &height );
	if( length == 0 )
	{
		return;
	}

	bool sizevalid = false;

	if( width == height )
	{
		if( width == 16 ||
			width == 32 ||
			width == 64 )
		{
			sizevalid = true;
		}
	}

	if( !sizevalid )
		return;

	while( length & 3 )
		length++;

	// Write Header
	wadinfo_t	header;
	header.identification[ 0 ] = 'W';
	header.identification[ 1 ] = 'A';
	header.identification[ 2 ] = 'D';
	header.identification[ 3 ] = '3';
	header.numlumps = 1;
	header.infotableofs = 0;

	buffer.Put( &header, sizeof( wadinfo_t ) );

	// Fill Ino info table
	lumpinfo_t	info;
	Q_memset( &info, 0, sizeof( info ) );
	Q_strncpy( info.name, pszName, sizeof( info.name ) );
	info.filepos = ( int )sizeof( wadinfo_t );
	info.size = info.disksize = length;
	info.type = TYP_LUMPY;
	info.compression = 0;

	// Write Lump
	buffer.Put( buf, length );

	// Write info table
	buffer.Put( &info, sizeof( lumpinfo_t ) );

	int savepos = buffer.TellPut();

	buffer.SeekPut( CUtlBuffer::SEEK_HEAD, 0 );

	header.infotableofs = length + sizeof( wadinfo_t );

	buffer.Put( &header, sizeof( wadinfo_t ) );

	buffer.SeekPut( CUtlBuffer::SEEK_HEAD, savepos );

	// Output to file
	FileHandle_t file;
	file = filesystem()->Open( "tempdecal.wad", "wb", "GAMECONFIG" );
	if( file != FILESYSTEM_INVALID_HANDLE )
	{
		filesystem()->Write( buffer.Base(), buffer.TellPut(), file );
		filesystem()->Close( file );
	}

}

typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;


TargaHeader		targa_header;

int fgetLittleShort( byte** ppBuffer )
{
	byte b1 = ( *ppBuffer )[ 0 ];
	byte b2 = ( *ppBuffer )[ 1 ];

	*ppBuffer += 2;

	return ( short ) ( b1 + b2 * 256 );
}

int fgetLittleLong( byte** ppBuffer )
{
	byte b1 = ( *ppBuffer )[ 0 ];
	byte b2 = ( *ppBuffer )[ 1 ];
	byte b3 = ( *ppBuffer )[ 2 ];
	byte b4 = ( *ppBuffer )[ 3 ];

	*ppBuffer += 4;

	return b1 + ( b2 << 8 ) + ( b3 << 16 ) + ( b4 << 24 );
}

bool LoadTGA( const char *szFilename, byte *buffer, int bufferSize, int *width, int *height )
{
	auto hFile = vgui2::filesystem()->Open( szFilename, "rb" );

	auto size = vgui2::filesystem()->Size( hFile );

	auto pFileBuffer = reinterpret_cast<byte*>( stackalloc( size ) );

	vgui2::filesystem()->Read( pFileBuffer, size, hFile );

	vgui2::filesystem()->Close( hFile );

	auto pFileData = pFileBuffer;

	if( width )
		*width = 0;

	if( height )
		*height = 0;

	targa_header.id_length = static_cast<unsigned char>( *pFileBuffer );
	++pFileBuffer;
	targa_header.colormap_type = static_cast<unsigned char>( *pFileBuffer );
	++pFileBuffer;
	targa_header.image_type = static_cast<unsigned char>( *pFileBuffer );
	++pFileBuffer;

	targa_header.colormap_index = fgetLittleShort( &pFileBuffer );
	targa_header.colormap_length = fgetLittleShort( &pFileBuffer );
	targa_header.colormap_size = static_cast<unsigned char>( *pFileBuffer );
	++pFileBuffer;
	targa_header.x_origin = fgetLittleShort( &pFileBuffer );
	targa_header.y_origin = fgetLittleShort( &pFileBuffer );
	targa_header.width = fgetLittleShort( &pFileBuffer );
	targa_header.height = fgetLittleShort( &pFileBuffer );
	targa_header.pixel_size = static_cast<unsigned char>( *pFileBuffer );
	++pFileBuffer;
	targa_header.attributes = static_cast<unsigned char>( *pFileBuffer );
	++pFileBuffer;

	if( targa_header.image_type != 2
		&& targa_header.image_type != 10 )
	{
		return false;
	}

	if( targa_header.colormap_type != 0
		|| ( targa_header.pixel_size != 32 && targa_header.pixel_size != 24 ) )
	{
		return false;
	}

	const int columns = targa_header.width;
	const int rows = targa_header.height;
	const int numPixels = columns * rows;

	if( width )
		*width = targa_header.width;

	if( height )
		*height = targa_header.height;

	//TODO: Equation reverse engineered by IDA seems wrong, produces negative numbers for most images? - Solokiller
	//if( 4 - ( numPixels * ( targa_header.pixel_size == 24 ) != 0 ) > bufferSize )
	if( bufferSize < ( numPixels * ( 4 - ( targa_header.pixel_size == 24 ) ) ) )
	{
		return false;
	}

	if( targa_header.id_length != 0 )
		pFileBuffer += targa_header.id_length;  // skip TARGA image comment

	byte *pixbuf;

	if( targa_header.image_type == 2 ) {  // Uncompressed, RGB images
		for( int row = rows - 1; row >= 0; row-- ) {
			pixbuf = buffer + row*columns * 4;
			for( int column = 0; column<columns; column++ ) {
				unsigned char red, green, blue, alphabyte;
				switch( targa_header.pixel_size ) {
				case 24:

					blue = pFileBuffer[ 0 ];
					green = pFileBuffer[ 1 ];
					red = pFileBuffer[ 2 ];

					pFileBuffer += 3;

					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = 255;
					break;
				case 32:
					blue = pFileBuffer[ 0 ];
					green = pFileBuffer[ 1 ];
					red = pFileBuffer[ 2 ];
					alphabyte = pFileBuffer[ 3 ];

					pFileBuffer += 4;

					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = alphabyte;
					break;
				}
			}
		}
	}
	else if( targa_header.image_type == 10 ) {   // Runlength encoded RGB images
		unsigned char red, green, blue, alphabyte, packetHeader, packetSize, j;
		for( int row = rows - 1; row >= 0; row-- ) {
			pixbuf = buffer + row*columns * 4;
			for( int column = 0; column<columns; ) {
				packetHeader = *pFileBuffer;
				++pFileBuffer;
				packetSize = 1 + ( packetHeader & 0x7f );
				if( packetHeader & 0x80 ) {        // run-length packet
					switch( targa_header.pixel_size ) {
					case 24:
						blue = pFileBuffer[ 0 ];
						green = pFileBuffer[ 1 ];
						red = pFileBuffer[ 2 ];

						pFileBuffer += 3;

						alphabyte = 255;
						break;
					case 32:
						blue = pFileBuffer[ 0 ];
						green = pFileBuffer[ 1 ];
						red = pFileBuffer[ 2 ];
						alphabyte = pFileBuffer[ 3 ];

						pFileBuffer += 4;
						break;
					}

					for( j = 0; j<packetSize; j++ ) {
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = alphabyte;
						column++;
						if( column == columns ) { // run spans across rows
							column = 0;
							if( row>0 )
								row--;
							else
								goto breakOut;
							pixbuf = buffer + row*columns * 4;
						}
					}
				}
				else {                            // non run-length packet
					for( j = 0; j<packetSize; j++ ) {
						switch( targa_header.pixel_size ) {
						case 24:
							blue = pFileBuffer[ 0 ];
							green = pFileBuffer[ 1 ];
							red = pFileBuffer[ 2 ];

							pFileBuffer += 3;

							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = 255;
							break;
						case 32:
							blue = pFileBuffer[ 0 ];
							green = pFileBuffer[ 1 ];
							red = pFileBuffer[ 2 ];
							alphabyte = pFileBuffer[ 3 ];

							pFileBuffer += 4;

							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = alphabyte;
							break;
						}
						column++;
						if( column == columns ) { // pixel packet run spans across rows
							column = 0;
							if( row>0 )
								row--;
							else
								goto breakOut;
							pixbuf = buffer + row*columns * 4;
						}
					}
				}
			}
		breakOut:;
		}
	}

	stackfree( pFileBuffer );

	return true;
}

bool UpdateLogoWADFromTGA( const char* tga )
{
	char logoname[ 32 ];
	char *pszName;
	Q_strncpy( logoname, "LOGO", sizeof( logoname ) );
	pszName = &logoname[ 0 ];

	// Generate lump

	CUtlBuffer sourceBuffer( 0, 20480 );

	int width, height;

	if( !LoadTGA( tga, reinterpret_cast<byte*>( sourceBuffer.Base() ), sourceBuffer.Size(), &width, &height ) )
		return false;

	CUtlBuffer rBuffer( 0, 20480 );
	CUtlBuffer gBuffer( 0, 20480 );
	CUtlBuffer bBuffer( 0, 20480 );

	int i, j, k;

	for( i = GrabMipFromTGA(
			reinterpret_cast<byte*>( sourceBuffer.Base() ),
			reinterpret_cast<byte*>( rBuffer.Base() ),
			logoname,
			RGB( 255, 255, 255 ),
			width,
			height
		);
		 i & 3;
		 ++i )
	{
	}

	for( j = GrabMipFromTGA(
		reinterpret_cast<byte*>( sourceBuffer.Base() ),
		reinterpret_cast<byte*>( gBuffer.Base() ),
		logoname,
		RGB( 0, 255, 255 ),
		width,
		height
	);
		j & 3;
		++j )
	{
	}

	for( k = GrabMipFromTGA(
		reinterpret_cast<byte*>( sourceBuffer.Base() ),
		reinterpret_cast<byte*>( bBuffer.Base() ),
		logoname,
		RGB( 0, 0, 255 ),
		width,
		height
	);
		k & 3;
		++k )
	{
	}

	bool sizevalid = false;

	if( j && i && k && width == height )
	{
		if( width == 16 ||
			width == 32 ||
			width == 64 )
		{
			sizevalid = true;
		}
	}

	if( !sizevalid )
		return false;

	// Write Header
	wadinfo_t	header;
	header.identification[ 0 ] = 'W';
	header.identification[ 1 ] = 'A';
	header.identification[ 2 ] = 'D';
	header.identification[ 3 ] = '3';
	header.numlumps = 1;
	header.infotableofs = k + i + j + 12;

	CUtlBuffer buffer( 0, 20480 );

	buffer.Put( &header, sizeof( wadinfo_t ) );

	// Fill Ino info table
	lumpinfo_t	info;
	Q_memset( &info, 0, sizeof( info ) );
	Q_strncpy( info.name, pszName, sizeof( info.name ) );
	info.filepos = ( int )sizeof( wadinfo_t );
	info.size = info.disksize = i;
	info.type = TYP_LUMPY;
	info.compression = 0;

	// Write Lump
	buffer.Put( rBuffer.Base(), i );
	buffer.Put( gBuffer.Base(), j );
	buffer.Put( bBuffer.Base(), k );

	// Write info table
	buffer.Put( &info, sizeof( lumpinfo_t ) );

	// Output to file
	FileHandle_t file;
	file = filesystem()->Open( "tempdecal.wad", "wb", "GAMECONFIG" );
	if( file != FILESYSTEM_INVALID_HANDLE )
	{
		filesystem()->Write( buffer.Base(), buffer.TellPut(), file );
		filesystem()->Close( file );
	}

	return true;
}
