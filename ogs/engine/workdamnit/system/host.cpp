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

#include "precompiled.hpp"
#include "system/host.hpp"
#include "system/host_cmd.hpp"
#include "system/system.hpp"
#include "system/buildinfo.hpp"
#include "memory/mem.hpp"
#include "memory/zone.hpp"
#include "filesystem/filesystem_internal.hpp"
#include "filesystem/hashpak.hpp"
#include "filesystem/wad.hpp"
#include "resources/cmodel.hpp"
#include "resources/model_rehlds.hpp"
#include "resources/decal.hpp"
#include "network/net_msg.hpp"
#include "system/client.hpp"
#include "console/cmd.hpp"
#include "console/console.hpp"
#include "system/server.hpp"
#include "system/sv_log.hpp"
#include "network/sv_steam3.hpp"
#include "resources/sv_upld.hpp"
#include "input/keys.hpp"
#include "sound/sound.hpp"
#include "voice/voice.hpp"
#include "graphics/screen.hpp"
#include "graphics/vid.hpp"
#include "graphics/view.hpp"

CHost *gpHost = nullptr;

// unsigned short *host_basepal;
// int minimum_memory;
client_t *host_client;
qboolean gfNoMasterServer;
// qboolean g_bUsingInGameAdvertisements;


unsigned short *host_basepal;
// unsigned char *host_colormap;
// const char *g_InGameAdsAllowed[3];

NOXREF void Host_EndGame(const char *message, ...)
{
	
}

void NORETURN Host_Error(const char *error, ...)
{
	gpHost->Error(error);
};

void Host_InitLocal()
{
	gpHost->InitLocal();
}

NOXREF void Info_WriteVars(FileHandle_t fp)
{
	NOXREFCHECK;
	
	cvar_t *pcvar;
	char *s;
	char pkey[512];

	static char value[4][512];
	static int valueindex;

	char *o;

	valueindex = (valueindex + 1) % 4;
	s = &cls.userinfo[0];

	if(*s == '\\')
		s++;

	while(1)
	{
		o = pkey;
		while(*s != '\\')
		{
			if(!*s)
				return;
			*o++ = *s++;
		}

		*o = 0;
		s++;

		o = value[valueindex];

		while(*s != '\\' && *s)
		{
			if(!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		pcvar = Cvar_FindVar(pkey);
		if(!pcvar && pkey[0] != '*')
			FS_FPrintf(fp, "setinfo \"%s\" \"%s\"\n", pkey, value[valueindex]);

		if(!*s)
			return;
		s++;
	};
};

void Host_WriteConfiguration()
{
	gpHost->WriteConfig();
};

void Host_WriteCustomConfig()
{
	gpHost->WriteCustomConfig();
};

void Host_ClientCommands(const char *fmt, ...)
{
	gpHost->ClientComands(//todo);
};

void Host_ClearClients(qboolean bFramesOnly)
{
	gpHost->ClientClients(bFramesOnly);
};

void Host_ShutdownServer(qboolean crash)
{
	gpHost->ShutdownServer(crash);
};



void Host_CheckDyanmicStructures() // dynamic?
{
	
}

void Host_ClearMemory(qboolean bQuiet)
{
	gpHost->ClearMemory(bQuiet);
}

qboolean Host_FilterTime(float time)
{
	return gpHost->FilterTime(time);
}

qboolean Master_IsLanGame()
{
	return sv_lan.value != 0.0f;
}

void Master_Heartbeat_f()
{
	// Steam_ForceHeartbeat in move?
	CRehldsPlatformHolder::get()->SteamGameServer()->ForceHeartbeat();
}

void Host_ComputeFPS(double frametime)
{
	
}

void Host_GetHostInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap)
{
	
}

void Host_Speeds(double *time)
{
	
};


void Host_UpdateScreen()
{
	
}


void Host_UpdateSounds()
{
	
}

void Host_CheckConnectionFailure()
{
	
}


void _Host_Frame(float time)
{
	
}

int Host_Frame(float time, int iState, int *stateInfo)
{
	
}

void CheckGore()
{
	gpHost->CheckGore();
};

qboolean Host_IsSinglePlayerGame()
{
	
}

qboolean Host_IsServerActive()
{
	
}

void Host_Version()
{
	
};

int Host_Init(quakeparms_t *parms)
{
	
}

void Host_Shutdown()
{
	
};

typedef struct GameToAppIDMapItem_s
{
	unsigned int iAppID;
	const char *pGameDir;
} GameToAppIDMapItem_t;

GameToAppIDMapItem_t g_GameToAppIDMap[11] =
{
	0x0A, "cstrike",
	0x14, "tfc",
	0x1E, "dod",
	0x28, "dmc",
	0x32, "gearbox",
	0x3C, "ricochet",
	0x46, "valve",
	0x50, "czero",
	0x64, "czeror",
	0x82, "bshift",
	0x96, "cstrike_beta",
};

int GetGameAppID()
{
	char arg[260];
	char gd[260];

	COM_ParseDirectoryFromCmd("-game", gd, "valve");
	COM_FileBase(gd, arg);
	
	for(int i = 0; i < ARRAYSIZE(g_GameToAppIDMap); i++)
	{
		if(!Q_stricmp(g_GameToAppIDMap[i].pGameDir, arg))
			return g_GameToAppIDMap[i].iAppID;
	};

	return 70;
};

qboolean IsGameSubscribed(const char *gameName)
{
#ifdef _WIN32
	for(int i = 0; i < ARRAYSIZE(g_GameToAppIDMap); i++)
	{
		if(!Q_stricmp(g_GameToAppIDMap[i].pGameDir, gameName))
			return ISteamApps_BIsSubscribedApp(g_GameToAppIDMap[i].iAppID);
	}

	return ISteamApps_BIsSubscribedApp(70);
#else //_WIN32
	return 0;
#endif
}

NOXREF qboolean BIsValveGame()
{
	NOXREFCHECK;
	
	for(int i = 0; i < ARRAYSIZE(g_GameToAppIDMap); i++)
	{
		if(!Q_stricmp(g_GameToAppIDMap[i].pGameDir, com_gamedir))
			return TRUE;
	}
	return FALSE;
}

#if 0
/*
==================
Host_SimulationTime

This determines if enough time has passed to run a simulation frame
==================
*/
qboolean Host_SimulationTime(float time)
{
	float fps;

	if (oldrealtime > realtime)
		oldrealtime = 0;

	if (cl_maxfps.value)
		fps = max(30.0, Q_min(cl_maxfps.value, 72.0));
	else
		fps = max(30.0, Q_min(rate.value/80.0, 72.0));

	if (!cls.timedemo && (realtime + time) - oldrealtime < 1.0/fps)
		return false;			// framerate is too high
	
	return true;
};
#endif

/*
void Host_Frame(float time)
{
        static double time1 = 0;
        static double time2 = 0;
        static double time3 = 0;
        int           pass1, pass2, pass3;
        float         fps;
		
        if(setjmp(host_abort))
                return; 

        // decide the simulation time
        realtime += time;
        if(oldrealtime > realtime)
                oldrealtime = 0;

        if(cl_maxfps.value)
                fps = max(30.0, Q_min(cl_maxfps.value, 72.0));
        else
                fps = max(30.0, Q_min(rate.value / 80.0, 72.0));

        if(!cls.timedemo && realtime - oldrealtime < 1.0 / fps)
                return; // framerate is too high

        host_frametime = realtime - oldrealtime;
        oldrealtime    = realtime;
        if(host_frametime > 0.2)
                host_frametime = 0.2;

        // get new key events
        Sys_SendKeyEvents();

        // allow mice or other external controllers to add commands
        IN_Commands();

        Cbuf_Execute();

        CL_ReadPackets();

        // send intentions now
        // resend a connection request if necessary
        if(cls.state == ca_disconnected)
                CL_CheckForResend();
        else
                CL_SendCmd();

        // Set up prediction for other players
        CL_SetUpPlayerPrediction(false);

        // do client side motion prediction
        CL_PredictMove();

        // Set up prediction for other players
        CL_SetUpPlayerPrediction(true);

        
        CL_EmitEntities();

        // update video
        if(host_speeds.value)
                time1 = Sys_DoubleTime();

        SCR_UpdateScreen();

        if(host_speeds.value)
                time2 = Sys_DoubleTime();

        CDAudio_Update();

        if(host_speeds.value)
        {
                pass1 = (time1 - time3) * 1000;
                time3 = Sys_DoubleTime();
                pass2 = (time2 - time1) * 1000;
                pass3 = (time3 - time2) * 1000;
                Con_Printf("%3i tot %3i server %3i gfx %3i snd\n",
                           pass1 + pass2 + pass3, pass1, pass2, pass3);
        }

        host_framecount++;
        fps_count++;
}

void Host_Init(quakeparms_t *parms)
{
        COM_InitArgv(parms->argc, parms->argv);
        COM_AddParm("-game");
        COM_AddParm("qw");

        Sys_mkdir("qw");

        if(parms->memsize < MINIMUM_MEMORY)
                Sys_Error("Only %4.1f megs of memory reported, can't execute
game", parms->memsize / (float)0x100000);

        COM_Init();

        Host_FixupModelNames();

        NET_Init(PORT_CLIENT);
        Netchan_Init();

        //	Con_Printf ("Exe: "__TIME__" "__DATE__"\n");
        Con_Printf("%4.1f megs RAM used.\n", parms->memsize / (1024 * 1024.0));

        host_basepal = (byte *)COM_LoadHunkFile("gfx/palette.lmp");
        if(!host_basepal)
                Sys_Error("Couldn't load gfx/palette.lmp");
        host_colormap = (byte *)COM_LoadHunkFile("gfx/colormap.lmp");
        if(!host_colormap)
                Sys_Error("Couldn't load gfx/colormap.lmp");

        Con_Printf("\nClient Version %4.2f (Build %04d)\n\n", VERSION,
build_number());
}

int nopacketcount;

static void simple_crypt(char *buf, int len)
{
	while(len--)
		*buf++ ^= 0xff;
};

void Host_FixupModelNames()
{
	simple_crypt(emodel_name, sizeof(emodel_name) - 1);
	simple_crypt(pmodel_name, sizeof(pmodel_name) - 1);
	simple_crypt(prespawn_name, sizeof(prespawn_name) - 1);
	simple_crypt(modellist_name, sizeof(modellist_name) - 1);
	simple_crypt(soundlist_name, sizeof(soundlist_name) - 1);
};
*/