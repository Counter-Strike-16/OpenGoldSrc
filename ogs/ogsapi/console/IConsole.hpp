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
/// @brief console module interface

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

	/// Execute the command text
	virtual void ExecuteText(int anExecMode, const char *asText) = 0;

	/// Registers a command and the function to call for it
	virtual void AddCommand(const char *asName, pfnConCmdCallback afnCallback) = 0;

	/// Removes a command
	virtual void RemoveCommand(const char *asName) = 0;

	/// @return Argument count of current processing command
	virtual int Cmd_GetArgCount() const = 0;

	///
	virtual const char *Cmd_GetArgVal(int i) const = 0;

	/**
	* Register a new console variable from old cvar_t struct data (that already has the name, value, and optionally the archive elements set)
	*
	* @param apVar - reference to cvar_t struct
	* @return true if the cvar was successfully registered
	*/
	virtual bool RegisterCvar(const cvar_t &apCvar, const char *asDesc = "<nodesc>") const = 0;

	///
	virtual cvar_t *AddCvar(const char *asName, const char *asValue, int flags, const char *asDesc = "") const = 0;

	///
	virtual bool RemoveCvar(const char *asName) const = 0;
	
	/**
	* Finds the cvar with the given name
	*
	* @return NULL if there is no cvar with the given name
	*/
	// virtual IConVar *FindCvar(const char *asName) const = 0;
	virtual cvar_t *FindCvar(const char *asName) const = 0;

	///
	virtual void SetCvarString(const char *asName, const char *asValue) = 0;

	///
	virtual void SetCvarFloat(const char *asName, float afValue) = 0;
	
	///
	virtual const char *GetCvarString(const char *asName) const = 0;
	
	///
	virtual float GetCvarFloat(const char *asName) const = 0;
	
	/// Get pointer to console command alias list
	virtual cmdalias_t *GetAliasList() = 0;
};