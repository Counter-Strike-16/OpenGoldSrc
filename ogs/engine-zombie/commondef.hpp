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
/// @brief common definitions & includes

#pragma once

/*
// speed sapping error checking
// define PARANOID

#ifdef _WIN32
#pragma warning(disable : 4244 4127 4201 4214 4514 4305 4115 4018)
#endif

#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#include "bothdefs.h"

#include "system/common.hpp"
#include "system/system.hpp"

#include "memory/mem.hpp"
#include "memory/zone.hpp"

#include "math/mathlib_e.hpp"

#include "console/cmd.hpp"
#include "console/console.hpp"
#include "console/cvar.hpp"

#include "filesystem/filesystem_internal.hpp"
#include "filesystem/wad.hpp"

#include "network/net.hpp"
//#include "network/protocol.hpp"

//#include "bspfile.h"

#include "graphics/draw.hpp"
#include "graphics/vid.hpp"
//#include "graphics/render.hpp"

//#include "screen.h"

//#include "sbar.h"

#include "sound/sound.hpp"

#include "client/client.hpp"

#ifdef GLQUAKE
#include "graphics/gl_model.h"
#else
#include "graphics/draw/d_iface.h"
#include "rehlds/model.h"
#endif

//#include "input/input.h"
#include "console/console.h"
#include "input/keys.h"
//#include "view.h"
//#include "menu.h"
#include "common/crc.h"
//#include "cdaudio.h"
//#include "pmove.h"

#ifdef GLQUAKE
#include "graphics/glquake.h"
#endif

#define MAX_NUM_ARGVS 50

extern qboolean noclip_anglehack;

extern cvar_t sys_nostdout;

extern cvar_t password;

extern byte *host_basepal;
extern byte *host_colormap;
extern int host_framecount; // incremented every frame, never reset

void Host_ServerFrame(void);

qboolean Host_SimulationTime(float time);

extern qboolean
msg_suppress_1; // suppresses resolution and cache size console output
                //  an fullscreen DIB focus gain/loss
*/