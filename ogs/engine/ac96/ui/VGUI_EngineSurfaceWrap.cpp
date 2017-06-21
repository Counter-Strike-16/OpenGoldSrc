#include <SDL2/SDL.h>

#include <VGUI_Cursor.h>
#include <VGUI_Dar.h>
#include <VGUI_FileInputStream.h>
#include <VGUI_Font.h>

#include "quakedef.h"

#include "VGUI_EngineSurfaceWrap.h"

struct FontInfoVGUI
{
	int id;
	int pageCount;
	int pageForChar[ 256 ];
	int bindIndex[ 8 ];
	float texCoord[ 256 ][ 8 ];
	int contextCount;
};

static vgui::Font* staticFont = nullptr;
static FontInfoVGUI* staticFontInfoVGUI = nullptr;

static vgui::Dar<FontInfoVGUI*> staticFontInfoVGUIDar;

static int staticContextCount = 0;

//vgui::Cursor::DefaultCursor->SDL_SystemCursor mapping
// TODO: why does this have a size of 20 when only 14 are needed?
static SDL_Cursor* staticDefaultCursor[ 20 ] = {};

//The currently active cursor
static SDL_Cursor* staticCurrentCursor = nullptr;

CEngineSurfaceWrap::CEngineSurfaceWrap( vgui::Panel* embeddedPanel, IEngineSurface* engineSurface )
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

CEngineSurfaceWrap::~CEngineSurfaceWrap()
{
	delete _emulatedMouseImage;

	for( int i = 0; i < ARRAYSIZE( staticDefaultCursor ); ++i )
	{
		if( staticDefaultCursor[ i ] )
			SDL_FreeCursor( staticDefaultCursor[ i ] );
	}
}

void CEngineSurfaceWrap::setTitle( const char* title )
{
	// Nothing
}

bool CEngineSurfaceWrap::setFullscreenMode( int wide, int tall, int bpp )
{
	// Nothing
	return false;
}

void CEngineSurfaceWrap::setWindowedMode()
{
	// Nothing
}

void CEngineSurfaceWrap::setAsTopMost( bool state )
{
	// Nothing
}

void CEngineSurfaceWrap::createPopup( vgui::Panel* embeddedPanel )
{
	// Nothing
}

bool CEngineSurfaceWrap::hasFocus()
{
	return true;
}

bool CEngineSurfaceWrap::isWithin( int x, int y )
{
	// Nothing
	return true;
}

int CEngineSurfaceWrap::createNewTextureID()
{
	return 0;
}

void CEngineSurfaceWrap::GetMousePos( int &x, int &y )
{
}

void CEngineSurfaceWrap::drawSetColor( int r, int g, int b, int a )
{
}

void CEngineSurfaceWrap::drawFilledRect( int x0, int y0, int x1, int y1 )
{
}

void CEngineSurfaceWrap::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
}

void CEngineSurfaceWrap::drawSetTextFont( vgui::Font* font )
{
}

void CEngineSurfaceWrap::drawSetTextColor( int r, int g, int b, int a )
{
}

void CEngineSurfaceWrap::drawSetTextPos( int x, int y )
{
}

void CEngineSurfaceWrap::drawPrintText( const char* text, int textLen )
{
}

void CEngineSurfaceWrap::drawSetTextureRGBA( int id, const char* rgba, int wide, int tall )
{
}

void CEngineSurfaceWrap::drawSetTexture( int id )
{
}

void CEngineSurfaceWrap::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
}

void CEngineSurfaceWrap::invalidate( vgui::Panel* panel )
{
	// Nothing
}

void CEngineSurfaceWrap::enableMouseCapture( bool state )
{
	// Nothing
}

void CEngineSurfaceWrap::setCursor( vgui::Cursor* cursor )
{
}

void CEngineSurfaceWrap::swapBuffers()
{
	// Nothing
}

void CEngineSurfaceWrap::pushMakeCurrent( vgui::Panel* panel, bool useInsets )
{
}

void CEngineSurfaceWrap::popMakeCurrent( vgui::Panel* panel )
{
}

void CEngineSurfaceWrap::applyChanges()
{
	// Nothing
}

void CEngineSurfaceWrap::AppHandler( void* event, void* userData )
{
}

void CEngineSurfaceWrap::lockCursor()
{
}

void CEngineSurfaceWrap::unlockCursor()
{
}