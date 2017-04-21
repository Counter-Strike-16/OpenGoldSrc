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

#include "console/IConCmdSystem.hpp"

struct IConCmdArgs;

class CConCmdHandler : public IConCmdSystem
{
public:
	void Init();
	void Shutdown();
	
	bool IsCmdExists(const char *asName) const;
	
	void AddCommand(const char *asName, pfnConCmdCallback afnCallback, const char *asDesc = "");
	//void AddCommand(cmdFunction_t function, int flags, argCompletion_t argCompletion = NULL );
	void RemoveCommand(const char *asName);
	//void RemoveFlaggedCommands(int flags);
	
	IConCmd *GetFirstCmd() const;
	//IConCmd *GetList() const;
	// IConCmd should be returned from GetList in case we have GetNext method
	// in our IConCmd
	// or return IConCmdList which will be used as a wrapper around the
	// std list of IConCmd
	// the legacy code isn't connected with cmd_function_t struct so we can
	// freely change the way of work of this thing
	//IConCmd *GetCommands() const {return commands;}
	
	IConCmd *FindCmd(const char *asName) const;
	IConCmd *FindCmdPrev(const char *asName) const;
	
	const IConCmdArgs &TokenizeString(const char *asArgs) const;
private:
	void Cmd_Wait_f(); // static void Wait_f( const IConCmdArgs &args );
	void Cmd_StuffCmds_f();
	void Cmd_Exec_f(); // static void Exec_f( const IConCmdArgs &args );
	void Cmd_Echo_f(); // static void Echo_f( const IConCmdArgs &args );
	void Cmd_Alias_f();
	void Cmd_CmdList_f();
	
	cmd_function_t *cmd_functions{nullptr}; // CConCmd *commands;
	cmdalias_t *cmd_alias{nullptr};
};