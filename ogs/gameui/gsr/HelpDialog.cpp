#include <KeyValues.h>
#include <vgui/ISystem.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Label.h>

#include "HelpDialog.h"
#include "Taskbar.h"

CHelpDialog::CHelpDialog()
	: BaseClass( nullptr, "HelpDialog", true )
{
	g_pTaskbar->AddTask( GetVPanel() );

	m_pInfoText = new vgui2::Label( this, "InfoText", "#GameUI_InfoText" );
	m_pClose = new vgui2::Button( this, "CloseButton", "#GameUI_Close" );
	m_pNeverShowAgain = new vgui2::CheckButton( this, "NeverShowButton", "#GameUI_NeverShowButton" );

	SetTitle( "#GameUI_HalfLifeDesktopHelp", true );
	LoadControlSettings( "Resource/DialogHelpIngame.res" );
}

void CHelpDialog::OnClose()
{
	BaseClass::OnClose();

	if( m_pNeverShowAgain->IsSelected() )
	{
		vgui2::system()->SetRegistryInteger( "HKEY_CURRENT_USER\\Software\\Valve\\Tracker\\NeverShowHelp", 1 );
	}

	MarkForDeletion();
}

void CHelpDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "Close" ) )
	{
		PostMessage( this, new KeyValues( "Close" ) );
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}
