#include <KeyValues.h>

#include "CvarTextEntry.h"
#include "EngineInterface.h"

CCvarTextEntry::CCvarTextEntry( vgui2::Panel* parent, const char* panelName, const char* cvarname )
	: BaseClass( parent, panelName )
{
	m_pszStartValue[ 0 ] = '\0';

	if( cvarname )
	{
		m_pszCvarName = strdup( cvarname );
	}
	else
		m_pszCvarName = nullptr;

	if( m_pszCvarName )
	{
		auto pszValue = engine->pfnGetCvarString( m_pszCvarName );

		if( pszValue && *pszValue )
		{
			SetText( pszValue );
			strncpy( m_pszStartValue, pszValue , ARRAYSIZE( m_pszStartValue ) );
			m_pszStartValue[ ARRAYSIZE( m_pszStartValue ) - 1 ] = '\0';
		}
	}

	AddActionSignalTarget( this );
}

CCvarTextEntry::~CCvarTextEntry()
{
	free( m_pszCvarName );
}

void CCvarTextEntry::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	//TODO: shouldn't this be >= MAX_VALUE_SIZE? - Solokiller
	if( GetMaximumCharCount() < 0 || GetMaximumCharCount() > MAX_VALUE_SIZE )
		SetMaximumCharCount( MAX_VALUE_SIZE - 1 );
}

void CCvarTextEntry::OnTextChanged()
{
	if( m_pszCvarName )
	{
		char buf[ MAX_VALUE_SIZE ];

		GetText( buf, ARRAYSIZE( buf ) );

		if( stricmp( buf, m_pszStartValue ) )
		{
			PostActionSignal( new KeyValues( "ControlModified" ) );
		}
	}
}

void CCvarTextEntry::ApplyChanges( bool immediate )
{
	if( m_pszCvarName )
	{
		char szText[ MAX_VALUE_SIZE ];
		GetText( szText, ARRAYSIZE( szText ) );

		if( szText[ 0 ] )
		{
			if( immediate )
			{
				engine->Cvar_Set( m_pszCvarName, szText );
			}
			else
			{
				char szCommand[ 256 ];
				snprintf( szCommand, ARRAYSIZE( szCommand ), "%s \"%s\"\n", m_pszCvarName, szText );
				engine->pfnClientCmd( szCommand );
			}
			strncpy( m_pszStartValue, szText, ARRAYSIZE( m_pszStartValue ) );
			m_pszStartValue[ ARRAYSIZE( m_pszStartValue ) - 1 ] = '\0';
		}
	}
}

void CCvarTextEntry::Reset()
{
	auto pszValue = engine->pfnGetCvarString( m_pszCvarName );

	if( pszValue && *pszValue )
	{
		SetText( pszValue );
		strncpy( m_pszStartValue, pszValue, ARRAYSIZE( m_pszStartValue ) );
		m_pszStartValue[ ARRAYSIZE( m_pszStartValue ) - 1 ] = '\0';
	}
}

bool CCvarTextEntry::HasBeenModified()
{
	char szText[ MAX_VALUE_SIZE ];
	GetText(szText, ARRAYSIZE( szText ) );

	return stricmp( szText, m_pszStartValue ) != 0;
}
