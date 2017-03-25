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

#include "precompiled.hpp"
#include "system/Host.hpp"
#include "system/common.hpp"
#include "system/System.hpp"

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
/*
	CRehldsPlatformHolder::get()->srand(CRehldsPlatformHolder::get()->time(NULL));

	Q_memcpy(&host_parms, parms, sizeof(host_parms));
	com_argc = parms->argc;
	com_argv = parms->argv;
	realtime = 0;

	Memory_Init(parms->membase, parms->memsize);

	Voice_RegisterCvars();
	Cvar_RegisterVariable(&console);

	if(COM_CheckParm("-console") || COM_CheckParm("-toconsole") || COM_CheckParm("-dev"))
		Cvar_DirectSet(&console, "1.0");

	InitLocal();

	if(COM_CheckParm("-dev"))
		Cvar_SetValue("developer", 1.0);

// Engine string pooling
#ifdef REHLDS_FIXES
	Ed_StrPool_Init();
#endif // REHLDS_FIXES

	//FR_Init(); // don't put it under REHLDS_FLIGHT_REC to allow recording via Rehlds API

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
	W_LoadWadFile("gfx.wad");
	W_LoadWadFile("fonts.wad");
	Key_Init();
	mpConsole->Init();
	Decal_Init();
	Mod_Init();

	NET_Init();
	Netchan_Init();

	DELTA_Init();

	SV_Init();

	// SystemWrapper_Init();
	
	PrintVersion();

	// Rehlds Security
	//Rehlds_Security_Init();
	
	char versionString[256];
	Q_snprintf(versionString, sizeof(versionString), "%s,%i,%i", gpszVersionString, PROTOCOL_VERSION, build_number());
	Cvar_Set("sv_version", versionString);
	
	mpConsole->DPrintf("%4.1f Mb heap\n", (double)parms->memsize / (1024.0f * 1024.0f));
	
	//R_InitTextures();
	
	HPAK_CheckIntegrity("custom");
	
	Q_memset(&g_module, 0, sizeof(g_module));
	
	if(cls.state != ca_dedicated)
	{
		// CSystem::Error("Only dedicated server mode is supported");

		color24 *disk_basepal = (color24 *)COM_LoadHunkFile("gfx/palette.lmp");
		
		if(!disk_basepal)
			CSystem::Error("Host_Init: Couldn't load gfx/palette.lmp");

		host_basepal = (unsigned short *)Hunk_AllocName(sizeof(PackedColorVec) * 256, "palette.lmp");
		
		for(int i = 0; i < 256; i++)
		{
			PackedColorVec *basepal = (PackedColorVec *)&host_basepal[i];

			basepal->b = disk_basepal->r;
			basepal->g = disk_basepal->g;
			basepal->r = disk_basepal->b;
			basepal->a = 0; // alpha

			disk_basepal++;
		};
		
		//if(!LoadRender())
		//	return 0;
		//GL_Init(); gpRender->Init();
		
		PM_Init(&g_clmove);
		CL_InitEventSystem();
		ClientDLL_Init();
		// VGui_Startup();

		if(!VID_Init(host_basepal))
		{
			// VGui_Shutdown();
			return 0;
		};

		// IN_Init ();
		//Draw_Init();
		SCR_Init();
		//R_Init();
		S_Init();
		// CDAudio_Init();
		// Voice_Init("voice_speex", 1);
		// DemoPlayer_Init();
		// cls.state = ca_disconnected;
		// Sbar_Init ();
		CL_Init();
	}
	else
		mpConsole->RegisterVariable(&suitvolume);

	mpCmdBuffer->InsertText("exec valve.rc\n");
	
	Hunk_AllocName(0, "-HOST_HUNKLEVEL-");
	host_hunklevel = Hunk_LowMark();
	
	giActive = DLL_ACTIVE;
	scr_skipupdate = FALSE;
*/
	
	CheckGore();
	
	host_initialized = true;
	return 1;
};

void CHost::InitLocal()
{
	/*
	InitCommands();

	mpConsole->RegisterVariable(&host_killtime);
	mpConsole->RegisterVariable(&sys_ticrate);
	mpConsole->RegisterVariable(&fps_max);
	mpConsole->RegisterVariable(&fps_override);
	mpConsole->RegisterVariable(&host_name);
	mpConsole->RegisterVariable(&host_limitlocal);

	sys_timescale.value = 1.0f;

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
	
	if(host_initialized) // Client-side
		WriteConfig();

	//SV_ServerShutdown(); // Deactivate
	//Voice_Deinit();
	
	host_initialized = false;
/*
	// CDAudio_Shutdown();
	// VGui_Shutdown();
	
	if(cls.state != ca_dedicated)
		ClientDLL_Shutdown();

	// Rehlds Security
	//Rehlds_Security_Shutdown();

	Cmd_RemoveGameCmds();
	Cmd_Shutdown();
	Cvar_Shutdown();
	
	HPAK_FlushHostQueue();
	
	SV_DeallocateDynamicData();

	client_t *pclient = g_psvs.clients;
	
	for(int i = 0; i < g_psvs.maxclientslimit; i++, pclient++)
		SV_ClearFrames(&pclient->frames);

	mpServer->Shutdown();
	
	// SystemWrapper_ShutDown();
	
	mpNetwork->Shutdown();
	mpSound->Shutdown();
	mpConsole->Shutdown();
	
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

void CHost::EndGame(const char *message, ...)
{
	/*
	NOXREFCHECK;
	
	int oldn;
	va_list argptr;
	char string[1024];

	va_start(argptr, message);
	Q_vsnprintf(string, sizeof(string), message, argptr);
	va_end(argptr);

	mpConsole->DPrintf("%s: %s\n", __FUNCTION__, string);

	oldn = cls.demonum;

	if(g_psv.active)
		ShutdownServer(false);

	cls.demonum = oldn;

	if(!cls.state)
		CSystem::Error("%s: %s\n", __FUNCTION__, string);

	if(oldn != -1)
	{
		CL_Disconnect_f();
		cls.demonum = oldn;
		Host_NextDemo();
		longjmp(host_enddemo, 1);
	};

	CL_Disconnect();
	mpCmdBuffer->AddText("cd stop\n");
	mpCmdBuffer->Execute();
	longjmp(host_abortserver, 1);*/
};

void NORETURN CHost::Error(const char *error, ...)
{
	static bool inerror = false;

	if(inerror)
		Sys_Error("%s: recursively entered", __FUNCTION__);

	inerror = true;
	
	va_list argptr;
	va_start(argptr, error);
	
	//SCR_EndLoadingPlaque();
	
	char string[1024];
	Q_vsnprintf(string, sizeof(string), error, argptr);
	va_end(argptr);

	//if(g_psv.active && developer.value != 0.0f)
		//CL_WriteMessageHistory(0, 0);

	//mpConsole->Printf("%s: %s\n", __FUNCTION__, string);
	
	//if(g_psv.active)
		//ShutdownServer(false);

	//if(cls.state)
	{
		//CL_Disconnect();
		//cls.demonum = -1;
		inerror = false;
		//longjmp(host_abortserver, 1);
	};
	
	Sys_Error("%s: %s\n", __FUNCTION__, string);
};

void CHost::WriteConfig()
{
#ifndef SWDS
	FILE *f;
	kbutton_t *ml;
	kbutton_t *jl;
	char nameBuf[4096];

	if(!host_initialized || cls.state == ca_dedicated)
		return;

#ifdef _WIN32
	Sys_GetRegKeyValue("Software\\Valve\\Steam", "rate", rate_.string);
	if(cl_name.string && Q_stricmp(cl_name.string, "unnamed") &&
	   Q_stricmp(cl_name.string, "player") && Q_strlen(cl_name.string))
		Sys_GetRegKeyValue("Software\\Valve\\Steam", "LastGameNameUsed", cl_name.string);
#else
	SetRateRegistrySetting(rate_.string);
#endif // _WIN32
	
	if(Key_CountBindings() <= 1)
	{
		mpConsole->Printf("skipping config.cfg output, no keys bound\n");
		return;
	};

	bool bSetFileToReadOnly = false;
	f = FS_OpenPathID("config.cfg", "w", "GAMECONFIG");
	if(!f)
	{
		if(!developer.value || !FS_FileExists("../goldsrc/dev_build_all.bat"))
		{
			if(FS_GetLocalPath("config.cfg", nameBuf, sizeof(nameBuf)))
			{
				bSetFileToReadOnly = true;
				chmod(nameBuf, S_IREAD | S_IWRITE);
			}
			f = FS_OpenPathID("config.cfg", "w", "GAMECONFIG");
			if(!f)
			{
				mpConsole->Printf("Couldn't write config.cfg.\n");
				return;
			}
		}
	}

	FS_FPrintf(f, "// This file is overwritten whenever you change your user "
	              "settings in the game.\n");
	FS_FPrintf(
	f, "// Add custom configurations to the file \"userconfig.cfg\".\n\n");
	FS_FPrintf(f, "unbindall\n");

	Key_WriteBindings(f);
	Cvar_WriteVariables(f);
	Info_WriteVars(f);

	ml = ClientDLL_FindKey("in_mlook");
	jl = ClientDLL_FindKey("in_jlook");

	if(ml && (ml->state & 1))
		FS_FPrintf(f, "+mlook\n");

	if(jl && (jl->state & 1))
		FS_FPrintf(f, "+jlook\n");

	FS_FPrintf(f, "exec userconfig.cfg\n");
	FS_Close(f);

	if(bSetFileToReadOnly)
	{
		FS_GetLocalPath("config.cfg", nameBuf, sizeof(nameBuf));
		chmod(nameBuf, S_IREAD);
	};
#endif // SWDS
};

void CHost::WriteCustomConfig()
{
	/*
#ifndef SWDS
	FILE *f;
	kbutton_t *ml;
	kbutton_t *jl;
#endif
	
	char configname[261];
	Q_snprintf(configname, 257, "%s", Cmd_Args());
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
		if(host_initialized && cls.state != ca_dedicated)
		{
			if(Key_CountBindings() < 2)
				mpConsole->Printf("skipping config.cfg output, no keys bound\n");
			else
			{
				Q_strcat(configname, ".cfg");
				f = FS_OpenPathID(configname, "w", "GAMECONFIG");
				if(!f)
				{
					mpConsole->Printf("Couldn't write %s.\n", configname);
					return;
				}

				FS_FPrintf(f, "unbindall\n");
				Key_WriteBindings(f);
				Cvar_WriteVariables(f);
				Info_WriteVars(f);

				ml = ClientDLL_FindKey("in_mlook");
				jl = ClientDLL_FindKey("in_jlook");

				if(ml && ml->state & 1)
					FS_FPrintf(f, "+mlook\n");

				if(jl && jl->state & 1)
					FS_FPrintf(f, "+jlook\n");

				FS_Close(f);
				mpConsole->Printf("%s successfully created!\n", configname);
			}
		}
	}
#endif // SWDS
*/
};

void CHost::ClientCommands(const char *fmt, ...)
{
	/*
	va_list argptr;
	char string[1024];

	va_start(argptr, fmt);
	
	if(!host_client->fakeclient)
	{
		Q_vsnprintf(string, sizeof(string), fmt, argptr);
		string[sizeof(string) - 1] = 0;

		MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
		MSG_WriteString(&host_client->netchan.message, string);
	};
	
	va_end(argptr);
	*/
};

void CHost::ClearClients(bool bFramesOnly)
{
	/*
	int i;
	int j;
	client_frame_t *frame;
	netadr_t save;

	host_client = g_psvs.clients;
	for(i = 0; i < g_psvs.maxclients; i++, host_client++)
	{
		if(host_client->frames)
		{
			for(j = 0; j < SV_UPDATE_BACKUP; j++)
			{
				frame = &(host_client->frames[j]);
				SV_ClearPacketEntities(frame);
				frame->senttime = 0;
				frame->ping_time = -1;
			}
		}
		if(host_client->netchan.remote_address.type)
		{
			Q_memcpy(&save, &host_client->netchan.remote_address, sizeof(netadr_t));
			Q_memset(&host_client->netchan, 0, sizeof(netchan_t));
			Netchan_Setup(NS_SERVER, &host_client->netchan, save, host_client - g_psvs.clients, (void *)host_client, SV_GetFragmentSize);
		};
		
		COM_ClearCustomizationList(&host_client->customdata, 0);
	};

	if(bFramesOnly == FALSE)
	{
		host_client = g_psvs.clients;
		for(i = 0; i < g_psvs.maxclientslimit; i++, host_client++)
			SV_ClearFrames(&host_client->frames);

		Q_memset(g_psvs.clients, 0, sizeof(client_t) * g_psvs.maxclientslimit);
		SV_AllocClientFrames();
	};
	*/
};

void CHost::ShutdownServer(bool crash)
{
	/*
	int i;
	if(!g_psv.active)
		return;

	SV_ServerShutdown();
	g_psv.active = FALSE;
	NET_ClearLagData(TRUE, TRUE);

	host_client = g_psvs.clients;
	for(i = 0; i < g_psvs.maxclients; i++, host_client++)
	{
		if(host_client->active || host_client->connected)
			SV_DropClient(host_client, crash, "Server shutting down");
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

void CHost::CheckDynamicStructures()
{
	/*
	client_t *cl = g_psvs.clients;

	for(int i = 0; i < g_psvs.maxclients; i++, cl++)
	{
		if(cl->frames)
			SV_ClearFrames(&cl->frames);
	};
	*/
};

void CHost::ClearMemory(bool bQuiet)
{
	// Engine string pooling
#ifdef REHLDS_FIXES
	Ed_StrPool_Reset();
#endif // REHLDS_FIXES

/*
	CM_FreePAS();
	SV_ClearEntities();

	if(!bQuiet)
		mpConsole->DPrintf("Clearing memory\n");

	//D_FlushCaches();
	Mod_ClearAll();
	
	if(host_hunklevel)
	{
		CheckDynamicStructures();
		Hunk_FreeToLowMark(host_hunklevel);
	};

	cls.signon = 0;
	SV_ClearCaches();
	Q_memset(&g_psv, 0, sizeof(server_t));
	CL_ClearClientState();
	SV_ClearClientStates();
*/
};

bool CHost::FilterTime(float time)
{
	float fps;
	static int command_line_ticrate = -1;

	/*
	if(host_framerate.value > 0.0f)
	{
		if(IsSinglePlayerGame() || cls.demoplayback)
		{
			host_frametime = sys_timescale.value * host_framerate.value;
			realtime += host_frametime;
			return true;
		};
	};

	realtime += sys_timescale.value * time;

	if(gbIsDedicatedServer)
	{
		if(command_line_ticrate == -1)
			command_line_ticrate = COM_CheckParm("-sys_ticrate");

		if(command_line_ticrate > 0)
			fps = Q_atof(com_argv[command_line_ticrate + 1]);
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

		if(g_psv.active || cls.state == ca_disconnected || cls.state == ca_active)
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
	};
	*/

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
	//SV_CountPlayers(&clients);
	*nActive = clients;

	if(unused)
		*unused = 0;

	if(pszMap)
	{
		//if(g_psv.name[0])
			//Q_strcpy(pszMap, g_psv.name);
		//else
			//*pszMap = 0;

		strcpy(pszMap, "nowhere");
	};

	*nMaxPlayers = 0; //g_psvs.maxclients;
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
			}
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
	/*
	if(!gfBackground)
	{
		SCR_UpdateScreen();
		
		if(cl_inmovie)
		{
			if(*(float *)&scr_con_current == 0.0f)
				VID_WriteBuffer(NULL);
		}
	}
	*/
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
		S_Update(&s_AudioState); // S_Update(r_origin, vpn, vright, vup);
	else
		S_Update(NULL); // S_Update(vec3_origin, vec3_origin, vec3_origin, vec3_origin);
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

	if(g_psv.active)
		CL_Move();

	host_times[1] = CSystem::FloatTime();
	
	mpServer->Frame(host_frametime); // netserver or gameserver (works as netserver->gameserver) frame

	host_times[2] = CSystem::FloatTime();
	
	mpServer->CheckForRcon();

	if(!g_psv.active)
		CL_Move();

	ClientDLL_Frame(host_frametime);
	CL_SetLastUpdate();

	// fetch results from server
	CL_ReadPackets();
	
	// wipe all the local states that are older than the latest state 
	// received from the server and reapply the rest above it
	CL_RedoPrediction();
	
	CL_VoiceIdle();
	
	// build a refresh entity list
	CL_EmitEntities();
	
	CL_CheckForResend();

	while(CL_RequestMissingResources())
		;

	CL_UpdateSoundFade();
	
	CheckConnectionFailure();
	
	// CL_HTTPUpdate();
	Steam_ClientRunFrame();
	ClientDLL_CAM_Think();
	CL_MoveSpectatorCamera();

	host_times[3] = CSystem::FloatTime();
*/
	UpdateScreen();

	//host_times[4] = CSystem::FloatTime();

	//CL_DecayLights();

	UpdateSounds();
	
	//CDAudio_Update(); // call here or inside the UpdateSounds method

	//host_times[0] = host_times[5];
	//host_times[5] = CSystem::FloatTime();

	PrintSpeeds(host_times);

	++host_framecount;

	/*
	CL_AdjustClock();

	if(sv_stats.value == 1.0f)
		UpdateStats();

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
	
	if(host_profile.value != 0.0f)
		time1 = Sys_FloatTime();
*/

	_Frame(time);
	
/*
	if(host_profile.value != 0.0)
		time2 = Sys_FloatTime();

	if(giStateInfo)
	{
		*stateInfo = giStateInfo;
		giStateInfo = 0;
		mpCmdBuffer->Execute();
	};

	if(host_profile.value != 0.0)
	{
		static double timetotal;
		static int timecount;

		timecount++;
		timetotal += time2 - time1;
		if(++timecount >= 1000)
		{
			int m = (timetotal * 1000.0 / (double)timecount);
			int c = 0;
			timecount = 0;
			timetotal = 0.0;
			for(int i = 0; i < g_psvs.maxclients; i++)
			{
				if(g_psvs.clients[i].active)
					++c;
			};

			mpConsole->Printf("host_profile: %2i clients %2i msec\n", c, m);
		};
	};
	*/
	return 0; //giActive;
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
	/*
	if(g_psv.active)
		return g_psvs.maxclients == 1;
	else
		return cl.maxclients == 1;
	*/
	return false;
};

bool CHost::IsServerActive()
{
	return false; //g_psv.active;
};

void CHost::PrintVersion()
{
	char szFileName[MAX_PATH];
	
	Q_strcpy(szFileName, "steam.inf");
	
	//Q_strcpy(gpszVersionString, "1.0.1.4");
	//Q_strcpy(gpszProductString, "valve");
	
/*	
	FileHandle_t fp = mpFS->Open(szFileName, "r");
	
	if(fp)
	{
		int bufsize = mpFS->Size(fp);
		char *buffer = (char *)Mem_Malloc(bufsize + 1);
		
		mpFS->Read(buffer, bufsize, 1, fp);
		
		char *pbuf = buffer;
		
		mpFS->Close(fp);
		
		buffer[bufsize] = 0;
		int gotKeys = 0;

		pbuf = COM_Parse(pbuf);
		if(pbuf)
		{
			while(Q_strlen(com_token) > 0 && gotKeys <= 1)
			{
				if(!Q_strnicmp(com_token, "PatchVersion=", Q_strlen("PatchVersion=")))
				{
					Q_strncpy(gpszVersionString, &com_token[Q_strlen("PatchVersion=")], sizeof(gpszVersionString));
					gpszVersionString[sizeof(gpszVersionString) - 1] = 0;
					if(COM_CheckParm("-steam"))
					{
						char szSteamVersionId[32];
						FS_GetInterfaceVersion(szSteamVersionId,
						                       sizeof(szSteamVersionId) - 1);
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

				pbuf = COM_Parse(pbuf);
				if(!pbuf)
					break;
			};
		};
		
		if(buffer)
			Mem_Free(buffer);
	};

	if(cls.state != ca_dedicated)
	{
		mpConsole->DPrintf("Protocol version %i\nExe version %s (%s)\n", PROTOCOL_VERSION, gpszVersionString, gpszProductString);
		mpConsole->DPrintf("Exe build: " __BUILD_TIME__ " " __BUILD_DATE__ " (%i)\n", build_number());
	}
	else
	{
		mpConsole->Printf("Protocol version %i\nExe version %s (%s)\n", PROTOCOL_VERSION, gpszVersionString, gpszProductString);
		mpConsole->Printf("Exe build: " __BUILD_TIME__ " " __BUILD_DATE__ " (%i)\n", build_number());
	};
	*/
};

void CHost::ClearIOStates()
{
#ifndef SWDS
	for(int i = 0; i < 256; ++i)
		Key_Event(i, false);
	
	Key_ClearStates();
	ClientDLL_ClearStates();
#endif // SWDS
};