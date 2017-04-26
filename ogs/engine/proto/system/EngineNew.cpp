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
	Cvar_RegisterVariable(&console);

	if(mpCmdLine->FindArg("-console") || mpCmdLine->FindArg("-toconsole") || mpCmdLine->FindArg("-dev"))
		Cvar_DirectSet(&console, "1.0");
*/
	
	mpConsole = std::make_unique<CConsole>();
	mpNetwork = std::make_unique<CNetwork>(mpConsole.get());
	mpSound = std::make_unique<CSound>(nullptr, nullptr, nullptr);
	mpScreen = std::make_unique<CScreen>(nullptr);
	mpGameLoaderHandler = std::make_unique<CGameLoaderHandler>();
	//mpClientLoaderHandler = std::make_unique<CClientLoaderHandler>();
	mpInput = std::make_unique<CInput>(nullptr);
	
	// Add native and legacy game module loaders
	// NOTE: This thing allows us to potentially some new loaders for different game formats
	// but this possibility looks useless for now
	mpGameLoaderHandler->AddLoader(new CNativeGameLoader());
	mpGameLoaderHandler->AddLoader(new CLegacyGameLoader(/*gpEngFuncs*/));
	
	// Add native and legacy client module loaders
	// NOTE: Same as above here
	//mpClientLoaderHandler->AddLoader(new CNativeClientLoader());
	//mpClientLoaderHandler->AddLoader(new CLegacyClientLoader());
	
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
	
	Key_Init();
*/

	mpConsole->Init();
	mpInput->Init();

	//Decal_Init();
	//Mod_Init();

	mpNetwork->Init();
	//Netchan_Init();

	//DELTA_Init();

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
	static bool isdown = false;

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
	
	//mpInput->Shutdown();
	
	mpConsole->Shutdown();
	
	/*
	ReleaseEntityDlls();
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
	Key_Shutdown();
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
	return 0;
};

void CEngine::AddCommandText_noVirt(const char *asText)
{
};

void CEngine::GetHostInfo_noVirt(THostInfo &aInfo)
{
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

void CEngine::InitCommands()
{
};

bool CEngine::FilterTime(float time)
{
	if(host_framerate.value > 0.0f)
	{
		if(IsSinglePlayerGame() /*|| cls.demoplayback*/)
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