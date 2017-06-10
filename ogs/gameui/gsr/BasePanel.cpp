#include <tier0/platform.h>

#include <FileSystem.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>

#include "BasePanel.h"
#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "igameuifuncs.h"
#include "Taskbar.h"

CBasePanel::CBasePanel()
	: vgui2::Panel( nullptr, "BasePanel" )
{
}

vgui2::VPANEL CBasePanel::IsWithinTraverse( int x, int y, bool traversePopups )
{
	if( !IsVisible() )
		return NULL_HANDLE;

	return BaseClass::IsWithinTraverse( x, y, false );
}

void CBasePanel::OnChildAdded( vgui2::VPANEL child )
{
	if( g_pTaskbar )
		g_pTaskbar->AddTask( child );
}

void CBasePanel::PaintBackground()
{
	auto pszLevelName = engine->pfnGetLevelName();

	//Draw transparently if a level is loaded.
	//But not if we're in a career match.
	if( pszLevelName && *pszLevelName &&
		m_eBackgroundState != BACKGROUND_CAREERLOAD )
	{
		int swide, stall;
		vgui2::surface()->GetScreenSize( swide, stall );
		vgui2::surface()->DrawSetColor( 0, 0, 0, 128 );
		vgui2::surface()->DrawFilledRect( 0, 0, swide, stall );
	}
	else
	{
		switch( m_eBackgroundState )
		{
		case BACKGROUND_BLACK:
		case BACKGROUND_CAREERLOAD:
			{
				//Draw black screen if loading a career match.
				int swide, stall;
				vgui2::surface()->GetScreenSize( swide, stall );
				vgui2::surface()->DrawSetColor( 0, 0, 0, 255 );
				vgui2::surface()->DrawFilledRect( 0, 0, swide, stall );
				break;
			}

		case BACKGROUND_DESKTOPIMAGE:
		case BACKGROUND_LOADING:
			{
				DrawBackgroundImage();
				break;
			}

		default: break;
		}
	}
}

void CBasePanel::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	auto hFile = vgui2::filesystem()->Open( "resource/BackgroundLayout.txt", "rt" );

	if( FILESYSTEM_INVALID_HANDLE != hFile )
	{
		auto size = vgui2::filesystem()->Size( hFile );

		auto pszBuffer = reinterpret_cast<char*>( stackalloc( size + 1 ) );

		vgui2::filesystem()->Read( pszBuffer, size, hFile );
		pszBuffer[ size ] = '\0';

		vgui2::filesystem()->Close( hFile );

		int vid_level;
		gameuifuncs->GetCurrentRenderer( nullptr, 0, nullptr, nullptr, nullptr, &vid_level );

		auto pszData = pszBuffer;

		char token[ 512 ];

		while( pszData && *pszData )
		{
			pszData = vgui2::filesystem()->ParseFile( pszData, token, nullptr );

			if( !pszData || !token[ 0 ] )
				break;

			if( !stricmp( token, "resolution" ) )
			{
				pszData = vgui2::filesystem()->ParseFile( pszData, token, nullptr );
				m_iBaseResX = strtol( token, nullptr, 10 );

				pszData = vgui2::filesystem()->ParseFile( pszData, token, nullptr );
				m_iBaseResY = strtol( token, nullptr, 10 );
			}
			else
			{
				auto& image = m_ImageID[ m_ImageID.AddToTail() ];

				image.imageID = vgui2::surface()->CreateNewTextureID();

				auto pszExt = strstr( token, ".tga" );

				if( pszExt )
					*pszExt = '\0';

				vgui2::surface()->DrawSetTextureFile( image.imageID, token, vid_level == 0, false );

				vgui2::surface()->DrawGetTextureSize( image.imageID, image.width, image.height );

				pszData = vgui2::filesystem()->ParseFile( pszData, token, nullptr );
				image.scaled = !stricmp( token, "scaled" );

				pszData = vgui2::filesystem()->ParseFile( pszData, token, nullptr );
				image.x = strtol( token, nullptr, 10 );

				pszData = vgui2::filesystem()->ParseFile( pszData, token, nullptr );
				image.y = strtol( token, nullptr, 10 );
			}
		}

		stackfree( pBuffer );
	}
}

void CBasePanel::SetBackgroundRenderState( EBackgroundState state )
{
	m_eBackgroundState = state;
}

void CBasePanel::DrawBackgroundImage()
{
	int wide, tall;
	GetSize( wide, tall );

	int swide, stall;
	vgui2::surface()->GetScreenSize( swide, stall );

	const float xScale = static_cast<double>( swide ) / m_iBaseResX;
	const float yScale = static_cast<double>( stall ) / m_iBaseResY;

	for( int i = 0; i < m_ImageID.Count(); ++i )
	{
		const auto& image = m_ImageID[ i ];

		const auto xPos = static_cast<int>( round( image.x * xScale ) );
		const auto yPos = static_cast<int>( round( image.y * yScale ) );

		int dw, dt;

		if( image.scaled )
		{
			dw = static_cast<int>( round( ( image.x + image.width ) * xScale ) );
			dt = static_cast<int>( round( ( image.y + image.height ) * yScale ) );
		}
		else
		{
			dw = xPos + image.width;
			dt = yPos + image.height;
		}

		vgui2::surface()->DrawSetColor( 255, 255, 255, 255 );
		vgui2::surface()->DrawSetTexture( image.imageID );
		vgui2::surface()->DrawTexturedRect( xPos, yPos, dw, dt );
	}
}
