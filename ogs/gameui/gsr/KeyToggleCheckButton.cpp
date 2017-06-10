#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "igameuifuncs.h"
#include "KeyToggleCheckButton.h"

CKeyToggleCheckButton::CKeyToggleCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* key, const char* cmdname )
	: BaseClass( parent, panelName, text )
{
	m_pszKeyName = key ? strdup( key ) : nullptr;
	m_pszCmdName = cmdname ? strdup( cmdname ) : nullptr;

	if( m_pszKeyName )
	{
		gameuifuncs->IsKeyDown( m_pszKeyName, m_bStartValue );

		if( HasBeenModified() )
			SetSelected( m_bStartValue );
	}
}

CKeyToggleCheckButton::~CKeyToggleCheckButton()
{
	free( m_pszKeyName );
	free( m_pszCmdName );
}

void CKeyToggleCheckButton::Paint()
{
	BaseClass::Paint();

	bool isdown;

	if( m_pszKeyName &&
		gameuifuncs->IsKeyDown( m_pszKeyName, isdown ) &&
		m_bStartValue != isdown )
	{
		SetSelected( isdown );
		m_bStartValue = isdown;
	}
}

void CKeyToggleCheckButton::Reset()
{
	gameuifuncs->IsKeyDown( m_pszKeyName, m_bStartValue );

	if( HasBeenModified() )
		SetSelected( m_bStartValue );
}

void CKeyToggleCheckButton::ApplyChanges()
{
	if( m_pszCmdName && *m_pszCmdName )
	{
		char szCommand[ 256 ];
		snprintf( szCommand, ARRAYSIZE( szCommand ), "%c%s\n", IsSelected() ? '+' : '-', m_pszCmdName );
		engine->pfnClientCmd( szCommand );
	}
}

bool CKeyToggleCheckButton::HasBeenModified()
{
	return m_bStartValue != IsSelected();
}
