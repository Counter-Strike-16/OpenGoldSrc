#include <cstdint>

#include <FileSystem.h>
#include <KeyValues.h>

#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "savegame_version.h"
#include "SaveGameDialog.h"
#include "Taskbar.h"
#include "BaseUI/IBaseUI.h"

int TimeStampSortFunc( vgui2::ListPanel* pPanel, const vgui2::ListPanelItem& p1, const vgui2::ListPanelItem& p2 )
{
	if( p1.kv->GetInt( "timestamp", 0 ) < p2.kv->GetInt( "timestamp", 0 ) )
		return 1;
	else
		return -1;
}

#define MAKEID(d,c,b,a)	( ((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | ((int)(d)) )

int SaveReadNameAndComment( FileHandle_t f, char *name, char *comment )
{
	int i, tag, size, tokenSize, tokenCount;
	char *pSaveData, *pFieldName, **pTokenList;

	vgui2::filesystem()->Read( &tag, sizeof( int ), f );
	if( tag != MAKEID( 'J', 'S', 'A', 'V' ) )
	{
		return 0;
	}

	vgui2::filesystem()->Read( &tag, sizeof( int ), f );
	if( tag != SAVEGAME_VERSION )				// Enforce version for now
	{
		return 0;
	}

	name[ 0 ] = '\0';
	comment[ 0 ] = '\0';
	vgui2::filesystem()->Read( &size, sizeof( int ), f );

	vgui2::filesystem()->Read( &tokenCount, sizeof( int ), f );	// These two ints are the token list
	vgui2::filesystem()->Read( &tokenSize, sizeof( int ), f );
	size += tokenSize;

	// Sanity Check.
	if( tokenCount < 0 || tokenCount > 1024 * 1024 * 32 )
	{
		return 0;
	}

	if( tokenSize < 0 || tokenSize > 1024 * 1024 * 32 )
	{
		return 0;
	}

	pSaveData = ( char * )new char[ size ];
	vgui2::filesystem()->Read( pSaveData, size, f );

	int nNumberOfFields;

	char *pData;
	int nFieldSize;

	pData = pSaveData;

	// Allocate a table for the strings, and parse the table
	if( tokenSize > 0 )
	{
		pTokenList = new char *[ tokenCount ];

		// Make sure the token strings pointed to by the pToken hashtable.
		for( i = 0; i<tokenCount; i++ )
		{
			pTokenList[ i ] = *pData ? pData : NULL;	// Point to each string in the pToken table
			while( *pData++ );				// Find next token (after next null)
		}
	}
	else
		pTokenList = NULL;

	// short, short (size, index of field name)
	nFieldSize = *( short * ) pData;
	pData += sizeof( short );
	pFieldName = pTokenList[ *( short * ) pData ];

	if( stricmp( pFieldName, "GameHeader" ) )
	{
		delete[] pSaveData;
		return 0;
	};

	// int (fieldcount)
	pData += sizeof( short );
	nNumberOfFields = *( int* ) pData;
	pData += nFieldSize;

	// Each field is a short (size), short (index of name), binary string of "size" bytes (data)
	for( i = 0; i < nNumberOfFields; i++ )
	{
		// Data order is:
		// Size
		// szName
		// Actual Data

		nFieldSize = *( short * ) pData;
		pData += sizeof( short );

		pFieldName = pTokenList[ *( short * ) pData ];
		pData += sizeof( short );

		if( !stricmp( pFieldName, "comment" ) )
		{
			Q_strncpy( comment, pData, nFieldSize );
		}
		else if( !stricmp( pFieldName, "mapName" ) )
		{
			Q_strncpy( name, pData, nFieldSize );
		};

		// Move to Start of next field.
		pData += nFieldSize;
	};

	// Delete the string table we allocated
	delete[] pTokenList;
	delete[] pSaveData;

	if( strlen( name ) > 0 && strlen( comment ) > 0 )
		return 1;

	return 0;
}

CSaveGameDialog::CSaveGameDialog( vgui2::Panel* parent )
	: BaseClass( parent, "SaveGameDialog", true )
{
	SetMinimumSize( 300, 300 );
	SetBounds( 0, 0, 512, 384 );
	SetSizeable( true );
	MakePopup();

	g_pTaskbar->AddTask( GetVPanel() );

	SetTitle( "#GameUI_SaveGame", true );

	auto pHelp = new vgui2::Label( this, "HelpText", "#GameUI_SaveGameHelp" );

	auto pSaveButton = new vgui2::Button( this, "Save", "#GameUI_Save" );
	pSaveButton->SetCommand( "Save" );

	auto pCancelButton = new vgui2::Button( this, "Cancel", "#GameUI_Cancel" );
	pCancelButton->SetCommand( "Close" );

	CreateSavedGamesList();
	ScanSavedGames();

	LoadControlSettings( "Resource/SaveGameDialog.res" );
}

void CSaveGameDialog::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}

void CSaveGameDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "Save" ) )
	{
		auto pItem = m_pGameList->GetItem( m_pGameList->GetSelectedItem( 0 ) );

		if( pItem )
		{
			auto pszShortName = pItem->GetString( "ShortName" );

			if( pszShortName && *pszShortName )
			{
				char s[ MAX_PATH ];
				snprintf( s, ARRAYSIZE( s ), "save %s\n", pszShortName );
				engine->pfnClientCmd( s );

				if( baseuifuncs )
					baseuifuncs->HideGameUI();

				OnClose();
			}
		}
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void CSaveGameDialog::CreateSavedGamesList()
{
	m_pGameList = new vgui2::ListPanel( this, "listpanel_loadgame" );

	m_pGameList->AddColumnHeader( 0, "Game", "#GameUI_SavedGame", 175 );
	m_pGameList->AddColumnHeader( 1, "Type", "#GameUI_Type", 100 );
	m_pGameList->AddColumnHeader( 2, "Elapsed Time", "#GameUI_ElapsedTime", 75 );
	m_pGameList->AddColumnHeader( 3, "Time", "#GameUI_TimeStamp", 175 );

	m_pGameList->SetSortFunc( 3, &::TimeStampSortFunc );
	m_pGameList->SetSortColumn( 3 );
}

bool CSaveGameDialog::ParseSaveData( const char* pszFileName, const char* pszShortName, KeyValues* kv )
{
	if( !pszShortName || !pszFileName || !kv )
		return 0;

	kv->SetString( "ShortName", pszShortName );
	kv->SetString( "FileName", pszFileName );

	auto hFile = vgui2::filesystem()->Open( pszFileName, "rb" );

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

void CSaveGameDialog::ScanSavedGames()
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

	auto pKV = new KeyValues( "SavedGame" );

	pKV->SetString( "Game", "#GameUI_SaveGame_NewSavedGame" );
	pKV->SetString( "Elapsed Time", "#GameUI_SaveGame_New" );
	pKV->SetString( "Time", "#GameUI_SaveGame_Current" );
	pKV->SetInt( "timestamp", INT_MAX );
	pKV->SetString( "ShortName", FindSaveSlot() );

	m_pGameList->AddItem( pKV, 0, false, false );

	// sort the save list
	m_pGameList->SortList();

	m_pGameList->SetSingleSelectedItem( m_pGameList->GetItemIDFromRow( 0 ) );
}

const char* CSaveGameDialog::FindSaveSlot()
{
	static char szFileName[ 512 ];

	for( int i = 0; i < 1000; ++i )
	{
		snprintf( szFileName, ARRAYSIZE( szFileName ), "save/Half-Life-%03i.sav", i );

		auto hFile = vgui2::filesystem()->Open( szFileName, "rb", "GAMECONFIG" );

		if( FILESYSTEM_INVALID_HANDLE == hFile )
		{
			return szFileName;
		}

		vgui2::filesystem()->Close( hFile );
	}

	return "error.sav";
}
