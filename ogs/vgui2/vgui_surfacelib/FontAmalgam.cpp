#include <tier0/platform.h>

#include "FontAmalgam.h"
#include "Win32Font.h"

CFontAmalgam::CFontAmalgam()
	: m_Fonts( 0, 4 )
{
}

CFontAmalgam::~CFontAmalgam()
{
	m_Fonts.Purge();
}

void CFontAmalgam::SetName( const char* name )
{
	strncpy( m_szName, name, ARRAYSIZE( m_szName ) - 1 );
	m_szName[ ARRAYSIZE( m_szName ) - 1 ] = '\0';
}

void CFontAmalgam::AddFont( CWin32Font* font, int lowRange, int highRange )
{
	auto& range = m_Fonts[ m_Fonts.AddToTail() ];

	range.font = font;
	range.lowRange = lowRange;
	range.highRange = highRange;

	m_iMaxHeight = max( m_iMaxHeight, font->GetHeight() );
	m_iMaxWidth = max( m_iMaxWidth, font->GetMaxCharWidth() );
}

CWin32Font* CFontAmalgam::GetFontForChar( int ch )
{
	if( m_Fonts.Count() <= 0 )
		return nullptr;

	auto pRange = &m_Fonts[ 0 ];

	for( int i = 0; i < m_Fonts.Count(); ++i, ++pRange )
	{
		if( pRange->lowRange <= ch && ch <= pRange->highRange )
		{
			return pRange->font;
		}
	}

	return nullptr;
}

int CFontAmalgam::GetFontHeight()
{
	if( m_Fonts.Count() )
		return m_Fonts[ 0 ].font->GetHeight();

	return m_iMaxHeight;
}

int CFontAmalgam::GetFontMaxWidth()
{
	return m_iMaxWidth;
}

const char* CFontAmalgam::GetFontName( int i )
{
	return m_Fonts[ i ].font->GetName();
}

int CFontAmalgam::GetFlags( int i )
{
	if( m_Fonts[ i ].font )
		return m_Fonts[ i ].font->GetFlags();
	
	return 0;
}

int CFontAmalgam::GetCount()
{
	return m_Fonts.Count();
}

bool CFontAmalgam::GetUnderlined()
{
	if( m_Fonts.Count() )
		return m_Fonts[ 0 ].font->GetUnderlined();

	return false;
}

void CFontAmalgam::RemoveAll()
{
	//Note: this is never called so it didn't exist in the binary, this is just a guess. - Solokiller
	m_Fonts.RemoveAll();

	m_iMaxWidth = 0;
	m_iMaxHeight = 0;
}