#include <cwctype>

#include "commondef.hpp"
#include "BaseUISurface.hpp"
#include "EngineSurface.hpp"
#include "text_draw.hpp"

#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui_controls/Controls.h>

static vgui2::HFont _consoleFont = NULL_HANDLE;
static vgui2::HFont _creditsFont = NULL_HANDLE;

static SDK_Color _col( 255, 255, 255, 255 );

void VGUI2_Draw_Init()
{
	auto pScheme = vgui2::scheme()->GetIScheme( vgui2::scheme()->GetDefaultScheme() );

	if( !_consoleFont )
		_consoleFont = pScheme->GetFont( "EngineFont", true );

	if( !_creditsFont )
		_creditsFont = pScheme->GetFont( "Legacy_CreditsFont", true );
}

void VGUI2_ResetCurrentTexture()
{
	g_BaseUISurface.m_iCurrentTexture = 0;
}

unsigned int VGUI2_GetConsoleFont()
{
	return static_cast<unsigned int>( _consoleFont );
}

unsigned int VGUI2_GetCreditsFont()
{
	return static_cast<unsigned int>( _creditsFont );
}

void VGUI2_Draw_SetTextColor( int r, int g, int b )
{
	_col[ 3 ] = 255;
	_col[ 0 ] = r;
	_col[ 1 ] = g;
	_col[ 2 ] = b;
}

void VGUI2_Draw_SetTextColorAlpha( int r, int g, int b, int a )
{
	_col[ 0 ] = r;
	_col[ 1 ] = g;
	_col[ 2 ] = b;
	_col[ 3 ] = a;
}

const wchar_t* VGUI2_Find_String( const char* str )
{
	static wchar_t tmpString[ 1024 ];

	if( str && *str == '#' )
	{
		auto pszLocalized = vgui2::localize()->Find( str );

		return pszLocalized;
	}

	vgui2::localize()->ConvertANSIToUnicode( str, tmpString, sizeof( tmpString ) );

	return tmpString;
}

int VGUI2_GetFontWide( wchar_t ch, unsigned int font )
{
	int a, b, c;
	vgui2::surface()->GetCharABCwide( font, ch, a, b, c );

	return c + b + a;
}

int VGUI2_GetFontTall( unsigned int font )
{
	return vgui2::surface()->GetFontTall( font );
}

int VGUI2_Draw_StringLen( const char* psz, unsigned int font )
{
	return VGUI2_Draw_StringLenW( VGUI2_Find_String( psz ), font );
}

int VGUI2_Draw_StringLenW( const wchar_t* wsz, unsigned int font )
{
	int len = 0;

	for( size_t i = 0; i < wcslen( wsz ); ++i )
	{
		len += VGUI2_GetFontWide( wsz[ i ], font );
	}

	return len;
}

int VGUI2_DrawString( int x, int y, const char* str, unsigned int font )
{
	g_BaseUISurface._engineSurface->resetViewPort();

	vgui2::surface()->DrawSetTextFont( font );
	vgui2::surface()->DrawSetTextPos( x, y );
	vgui2::surface()->DrawSetTextColor( _col );

	auto pszString = VGUI2_Find_String( str );

	for( size_t i = 0; i < wcslen( pszString ); ++i )
	{
		if( !iswprint( pszString[ i ] ) )
		{
			//TODO: this could modify localized strings managed by ILocalize - Solokiller
			const_cast<wchar_t*>( pszString )[ i ] = L' ';
		}
	}

	vgui2::surface()->DrawPrintText( pszString, wcslen( pszString ) );
	vgui2::surface()->DrawFlushText();

	return VGUI2_Draw_StringLenW( pszString, font );
}

int VGUI2_DrawStringClient( int x, int y, const char* str, int r, int g, int b )
{
	g_BaseUISurface._engineSurface->resetViewPort();

	auto pszString = VGUI2_Find_String( str );

	int endX = x;

	for( size_t i = 0; i < wcslen( pszString ); ++i )
	{
		endX += VGUI2_Draw_CharacterAdd( endX, y, pszString[ i ], r, g, b, _creditsFont );
	}

	return endX - x;
}

int VGUI2_DrawStringReverseClient( int x, int y, const char* str, int r, int g, int b )
{
	g_BaseUISurface._engineSurface->resetViewPort();

	auto pszString = VGUI2_Find_String( str );

	//TODO: this assumes that the string has at least one character - Solokiller
	auto pszNext = &pszString[ wcslen( pszString ) - 2 ];

	//Use the width of the last character for metrics
	int a, z, c;
	vgui2::surface()->GetCharABCwide( _creditsFont, *pszNext, a, z, c );

	const auto charWidth = a + z + c;

	int endX = x - charWidth;

	for( ; pszNext != pszString; --pszNext )
	{
		endX -= VGUI2_Draw_CharacterAdd( endX, y, *pszNext, r, g, b, _creditsFont );
	}

	return x - endX + charWidth;
}

int VGUI2_Draw_Character( int x, int y, int ch, unsigned int font )
{
	g_BaseUISurface._engineSurface->resetViewPort();

	vgui2::surface()->DrawSetTextFont( font );
	vgui2::surface()->DrawSetTextPos( x, y );
	vgui2::surface()->DrawSetTextColor( _col );

	if( iswprint( ch ) )
	{
		vgui2::surface()->DrawUnicodeChar( ch );
		vgui2::surface()->DrawFlushText();
	}

	return VGUI2_GetFontWide( ch, font );
}

int VGUI2_Draw_CharacterAdd( int x, int y, int ch, int r, int g, int b, unsigned int font )
{
	g_BaseUISurface._engineSurface->resetViewPort();

	VGUI2_Draw_SetTextColor( r, g, b );

	vgui2::surface()->DrawSetTextFont( font );
	vgui2::surface()->DrawSetTextPos( x, y );
	vgui2::surface()->DrawSetTextColor( _col );

	if( iswprint( ch ) )
	{
		vgui2::surface()->DrawUnicodeCharAdd( ch );
	}

	//TODO: unused? - Solokiller
	VGUI2_GetFontTall( font );

	return VGUI2_GetFontWide( ch, font );
}

int VGUI2_MessageFontInfo( short* pWidth, unsigned int font )
{
	//TODO: is this needed to calculate font sizes? - Solokiller
	vgui2::surface()->DrawSetTextFont( font );

	if( pWidth )
	{
		for( int i = 0; i < 256; ++i )
		{
			pWidth[ i ] = VGUI2_GetFontWide( i, font );
		}
	}

	return VGUI2_GetFontTall( font );
}