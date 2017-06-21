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
#include "system/engineapi.hpp"
#include "system/common.hpp"
#include "system/system.hpp"
#include "system/iengine.hpp"
#include "system/igame.hpp"
#include "system/traceinit.h"
#include "system/buildinfo.hpp"
#include "filesystem/filesystem_.hpp"

int RunListenServer(void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory);

static CEngineAPI g_CEngineAPI;

/*
IBaseInterface *CreateCEngineAPI()
{
        return &g_CEngineAPI;
};

InterfaceReg g_CreateCEngineAPI = InterfaceReg(CreateCEngineAPI,
"VENGINE_LAUNCHER_API_VERSION002");
*/

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEngineAPI, IEngineAPI, VENGINE_LAUNCHER_API_VERSION, g_CEngineAPI);

void F(IEngineAPI **api)
{
	CreateInterfaceFn fnEngineFactory = Sys_GetFactoryThis();
	*api = (IEngineAPI *)fnEngineFactory(VENGINE_LAUNCHER_API_VERSION, NULL);
};

// non-const char *cmdline?
int CEngineAPI::Run(void *instance, char *basedir, const char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory)
{
	//if(!Q_strstr(cmdline, "-nobreakpad"))
		//SteamAPI_UseBreakpadCrashHandler(va("%d", build_number()), __DATE__, __TIME__, 0, 0, 0);

	return RunListenServer(instance, basedir, cmdline, postRestartCmdLineArgs, launcherFactory, filesystemFactory);
};

int RunListenServer( void *instance, char *basedir, char *cmdline, char *postRestartCmdLineArgs, CreateInterfaceFn launcherFactory, CreateInterfaceFn filesystemFactory )
{
	static char OrigCmd[ 1024 ];

	//g_pPostRestartCmdLineArgs = postRestartCmdLineArgs;
	strcpy( OrigCmd, cmdline );

	TraceInit( "Sys_InitArgv( OrigCmd )", "Sys_ShutdownArgv()", 0 );
	Sys_InitArgv( OrigCmd );

	eng->SetQuitting( IEngine::QUIT_NOTQUITTING );
	
	registry->Init();
	
	//Steam_InitClient();
	
	int result = 0; //DLL_QUITTING;

	TraceInit( "FileSystem_Init(basedir, (void *)filesystemFactory)", "FileSystem_Shutdown()", 0 );

	if( FileSystem_Init( basedir, filesystemFactory ) )
	{
		//VideoMode_Create();

		result = 0; //ENGRUN_UNSUPPORTED_VIDEOMODE;
		registry->WriteInt( "CrashInitializingVideoMode", 1 );

		if( true /*videomode->Init( instance )*/ )
		{
			result = 0; //ENGRUN_CHANGED_VIDEOMODE;
			registry->WriteInt( "CrashInitializingVideoMode", 0 );

			if( game->Init(	instance ) )
			{
				result = 0; //ENGRUN_UNSUPPORTED_VIDEOMODE;

				if( eng->Load( false, basedir, cmdline ) )
				{
					while( true )
					{
						game->SleepUntilInput( 0 );

						if( eng->GetQuitting() != IEngine::QUIT_NOTQUITTING )
							break;

						eng->Frame();
					}

					//result = eng->GetQuitting() != IEngine::QUIT_TODESKTOP ? ENGRUN_CHANGED_VIDEOMODE : DLL_QUITTING;

					eng->Unload();
				}

				game->Shutdown();
			}

			//videomode->Shutdown();
		}

		TraceShutdown( "FileSystem_Shutdown()", 0 );

		FileSystem_Shutdown();
		registry->Shutdown();

		TraceShutdown( "Sys_ShutdownArgv()", 0 );
	}

	return result;
};