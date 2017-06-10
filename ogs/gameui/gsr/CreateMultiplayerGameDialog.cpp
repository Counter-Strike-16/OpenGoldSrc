//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "CreateMultiplayerGameDialog.h"
#include "CreateMultiplayerGameServerPage.h"
#include "CreateMultiplayerGameGameplayPage.h"
#include "CreateMultiplayerGameBotPage.h"

#include "EngineInterface.h"
#include "ModInfo.h"
#include "GameUI_Interface.h"
#include "Taskbar.h"

#include <stdio.h>

using namespace vgui2;

#include <vgui/ILocalize.h>

#include "FileSystem.h"
#include <KeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameDialog::CCreateMultiplayerGameDialog( vgui2::Panel *parent ) : PropertyDialog( parent, "CreateMultiplayerGameDialog" )
{
	SetSize( 348, 460 );
	m_bBotsEnabled = false;

	SetTitle( "#GameUI_CreateServer", true );
	SetOKButtonText( "#GameUI_Start" );

	if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
	{
		m_bBotsEnabled = true;
	}

	g_pTaskbar->AddTask( GetVPanel() );

	SetTitle( "#GameUI_CreateServer", true );

	SetOKButtonText( "#GameUI_Start" );

	m_pServerPage = new CCreateMultiplayerGameServerPage( this, "ServerPage" );
	m_pGameplayPage = new CCreateMultiplayerGameGameplayPage( this, "GameplayPage" );
	m_pBotPage = NULL;

	AddPage( m_pServerPage, "#GameUI_Server" );
	AddPage( m_pGameplayPage, "#GameUI_Game" );

	// create KeyValues object to load/save config options
	m_pBotSavedData = new KeyValues( "CSBotConfig" );

	// load the config data
	if( !m_pBotSavedData->LoadFromFile( vgui2::filesystem(), "CSBotConfig.vdf", "CONFIG" ) )
	{
		m_pBotSavedData->SetString( "bot_difficulty", "0" );
		m_pBotSavedData->SetString( "bot_join_after_player", "1" );
		m_pBotSavedData->SetString( "bot_allow_rogues", "1" );
		m_pBotSavedData->SetString( "bot_allow_pistols", "1" );
		m_pBotSavedData->SetString( "bot_allow_shotguns", "1" );
		m_pBotSavedData->SetString( "bot_allow_sub_machine_guns", "1" );
		m_pBotSavedData->SetString( "bot_allow_machine_guns", "1" );
		m_pBotSavedData->SetString( "bot_allow_rifles", "1" );
		m_pBotSavedData->SetString( "bot_allow_snipers", "1" );
		m_pBotSavedData->SetString( "bot_allow_grenades", "1" );
		m_pBotSavedData->SetString( "bot_allow_shield", "1" );
		m_pBotSavedData->SetString( "bot_join_team", "any" );
		m_pBotSavedData->SetString( "bot_quota", "9" );
		m_pBotSavedData->SetString( "bot_defer_to_human", "0" );
		m_pBotSavedData->SetString( "bot_chatter", "normal" );
	}

	if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
	{
		// add a page of advanced bot controls
		// NOTE: These controls will use the bot keys to initialize their values
		m_pBotPage = new CCreateMultiplayerGameBotPage( this, "BotPage", m_pBotSavedData );
		AddPage( m_pBotPage, "#GameUI_CPUPlayerOptions" );
	}

	int iBotDifficulty;

	if( engine->pfnGetCvarPointer( "bot_difficulty" ) )
	{
		iBotDifficulty = static_cast<int>( engine->pfnGetCvarFloat( "bot_difficulty" ) );
	}
	else
	{
		iBotDifficulty = m_pBotSavedData->GetInt( "bot_difficulty" );
	}

	if( engine->pfnGetCvarPointer( "bot_quota" ) )
	{
		m_pServerPage->SetBotQuota( static_cast<int>( engine->pfnGetCvarFloat( "bot_quota" ) ) );
	}
	else
	{
		m_pServerPage->SetBotQuota( atoi( m_pBotSavedData->GetString( "bot_quota" ) ) );
	}

	if( m_bBotsEnabled )
	{
		m_pServerPage->SetBotsEnabled( true );
		iBotDifficulty = clamp( iBotDifficulty, 0, 3 );
	}
	else
	{
		m_pServerPage->SetBotsEnabled( false );
	}

	auto pszMapName = m_pBotSavedData->GetString( "map" );

	if( *pszMapName )
		m_pServerPage->SetMap( pszMapName );

	static const char* const buttonName[ 4 ] = 
	{
		"SkillLevelEasy",
		"SkillLevelNormal",
		"SkillLevelHard",
		"SkillLevelExpert"
	};

	auto pSkillPanel = m_pServerPage->FindChildByName( buttonName[ iBotDifficulty ] );

	//TODO: this assumes that the skill panel is a button or subclass thereof - Solokiller
	if( pSkillPanel )
		static_cast<vgui2::Button*>( pSkillPanel )->SetSelected( true );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameDialog::~CCreateMultiplayerGameDialog()
{
	if( m_pBotSavedData )
	{
		m_pBotSavedData->deleteThis();
		m_pBotSavedData = nullptr;
	}
}

void CCreateMultiplayerGameDialog::SetTitle( const char* title, bool surfaceTitle )
{
	BaseClass::SetTitle( title, surfaceTitle );

	if( g_pTaskbar )
	{
		wchar_t w_szTitle[ 1024 ];
		auto pszString = vgui2::localize()->Find( title );

		if( !pszString )
		{
			vgui2::localize()->ConvertANSIToUnicode( title, w_szTitle, sizeof( w_szTitle ) );
			pszString = w_szTitle;
		}

		g_pTaskbar->SetTitle( GetVPanel(), w_szTitle );
	}
}

//-----------------------------------------------------------------------------
// Purpose: runs the server when the OK button is pressed
//-----------------------------------------------------------------------------
bool CCreateMultiplayerGameDialog::OnOK( bool applyOnly )
{
	BaseClass::OnOK( applyOnly );

	// get these values from m_pServerPage and store them temporarily
	char szMapName[ 64 ], szHostName[ 64 ], szPassword[ 64 ];
	Q_strncpy( szMapName, m_pServerPage->GetMapName(), sizeof( szMapName ) );
	Q_strncpy( szHostName, m_pGameplayPage->GetHostName(), sizeof( szHostName ) );
	Q_strncpy( szPassword, m_pGameplayPage->GetPassword(), sizeof( szPassword ) );

	// save the config data
	if( m_bBotsEnabled )
	{
		int iBotSkillLevel = -1;

		if( m_pServerPage->GetBotsEnabled() )
		{
			if( m_pServerPage->GetControlInt( "SkillLevelEasy", 0 ) )
				iBotSkillLevel = 0;

			if( m_pServerPage->GetControlInt( "SkillLevelNormal", 0 ) )
				iBotSkillLevel = 1;

			if( m_pServerPage->GetControlInt( "SkillLevelHard", 0 ) )
				iBotSkillLevel = 2;

			if( m_pServerPage->GetControlInt( "SkillLevelExpert", 0 ) )
				iBotSkillLevel = 3;
		}

		m_pBotSavedData->SetInt( "bot_difficulty", iBotSkillLevel );

		if( m_pBotPage )
			m_pBotPage->UpdateKeys( m_pBotSavedData );

		m_pBotSavedData->SetInt( "bot_difficulty", m_pServerPage->GetBotQuota() );

		if( m_pBotSavedData->GetInt( "bot_difficulty" ) < 0 )
			m_pBotSavedData->SetInt( "bot_quota", 0 );

		if( m_pServerPage->IsRandomMapSelected() )
		{
			// it's set to random map, just save an
			m_pBotSavedData->SetString( "map", "" );
		}
		else
		{
			m_pBotSavedData->SetString( "map", szMapName );
		}

		// save config to a file
		m_pBotSavedData->SaveToFile( vgui2::filesystem(), "CSBotConfig.vdf", "CONFIG" );

		char szMapCommand[ 1024 ];

		// create the command to execute
		Q_snprintf( szMapCommand, sizeof( szMapCommand ), "disconnect\nsv_lan 1\nsetmaster enable\nmaxplayers %i\nsv_password \"%s\"\nhostname \"%s\"\ncd fadeout\nmap %s\n",
					m_pGameplayPage->GetMaxPlayers(),
					szPassword,
					szHostName,
					szMapName
		);

		// exec
		engine->pfnClientCmd( szMapCommand );
		engine->pfnClientCmd( "wait\nwait\n" );

		for( auto i = m_pBotSavedData->GetFirstSubKey(); i; i = i->GetNextKey() )
		{
			if( !stricmp( i->GetName(), "map" ) )
			{
				static char buffer[ 128 ];

				if( i->GetDataType() == KeyValues::TYPE_STRING )
				{
					sprintf( buffer, "%s \"%s\"\n", i->GetName(), i->GetString() );
				}
				else
				{
					sprintf( buffer, "%s %d\n", i->GetName(), i->GetInt() );
				}

				engine->pfnClientCmd( buffer );
			}
		}

		engine->pfnClientCmd( "mp_autoteambalance 0\n" );
		engine->pfnClientCmd( "mp_limitteams 0\n" );
	}
	else
	{
		char szMapCommand[ 1024 ];

		sprintf(
			szMapCommand,
			"disconnect\nsv_lan 1\nsetmaster enable\nmaxplayers %i\nsv_password \"%s\"\nhostname \"%s\"\ncd fadeout\nmap %s\n",
			m_pGameplayPage->GetMaxPlayers(),
			szPassword,
			szHostName,
			szMapName );

		engine->pfnClientCmd( szMapCommand );
		engine->pfnClientCmd( "wait\nwait\n" );
	}

	return true;
}

void CCreateMultiplayerGameDialog::OnClose()
{
	MarkForDeletion();
	BaseClass::OnClose();
}
