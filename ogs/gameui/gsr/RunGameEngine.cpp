#include "EngineInterface.h"
#include "RunGameEngine.h"

EXPOSE_SINGLE_INTERFACE( CRunGameEngine, IRunGameEngine, RUNGAMEENGINE_INTERFACE_VERSION );

CRunGameEngine::~CRunGameEngine()
{
}

bool CRunGameEngine::IsRunning()
{
	return true;
}

bool CRunGameEngine::AddTextCommand( const char *text )
{
	engine->pfnClientCmd( const_cast<char*>( text ) );
	return true;
}

CRunGameEngine::ERunResult CRunGameEngine::RunEngine( int iAppID, const char *gameDir, const char *commandLineParams )
{
	//Nothing
	return k_ERunResultNotInitialized;
}

bool CRunGameEngine::IsInGame()
{
	auto pszLevelName = engine->pfnGetLevelName();

	return pszLevelName && *pszLevelName;
}

bool CRunGameEngine::GetGameInfo( char *infoBuffer, int bufferSize )
{
	//Nothing
	return false;
}

void CRunGameEngine::SetTrackerUserID( int trackerID, const char *trackerName )
{
	char buf[ 32 ];
	snprintf( buf, ARRAYSIZE( buf ), "%d", trackerID );

	engine->PlayerInfo_SetValueForKey( "*fid", buf );
	engine->PlayerInfo_SetValueForKey( "*fname", trackerName );

	auto pszName = engine->pfnGetCvarString( "name" );

	if( pszName && trackerName && *trackerName )
	{
		//If the player's name is the default, change it to the tracker name.
		if( !strncmp( pszName, "Player", 7 ) )
			engine->Cvar_Set( "name", const_cast<char*>( trackerName ) );
	}
}

int CRunGameEngine::GetPlayerCount()
{
	return engine->GetMaxClients();
}

unsigned int CRunGameEngine::GetPlayerFriendsID( int playerIndex )
{
	//Nothing
	return 0;
}

const char *CRunGameEngine::GetPlayerName( int friendsID )
{
	auto index = engine->GetPlayerForTrackerID( friendsID );

	if( index )
	{
		hud_player_info_t playerInfo;

		engine->pfnGetPlayerInfo( index, &playerInfo );

		return playerInfo.name;
	}

	return nullptr;
}

const char *CRunGameEngine::GetPlayerFriendsName( int friendsID )
{
	//Nothing
	return nullptr;
}

unsigned int CRunGameEngine::GetEngineBuildNumber()
{
	//Nothing
	return 0;
}

const char *CRunGameEngine::GetProductVersionString()
{
	//Nothing
	return nullptr;
}

unsigned int CRunGameEngine::GetPlayerUserID( int playerIndex )
{
	auto trackerID = engine->GetTrackerIDForPlayer( playerIndex );

	return trackerID > 0 ? trackerID : 0;
}
