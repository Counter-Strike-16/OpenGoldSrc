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
/// @brief Console module interface draft

#pragma once

#include "public/interface.h"
#include "common/cvardef.h"
#include "common/con_nprint.h"

const char OGS_CONSOLE_INTERFACE_VERSION[] = "OGSConsole001";

/// Console command callback function
typedef void (*pfnConCmdCallback)();

typedef struct cmdalias_s cmdalias_t;

struct IConsole : public IBaseInterface
{
	/// Prints a formatted message into the console
	virtual void Printf(int anPrintLevel, const char *asMsg, ...) = 0;
	
	/**
	* Similar to Con_NPrintf, but allows specifying custom text color and duration information
	*/
	//virtual void NotifyPrintfEx(const con_nprint_t *apInfo, PRINTF_FORMAT_STRING const char *asMsg, ...) = 0;

	/// Execute a command text
	virtual void ExecuteText(int anExecMode, const char *asText) = 0;

	///
	virtual void AddCommand(const char *asName, pfnConCmdCallback afnCallback) = 0;

	///
	virtual void RemoveCommand(const char *asName) = 0;

	/// @return Argument count of current processing command
	virtual int Cmd_Argc() const = 0;

	///
	virtual const char *Cmd_Argv(int i) const = 0;

	///
	virtual bool Cvar_Register(cvar_t *apCvar) const = 0;

	///
	virtual cvar_t *Cvar_Add(const char *asName, const char *asValue, int flags, const char *asDesc = "") const = 0;

	///
	virtual bool Cvar_Remove(const char *asName) const = 0;

	///
	virtual cvar_t *Cvar_Get(const char *asName) const = 0;

	///
	virtual cvar_t *Cvar_Set(const char *asName, const char *asValue) const = 0;

	///
	virtual void Cvar_SetValue(const char *asName, float afValue) = 0;
	
	///
	virtual cmdalias_t *GetAliasList() = 0;
};