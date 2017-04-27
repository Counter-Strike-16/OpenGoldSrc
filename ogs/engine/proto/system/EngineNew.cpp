/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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

#include "precompiled.hpp"
#include "system/common.hpp"
#include "system/EngineNew.hpp"
#include "system/Host.hpp"
#include "system/CommandLine.hpp"
#include "filesystem/FileSystem.hpp"
#include "filesystem/FileSystemProvider.hpp"
#include "console/Console.hpp"
#include "game/GameLoaderHandler.hpp"
#include "game/NativeGameLoader.hpp"
#include "game/LegacyGameLoader.hpp"
#include "sound/Sound.hpp"
#include "input/Input.hpp"
#include "network/Network.hpp"
#include "graphics/Screen.hpp"

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

CEngine gEngine;
IEngine *eng = &gEngine;

#else // HOOK_ENGINE

IEngine *eng;

#endif // HOOK_ENGINE

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

cvar_t host_name = { "hostname", "Half-Life", 0, 0.0f, NULL };
cvar_t host_speeds = { "host_speeds", "0", 0, 0.0f, NULL };
cvar_t host_profile = { "host_profile", "0", 0, 0.0f, NULL };
cvar_t developer = { "developer", "0", 0, 0.0f, NULL };
cvar_t host_limitlocal = { "host_limitlocal", "0", 0, 0.0f, NULL };
cvar_t skill = { "skill", "1", 0, 0.0f, NULL };
cvar_t deathmatch = { "deathmatch", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t coop = { "coop", "0", FCVAR_SERVER, 0.0f, NULL };

cvar_t sys_ticrate = { "sys_ticrate", "100.0", 0, 0.0f, NULL };
cvar_t sys_timescale = { "sys_timescale", "1.0", 0, 0.0f, NULL };
cvar_t fps_max = { "fps_max", "100.0", FCVAR_ARCHIVE, 0.0f, NULL };
cvar_t host_killtime = { "host_killtime", "0.0", 0, 0.0f, NULL };
cvar_t sv_stats = { "sv_stats", "1", 0, 0.0f, NULL };
cvar_t fps_override = { "fps_override", "0", 0, 0.0f, NULL };
cvar_t host_framerate = { "host_framerate", "0", 0, 0.0f, NULL };
cvar_t pausable = { "pausable", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t suitvolume = { "suitvolume", "0.25", FCVAR_ARCHIVE, 0.0f, NULL };

#else // HOOK_ENGINE

cvar_t host_name;
cvar_t host_speeds;
cvar_t host_profile;
cvar_t developer;
cvar_t host_limitlocal;
cvar_t skill;
cvar_t deathmatch;
cvar_t coop;

cvar_t sys_ticrate;
cvar_t sys_timescale;
cvar_t fps_max;
cvar_t host_killtime;
cvar_t sv_stats;
cvar_t fps_override;
cvar_t host_framerate;
cvar_t pausable;
cvar_t suitvolume;

#endif // HOOK_ENGINE

bool CEngine::Init(const TEngineLoadParams &aLoadParams)
{
	return Init_noVirt(aLoadParams);
};

void CEngine::Shutdown()
{
	Shutdown_noVirt();
};

int CEngine::Frame()
{
	return Frame_noVirt();
};

void CEngine::AddCommandText(const char *asText)
{
	AddCommandText_noVirt(asText);
};

void CEngine::GetHostInfo(THostInfo &aInfo)
{
	GetHostInfo_noVirt(aInfo);
};

bool CEngine::Init_noVirt(const TEngineLoadParams &aLoadParams)
{
	//CRehldsPlatformHolder::get()->srand(CRehldsPlatformHolder::get()->time(NULL));

	//Q_memcpy(&host_parms, parms, sizeof(host_parms));
	//com_argc = parms->argc;
	//com_argv = parms->argv;
	
	realtime = 0.0f;
	
/*
	Memory_Init(parms->membase, parms->memsize);

	Voice_RegisterCvars();
	mpConVarHandler->Register(&console);

	if(mpCmdLine->FindArg("-console") || mpCmdLine->FindArg("-toconsole") || mpCmdLine->FindArg("-dev"))
		Cvar_DirectSet(&console, "1.0");
*/
	
	mpSystem = std::make_unique<CSystemNew>();
	
	mpCmdLine = std::make_unique<CCmdLine>();
	
	mpFileSystemProvider = std::make_unique<CFileSystemProvider>();
	mpFileSystem = std::make_unique<CFileSystem>(mpFileSystemProvider->GetFromFactory(aLoadParams.filesystemFactory));
	
	//mpHost = std::make_unique<CHost>(mpFileSystem.get());
	
	mpConsole = std::make_unique<CConsole>();
	mpNetwork = std::make_unique<CNetwork>(mpConsole.get());
	mpSound = std::make_unique<CSound>(nullptr, nullptr, nullptr);
	mpScreen = std::make_unique<CScreen>(nullptr);
	mpGameLoaderHandler = std::make_unique<CGameLoaderHandler>();
	//mpClientLoaderHandler = std::make_unique<CClientLoaderHandler>();
	mpInput = std::make_unique<CInput>(nullptr);
	//mpGame = std::make_unique<CGame>();
	
	// Add native and legacy game module loaders
	// NOTE: This thing allows us to potentially add some new
	// loaders for different game formats but this 
	// possibility looks useless for now
	mpGameLoaderHandler->AddLoader(new CNativeGameLoader());
	mpGameLoaderHandler->AddLoader(new CLegacyGameLoader(/*gpEngFuncs*/));
	
	// Add native and legacy client module loaders
	// NOTE: Same as above here
	//mpClientLoaderHandler->AddLoader(new CNativeClientLoader());
	//mpClientLoaderHandler->AddLoader(new CLegacyClientLoader());
	
	// get architecture info
	if(!mpSystem->Init())
		return false;
	
	// initialize the file system
	//TraceInit("FileSystem_Init(basedir, (void *)filesystemFactory)", "FileSystem_Shutdown()", 0);
	//if(!mpFileSystem->Init(basedir))
		//return false;
	
	InitLocal();

	//if(mpCmdLine->CheckArg("-dev"))
		//Cvar_SetValue("developer", 1.0);

	// Engine string pooling
#ifdef REHLDS_FIXES
	Ed_StrPool_Init();
#endif // REHLDS_FIXES

	//FR_Init(); // don't put it under REHLDS_FLIGHT_REC to allow recording via Rehlds API
/*
	mpCmdBuffer->Init();
	Cmd_Init();
	Cvar_Init();
	Cvar_CmdInit();

#ifdef REHLDS_FLIGHT_REC
	FR_Rehlds_Init();
#endif // REHLDS_FLIGHT_REC

	V_Init();
	Chase_Init();
	COM_Init(parms->basedir);
	Host_ClearSaveDirectory(); // mpSaveHandler->
	HPAK_Init();
	
	mpWADManager->LoadWadFile("gfx.wad"); // WADArchive->LoadFromFile?
	mpWADManager->LoadWadFile("fonts.wad");
*/
	
	// init the console so we can take prints
	mpConsole->Init();
	
	// init the input
	mpInput->Init();

	//Decal_Init();
	//Mod_Init();
	
	// initialize networking
	//Sys_InitNetworking();
	mpNetwork->Init();

	//mpServer->Init();

	// SystemWrapper_Init();

	PrintVersion();

	// Rehlds Security
	//Rehlds_Security_Init();
	
	char versionString[256];
	Q_snprintf(versionString, sizeof(versionString), "%s,%i,%i", gpszVersionString, PROTOCOL_VERSION, build_number());
	//Cvar_Set("sv_version", versionString);
	
	mpConsole->DevPrintf("%4.1f Mb heap\n", (double)parms->memsize / (1024.0f * 1024.0f));
	
	//R_InitTextures();
	
	//HPAK_CheckIntegrity("custom");
	
	//Q_memset(&g_module, 0, sizeof(g_module));
	
	//if(cls.state != ca_dedicated)
	{
#ifdef SWDS
		CSystem::Error("Only dedicated server mode is supported");
#else
		color24 *disk_basepal = nullptr; //(color24 *)COM_LoadHunkFile("gfx/palette.lmp");
		
		if(!disk_basepal)
			CSystem::Error("Host_Init: Couldn't load gfx/palette.lmp");

		//host_basepal = (unsigned short *)Hunk_AllocName(sizeof(PackedColorVec) * 256, "palette.lmp");
		
		for(int i = 0; i < 256; i++)
		{
			//PackedColorVec *basepal = (PackedColorVec *)&host_basepal[i];

			//basepal->b = disk_basepal->r;
			//basepal->g = disk_basepal->g;
			//basepal->r = disk_basepal->b;
			//basepal->a = 0; // alpha

			disk_basepal++;
		};
		
		//if(!LoadRender())
		//	return 0;
		//GL_Init(); gpRender->Init();
		
		//PM_Init(&g_clmove);
		//CL_InitEventSystem();
		//ClientDLL_Init(); // it's not loaded yet!
		// VGui_Startup();

		//if(!mpVideo->Init(host_basepal))
		{
			// VGui_Shutdown();
			//return 0;
		};

		// IN_Init ();
		//Draw_Init();
		
		mpScreen->Init();
		
		//R_Init();
		
		mpSound->Init();
		
		// CDAudio_Init();
		// Voice_Init("voice_speex", 1);
		// DemoPlayer_Init();
		// cls.state = ca_disconnected;
		// Sbar_Init ();
		//CL_Init();
#endif // SWDS
	}
	//else
		//mpConsole->RegisterVariable(&suitvolume);

	mpCmdProcessor->InsertCmdText("exec valve.rc\n");
	
	//Hunk_AllocName(0, "-HOST_HUNKLEVEL-");
	//host_hunklevel = Hunk_LowMark();
	
	//giActive = DLL_ACTIVE;
	//scr_skipupdate = false; // mpScreen->SetSkipUpdate(false);
	
	CheckGore();
	
	//TraceInit("Sys_InitAuthentication()", "Sys_ShutdownAuthentication()", 0);
	CSystem::InitAuthentication();
	
	// Since the dedicated mode is oriented on multiplayer (only)
	// We should init the game dll and enable the multiplayer mode for the network here
	if(mbDedicated)
	{
		InitGame(); // We should immediately init the game module for dedicated mode
					// (client is initializing it after the first call to new game)
		//mpNetwork->Config(TRUE); // double call (already called inside host_initgamedll)
	}
#ifndef SWDS
	//else
		//ClientDLL_ActivateMouse();
#endif
	
	host_initialized = true;
	return true;
};

void CEngine::Shutdown_noVirt()
{
	static bool isdown = false; // com_shuttingDown

	if(isdown)
	{
		printf("recursive shutdown\n");
		return;
	};

	isdown = true;
	
	//if(!mbDedicated)
		//ClientDLL_DeactivateMouse();
	
	//if(host_initialized) // Client-side
		//WriteConfig(); // TODO: also call it before the new game

	//mpServer->ServerShutdown(); // Deactivate
	//Voice_Deinit();
	
	host_initialized = false;
	
	// CDAudio_Shutdown();
	// VGui_Shutdown();
	
	//if(cls.state != ca_dedicated)
		//ClientDLL_Shutdown();

	// Rehlds Security
	//Rehlds_Security_Shutdown();
	
/*
	Cmd_RemoveGameCmds(); // Host_?
	Cmd_Shutdown();
	Cvar_Shutdown();
	
	HPAK_FlushHostQueue();
	
	mpServer->DeallocateDynamicData();

	client_t *pclient = g_psvs.clients;
	
	for(int i = 0; i < g_psvs.maxclientslimit; i++, pclient++)
		SV_ClearFrames(&pclient->frames);

	mpServer->Shutdown();
*/
	
	// SystemWrapper_ShutDown();

	mpNetwork->Shutdown();
	mpSound->Shutdown();
	mpInput->Shutdown();
	mpConsole->Shutdown();
	
	/*
	mpGameHandler->ReleaseEntityDlls();
	CL_ShutDownClientStatic();
	CM_FreePAS();

	//if(wadpath)
	//{
		//Mem_Free(wadpath);
		//wadpath = NULL;
	//};

	//if(cls.state != ca_dedicated)
		//Draw_Shutdown();

	//Draw_DecalShutdown();
	W_Shutdown();
	
	Log_Printf("Server shutdown\n");
	Log_Close();
	
	COM_Shutdown();
	CL_Shutdown();
	DELTA_Shutdown();
*/
	
	realtime = 0.0f;
	
	// host.time from these two? cl time should be in sync with sv time anyway;
	// if we on a local server then just use host.time for both sides
	// if we are connected to any remote server then use host.time for client side
	
	//g_psv.time = 0.0f;
	//cl.time = 0.0f;
};

int CEngine::Frame_noVirt()
{
#ifndef SWDS
	//(*(void (**)())(*(DWORD *)cdaudio + 24))();
#endif // SWDS

	//if(!game->IsActiveApp())
		//game->SleepUntilInput(m_nDLLState != DLL_PAUSED ? MINIMIZED_SLEEP : NOT_FOCUS_SLEEP);

	m_fCurTime = CSystem::GetFloatTime();
	m_fFrameTime = m_fCurTime - m_fOldTime;
	m_fOldTime = m_fCurTime;

	if(m_fFrameTime < 0.0f)
		m_fFrameTime = 0.001f;

	if(m_nDLLState == DLL_INACTIVE)
		return m_nDLLState;

	static int dummy;
	static int iState;
	
	iState = mpHost->Frame(m_fFrameTime, m_nDLLState, &dummy);
	
	// Compare a new state with the current
	if(iState == m_nDLLState)
		return m_nDLLState;

	SetState(iState);

	if(m_nDLLState == DLL_CLOSE)
		SetQuitting(QUIT_TODESKTOP);
	else if(m_nDLLState == DLL_RESTART)
		SetQuitting(QUIT_RESTART);

	return m_nDLLState;
};

void CEngine::AddCommandText_noVirt(const char *asText)
{
	//mpCmdBuffer->AddText(asText); //mpConsole->AddCommandText(asText); // Cbuf_AddText
};

// Move back to host?
void CEngine::GetHostInfo_noVirt(THostInfo &aInfo)
{
	if(rolling_fps > 0.0f)
		aInfo.fFPS = 1.0f / rolling_fps;
	else
	{
		rolling_fps = 0.0f;
		aInfo.fFPS = 0;
	};

	int nClients = 0;
	//mpServer->CountPlayers(&nClients);
	aInfo.nActivePlayers = nClients;

	if(aInfo.sMap)
	{
		//if(g_psv.name[0])
			//Q_strcpy(aInfo.sMap, g_psv.GetMapName());
		//else
			Q_strcpy(aInfo.sMap, "<no map>");
	};

	aInfo.nMaxPlayers = 0; //g_psvs.maxclients; // mpServer->GetMaxClients();
};

void CEngine::InitLocal()
{
	InitCommands();
	
	/*
	mpConsole->RegisterVariable(&host_killtime);
	mpConsole->RegisterVariable(&sys_ticrate);
	mpConsole->RegisterVariable(&fps_max);
	mpConsole->RegisterVariable(&fps_override);
	mpConsole->RegisterVariable(&host_name);
	mpConsole->RegisterVariable(&host_limitlocal);
	*/

	sys_timescale.value = 1.0f;

	/*
	mpConsole->RegisterVariable(&host_framerate);
	mpConsole->RegisterVariable(&host_speeds);
	mpConsole->RegisterVariable(&host_profile);
	
	mpConsole->RegisterVariable(&mp_logfile);
	mpConsole->RegisterVariable(&mp_logecho);
	
	mpConsole->RegisterVariable(&sv_log_onefile);
	mpConsole->RegisterVariable(&sv_log_singleplayer);
	mpConsole->RegisterVariable(&sv_logsecret);
	mpConsole->RegisterVariable(&sv_stats);
	
	mpConsole->RegisterVariable(&developer);
	mpConsole->RegisterVariable(&deathmatch);
	mpConsole->RegisterVariable(&coop);
	mpConsole->RegisterVariable(&pausable);
	mpConsole->RegisterVariable(&skill);

	mpServer->SetupMaxClients();
	*/
};

/*
==================
Host_InitCommands
==================
*/
void CEngine::InitCommands()
{
	// CONCMD HELL
	
#ifdef HOOK_ENGINE
	Cmd_AddCommand("shutdownserver", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_KillServer_f", (void *)Host_KillServer_f));
	Cmd_AddCommand("soundfade", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Soundfade_f", (void *)Host_Soundfade_f));
	Cmd_AddCommand("status", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Status_f", (void *)Host_Status_f));
	Cmd_AddCommand("stat", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Status_Formatted_f", (void *)Host_Status_Formatted_f));
	Cmd_AddCommand("quit", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Quit_f", (void *)Host_Quit_f));
	Cmd_AddCommand("_restart", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Quit_Restart_f", (void *)Host_Quit_Restart_f));
	Cmd_AddCommand("exit", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Quit_f", (void *)Host_Quit_f));
	Cmd_AddCommand("map", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Map_f", (void *)Host_Map_f));
	Cmd_AddCommand("career", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Career_f", (void *)Host_Career_f));
	Cmd_AddCommand("maps", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Maps_f", (void *)Host_Maps_f));
	Cmd_AddCommand("restart", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Restart_f", (void *)Host_Restart_f));
	Cmd_AddCommand("reload", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Reload_f", (void *)Host_Reload_f));
	Cmd_AddCommand("changelevel", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Changelevel_f", (void *)Host_Changelevel_f));
	Cmd_AddCommand("changelevel2", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Changelevel2_f", (void *)Host_Changelevel2_f));
	Cmd_AddCommand("reconnect", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Reconnect_f", (void *)Host_Reconnect_f));
	Cmd_AddCommand("version", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Version_f", (void *)Host_Version_f));
	Cmd_AddCommand("say", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Say_f", (void *)Host_Say_f));
	Cmd_AddCommand("say_team", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Say_Team_f", (void *)Host_Say_Team_f));
	Cmd_AddCommand("tell", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Tell_f", (void *)Host_Tell_f));
	Cmd_AddCommand("kill", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Kill_f", (void *)Host_Kill_f));
	Cmd_AddCommand("pause", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_TogglePause_f", (void *)Host_TogglePause_f));
	Cmd_AddCommand("setpause", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Pause_f", (void *)Host_Pause_f));
	Cmd_AddCommand("unpause", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Unpause_f", (void *)Host_Unpause_f));
	Cmd_AddCommand("kick", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Kick_f", (void *)Host_Kick_f));
	Cmd_AddCommand("ping", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Ping_f", (void *)Host_Ping_f));
	Cmd_AddCommand("motd", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Motd_f", (void *)Host_Motd_f));
	Cmd_AddCommand("motd_write", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Motd_Write_f", (void *)Host_Motd_Write_f));
	Cmd_AddCommand("stats", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Stats_f", (void *)Host_Stats_f));
	Cmd_AddCommand("load", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Loadgame_f", (void *)Host_Loadgame_f));
	Cmd_AddCommand("save", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Savegame_f", (void *)Host_Savegame_f));
	Cmd_AddCommand("autosave", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_AutoSave_f", (void *)Host_AutoSave_f));
	Cmd_AddCommand("writecfg", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_WriteCustomConfig", (void *)Host_WriteCustomConfig));
	Cmd_AddCommand("startdemos",(xcommand_t)GetOriginalFuncAddrOrDefault("Host_Startdemos_f", (void *)Host_Startdemos_f));
	Cmd_AddCommand("demos", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Demos_f", (void *)Host_Demos_f));
	Cmd_AddCommand("stopdemo", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Stopdemo_f", (void *)Host_Stopdemo_f));
	Cmd_AddCommand("setinfo", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_SetInfo_f", (void *)Host_SetInfo_f));
	Cmd_AddCommand("fullinfo", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_FullInfo_f", (void *)Host_FullInfo_f));
	Cmd_AddCommand("mcache", (xcommand_t)GetOriginalFuncAddrOrDefault("Mod_Print", (void *)Mod_Print));
	Cmd_AddCommand("interp", (xcommand_t)GetOriginalFuncAddrOrDefault("Host_Interp_f", (void *)Host_Interp_f));
	Cmd_AddCommand("setmaster", (xcommand_t)GetOriginalFuncAddrOrDefault("Master_SetMaster_f", (void *)Master_SetMaster_f));
	Cmd_AddCommand("heartbeat", (xcommand_t)GetOriginalFuncAddrOrDefault("Master_Heartbeat_f", (void *)Master_Heartbeat_f));
#else // HOOK_ENGINE

#ifndef SWDS
	//mpConCmdHandler->AddCommand("cd", CD_Command_f);
	//mpConCmdHandler->AddCommand("mp3", MP3_Command_f);
	//mpConCmdHandler->AddCommand("_careeraudio", CareerAudio_Command_f);
#endif // SWDS

/*
	mpConCmdHandler->AddCommand("shutdownserver", Host_KillServer_f);
	mpConCmdHandler->AddCommand("soundfade", Host_Soundfade_f);
	mpConCmdHandler->AddCommand("status", Host_Status_f);
	mpConCmdHandler->AddCommand("stat", Host_Status_Formatted_f);
	mpConCmdHandler->AddCommand("quit", Host_Quit_f);
	mpConCmdHandler->AddCommand("_restart", Host_Quit_Restart_f);

#ifndef SWDS
	//mpConCmdHandler->AddCommand("_setrenderer", Host_SetRenderer_f);
	//mpConCmdHandler->AddCommand("_setvideomode", Host_SetVideoMode_f);
	//mpConCmdHandler->AddCommand("_setgamedir", Host_SetGameDir_f);
	mpConCmdHandler->AddCommand("_sethdmodels", Host_SetHDModels_f);
	mpConCmdHandler->AddCommand("_setaddons_folder", Host_SetAddonsFolder_f);
	mpConCmdHandler->AddCommand("_set_vid_level", Host_SetVideoLevel_f);
#endif // SWDS

	mpConCmdHandler->AddCommand("exit", Host_Quit_f);
	mpConCmdHandler->AddCommand("map", Host_Map_f); ([this](){Map_f();})
	mpConCmdHandler->AddCommand("career", Host_Career_f);
	mpConCmdHandler->AddCommand("maps", Host_Maps_f);
	mpConCmdHandler->AddCommand("restart", Host_Restart_f);
	mpConCmdHandler->AddCommand("reload", Host_Reload_f);

	mpConCmdHandler->AddCommand("changelevel", Host_Changelevel_f);
	mpConCmdHandler->AddCommand("changelevel2", Host_Changelevel2_f);

	mpConCmdHandler->AddCommand("reconnect", Host_Reconnect_f);
	mpConCmdHandler->AddCommand("version", Host_Version_f);
	mpConCmdHandler->AddCommand("say", Host_Say_f);
	mpConCmdHandler->AddCommand("say_team", Host_Say_Team_f);
	mpConCmdHandler->AddCommand("tell", Host_Tell_f);
	mpConCmdHandler->AddCommand("kill", Host_Kill_f);
	mpConCmdHandler->AddCommand("pause", Host_TogglePause_f);
	mpConCmdHandler->AddCommand("setpause", Host_Pause_f);
	mpConCmdHandler->AddCommand("unpause", Host_Unpause_f);
	mpConCmdHandler->AddCommand("kick", Host_Kick_f);
	mpConCmdHandler->AddCommand("ping", Host_Ping_f);
	mpConCmdHandler->AddCommand("motd", Host_Motd_f);
	mpConCmdHandler->AddCommand("motd_write", Host_Motd_Write_f);
	mpConCmdHandler->AddCommand("stats", Host_Stats_f);
	mpConCmdHandler->AddCommand("load", Host_Loadgame_f);
	mpConCmdHandler->AddCommand("save", Host_Savegame_f);
	mpConCmdHandler->AddCommand("autosave", Host_AutoSave_f);
	mpConCmdHandler->AddCommand("writecfg", Host_WriteCustomConfig);

#ifndef SWDS
	mpConCmdHandler->AddCommand("+voicerecord", Host_VoiceRecordStart_f);
	mpConCmdHandler->AddCommand("-voicerecord", Host_VoiceRecordStop_f);
#endif // SWDS

	mpConCmdHandler->AddCommand("startdemos", Host_Startdemos_f);
	mpConCmdHandler->AddCommand("demos", Host_Demos_f);
	mpConCmdHandler->AddCommand("stopdemo", Host_Stopdemo_f);
	
	mpConCmdHandler->AddCommand("setinfo", Host_SetInfo_f);
	mpConCmdHandler->AddCommand("fullinfo", Host_FullInfo_f);

#ifndef SWDS
	mpConCmdHandler->AddCommand("god", Host_God_f);
	mpConCmdHandler->AddCommand("notarget", Host_Notarget_f);
	mpConCmdHandler->AddCommand("fly", Host_Fly_f);
	mpConCmdHandler->AddCommand("noclip", Host_Noclip_f);
	
	mpConCmdHandler->AddCommand("viewmodel", Host_Viewmodel_f);
	mpConCmdHandler->AddCommand("viewframe", Host_Viewframe_f);
	mpConCmdHandler->AddCommand("viewnext", Host_Viewnext_f);
	mpConCmdHandler->AddCommand("viewprev", Host_Viewprev_f);
#endif // SWDS

	Cmd_AddCommand("mcache", Mod_Print);
	Cmd_AddCommand("interp", Host_Interp_f);
	
	//Cmd_AddCommand("setmaster", Master_SetMaster_f);
	Cmd_AddCommand("heartbeat", Master_Heartbeat_f);
*/
#endif // HOOK_ENGINE

	//Cvar_RegisterVariable(&gHostMap);
	//Cvar_RegisterVariable(&voice_recordtofile);
	//Cvar_RegisterVariable(&voice_inputfromfile);
};

bool CEngine::FilterTime(float time)
{
	if(host_framerate.value > 0.0f)
	{
		if(mpHost->IsSinglePlayerGame() /*|| cls.demoplayback*/)
		{
			host_frametime = sys_timescale.value * host_framerate.value;
			realtime += host_frametime;
			return true;
		};
	};

	realtime += sys_timescale.value * time;
	
	float fps = 0.0f;
	
	if(mbDedicated)
	{
		static int command_line_ticrate = -1;
		
		if(command_line_ticrate == -1)
			command_line_ticrate = mpCmdLine->FindArg("-sys_ticrate");
		
		if(command_line_ticrate > 0)
			fps = Q_atof(mpCmdLine->GetArg(command_line_ticrate + 1)); // com_argv[command_line_ticrate + 1]
		else
			fps = sys_ticrate.value;

		if(fps > 0.0f)
		{
			if(1.0f / (fps + 1.0f) > realtime - oldrealtime)
				return false;
		};
	}
	else
	{
		fps = 31.0f;

		if(false) //if(g_psv.active || cls.state == ca_disconnected || cls.state == ca_active)
		{
			fps = 0.5f;

			if(fps_max.value >= 0.5f)
				fps = fps_max.value;
		};

		if(!fps_override.value)
		{
			if(fps > 100.0f)
				fps = 100.0f;
		};

/*
		if(cl.maxclients > 1)
		{
			if(fps < 20.0f)
				fps = 20.0f;
		};

		if(gl_vsync.value)
		{
			if(!fps_override.value)
				fps = 100.f;
		};

		if(!cls.timedemo)
		{
			if(sys_timescale.value / (fps + 0.5f) > realtime - oldrealtime)
				return false;
		};
*/
	};

	host_frametime = realtime - oldrealtime;
	oldrealtime = realtime;

	if(host_frametime > 0.25f)
		host_frametime = 0.25f;

	return true;
};

/*
==================
Host_Frame

Runs all active servers
==================
*/
void CEngine::_Frame(float time)
{
	static double host_times[6];
	
	//if(setjmp(host_enddemo))
		//return;

	// Unknown_windows_func_01D37CD0();
	
	if(!FilterTime(time))
		return;

#ifdef REHLDS_FLIGHT_REC
	static long frameCounter = 0;
	
	if(rehlds_flrec_frame.string[0] != '0')
		FR_StartFrame(frameCounter);
#endif // REHLDS_FLIGHT_REC

	// SystemWrapper_RunFrame(host_frametime);

	//if(g_modfuncs.m_pfnFrameBegin)
		//g_modfuncs.m_pfnFrameBegin();
	
	ComputeFPS(host_frametime);
	
	//R_SetStackBase();
	//CL_CheckClientState();

	// process console commands
	mpCmdProcessor->ExecCmdBuffer();

	//ClientDLL_UpdateClientData();

	//if(mpServer->IsActive())
		//CL_Move();

	host_times[1] = CSystem::GetFloatTime();

	//mpServer->Frame(host_frametime); // netserver or gameserver (works as netserver->gameserver) frame

	host_times[2] = CSystem::GetFloatTime();

/*	
	mpServer->CheckForRcon();

	if(!g_psv.IsActive())
		CL_Move();

	ClientDLL_Frame(host_frametime);
	CL_SetLastUpdate();

	// fetch results from server
	CL_ReadPackets();
	
	// wipe all the local states that are older than the latest state 
	// received from the server and reapply the rest above it
	//			received state time
	//					|
	//					v
	// [----------------|---------------]
	//				<--- will be removed
	//					|---> reapply our predicted states
	CL_RedoPrediction();
	
	CL_VoiceIdle();
	
	// build a refresh entity list
	CL_EmitEntities();
	
	CL_CheckForResend();

	while(CL_RequestMissingResources())
		;
*/

	CheckConnectionFailure();

/*	
	// CL_HTTPUpdate();
	Steam_ClientRunFrame();
	ClientDLL_CAM_Think();
	CL_MoveSpectatorCamera();
*/

	host_times[3] = CSystem::GetFloatTime();

	UpdateScreen();

	host_times[4] = CSystem::GetFloatTime();

	//CL_DecayLights();

	UpdateSounds();
	
	//CDAudio_Update(); // call here or inside the UpdateSounds method

	host_times[0] = host_times[5];
	host_times[5] = CSystem::GetFloatTime();

	PrintSpeeds(host_times);

	++host_framecount;

	//CL_AdjustClock();

	if(sv_stats.value == 1.0f)
		UpdateStats();
/*
	if(host_killtime.value != 0.0 && host_killtime.value < g_psv.time)
		Host_Quit_f();

	// Rehlds Security
	//Rehlds_Security_Frame();

#ifdef REHLDS_FLIGHT_REC
	if(rehlds_flrec_frame.string[0] != '0')
		FR_EndFrame(frameCounter);

	frameCounter++;
#endif // REHLDS_FLIGHT_REC
*/
};

int CEngine::Frame(float time, int iState, int *stateInfo)
{
/*
	// something bad happened, or the server disconnected
	if(setjmp(host_abortserver))
		return giActive;

	if(giActive != 3 || !g_iQuitCommandIssued)
		giActive = iState;

	*stateInfo = 0;
*/
	
	double time1;
	double time2;
	
	if(host_profile.value != 0.0f)
		time1 = CSystem::GetFloatTime();

	static int nFrame = 0;
	nFrame++;

	// 100 frames interval
	if(nFrame % 100 == 0)
		CSystem::Printf("Flap #%d (FrameTime: %2f, %d)\n", nFrame, time, iState);
	
	_Frame(time);
	
	if(host_profile.value != 0.0f)
		time2 = CSystem::GetFloatTime();

/*
	if(giStateInfo)
	{
		*stateInfo = giStateInfo;
		giStateInfo = 0;
		mpCmdProcessor->ExecCmdBuffer();
	};
*/

	if(host_profile.value != 0.0f)
	{
		static double timetotal;
		static int timecount;

		timecount++;
		timetotal += time2 - time1;

		if(++timecount >= 1000)
		{
			int m = (timetotal * 1000.0f / (double)timecount);
			int c = 0;

			timecount = 0;
			timetotal = 0.0f;
		
			//mpServer->GetActiveClients(); or GetClients(bool active = true)
			//for(int i = 0; i < g_psvs.GetMaxClients(); i++)
			{
				//if(g_psvs.GetClient(i)->IsActive()) // GetClients()[i]
					//++c;
			};

			mpConsole->Printf("host_profile: %2i clients %2i msec\n", c, m);
		};
	};

	return 1; //giActive;
};