/*

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// Host.cpp -- coordinates spawning and killing of local servers

#include "Host.h"
#include "CmdBuffer.h"
#include "network/Network.h"
#include "sound/Sound.h"
#include "input/Input.h"
#include "quakedef.h"
#include "r_local.h"

/*

A server can always be started, even if the system started out as a client
to a remote system.

A client can NOT be started if the system started as a dedicated server.

Memory is cleared / released when a server or client begins, not when they end.

*/

double		host_frametime;
double		host_time;
double		realtime;				// without any filtering or bounding
double		oldrealtime;			// last frame run

int			host_hunklevel;

int			minimum_memory;

client_t	*host_client;			// current client

jmp_buf 	host_abortserver;

byte		*host_basepal;
byte		*host_colormap;

cvar_t	host_framerate = {"host_framerate","0"};	// set for slow motion
cvar_t	host_speeds = {"host_speeds","0"};			// set for running times

cvar_t	sys_ticrate = {"sys_ticrate","0.05"}; // 100
cvar_t	serverprofile = {"serverprofile","0"};

cvar_t	fraglimit = {"fraglimit", "0", false, true};
cvar_t	timelimit = {"timelimit", "0", false, true};
cvar_t	teamplay = {"teamplay", "0", false, true};

cvar_t	samelevel = {"samelevel", "0"};
cvar_t	noexit = {"noexit", "0", false, true};

cvar_t	developer = {"developer", "0"};

cvar_t	skill = {"skill", "1"};						// 0 - 3
cvar_t	deathmatch = {"deathmatch", "0"};			// 0, 1, or 2
cvar_t	coop = {"coop", "0"};			// 0 or 1

cvar_t	pausable = {"pausable", "1"};

/*
====================
Host_Init
====================
*/
void CHost::Init(quakeparms_t *parms)
{
	if (standard_quake)
		minimum_memory = MINIMUM_MEMORY;
	else
		minimum_memory = MINIMUM_MEMORY_LEVELPAK;

	if (COM_CheckParm ("-minmemory"))
		parms->memsize = minimum_memory;

	host_parms = *parms;

	if (parms->memsize < minimum_memory)
		Sys_Error ("Only %4.1f megs of memory available, can't execute game", parms->memsize / (float)0x100000);

	com_argc = parms->argc;
	com_argv = parms->argv;
	
	// Lazy subsystem initialization
	// TODO: don't forget to delete all this crap
	mpCmdBuffer = new CCmdBuffer();
	mpNetwork = new CNetwork();
	mpSound = new CSound();
	mpInput = new CInput();

	Memory_Init (parms->membase, parms->memsize);
	mpCmdBuffer->Init();
	mpCmdSystem->Init();	
	V_Init ();
	Chase_Init ();
	Host_InitVCR (parms);
	COM_Init (parms->basedir);
	InitLocal ();
	W_LoadWadFile ("gfx.wad");
	Key_Init (); // move to input
	Con_Init ();	
	M_Init ();	
	PR_Init ();
	Mod_Init ();
	mpNetwork->Init ();
	SV_Init (); // move to network?

	Con_Printf ("Exe: "__TIME__" "__DATE__"\n");
	Con_Printf ("%4.1f megabyte heap\n",parms->memsize/ (1024*1024.0));
	
	R_InitTextures ();		// needed even for dedicated servers
 
	if (cls->GetState() != ca_dedicated)
	{
		host_basepal = (byte *)COM_LoadHunkFile ("gfx/palette.lmp");
		if (!host_basepal)
			Sys_Error ("Couldn't load gfx/palette.lmp");
		host_colormap = (byte *)COM_LoadHunkFile ("gfx/colormap.lmp");
		if (!host_colormap)
			Sys_Error ("Couldn't load gfx/colormap.lmp");

#ifndef _WIN32 // on non win32, mouse comes before video for security reasons
		mpInput->Init();
#endif
		//VID_Init(host_basepal);

		//Draw_Init ();
		//SCR_Init ();
		//R_Init ();
		
#ifndef	_WIN32
	// on Win32, sound initialization has to come before video initialization, so we
	// can put up a popup if the sound hardware is in use
		mpSound->Init();
#else

#ifdef	GLQUAKE
	// FIXME: doesn't use the new one-window approach yet
		mpSound->Init();
#endif

#endif	// _WIN32
		//CDAudio_Init ();
		//Sbar_Init ();
		//CL_Init ();
		
#ifdef _WIN32 // on non win32, mouse comes before video for security reasons
		mpInput->Init();
#endif
	};
	
	//Cbuf_InsertText ("exec quake.rc\n");
	//Cbuf_AddText ("echo Type connect <internet address> or use GameSpy to connect to a game.\n");
	//Cbuf_AddText ("cl_warncmd 1\n");

	//Hunk_AllocName (0, "-HOST_HUNKLEVEL-");
	//host_hunklevel = Hunk_LowMark ();
	
	bInitialized = true;
	Sys_Printf("========OGS Initialized=========\n");	
};

/*
===============
Host_Shutdown

FIXME: this is a callback from Sys_Quit and Sys_Error.  It would be better
to run quit through here before the final handoff to the sys code.
===============
*/
void CHost::Shutdown()
{
	static bool bShuttingDown = false;
	
	if(bShuttingDown)
	{
		printf("recursive shutdown\n");
		return;
	};
	
	bShuttingDown = true;
	
	// Keep Con_Printf from trying to update the screen
	//scr_disabled_for_loading = true;
	
	WriteConfig();
	
	//CDAudio_Shutdown();
	mpNetwork->Shutdown();
	mpSound->Shutdown();
	mpInput->Shutdown();
	
	if(cls->GetState() != ca_dedicated) // host_basepal instead of this in qw
		VID_Shutdown();
};

/*
================
Host_EndGame

Call this to drop to a console without exiting
================
*/
void CHost::EndGame(char *message, ...)
{
	va_list		argptr;
	char		string[1024];
	
	va_start (argptr,message);
	vsprintf (string,message,argptr);
	va_end (argptr);
	
	Con_DPrintf ("Host_EndGame: %s\n",string);
	
	if (sv->IsActive())
		ShutdownServer (false);

	if (cls->GetState() == ca_dedicated)
		Sys_Error ("Host_EndGame: %s\n",string);	// dedicated servers exit
	
	if (cls.demonum != -1)
		CL_NextDemo ();
	else
		CL_Disconnect ();

	longjmp (host_abortserver, 1);
}

/*
================
Host_Error

This shuts down both the client and server and exits
================
*/
void CHost::Error(char *error, ...)
{
	va_list		argptr;
	char		string[1024];
	static	qboolean inerror = false;
	
	if (inerror)
		Sys_Error ("Host_Error: recursively entered");
	
	inerror = true;
	
	SCR_EndLoadingPlaque ();		// reenable screen updates

	va_start (argptr,error);
	vsprintf (string,error,argptr);
	va_end (argptr);
	Con_Printf ("Host_Error: %s\n",string);
	
	if (sv->IsActive())
		ShutdownServer (false);

	if (cls->GetState() == ca_dedicated)
		Sys_Error ("Host_Error: %s\n",string);	// dedicated servers exit

	CL_Disconnect ();
	
	cls.demonum = -1;

	inerror = false;
	
	// FIXME
	//Sys_Error ("Host_Error: %s\n",string);

	longjmp (host_abortserver, 1);
}

/*
================
Host_FindMaxClients
================
*/
void CHost::FindMaxClients()
{
	int		i;

	svs.maxclients = 1;
		
	i = COM_CheckParm ("-dedicated");
	
	if (i)
	{
		cls->SetState(ca_dedicated);
		
		if (i != (com_argc - 1))
		{
			svs.maxclients = Q_atoi (com_argv[i+1]);
		}
		else
			svs.maxclients = 8;
	}
	else
		cls->SetState(ca_disconnected);

	i = COM_CheckParm ("-listen");
	if (i)
	{
		if (cls->GetState() == ca_dedicated)
			Sys_Error ("Only one of -dedicated or -listen can be specified");
		
		if (i != (com_argc - 1))
			svs.maxclients = Q_atoi (com_argv[i+1]);
		else
			svs.maxclients = 8;
	}
	if (svs.maxclients < 1)
		svs.maxclients = 8;
	else if (svs.maxclients > MAX_SCOREBOARD)
		svs.maxclients = MAX_SCOREBOARD;

	svs.maxclientslimit = svs.maxclients;
	
	if (svs.maxclientslimit < 4)
		svs.maxclientslimit = 4;
	
	svs.clients = Hunk_AllocName (svs.maxclientslimit*sizeof(client_t), "clients");

	if (svs.maxclients > 1)
		Cvar_SetValue ("deathmatch", 1.0);
	else
		Cvar_SetValue ("deathmatch", 0.0);
};

/*
=======================
Host_InitLocal
======================
*/
void CHost::InitLocal(void)
{
	InitCommands();
	
	Cvar_RegisterVariable (&host_framerate);
	Cvar_RegisterVariable (&host_speeds);

	Cvar_RegisterVariable (&sys_ticrate);
	Cvar_RegisterVariable (&serverprofile);

	Cvar_RegisterVariable (&fraglimit);
	Cvar_RegisterVariable (&timelimit);
	Cvar_RegisterVariable (&teamplay);
	Cvar_RegisterVariable (&samelevel);
	Cvar_RegisterVariable (&noexit);
	Cvar_RegisterVariable (&skill);
	Cvar_RegisterVariable (&developer);
	Cvar_RegisterVariable (&deathmatch);
	Cvar_RegisterVariable (&coop);

	Cvar_RegisterVariable (&pausable);

	FindMaxClients();
	
	host_time = 1.0;		// so a think at time 0 won't get called
};

/*
===============
Host_WriteConfiguration

Writes key bindings and archived cvars to config.cfg
===============
*/
void CHost::WriteConfig()
{
	FILE	*f;

// dedicated servers initialize the host but don't parse and set the
// config.cfg cvars
	if (bInitialized & !isDedicated)
	{
		f = fopen (va("%s/config.cfg",com_gamedir), "w");
		if (!f)
		{
			Con_Printf ("Couldn't write config.cfg.\n");
			return;
		}
		
		Key_WriteBindings (f);
		Cvar_WriteVariables (f);

		fclose (f);
	};
};

/*
=================
Host_ClientCommands

Send text over to the client to be executed
=================
*/
void Host_ClientCommands (char *fmt, ...)
{
	va_list		argptr;
	char		string[1024];
	
	va_start (argptr,fmt);
	vsprintf (string, fmt,argptr);
	va_end (argptr);
	
	MSG_WriteByte (&host_client->message, svc_stufftext);
	MSG_WriteString (&host_client->message, string);
}

/*
==================
Host_ShutdownServer

This only happens at the end of a game, not between levels
==================
*/
void CHost::ShutdownServer(bool crash)
{
	int		i;
	int		count;
	sizebuf_t	buf;
	char		message[4];
	double	start;

	if (!sv.active)
		return;

	sv.active = false;

// stop all client sounds immediately
	if (cls.state == ca_connected)
		CL_Disconnect ();

// flush any pending messages - like the score!!!
	start = Sys_FloatTime();
	do
	{
		count = 0;
		for (i=0, host_client = svs.clients ; i<svs.maxclients ; i++, host_client++)
		{
			if (host_client->active && host_client->message.cursize)
			{
				if (NET_CanSendMessage (host_client->netconnection))
				{
					NET_SendMessage(host_client->netconnection, &host_client->message);
					SZ_Clear (&host_client->message);
				}
				else
				{
					NET_GetMessage(host_client->netconnection);
					count++;
				}
			}
		}
		if ((Sys_FloatTime() - start) > 3.0)
			break;
	}
	while (count);

// make sure all the clients know we're disconnecting
	buf.data = message;
	buf.maxsize = 4;
	buf.cursize = 0;
	MSG_WriteByte(&buf, svc_disconnect);
	count = mpNetwork->SendToAll(&buf, 5);
	if (count)
		Con_Printf("Host_ShutdownServer: NET_SendToAll failed for %u clients\n", count);

	for (i=0, host_client = svs.clients ; i<svs.maxclients ; i++, host_client++)
		if (host_client->active)
			SV_DropClient(crash);

//
// clear structures
//
	memset (&sv, 0, sizeof(sv));
	memset (svs.clients, 0, svs.maxclientslimit*sizeof(client_t));
}


/*
================
Host_ClearMemory

This clears all the memory used by both the client and server, but does
not reinitialize anything.
================
*/
void CHost::ClearMemory()
{
	Con_DPrintf ("Clearing memory\n");
	D_FlushCaches ();
	Mod_ClearAll ();
	
	if (host_hunklevel)
		Hunk_FreeToLowMark (host_hunklevel);

	cls.signon = 0;
	memset (&sv, 0, sizeof(sv));
	memset (&cl, 0, sizeof(cl));
}

//============================================================================

/*
===================
Host_FilterTime

Returns false if the time is too short to run a frame
===================
*/
bool CHost::FilterTime (float time)
{
	realtime += time;

	if (!cls.timedemo && realtime - oldrealtime < 1.0/72.0)
		return false;		// framerate is too high

	host_frametime = realtime - oldrealtime;
	oldrealtime = realtime;

	if (host_framerate.value > 0)
		host_frametime = host_framerate.value;
	else
	{	// don't allow really long or short frames
		if (host_frametime > 0.1)
			host_frametime = 0.1;
		if (host_frametime < 0.001)
			host_frametime = 0.001;
	}
	
	return true;
}

/*
===================
Host_GetConsoleCommands

Add them exactly as if they had been typed at the console
===================
*/
void CHost::GetConsoleCommands()
{
	char	*cmd;

	while (1)
	{
		cmd = Sys_ConsoleInput ();
		if (!cmd)
			break;
		
		mpCmdBuffer->AddText(cmd);
	}
}


/*
==================
Host_ServerFrame

==================
*/
#ifdef FPS_20

void _Host_ServerFrame (void)
{
// run the world state	
	pr_global_struct->frametime = host_frametime;

// read client messages
	SV_RunClients ();
	
// move things around and think
// always pause in single player if in console or menus
	if (!sv.paused && (svs.maxclients > 1 || key_dest == key_game) )
		SV_Physics ();
}

void Host_ServerFrame (void)
{
	float	save_host_frametime;
	float	temp_host_frametime;

// run the world state	
	pr_global_struct->frametime = host_frametime;

// set the time and clear the general datagram
	SV_ClearDatagram ();
	
// check for new clients
	SV_CheckForNewClients ();

	temp_host_frametime = save_host_frametime = host_frametime;
	while(temp_host_frametime > (1.0/72.0))
	{
		if (temp_host_frametime > 0.05)
			host_frametime = 0.05;
		else
			host_frametime = temp_host_frametime;
		temp_host_frametime -= host_frametime;
		_Host_ServerFrame ();
	}
	host_frametime = save_host_frametime;

// send all messages to the clients
	SV_SendClientMessages ();
}

#else

void Host_ServerFrame (void)
{
// run the world state	
	pr_global_struct->frametime = host_frametime;

// set the time and clear the general datagram
	SV_ClearDatagram ();
	
// check for new clients
	SV_CheckForNewClients ();

// read client messages
	SV_RunClients ();
	
// move things around and think
// always pause in single player if in console or menus
	if (!sv.paused && (svs.maxclients > 1 || key_dest == key_game) )
		SV_Physics ();

// send all messages to the clients
	SV_SendClientMessages ();
}

#endif

/*
==================
Host_Frame

Runs all active servers
==================
*/
void _Host_Frame (float time)
{
	static double		time1 = 0;
	static double		time2 = 0;
	static double		time3 = 0;
	int			pass1, pass2, pass3;

	if (setjmp (host_abortserver) )
		return;			// something bad happened, or the server disconnected

// keep the random time dependent
	rand ();
	
// decide the simulation time
	if (!Host_FilterTime (time))
		return;			// don't run too fast, or packets will flood out
		
// get new key events
	Sys_SendKeyEvents ();

// allow mice or other external controllers to add commands
	mpInput->Commands ();

// process console commands
	mpCmdBuffer->Execute ();

	mpNetwork->Poll();

// if running the server locally, make intentions now
	if (sv.active)
		CL_SendCmd ();
	
//-------------------
//
// server operations
//
//-------------------

// check for commands typed to the host
	Host_GetConsoleCommands ();
	
	if (sv.active)
		Host_ServerFrame ();

//-------------------
//
// client operations
//
//-------------------

// if running the server remotely, send intentions now after
// the incoming messages have been read
	if (!sv.active)
		CL_SendCmd ();

	host_time += host_frametime;

// fetch results from server
	if (cls.state == ca_connected)
	{
		CL_ReadFromServer ();
	}

// update video
	if (host_speeds.value)
		time1 = Sys_FloatTime ();
		
	SCR_UpdateScreen ();

	if (host_speeds.value)
		time2 = Sys_FloatTime ();
		
// update audio
	if (cls.signon == SIGNONS)
	{
		S_Update (r_origin, vpn, vright, vup);
		CL_DecayLights ();
	}
	else
		S_Update (vec3_origin, vec3_origin, vec3_origin, vec3_origin);
	
	CDAudio_Update();

	if (host_speeds.value)
	{
		pass1 = (time1 - time3)*1000;
		time3 = Sys_FloatTime ();
		pass2 = (time2 - time1)*1000;
		pass3 = (time3 - time2)*1000;
		Con_Printf ("%3i tot %3i server %3i gfx %3i snd\n",
					pass1+pass2+pass3, pass1, pass2, pass3);
	}
	
	host_framecount++;
}

void CHost::Frame(float time)
{
	double	time1, time2;
	static double	timetotal;
	static int		timecount;
	int		i, c, m;

	if (!serverprofile.value)
	{
		_Host_Frame (time);
		return;
	}
	
	time1 = Sys_FloatTime ();
	_Host_Frame (time);
	time2 = Sys_FloatTime ();	
	
	timetotal += time2 - time1;
	timecount++;
	
	if (timecount < 1000)
		return;

	m = timetotal*1000/timecount;
	timecount = 0;
	timetotal = 0;
	c = 0;
	for (i=0 ; i<svs.maxclients ; i++)
	{
		if (svs.clients[i].active)
			c++;
	}

	Con_Printf ("serverprofile: %2i clients %2i msec\n",  c,  m);
}

//============================================================================

extern int vcrFile;
#define	VCR_SIGNATURE	0x56435231
// "VCR1"

void Host_InitVCR (quakeparms_t *parms)
{
	int		i, len, n;
	char	*p;
	
	if (COM_CheckParm("-playback"))
	{
		if (com_argc != 2)
			Sys_Error("No other parameters allowed with -playback\n");

		Sys_FileOpenRead("quake.vcr", &vcrFile);
		if (vcrFile == -1)
			Sys_Error("playback file not found\n");

		Sys_FileRead (vcrFile, &i, sizeof(int));
		if (i != VCR_SIGNATURE)
			Sys_Error("Invalid signature in vcr file\n");

		Sys_FileRead (vcrFile, &com_argc, sizeof(int));
		com_argv = malloc(com_argc * sizeof(char *));
		com_argv[0] = parms->argv[0];
		for (i = 0; i < com_argc; i++)
		{
			Sys_FileRead (vcrFile, &len, sizeof(int));
			p = malloc(len);
			Sys_FileRead (vcrFile, p, len);
			com_argv[i+1] = p;
		}
		com_argc++; /* add one for arg[0] */
		parms->argc = com_argc;
		parms->argv = com_argv;
	}

	if ( (n = COM_CheckParm("-record")) != 0)
	{
		vcrFile = Sys_FileOpenWrite("quake.vcr");

		i = VCR_SIGNATURE;
		Sys_FileWrite(vcrFile, &i, sizeof(int));
		i = com_argc - 1;
		Sys_FileWrite(vcrFile, &i, sizeof(int));
		for (i = 1; i < com_argc; i++)
		{
			if (i == n)
			{
				len = 10;
				Sys_FileWrite(vcrFile, &len, sizeof(int));
				Sys_FileWrite(vcrFile, "-playback", len);
				continue;
			}
			len = Q_strlen(com_argv[i]) + 1;
			Sys_FileWrite(vcrFile, &len, sizeof(int));
			Sys_FileWrite(vcrFile, com_argv[i], len);
		}
	}
	
}