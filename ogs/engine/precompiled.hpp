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

/*
#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif
*/

#include <direct.h>
#include <io.h>
#include <sys/stat.h>

#include "maintypes.h"

#include "common/commontypes.h"
#include "common/mathlib.h"
#include "common/const.h"

#include "common/com_model.h"
#include "common/usercmd.h"
#include "engine/eiface.h"

/*
#include "rehlds/modelgen.h"
#include "rehlds/spritegn.h"
*/

#include "engine/custom.h"
#include "engine/customentity.h"
#include "engine/edict.h"
#include "engine/progdefs.h"
#include "engine/progs.h"
#include "engine/shake.h"
#include "engine/studio.h"

#include "common/beamdef.h"
#include "common/cl_entity.h"
#include "common/con_nprint.h"
#include "common/crc.h"
#include "common/cvardef.h"
#include "common/demo_api.h"
#include "common/dlight.h"
#include "common/entity_state.h"
#include "common/entity_types.h"
#include "common/enums.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/event_flags.h"
#include "common/ivoicetweak.h"
#include "common/kbutton.h"
#include "common/net_api.h"
#include "common/netadr.h"
#include "common/particledef.h"
#include "common/qfont.h"
#include "common/r_efx.h"
#include "common/r_studioint.h"
#include "common/ref_params.h"
#include "common/screenfade.h"
#include "common/Sequence.h"
#include "common/triangleapi.h"
#include "common/weaponinfo.h"

//#include "rehlds/bspfile.h"

#include "public/keydefs.h"

#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_movevars.h"
#include "pm_shared/pm_shared.h"

//#include "version/version.h"

/*
#include "asmlib.h"
#include "common/mathlib.h"
#include "math/sse_mathfun.h"
#include "public/archtypes.h"

//#include "sys_shared.h"
//#include "crc32c.h"
#include "rehlds/static_map.h"

//#include "ed_strpool.h"
*/

// All headers of engine module
// Needed for external code
#ifndef REHLDS_SELF
	#include "engine_pch.h"
#endif

//#include "memory.h"

#include "SteamCommon.h"

#include "platform_rehlds.h"

//#include "RehldsRuntimeConfig.h"
//#include "rehlds_debug.h"

/*
#ifdef HOOK_ENGINE
#include "hooker.h"
#endif

// valve libs stuff
#include "dbg/dbg.h"
#include "tier0/platform.h"
*/

#include "iregistry.h"

#include "public/interface.h"

/*
#include "tier1/utlbuffer.h"
#include "tier1/utlrbtree.h"

#include "iosutil.h"

// testsuite
/*
#include "anonymizer.h"
#include "funccalls.h"
#include "player.h"
#include "recorder.h"
#include "testsuite.h"

//#include "bzip2/bzlib.h"
#include "system/igame.h"

#include "hookchains_impl.h"
*/

#include "rehlds/rehlds_api.h"
#include "rehlds/rehlds_interfaces.h"

#include "rehlds_api_impl.h"
#include "rehlds_interfaces_impl.h"

/*
#include "system/iengine.h"
//#include "FlightRecorderImpl.h"
//#include "flight_recorder.h"
//#include "rehlds_security.h"

#include "dlls/cdll_dll.h"
*/

#include "shiftutil/SharedLib.hpp"