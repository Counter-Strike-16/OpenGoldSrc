#ifndef GAMEUI_RUNGAMEENGINE_H
#define GAMEUI_RUNGAMEENGINE_H

#include <tier0/platform.h>

#include "IRunGameEngine.h"

class CRunGameEngine : public IRunGameEngine
{
public:
	~CRunGameEngine();

	bool IsRunning() override;

	bool AddTextCommand( const char *text ) override;

	ERunResult RunEngine( int iAppID, const char *gameDir, const char *commandLineParams ) override;

	bool IsInGame() override;

	bool GetGameInfo( char *infoBuffer, int bufferSize ) override;

	void SetTrackerUserID( int trackerID, const char *trackerName ) override;

	int GetPlayerCount() override;

	unsigned int GetPlayerFriendsID( int playerIndex ) override;

	const char *GetPlayerName( int friendsID ) override;

	const char *GetPlayerFriendsName( int friendsID ) override;

	unsigned int GetEngineBuildNumber() override;
	const char *GetProductVersionString() override;

	unsigned int GetPlayerUserID( int playerIndex ) override;
};

#endif //GAMEUI_RUNGAMEENGINE_H
