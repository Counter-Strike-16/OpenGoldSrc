/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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

#include "precompiled.hpp"
#include "system/CommandLine.hpp"
#include "system/common.hpp"

CCmdLine::CCmdLine(int argc, char **argv)
{
	Init(argc, argv);
};

void CCmdLine::Init(int argc, char *argv[])
{
	bool safe = false;

	static char *largv[MAX_NUM_ARGVS + NUM_SAFE_ARGVS + 1];

	int j = 0;
	char *c = nullptr;

	// Reconstruct the full command line
	com_cmdline[0] = 0;
	
	for(int i = 0, j = 0; i < MAX_NUM_ARGVS && i < argc && j < COM_MAX_CMD_LINE - 1; i++)
	{
		c = argv[i];
		if(*c)
		{
			while(*c && j < COM_MAX_CMD_LINE - 1)
				com_cmdline[j++] = *c++;
			
			if(j >= COM_MAX_CMD_LINE - 1)
				break;
			
			com_cmdline[j++] = ' ';
		};
	};
	
	com_cmdline[j] = 0;

	// Copy args pointers to our array
	for(com_argc = 0; (com_argc < MAX_NUM_ARGVS) && (com_argc < argc); com_argc++)
	{
		largv[com_argc] = argv[com_argc];

		if(!Q_strcmp("-safe", argv[com_argc]))
			safe = true;
	};

	// Add arguments introducing more failsafeness
	if(safe)
	{
		// force all the safe-mode switches. Note that we reserved extra space in
		// case we need to add these, so we don't need an overflow check
		for(int i = 0; i < NUM_SAFE_ARGVS; ++i)
		{
			largv[com_argc] = safeargvs[i];
			com_argc++;
		};
	};

	largv[com_argc] = " ";
	com_argv = largv;
};

int CCmdLine::FindArg(const char *asArg) const
{
	for(int i = 1; i < com_argc; ++i)
	{
		if(!com_argv[i])
			continue;

		if(!Q_strcmp(asArg, (const char *)com_argv[i]))
			return i;
	};

	return 0;
};

const char *CCmdLine::GetArg(int anArg) const
{
	return com_argv[anArg];
};