// vi: set ts=4 sw=4 :
// vim: set tw=75 :

/*
*	This file is part of the Magenta Engine
*
*	Copyright (C) 2016 Sh1ft0x0EF
*
*	Magenta Engine is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	Magenta Engine is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with Magenta Engine. If not, see <http://www.gnu.org/licenses/>.
*
*	In addition, as a special exception, the author gives permission to
*	link the code of this program with the Half-Life Game Engine("HL
*	Engine") and Modified Game Libraries("MODs") developed by Valve,
*	L.L.C("Valve").  You must obey the GNU General Public License in all
*	respects for all of the code used other than the HL Engine and MODs
*	from Valve.  If you modify this file, you may extend this exception
*	to your version of the file, but you are not obligated to do so.  If
*	you do not wish to do so, delete this exception statement from your
*	version.
*/

// Metamod.cpp - (main) implementation of metamod operations

/*
 * Copyright (c) 2001-2006 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
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

#include "Metamod.hpp" // me
#include "log_meta.h" // META_LOG, etc
#include "enginecallbacks.h" // GET_GAME_DIR, etc
#include "support_meta.h" // valid_gamedir_file, etc
#include "osdep.h" // DLOPEN, getcwd, is_absolute_path,
#include "debug/Debug.hpp"
#include <thread>

#include <malloc.h>				// malloc, etc
#include <errno.h>				// errno, etc

#include <extdll.h>				// always

#include "mreg.h"				// class mCmdList, etc
#include "mutil.h"				// mutil_funcs_t, etc
#include "reg_support.h"		// meta_AddServerCommand, etc
#include "game_support.h"		// lookup_game, etc
#include "commands_meta.h"		// meta_register_cmdcvar, etc
#include "thread_logparse.h"	// logparse_handle, etc

#include "info_name.h"			// VNAME, etc
#include "vdate.h"				// COMPILE_TIME, etc
#include "linkent.h"

cvar_t meta_version = {"metamod_version", VVERSION, FCVAR_SERVER, 0, NULL};

//! Holds engine functionality callbacks
HL_enginefuncs_t g_engfuncs;
globalvars_t *gpGlobals;
engine_t Engine;

MConfig static_config;
MConfig *Config = &static_config;

option_t global_options[] =
{
	{"debuglevel",		CF_INT,			&Config->debuglevel,	"0"},
	{"gamedll",			CF_PATH,		&Config->gamedll,		NULL},
	{"plugins_file",	CF_PATH,		&Config->plugins_file,	PLUGINS_INI},
	{"exec_cfg",		CF_STR,			&Config->exec_cfg,		EXEC_CFG},
	{"autodetect",		CF_BOOL,		&Config->autodetect,	"yes"},
	{"clientmeta",		CF_BOOL,		&Config->clientmeta,	"yes"},
	
	// List terminator
	{NULL, CF_NONE, NULL, NULL}
};

CMetamod::CMetamod(IConsole *apConsole, /*IEngineShared *apEngine,*/ enginefuncs_t *apEngFuncsFromEngine, globalvars_t *pGlobals) : mpConsole(apConsole)
{
	LogMsg(eMsgType_Info, "Constructing the main metamod component...\n");
	
	#ifdef linux
		metamod_handle = get_module_handle_of_memptr((void*)&g_engfuncs);
	#endif
	
	gpGlobals = pGlobals;
	
	Engine.funcs = &g_engfuncs;
	Engine.globals = pGlobals;
	Engine.info.initialise(apEngFuncsFromEngine);
	
	g_engfuncs.initialise_interface(apEngFuncsFromEngine);
	
	// Load plugins, load game dll
	if(!Startup())
		metamod_not_loaded = 1;
};

CMetamod::~CMetamod()
{
	LogMsg(eMsgType_Info, "Destructing the main metamod component...\n");
};

bool CMetamod::Init()
{
	LogMsg(eMsgType_Info, "Initializing the main metamod component...\n");
	return true;
};

void CMetamod::Shutdown()
{
};

int CMetamod::Startup()
{
	char *cp, *mmfile = "", *cfile = "";
	
	META_CONS("   ");
	META_CONS("   %s version %s Copyright (c) 2001-%s %s", VNAME, VVERSION, COPYRIGHT_YEAR, VAUTHOR);
	META_CONS("     Patch: %s v%d Copyright (c) 2004-%s %s", VPATCH_NAME, VPATCH_IVERSION, VPATCH_COPYRIGHT_YEAR, VPATCH_AUTHOR);
	META_CONS("   %s comes with ABSOLUTELY NO WARRANTY; for details type `meta gpl'.", VNAME);
	META_CONS("   This is free software, and you are welcome to redistribute it");
	META_CONS("   under certain conditions; type `meta gpl' for details.");
	META_CONS("   ");
	
	META_LOG("%s v%s  %s", VNAME, VVERSION, VDATE);
	META_LOG("by %s", VAUTHOR);
	META_LOG("   %s", VURL);
	META_LOG(" Patch: %s v%d", VPATCH_NAME, VPATCH_IVERSION);
	META_LOG(" by %s", VPATCH_AUTHOR);
	META_LOG("    %s", VPATCH_WEBSITE);
	META_LOG("compiled: %s %s (%s)", COMPILE_TIME, COMPILE_TZONE, OPT_TYPE);
	
	// If running with "+developer", allow an opportunity to break in with a debugger
	if(mpConsole->GetConVar("developer")->GetBool())
		std::this_thread::sleep(1ms);
	
	// Get gamedir, very early on,
	// because it seems we need it all over the place here at the start
	if(!InitGameDLL())
	{
		META_ERROR("Failed to init game DLL; exiting...");
		return 0;
	};
	
	// Register various console commands and cvars.
	// Can I do these here, rather than waiting for GameDLLInit() ?  
	// Looks like it works okay..
	meta_register_cmdcvar();
	{
		// Dirty hacks
		int vers[4] = {RC_VERS_DWORD};
		char mvers[32];
		
		if(vers[2] == 0)
			safevoid_snprintf(mvers, sizeof(mvers), "%d.%dp%d", vers[0], vers[1], vers[3]);	
		else
			safevoid_snprintf(mvers, sizeof(mvers), "%d.%d.%dp%d", vers[0], vers[1], vers[2], vers[3]);	
		
		mpConsole->GetConVar(meta_version.name)->SetString(mvers);
	};
	
	// Set a slight debug level for developer mode, if debug level not already set
	if((int)CVAR_GET_FLOAT("developer") != 0 && (int)meta_debug.value == 0)
		mpConsole->GetConVar("meta_debug")->SetFloat(float)(meta_debug_value = 3));
	
	// Init default values
	Config->init(global_options);
	
	// Find config file
	cfile = CONFIG_INI;
	
	if((cp = LOCALINFO("mm_configfile")) && *cp != '\0')
	{
		META_LOG("Configfile specified via localinfo: %s", cp);
		
		if(valid_gamedir_file(cp))
			cfile = cp;
		else
			META_WARNING("Empty/missing config.ini file: %s; falling back to %s", cp, cfile);
	};
	
	// Load config file
	if(valid_gamedir_file(cfile))
		Config->load(cfile);
	else
		META_DEBUG(2, ("No config.ini file found: %s", CONFIG_INI));
	
	// Now, override config options with localinfo commandline options
	if((cp = LOCALINFO("mm_debug")) && *cp != '\0')
	{
		META_LOG("Debuglevel specified via localinfo: %s", cp);
		Config->set("debuglevel", cp);
	};
	
	if((cp = LOCALINFO("mm_gamedll")) && *cp != '\0')
	{
		META_LOG("Gamedll specified via localinfo: %s", cp);
		Config->set("gamedll", cp);
	};
	
	if((cp = LOCALINFO("mm_pluginsfile")) && *cp != '\0')
	{
		META_LOG("Pluginsfile specified via localinfo: %s", cp);
		Config->set("plugins_file", cp);
	};
	
	if((cp = LOCALINFO("mm_execcfg")) && *cp != '\0')
	{
		META_LOG("Execcfg specified via localinfo: %s", cp);
		Config->set("exec_cfg", cp);
	};
	
	if((cp = LOCALINFO("mm_autodetect")) && *cp != '\0')
	{
		META_LOG("Autodetect specified via localinfo: %s", cp);
		Config->set("autodetect", cp);
	};
	
	if((cp = LOCALINFO("mm_clientmeta")) && *cp != '\0')
	{
		META_LOG("Clientmeta specified via localinfo: %s", cp);
		Config->set("clientmeta", cp);
	};
	
	// Check for an initial debug level, since cfg files don't get exec'd until later
	if(Config->debuglevel != 0)
		mpConsole->GetConVar("meta_debug")->SetFloat((float)(meta_debug_value = Config->debuglevel));
	
	// Prepare for registered commands from plugins
	RegCmds = new MRegCmdList();
	RegCvars = new MRegCvarList();
	
	// Prepare for registered user messages from gamedll
	RegMsgs = new MRegMsgList();
	
	// Copy, and store pointer in Engine struct.  Yes, we could just store
	// the actual engine_t struct in Engine, but then it wouldn't be a
	// pointer to match the other g_engfuncs
	g_plugin_engfuncs.set_from(Engine.funcs);
	Engine.pl_funcs = &g_plugin_engfuncs;
	
	// Substitute our special versions of various commands
	Engine.pl_funcs->pfnAddServerCommand = meta_AddServerCommand;
	Engine.pl_funcs->pfnCVarRegister = meta_CVarRegister;
	Engine.pl_funcs->pfnCvar_RegisterVariable = meta_CVarRegister;
	Engine.pl_funcs->pfnRegUserMsg = meta_RegUserMsg;
	
	if(IS_VALID_PTR((void*)Engine.pl_funcs->pfnQueryClientCvarValue))
		Engine.pl_funcs->pfnQueryClientCvarValue = meta_QueryClientCvarValue;
	else
		Engine.pl_funcs->pfnQueryClientCvarValue = NULL;
	
	if(!IS_VALID_PTR((void*)Engine.pl_funcs->pfnQueryClientCvarValue2))
		Engine.pl_funcs->pfnQueryClientCvarValue2 = NULL;
	
	// Before, we loaded plugins before loading the game DLL, so that if no
	// plugins caught engine functions, we could pass engine funcs straight
	// to game dll, rather than acting as intermediary.  (Should perform
	// better, right?)
	//
	// But since a plugin can be loaded at any time, we have to go ahead
	// and catch the engine funcs regardless.  Also, we want to give each
	// plugin a copy of the gameDLL's api tables, in case they need to call
	// API functions directly.
	//
	// Thus, load gameDLL first, then plugins.
	//
	// However, we have to init the Plugins object first, because if the
	// gamedll calls engine functions during GiveFnptrsToDll (like hpb_bot
	// does) then it needs to be non-null so META_ENGINE_HANDLE won't crash.
	//
	// However, having replaced valid_file with valid_gamedir_file, we need
	// to at least initialize the gameDLL to include the gamedir, before 
	// looking for plugins.ini.
	//
	// In fact, we need gamedir even earlier, so moved up above

	// Fall back to old plugins filename, if configured one isn't found
	mmfile = PLUGINS_INI;
	
	if(!valid_gamedir_file(PLUGINS_INI) && valid_gamedir_file(OLD_PLUGINS_INI))
		mmfile = OLD_PLUGINS_INI;
	
	if(valid_gamedir_file(Config->plugins_file))
		mmfile = Config->plugins_file;
	else
		META_WARNING("Plugins file is empty/missing: %s; falling back to %s", Config->plugins_file, mmfile);
	
	Plugins = new MPluginList(mmfile);
	
	if(!LoadGameDLL())
	{
		META_ERROR("Failed to load game DLL; exiting...");
		return 0;
	};
	
	if(!Plugins->load())
	{
		META_WARNING("Failure to load plugins...");
		// Exit on failure here? Dunno...
	};
	
	// Allow for commands to metamod plugins at startup
	// Autoexec.cfg is read too early, and server.cfg is read too late
	//
	// Only attempt load if the file appears to exist and be non-empty, to
	// avoid confusing users with "couldn't exec exec.cfg" console messages
	if(valid_gamedir_file(Config->exec_cfg))
		mmfile = Config->exec_cfg;
	else if(valid_gamedir_file(OLD_EXEC_CFG))
		mmfile = OLD_EXEC_CFG;
	else
		mmfile = NULL;
	
	if(mmfile)
	{
		if(mmfile[0] == '/')
			META_WARNING("Cannot exec absolute pathnames: %s", mmfile);
		else
		{
			char cmd[NAME_MAX];
			META_LOG("Exec'ing metamod exec.cfg: %s...", mmfile);
			safevoid_snprintf(cmd, sizeof(cmd), "exec %s\n", mmfile);
			SERVER_COMMAND(cmd);
		};
	};
	
	return 1;
};

mBOOL CMetamod::InitGameDLL()
{
	char gamedir[PATH_MAX];
	char *cp;
	
	memset(&GameDLL, 0, sizeof(GameDLL));
	
	GET_GAME_DIR(gamedir);
	normalize_pathname(gamedir);
	//
	// As of 1.1.1.1, the engine routine GET_GAME_DIR no longer returns a
	// full-pathname, but rather just the path specified as the argument to
	// "-game".
	//
	// However, since we have to work properly under both the new version
	// as well as previous versions, we have to support both possibilities.
	//
	// Note: the code has always assumed the server op wouldn't do:
	//    hlds -game other/firearms
	//
	if(is_absolute_path(gamedir))
	{
		// Old style; GET_GAME_DIR returned full pathname.  Copy this into
		// our gamedir, and truncate to get the game name.
		// (note check for both linux and win32 full pathname.)
		STRNCPY(GameDLL.gamedir, gamedir, sizeof(GameDLL.gamedir));
		cp = strrchr(gamedir, '/') + 1;
		STRNCPY(GameDLL.name, cp, sizeof(GameDLL.name));
	}
	else
	{
		// New style; GET_GAME_DIR returned game name.  Copy this into our
		// game name, and prepend the current working directory.
		char buf[PATH_MAX];
		if(!getcwd(buf, sizeof(buf)))
		{
			META_WARNING("dll: Couldn't get cwd; %s", strerror(errno));
			RETURN_ERRNO(mFALSE, ME_NULLRESULT);
		};
		
		safevoid_snprintf(GameDLL.gamedir, sizeof(GameDLL.gamedir), "%s/%s", buf, gamedir);
		STRNCPY(GameDLL.name, gamedir, sizeof(GameDLL.name));
	};
	
	META_DEBUG(3, ("Game: %s", GameDLL.name));
	return mTRUE;
};

mBOOL CMetamod::LoadGameDLL()
{
	int iface_vers;
	int found = 0;
	
	GIVE_ENGINE_FUNCTIONS_FN pfn_give_engfuncs;
	GETNEWDLLFUNCTIONS_FN pfn_getapinew;
	GETENTITYAPI2_FN pfn_getapi2;
	GETENTITYAPI_FN pfn_getapi;
	
	if(!setup_gamedll(&GameDLL))
	{
		META_WARNING("dll: Unrecognized game: %s", GameDLL.name);
		// meta_errno should be already set in lookup_game()
		return(mFALSE);
	};
	
	// Open the game DLL
	if(!(GameDLL.handle = DLOPEN(GameDLL.pathname)))
	{
		META_WARNING("dll: Couldn't load game DLL %s: %s", GameDLL.pathname, DLERROR());
		RETURN_ERRNO(mFALSE, ME_DLOPEN);
	};

	// Used to only pass our table of engine funcs if a loaded plugin
	// wanted to catch one of the functions, but now that plugins are
	// dynamically loadable at any time, we have to always pass our table,
	// so that any plugin loaded later can catch what they need to.
	if((pfn_give_engfuncs = (GIVE_ENGINE_FUNCTIONS_FN) DLSYM(GameDLL.handle, "GiveFnptrsToDll")))
	{
			pfn_give_engfuncs(&meta_engfuncs, gpGlobals);
			META_DEBUG(3, ("dll: Game '%s': Called GiveFnptrsToDll", GameDLL.name));
			
			//activate linkent-replacement after give_engfuncs so that if game dll is 
			//plugin too and uses same method we get combined export table of plugin 
			//and game dll
			if(!init_linkent_replacement(metamod_handle, GameDLL.handle))
			{
				META_WARNING("dll: Couldn't load linkent replacement for game DLL");
				RETURN_ERRNO(mFALSE, ME_DLERROR);
			};
	}
	else
	{
		META_WARNING("dll: Couldn't find GiveFnptrsToDll() in game DLL '%s': %s", GameDLL.name, DLERROR());
		RETURN_ERRNO(mFALSE, ME_DLMISSING);
	};
	
	// Yes...another macro.
#define GET_FUNC_TABLE_FROM_GAME(gamedll, pfnGetFuncs, STR_GetFuncs, struct_field, API_TYPE, TABLE_TYPE, vers_pass, vers_int, vers_want, gotit) \
		if((pfnGetFuncs = (API_TYPE) DLSYM(gamedll.handle, STR_GetFuncs))) { \
			gamedll.funcs.struct_field = (TABLE_TYPE*) calloc(1, sizeof(TABLE_TYPE)); \
			if(!gamedll.funcs.struct_field) {\
				META_WARNING("malloc failed for gamedll struct_field: %s", STR_GetFuncs); \
			} \
			else if(pfnGetFuncs(gamedll.funcs.struct_field, vers_pass)) { \
				META_DEBUG(3, ("dll: Game '%s': Found %s", gamedll.name, STR_GetFuncs)); \
				gotit=1; \
			} \
			else { \
				META_WARNING("dll: Failure calling %s in game '%s'", STR_GetFuncs, gamedll.name); \
				free(gamedll.funcs.struct_field); \
				gamedll.funcs.struct_field=NULL; \
				if(vers_int != vers_want) { \
					META_WARNING("dll: Interface version didn't match; we wanted %d, they had %d", vers_want, vers_int); \
					/* reproduce error from engine */ \
					META_CONS("=================="); \
					META_CONS("Game DLL version mismatch"); \
					META_CONS("DLL version is %d, engine version is %d", vers_int, vers_want); \
					if(vers_int > vers_want) \
						META_CONS("Engine appears to be outdated, check for updates"); \
					else \
						META_CONS("The game DLL for %s appears to be outdated, check for updates", GameDLL.name); \
					META_CONS("=================="); \
					ALERT(at_error, "Exiting...\n"); \
				} \
			} \
		} \
		else { \
			META_DEBUG(5, ("dll: Game '%s': No %s", gamedll.name, STR_GetFuncs)); \
			gamedll.funcs.struct_field=NULL; \
		}
	
	// Look for API-NEW interface in Game dll.  We do this before API2/API, because
	// that's what the engine appears to do..
	iface_vers = NEW_DLL_FUNCTIONS_VERSION;
	
	GET_FUNC_TABLE_FROM_GAME(GameDLL, pfn_getapinew, "GetNewDLLFunctions", newapi_table, 
			GETNEWDLLFUNCTIONS_FN, meta_new_dll_functions_t,
			&iface_vers, iface_vers, NEW_DLL_FUNCTIONS_VERSION, found);
	
	// Look for API2 interface in plugin; preferred over API-1
	found = 0;
	iface_vers = INTERFACE_VERSION;
	
	GET_FUNC_TABLE_FROM_GAME(GameDLL, pfn_getapi2, "GetEntityAPI2", dllapi_table, 
			GETENTITYAPI2_FN, DLL_FUNCTIONS,
			&iface_vers, iface_vers, INTERFACE_VERSION, found);

	// Look for API-1 in plugin, if API2 interface wasn't found
	if(!found)
	{
		found = 0;
		GET_FUNC_TABLE_FROM_GAME(GameDLL, pfn_getapi, "GetEntityAPI", dllapi_table, 
				GETENTITYAPI_FN, DLL_FUNCTIONS,
				INTERFACE_VERSION, INTERFACE_VERSION, INTERFACE_VERSION, found);
	};
	
	// If didn't find either, return failure
	if(!found)
	{
		META_WARNING("dll: Couldn't find either GetEntityAPI nor GetEntityAPI2 in game DLL '%s'", GameDLL.name);
		RETURN_ERRNO(mFALSE, ME_DLMISSING);
	};
	
	META_LOG("Game DLL for '%s' loaded successfully", GameDLL.desc);
	return mTRUE;
};