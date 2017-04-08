/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file
/// @brief primary header for host

#pragma once

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

//=============================================================================

#include <memory>
#include "game/GameServer.hpp"
#include "console/Console.hpp"
#include "network/Network.hpp"
#include "sound/Sound.hpp"
#include "graphics/Screen.hpp"

typedef struct quakeparms_s
{
	char *basedir;
	char *cachedir; // for development over ISDN lines

	int argc;
	char **argv;

	void *membase;
	int memsize;
} quakeparms_t;

class CFileSystem;

class CHost
{
public:
	CHost(CFileSystem *apFileSystem) : mpFileSystem(apFileSystem){}

	int Init(quakeparms_t *parms);
	void InitLocal();
	
	void Shutdown();
	
	void _Frame(float time);
	int Frame(float time, int iState, int *stateInfo);
	
	void EndGame(const char *message, ...);

	void NORETURN Error(const char *error, ...);

	void WriteConfig();
	void WriteCustomConfig();

	void ClientCommands(const char *fmt, ...);
	void ClearClients(bool bFramesOnly);
	void ShutdownServer(bool crash);

	void CheckDynamicStructures();
	void ClearMemory(bool bQuiet);
	bool FilterTime(float time);

	void ComputeFPS(double frametime);
	void GetInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap);
	void PrintSpeeds(double *time); // CalcSpeeds

	void UpdateScreen();
	void UpdateSounds();

	void CheckConnectionFailure();

	void CheckGore();

	bool IsSinglePlayerGame();
	bool IsServerActive();

	void PrintVersion();
	
	bool IsInitialized() const {return host_initialized;}
	
	int GetStartTime();
	
	void InitializeGameDLL();
	
	void UpdateStats();
	
	//
	
/*
	void GetStatsString(char *buf, int bufSize);
	
	void Status_Printf(qboolean conprint, qboolean log, char *fmt, ...);
	void Map(qboolean bIsDemo, char *mapstring, char *mapName, qboolean loadGame);
	
	// save handler
	
	const char *FindRecentSave(char *pNameBuf);
	char *SaveGameDirectory();
	void SavegameComment(char *pszBuffer, int iSizeBuffer);
	void SaveAgeList(const char *pName, int count);
	int ValidSave();
	
	SAVERESTOREDATA *SaveInit(int size);
	void SaveExit(SAVERESTOREDATA *save);
	
	qboolean SaveGameSlot(const char *pSaveName, const char *pSaveComment);
	qboolean SaveGame(const char *pszSlot, const char *pszComment);
	
	int SaveReadHeader(FileHandle_t pFile, GAME_HEADER *pHeader, int readGlobalState);
	void SaveReadComment(FileHandle_t f, char *name);
	
	int LoadGame(const char *pName);
	int Host_Load(const char *pName);
	SAVERESTOREDATA *SaveGamestate();
	void EntityInit(edict_t *pEdict, int className);
	SAVERESTOREDATA *LoadSaveData(const char *level);
	void ParseSaveTables(SAVERESTOREDATA *pSaveData, SAVE_HEADER *pHeader, int updateGlobals);
	void EntityPatchWrite(SAVERESTOREDATA *pSaveData, const char *level);
	void EntityPatchRead(SAVERESTOREDATA *pSaveData, const char *level);
	int LoadGamestate(char *level, int createPlayers);
	int EntryInTable(SAVERESTOREDATA *pSaveData, const char *pMapName, int index);
	void LandmarkOrigin(SAVERESTOREDATA *pSaveData, vec_t *output, const char *pLandmarkName);
	int EntityInSolid(edict_t *pent);
	int CreateEntityList(SAVERESTOREDATA *pSaveData, int levelMask);
	void LoadAdjacentEntities(const char *pOldLevel, const char *pLandmarkName);
	int FileSize(FileHandle_t pFile);
	void FileCopy(FileHandle_t pOutput, FileHandle_t pInput, int fileSize);
	void DirectoryCopy(const char *pPath, FileHandle_t pFile);
	void DirectoryExtract(FileHandle_t pFile, int fileCount);
	int DirectoryCount(const char *pPath);
	void Host_ClearSaveDirectory();
	void Host_ClearGameState();
	void Host_Say(qboolean teamonly);
	NOXREF void Host_EndSection(const char *pszSection);
	
	void Host_NextDemo();
*/
private:
	// Console commands
/*	
	void Host_Motd_f();
	void Host_Motd_Write_f();

	void Host_Stats_f();
	void Host_Quit_f();
	void Host_Quit_Restart_f();

	void Host_Status_f();
	void Host_Status_Formatted_f();
	void Host_Ping_f();

	void Host_Map_f();
	void Host_Career_f();
	void Host_Maps_f();
	void Host_Changelevel_f();

	void Host_Restart_f();
	void Host_Reload_f();
	void Host_Reconnect_f();

	void Host_Savegame_f();
	void Host_AutoSave_f();

	void Host_Loadgame_f();

	void Host_Changelevel2_f();
	void Host_Version_f();
	void Host_FullInfo_f();
	NOXREF void Host_KillVoice_f();
	void Host_SetInfo_f();
	void Host_Say_f();
	void Host_Say_Team_f();
	void Host_Tell_f();
	void Host_Kill_f();
	void Host_TogglePause_f();
	void Host_Pause_f();
	void Host_Unpause_f();
	void Host_Interp_f();

	void Host_Startdemos_f();
	void Host_Demos_f();
	void Host_Stopdemo_f();
	void Host_Soundfade_f();
	void Host_KillServer_f();
	void Host_VoiceRecordStart_f();
	void Host_VoiceRecordStop_f();
	void Host_Crash_f();
*/
	
	//
	
	void ClearIOStates();
	
	void InitCommands();
	
	char gpszVersionString[32];
	char gpszProductString[32];
	
	std::unique_ptr<CConsole> mpConsole; // IConsole
	//std::unique_ptr<CCmdBuffer> mpCmdBuffer;
	
	std::unique_ptr<CNetwork> mpNetwork;
	std::unique_ptr<CSound> mpSound;
	std::unique_ptr<CGameServer> mpServer;
	std::unique_ptr<CScreen> mpScreen;
	
	CFileSystem *mpFileSystem{nullptr};

	quakeparms_t *host_params{nullptr};
	
	bool host_initialized{false}; // true if into command execution
	
	double realtime{0.0f};  // without any filtering or bounding;
							// not bounded in any way, changed at
							// start of every frame, never reset
	double oldrealtime{0.0f}; // last frame run
	
	double host_frametime{0.0f};
	
	double rolling_fps{0.0f};
	
	int32 startTime{0};
	
	int host_framecount{0}; // incremented every frame, never reset
	
	int host_hunklevel{0};

	//jmp_buf host_abortserver;
	//jmp_buf host_enddemo;
};