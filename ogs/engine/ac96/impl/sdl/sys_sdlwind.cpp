#include <cstdio>

#include "winheaders.h"

#include <SDL2/SDL.h>

#include "quakedef.h"
#include "client.h"

#include "vgui2/BaseUISurface.h"

#include "cdll_int.h"
#include "gl_vidnt.h"
#include "IGame.h"
#include "qgl.h"
#include "render.h"
#include "IEngine.h"
#include "sys_getmodes.h"
#include "vgui_int.h"

uint32 mouseCode = 0;

static bool bPausedByCareer = false;

bool BUsesSDLInput()
{
	static bool s_bUseRawInput;

	s_bUseRawInput = false;

	if( BIsValveGame() ||
		Q_strstr( g_szfullClientName, "valve/cl_dlls/client" ) ||
		Q_strstr( g_szfullClientName, "cstrike/cl_dlls/client" ) ||
		Q_strstr( g_szfullClientName, "tfc/cl_dlls/client" ) ||
		Q_strstr( g_szfullClientName, "dod/cl_dlls/client" ) ||
		Q_strstr( g_szfullClientName, "gearbox/cl_dlls/client" ) ||
		Q_strstr( g_szfullClientName, "bshift/cl_dlls/client" ) )
	{
		s_bUseRawInput = true;
	}

	return s_bUseRawInput;
}

void GetWindowNameFromGameDir( char* output, int outputBufferSize )
{
	FileHandle_t hLiblist = FS_Open( "liblist.gam", "rt" );

	if( hLiblist )
	{
		char line[ 512 ];

		while( !FS_EndOfFile( hLiblist ) )
		{
			*line = '\0';
			FS_ReadLine( line, sizeof( line ) - 1, hLiblist );

			if( !strnicmp( line, "game", 4 ) )
			{
				auto pszStart = strchr( line, '"' );

				if( pszStart )
				{
					auto pszEnd = strchr( pszStart + 1, '"' );

					if( pszEnd )
					{
						const int uiLength = pszEnd - pszStart;

						if( ( uiLength - 1 ) < outputBufferSize )
						{
							strncpy( output, pszStart + 1, uiLength - 1 );
							output[ uiLength - 1 ] = '\0';
						}
					}
				}
				break;
			}
		}

		FS_Close( hLiblist );
	}
}

class CGame final : public IGame
{
public:
	CGame() = default;
	~CGame() = default;

	bool Init( void *pvInstance ) override;
	bool Shutdown() override;

	bool CreateGameWindow() override;
	void SleepUntilInput( int time ) override;

	void* GetMainWindow() override;
	void** GetMainWindowAddress() override;

	void SetWindowXY( int x, int y ) override;
	void SetWindowSize( int w, int h ) override;
	void GetWindowRect( int* x, int* y, int* w, int* h ) override;

	bool IsActiveApp() override;
	bool IsMultiplayer() override;

	void PlayStartupVideos() override;
	void PlayAVIAndWait( const char *fileName ) override;

	void SetCursorVisible( bool bState ) override;

	void SetActiveApp( bool active );

	void AppActivate( bool fActive );

private:
	bool m_bActiveApp = true;

	SDL_Window* m_hSDLWindow = nullptr;
	SDL_GLContext m_hSDLGLContext = nullptr;

	bool m_bExpectSyntheticMouseMotion = false;
	int m_nMouseTargetX = 0;
	int m_nMouseTargetY = 0;

	int m_nWarpDelta = 0;

	bool m_bCursorVisible = true;

	int m_x = 0;
	int m_y = 0;

	int m_width = 0;
	int m_height = 0;

	bool m_bMultiplayer = false;
};

namespace
{
static CGame g_Game;
}

IGame* game = &g_Game;

bool CGame::Init( void *pvInstance )
{
	return true;
}

bool CGame::Shutdown()
{
	SDL_DestroyWindow( m_hSDLWindow );
	m_hSDLWindow = nullptr;
	SDL_Quit();

	return true;
}

bool CGame::CreateGameWindow()
{
	if( COM_CheckParm( "-noontop" ) )
		SDL_SetHint( "SDL_ALLOW_TOPMOST", "0" );

	SDL_SetHint( "SDL_VIDEO_X11_XRANDR", "1" );
	SDL_SetHint( "SDL_VIDEO_X11_XVIDMODE", "1" );

	SDL_InitSubSystem( SDL_INIT_EVERYTHING );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 4 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );

	char gameWindowName[ 512 ];
	strcpy( gameWindowName, "Half-Life" );

	//Check if liblist overrides the window title.
	GetWindowNameFromGameDir( gameWindowName, sizeof( gameWindowName ) );
		
	//Added SDL_WINDOW_OPENGL. - Solokiller
	Uint32 uiFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL;

	if( COM_CheckParm( "-noborder" ) )
		uiFlags |= SDL_WINDOW_BORDERLESS;

	m_hSDLWindow = SDL_CreateWindow( gameWindowName, 0, 0, 640, 480, uiFlags );

	if( !m_hSDLWindow )
	{
		//Try 16 bit color depth.
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 3 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 3 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 3 );
		m_hSDLWindow = SDL_CreateWindow( gameWindowName, 0, 0, 640, 480, uiFlags );

		if( !m_hSDLWindow )
			Sys_Error( "Failed to create SDL Window" );
	}

	byte* buffer = ( byte* ) malloc( 512 * 512 );
	int width, height;
	if( LoadTGA( "game.tga", buffer, 512 * 512, &width, &height ) )
	{
		//Flip the image vertically.
		byte* bufferline = ( byte* ) malloc( 4 * width );

		for( int i = 0; i < height / 2; ++i )
		{
			memcpy( bufferline, &buffer[ width * i * 4 ], width * 4 );
			memcpy( &buffer[ width * i * 4 ], &buffer[ width * ( height - i - 1 ) * 4 ], width * 4 );
			memcpy( &buffer[ width * ( height - i - 1 ) * 4 ], bufferline, width * 4 );
		}

		free( bufferline );

		SDL_Surface* pSurface = SDL_CreateRGBSurfaceFrom( buffer, width, height, 32, 4 * width, 0xFF, 0xFF << 8, 0xFF << 16, 0xFF << 24 );
	
		if( pSurface )
		{
			SDL_SetWindowIcon( m_hSDLWindow, pSurface );
			SDL_FreeSurface( pSurface );
		}
	}

	free( buffer );

	SDL_ShowWindow( m_hSDLWindow );

	m_hSDLGLContext = SDL_GL_CreateContext( m_hSDLWindow );

	if( !m_hSDLWindow || !m_hSDLGLContext )
		Sys_Error( "Failed to create SDL Window" );

	int r, g, b, a, depth;

	if( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &r ) )
	{
		r = 0;
		Con_Printf( "Failed to get GL RED size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &g ) )
	{
		g = 0;
		Con_Printf( "Failed to get GL GREEN size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &b ) )
	{
		b = 0;
		Con_Printf( "Failed to get GL BLUE size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &a ) )
	{
		a = 0;
		Con_Printf( "Failed to get GL ALPHA size (%s)\n", SDL_GetError() );
	}
	if( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &depth ) )
	{
		depth = 0;
		Con_Printf( "Failed to get GL DEPTH size (%s)\n", SDL_GetError() );
	}
	Con_Printf( "GL_SIZES:  r:%d g:%d b:%d a:%d depth:%d\n", r, g, b, a, depth );
	if( r <= 4 || g <= 4 || b <= 4 || depth <= 15 || gl_renderer && Q_strstr( gl_renderer, "GDI Generic" ) )
		Sys_Error( "Failed to create SDL Window, unsupported video mode. A 16-bit color depth desktop is required and a supported GL driver.\n" );

	m_nMouseTargetX = 320;
	m_nMouseTargetY = 240;
	m_nWarpDelta = 213;

	return true;
}

void CGame::SleepUntilInput( int time )
{
	SDL_PumpEvents();

	SDL_Event ev;

	if( SDL_WaitEventTimeout( &ev, time ) > 0 )
	{
		do
		{
			switch( ev.type )
			{
			case SDL_TEXTINPUT:
				{
					if( key_dest == key_message )
					{
						size_t uiSpaceLeft = ( MAX_CHAT_BUFFER - 1 ) - chat_bufferlen;

						const int iLength = Q_strlen( ev.text.text );

						if( static_cast<size_t>( iLength ) <= uiSpaceLeft )
							uiSpaceLeft = iLength;

						Q_strncpy( chat_buffer, ev.edit.text, ARRAYSIZE( chat_buffer ) );
						chat_bufferlen += uiSpaceLeft;
					}

					break;
				}

			case SDL_WINDOWEVENT:
				{
					switch( ev.window.event )
					{
					case SDL_WINDOWEVENT_CLOSE:
						if( eng->GetState() == 1 )
							eng->SetQuitting( IEngine::QUIT_TODESKTOP );
						SDL_DestroyWindow( m_hSDLWindow );
						m_hSDLWindow = nullptr;
						return;

					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_FOCUS_LOST:
						AppActivate( false );
						break;

					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						AppActivate( true );
						break;

					case SDL_WINDOWEVENT_RESIZED:
						game->SetWindowSize( ev.window.data1, ev.window.data2 );

						videomode->UpdateWindowPosition();
						break;

					case SDL_WINDOWEVENT_MOVED:
						if( videomode->IsWindowedMode() )
						{
							game->SetWindowXY( ev.window.data1, ev.window.data2 );
							videomode->UpdateWindowPosition();
						}
						break;

					default: break;
					}
					break;
				}

			case SDL_KEYDOWN:
				{
					eng->TrapKey_Event( GetEngineKeyFromSDLScancode( ev.key.keysym.scancode ), true );
					break;
				}
			case SDL_KEYUP:
				{
					eng->TrapKey_Event( GetEngineKeyFromSDLScancode( ev.key.keysym.scancode ), false );
					break;
				}

			case SDL_QUIT:
				{
					if( eng->GetState() == 1 )
					{
						eng->SetQuitting( IEngine::QUIT_TODESKTOP );
					}
					break;
				}

			case SDL_MOUSEWHEEL:
				{
					eng->TrapKey_Event(
						( ev.wheel.y > 0 ) ? K_MWHEELUP : K_MWHEELDOWN, true );

					eng->TrapKey_Event(
						( ev.wheel.y > 0 ) ? K_MWHEELUP : K_MWHEELDOWN, false );
					break;
				}

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				{
					//TODO: define constants for mouseCode - Solokiller
					switch( ev.button.button )
					{
					case SDL_BUTTON_LEFT:
						if( ev.type == SDL_MOUSEBUTTONDOWN )
						{
							mouseCode |= 1u;
						}
						else
						{
							mouseCode &= 0xFFFFFFFE;
						}
						break;
					case SDL_BUTTON_X2:
					case 7:
					case 9:
						if( ev.type == SDL_MOUSEBUTTONDOWN )
						{
							mouseCode |= 0x10u;
						}
						else
						{
							mouseCode &= 0xFFFFFFEF;
						}
						break;
					case SDL_BUTTON_X1:
					case 6:
					case 8:
						if( ev.type == SDL_MOUSEBUTTONDOWN )
						{
							mouseCode |= 8u;
						}
						else
						{
							mouseCode &= 0xFFFFFFF7;
						}
						break;
					case SDL_BUTTON_RIGHT:
						if( ev.type == SDL_MOUSEBUTTONDOWN )
						{
							mouseCode |= 2u;
						}
						else
						{
							mouseCode &= 0xFFFFFFFD;
						}
						break;
					case SDL_BUTTON_MIDDLE:
						if( ev.type == SDL_MOUSEBUTTONDOWN )
						{
							mouseCode |= 4u;
						}
						else
						{
							mouseCode &= 0xFFFFFFFB;
						}
						break;

					default: break;
					}

					eng->TrapMouse_Event( mouseCode, ev.type == SDL_MOUSEBUTTONDOWN );
					break;
				}

			case SDL_MOUSEMOTION:
				{
					if( !cl_mousegrab.value && !m_bActiveApp )
						break;

					if( m_rawinput.value || !m_bActiveApp )
						break;

					if( g_BaseUISurface.IsCursorVisible() && !s_bCursorVisible && BUsesSDLInput() )
					{
						if( m_bExpectSyntheticMouseMotion &&
							ev.motion.x == m_nMouseTargetX &&
							ev.motion.y == m_nMouseTargetY )
						{
							m_bExpectSyntheticMouseMotion = false;

							if( !m_rawinput.value )
							{
								int deltaX, deltaY;
								SDL_GetRelativeMouseState( &deltaX, &deltaY );
							}
							continue;
						}

						if( !m_bCursorVisible )
						{
							if( ev.window.data2 >= m_nMouseTargetX - m_nWarpDelta &&
								ev.window.data2 >= m_nMouseTargetX + m_nWarpDelta &&
								ev.motion.y >= m_nMouseTargetY - m_nWarpDelta &&
								ev.motion.y <= m_nMouseTargetY + m_nWarpDelta )
							{
									break;
							}

							SDL_WarpMouseInWindow( m_hSDLWindow, m_nMouseTargetX, m_nMouseTargetY );
							m_bExpectSyntheticMouseMotion = true;
						}
					}

					break;
				}
			}

			VGui_CallEngineSurfaceAppHandler( &ev, 0 );
		}
		while( SDL_PollEvent( &ev ) > 0 );
	}
}

void* CGame::GetMainWindow()
{
	return m_hSDLWindow;
}

void** CGame::GetMainWindowAddress()
{
	return reinterpret_cast<void**>( &m_hSDLWindow );
}

void CGame::SetWindowXY( int x, int y )
{
	m_x = x;
	m_y = y;
}

void CGame::SetWindowSize( int w, int h )
{
	m_width = w;
	m_height = h;

	SDL_SetWindowSize( m_hSDLWindow, w, h );
	m_nMouseTargetX = m_width / 2;
	m_nMouseTargetY = m_height / 2;

	int iMinWarp = 200;

	if( ( m_height / 3 ) >= iMinWarp )
		iMinWarp = m_height / 3;

	m_nWarpDelta = iMinWarp;
}

void CGame::GetWindowRect( int* x, int* y, int* w, int* h )
{
	if( x )
		*x = m_x;

	if( y )
		*y = m_y;

	if( w )
		*w = m_width;

	if( h )
		*h = m_height;
}

bool CGame::IsActiveApp()
{
	return m_bActiveApp;
}

bool CGame::IsMultiplayer()
{
	return m_bMultiplayer;
}

void CGame::PlayStartupVideos()
{
	//Nothing
}

void CGame::PlayAVIAndWait( const char *fileName )
{
	//Nothing
}

void CGame::SetCursorVisible( bool bState )
{
	m_bCursorVisible = bState;

	if( !bState && !cl_mousegrab.value )
		SDL_SetWindowGrab( m_hSDLWindow, SDL_FALSE );
}

void CGame::SetActiveApp( bool active )
{
	m_bActiveApp = active;
}

void CGame::AppActivate( bool fActive )
{
	if( fActive )
	{
		fputs( "AppActive: active\n", stderr );
		
		if( host_initialized )
		{
			ClearIOStates();

			//TODO: implement - Solokiller
			//( *( ( void( __cdecl ** )( _DWORD ) )cdaudio->_vptr.ICDAudio + 5 ) )( cdaudio );

			if( videomode->GetInitialized() )
			{
				if( !VGuiWrap2_IsGameUIVisible() )
					ClientDLL_ActivateMouse();

				g_BaseUISurface.CalculateMouseVisible();

				int dx, dy;
				g_BaseUISurface.GetMouseDelta( dx, dy );
			}

			if( VGuiWrap2_IsInCareerMatch() && bPausedByCareer )
			{
				Cmd_ExecuteString( "unpause", src_command );
				bPausedByCareer = false;
			}
		}
	}
	else
	{
		fputs( "AppActive: not active\n", stderr );

		if( host_initialized )
		{
			if( videomode->GetInitialized() &&
				!VGuiWrap2_IsGameUIVisible() )
				ClientDLL_DeactivateMouse();

			//TODO: implement - Solokiller
			//( *( ( void( __cdecl ** )( _DWORD ) )cdaudio->_vptr.ICDAudio + 4 ) )( cdaudio );

			//TODO: implement - Solokiller
			if( VGuiWrap2_IsInCareerMatch() /*&& ei.paused == false*/ )
			{
				Cmd_ExecuteString( "setpause", src_command );
				bPausedByCareer = true;
			}
		}
	}
	m_bActiveApp = fActive;
}