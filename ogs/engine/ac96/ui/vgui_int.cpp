/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include "precompiled.hpp"
#include "quakedef.h"
#include "vgui_EngineSurface.h"
#include "ui/vgui_int.hpp"

void VGui_Startup()
{
	VGuiWrap_Startup();
	VGuiWrap2_Startup();
}

void VGui_Shutdown()
{
	VGuiWrap_Shutdown();
	VGuiWrap2_Shutdown();
	EngineSurface::freeEngineSurface();
}

void VGui_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	if( !VGuiWrap2_CallEngineSurfaceAppHandler( event, userData ) )
		VGuiWrap_CallEngineSurfaceAppHandler( event, userData );
}

vgui::Panel* VGui_GetPanel()
{
	return VGuiWrap_GetPanel();
}

void VGui_ReleaseMouse()
{
	VGuiWrap_ReleaseMouse();
	VGuiWrap2_ReleaseMouse();
}

void VGui_GetMouse()
{
	VGuiWrap_GetMouse();
	VGuiWrap2_GetMouse();
}

void VGui_SetVisible( bool state )
{
	VGuiWrap_SetVisible( state );
	VGuiWrap2_SetVisible( state );
}

void VGui_Paint()
{
	if( VGuiWrap2_UseVGUI1() )
	{
		VGuiWrap_Paint( !VGuiWrap2_IsGameUIVisible() );
		VGuiWrap2_Paint();
	}
	else
	{
		VGuiWrap_Paint( false );
		VGuiWrap2_Paint();
	}
}

bool VGui_GameUIKeyPressed()
{
	return VGuiWrap2_GameUIKeyPressed();
}

bool VGui_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	return VGuiWrap2_Key_Event( down, keynum, pszCurrentBinding ) != 0;
}

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#endif

struct
#ifndef WIN32
	//TODO: verify that this works. - Solokiller
	__attribute__( ( packed ) ) __attribute__( ( aligned( 2 ) ) )
#endif
	BITMAPFILEHEADER
{
	uint16 bfType;
	uint32 bfSize;
	uint16 bfReserved1;
	uint16 bfReserved2;
	uint32 bfOffBits;
};

#ifdef WIN32
#pragma pack( pop )
#endif

struct BITMAPINFOHEADER
{
	uint32 biSize;
	int32 biWidth;
	int32 biHeight;
	uint16 biPlanes;
	uint16 biBitCount;
	uint32 biCompression;
	uint32 biSizeImage;
	int32 biXPelsPerMeter;
	int32 biYPelsPerMeter;
	uint32 biClrUsed;
	uint32 biClrImportant;
};

struct BMPQuad
{
	byte b, g, r, reserved;
};

struct BITMAPINFO
{
	BITMAPINFOHEADER    bmiHeader;
	BMPQuad             bmiColors[ 1 ];
};

#define BMP_TYPE 0x4D42

bool VGui_LoadBMP( FileHandle_t file, byte* buffer, int bufsize, int* width, int* height )
{
	const auto size = FS_Size( file );

	BITMAPFILEHEADER bmfHeader;

	FS_Read( &bmfHeader, sizeof( BITMAPFILEHEADER ), file );

	bool bSuccess = false;

	if( bmfHeader.bfType == BMP_TYPE )
	{
		const auto dataSize = size - sizeof( BITMAPFILEHEADER );

		auto pBuffer = reinterpret_cast<byte*>( malloc( dataSize ) );

		FS_Read( pBuffer, dataSize, file );

		auto pInfo = reinterpret_cast<BITMAPINFO*>( pBuffer );

		*width = pInfo->bmiHeader.biWidth;
		*height = pInfo->bmiHeader.biHeight;

		int iWidth = *width;

		if( *width & 3 )
			iWidth = AlignValue( *width, 16 );

		auto pPalette = pInfo->bmiColors;

		auto pSource = reinterpret_cast<byte*>( pInfo ) + bmfHeader.bfOffBits - sizeof( bmfHeader );

		auto pDest = buffer;

		//Convert into an RGBA format.
		for( int y = 0; y < *height; ++y )
		{
			for( int x = 0; x < *width; ++x, pDest += 4 )
			{
				auto pPixels = &pSource[ x + iWidth * ( *height - y - 1 ) ];

				pDest[ 0 ] = pPalette[ *pPixels ].r;
				pDest[ 1 ] = pPalette[ *pPixels ].g;
				pDest[ 2 ] = pPalette[ *pPixels ].b;
				pDest[ 3 ] = 0xFF;
			}
		}

		free( pBuffer );

		bSuccess = true;
	}

	FS_Close( file );

	return bSuccess;
}