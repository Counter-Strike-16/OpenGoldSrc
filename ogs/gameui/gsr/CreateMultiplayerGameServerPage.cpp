//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//
#include "CreateMultiplayerGameServerPage.h"

using namespace vgui2;

#include <KeyValues.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/RadioButton.h>
#include <vgui_controls/CheckButton.h>
#include "FileSystem.h"
#include "EngineInterface.h"
#include "CvarToggleCheckButton.h"
#include "Random.h"

#include "ModInfo.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

#define RANDOM_MAP "#GameUI_RandomMap"
#define RANDOM_MAP_NAME "< Random map >"

bool CaselessStringLessThan( const CUtlSymbol& lhs, const CUtlSymbol& rhs )
{
	if( !lhs.IsValid() )
		return false;

	if( !rhs.IsValid() )
		return true;

	return stricmp( lhs.String(), rhs.String() ) < 0;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::CCreateMultiplayerGameServerPage( vgui2::Panel *parent, const char *name )
	: PropertyPage( parent, name )
	, m_MapNames( &CaselessStringLessThan )
{
	// we can use this if we decide we want to put "listen server" at the end of the game name
	m_pMapList = new ComboBox( this, "MapList", 12, false );

	m_pBotQuotaCombo = new vgui2::TextEntry( this, "BotQuotaCombo" );

	m_pEnableTutorCheck = new CCvarToggleCheckButton( this, "CheckButtonTutor", "#CStrike_Tutor_Enabled", "tutor_enable" );

	m_pEnableBotsCheck = new CheckButton( this, "EnableBotsCheck", "enable bots" );

	LoadControlSettings( "Resource/CreateMultiplayerGameServerPage.res" );

	LoadMapList();

	if( stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
		m_pEnableTutorCheck->SetVisible( false );

	m_szMapName[ 0 ] = '\0';
	m_szHostName[ 0 ] = '\0';
	m_szPassword[ 0 ] = '\0';

	// initialize hostname
	SetControlString( "ServerNameEdit", ModInfo().GetGameDescription() );//szHostName);

	// initialize password
	SetControlString("PasswordEdit", engine->pfnGetCvarString("sv_password"));

	char szBuffer[ 4 ];

	snprintf( szBuffer, ARRAYSIZE( szBuffer ) - 1, "%d", m_iMaxPlayers );
	szBuffer[ ARRAYSIZE( szBuffer ) - 1 ] = '\0';

	SetControlString( "MaxPlayersEdit", szBuffer );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::~CCreateMultiplayerGameServerPage()
{
}

//-----------------------------------------------------------------------------
// Purpose: called to get the info from the dialog
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::OnApplyChanges()
{
	strncpy( m_szHostName, GetControlString( "ServerNameEdit", "Half-Life" ), ARRAYSIZE( m_szHostName ) );
	strncpy( m_szPassword, GetControlString( "PasswordEdit", "" ), ARRAYSIZE( m_szPassword ) );

	m_iMaxPlayers = atoi( GetControlString( "MaxPlayersEdit", "8" ) );

	auto pMapData = m_pMapList->GetActiveItemUserData();

	strncpy( m_szMapName, pMapData->GetString( "mapname" ), ARRAYSIZE( m_szMapName ) );

	if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
		m_pEnableTutorCheck->ApplyChanges();
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::LoadMaps( const char *pszPathID )
{
	//TODO: should be FILESYSTEM_INVALID_FIND_HANDLE - Solokiller
	FileFindHandle_t findHandle = NULL;

	auto pszFilter = ModInfo().GetMPFilter();

	if( pszFilter && !( *pszFilter ) )
		pszFilter = nullptr;

	const char *pszFilename = vgui2::filesystem()->FindFirst( "maps/*.bsp", &findHandle, pszPathID );
	if( pszFilename )
	{
		do
		{
			char mapname[ 256 ];

			Q_snprintf( mapname, sizeof( mapname ), "maps/%s", pszFilename );

			// remove the text 'maps/' and '.bsp' from the file name to get the map name

			const char *str = Q_strstr( pszFilename, "maps" );
			if( str )
			{
				Q_strncpy( mapname, str + 5, sizeof( mapname ) - 1 );	// maps + \\ = 5
			}
			else
			{
				Q_strncpy( mapname, pszFilename, sizeof( mapname ) - 1 );
			}
			char *ext = Q_strstr( mapname, ".bsp" );
			if( ext )
			{
				*ext = 0;
			}

			//!! hack: strip out single player HL maps
			// this needs to be specified in a seperate file
			if( !stricmp( ModInfo().GetGameDescription(), "Half-Life" ) &&
				( mapname[ 0 ] == 'c' || mapname[ 0 ] == 't' ) &&
				mapname[ 2 ] == 'a' && mapname[ 1 ] >= '0' && mapname[ 1 ] <= '5' )
			{
				continue;
			}

			//!! hack: same for OpFor - Solokiller
			if( !stricmp( ModInfo().GetGameDescription(), "Opposing Force" ) &&
				mapname[ 0 ] == 'o' && mapname[ 1 ] == 'f' )
			{
				continue;
			}

			// strip out maps that shouldn't be displayed
			if( pszFilter )
			{
				if( strstr( mapname, pszFilename ) )
					continue;
			}

			// add to the map list
			m_MapNames.InsertIfNotFound( CUtlSymbol( mapname ) );

			// get the next file
		}
		while( ( pszFilename = vgui2::filesystem()->FindNext( findHandle ) ) != nullptr );
	}
	vgui2::filesystem()->FindClose( findHandle );
}



//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::LoadMapList()
{
	// clear the current list (if any)
	m_pMapList->DeleteAllItems();
	m_MapNames.RemoveAll();

	// add special "name" to represent loading a randomly selected map
	m_pMapList->AddItem( RANDOM_MAP, new KeyValues( "data", "mapname", RANDOM_MAP_NAME ) );

	// iterate the filesystem getting the list of all the files
	// UNDONE: steam wants this done in a special way, need to support that
	const char *pathID = "GAME";
	if( !stricmp( ModInfo().GetGameDescription(), "Half-Life" ) )
	{
		pathID = NULL; // hl is the base dir
	}

	// Load the GameDir maps
	LoadMaps( pathID );

	// If we're not the Valve directory and we're using a "fallback_dir" in gameinfo.txt then include those maps...
	// (pathID is NULL if we're "Half-Life")
	const char *pszFallback = ModInfo().GetFallbackDir();
	if( pathID && pszFallback[ 0 ] )
	{
		LoadMaps( "GAME_FALLBACK" );
	}

	//Load maps from the downloads directory as well (SteamPipe addition)
	LoadMaps( "GAMEDOWNLOAD" );

	for( auto i = m_MapNames.FirstInorder(); i != m_MapNames.InvalidIndex(); i = m_MapNames.NextInorder( i ) )
	{
		const auto& mapname = m_MapNames[ i ];

		m_pMapList->AddItem( mapname.String(), new KeyValues( "data", "mapname", mapname.String() ) );
	}

	m_MapNames.RemoveAll();

	// set the first item to be selected
	m_pMapList->ActivateItem( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CCreateMultiplayerGameServerPage::IsRandomMapSelected()
{
	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString( "mapname" );
	if( !stricmp( mapname, RANDOM_MAP_NAME ) )
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CCreateMultiplayerGameServerPage::GetMapName()
{
	int count = m_pMapList->GetItemCount();

	// if there is only one entry it's the special "select random map" entry
	if( count <= 1 )
		return NULL;

	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString( "mapname" );
	if( !strcmp( mapname, RANDOM_MAP_NAME ) )
	{
		int which = RandomLong( 1, count - 1 );
		mapname = m_pMapList->GetItemUserData( which )->GetString( "mapname" );
	}

	return mapname;
}

//-----------------------------------------------------------------------------
// Purpose: Sets currently selected map in the map combobox
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::SetMap( const char *mapName )
{
	for( int i = 0; i < m_pMapList->GetItemCount(); i++ )
	{
		if( !m_pMapList->IsItemIDValid( i ) )
			continue;

		if( !stricmp( m_pMapList->GetItemUserData( i )->GetString( "mapname" ), mapName ) )
		{
			m_pMapList->ActivateItem( i );
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::OnCheckButtonChecked()
{
	SetControlEnabled( "SkillLevelExpert", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "SkillLevelHard", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "SkillLevelNormal", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "SkillLevelEasy", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "BotQuotaCombo", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "BotQuotaLabel", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "BotDifficultyLabel", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "BotLabel1", m_pEnableBotsCheck->IsSelected() );
	SetControlEnabled( "BotLabel2", m_pEnableBotsCheck->IsSelected() );
}

int CCreateMultiplayerGameServerPage::GetBotQuota()
{
	char buf[ 32 ];

	m_pBotQuotaCombo->GetText( buf, ARRAYSIZE( buf ) );
	return atoi( buf );
}

void CCreateMultiplayerGameServerPage::SetBotQuota( int quota )
{
	char buf[ 32 ];

	sprintf( buf, "%d", quota );
	m_pBotQuotaCombo->SetText( buf );
}

bool CCreateMultiplayerGameServerPage::GetBotsEnabled()
{
	return m_pEnableBotsCheck->IsSelected();
}

void CCreateMultiplayerGameServerPage::SetBotsEnabled( bool enabled )
{
	m_pEnableBotsCheck->SetSelected( enabled );
}
