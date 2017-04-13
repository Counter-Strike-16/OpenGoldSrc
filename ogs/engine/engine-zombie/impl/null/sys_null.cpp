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
/// @brief null system driver to aid porting efforts

#include "precompiled.hpp"
#include "errno.h"
#include "commondef.hpp"

/*
===============================================================================

FILE IO

===============================================================================
*/

/*
================
filelength
================
*/
int filelength(FILE *f)
{
	int pos = ftell(f);
	fseek(f, 0, SEEK_END);

	int end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
};

int Sys_FileTime(char *path)
{
	FILE *f = fopen(path, "rb");

	if(f)
	{
		fclose(f);
		return 1;
	};

	return -1;
};

void Sys_mkdir(char *path){};

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length){};

void Sys_DebugLog(char *file, char *fmt, ...){};

void Sys_Error(char *error, ...)
{
	va_list argptr;

	printf("Sys_Error: ");
	va_start(argptr, error);
	vprintf(error, argptr);
	va_end(argptr);
	printf("\n");

	exit(1);
};

void Sys_Printf(char *fmt, ...)
{
	va_list argptr;

	va_start(argptr, fmt);
	vprintf(fmt, argptr);
	va_end(argptr);
};

double Sys_FloatTime()
{
	static double t;

	t += 0.1f;

	return t;
};

void Sys_Sleep(){};

void Sys_HighFPPrecision(){};

void Sys_LowFPPrecision(){};

//=============================================================================

void main(int argc, char **argv)
{
	static quakeparms_t parms;

	parms.memsize = 8*1024*1024; // 5861376
	parms.membase = malloc(parms.memsize);
	parms.basedir = ".";

	COM_InitArgv(argc, argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	printf("Host_Init\n");
	Host_Init(&parms);

	while(1)
		Host_Frame(0.1f);
};

int curtime;

unsigned sys_frame_time;

void Sys_Error(char *error, ...)
{
	va_list argptr;

	printf("Sys_Error: ");
	va_start(argptr, error);
	vprintf(error, argptr);
	va_end(argptr);
	printf("\n");

	exit(1);
};

void Sys_Quit()
{
	exit(0);
};

void Sys_UnloadGame()
{
};

void *Sys_GetGameAPI(void *parms)
{
	return NULL;
};

char *Sys_ConsoleInput()
{
	return NULL;
};

void Sys_ConsoleOutput(char *string)
{
};

void Sys_SendKeyEvents()
{
};

void Sys_AppActivate()
{
};

void Sys_CopyProtect()
{
};

char *Sys_GetClipboardData()
{
	return NULL;
};

void *Hunk_Begin(int maxsize)
{
	return NULL;
};

void *Hunk_Alloc(int size)
{
	return NULL;
};

void Hunk_Free(void *buf)
{
};

int Hunk_End()
{
	return 0;
};

int Sys_Milliseconds()
{
	return 0;
};

char *Sys_FindFirst(char *path, unsigned musthave, unsigned canthave)
{
	return NULL;
};

char *Sys_FindNext(unsigned musthave, unsigned canthave)
{
	return NULL;
};

void Sys_FindClose()
{
};

void Sys_Init()
{
};



#define MAX_HANDLES             10
FILE    *sys_handles[MAX_HANDLES];

int             findhandle (void)
{
	int             i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}

int Sys_FileOpenRead (char *path, int *hndl)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = -1;
		return -1;
	}
	sys_handles[i] = f;
	*hndl = i;
	
	return filelength(f);
}

int Sys_FileOpenWrite (char *path)
{
	FILE    *f;
	int             i;
	
	i = findhandle ();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;
	
	return i;
}

void Sys_FileClose (int handle)
{
	fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek (int handle, int position)
{
	fseek (sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
	return fread (dest, 1, count, sys_handles[handle]);
}

int Sys_FileWrite (int handle, void *data, int count)
{
	return fwrite (data, 1, count, sys_handles[handle]);
}