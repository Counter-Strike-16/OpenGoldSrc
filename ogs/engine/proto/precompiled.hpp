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

#pragma once

#include <direct.h>
#include <io.h>
#include <sys/stat.h>

//#include "version/version.h"

// common headers
#include "common/maintypes.h"
#include "common/SteamCommon.h"

// hlsdk headers
#include "hlsdk_pch.hpp"

// public headers
//#include "public/asmlib.h"
#include "public/iregistry.h"
//#include "public/iosutil.h"
#include "public/savegame_version.h"

// valve libs stuff
#include "public/tier0/commonmacros.h"
/*
#include "public/tier0/dbg.h"
#include "public/tier0/platform.h"

#include "public/tier1/utlbuffer.h"
#include "public/tier1/utlrbtree.h"
*/

// public rehlds headers
#include "public/rehlds/sys_shared.h"
#include "public/rehlds/crc32c.h"
#include "public/rehlds/rehlds_api.h"
#include "public/rehlds/rehlds_interfaces.h"
/*
#include "public/rehlds/modelgen.h"
#include "public/rehlds/spritegn.h"

#include "public/rehlds/bspfile.h"

#include "public/rehlds/static_map.h"
*/

// engine headers
#ifndef REHLDS_SELF
	#include "engine_pch.hpp"
#endif

// hookers headers
//#include "hookers/rehlds_debug.h"
//#include "hookers/memory.h"

#ifdef HOOK_ENGINE
	#include "hookers/hooker.h"
#endif

// testing suite
/*
#include "testsuite/anonymizer.h"
#include "testsuite/funccalls.h"
#include "testsuite/player.h"
#include "testsuite/recorder.h"
#include "testsuite/testsuite.h"
*/

// bzip2
//#include "bzip2/bzlib.h"

//#include "rehlds/rehlds_pch.hpp"

// game dll code
//#include "dlls/cdll_dll.h"

// shiftutil
#include "shiftutil/SharedLib.hpp"