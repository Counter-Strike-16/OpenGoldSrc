#include <cctype>

#include <SDL2/SDL.h>

#include <VGUI_App.h>
#include <VGUI_Cursor.h>
#include <VGUI_Dar.h>
#include <VGUI_FileInputStream.h>
#include <VGUI_Font.h>
#include <VGUI_ImagePanel.h>
#include <VGUI_Panel.h>

#include "quakedef.h"
#include "client.h"
#include "EngineSurface.h"
#include "gl_vidnt.h"
#include "IGame.h"
#include "vgui2/IMouseControl.h"
#include "sys_getmodes.h"
#include "VGUI_EngineSurfaceWrap.h"
#include "vgui_int.h"
#include "vgui2/BaseUISurface.h"
#include "vgui2/BaseUI_Interface.h"
#include "vgui2/src/vgui_key_translation.h"

#define FONT_MAX_PAGES 8
#define FONT_PAGE_SIZE 512

struct FontInfoVGUI
{
	int id;
	int pageCount;
	int pageForChar[ 256 ];
	int bindIndex[ FONT_MAX_PAGES ];
	float texCoord[ 256 ][ FONT_MAX_PAGES ];
	int contextCount;
};

static vgui::Font* staticFont = nullptr;
static FontInfoVGUI* staticFontInfoVGUI = nullptr;

static vgui::Dar<FontInfoVGUI*> staticFontInfoVGUIDar;

static int staticContextCount = 0;

static byte staticRGBA[ ( FONT_PAGE_SIZE * 2 ) * ( FONT_PAGE_SIZE * 2 ) ];

//vgui::Cursor::DefaultCursor->SDL_SystemCursor mapping
//TODO: why does this have a size of 20 when only 14 are needed? - Solokiller
static SDL_Cursor* staticDefaultCursor[ 20 ] = {};

//The currently active cursor
static SDL_Cursor* staticCurrentCursor = nullptr;

EngineSurfaceWrap::EngineSurfaceWrap( vgui::Panel* embeddedPanel, IEngineSurface* engineSurface )
	:vgui::SurfaceBase( embeddedPanel )
	, _engineSurface( engineSurface )
{
	staticFont = nullptr;
	staticFontInfoVGUI = nullptr;

	staticFontInfoVGUIDar.removeAll();

	++staticContextCount;

	{
		auto pStream = new vgui::FileInputStream( "valve/gfx/vgui/mouse.tga", false );

		_emulatedMouseImage = new vgui::BitmapTGA( pStream, true );

		pStream->close();

		delete pStream;
	}

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
	staticDefaultCursor[ 5 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_CROSSHAIR );
	staticDefaultCursor[ 6 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENS );
	staticDefaultCursor[ 7 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENWSE );
	staticDefaultCursor[ 8 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENESW );
	staticDefaultCursor[ 9 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEWE );
	staticDefaultCursor[ 10 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENS );
	staticDefaultCursor[ 11 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEALL );
	staticDefaultCursor[ 12 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_NO );
	staticDefaultCursor[ 13 ] = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_HAND );

	staticCurrentCursor = staticDefaultCursor[ 2 ];
}

EngineSurfaceWrap::~EngineSurfaceWrap()
{
	delete _emulatedMouseImage;

	for( int i = 0; i < ARRAYSIZE( staticDefaultCursor ); ++i )
	{
		if( staticDefaultCursor[ i ] )
			SDL_FreeCursor( staticDefaultCursor[ i ] );
	}
}

void EngineSurfaceWrap::setTitle( const char* title )
{
	// Nothing
}

bool EngineSurfaceWrap::setFullscreenMode( int wide, int tall, int bpp )
{
	// Nothing
	return false;
}

void EngineSurfaceWrap::setWindowedMode()
{
	// Nothing
}

void EngineSurfaceWrap::setAsTopMost( bool state )
{
	// Nothing
}

void EngineSurfaceWrap::createPopup( vgui::Panel* embeddedPanel )
{
	// Nothing
}

bool EngineSurfaceWrap::hasFocus()
{
	return true;
}

bool EngineSurfaceWrap::isWithin( int x, int y )
{
	// Nothing
	return true;
}

int EngineSurfaceWrap::createNewTextureID()
{
	return _engineSurface->createNewTextureID();
}

void EngineSurfaceWrap::GetMousePos( int &x, int &y )
{
	SDL_GetMouseState( &x, &y );

	if( !VideoMode_IsWindowed() )
	{
		int wwidth, wheight;
		SDL_GetWindowSize( reinterpret_cast<SDL_Window*>( game->GetMainWindow() ), &wwidth, &wheight );

		int vwidth, vheight;
		VideoMode_GetCurrentVideoMode( &vwidth, &vheight, nullptr );

		x = static_cast<int>( x * ( static_cast<double>( vwidth ) / wwidth ) );
		y = static_cast<int>( y * ( static_cast<double>( vheight ) / wheight ) );

		x = static_cast<int>( x + static_cast<double>( x - vwidth / 2 ) * ( GetXMouseAspectRatioAdjustment() - 1.0 ) );
		y = static_cast<int>( y + static_cast<double>( y - vheight / 2 ) * ( GetYMouseAspectRatioAdjustment() - 1.0 ) );
	}
}

void EngineSurfaceWrap::drawSetColor( int r, int g, int b, int a )
{
	_engineSurface->drawSetColor( r, g, b, a );
}

void EngineSurfaceWrap::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	_engineSurface->drawFilledRect( x0, y0, x1, y1 );
}

void EngineSurfaceWrap::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	_engineSurface->drawOutlinedRect( x0, y0, x1, y1 );
}

void EngineSurfaceWrap::drawSetTextFont( vgui::Font* font )
{
	staticFont = font;

	if( !font )
		return;

	staticFontInfoVGUI = nullptr;

	bool bMatch = false;

	int i;

	for( i = 0; i < staticFontInfoVGUIDar.getCount(); ++i )
	{
		if( font->getId() == staticFontInfoVGUIDar[ i ]->id )
		{
			staticFontInfoVGUI = staticFontInfoVGUIDar[ i ];

			if( staticFontInfoVGUI->contextCount == staticContextCount )
			{
				bMatch = true;
				break;
			}
		}
}

	if( staticFontInfoVGUI )
{
		//if we don't match, the font needs updating
		//TODO: this leaks GPU texture memory because old ones aren't freed - Solokiller
		if( bMatch )
			return;
	}
	else
	{
		staticFontInfoVGUI = new FontInfoVGUI;

		memset( staticFontInfoVGUI, 0, sizeof( *staticFontInfoVGUI ) );

		staticFontInfoVGUI->contextCount = -1;

		staticFontInfoVGUI->id = font->getId();

		staticFontInfoVGUIDar.addElement( staticFontInfoVGUI );
	}

	staticFontInfoVGUI->contextCount = staticContextCount;

	memset( staticRGBA, 0, sizeof( staticRGBA ) );

	int abcA, abcB, abcC;

	int iX = 0;
	int iY = 0;

	int currentPage = 0;

	for( int ch = 0; ch < 256; ++ch )
	{
		staticFont->getCharABCwide( ch, abcA, abcB, abcC );

		//On Linux, use the font's width instead
#ifndef WIN32
		auto wide = font->getWide();
#else
		auto wide = abcB;
#endif

		if( isprint( ch ) )
		{
			auto tall = staticFont->getTall();

			if( iX + wide >= FONT_PAGE_SIZE )
			{
				iX = 0;
				iY += tall + 1;
			}

			if( iY + tall >= FONT_PAGE_SIZE )
			{
				//Out of space in this page, upload and allocate next
				if( !staticFontInfoVGUI->bindIndex[ currentPage ] )
				{
					staticFontInfoVGUI->bindIndex[ currentPage ] = createNewTextureID();
				}

				drawSetTextureRGBA( staticFontInfoVGUI->bindIndex[ currentPage ], reinterpret_cast<const char*>( staticRGBA ), FONT_PAGE_SIZE, FONT_PAGE_SIZE );

				++currentPage;

				iX = 0;
				iY = 0;

				memset( staticRGBA, 0, sizeof( staticRGBA ) );
			}
			else
			{
				tall += iY;
			}

			if( currentPage == FONT_MAX_PAGES )
				break;

			staticFont->getCharRGBA( ch, iX, iY, FONT_PAGE_SIZE, FONT_PAGE_SIZE, staticRGBA );

			staticFontInfoVGUI->pageForChar[ ch ] = currentPage;

			staticFontInfoVGUI->texCoord[ ch ][ 0 ] = iX / static_cast<float>( FONT_PAGE_SIZE );
			staticFontInfoVGUI->texCoord[ ch ][ 1 ] = iY / static_cast<float>( FONT_PAGE_SIZE );
			staticFontInfoVGUI->texCoord[ ch ][ 2 ] = ( iX + wide ) / static_cast<float>( FONT_PAGE_SIZE );
			staticFontInfoVGUI->texCoord[ ch ][ 3 ] = tall / static_cast<float>( FONT_PAGE_SIZE );

			iX += wide + 1;
		}
	}

	//Make sure the last page is uploaded.
	if( !staticFontInfoVGUI->bindIndex[ currentPage ] )
	{
		staticFontInfoVGUI->bindIndex[ currentPage ] = createNewTextureID();
	}

	drawSetTextureRGBA( staticFontInfoVGUI->bindIndex[ currentPage ], reinterpret_cast<const char*>( staticRGBA ), FONT_PAGE_SIZE, FONT_PAGE_SIZE );

	//This can be 9 if we've previously filled up 8 pages.
	++currentPage;

	staticFontInfoVGUI->pageCount = currentPage;
}

void EngineSurfaceWrap::drawSetTextColor( int r, int g, int b, int a )
{
	_engineSurface->drawSetTextColor( r, g, b, a );
}

void EngineSurfaceWrap::drawSetTextPos( int x, int y )
{
	_engineSurface->drawSetTextPos( x, y );
}

void EngineSurfaceWrap::drawPrintText( const char* text, int textLen )
{
	if( text && staticFont && staticFontInfoVGUI )
	{
		int iX, iY;
		drawGetTextPos( iX, iY );

		auto tall = staticFont->getTall();

		int a, b, c;

		for( int i = 0; i < textLen; ++i )
		{
			const auto ch = text[ i ];

			staticFont->getCharABCwide( ch, a, b, c );

			iX += a;

			//On Linux, use the font's width instead
#ifndef WIN32
			auto wide = font->getWide();
#else
			auto wide = b;
#endif

			drawSetTexture( staticFontInfoVGUI->bindIndex[ staticFontInfoVGUI->pageForChar[ ch ] ] );

			//TODO: not 100% sure this isn't supposed to be *Add - Solokiller
			drawPrintChar(
				iX, iY, wide, tall,
				staticFontInfoVGUI->texCoord[ ch ][ 0 ],
				staticFontInfoVGUI->texCoord[ ch ][ 1 ],
				staticFontInfoVGUI->texCoord[ ch ][ 2 ],
				staticFontInfoVGUI->texCoord[ ch ][ 3 ]
			);

			iX += wide + c;
}

		drawSetTextPos( iX, iY );
	}
}

void EngineSurfaceWrap::drawSetTextureRGBA( int id, const char* rgba, int wide, int tall )
{
	_engineSurface->drawSetTextureRGBA( id, reinterpret_cast<const byte*>( rgba ), wide, tall, false, false ); 
}

void EngineSurfaceWrap::drawSetTexture( int id )
{
	_engineSurface->drawSetTexture( id );
}

void EngineSurfaceWrap::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	_engineSurface->drawTexturedRect( x0, y0, x1, y1 );
}

void EngineSurfaceWrap::invalidate( vgui::Panel* panel )
{
	// Nothing
}

void EngineSurfaceWrap::enableMouseCapture( bool state )
{
	// Nothing
}

void EngineSurfaceWrap::setCursor( vgui::Cursor* cursor )
{
	if( mousecontrol->VGUI2MouseControl() ||
		VGuiWrap2_IsGameUIVisible() ||
		!cursor ||
		_cursorLocked )
		return;

	_currentCursor = cursor;
	const bool bWasVisible = s_bCursorVisible;
	s_bCursorVisible = true;
	_emulatedCursor->setImage( _emulatedMouseImage );

	switch( cursor->getDefaultCursor() )
	{
	case vgui::Cursor::dc_none:
		{
			_emulatedCursor->setImage( nullptr );
			s_bCursorVisible = false;
		}

	case vgui::Cursor::dc_arrow:
	case vgui::Cursor::dc_ibeam:
	case vgui::Cursor::dc_hourglass:
	case vgui::Cursor::dc_crosshair:
	case vgui::Cursor::dc_up:
	case vgui::Cursor::dc_sizenwse:
	case vgui::Cursor::dc_sizenesw:
	case vgui::Cursor::dc_sizewe:
	case vgui::Cursor::dc_sizens:
	case vgui::Cursor::dc_sizeall:
	case vgui::Cursor::dc_no:
	case vgui::Cursor::dc_hand:
		{
			staticCurrentCursor = staticDefaultCursor[ cursor->getDefaultCursor() ];
			break;
		}

	default: break;
	}

	if( s_bCursorVisible )
		SDL_SetCursor( staticCurrentCursor );

	if( !_cursorLocked )
	{
		//Cursor state change, update.
		if( s_bCursorVisible != bWasVisible )
		{
			if( m_rawinput.value && BUsesSDLInput() )
			{
				SDL_SetRelativeMouseMode( s_bCursorVisible ? SDL_FALSE : SDL_TRUE );
			}
			else
			{
				SDL_ShowCursor( s_bCursorVisible ? 1 : 0 );
			}
}

		SDL_PumpEvents();

		int x, y;
		SDL_GetRelativeMouseState( &x, &y );
	}
}

void EngineSurfaceWrap::swapBuffers()
{
	// Nothing
}

void EngineSurfaceWrap::pushMakeCurrent( vgui::Panel* panel, bool useInsets )
{
	int insets[ 4 ] = { 0, 0, 0, 0 };
	int absExtents[ 4 ];
	int clipRect[ 4 ];

	if( useInsets )
	{
		panel->getInset( 
			insets[ 0 ],
			insets[ 1 ],
			insets[ 2 ],
			insets[ 3 ]
		);
	}

	panel->getAbsExtents(
		absExtents[ 0 ],
		absExtents[ 1 ],
		absExtents[ 2 ],
		absExtents[ 3 ]
	);

	panel->getClipRect(
		clipRect[ 0 ],
		clipRect[ 1 ],
		clipRect[ 2 ],
		clipRect[ 3 ]
	);

	_engineSurface->pushMakeCurrent( insets, absExtents, clipRect, true );
}

void EngineSurfaceWrap::popMakeCurrent( vgui::Panel* panel )
{
	_engineSurface->popMakeCurrent();
}

void EngineSurfaceWrap::applyChanges()
{
	// Nothing
}

static void CheckModState( vgui::App* app, vgui::SurfaceBase* surface )
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
				app->internalKeyReleased( key, surface );
			}
			else
			{
				app->internalKeyPressed( key, surface );
				//TODO: missing? - Solokiller
				//app->internalKeyCodeTyped( key, surface );
			}
		}
	};

	helper( KMOD_LSHIFT, vgui::KEY_LSHIFT );
	helper( KMOD_LALT, vgui::KEY_LALT );
	helper( KMOD_LCTRL, vgui::KEY_LCONTROL );

	//TODO: shouldn't this be KEY_RSHIFT? - Solokiller
	helper( KMOD_RSHIFT, vgui::KEY_LSHIFT );
	helper( KMOD_RALT, vgui::KEY_RALT );
	helper( KMOD_RCTRL, vgui::KEY_RCONTROL );

	helper( KMOD_CAPS, vgui::KEY_CAPSLOCK );
	helper( KMOD_LGUI, vgui::KEY_LWIN );
	helper( KMOD_RGUI, vgui::KEY_RWIN );

	s_lastModifierCode = state;
}

void EngineSurfaceWrap::AppHandler( void* event, void* userData )
{
	auto pApp = vgui::App::getInstance();

	if( pApp && this )
	{
		auto& ev = *reinterpret_cast<SDL_Event*>( event );

		switch( ev.type )
		{
		case SDL_MOUSEMOTION:
			{
				if( VideoMode_IsWindowed() )
				{
					pApp->internalCursorMoved( ev.motion.x, ev.motion.y, this );
				}
				else
				{
					int wW, wT;
					SDL_GetWindowSize( reinterpret_cast<SDL_Window*>( game->GetMainWindow() ), &wW, &wT );

					int vW, vT;
					VideoMode_GetCurrentVideoMode( &vW, &vT, nullptr );

					const int relX = static_cast<int>( floor( ev.motion.x * ( static_cast<double>( vW ) / wW ) ) );
					const int relY = static_cast<int>( floor( ev.motion.y * ( static_cast<double>( vT ) / wT ) ) );

					const double flX = static_cast<double>( relX - vW / 2 ) * ( GetXMouseAspectRatioAdjustment() - 1.0 ) + relX;
					const double flY = static_cast<double>( relY - vT / 2 ) * ( GetYMouseAspectRatioAdjustment() - 1.0 ) + relY;

					pApp->internalCursorMoved(
						static_cast<int>( flX ),
						static_cast<int>( flY ),
						this
					);
				}
				break;
			}

		case SDL_MOUSEWHEEL:
			{
				pApp->internalMouseWheeled( ev.wheel.x, this );
				break;
			}

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			{
				vgui::MouseCode code;

				switch( ev.button.button )
				{
				default:
				case SDL_BUTTON_LEFT:
					{
						code = vgui::MOUSE_LEFT;
						break;
					}

				case SDL_BUTTON_MIDDLE:
					{
						code = vgui::MOUSE_MIDDLE;
						break;
					}

				case SDL_BUTTON_RIGHT:
					{
						code = vgui::MOUSE_RIGHT;
						break;
					}
				}

				if( ev.type == SDL_MOUSEBUTTONDOWN )
					pApp->internalMousePressed( code, this );
				else
					pApp->internalMouseReleased( code, this );

				break;
			}

		case SDL_KEYDOWN:
			{
				//VGUI1 key codes start at KEY_0, VGUI2 key codes start at KEY_NONE, so adjust the code
				pApp->internalKeyPressed( static_cast<vgui::KeyCode>( KeyCode_VirtualKeyToVGUI( ev.key.keysym.sym ) - vgui2::KEY_0 ), this );
				pApp->internalKeyTyped( static_cast<vgui::KeyCode>( KeyCode_VirtualKeyToVGUI( ev.key.keysym.sym ) - vgui2::KEY_0 ), this );
				
				CheckModState( pApp, this );
				break;
			}

		case SDL_KEYUP:
			{
				pApp->internalKeyReleased( static_cast<vgui::KeyCode>( KeyCode_VirtualKeyToVGUI( ev.key.keysym.sym ) - vgui2::KEY_0 ), this );
				
				CheckModState( pApp, this );
				break;
			}

			//TODO: missing case for double pressed - Solokiller

		default: break;
		}
	}
}

void EngineSurfaceWrap::lockCursor()
{
	_cursorLocked = true;
}

void EngineSurfaceWrap::unlockCursor()
{
	_cursorLocked = false;

	setCursor( _currentCursor );
}

void EngineSurfaceWrap::drawLine( int x1, int y1, int x2, int y2 )
{
	_engineSurface->drawLine( x1, y1, x2, y2 );
}

void EngineSurfaceWrap::drawPolyLine( int* px, int* py, int n )
{
	_engineSurface->drawPolyLine( px, py, n );
}

void EngineSurfaceWrap::drawTexturedPolygon( vgui2::VGuiVertex* pVertices, int n )
{
	_engineSurface->drawTexturedPolygon( pVertices, n );
}

void EngineSurfaceWrap::drawSetTextureBGRA( int id, const char* rgba, int wide, int tall, int hardwareFilter, int hasAlphaChannel )
{
	_engineSurface->drawSetTextureBGRA( id, reinterpret_cast<const byte*>( rgba ), wide, tall, hardwareFilter, hasAlphaChannel );
}

void EngineSurfaceWrap::drawUpdateRegionTextureBGRA( int nTextureID, int x, int y, const byte* pchData, int wide, int tall )
{
	_engineSurface->drawUpdateRegionTextureBGRA( nTextureID, x, y, pchData, wide, tall );
}

void EngineSurfaceWrap::drawGetTextPos( int& x, int& y )
{
	_engineSurface->drawGetTextPos( x, y );
}

void EngineSurfaceWrap::drawPrintChar( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 )
{
	_engineSurface->drawPrintChar( x, y, wide, tall, s0, t0, s1, t1 );
}

void EngineSurfaceWrap::drawPrintCharAdd( int x, int y, int wide, int tall, float s0, float t0, float s1, float t1 )
{
	_engineSurface->drawPrintCharAdd( x, y, wide, tall, s0, t0, s1, t1 );
}