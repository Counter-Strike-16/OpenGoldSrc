#define	QUAKE_GAME			// as opposed to utilities

//define	PARANOID			// speed sapping error checking

#ifdef _WIN32
#pragma warning( disable : 4244 4127 4201 4214 4514 4305 4115 4018)
#endif

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>

//#include "bothdefs.h"

#include "system/common.h"
//#include "bspfile.h"
#include "graphics/vid.h"
#include "system/system.h"
#include "filesystem/filesystem_internal.h"
#include "memory/zone.h"
#include "memory/mem.h"
#include "common/mathlib.h"
#include "filesystem/wad.h"
#include "graphics/draw.h"
#include "console/cvar.h"
//#include "screen.h"
#include "network/net.h"
//#include "protocol.h"
#include "console/cmd.h"
//#include "sbar.h"
#include "sound/sound.h"
//#include "render.h"
#include "client/client.h"

#ifdef GLQUAKE
#include "graphics/gl_model.h"
#else
#include "model.h"
#include "d_iface.h"
#endif

//#include "input/input.h"
#include "input/keys.h"
#include "console/console.h"
//#include "view.h"
//#include "menu.h"
#include "common/crc.h"
//#include "cdaudio.h"
//#include "pmove.h"

#ifdef GLQUAKE
#include "graphics/glquake.h"
#endif

#define MAX_NUM_ARGVS	50

extern qboolean noclip_anglehack;

extern	cvar_t		sys_nostdout;

extern	cvar_t	password;

extern	byte		*host_basepal;
extern	byte		*host_colormap;
extern	int			host_framecount;	// incremented every frame, never reset			

void Host_ServerFrame (void);

qboolean Host_SimulationTime(float time);

void Host_Quit_f (void);

extern qboolean		msg_suppress_1;		// suppresses resolution and cache size console output
										//  an fullscreen DIB focus gain/loss