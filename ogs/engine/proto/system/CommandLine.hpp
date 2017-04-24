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
/// @brief command line args

// Our internal command line component
// We can use the one from the vstdlib via CommandLine()->
// but it's currently unclear if someone else is using it
// in which case it could call the double init on it so
// we currently can't trust it
// NOTE: merge with CConCmdArgs?

#pragma once

class CCmdLine
{
public:
	CCmdLine(int argc, char **argv);
	~CCmdLine() = default;
	
	void Init(int argc, char **argv); // it's currently possible to multi-init the cmdline
	
	int FindArg(const char *asArg) const; // HasArg
	
	const char *GetArg(int anArg) const;
	
	const char *ToString() const {return com_cmdline;}
	
	int GetArgCount() const {return com_argc;}
private:
	static char *safeargvs[NUM_SAFE_ARGVS] =
	{
		"-stdvid",
		"-nolan",
		"-nosound",
		"-nocdaudio",
		"-nojoy",
		"-nomouse",
		"-dibonly"
	};
	
	int com_argc{0};
	
	char **com_argv{nullptr};
	
	char com_cmdline[COM_MAX_CMD_LINE];
};