#include "CommandCheckButton.h"
#include "EngineInterface.h"

CCommandCheckButton::CCommandCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* downcmd, const char* upcmd )
	: BaseClass( parent, panelName, text )
{
	m_pszDown = downcmd ? strdup( downcmd ) : nullptr;
	m_pszUp = upcmd ? strdup( upcmd ) : nullptr;
}

CCommandCheckButton::~CCommandCheckButton()
{
	free( m_pszDown );
	free( m_pszUp );
}

void CCommandCheckButton::SetSelected( bool state )
{
	BaseClass::SetSelected( state );

	auto pszCmd = IsSelected() ? m_pszDown : m_pszUp;

	if( pszCmd )
	{
		engine->pfnClientCmd( pszCmd );
		engine->pfnClientCmd( "\n" );
	}
}
