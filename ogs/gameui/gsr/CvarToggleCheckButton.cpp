#include <KeyValues.h>

#include "CvarToggleCheckButton.h"
#include "EngineInterface.h"

CCvarToggleCheckButton::CCvarToggleCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* cvarname )
	: BaseClass( parent, panelName, text )
{
	if( cvarname )
	{
		m_pszCvarName = strdup( cvarname );
	}
	else
		m_pszCvarName = nullptr;

	if( m_pszCvarName )
	{
		m_bStartValue = engine->pfnGetCvarFloat( m_pszCvarName ) > 0;
		SetSelected( m_bStartValue );
	}

	AddActionSignalTarget( this );
}

CCvarToggleCheckButton::~CCvarToggleCheckButton()
{
	free( m_pszCvarName );
}

void CCvarToggleCheckButton::SetSelected( bool state )
{
	BaseClass::SetSelected( state );
}

void CCvarToggleCheckButton::Paint()
{
	if( m_pszCvarName && *m_pszCvarName )
	{
		const bool bSelected = engine->pfnGetCvarFloat( m_pszCvarName ) > 0;

		if( bSelected != m_bStartValue )
		{
			m_bStartValue = bSelected;
			SetSelected( m_bStartValue );
		}
	}

	BaseClass::Paint();
}

void CCvarToggleCheckButton::OnButtonChecked()
{
	if( HasBeenModified() )
	{
		PostActionSignal( new KeyValues( "ControlModified" ) );
	}
}

void CCvarToggleCheckButton::ApplyChanges()
{
	m_bStartValue = IsSelected();
	engine->Cvar_SetValue( m_pszCvarName, m_bStartValue ? 1 : 0 );
}

void CCvarToggleCheckButton::Reset()
{
	m_bStartValue = engine->pfnGetCvarFloat( m_pszCvarName ) > 0;
	SetSelected( m_bStartValue );
}

bool CCvarToggleCheckButton::HasBeenModified()
{
	return m_bStartValue != IsSelected();
}
