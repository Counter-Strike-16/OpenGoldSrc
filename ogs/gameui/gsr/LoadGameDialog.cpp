#include <FileSystem.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/ListPanel.h>

#include "EngineInterface.h"
#include "LoadGameDialog.h"
#include "SaveGameDialog.h"
#include "Taskbar.h"

CLoadGameDialog::CLoadGameDialog( vgui2::Panel* parent )
	: BaseClass( parent, "LoadGameDialog", true )
{
	SetBounds( 0, 0, 512, 384 );
	SetMinimumSize( 256, 300 );
	SetSizeable( true );
	MakePopup();

	g_pTaskbar->AddTask( GetVPanel() );

	SetTitle( "#GameUI_LoadGame", true );

	auto pHelp = new vgui2::Label( this, "HelpText", "#GameUI_LoadGameHelp" );

	auto pLoadButton = new vgui2::Button( this, "Load", "#GameUI_Load" );
	pLoadButton->SetCommand( "Load" );

	auto pCancelButton = new vgui2::Button( this, "Cancel", "#GameUI_Cancel" );
	pCancelButton->SetCommand( "Close" );
	
	CreateSavedGamesList();
	ScanSavedGames();

	LoadControlSettings( "Resource\\LoadGameDialog.res" );
}

void CLoadGameDialog::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}

void CLoadGameDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "Load" ) )
	{
		auto pItem = m_pGameList->GetItem( m_pGameList->GetSelectedItem( 0 ) );

		if( pItem )
		{
			auto pszName = pItem->GetString( "ShortName" );

			if( pszName && *pszName )
			{
				char buf[ MAX_PATH ];
				snprintf( buf, ARRAYSIZE( buf ), "load %s\n", pszName );
				engine->pfnClientCmd( buf );
				OnClose();
			}
		}
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void CLoadGameDialog::CreateSavedGamesList()
{
	m_pGameList = new vgui2::ListPanel( this, "listpanel_loadgame" );

	m_pGameList->AddColumnHeader( 0, "Game", "#GameUI_SavedGame", 175 );
	m_pGameList->AddColumnHeader( 1, "Type", "#GameUI_Type", 100 );
	m_pGameList->AddColumnHeader( 2, "Elapsed Time", "#GameUI_ElapsedTime", 75 );
	m_pGameList->AddColumnHeader( 3, "Time", "#GameUI_TimeStamp", 175 );

	m_pGameList->SetSortFunc( 3, &::TimeStampSortFunc );
	m_pGameList->SetSortColumn( 3 );
}

//-----------------------------------------------------------------------------
// Purpose: Parses the save game info out of the .sav file header
//-----------------------------------------------------------------------------
bool CLoadGameDialog::ParseSaveData( const char* pszFileName, const char* pszShortName, KeyValues* kv )
{
	if( !pszShortName || !pszFileName || !kv )
		return 0;

	kv->SetString( "ShortName", pszShortName );
	kv->SetString( "FileName", pszFileName );

	auto hFile = vgui2::filesystem()->Open( pszFileName, "rb", "GAMECONFIG" );

	if( FILESYSTEM_INVALID_HANDLE != hFile )
	{
		char szMapName[ SAVEGAME_MAPNAME_LEN ];
		char szComment[ SAVEGAME_COMMENT_LEN ];

		const int readok = SaveReadNameAndComment( hFile, szMapName, szComment );

		vgui2::filesystem()->Close( hFile );

		if( readok )
		{
			kv->SetString( "Map", szMapName );
			kv->SetInt( "Quick", strstr( pszFileName, "quick" ) ? 1 : 0 );
			kv->SetInt( "Autosave", strstr( pszFileName, "autosave" ) ? 1 : 0 );

			const auto uiCommentLength = strlen( szComment );

			char szElapsedTime[ SAVEGAME_ELAPSED_LEN ] = "??";

			if( uiCommentLength > 5 )
			{
				strncpy( szElapsedTime, &szComment[ uiCommentLength - 5 ], 5u );
				szElapsedTime[ 5 ] = '\0';
				szComment[ uiCommentLength - 5 ] = '\0';
				for( int i = uiCommentLength - 6; i > 0 && szComment[ i ] == ' '; --i )
				{
					szComment[ i ] = '\0';
				}
			}

			const char* pszType = nullptr;

			if( strstr( pszFileName, "quick" ) )
				pszType = "#GameUI_QuickSave";
			else if( strstr( pszFileName, "autosave" ) )
				pszType = "#GameUI_AutoSave";

			kv->SetString( "Type", pszType );
			kv->SetString( "Game", szComment );
			kv->SetString( "Elapsed Time", szElapsedTime );

			auto timestamp = vgui2::filesystem()->GetFileTime( pszFileName );

			char szFileTime[ 32 ];
			vgui2::filesystem()->FileTimeToString( szFileTime, ARRAYSIZE( szFileTime ), timestamp );

			auto pszNewLine = strchr( szFileTime, '\n' );
			if( pszNewLine )
				*pszNewLine = '\0';

			kv->SetString( "Time", szFileTime );
			kv->SetInt( "timestamp", timestamp );

			return true;
		}
	}

	return false;
}

void CLoadGameDialog::ScanSavedGames()
{
	// populate list box with all saved games on record:
	char	szDirectory[ _MAX_PATH ] = "save/*.sav";

	// iterate the saved files
	FileFindHandle_t handle;
	const char *pFileName = vgui2::filesystem()->FindFirst( szDirectory, &handle );
	while( pFileName )
	{
		if( !Q_strnicmp( pFileName, "HLSave", strlen( "HLSave" ) ) )
		{
			pFileName = vgui2::filesystem()->FindNext( handle );
			continue;
		}

		char szFileName[ _MAX_PATH ];
		Q_snprintf( szFileName, sizeof( szFileName ), "save/%s", pFileName );

		auto pKV = new KeyValues( "SavedGame" );

		if( ParseSaveData( szFileName, pFileName, pKV ) )
		{
			m_pGameList->AddItem( pKV, 0, false, false );
		}

		pKV->deleteThis();

		pFileName = vgui2::filesystem()->FindNext( handle );
	}

	vgui2::filesystem()->FindClose( handle );

	// sort the save list
	m_pGameList->SortList();

	m_pGameList->SetSingleSelectedItem( m_pGameList->GetItemIDFromRow( 0 ) );
}
