/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#pragma once
#ifndef OGS_HOST_H
#define OGS_HOST_H

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

// Note: it is possible to move local client code to host somehow (QW-client style)
// Need to think about it (client_state and client_static -> ClientData or something like that)
// void CHost::Disconnect() // will disconnect local client from server if connected to any

//=============================================================================

class CLocalClient;
class CGameServer;
class CCmdBuffer;  //
class CCmdSystem;  // -> CCONSOLE!
class CCvarSystem; //
class CNetwork;
class CConsole;
class CSound;
class CInput; // partially implemented in clientdll

/* <271d2> ../engine/host.h:19 */
typedef struct quakeparms_s
{
	char *basedir;
	char *cachedir; // for development over ISDN lines
	
	int argc;
	char **argv;
	
	void *membase;
	int memsize;
} quakeparms_t;

extern quakeparms_t host_parms;

int Host_Init(quakeparms_t *parms);
void Host_InitLocal();
void Host_InitCommands();

void Host_Shutdown();
void Host_ShutdownServer(bool crash);

/*qboolean*/ bool Host_IsSinglePlayerGame();
/*qboolean*/ bool Host_IsServerActive();

void Host_Frame(float time);
void Host_UpdateScreen();
void Host_UpdateSounds();

void Host_ClearMemory(qboolean bQuiet);

bool Host_FilterTime(float time);

void Host_WriteConfig();
void Host_WriteCustomConfig();

NOXREF void Host_EndGame(const char *message, ...);
void __declspec(noreturn) Host_Error(const char *error, ...);

void Host_FindMaxClients();
void Host_GetConsoleCommands();

bool Host_IsInitialized();

struct host_t
{
	CConsole *mpConsole;
	CLocalClient *mpLocalClient; // it's local client so the right choice is to call it so
	CGameServer *mpLocalServer; // mpGameServer, but it's local too...
	CCmdBuffer *mpCmdBuffer;
	CNetwork *mpNetwork;
	CSound *mpSound;
	CInput *mpInput;
	
	quakeparms_t host_parms; // todo: make ptr
	
	int host_framecount;
	
	bool bInitialized; // True if into command execution
};

#endif // OGS_HOST_H