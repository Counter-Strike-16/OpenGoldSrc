#include <cwctype>
#include <clocale>

#include <tier0/platform.h>

#include "FontManager.h"
#include "Win32Font.h"

#undef CreateFont

static CFontManager s_FontManager;

struct Win98ForeignFallbackFont_t
{
	const char* language;
	const char* fallbackFont;
};

Win98ForeignFallbackFont_t g_Win98ForeignFallbackFonts[] = 
{
	{ "russian", "system" },
	{ "japanese", "win98japanese" },
	{ "thai", "system" },
	{ nullptr, "Tahoma" }
};

struct FallbackFont_t
{
	const char* font;
	const char* fallbackFont;
};

FallbackFont_t g_FallbackFonts[] =
{
	{ "Times New Roman", "Courier New" },
	{ "Courier New", "Courier" },
	{ "Verdana", "Arial" },
	{ "Trebuchet MS", "Arial" },
	{ "Tahoma", nullptr },
	{ nullptr, "Tahoma" }
};

static const char* const g_szValidAsianFonts[] = 
{
	"Marlett",
	nullptr
};

CFontManager& FontManager()
{
	return s_FontManager;
}

CFontManager::CFontManager()
{
	m_FontAmalgams.EnsureCapacity( 100 );

	//Font 0 is a dummy since HFont is 1 based.
	m_FontAmalgams.InsertBefore( m_FontAmalgams.Count() );

	m_Win32Fonts.EnsureCapacity( 100 );

	setlocale( LC_ALL, "" );
	setlocale( LC_CTYPE, "" );
	setlocale( LC_MONETARY, "" );
	setlocale( LC_NUMERIC, "" );
}

CFontManager::~CFontManager()
{
	ClearAllFonts();
}

void CFontManager::SetLanguage( const char* language )
{
	strncpy( m_szLanguage, language, ARRAYSIZE( m_szLanguage ) );
}

void CFontManager::ClearAllFonts()
{
	m_Win32Fonts.PurgeAndDeleteElements();
}

int CFontManager::GetFontByName( const char* name )
{
	for( int i = 1; i < m_FontAmalgams.Count(); ++i )
	{
		if( !stricmp( name, m_FontAmalgams[ i ].Name() ) )
		{
			return i;
		}
	}

	return 0;
}

CWin32Font* CFontManager::GetFontForChar( vgui2::HFont font, wchar_t ch )
{
	return m_FontAmalgams[ font ].GetFontForChar( ch );
}

void CFontManager::GetCharABCwide( vgui2::HFont font, int ch, int& a, int& b, int& c )
{
	auto pFont = m_FontAmalgams[ font ].GetFontForChar( ch );

	if( pFont )
	{
		pFont->GetCharABCWidths( ch, a, b, c );
	}
	else
	{
		c = 0;
		a = 0;
		b = m_FontAmalgams[ font ].GetFontMaxWidth();
	}
}

int CFontManager::GetFontTall( vgui2::HFont font )
{
	return m_FontAmalgams[ font ].GetFontHeight();
}

int CFontManager::GetFontAscent( vgui2::HFont font, wchar_t wch )
{
	auto pFont = m_FontAmalgams[ font ].GetFontForChar( wch );

	if( pFont )
		return pFont->GetAscent();

	return 0;
}

int CFontManager::GetCharacterWidth( vgui2::HFont font, int ch )
{
	if( iswprint( ch ) )
	{
		int a, b, c;

		auto pFont = m_FontAmalgams[ font ].GetFontForChar( ch );

		if( pFont )
		{
			pFont->GetCharABCWidths( ch, a, b, c );
		}
		else
		{
			c = 0;
			a = 0;
			b = m_FontAmalgams[ font ].GetFontMaxWidth();
		}

		return c + b + a;
	}

	return 0;
}

void CFontManager::GetTextSize( vgui2::HFont font, const wchar_t* text, int& wide, int& tall )
{
	wide = 0;
	tall = 0;

	if( !text )
		return;

	tall = m_FontAmalgams[ font ].GetFontHeight();

	for( const wchar_t* pszNext = text;  *pszNext; ++pszNext )
	{
		while( *pszNext == L'\n' )
		{
			++pszNext;

			tall += m_FontAmalgams[ font ].GetFontHeight();

			if( !( *pszNext ) )
				return;
		}

		//TODO: why does this check exist? - Solokiller
		if( *pszNext != L'&' )
		{
			const auto width = GetCharacterWidth( font, *pszNext );

			if( width > wide )
				wide = width;
		}
	}
}

bool CFontManager::IsFontForeignLanguageCapable( const char* windowsFontName )
{
	for( int i = 0; g_szValidAsianFonts[ i ]; ++i )
	{
		if( !stricmp( g_szValidAsianFonts[ i ], windowsFontName ) )
			return true;
	}

	return false;
}

const char* CFontManager::GetFallbackFontName( const char* windowsFontName )
{
	int i;

	for( i = 0; g_FallbackFonts[ i ].font; ++i )
	{
		if( !stricmp( g_FallbackFonts[ i ].font, windowsFontName ) )
			break;
	}

	return g_FallbackFonts[ i ].fallbackFont;
}

const char* CFontManager::GetForeignFallbackFontName()
{
	if( s_bSupportsUnicode )
		return "Tahoma";

	int i;

	for( i = 0; g_Win98ForeignFallbackFonts[ i ].language; ++i )
	{
		if( !stricmp( g_Win98ForeignFallbackFonts[ i ].language, m_szLanguage ) )
			break;
	}

	return g_FallbackFonts[ i ].fallbackFont;
}

bool CFontManager::GetFontUnderlined( vgui2::HFont font )
{
	return m_FontAmalgams[ font ].GetUnderlined();
}

vgui2::HFont CFontManager::CreateFont()
{
	return static_cast<vgui2::HFont>( m_FontAmalgams.AddToTail() );
}

CWin32Font* CFontManager::CreateOrFindWin32Font( const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
{
	for( int i = 0; i < m_Win32Fonts.Count(); ++i )
	{
		if( m_Win32Fonts[ i ]->IsEqualTo(
			windowsFontName,
			tall,
			weight,
			blur,
			scanlines,
			flags ) )
		{
			return m_Win32Fonts[ i ];
		}
	}

	auto pFont = new CWin32Font();

	m_Win32Fonts[ m_Win32Fonts.AddToTail() ] = pFont;

	if( !pFont->Create(
		windowsFontName,
		tall,
		weight,
		blur,
		scanlines,
		flags
	) )
	{
		delete pFont;
		m_Win32Fonts.Remove( m_Win32Fonts.Count() - 1 );

		return nullptr;
	}

	return pFont;
}

bool CFontManager::AddGlyphSetToFont( vgui2::HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
{
	auto& fontAmalgam = m_FontAmalgams[ font ];

	if( fontAmalgam.GetCount() > 0 )
		return false;

	auto pFont = CreateOrFindWin32Font(
		windowsFontName,
		tall, weight,
		blur, scanlines,
		flags
	);

	for( auto pszFontName = windowsFontName;
		 pszFontName;
		 pszFontName = GetFallbackFontName( pszFontName ) )
	{
		auto pszForeignFallback = GetForeignFallbackFontName();

		if( pFont &&
			(
				IsFontForeignLanguageCapable( pszFontName ) ||
				!stricmp( pszForeignFallback, pszFontName )
			)
		)
		{
			fontAmalgam.AddFont( pFont, 0, 0xFFFF );
			return true;
		}

		auto pSecondFont = CreateOrFindWin32Font(
			pszForeignFallback,
			tall, weight,
			blur, scanlines,
			flags
		);

		if( pSecondFont )
		{
			if( !pFont )
			{
				fontAmalgam.AddFont( pSecondFont, 0, 0xFFFF );
			}
			else
			{
				fontAmalgam.AddFont( pFont, 0, 0xFF );
				fontAmalgam.AddFont( pSecondFont, 0x100, 0xFFFF );
			}

			return true;
		}
	}

	return false;
}