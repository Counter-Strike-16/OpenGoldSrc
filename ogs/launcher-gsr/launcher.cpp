#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>  
#include <sys/stat.h>  

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <direct.h>
#include <io.h>
#include <process.h>

#include <WinSock2.h>

#include "engine_launcher_api.h"
#include "FilePaths.h"
#include "FileSystem.h"
#include "ICommandLine.h"
#include "interface.h"
#include "IRegistry.h"

//TODO: Linux version doesn't use registry so don't include it - Solokiller

char com_gamedir[ MAX_PATH ] = {};

IFileSystem* g_pFileSystem = nullptr;

bool LR_FileExists( const char* pszFilename )
{
	struct stat buf;

	return stat( pszFilename, &buf ) == 0;
}

const char SubKey[] = "Software\\Valve\\Steam";

/**
*	Sets up our current environment in the registry and tells Steam to launch us.
*	TODO: Steamworks provides a function (SteamAPI_RestartAppIfNecessary) to do this automatically. Replace? - Solokiller
*/
void LR_LaunchThroughSteam( const char* lpData )
{
	char Filename[ MAX_PATH ];

	HMODULE hThisModule = GetModuleHandleA( nullptr );

	GetModuleFileNameA( hThisModule, Filename, sizeof( Filename ) );

	{
		char* pszLastSlash = strrchr( Filename, '\\' );

		if( pszLastSlash )
			*pszLastSlash = '\0';
	}

	HKEY phkResult;

	//Set the registry entries for the current instance.
	if( ERROR_SUCCESS == RegOpenKeyA( HKEY_CURRENT_USER, SubKey, &phkResult ) )
	{
		const size_t uiFilenameLength = strlen( Filename );
		RegSetValueExA( phkResult, "TempAppPath", 0, REG_SZ, reinterpret_cast<const BYTE*>( Filename ), uiFilenameLength + 1 );

		const size_t uiCmdLineLength = strlen( lpData );
		RegSetValueExA( phkResult, "TempAppCmdLine", 0, REG_SZ, reinterpret_cast<const BYTE*>( lpData ), uiCmdLineLength + 1 );

		const DWORD Data = -1;

		RegSetValueExA( phkResult, "TempAppID", 0, REG_DWORD, reinterpret_cast<const BYTE*>( &Data ), sizeof( Data ) );
		RegCloseKey( phkResult );
	}

	//Find the Steam IPC window and send a message informing it of our startup.
	HWND hWnd = FindWindowA( "Valve_SteamIPC_Class", "Hidden Window" );

	if( hWnd != NULL )
	{
		//TODO: message ID is a custom one. Still used since SDL2 switch? - Solokiller
		PostMessageA( hWnd, 0x403u, 0, 0 );
	}
	else
	{
		//Steam isn't running yet, start it up and have it launch us.
		char szSteamFilename[ MAX_PATH ] = {};

		char Buffer[ MAX_PATH ];

		if( GetCurrentDirectoryA( sizeof( Buffer ), Buffer ) )
		{
			//Find the Steam or Steam development executable to launch.
			char* pszLastSlash = strrchr( Buffer, '\\' );

			FILE* pFile = nullptr;

			while( pszLastSlash )
			{
				//Dev builds first.
				pszLastSlash[ 1 ] = '\0';
				strcat( pszLastSlash, "steam_dev.exe" );
				pFile = fopen( Buffer, "rb" );
				if( pFile )
					break;
				//Public builds next.
				pszLastSlash[ 1 ] = '\0';
				strcat( pszLastSlash, "steam.exe" );
				pFile = fopen( Buffer, "rb" );
				if( pFile )
					break;
				//Go up a directory if possible, else exit.
				*pszLastSlash = '\0';
				pszLastSlash = strrchr( Buffer, '\\' );
			}

			if( pFile )
			{
				fclose( pFile );
				strcpy( szSteamFilename, Buffer );
			}
		}

		//Couldn't find the Steam exe, see if there is a registry entry for it.
		if( !( *szSteamFilename ) && ERROR_SUCCESS == RegOpenKeyA( HKEY_CURRENT_USER, SubKey, &phkResult ) )
		{
			DWORD Type;
			DWORD Data = MAX_PATH;
			RegQueryValueExA( phkResult, "SteamExe", 0u, &Type, reinterpret_cast<BYTE*>( szSteamFilename ), &Data );
			RegCloseKey( phkResult );
		}

		if( *szSteamFilename )
		{
			//TODO: UTIL_FixSlashes - Solokiller
			char* pszPtr = szSteamFilename;
			if( *szSteamFilename )
			{
				do
				{
					if( *pszPtr == '/' )
						*pszPtr = '\\';
					++pszPtr;
				}
				while( *pszPtr );
			}

			strcpy( Buffer, szSteamFilename );

			//Change directory to Steam directory.
			char* pszLastSlash = strrchr( Buffer, '\\' );
			if( pszLastSlash )
			{
				*pszLastSlash = '\0';
				_chdir( Buffer );
			}

			//Launch Steam and tell it to launch us again.
			const char* const pszArgs[] = 
			{
				szSteamFilename,
				"-silent",
				"-applaunch",
				nullptr
			};

			_spawnv( _P_NOWAIT, szSteamFilename, pszArgs );
		}
		else
		{
			//Tell user about problem.
			MessageBoxA( NULL, "Error running game: could not find steam.exe to launch", "Fatal Error", MB_OK | MB_ICONERROR );
		}
	}
}

/**
*	Verifies that we were launched through Steam, launches us through Steam if not.
*/
bool LR_VerifySteamStatus( const char* pszCommandLine, const char* pszFileSystemSteam, const char* pszFileSystemStdio )
{
	//TODO: We can't actually let this code run because Steam won't let us launch, since we count as a different exe. - Solokiller
	return false;

#if 0
	const char* pszResult = strstr( pszCommandLine, "-steam" );

	if( pszResult )
	{
		const char cEnd = pszResult[ strlen( "-steam" ) ];

		//-steam was present, or the no-Steam filesysteam is present (no-Steam launch allowed), or there is no Steam filesystem (unknown other filesystem present).
		//TODO: obsolete since Steampipe update: no more Steam filesystem, only Stdio - Solokiller
		if( ( cEnd == '\0' || isspace( cEnd ) )
			|| LR_FileExists( pszFileSystemStdio )
			|| !LR_FileExists( pszFileSystemSteam ) )
		{
			return false;
		}
	}

	LR_LaunchThroughSteam( pszCommandLine );

	return true;
#endif
}

bool Sys_GetExecutableName( char* pszFilename, size_t uiSize )
{
	HMODULE hThisModule = GetModuleHandleA( nullptr );
	//TODO: won't work properly on WinXP, see https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197(v=vs.85).aspx - Solokiller
	return GetModuleFileNameA( hThisModule, pszFilename, uiSize ) != 0;
}

CSysModule* LoadFilesystemModule( const char* exename, bool bRunningSteam )
{
	auto pModule = Sys_LoadModule( filepath::FILESYSTEM_STDIO );

	if( !pModule )
	{
		if( strchr( exename, ';' ) )
		{
			MessageBoxA( NULL, "Game cannot be run from directories containing the semicolon char", "Fatal Error", MB_ICONERROR );
			return nullptr;
		}

		struct _finddata_t find_data;

		auto result = _findfirst( filepath::FILESYSTEM_STDIO, &find_data );

		if( result == -1 )
		{
			MessageBoxA( NULL, "Could not find filesystem dll to load.", "Fatal Error", MB_ICONERROR );
		}
		else
		{
			MessageBoxA( NULL, "Could not load filesystem dll.", "Fatal Error", MB_ICONERROR );
			_findclose( result );
		}
	}

	return pModule;
}

static char szLongPath[ MAX_PATH ] = {};

/**
*	Gets the directory that this executable is running from.
*/
char* UTIL_GetBaseDir()
{
	char Filename[ MAX_PATH ];

	if( GetModuleFileNameA( NULL, Filename, sizeof( Filename ) ) )
	{
		GetLongPathNameA( Filename, szLongPath, sizeof( szLongPath ) );

		char* pszLastSlash = strrchr( szLongPath, '\\' );

		if( *pszLastSlash )
			pszLastSlash[ 1 ] = '\0';

		const size_t uiLength = strlen( szLongPath );

		if( uiLength > 0 )
		{
			char* pszEnd = &szLongPath[ uiLength - 1 ];

			if( *pszEnd == '\\' || *pszEnd == '/' )
				*pszEnd = '\0';
		}
	}

	return szLongPath;
}

void SetEngineDLL( const char** ppEngineDLL )
{
	*ppEngineDLL = "hw.dll";

	const char* pEngineDLLSetting = registry->ReadString( "EngineDLL", "hw.dll" );
	if( _stricmp( pEngineDLLSetting, "hw.dll" ) )
	{
		if( !_stricmp( pEngineDLLSetting, "sw.dll" ) )
			*ppEngineDLL = "sw.dll";
	}
	else
	{
		*ppEngineDLL = "hw.dll";
	}

	if( cmdline->CheckParm( "-soft", nullptr )
		|| cmdline->CheckParm( "-software", nullptr ) )
	{
		*ppEngineDLL = "sw.dll";
	}
	else if( cmdline->CheckParm( "-gl", nullptr )
			 || cmdline->CheckParm( "-d3d", nullptr ) )
	{
		*ppEngineDLL = "hw.dll";
	}

	registry->WriteString( "EngineDLL", *ppEngineDLL );
}

bool OnVideoModeFailed()
{
	registry->WriteInt( "ScreenBPP", 16 );
	registry->WriteInt( "ScreenHeight", 640 );
	registry->WriteInt( "ScreenWidth", 480 );

	registry->WriteString( "EngineDLL", "hw.dll" );

	return MessageBoxA(
		NULL, 
		"The specified video mode is not supported.", "Video mode change failure", 
		MB_OKCANCEL | MB_ICONERROR | MB_ICONQUESTION ) == IDOK;
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
)
{
	//If we aren't allowed to continue launching, exit now.
	if( LR_VerifySteamStatus( lpCmdLine, filepath::FILESYSTEM_STDIO, filepath::FILESYSTEM_STDIO ) )
		return EXIT_SUCCESS;

	HANDLE hMutex = CreateMutexA( nullptr, FALSE, "ValveHalfLifeLauncherMutex" );

	if( NULL != hMutex )
	{
		GetLastError();
	}

	DWORD dwMutexResult = WaitForSingleObject( hMutex, 0 );

	if( dwMutexResult != WAIT_OBJECT_0 && dwMutexResult != WAIT_ABANDONED )
	{
		MessageBoxA( NULL, "Could not launch game.\nOnly one instance of this game can be run at a time.", "Fatal Error", MB_OK | MB_ICONERROR );
		return EXIT_SUCCESS;
	}

	WSADATA WSAData;
	WSAStartup( MAKEWORD( 2, 0 ), &WSAData );

	registry->Init();

	cmdline->CreateCmdLine( GetCommandLineA() );

	char Filename[ 256 ];

	Sys_GetExecutableName( Filename, sizeof( Filename ) );

	//If this isn't hl.exe, force the game to be whichever game this exe is for.
	{
		char* pszLastSlash = strrchr( Filename, '\\' ) + 1;

		//E.g. cstrike.exe -> -game cstrike.
		if( _stricmp( "hl.exe", pszLastSlash ) && !cmdline->CheckParm( "-game", nullptr ) )
		{
			//This assumes that the program extension is ".exe" or another 4 character long extension.
			pszLastSlash[ strlen( pszLastSlash ) - 4 ] = '\0';
			cmdline->SetParm( "-game", pszLastSlash );
		}
	}

	//Set the game name.
	{
		//TODO: this is wrong. pszGame will point to the rest of the command line after and including -game.
		//It should copy the second parameter's result value. - Solokiller
		const char* pszGame = cmdline->CheckParm( "-game", nullptr );

		//Default to Half-Life.
		if( !pszGame )
			pszGame = "valve";

		strncpy( com_gamedir, pszGame, sizeof( com_gamedir ) );
		com_gamedir[ sizeof( com_gamedir ) - 1 ] = '\0';
	}

	//TODO: Could be the CRT heap init, but why is this here? - Solokiller
	//sub_14032FD(0);

	//Remove old libraries distributed with older Half-Life versions.
	_unlink( "mssv29.asi" );
	_unlink( "mssv12.asi" );
	_unlink( "mp3dec.asi" );
	_unlink( "opengl32.dll" );

	//If the game crashed during video mode initialization, reset video mode to default.
	if( registry->ReadInt( "CrashInitializingVideoMode", 0 ) )
	{
		registry->WriteInt( "CrashInitializingVideoMode", 0 );

		const char* pszEngineDLL = registry->ReadString( "EngineDLL", "hw.dll" );

		if( !_stricmp( pszEngineDLL, "hw.dll" ) )
		{
			const char* pszCaption = "Video mode change failure";
			const char* pszMessage;

			if( registry->ReadInt( "EngineD3D", 0 ) )
			{
				registry->WriteInt( "EngineD3D", 0 );

				pszMessage = 
					"The game has detected that the previous attempt to start in D3D video mode failed.\n"
					"The game will now run attempt to run in openGL mode.";
			}
			else
			{
				//TODO: Shouldn't this be sw.dll? - Solokiller
				registry->WriteString( "EngineDLL", "hw.dll" );

				pszMessage = 
					"The game has detected that the previous attempt to start in openGL video mode failed.\n"
					"The game will now run in software mode.";
			}

			//Ask the user if they want to continue.
			if( MessageBoxA( NULL, pszMessage, pszCaption, MB_OKCANCEL | MB_ICONERROR | MB_ICONQUESTION ) != IDOK )
				return EXIT_SUCCESS;

			registry->WriteInt( "ScreenBPP", 16 );
			registry->WriteInt( "ScreenHeight", 640 );
			registry->WriteInt( "ScreenWidth", 480 );
		}
	}

	static char szNewCommandParams[ 2048 ];

	bool bRestartEngine = false;

	do
	{
		//Load and mount the filesystem.
		auto hModule = LoadFilesystemModule( Filename, cmdline->CheckParm( "-game", nullptr ) != nullptr );
	
		if( !hModule )
			break;

		{
			auto factoryFn = Sys_GetFactory( hModule );

			g_pFileSystem = static_cast<IFileSystem*>( factoryFn( FILESYSTEM_INTERFACE_VERSION, nullptr ) );
		}

		g_pFileSystem->Mount();

		g_pFileSystem->AddSearchPath( UTIL_GetBaseDir(), "ROOT" );

		bRestartEngine = false;
		EngineRunResult runResult = ENGRUN_QUITTING;

		szNewCommandParams[ 0 ] = '\0';

		const char* pszLibFileName;
		SetEngineDLL( &pszLibFileName );

		auto hLibModule = Sys_LoadModule( pszLibFileName );

		if( hLibModule )
		{
			auto factoryFn = Sys_GetFactory( hLibModule );

			if( factoryFn )
			{
				auto pEngine = static_cast<IEngineAPI*>( factoryFn( ENGINE_LAUNCHER_INTERFACE_VERSION, nullptr ) );
			
				if( pEngine )
				{
					runResult = static_cast<EngineRunResult>( 
						pEngine->Run(
							hInstance, 
							UTIL_GetBaseDir(),
							cmdline->GetCmdLine(), 
							szNewCommandParams, 
							Sys_GetFactoryThis(), 
							Sys_GetFactory( hModule )
						)
					);
				}
			}

			Sys_UnloadModule( hLibModule );
		}
		else
		{
			char Text[ 512 ];

			snprintf( Text, sizeof( Text ), "Could not load %s.", pszLibFileName );
			MessageBoxA( NULL, Text, "Fatal Error", MB_ICONERROR );
		}

		switch( runResult )
		{
		case ENGRUN_QUITTING:
			bRestartEngine = false;
			break;
		case ENGRUN_CHANGED_VIDEOMODE:
			bRestartEngine = true;
			break;
		case ENGRUN_UNSUPPORTED_VIDEOMODE:
			bRestartEngine = OnVideoModeFailed();
			break;
		default:
			break;
		}

		//If we're restarting, remove any parameters that could affect video mode changes.
		//Also remove parameters that trigger events automatically, such as connecting to a server.
		cmdline->RemoveParm( "-sw" );
		cmdline->RemoveParm( "-startwindowed" );
		cmdline->RemoveParm( "-windowed" );
		cmdline->RemoveParm( "-window" );
		cmdline->RemoveParm( "-full" );
		cmdline->RemoveParm( "-fullscreen" );
		cmdline->RemoveParm( "-soft" );
		cmdline->RemoveParm( "-software" );
		cmdline->RemoveParm( "-gl" );
		cmdline->RemoveParm( "-d3d" );
		cmdline->RemoveParm( "-w" );
		cmdline->RemoveParm( "-width" );
		cmdline->RemoveParm( "-h" );
		cmdline->RemoveParm( "-height" );
		cmdline->RemoveParm( "+connect" );
		cmdline->SetParm( "-novid", nullptr );

		//User changed game.
		if( strstr( szNewCommandParams, "-game" ) )
		{
			cmdline->RemoveParm( "-game" );
		}

		//Remove saved game load command if new command is present.
		if( strstr( szNewCommandParams, "+load" ) )
		{
			cmdline->RemoveParm( "+load" );
		}

		//Append new command line to process properly.
		cmdline->AppendParm( szNewCommandParams, nullptr );

		g_pFileSystem->Unmount();
		Sys_UnloadModule( hModule );
	}
	while( bRestartEngine );

	registry->Shutdown();

	ReleaseMutex( hMutex );
	CloseHandle( hMutex );
	WSACleanup();

	return EXIT_SUCCESS;
}
