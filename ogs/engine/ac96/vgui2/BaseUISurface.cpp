#include "precompiled.hpp"
#include "quakedef.hpp"

#include <SDL2/SDL.h>

#include <html/ichromehtmlwrapper.h>

#include <vgui/Cursor.h>
#include <vgui/Dar.h>
#include <vgui/IInputInternal.h>
#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui2/src/vgui_key_translation.h>
#include <vgui2/vgui_surfacelib/FontManager.h>

#include <vgui_controls/Controls.h>

#include "BaseUI_Interface.hpp"
#include "BaseUISurface.hpp"
#include "client/client.hpp"
#include "EngineSurface.hpp"
#include "FontTextureCache.hpp"
#include "gl_vidnt.hpp"
#include "IGame.hpp"
#include "qgl.hpp"
#include "sys_getmodes.hpp"

using vgui2::ISurface;

bool s_bCursorVisible = true;

BaseUISurface g_BaseUISurface;

vgui2::IInputInternal* g_InputInternal = nullptr;

vgui2::IInputInternal* inputinternal()
{
	return g_InputInternal;
}

cvar_t sdl_double_click_size = { "sdl_double_click_size", "2" };
cvar_t sdl_double_click_time = { "sdl_double_click_time", "400" };

static int staticContextCount = 0;
static SDL_Cursor* staticDefaultCursor[ 20 ] = {};
static SDL_Cursor* staticCurrentCursor = nullptr;
static bool s_bUsedRawInputLast = false;

static vgui2::Dar<vgui2::VPANEL> staticPopupList;

static CFontTextureCache g_FontTextureCache;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( BaseUISurface, ISurface, VGUI_SURFACE_INTERFACE_VERSION_GS, g_BaseUISurface );

bool BaseUISurface::m_bTranslateExtendedKeys = true;

BaseUISurface::BaseUISurface()
{
	++staticContextCount;

	m_bTranslateExtendedKeys = true;

	if( !SDL_WasInit( SDL_INIT_VIDEO ) )
	{
		SDL_SetHint( "SDL_VIDEO_X11_XRANDR", "1" );
		SDL_SetHint( "SDL_VIDEO_X11_XVIDMODE", "1" );
		SDL_InitSubSystem( SDL_INIT_VIDEO );
	}

	staticDefaultCursor[ 1 ] = nullptr;
	staticDefaultCursor[ 2 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_ARROW );
	staticDefaultCursor[ 3 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_IBEAM );
	staticDefaultCursor[ 4 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_WAIT );
	staticDefaultCursor[ 6 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_CROSSHAIR );
	staticDefaultCursor[ 7 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENS );
	staticDefaultCursor[ 8 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENWSE );
	staticDefaultCursor[ 9 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENESW );
	staticDefaultCursor[ 10 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEWE );
	staticDefaultCursor[ 11 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENS );
	staticDefaultCursor[ 12 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEALL );
	staticDefaultCursor[ 13 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_NO );
	staticDefaultCursor[ 14 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_HAND );

	staticCurrentCursor = staticDefaultCursor[ 2 ];
}

BaseUISurface::~BaseUISurface()
{
	m_bAllowJavaScript = false;
}

void BaseUISurface::Init( vgui2::VPANEL embeddedPanel, IEngineSurface* engineSurface, IHTMLChromeController* pChromeController )
{
	_engineSurface = engineSurface;
	SetEmbeddedPanel( embeddedPanel );

	if( SteamApps() )
	{
		SetLanguage( SteamApps()->GetCurrentGameLanguage() );
	}

	m_pEmulatedMouseImage = vgui2::scheme()->GetImage( "gfx/vgui/mouse", false );

	AddCustomFontFile( "valve/resource/marlett.ttf" );

	Cvar_RegisterVariable( &sdl_double_click_size );
	Cvar_RegisterVariable( &sdl_double_click_time );

	m_pChromeController = pChromeController;

	if( m_pChromeController )
	{
		m_pChromeController->Init( "htmlcache", "htmlcookies" );

		m_pChromeController->SetCefThreadTargetFrameRate( 60 );
	}
}

void BaseUISurface::GetMouseDelta( int& x, int& y )
{
	x = m_deltax;
	y = m_deltay;

	m_deltax = m_deltay = 0;
}

void BaseUISurface::SetScreenBounds( int x, int y, int wide, int tall )
{
	m_iSurfaceBounds[ 0 ] = x;
	m_iSurfaceBounds[ 1 ] = y;
	m_iSurfaceBounds[ 2 ] = wide;
	m_iSurfaceBounds[ 3 ] = tall;
}

vgui2::MouseCode SDLMouseButtonToVGUI2MouseCode( Uint8 button )
{
	switch( button )
	{
	default:
	case SDL_BUTTON_LEFT:	return vgui2::MOUSE_LEFT;
	case SDL_BUTTON_MIDDLE:	return vgui2::MOUSE_MIDDLE;
	case SDL_BUTTON_RIGHT:	return vgui2::MOUSE_RIGHT;
	case SDL_BUTTON_X1:		return vgui2::MOUSE_4;
	case SDL_BUTTON_X2:		return vgui2::MOUSE_5;
	case SDL_BUTTON_X2 + 1:	return vgui2::MOUSE_4;
	case SDL_BUTTON_X2 + 2:	return vgui2::MOUSE_5;
	case SDL_BUTTON_X2 + 3:	return vgui2::MOUSE_4;
	case SDL_BUTTON_X2 + 4:	return vgui2::MOUSE_5;
	}
}

static void CheckModState()
{
	static uint32 s_lastModifierCode = 0;

	auto state = SDL_GetModState();

	auto changed = s_lastModifierCode ^ state;

	auto helper = [ = ]( auto mod, auto key )
	{
		if( changed & mod )
		{
			if( s_lastModifierCode & mod )
			{
				g_InputInternal->InternalKeyCodeReleased( key );
			}
			else
			{
				g_InputInternal->InternalKeyCodePressed( key );
				g_InputInternal->InternalKeyCodeTyped( key );
			}
		}
	};

	helper( KMOD_LSHIFT, vgui2::KEY_LSHIFT );
	helper( KMOD_LALT, vgui2::KEY_LALT );
	helper( KMOD_LCTRL, vgui2::KEY_LCONTROL );

	//TODO: shouldn't this be KEY_RSHIFT? - Solokiller
	helper( KMOD_RSHIFT, vgui2::KEY_LSHIFT );
	helper( KMOD_RALT, vgui2::KEY_RALT );
	helper( KMOD_RCTRL, vgui2::KEY_RCONTROL );

	helper( KMOD_CAPS, vgui2::KEY_CAPSLOCK );
	helper( KMOD_LGUI, vgui2::KEY_LWIN );
	helper( KMOD_RGUI, vgui2::KEY_RWIN );

	s_lastModifierCode = state;
}

void BaseUISurface::AppHandler( void* event, void* userData )
{
	SDL_Event ev = *reinterpret_cast<SDL_Event*>( event );

	switch( ev.type )
	{
	case SDL_MOUSEMOTION:
		{
			if( VideoMode_IsWindowed() )
			{
				g_InputInternal->InternalCursorMoved( ev.motion.x, ev.motion.y );
			}
			else
			{
				auto pWindow = reinterpret_cast<SDL_Window*>( game->GetMainWindow() );
			
				int wW, wT;
				SDL_GetWindowSize( pWindow, &wW, &wT );

				int vW, vT;
				VideoMode_GetCurrentVideoMode( &vW, &vT, nullptr );

				const int relX = static_cast<int>( floor( ev.motion.x * ( static_cast<double>( vW ) / wW ) ) );
				const int relY = static_cast<int>( floor( ev.motion.y * ( static_cast<double>( vT ) / wT ) ) );

				const double flX = static_cast<double>( relX - vW / 2 ) * ( GetXMouseAspectRatioAdjustment() - 1.0 ) + relX;
				const double flY = static_cast<double>( relY - vT / 2 ) * ( GetYMouseAspectRatioAdjustment() - 1.0 ) + relY;

				g_InputInternal->InternalCursorMoved( static_cast<int>( floor( flX ) ), static_cast<int>( floor( flY ) ) );
			}

			m_deltax += ev.motion.xrel;
			m_deltay += ev.motion.yrel;
			break;
		}

	case SDL_MOUSEWHEEL:
		{
			g_InputInternal->InternalMouseWheeled( ev.wheel.y );
			break;
		}

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		{
			const auto mouseCode = SDLMouseButtonToVGUI2MouseCode( ev.button.button );

			if( ev.type != SDL_MOUSEBUTTONDOWN )
			{
				g_InputInternal->InternalMouseReleased( mouseCode );
			}
			else
			{
				if( m_bGotMouseButtonDown )
				{
					if( sdl_double_click_time.value >= ( ev.button.timestamp - m_MouseButtonDownTimeStamp ) &&
						sdl_double_click_size.value >= static_cast<double>( ev.button.x - m_MouseButtonDownX ) &&
						sdl_double_click_size.value >= static_cast<double>( ev.button.y - m_MouseButtonDownY ) )
					{
						m_bGotMouseButtonDown = false;
						g_InputInternal->InternalMouseDoublePressed( mouseCode );
						break;
					}
				}

				m_MouseButtonDownTimeStamp = ev.button.timestamp;

				m_MouseButtonDownX = ev.button.x;
				m_MouseButtonDownY = ev.button.y;
				m_bGotMouseButtonDown = true;

				g_InputInternal->InternalMousePressed( mouseCode );
			}

			break;
		}

	case SDL_KEYUP:
		{
			g_InputInternal->InternalKeyCodeReleased( KeyCode_VirtualKeyToVGUI( ev.key.keysym.sym ) );
			CheckModState();
			break;
		}

	case SDL_KEYDOWN:
		{
			g_InputInternal->InternalKeyCodePressed( KeyCode_VirtualKeyToVGUI( ev.key.keysym.sym ) );
			g_InputInternal->InternalKeyCodeTyped( KeyCode_VirtualKeyToVGUI( ev.key.keysym.sym ) );
			CheckModState();
			break;
		}

	case SDL_TEXTINPUT:
		{
			uchar32 value;

			bool bError;

			for( size_t i = 0; i < ARRAYSIZE( ev.text.text ) && ev.text.text[ i ]; )
			{
				const auto bytes = Q_UTF8ToUChar32( &ev.text.text[ i ], value, bError );

				if( bError )
					break;

				g_InputInternal->InternalKeyTyped( value );

				i += bytes;
			}

			break;
		}
	}
}

void BaseUISurface::DrawSetTextureRGBAWithAlphaChannel( int id, const byte* rgba, int wide, int tall, int hardwareFilter )
{
	_engineSurface->drawSetTextureRGBA( id, rgba, wide, tall, hardwareFilter, true );
}

void BaseUISurface::DrawSetSubTextureRGBA( int textureID, int drawX, int drawY, const byte* rgba, int subTextureWide, int subTextureTall )
{
	_engineSurface->drawSetSubTextureRGBA( textureID, drawX, drawY, rgba, subTextureWide, subTextureTall );
}

void BaseUISurface::Shutdown()
{
	if( m_pChromeController )
		m_pChromeController->Shutdown();

	m_CustomFontFileNames.RemoveAll();

	for( int i = 0; i < ARRAYSIZE( staticDefaultCursor ); ++i )
	{
		if( staticDefaultCursor[ i ] )
			SDL_FreeCursor( staticDefaultCursor[ i ] );
	}
}

void BaseUISurface::RunFrame()
{
	if( m_pChromeController )
		m_pChromeController->RunFrame();
}

vgui2::VPANEL BaseUISurface::GetEmbeddedPanel()
{
	return _embeddedPanel;
}

void BaseUISurface::SetEmbeddedPanel( vgui2::VPANEL panel )
{
	_embeddedPanel = panel;
}

void BaseUISurface::PushMakeCurrent( vgui2::VPANEL panel, bool useInsets )
{
	int insets[ 4 ] = {};

	m_iCurrentTexture = 0;

	if( useInsets )
	{
		vgui2::ipanel()->GetInset( panel,
								   insets[ 0 ],
								   insets[ 1 ],
								   insets[ 2 ],
								   insets[ 3 ] );
	}

	int absExtents[ 4 ] = {};
	vgui2::ipanel()->GetAbsPos( panel,
								absExtents[ 0 ],
								absExtents[ 1 ] );

	int wide, tall;
	vgui2::ipanel()->GetSize( panel, wide, tall );

	absExtents[ 2 ] = wide + absExtents[ 0 ];
	absExtents[ 3 ] = tall + absExtents[ 1 ];

	int clipRect[ 4 ];
	vgui2::ipanel()->GetClipRect( panel,
								  clipRect[ 0 ],
								  clipRect[ 1 ],
								  clipRect[ 2 ],
								  clipRect[ 3 ] );

	_engineSurface->pushMakeCurrent( insets, absExtents, clipRect, false );
}

void BaseUISurface::PopMakeCurrent( vgui2::VPANEL panel )
{
	_engineSurface->popMakeCurrent();
}

void BaseUISurface::DrawSetColor( int r, int g, int b, int a )
{
	_engineSurface->drawSetColor( r, g, b, 255 - a );
}

void BaseUISurface::DrawSetColor( SDK_Color col )
{
	DrawSetColor( col.r(), col.g(), col.b(), col.a() );
}

void BaseUISurface::DrawFilledRect( int x0, int y0, int x1, int y1 )
{
	_engineSurface->drawFilledRect( x0, y0, x1, y1 );
}

void BaseUISurface::DrawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	_engineSurface->drawOutlinedRect( x0, y0, x1, y1 );
}

void BaseUISurface::DrawLine( int x0, int y0, int x1, int y1 )
{
	_engineSurface->drawLine( x0, y0, x1, y1 );
}

void BaseUISurface::DrawPolyLine( int* px, int* py, int numPoints )
{
	_engineSurface->drawPolyLine( px, py, numPoints );
}

void BaseUISurface::DrawSetTextFont( vgui2::HFont font )
{
	m_hCurrentFont = font;
}

void BaseUISurface::DrawSetTextColor( int r, int g, int b, int a )
{
	_engineSurface->drawSetTextColor( r, g, b, 255 - a );
}

void BaseUISurface::DrawSetTextColor( SDK_Color col )
{
	DrawSetTextColor( col.r(), col.g(), col.b(), col.a() );
}

void BaseUISurface::DrawSetTextPos( int x, int y )
{
	_engineSurface->drawSetTextPos( x, y );
}

void BaseUISurface::DrawGetTextPos( int& x, int& y )
{
	_engineSurface->drawGetTextPos( x, y );
}

void BaseUISurface::DrawPrintText( const wchar_t* text, int textLen )
{
	if( !text || textLen <= 0 )
		return;

	for( int i = 0; i < textLen; ++i )
	{
		DrawUnicodeChar( text[ i ] );
	}
}

void BaseUISurface::DrawUnicodeChar( wchar_t wch )
{
	if( !m_hCurrentFont )
		return;

	int x, y;
	DrawGetTextPos( x, y );

	auto tall = GetFontTall( m_hCurrentFont );

	int a, b, c;
	GetCharABCwide( m_hCurrentFont, wch, a, b, c );

	const bool bUnderlined = FontManager().GetFontUnderlined( m_hCurrentFont );

	int wide = b;

	x += a;

	if( bUnderlined )
	{
		wide = a + b + c;
	}

	int textureID;
	float texCoords[ 4 ];

	if( g_FontTextureCache.GetTextureForChar( m_hCurrentFont, wch, &textureID, texCoords ) )
	{
		if( textureID != -1 )
			DrawSetTexture( textureID );

		_engineSurface->drawPrintChar(
			x, y,
			wide,
			tall,
			texCoords[ 0 ],
			texCoords[ 1 ],
			texCoords[ 2 ],
			texCoords[ 3 ]
		);

		x += wide;

		if( !bUnderlined )
		{
			x += c;
		}

		DrawSetTextPos( x, y );
	}
}

void BaseUISurface::DrawUnicodeCharAdd( wchar_t wch )
{
	if( !m_hCurrentFont )
		return;

	int x, y;
	DrawGetTextPos( x, y );

	auto tall = GetFontTall( m_hCurrentFont );

	int a, b, c;
	GetCharABCwide( m_hCurrentFont, wch, a, b, c );

	//TODO: no underlined check? - Solokiller

	int wide = b;

	x += a;

	int textureID;
	float texCoords[ 4 ];

	if( g_FontTextureCache.GetTextureForChar( m_hCurrentFont, wch, &textureID, texCoords ) )
	{
		if( textureID != -1 )
			DrawSetTexture( textureID );

		_engineSurface->drawPrintCharAdd(
			x, y,
			wide,
			tall,
			texCoords[ 0 ],
			texCoords[ 1 ],
			texCoords[ 2 ],
			texCoords[ 3 ]
		);

		x += wide + c;

		DrawSetTextPos( x, y );
	}
}

void BaseUISurface::DrawFlushText()
{
	_engineSurface->drawFlushText();
}

vgui2::IHTML* BaseUISurface::CreateHTMLWindow( vgui2::IHTMLEvents* events, vgui2::VPANEL context )
{
	//Nothing
	return nullptr;
}

void BaseUISurface::PaintHTMLWindow( vgui2::IHTML* htmlwin )
{
	//Nothing
}

void BaseUISurface::DeleteHTMLWindow( vgui2::IHTML* htmlwin )
{
	//Nothing
}

void BaseUISurface::DrawSetTextureFile( int id, const char* filename, int hardwareFilter, bool forceReload )
{
	_engineSurface->drawSetTextureFile( id, filename, hardwareFilter, forceReload );
}

void BaseUISurface::DrawSetTextureRGBA( int id, const unsigned char* rgba, int wide, int tall, int hardwareFilter, bool forceReload )
{
	_engineSurface->drawSetTextureRGBA( id, rgba, wide, tall, hardwareFilter, false );
}

void BaseUISurface::DrawSetTexture( int id )
{
	if( m_iCurrentTexture == id )
		return;

	_engineSurface->drawSetTexture( id );

	m_iCurrentTexture = id;
}

void BaseUISurface::DrawGetTextureSize( int id, int& wide, int& tall )
{
	_engineSurface->drawGetTextureSize( id, wide, tall );
}

void BaseUISurface::DrawTexturedRect( int x0, int y0, int x1, int y1 )
{
	_engineSurface->drawTexturedRect( x0, y0, x1, y1 );
}

bool BaseUISurface::IsTextureIDValid( int id )
{
	return _engineSurface->isTextureIdValid( id ) != 0;
}

int BaseUISurface::CreateNewTextureID( bool procedural )
{
	return _engineSurface->createNewTextureID();
}

#ifdef _XBOX
void BaseUISurface::DestroyTextureID( int id )
{
	//TODO: implement
}

bool BaseUISurface::IsCachedForRendering( int id, bool bSyncWait )
{
	//TODO: implement
	return false;
}

void BaseUISurface::CopyFrontBufferToBackBuffer()
{
	//TODO: implement
}

void BaseUISurface::UncacheUnusedMaterials()
{
	//TODO: implement
}
#endif

void BaseUISurface::GetScreenSize( int& wide, int& tall )
{
	SCREENINFO screenInfo;
	screenInfo.iSize = sizeof( SCREENINFO );
	gEngfuncs.pfnGetScreenInfo( &screenInfo );
	wide = screenInfo.iWidth;
	tall = screenInfo.iHeight;
}

void BaseUISurface::SetAsTopMost( vgui2::VPANEL panel, bool state )
{
	//Nothing
}

void BaseUISurface::BringToFront( vgui2::VPANEL panel )
{
	vgui2::ipanel()->MoveToFront( panel );

	if( vgui2::ipanel()->IsPopup( panel ) )
	{
		//TODO: doesn't check if it's empty - Solokiller
		//If the panel isn't the last popup in the list, move it to the back.
		if( panel != staticPopupList[ staticPopupList.Count() - 1 ] )
		{
			for( int i = staticPopupList.Count() - 2; i >= 0; --i )
			{
				if( panel == staticPopupList[ i ] )
				{
					staticPopupList.Remove( i );
					staticPopupList.AddToTail( panel );
					return;
				}
			}
		}
	}
}

void BaseUISurface::SetForegroundWindow( vgui2::VPANEL panel )
{
	BringToFront( panel );
}

void BaseUISurface::SetPanelVisible( vgui2::VPANEL panel, bool state )
{
	//Nothing
}

void BaseUISurface::SetMinimized( vgui2::VPANEL panel, bool state )
{
	//Note: if SetPlat/Plat crash, it's probably due to invalid reverse engineered code. - Solokiller
	if( state )
	{
		vgui2::ipanel()->SetPlat( panel, reinterpret_cast<vgui2::SurfacePlat*>( true ) );
		vgui2::ipanel()->SetVisible( panel, false );
	}
	else
	{
		vgui2::ipanel()->SetPlat( panel, reinterpret_cast<vgui2::SurfacePlat*>( false ) );
	}
}

bool BaseUISurface::IsMinimized( vgui2::VPANEL panel )
{
	return reinterpret_cast<int>( vgui2::ipanel()->Plat( panel ) ) == 1;
}

void BaseUISurface::FlashWindow( vgui2::VPANEL panel, bool state )
{
	//Nothing
}

void BaseUISurface::SetTitle( vgui2::VPANEL panel, const wchar_t *title )
{
	//Nothing
}

void BaseUISurface::SetAsToolBar( vgui2::VPANEL panel, bool state )
{
	//Nothing
}

void BaseUISurface::CreatePopup( vgui2::VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput )
{
	if( !vgui2::ipanel()->GetParent( panel ) )
	{
		vgui2::ipanel()->SetParent( panel, GetEmbeddedPanel() );
	}

	vgui2::ipanel()->SetPopup( panel, true );

	int i;

	for( i = 0; i < staticPopupList.Count(); ++i )
	{
		if( panel == staticPopupList[ i ] )
			break;
	}

	if( i >= staticPopupList.Count() )
	{
		staticPopupList.AddToTail( panel );
	}

	vgui2::ipanel()->SetKeyBoardInputEnabled( panel, kbInput );
	vgui2::ipanel()->SetMouseInputEnabled( panel, mouseInput );
}

void BaseUISurface::SwapBuffers( vgui2::VPANEL panel )
{
	//Nothing
}

void BaseUISurface::Invalidate( vgui2::VPANEL panel )
{
	//Nothing
}

void BaseUISurface::SetCursor( vgui2::HCursor cursor )
{
	_currentCursor = cursor;

	if( _cursorLocked )
		return;

	const bool bWasVisible = s_bCursorVisible;
	s_bCursorVisible = true;

	bool bChange = true;

	switch( cursor )
		{
	case vgui2::dc_user:
				{
					s_bCursorVisible = false;
			staticCurrentCursor = nullptr;
			break;
				}

	case vgui2::dc_none:
		{
			bChange = false;
			s_bCursorVisible = false;
			}

	case vgui2::dc_arrow:
	case vgui2::dc_ibeam:
	case vgui2::dc_hourglass:
	case vgui2::dc_waitarrow:
	case vgui2::dc_crosshair:
	case vgui2::dc_up:
	case vgui2::dc_sizenwse:
	case vgui2::dc_sizenesw:
	case vgui2::dc_sizewe:
	case vgui2::dc_sizens:
	case vgui2::dc_sizeall:
	case vgui2::dc_no:
	case vgui2::dc_hand:
			{
				staticCurrentCursor = staticDefaultCursor[ cursor ];

			if( !bChange )
				break;
		}

	default:
		{
			SDL_SetCursor( staticCurrentCursor );
			break;
		}
	}

	const bool bUseRawInput = m_rawinput.value && BUsesSDLInput();

	if( s_bUsedRawInputLast != bUseRawInput )
	{
		if( bWasVisible )
		{
			SDL_SetRelativeMouseMode( SDL_FALSE );
		}
		else
		{
			SDL_ShowCursor( 1 );
		}

		s_bUsedRawInputLast = bUseRawInput;
	}

	//Cursor state change, update.
	if( s_bCursorVisible != bWasVisible )
	{
		if( bUseRawInput )
		{
			SDL_SetRelativeMouseMode( s_bCursorVisible ? SDL_FALSE : SDL_TRUE );
		}
		else
		{
			SDL_ShowCursor( s_bCursorVisible ? 1 : 0 );
		}

		if( cl_mousegrab.value )
		{
			SDL_SetWindowGrab(
				reinterpret_cast<SDL_Window*>( game->GetMainWindow() ),
				s_bCursorVisible ? SDL_FALSE : SDL_TRUE
			);
		}

		SDL_PumpEvents();

		int x, y;
		SDL_GetRelativeMouseState( &x, &y );

		game->SetCursorVisible( s_bCursorVisible );
	}
}

bool BaseUISurface::IsCursorVisible()
{
	return _currentCursor != vgui2::dc_none;
}

void BaseUISurface::ApplyChanges()
{
	//Nothing
}

bool BaseUISurface::IsWithin( int x, int y )
{
	//Nothing
	return true;
}

bool BaseUISurface::HasFocus()
{
	//Nothing
	return true;
}

bool BaseUISurface::SupportsFeature( SurfaceFeature_e feature )
{
	switch( feature )
	{
	case	ANTIALIASED_FONTS:
	case	DROPSHADOW_FONTS:	return true;
	default:					return false;
	}
}

void BaseUISurface::RestrictPaintToSinglePanel( vgui2::VPANEL panel )
{
	_restrictedPanel = panel;
	vgui2::input()->SetAppModalSurface( panel );
}

void BaseUISurface::SetModalPanel( vgui2::VPANEL )
{
	//Nothing
}

vgui2::VPANEL BaseUISurface::GetModalPanel()
{
	return _restrictedPanel;
}

void BaseUISurface::UnlockCursor()
{
	_cursorLocked = false;
}

void BaseUISurface::LockCursor()
{
	_cursorLocked = true;
}

void BaseUISurface::SetTranslateExtendedKeys( bool state )
{
	m_bTranslateExtendedKeys = state;
}

vgui2::VPANEL BaseUISurface::GetTopmostPopup()
{
	if( staticPopupList.Count() <= 0 )
		return NULL_HANDLE;

	return staticPopupList[ staticPopupList.Count() - 1 ];
}

void BaseUISurface::SetTopLevelFocus( vgui2::VPANEL subFocus )
{
	if( !subFocus )
		return;

	auto panel = subFocus;

	while( panel && !vgui2::ipanel()->IsPopup( panel ) )
	{
		panel = vgui2::ipanel()->GetParent( panel );
	}

	if( !panel )
		return;

	BringToFront( panel );
}

vgui2::HFont BaseUISurface::CreateFont()
{
	return FontManager().CreateFont();
}

bool BaseUISurface::AddGlyphSetToFont( vgui2::HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange )
{
	return FontManager().AddGlyphSetToFont( font, windowsFontName, tall, weight, blur, scanlines, flags );
}

bool BaseUISurface::AddCustomFontFile( const char* fontFileName )
{
	char fullPath[ 4096 ];

	vgui2::filesystem()->GetLocalPath( fontFileName, fullPath, sizeof( fullPath ) );

	CUtlSymbol symbol( fontFileName );

	m_CustomFontFileNames[ m_CustomFontFileNames.AddToTail() ] = symbol;

#ifdef WIN32
	return AddFontResourceExA( fullPath, FR_PRIVATE, nullptr ) > 0 ||
		AddFontResourceA( fullPath) > 0;
#else
#error "Implement me"
#endif
}

int BaseUISurface::GetFontTall( vgui2::HFont font )
{
	return FontManager().GetFontTall( font );
}

void BaseUISurface::GetCharABCwide( vgui2::HFont font, int ch, int &a, int &b, int &c )
{
	FontManager().GetCharABCwide( font, ch, a, b, c );
}

int BaseUISurface::GetCharacterWidth( vgui2::HFont font, int ch )
{
	return FontManager().GetCharacterWidth( font, ch );
}

void BaseUISurface::GetTextSize( vgui2::HFont font, const wchar_t *text, int &wide, int &tall )
{
	FontManager().GetTextSize( font, text, wide, tall );
}

vgui2::VPANEL BaseUISurface::GetNotifyPanel()
{
	//Nothing
	return NULL_HANDLE;
}

void BaseUISurface::SetNotifyIcon( vgui2::VPANEL context, vgui2::HTexture icon, vgui2::VPANEL panelToReceiveMessages, const char *text )
{
	//Nothing
}

void BaseUISurface::PlaySound( const char* fileName )
{
	gEngfuncs.pfnPlaySoundByName( const_cast<char*>( fileName ), 1.0 );
}

int BaseUISurface::GetPopupCount()
{
	return staticPopupList.Count();
}

vgui2::VPANEL BaseUISurface::GetPopup( int index )
{
	if( index < 0 || index >= staticPopupList.Count() )
		return NULL_HANDLE;

	return staticPopupList[ index ];
}

bool BaseUISurface::ShouldPaintChildPanel( vgui2::VPANEL childPanel )
{
	if( _restrictedPanel &&
		!vgui2::ipanel()->HasParent( childPanel, _restrictedPanel ) )
	{
	return false;
}

	const bool bIsPopup = vgui2::ipanel()->IsPopup( childPanel );

	if( bIsPopup && staticPopupList.Count() > 0 )
	{
		for( int i = 0; i < staticPopupList.Count(); ++i )
		{
			if( childPanel == staticPopupList[ i ] )
			{
				vgui2::ipanel()->Render_SetPopupVisible( childPanel, true );
				break;
			}
		}
	}

	return !bIsPopup;
}

bool BaseUISurface::RecreateContext( vgui2::VPANEL panel )
{
	//Nothing
	return true;
}

void BaseUISurface::AddPanel( vgui2::VPANEL panel )
{
	if( vgui2::ipanel()->IsPopup( panel ) )
		CreatePopup( panel, false, false, false, true, true );
}

void BaseUISurface::ReleasePanel( vgui2::VPANEL panel )
{
	for( int i = 0; i < staticPopupList.Count(); ++i )
	{
		if( panel == staticPopupList[ i ] )
		{
			staticPopupList.Remove( i );
			break;
		}
	}

	if( _restrictedPanel == panel )
		_restrictedPanel = NULL_HANDLE;
}

void BaseUISurface::MovePopupToFront( vgui2::VPANEL panel )
{
	//TODO: doesn't check if the list is empty - Solokiller
	if( panel != staticPopupList[ staticPopupList.Count() - 1 ] )
	{
		for( int i = staticPopupList.Count() - 2; i >= 0; --i )
		{
			if( panel == staticPopupList[ i ] )
			{
				staticPopupList.Remove( i );
				staticPopupList.AddToTail( panel );
				return;
			}
		}
	}
}

void BaseUISurface::MovePopupToBack( vgui2::VPANEL panel )
{
	if( staticPopupList.Count() <= 0 )
		return;

	//See if it's in the list, remove it if so.
	int popupIndex;

	for( popupIndex = 0; popupIndex < staticPopupList.Count(); ++popupIndex )
	{
		if( panel == staticPopupList[ popupIndex ] )
			break;
	}

	if( popupIndex < staticPopupList.Count() - 1 )
	{
		staticPopupList.Remove( popupIndex );
	}

	staticPopupList.AddToHead( panel );
}

void BaseUISurface::SolveTraverse( vgui2::VPANEL panel, bool forceApplySchemeSettings )
{
	InternalSchemeSettingsTraverse( panel, forceApplySchemeSettings );
	InternalThinkTraverse( panel );
	InternalSolveTraverse( panel );
}

void BaseUISurface::PaintTraverse( vgui2::VPANEL panel )
{
	if( !vgui2::ipanel()->IsVisible( panel ) )
		return;

	if( panel != GetEmbeddedPanel() )
	{
		vgui2::ipanel()->PaintTraverse( panel, true );
		return;
	}

	auto targetPanel = panel;

	if( _restrictedPanel )
		targetPanel = _restrictedPanel;

	for( int i = 0; i < staticPopupList.Count(); ++i )
	{
		vgui2::ipanel()->Render_SetPopupVisible( staticPopupList[ i ], false );
	}

	vgui2::ipanel()->PaintTraverse( targetPanel, true );

	for( int i = 0; i < staticPopupList.Count(); ++i )
	{
		if( vgui2::ipanel()->Render_GetPopupVisible( staticPopupList[ i ] ) )
		{
			vgui2::ipanel()->PaintTraverse( staticPopupList[ i ], true );
		}
	}
}

void BaseUISurface::EnableMouseCapture( vgui2::VPANEL panel, bool state )
{
	//Nothing
}

void BaseUISurface::GetWorkspaceBounds( int& x, int& y, int& wide, int& tall )
{
	x = 0;
	y = 0;
	vgui2::ipanel()->GetSize( _embeddedPanel, wide, tall );
}

void BaseUISurface::GetAbsoluteWindowBounds( int& x, int& y, int& wide, int& tall )
{
	x = m_iSurfaceBounds[ 0 ];
	y = m_iSurfaceBounds[ 1 ];
	wide = m_iSurfaceBounds[ 2 ];
	tall = m_iSurfaceBounds[ 3 ];
}

void BaseUISurface::GetProportionalBase( int& width, int& height )
{
	width = BASE_WIDTH;
	height = BASE_HEIGHT;
}

void BaseUISurface::CalculateMouseVisible()
{
	if( m_bIgnoreMouseVisCalc )
		return;

	_needMouse = false;
	_needKB = false;

	for( int i = 0; i < vgui2::surface()->GetPopupCount(); ++i )
	{
		auto popup = vgui2::surface()->GetPopup( i );

		const bool bIsVisible = vgui2::ipanel()->IsVisible( popup );

		//Determine if the popup is visible.
		if( !bIsVisible )
			continue;

		auto parent = vgui2::ipanel()->GetParent( popup );

		while( parent && vgui2::ipanel()->IsVisible( parent ) )
		{
			parent = vgui2::ipanel()->GetParent( parent );
		}

		//Parent was invisible, ignore.
		if( parent )
			continue;

		_needMouse = vgui2::ipanel()->IsMouseInputEnabled( popup ) || _needMouse;
		_needKB = vgui2::ipanel()->IsKeyBoardInputEnabled( popup ) || _needKB;
	}

	UnlockCursor();

	if( _needMouse )
	{
		SetCursor( vgui2::dc_arrow );
	}
	else
	{
		SetCursor( vgui2::dc_none );
		LockCursor();
	}
}

bool BaseUISurface::NeedKBInput()
{
	return _needKB;
}

bool BaseUISurface::HasCursorPosFunctions()
{
	return true;
}

void BaseUISurface::SurfaceGetCursorPos( int& x, int& y )
{
	SDL_GetMouseState( &x, &y );

	if( !VideoMode_IsWindowed() )
	{
		int wWidth, wHeight;
		SDL_GetWindowSize(
			reinterpret_cast<SDL_Window*>( game->GetMainWindow() ),
			&wWidth,
			&wHeight );
		
		int width, height;
		VideoMode_GetCurrentVideoMode( &width, &height, nullptr );

		const int iScaledX = static_cast<int>( floor( x * ( width / wWidth ) ) );
		const int iScaledY = static_cast<int>( floor( y * ( height / wHeight ) ) );

		x = static_cast<int>(
				floor( 
					static_cast<double>( iScaledX ) + 
					( iScaledX - width / 2 ) * 
					( GetXMouseAspectRatioAdjustment() - 1.0 )
				)
			);

		y = static_cast<int>(
			floor(
				static_cast<double>( iScaledY ) +
				( iScaledY - height / 2 ) *
				( GetYMouseAspectRatioAdjustment() - 1.0 )
			)
		);
	}
}

void BaseUISurface::SurfaceSetCursorPos( int x, int y )
{
	if( !VideoMode_IsWindowed() )
	{
		int wWidth, wHeight;
		SDL_GetWindowSize(
			reinterpret_cast<SDL_Window*>( game->GetMainWindow() ),
			&wWidth,
			&wHeight );

		int width, height;
		VideoMode_GetCurrentVideoMode( &width, &height, nullptr );

		const int iScaledX = static_cast<int>( floor( x * ( width / wWidth ) ) );
		const int iScaledY = static_cast<int>( floor( y * ( height / wHeight ) ) );

		x = static_cast<int>(
			floor(
				static_cast<double>( iScaledX ) +
				( iScaledX - width / 2 ) *
				( GetXMouseAspectRatioAdjustment() - 1.0 )
			)
		);

		y = static_cast<int>(
			floor(
				static_cast<double>( iScaledY ) +
				( iScaledY - height / 2 ) *
				( GetYMouseAspectRatioAdjustment() - 1.0 )
			)
		);
	}

	SDL_WarpMouseInWindow( 
		reinterpret_cast<SDL_Window*>( game->GetMainWindow() ),
		x,
		y
	);
}

void BaseUISurface::DrawTexturedPolygon( vgui2::VGuiVertex* pVertices, int n )
{
	_engineSurface->drawTexturedPolygon( pVertices, n );
}

int BaseUISurface::GetFontAscent( vgui2::HFont font, wchar_t wch )
{
	return FontManager().GetFontAscent( font, wch );
}

void BaseUISurface::SetAllowHTMLJavaScript( bool state )
{
	m_bAllowJavaScript = state;
}

void BaseUISurface::SetLanguage( const char* pchLang )
{
	if( pchLang )
	{
		strncpy( m_szLanguage, pchLang, ARRAYSIZE( m_szLanguage ) );
	}
	else
	{
		strncpy( m_szLanguage, "english", ARRAYSIZE( m_szLanguage ) );
	}
}

const char* BaseUISurface::GetLanguage()
{
	return m_szLanguage;
}

bool BaseUISurface::DeleteTextureByID( int id )
{
	qglDeleteTextures( 1, reinterpret_cast<GLuint*>( &id ) );
	return true;
}

void BaseUISurface::DrawUpdateRegionTextureBGRA( int nTextureID, int x, int y, const unsigned char* pchData, int wide, int tall )
{
	_engineSurface->drawUpdateRegionTextureBGRA( nTextureID, x, y, pchData, wide, tall );
}

void BaseUISurface::DrawSetTextureBGRA( int id, const unsigned char* rgba, int wide, int tall )
{
	_engineSurface->drawSetTextureBGRA( id, rgba, wide, tall, true, false );
}

void BaseUISurface::CreateBrowser( vgui2::VPANEL panel, IHTMLResponses* pBrowser, bool bPopupWindow, const char *pchUserAgentIdentifier )
{
	if( m_pChromeController )
		m_pChromeController->CreateBrowser( pBrowser, false, "Valve Half-Life" );
}

void BaseUISurface::RemoveBrowser( vgui2::VPANEL panel, IHTMLResponses* pBrowser )
{
	if( m_pChromeController )
		m_pChromeController->RemoveBrowser( pBrowser );
}

IHTMLChromeController* BaseUISurface::AccessChromeHTMLController()
{
	return m_pChromeController;
}

/*
bool BaseUISurface::setFullscreenMode( int wide, int tall, int bpp )
{
	m_bFullScreen = true;
	return false;
}

void BaseUISurface::setWindowedMode()
{
	m_bFullScreen = false;
}

void BaseUISurface::SetAsTopMost( bool state )
{
	//Nothing
}

void BaseUISurface::SetAsToolBar( bool state )
{
	//Nothing
}

void BaseUISurface::PanelRequestFocus( vgui2::VPANEL panel )
{
	//Nothing
}

void BaseUISurface::EnableMouseCapture( bool state )
{
	//Nothing
}

void BaseUISurface::DrawPrintChar( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 )
{
	_engineSurface->drawPrintChar( x, y, wide, tall, s0, t0, s1, t1 );
}

void BaseUISurface::SetNotifyIcon( vgui2::Image *, vgui2::VPANEL, char const* )
{
	//Nothing
}

bool BaseUISurface::SetWatchForComputerUse( bool state )
{
	//Nothing
	return false;
}

double BaseUISurface::GetTimeSinceLastUse()
{
	//Nothing
	return 0;
}
*/

bool BaseUISurface::VGUI2MouseControl()
{
	return m_bVGUI2MouseControl;
}

void BaseUISurface::SetVGUI2MouseControl( bool state )
{
	m_bVGUI2MouseControl = state;
}

void BaseUISurface::InternalSchemeSettingsTraverse( vgui2::VPANEL panel, bool forceApplySchemeSettings )
{
	for( int i = 0; i < vgui2::ipanel()->GetChildCount( panel ); ++i )
	{
		auto child = vgui2::ipanel()->GetChild( panel, i );

		if( forceApplySchemeSettings || vgui2::ipanel()->IsVisible( child ) )
		{
			InternalSchemeSettingsTraverse( child, forceApplySchemeSettings );
		}
	}

	vgui2::ipanel()->PerformApplySchemeSettings( panel );
}

void BaseUISurface::InternalThinkTraverse( vgui2::VPANEL panel )
{
	vgui2::ipanel()->Think( panel );

	for( int i = 0; i < vgui2::ipanel()->GetChildCount( panel ); ++i )
	{
		auto child = vgui2::ipanel()->GetChild( panel, i );

		if( vgui2::ipanel()->IsVisible( child ) )
		{
			InternalThinkTraverse( child );
		}
	}
}

void BaseUISurface::InternalSolveTraverse( vgui2::VPANEL panel )
{
	vgui2::ipanel()->Solve( panel );

	for( int i = 0; i < vgui2::ipanel()->GetChildCount( panel ); ++i )
	{
		auto child = vgui2::ipanel()->GetChild( panel, i );

		if( vgui2::ipanel()->IsVisible( child ) )
		{
			InternalSolveTraverse( child );
		}
	}
}