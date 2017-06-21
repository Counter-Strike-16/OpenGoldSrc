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


CHost::CHost(CFileSystem *apFileSystem) : mpFileSystem(apFileSystem){}

CHost::~CHost() = default;

/*
====================
Host_Init
====================
*/
int CHost::Init(quakeparms_t *parms)
{
	

	
	
};



void CHost::Shutdown()
{
	
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
	// Should we disable the screen if we want to draw some animations on the screen during load?
	scr_disabled_for_loading = true; // mpScreen->SetActive(false);?
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
	mpCmdProcessor->AddCmdText("cd stop\n");
	mpCmdProcessor->ExecCmdBuffer();
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
				mpKeyBindingList->DumpToFile(f); //Key_WriteBindings(f);
				mpConVarList->DumpToFile(f); //Cvar_WriteVariables(f);
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

//void CGameServer::SendStuffText(IGameClient *apClient, const char *asMsg, ...)
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

void CHost::ComputeFPS(double frametime)
{
	rolling_fps = 0.6f * rolling_fps + 0.4f * frametime;
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

void CHost::CheckGore()
{
	//float fValue = bLowViolenceBuild ? 0.0f : 1.0f;
	
	//mpConVarHandler->SetVarValue("violence_hblood", fValue);
	//mpConVarHandler->SetVarValue("violence_hgibs", fValue);
	//mpConVarHandler->SetVarValue("violence_ablood", fValue);
	//mpConVarHandler->SetVarValue("violence_agibs", fValue);
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
	mpCmdBuffer->Exec();
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
	
	//SV_InitEncoders();
	//SV_GetPlayerHulls();
	//SV_CheckBlendingInterface();
	//SV_CheckSaveGameCommentInterface();
	
	mpCmdProcessor->ExecCmdBuffer();
	
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