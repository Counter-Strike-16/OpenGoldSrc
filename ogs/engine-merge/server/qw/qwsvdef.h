
// quakedef.h -- primary header for server

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
#include <ctype.h>

#include "bothdefs.h"

#include "common.h"
#include "bspfile.h"
#include "sys.h"
#include "zone.h"
#include "mathlib.h"

#include "cvar.h"
#include "net.h"
#include "protocol.h"
#include "cmd.h"
#include "model.h"
#include "crc.h"
#include "progs.h"

#include "server.h"
#include "world.h"
#include "pmove.h"

extern	cvar_t		sys_nostdout;

void SV_Error (char *error, ...);
void SV_Init (quakeparms_t *parms);