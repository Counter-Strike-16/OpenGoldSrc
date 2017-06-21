#include "EngineInterface.h"
#include "ModInfo.h"
#include "NewGameDialog.h"
#include "Taskbar.h"

CNewGameDialog::CNewGameDialog( vgui2::Panel* parent )
	: BaseClass( parent, "NewGameDialog" )
{
	SetBounds( 0, 0, 372, 160 );
	SetSizeable( false );
	MakePopup();

	g_pTaskbar->AddTask( GetVPanel() );

	SetTitle( "#GameUI_NewGame", true );

	auto pLabel = new vgui2::Label( this, "HelpText", "#GameUI_NewGameHelpText" );

	m_pTraining = new vgui2::RadioButton( this, "Training", "#GameUI_TrainingRoom" );
	m_pTraining->SetSelected( true );

	m_nPlayMode = 0;

	m_pEasy = new vgui2::RadioButton( this, "Easy", "#GameUI_Easy" );
	m_pMedium = new vgui2::RadioButton( this, "Medium", "#GameUI_Medium" );
	m_pHard = new vgui2::RadioButton( this, "Hard", "#GameUI_Hard" );

	auto pPlayButton = new vgui2::Button( this, "Play", "#GameUI_Play" );
	pPlayButton->SetCommand( "Play" );

	auto pCancelButton = new vgui2::Button( this, "Cancel", "#GameUI_Cancel" );
	pCancelButton->SetCommand( "Close" );

	LoadControlSettings( "Resource\\NewGameDialog.res" );
}

void CNewGameDialog::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}

void CNewGameDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "Play" ) )
	{
		char mapcommand[ 512 ];

		mapcommand[ 0 ] = '\0';

		m_nPlayMode = 0;

		if( m_pEasy->IsSelected() )
		{
			m_nPlayMode = 1;
		}
		else if( m_pMedium->IsSelected() )
		{
			m_nPlayMode = 2;
		}
		else if( m_pHard->IsSelected() )
		{
			m_nPlayMode = 3;
		}

		if( m_nPlayMode )
		{
			snprintf(
				mapcommand, ARRAYSIZE( mapcommand ),
				"disconnect\nmaxplayers 1\ndeathmatch 0\nskill %i\nmap %s\n",
				m_nPlayMode,
				ModInfo().GetStartMap()
			);
		}
		else
		{
			//No difficulty selected, start training map
			snprintf(
				mapcommand, ARRAYSIZE( mapcommand ),
				"disconnect\nmaxplayers 1\ndeathmatch 0\nmap %s\n",
				ModInfo().GetTrainMap()
			);
		}

		engine->pfnClientCmd( mapcommand );

		//New career started
		if( !stricmp( ModInfo().GetGameDescription(), "Condition-Zero" ) )
			engine->pfnClientCmd( "cl_newgame 1\n" );

		OnClose();
	}
	else
		BaseClass::OnCommand( command );
}
