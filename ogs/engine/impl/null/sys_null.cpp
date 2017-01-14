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

#include "quakedef.hpp"
#include "errno.h"

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

void Sys_mkdir(char *path)
{
};

/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length)
{
};

void Sys_DebugLog(char *file, char *fmt, ...)
{
};

void Sys_Error(char *error, ...)
{
	va_list argptr;

	printf("I_Error: ");
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

void Sys_Sleep()
{
};

void Sys_HighFPPrecision()
{
};

void Sys_LowFPPrecision()
{
};

//=============================================================================

void main(int argc, char **argv)
{
	quakeparms_t parms;

	parms.memsize = 5861376;
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