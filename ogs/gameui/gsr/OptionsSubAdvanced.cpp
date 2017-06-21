#include <vgui/ISurface.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Controls.h>

#include "ContentControlDialog.h"
#include "OptionsSubAdvanced.h"

COptionsSubAdvanced::COptionsSubAdvanced( vgui2::Panel* parent )
	: BaseClass( parent, nullptr )
{
	m_pContentCheckButton = new vgui2::CheckButton( this, "ContentlockButton", "#GameUI_ContentLock" );
	m_pContentCheckButton->SetCommand( "ContentControl" );

	LoadControlSettings( "Resource\\OptionsSubAdvanced.res" );

	m_pContentControlDialog = new CContentControlDialog( parent );
	m_pContentControlDialog->AddActionSignalTarget( this );

	int x, y, ww, wt;
	vgui2::surface()->GetWorkspaceBounds( x, y, ww, wt );

	int wide, tall;
	GetSize( wide, tall );

	SetPos( x + ( ww - wide ) / 2, y + ( wt - tall ) / 2 );

	m_pContentCheckButton->SetSelected( m_pContentControlDialog->IsPasswordEnabledInDialog() );
}

void COptionsSubAdvanced::OnApplyChanges()
{
	m_pContentControlDialog->ApplyPassword();
}

void COptionsSubAdvanced::OnResetData()
{
	m_pContentControlDialog->ResetPassword();
	m_pContentCheckButton->SetSelected( m_pContentControlDialog->IsPasswordEnabledInDialog() );
}

void COptionsSubAdvanced::OnContentControlClose()
{
	m_pContentCheckButton->SetSelected( m_pContentControlDialog->IsPasswordEnabledInDialog() );

	//TODO: this check makes no sense. Should be checking the old value against the new - Solokiller
	if( m_pContentControlDialog->IsPasswordEnabledInDialog() != m_pContentControlDialog->IsPasswordEnabledInDialog() )
	{
		PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
	}
}

void COptionsSubAdvanced::OnCommand( const char* command )
{
	if( !stricmp( command, "ContentControl" ) )
	{
		if( !m_pContentCheckButton->IsSelected() && !m_pContentControlDialog->IsPasswordEnabledInDialog() )
			m_pContentControlDialog->ResetPassword();
		else
			OnOpenContentControlDialog();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void COptionsSubAdvanced::OnOpenContentControlDialog()
{
	m_pContentControlDialog->Activate();
}
