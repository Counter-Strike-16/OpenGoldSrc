#include <KeyValues.h>

#include "CvarNegateCheckButton.h"
#include "EngineInterface.h"

CCvarNegateCheckButton::CCvarNegateCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* cvarname )
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
		m_bStartState = engine->pfnGetCvarFloat( m_pszCvarName ) < 0;
		SetSelected( m_bStartState );
	}

	AddActionSignalTarget( this );
}

CCvarNegateCheckButton::~CCvarNegateCheckButton()
{
	free( m_pszCvarName );
}

void CCvarNegateCheckButton::SetSelected( bool state )
{
	BaseClass::SetSelected( state );
}

void CCvarNegateCheckButton::Paint()
{
	if( m_pszCvarName )
	{
		const bool bSelected = engine->pfnGetCvarFloat( m_pszCvarName ) < 0;

		if( bSelected != m_bStartState )
		{
			m_bStartState = bSelected;
			SetSelected( m_bStartState );
		}
	}

	BaseClass::Paint();
}

void CCvarNegateCheckButton::OnButtonChecked()
{
	if( HasBeenModified() )
	{
		PostActionSignal( new KeyValues( "ControlModified" ) );
	}
}

void CCvarNegateCheckButton::ApplyChanges()
{
	if( m_pszCvarName && *m_pszCvarName )
	{
		auto flValue = fabs( engine->pfnGetCvarFloat( m_pszCvarName ) );

		float flMin;
		float flMax;

		if( flValue < 0.00001 )
		{
			flMin = -0.022;
			flMax = 0.022;
		}
		else
		{
			flMin = -flValue;
			flMax = flValue;
		}

		m_bStartState = IsSelected();

		engine->Cvar_SetValue( m_pszCvarName, m_bStartState ? flMin : flMax );
	}
}

void CCvarNegateCheckButton::Reset()
{
	m_bStartState = engine->pfnGetCvarFloat( m_pszCvarName ) < 0;
	SetSelected( m_bStartState );
}

bool CCvarNegateCheckButton::HasBeenModified()
{
	return m_bStartState != IsSelected();
}
