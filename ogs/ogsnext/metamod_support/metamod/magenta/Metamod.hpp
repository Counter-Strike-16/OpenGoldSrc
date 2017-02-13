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

// Metamod.hpp - (main) description of metamod operations

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

#ifndef METAMOD_HPP
#define METAMOD_HPP

#include "types_meta.h"			// mBOOL
#include "comp_dep.h"
#include "meta_api.h"			// meta_globals_t, META_RES, etc
#include "mlist.h"				// MPluginList, etc
#include "mreg.h"				// MRegCmdList, etc
#include "conf_meta.h"			// MConfig
#include "osdep.h"				// NAME_MAX, etc
#include "mplayer.h"			// MPlayerList
#include "meta_eiface.h"        // HL_enginefuncs_t, meta_enginefuncs_t
#include "engine_t.h"           // engine_t, Engine

// File that lists plugins to load at startup
constexpr auto PLUGINS_INI = "addons/metamod/plugins.ini";
constexpr auto OLD_PLUGINS_INI = "metamod.ini";

// File that contains commands to metamod plugins at startup
constexpr auto EXEC_CFG = "addons/metamod/exec.cfg";
constexpr auto OLD_EXEC_CFG = "metaexec.cfg";

// Previously, file that contained path for an override-gamedll
constexpr auto OLD_GAMEDLL_TXT = "metagame.ini";

// Generic config file
constexpr auto CONFIG_INI = "addons/metamod/config.ini";

// cvar to contain version
extern DLLHIDDEN cvar_t meta_version;

// SDK variables for storing engine funcs and globals
extern DLLHIDDEN HL_enginefuncs_t g_engfuncs;
extern DLLHIDDEN globalvars_t *gpGlobals;

// Config structure
extern DLLHIDDEN MConfig *Config;

interface IConsole;

// Info about the game dll/mod
typedef struct gamedll_s
{
	char name[NAME_MAX];		// ie "cstrike" (from gamedir)
	const char *desc;				// ie "Counter-Strike"
	char gamedir[PATH_MAX];		// ie "/home/willday/half-life/cstrike"
	char pathname[PATH_MAX];	// ie "/home/willday/half-life/cstrike/dlls/cs_i386.so"
	char const *file;			// ie "cs_i386.so"
	char real_pathname[PATH_MAX];	// in case pathname overridden by bot, etc
	DLHANDLE handle;
	gamedll_funcs_t funcs;		// dllapi_table, newapi_table
} gamedll_t;

class CMetamod
{
public:
	CMetamod(IConsole *apConsole, enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals);
	~CMetamod();
	
	bool Init();
	void Shutdown();
	
	// Very first metamod function that's run
	// Do startup operations...
	DLLINTERNAL int Startup();
	
	// Set initial GameDLL fields (name, gamedir)
	// meta_errno values:
	//  - ME_NULLRESULT	getcwd failed
	DLLINTERNAL mBOOL InitGameDLL();
	
	// Load game DLL
	// meta_errno values:
	//  - ME_DLOPEN		couldn't dlopen game dll file
	//  - ME_DLMISSING	couldn't find required routine in game dll
	//                	(GiveFnptrsToDll, GetEntityAPI, GetEntityAPI2)
	DLLINTERNAL mBOOL LoadGameDLL();
private:
	IConsole *mpConsole{nullptr};
public:
	// Was global and marked as extern in the header
	// so that data will be public for now
	
	DLLHIDDEN gamedll_t GameDLL;
	
	// Data provided to plugins
	// Separate copies to prevent plugins from modifying "readable" parts
	// See meta_api.h for meta_globals_t structure
	DLLHIDDEN meta_globals_t PublicMetaGlobals;
	DLLHIDDEN meta_globals_t PrivateMetaGlobals;
	
	// Holds cached player info, right now only things for querying cvars
	// Max players is always 32, small enough that we can use a static array
	DLLHIDDEN MPlayerList g_Players;
	
	DLLHIDDEN int requestid_counter{0};
	DLLHIDDEN int metamod_not_loaded{0};
	
	// Our modified version of the engine funcs, to give to plugins
	DLLHIDDEN meta_enginefuncs_t g_plugin_engfuncs;
	
	// Metamod module handle
	DLLHIDDEN DLHANDLE metamod_handle;
	
	// List of plugins loaded/opened/running
	DLLHIDDEN MPluginList *Plugins{nullptr};
	
	// List of command functions registered by plugins
	DLLHIDDEN MRegCmdList *RegCmds{nullptr};
	
	// List of cvar structures registered by plugins
	DLLHIDDEN MRegCvarList *RegCvars{nullptr};
	
	// List of user messages registered by gamedll
	DLLHIDDEN MRegMsgList *RegMsgs{nullptr};
};

#endif // METAMOD_HPP