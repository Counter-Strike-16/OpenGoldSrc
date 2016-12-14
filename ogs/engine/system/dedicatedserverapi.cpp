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

#include "precompiled.h"
#include "system/dedicatedserverapi.h"

IDedicatedExports *dedicated_;

#ifndef HOOK_ENGINE
	EXPOSE_SINGLE_INTERFACE(CDedicatedServerAPI, IDedicatedServerAPI, VENGINE_HLDS_API_VERSION);
#endif

bool CDedicatedServerAPI::Init(char *basedir, char *cmdline, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	dedicated_ = (IDedicatedExports*)launcherFactory(VENGINE_DEDICATEDEXPORTS_API_VERSION, NULL);
	if(!dedicated_)
		return false;
	
	Q_strncpy(m_OrigCmd, cmdline, charsmax(m_OrigCmd));
	
	if(!Q_strstr(cmdline, "-nobreakpad"))
		CRehldsPlatformHolder::get()->SteamAPI_UseBreakpadCrashHandler(va("%d", build_number()), __BUILD_DATE__, __BUILD_TIME__, 0, 0, 0);
	
	TraceInit("Sys_InitArgv( m_OrigCmd )", "Sys_ShutdownArgv()", 0);
	Sys_InitArgv(m_OrigCmd);
	eng->SetQuitting(IEngine::QUIT_NOTQUITTING);
	registry->Init();
	
	g_bIsDedicatedServer = TRUE;
	TraceInit("FileSystem_Init(basedir, (void *)filesystemFactory)", "FileSystem_Shutdown()", 0);
	
	if(FileSystem_Init(basedir, filesystemFactory) && game->Init(0) && eng->Load(true, basedir, cmdline))
	{
		char text[256];
		Q_snprintf(text, ARRAYSIZE(text), "exec %s\n", servercfgfile.string);
		text[255] = 0;
		Cbuf_InsertText(text);
		return true;
	};
	
	return false;
};

int CDedicatedServerAPI::Shutdown()
{
	eng->Unload();
	game->Shutdown();
	
	TraceShutdown("FileSystem_Shutdown()", 0);
	FileSystem_Shutdown();
	
	registry->Shutdown();
	
	TraceShutdown("Sys_ShutdownArgv()", 0);
	dedicated_ = NULL;
	return giActive;
};

bool CDedicatedServerAPI::RunFrame()
{
	if(eng->GetQuitting())
		return false;
	
	eng->Frame();
	return true;
};

void CDedicatedServerAPI::AddConsoleText(char *text)
{
	Cbuf_AddText(text);
};

void CDedicatedServerAPI::UpdateStatus(float *fps, int *nActive, int *nMaxPlayers, char *pszMap)
{
	Host_GetHostInfo(fps, nActive, NULL, nMaxPlayers, pszMap);
};