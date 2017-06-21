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

/// @file
/// @brief precompiled header for engine module

#pragma once

#include "system/common.hpp"
#include "system/server.hpp"
#include "system/system.hpp"
#include "system/engine.hpp"
#include "system/client.hpp"
#include "system/unicode_strtools.hpp"
#include "system/host.hpp"
#include "system/info.hpp"

#include "input/keys.hpp"

#include "resources/decal.hpp"

#include "network/delta.hpp"
#include "network/delta_jit.hpp"

#include "memory/zone.hpp"
#include "memory/mem.hpp"

#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "console/console.hpp"

#include "filesystem/filesystem_internal.hpp"
#include "filesystem/filesystem_.hpp"

#include "resources/cmodel.hpp"
#include "resources/model_rehlds.hpp"

#include "system/sv_log.hpp"
#include "system/host_cmd.hpp"

#include "network/sv_steam3.hpp"

#include "server/sv_user.hpp"

#include "physics/pmove.hpp"
#include "physics/pmovetst.hpp"

#include "world/pr_edict.hpp"
#include "world/pr_cmds.hpp"

#include "math/mathlib_e.hpp"
#include "math/sse_mathfun.hpp"

#include "world/world.hpp"
#include "world/ed_strpool.hpp"

#include "server/sv_phys.hpp"
#include "server/sv_move.hpp"
#include "server/sv_pmove.hpp"
#include "resources/studio_rehlds.hpp"
#include "network/net_ws.hpp"
#include "network/net_chan.hpp"

#include "client/tmessage.hpp"
#include "system/traceinit.hpp"
#include "filesystem/wad.hpp"
#include "resources/textures.hpp"
#include "graphics/vid.hpp"
#include "resources/l_studio.hpp"
#include "server/sv_remoteaccess.hpp"
#include "server/sv_upld.hpp"
#include "filesystem/hashpak.hpp"
#include "network/ipratelimit.hpp"
#include "network/ipratelimitWrapper.hpp"
#include "system/game.hpp"

#include "system/igame.h"
#include "system/iengine.h"