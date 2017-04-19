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
/// @brief primary header for host

// TODO: reduce the size of this crap

#include "precompiled.hpp"
#include "system/Host.hpp"
#include "system/common.hpp"
#include "system/System.hpp"
#include "system/SystemTypes.hpp"
#include "console/Console.hpp"
#include "game/GameLoaderHandler.hpp"
#include "input/Input.hpp"

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

/*
====================
Host_Init
====================
*/
int CHost::Init(quakeparms_t *parms)
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

	if(mpCmdLine->CheckArg("-console") || mpCmdLine->CheckArg("-toconsole") || mpCmdLine->CheckArg("-dev"))
		Cvar_DirectSet(&console, "1.0");
*/

	mpConsole = std::make_unique<CConsole>();
	mpNetwork = std::make_unique<CNetwork>(mpConsole.get());
	mpSound = std::make_unique<CSound>();
	mpScreen = std::make_unique<CScreen>();
	mpGameLoaderHandler = std::make_unique<CGameLoaderHandler>();
	mpInput = std::make_unique<CInput>();
	
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
	Host_ClearSaveDirectory();
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

		//if(!VID_Init(host_basepal))
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

	//mpCmdBuffer->InsertText("exec valve.rc\n");
	
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
	return 1;
};

void CHost::InitLocal()
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

void CHost::Shutdown()
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
	
	if(host_initialized) // Client-side
		WriteConfig(); // TODO: also call it before the new game

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
	
	//g_psv.time = 0.0f;
	//cl.time = 0.0f;
};

/*
================
Host_EndGame
================
*/
void CHost::EndGame(const char *message, ...)
{
	NOXREFCHECK;
	
	va_list argptr;
	char string[1024];

	va_start(argptr, message);
	Q_vsnprintf(string, sizeof(string), message, argptr);
	va_end(argptr);

	mpConsole->DevPrintf("%s: %s\n", __FUNCTION__, string);
	
#ifndef SWDS
	scr_disabled_for_loading = true;
#endif
	
	int oldn = 0; //cls.demonum;
	
	//if(mpServer->IsActive())
		//ShutdownServer(false);

	//cls.demonum = oldn;

	//if(!cls.state)
		//CSystem::Error("%s: %s\n", __FUNCTION__, string);

	if(oldn != -1)
	{
		//CL_Disconnect_f();
		//cls.demonum = oldn;
		//Host_NextDemo();
		//longjmp(host_enddemo, 1);
	};

	//CL_Disconnect();
	//mpCmdBuffer->AddText("cd stop\n");
	//mpCmdBuffer->Execute();
	//longjmp(host_abortserver, 1);
};

/*
================
Host_Error

This shuts down both the client and server
================
*/
void NORETURN CHost::Error(const char *error, ...)
{
	static bool inerror = false;

	if(inerror)
		CSystem::Error("%s: recursively entered", __FUNCTION__);

	inerror = true;
	
	va_list argptr;
	va_start(argptr, error);
	
	mpScreen->EndLoadingPlaque();
	
	char string[1024];
	Q_vsnprintf(string, sizeof(string), error, argptr);
	va_end(argptr);

	//if(mpServer->IsActive() && developer.value != 0.0f)
		//CL_WriteMessageHistory(0, 0);

	mpConsole->Printf("%s: %s\n", __FUNCTION__, string);
	
	//if(mpServer->IsActive())
		//ShutdownServer(false);

	//if(cls.state)
	{
		//CL_Disconnect();
		//cls.demonum = -1;
		inerror = false;
		//longjmp(host_abortserver, 1);
	};
	
	CSystem::Error("%s: %s\n", __FUNCTION__, string);
};

/*
===============
CL_WriteConfiguration

Writes key bindings and archived cvars to config.cfg
===============
*/
void CHost::WriteConfig()
{
#ifndef SWDS
	if(!host_initialized || cls.state == ca_dedicated) // == ca_uninitialized
		return;

#ifdef _WIN32
	CSystem::GetRegKeyValue("Software\\Valve\\Steam", "rate", rate_.string);
	
	if(cl_name.string && Q_stricmp(cl_name.string, "unnamed") && Q_stricmp(cl_name.string, "player") && Q_strlen(cl_name.string))
		CSystem::GetRegKeyValue("Software\\Valve\\Steam", "LastGameNameUsed", cl_name.string);
#else
	CSystem::SetRateRegistrySetting(rate_.string);
#endif // _WIN32
	
	if(Key_CountBindings() <= 1)
	{
		mpConsole->Printf("skipping config.cfg output, no keys bound\n");
		return;
	};

	bool bSetFileToReadOnly = false;
	
	char nameBuf[4096];
	
	CFile *f = mpFileSystem->OpenPathID("config.cfg", "w", "GAMECONFIG");
	
	if(!f)
	{
		if(!developer.value || !mpFileSystem->FileExists("../goldsrc/dev_build_all.bat"))
		{
			if(mpFileSystem->GetLocalPath("config.cfg", nameBuf, sizeof(nameBuf)))
			{
				bSetFileToReadOnly = true;
				chmod(nameBuf, S_IREAD | S_IWRITE);
			};
			
			f = mpFileSystem->OpenPathID("config.cfg", "w", "GAMECONFIG");
			
			if(!f)
			{
				mpConsole->Printf("Couldn't write config.cfg.\n");
				return;
			};
		};
	};

	f->Printf("// This file is overwritten whenever you change your user settings in the game.\n");
	f->Printf("// Add custom configurations to the file \"userconfig.cfg\".\n\n");
	f->Printf("unbindall\n");

	Key_WriteBindings(f);
	Cvar_WriteVariables(f);
	//mpClient->GetUserInfo()->WriteToFile(f);

	kbutton_t *ml = nullptr; //ClientDLL_FindKey("in_mlook");
	kbutton_t *jl = nullptr; //ClientDLL_FindKey("in_jlook");

	if(ml && (ml->state & 1))
		f->Printf("+mlook\n");

	if(jl && (jl->state & 1))
		f->Printf("+jlook\n");

	f->Printf("exec userconfig.cfg\n");
	
	mpFileSystem->Close(f);

	if(bSetFileToReadOnly)
	{
		mpFileSystem->GetLocalPath("config.cfg", nameBuf, sizeof(nameBuf));
		chmod(nameBuf, S_IREAD);
	};
#endif // SWDS
};

void CHost::WriteCustomConfig()
{
	char configname[261]; // magic
	//Q_snprintf(configname, 257, "%s", Cmd_Args());

	if(Q_strstr(configname, "..") || !Q_stricmp(configname, "config") ||
	   !Q_stricmp(configname, "autoexec") ||
	   !Q_stricmp(configname, "listenserver") ||
	   !Q_stricmp(configname, "server") ||
	   !Q_stricmp(configname, "userconfig"))
	{
		mpConsole->Printf("skipping writecfg output, invalid filename given\n");
	}
#ifndef SWDS
	else
	{
		//if(host_initialized && cls.state != ca_dedicated)
		{
			if(0 < 2) //if(Key_CountBindings() < 2)
				mpConsole->Printf("skipping config.cfg output, no keys bound\n");
			else
			{
				Q_strcat(configname, ".cfg");
				
				CFile *f = mpFileSystem->OpenPathID(configname, "w", "GAMECONFIG");
				
				if(!f)
				{
					mpConsole->Printf("Couldn't write %s.\n", configname);
					return;
				};

				f->Printf("unbindall\n");
				Key_WriteBindings(f);
				Cvar_WriteVariables(f);
				mpClient->GetUserInfo()->WriteToFile(f);

				kbutton_t *ml = nullptr; //ClientDLL_FindKey("in_mlook");
				kbutton_t *jl = nullptr; //ClientDLL_FindKey("in_jlook");

				if(ml && ml->state & 1)
					f->Printf("+mlook\n");

				if(jl && jl->state & 1)
					f->Printf("+jlook\n");

				mpFileSystem->Close(f);
				mpConsole->Printf("%s successfully created!\n", configname);
			};
		};
	};
#endif // SWDS
};

void CHost::ClientCommands(const char *fmt, ...)
{
	va_list argptr;
	char string[1024];

	va_start(argptr, fmt);
	
	//if(!host_client->fakeclient)
	{
		Q_vsnprintf(string, sizeof(string), fmt, argptr);
		string[sizeof(string) - 1] = 0;
		
		//host_client->netchan.message
		//INetMsg *pMsg = pHostClient->GetNetChan()->BeginMessage();
		//pHostClient->SendStuffText();
		// or
		//...->SendStuffText(pHostClient);
		
		//pMsg->WriteByte(svc_stufftext);
		//pMsg->WriteString(string);
	};

	va_end(argptr);
};

void CHost::ShutdownServer(bool crash)
{
	/*
	int i;
	
	if(!mpServer->IsActive())
		return;

	SV_ServerShutdown();
	g_psv.active = FALSE;
	NET_ClearLagData(TRUE, TRUE);

	host_client = g_psvs.clients;
	for(i = 0; i < g_psvs.maxclients; i++, host_client++)
	{
		if(host_client->IsActive() || host_client->IsConnected())
			host_client->Drop(crash, "Server shutting down"); // was SV_DropClient(client_t *, ...)
	};

	CL_Disconnect();
	SV_ClearEntities();
	SV_ClearCaches();
	//FreeAllEntPrivateData();
	Q_memset(&g_psv, 0, sizeof(server_t));
	CL_ClearClientState();

	SV_ClearClientStates();
	ClearClients(false);

	host_client = g_psvs.clients;
	for(i = 0; i < g_psvs.maxclientslimit; i++, host_client++)
		SV_ClearFrames(&host_client->frames);

	Q_memset(g_psvs.clients, 0, sizeof(client_t) * g_psvs.maxclientslimit);
	HPAK_FlushHostQueue();
	Steam_Shutdown();
	Log_Printf("Server shutdown\n");
	Log_Close();
	*/
};

void CHost::ClearMemory(bool bQuiet)
{
	// Engine string pooling
#ifdef REHLDS_FIXES
	Ed_StrPool_Reset();
#endif // REHLDS_FIXES

	//CM_FreePAS();
	//SV_ClearEntities();

	if(!bQuiet)
		mpConsole->DevPrintf("Clearing memory\n");

	//D_FlushCaches();
	//Mod_ClearAll();

	if(host_hunklevel)
	{
		mpServer->CheckDynamicStructures();
		//Hunk_FreeToLowMark(host_hunklevel);
	};

/*
	cls.signon = 0;
	mpServer->ClearCaches();
	
	Q_memset(&g_psv, 0, sizeof(server_t));
	
	CL_ClearClientState();
	
	mpServer->ClearClientStates();
*/
};

bool CHost::FilterTime(float time)
{
	static int command_line_ticrate = -1;

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
	
	if(gbIsDedicatedServer)
	{
		//if(command_line_ticrate == -1)
			//command_line_ticrate = mpCmdLine->CheckArg("-sys_ticrate");
		
		//if(command_line_ticrate > 0)
			//fps = Q_atof(com_argv[command_line_ticrate + 1]); // ->GetArgValue();
		//else
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

void CHost::ComputeFPS(double frametime)
{
	rolling_fps = 0.6f * rolling_fps + 0.4f * frametime;
};

void CHost::GetInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap)
{
	if(rolling_fps > 0.0f)
		*fps = 1.0f / rolling_fps;
	else
	{
		rolling_fps = 0.0f;
		*fps = 0;
	};

	int clients = 0;
	//mpServer->CountPlayers(&clients);
	*nActive = clients;

	if(unused)
		*unused = 0;

	if(pszMap)
	{
		//if(g_psv.name[0])
			//Q_strcpy(pszMap, g_psv.GetMapName());
		//else
			Q_strcpy(pszMap, "<no map>");
	};

	*nMaxPlayers = 0; //g_psvs.maxclients; // mpServer->GetMaxClients();
};

void CHost::PrintSpeeds(double *time) // Or CalcSpeeds
{
	float pass1, pass2, pass3, pass4, pass5;
	double frameTime;
	double fps;

#ifdef REHLDS_FIXES
	if(host_speeds.value != 0.0f) // FIXED: do calculations only if host_speeds is enabled
#endif // REHLDS_FIXES
	{
		pass1 = (float)((time[1] - time[0]) * 1000.0f);
		pass2 = (float)((time[2] - time[1]) * 1000.0f);
		pass3 = (float)((time[3] - time[2]) * 1000.0f);
		pass4 = (float)((time[4] - time[3]) * 1000.0f);
		pass5 = (float)((time[5] - time[4]) * 1000.0f);

		frameTime = (pass5 + pass4 + pass3 + pass2 + pass1) / 1000.0f;

		if(frameTime >= 0.0001f)
			fps = 1.0f / frameTime;
		else
			fps = 999.0f;
		
#ifndef REHLDS_FIXES
	};
	/*
		if(host_speeds.value != 0.0f)
#endif // REHLDS_FIXES
		{
			int ent_count = 0;
			
			for(int i = 0; i < g_psv.num_edicts; i++)
			{
				if(!g_psv.edicts[i].free)
					++ent_count;
			};
			
			mpConsole->Printf("%3i fps -- host(%3.0f) sv(%3.0f) cl(%3.0f) gfx(%3.0f) "
			           "snd(%3.0f) ents(%d)\n",
			           (int)fps,
			           pass1,
			           pass2,
			           pass3,
			           pass4,
			           pass5,
			           ent_count);
		}

#ifndef SWDS
		/*
		if(cl_gg.value != 0.0f)
		{
			// sub_1D10B2D
			CL_GGSpeeds(time[3]);
		}
		*/
#endif // SWDS
};

/*
==================
Host_UpdateScreen

Refresh the screen
==================
*/
void CHost::UpdateScreen()
{
	//if(!gfBackground)
	{
		mpScreen->Update();
		
		//if(cl_inmovie)
		{
			//if(*(float *)&scr_con_current == 0.0f)
				//VID_WriteBuffer(NULL);
		};
	};
};

/*
==================
Host_UpdateSounds

Update sound subsystem and cd audio
==================
*/
void CHost::UpdateSounds()
{
	//if(!gfBackground)
	{
		// S_PrintStats();
	};

	/*
#if defined(_WIN32) && !defined(SWDS)
	// update audio
	if(cl.IsActive()) // if(cls.state == ca_active)
		mpSound->Update(&s_AudioState); // S_Update(r_origin, vpn, vright, vup);
	else
		mpSound->Update(NULL); // S_Update(vec3_origin, vec3_origin, vec3_origin, vec3_origin);
#endif
  */
};

void CHost::CheckConnectionFailure()
{
	static int frames = 5;
	
	//if(cls.state == ca_disconnected && (giSubState & 4 || console.value == 0.0f))
	{
		if(frames-- > 0)
			return;

		//giActive = DLL_PAUSED;
		frames = 5;
	};
};

/*
==================
Host_Frame

Runs all active servers
==================
*/
void CHost::_Frame(float time)
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
	
/*
	//R_SetStackBase();
	CL_CheckClientState();

	// process console commands
	mpCmdBuffer->Execute();

	ClientDLL_UpdateClientData();

	if(mpServer->IsActive())
		CL_Move();
*/

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

	CL_UpdateSoundFade();
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

int CHost::Frame(float time, int iState, int *stateInfo)
{
	double time1;
	double time2;
/*
	// something bad happened, or the server disconnected
	if(setjmp(host_abortserver))
		return giActive;

	if(giActive != 3 || !g_iQuitCommandIssued)
		giActive = iState;

	*stateInfo = 0;
*/

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
		mpCmdBuffer->Execute();
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
			//for(int i = 0; i < g_psvs.maxclients; i++)
			{
				//if(g_psvs.clients[i].active)
					//++c;
			};

			mpConsole->Printf("host_profile: %2i clients %2i msec\n", c, m);
		};
	};

	return 1; //giActive;
};

void CHost::CheckGore()
{
	//float fValue = bLowViolenceBuild ? 0.0f : 1.0f;
	
	//Cvar_SetValue("violence_hblood", fValue);
	//Cvar_SetValue("violence_hgibs", fValue);
	//Cvar_SetValue("violence_ablood", fValue);
	//Cvar_SetValue("violence_agibs", fValue);
};

bool CHost::IsSinglePlayerGame()
{
	// it's better to replace this with host.maxclients
	// it will work the same
	// currently we check that local server is active
	// if so, we get the maxclients from there
	// otherwise, we're connected to some remote server
	// so we get our stored maxclients which we got from
	// that server
	
	/*
	if(mpServer->IsActive())
		return mpServer->GetMaxClients() == 1;
	else
		return mpLocalClient->GetServerData()->GetMaxClients() == 1;
	*/
	
	//mnMaxPlayers
	return mnMaxClients == 1;
};

bool CHost::IsServerActive() // remove?
{
	return false; //mpServer->IsActive();
};

void CHost::PrintVersion()
{
	char szFileName[MAX_PATH];
	
	Q_strcpy(szFileName, "steam.inf");
	
	Q_strcpy(gpszVersionString, "1.0.1.4");
	Q_strcpy(gpszProductString, "valve");
	
	CFile *fp = mpFileSystem->Open(szFileName, "r");
	
	if(fp)
	{
		int bufsize = fp->GetSize();
		char *buffer = nullptr; //(char *)Mem_Malloc(bufsize + 1);
		
		fp->Read(buffer, bufsize, 1);
		
		char *pbuf = buffer;
		
		mpFileSystem->Close(fp);
		
		buffer[bufsize] = 0;
		int gotKeys = 0;

		//pbuf = COM_Parse(pbuf);
		
		if(pbuf)
		{
			while(Q_strlen(com_token) > 0 && gotKeys <= 1)
			{
				if(!Q_strnicmp(com_token, "PatchVersion=", Q_strlen("PatchVersion=")))
				{
					Q_strncpy(gpszVersionString, &com_token[Q_strlen("PatchVersion=")], sizeof(gpszVersionString));
					gpszVersionString[sizeof(gpszVersionString) - 1] = 0;
					
					if(mpCmdLine->CheckArg("-steam"))
					{
						char szSteamVersionId[32];
						mpFileSystem->GetInterfaceVersion(szSteamVersionId, sizeof(szSteamVersionId) - 1);
						Q_snprintf(gpszVersionString, sizeof(gpszVersionString), "%s/%s", &com_token[Q_strlen("PatchVersion=")], szSteamVersionId);
						gpszVersionString[sizeof(gpszVersionString) - 1] = 0;
					};
					
					++gotKeys;
				}
				else if(!Q_strnicmp(com_token, "ProductName=", Q_strlen("ProductName=")))
				{
					++gotKeys;
					Q_strncpy(gpszProductString, &com_token[Q_strlen("ProductName=")], sizeof(gpszProductString) - 1);
					gpszProductString[sizeof(gpszProductString) - 1] = 0;
				};

				//pbuf = COM_Parse(pbuf);
				
				if(!pbuf)
					break;
			};
		};
		
		//if(buffer)
			//Mem_Free(buffer);
	};

	//if(cls.state != ca_dedicated)
	{
		//mpConsole->DevPrintf("Protocol version %i\nExe version %s (%s)\n", PROTOCOL_VERSION, gpszVersionString, gpszProductString);
		//mpConsole->DevPrintf("Exe build: " __BUILD_TIME__ " " __BUILD_DATE__ " (%i)\n", build_number());
	}
	//else
	{
		mpConsole->Printf("Protocol version %i\nExe version %s (%s)\n", PROTOCOL_VERSION, gpszVersionString, gpszProductString);
		mpConsole->Printf("Exe build: " __BUILD_TIME__ " " __BUILD_DATE__ " (%i)\n", build_number());
	};
};

int CHost::GetStartTime()
{
	return startTime;
};

void CHost::InitGame()
{
	//mpCmdBuffer->Execute();
	mpNetwork->Config(false); //g_psvs.maxclients > 1

	//if(g_psvs.dll_initialized)
	{
		mpConsole->DevPrintf("Sys_InitializeGameDLL called twice, skipping second call\n");
		return;
	};

#ifndef OGS_STATIC_GAME
	//mpFileParser->ParseFile("liblist.gam");
	mpGame = mpGameLoaderHandler->LoadGame(/*GetGamePath()*/);
	//CLegacyGame *pOldAPIGame = new HLGame();
	
	//LoadEntityDLLs(host_parms.basedir);
#else
	#error "Static game linkage is not supported!"
#endif // OGS_STATIC_GAME
	
	// Initialize the game
	if(mpGame)
		mpGame->Init();
	
	//pOldAPIGame->Init();
	
	// if we initializing it here then it's init state shouldn't be contained in server static data
	//g_psvs.dll_initialized = TRUE;
	
	//gEntityInterface.pfnGameInit();
	//gEntityInterface.pfnPM_Init(&g_svmove);
	//gEntityInterface.pfnRegisterEncoders();

	//SV_InitEncoders();
	//SV_GetPlayerHulls();
	//SV_CheckBlendingInterface();
	//SV_CheckSaveGameCommentInterface();
	
	//mpCmdBuffer->Execute();
	
#ifdef REHLDS_FIXES // DONE: Set cstrike flags on server start
	SetCStrikeFlags();
#endif
};

void CHost::ShutdownGame()
{
	// Shutdown the game
	if(mpGame)
		mpGame->Shutdown();

#ifndef OGS_STATIC_GAME
	if ( gameDLL )
	{
		Sys_DLL_Unload( gameDLL );
		gameDLL = nullptr;
	};
	
	mpGame = nullptr;
#endif
};

void CHost::UpdateStats()
{
	uint32 runticks = 0;
	uint32 cputicks = 0;

	static float last = 0.0f;
	static float lastAvg = 0.0f;

	static uint64 lastcputicks = 0;
	static uint64 lastrunticks = 0;

#ifdef _WIN32
/*
	struct _FILETIME ExitTime;
	struct _FILETIME UserTime;
	struct _FILETIME KernelTime;
	struct _FILETIME CreationTime;
	struct _FILETIME SystemTimeAsFileTime;

	if(!startTime)
		startTime = CSystem::GetFloatTime();

	if(last + 1.0f < CSystem::GetFloatTime())
	{
		GetProcessTimes(GetCurrentProcess(), &CreationTime, &ExitTime, &KernelTime, &UserTime);
		GetSystemTimeAsFileTime(&SystemTimeAsFileTime);

		//CRehldsPlatformHolder::get()->GetProcessTimes(GetCurrentProcess(), &CreationTime, &ExitTime, &KernelTime, &UserTime);
		//CRehldsPlatformHolder::get()->GetSystemTimeAsFileTime(&SystemTimeAsFileTime);

		if(!lastcputicks)
		{
			cputicks = CreationTime.dwLowDateTime;
			runticks = CreationTime.dwHighDateTime;

			lastcputicks = FILETIME_TO_QWORD(CreationTime);
		}
		else
		{
			cputicks = (uint32)(lastcputicks & 0xFFFFFFFF);
			runticks = (uint32)(lastcputicks >> 32);
		};

		cpuPercent = (FILETIME_TO_QWORD(UserTime) + FILETIME_TO_QWORD(KernelTime) -
		              lastrunticks) / (FILETIME_TO_QWORD(SystemTimeAsFileTime) - (double)FILETIME_TO_PAIR(runticks, cputicks));

		if(lastAvg + 5.0f < CSystem::GetFloatTime())
		{
			lastcputicks = FILETIME_TO_QWORD(SystemTimeAsFileTime);
			lastrunticks = FILETIME_TO_QWORD(UserTime) + FILETIME_TO_QWORD(KernelTime);
			lastAvg = last;
		};
		
		last = CSystem::GetFloatTime();
	};
*/
#else // _WIN32

	FILE *pFile;
	int32 dummy;
	int32 ctime;
	int32 stime;
	int32 start_time;
	char statFile[4096];
	struct sysinfo infos;

	if(!startTime)
		startTime = CSystem::GetFloatTime();

	if(CSystem::GetFloatTime() > last + 1.0f)
	{
		time(NULL);
		pid_t pid = getpid();
		Q_snprintf(statFile, sizeof(statFile), "/proc/%i/stat", pid);
		
		pFile = fopen(statFile, "rt");
		
		if(!pFile)
		{
			last = CSystem::GetFloatTime();
			return;
		};
		
		sysinfo(&infos);
		fscanf(pFile, "%d %s %c %d %d %d %d %d %lu %lu \t\t\t%lu %lu %lu %ld %ld "
		              "%ld %ld %ld %ld %lu \t\t\t%lu %ld %lu %lu %lu %lu %lu %lu "
		              "%lu %lu \t\t\t%lu %lu %lu %lu %lu %lu",
		       &dummy,
		       statFile,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &ctime,
		       &stime,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &start_time,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy,
		       &dummy);
		fclose(pFile);

		runticks = 100 * infos.uptime - start_time;
		cputicks = ctime + stime;

		if(!lastcputicks)
			lastcputicks = cputicks;

		if(lastrunticks)
			cpuPercent = (double)(cputicks - lastcputicks) / (double)(runticks - lastrunticks);
		else
			lastrunticks = runticks;

		if(lastAvg + 5.0f < CSystem::GetFloatTime())
		{
			lastcputicks = cputicks;
			lastrunticks = runticks;
			
			lastAvg = CSystem::GetFloatTime();
		};
		
		if(cpuPercent > 0.999f)
			cpuPercent = 0.999f;
		else if(cpuPercent < 0.0f)
			cpuPercent = 0.0f;
		
		last = CSystem::GetFloatTime();
	};

#endif // _WIN32
};

void CHost::ClearIOStates()
{
#ifndef SWDS
	//for(int i = 0; i < 256; ++i)
		//Key_Event(i, false);
	
	//Key_ClearStates();
	//ClientDLL_ClearStates();
#endif // SWDS
};

/*
==================
Host_InitCommands
==================
*/
void CHost::InitCommands()
{
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
	//Cmd_AddCommand("cd", CD_Command_f);
	//Cmd_AddCommand("mp3", MP3_Command_f);
	//Cmd_AddCommand("_careeraudio", CareerAudio_Command_f);
#endif // SWDS

/*
	Cmd_AddCommand("shutdownserver", Host_KillServer_f);
	Cmd_AddCommand("soundfade", Host_Soundfade_f);
	Cmd_AddCommand("status", Host_Status_f);
	Cmd_AddCommand("stat", Host_Status_Formatted_f);
	Cmd_AddCommand("quit", Host_Quit_f);
	Cmd_AddCommand("_restart", Host_Quit_Restart_f);

#ifndef SWDS
	//Cmd_AddCommand("_setrenderer", Host_SetRenderer_f);
	//Cmd_AddCommand("_setvideomode", Host_SetVideoMode_f);
	//Cmd_AddCommand("_setgamedir", Host_SetGameDir_f);
	Cmd_AddCommand("_sethdmodels", Host_SetHDModels_f);
	Cmd_AddCommand("_setaddons_folder", Host_SetAddonsFolder_f);
	Cmd_AddCommand("_set_vid_level", Host_SetVideoLevel_f);
#endif // SWDS

	Cmd_AddCommand("exit", Host_Quit_f);
	Cmd_AddCommand("map", Host_Map_f); ([this](){Map_f();})
	Cmd_AddCommand("career", Host_Career_f);
	Cmd_AddCommand("maps", Host_Maps_f);
	Cmd_AddCommand("restart", Host_Restart_f);
	Cmd_AddCommand("reload", Host_Reload_f);

	Cmd_AddCommand("changelevel", Host_Changelevel_f);
	Cmd_AddCommand("changelevel2", Host_Changelevel2_f);

	Cmd_AddCommand("reconnect", Host_Reconnect_f);
	Cmd_AddCommand("version", Host_Version_f);
	Cmd_AddCommand("say", Host_Say_f);
	Cmd_AddCommand("say_team", Host_Say_Team_f);
	Cmd_AddCommand("tell", Host_Tell_f);
	Cmd_AddCommand("kill", Host_Kill_f);
	Cmd_AddCommand("pause", Host_TogglePause_f);
	Cmd_AddCommand("setpause", Host_Pause_f);
	Cmd_AddCommand("unpause", Host_Unpause_f);
	Cmd_AddCommand("kick", Host_Kick_f);
	Cmd_AddCommand("ping", Host_Ping_f);
	Cmd_AddCommand("motd", Host_Motd_f);
	Cmd_AddCommand("motd_write", Host_Motd_Write_f);
	Cmd_AddCommand("stats", Host_Stats_f);
	Cmd_AddCommand("load", Host_Loadgame_f);
	Cmd_AddCommand("save", Host_Savegame_f);
	Cmd_AddCommand("autosave", Host_AutoSave_f);
	Cmd_AddCommand("writecfg", Host_WriteCustomConfig);

#ifndef SWDS
	Cmd_AddCommand("+voicerecord", Host_VoiceRecordStart_f);
	Cmd_AddCommand("-voicerecord", Host_VoiceRecordStop_f);
#endif // SWDS

	Cmd_AddCommand("startdemos", Host_Startdemos_f);
	Cmd_AddCommand("demos", Host_Demos_f);
	Cmd_AddCommand("stopdemo", Host_Stopdemo_f);
	
	Cmd_AddCommand("setinfo", Host_SetInfo_f);
	Cmd_AddCommand("fullinfo", Host_FullInfo_f);

#ifndef SWDS
	Cmd_AddCommand("god", Host_God_f);
	Cmd_AddCommand("notarget", Host_Notarget_f);
	Cmd_AddCommand("fly", Host_Fly_f);
	Cmd_AddCommand("noclip", Host_Noclip_f);
	
	Cmd_AddCommand("viewmodel", Host_Viewmodel_f);
	Cmd_AddCommand("viewframe", Host_Viewframe_f);
	Cmd_AddCommand("viewnext", Host_Viewnext_f);
	Cmd_AddCommand("viewprev", Host_Viewprev_f);
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