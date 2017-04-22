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

#pragma once

#include "console/IConVarSystem.hpp"

/*
==============================================================

CVAR

==============================================================

cvar_t variables are used to hold scalar or string variables that can be changed or displayed at the console or prog code as well as accessed directly
in C code.

The user can access cvars from the console in three ways:
r_draworder			prints the current value
r_draworder 0		sets the current value to 0
set r_draworder 0	as above, but creates the cvar if not present
Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.

==============================================================
*/

struct IConsole;
struct IConVar;
class CFile;

typedef struct cvar_s cvar_t;

using tConVarList = std::list<IConVar*>;

struct IConVarIterator
{
	virtual IConVar *GetFirst() const = 0;
	virtual IConVar *GetLast() const = 0;
	
	virtual IConVar *GetNext(IConVar *apVar) const = 0;
	virtual IConVar *GetPrev(IConVar *apVar) const = 0;
};

class CConVarIterator : public IConVarIterator
{
public:
	CConVarIterator(const tConVarList &alstVars) : mlstVars(alstVars){}
	~CConVarIterator() = default;
	
	IConVar *GetFirst() const {return mlstVars.begin();}
	IConVar *GetLast() const {return mlstVars.end();}
	
	IConVar *GetNext(IConVar *apVar) const;
	IConVar *GetPrev(IConVar *apVar) const;
private:
	tConVarList mlstVars;
};

// CConVarList/CConVarContainer
class CConVarHandler : public IConVarSystem
{
public:
	CConVarHandler(IConsole *apConsole) : mpConsole(apConsole){}
	~CConVarHandler() = default;
	
	void Init();
	void Shutdown();
	
	void Register(IConVar *apVar);
	//bool Insert(cvar_t *apVar);
	
	IConVar *Create(const char *asName, const char *asDefVal, int anFlags, const char *asDesc = "");
	
	IConVar *Find(const char *asName);
	//IConVar *FindPrev(const char *asName);
	
	IConVarIterator *GetIterator() const;
	
	void SetVarString(const char *asName, const char *asvalue, int anFlags = 0);
	void SetVarInt(const char *asName, const int anValue, int anFlags = 0);
	void SetVarFloat(const char *asName, const float afValue, int anFlags = 0);
	void SetVarBool(const char *asName, const bool abValue, int anFlags = 0);
	
	const char *GetVarString(const char *asName) const;
	int GetVarInt(const char *asName) const;
	float GetVarFloat(const char *asName) const;
	bool GetVarBool(const char *asName) const;
	
	bool HandleCommand(const IConCmdArgs &CmdArgs);
	
	const char *CompleteVar(const char *asSearch, bool abForward);
	
	// appends lines containing "set variable value" for all variables
	// with the archive flag set to true.
	NOXREF void WriteVariables(CFile *f); // WriteVars; Use const ref here?
	
	//NOXREF int CountServerVariables();
	int CountFlaggedVariables(int anFlag);
	
	void UnlinkExternals(); // I'm not sure it's the right place for that
	//void UnlinkFlagged(int anFlag);
private:
	void Cmd_CvarList_f(const IConCmdArgs &aCmdArgs);
	
	IConsole *mpConsole{nullptr};

	//cvar_t *cvar_vars{nullptr};
	
	tConVarList mlstVars;
};