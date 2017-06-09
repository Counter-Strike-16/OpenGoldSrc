#include <cstdio>

#include "quakedef.h"

#include "render.h"

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

bool LoadTGA2( const char *szFilename, byte *buffer, int bufferSize, int *width, int *height, bool errFail )
{
	int iLength;

	auto pFileBuffer = COM_LoadFile( szFilename, 5, &iLength );

	if( !pFileBuffer )
		return false;

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
		if( errFail )
			Sys_Error( "LoadTGA: Only type 2 and 10 targa RGB images supported\n" );
	
		COM_FreeFile( pFileData );
		Con_Printf( "LoadTGA: Only type 2 and 10 targa RGB images supported\n" );
		return false;
	}

	if( targa_header.colormap_type != 0
		|| ( targa_header.pixel_size != 32 && targa_header.pixel_size != 24 ) )
	{
		if( errFail )
			Sys_Error( "Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n" );

		COM_FreeFile( pFileData );
		Con_Printf( "Texture_LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n" );
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
		if( errFail )
			Sys_Error( "TGA texture has the wrong size!\n" );

		COM_FreeFile( pFileData );
		Con_Printf( "TGA texture has the wrong size!\n" );
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

	COM_FreeFile( pFileData );

	return true;
}

bool LoadTGA( const char *szFilename, byte *buffer, int bufferSize, int *width, int *height )
{
	return LoadTGA2( szFilename, buffer, bufferSize, width, height, false );
}
