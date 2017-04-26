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
#include "console/ConCmdHandler.hpp"

void CConCmdHandler::Shutdown()
{
	//cmd_functions = nullptr; // TODO: Check that memory from functions is released too
};

// It almost fully duplicates the Cmd_AddCommand

// Use this for call from user code, because it alloc string for the name
//AddMallocCommand
IConCmd *CConCmdHandler::Create(char *cmd_name, xcommand_t function, int flag, const char *asDesc)
{
	// Check in variables list
	//if(Cvar_FindVar(cmd_name) != NULL)
	{
		//mpConsole->Printf("%s: \"%s\" already defined as a var\n", __FUNCTION__, cmd_name);
		//return nullptr;
	};

	// Check if this command is already defined
	if(Exists(cmd_name))
	{
		mpConsole->Printf("%s: \"%s\" already defined\n", __FUNCTION__, cmd_name);
		return nullptr;
	};

	// Create cmd_function
	cmd_function_t *cmd = (cmd_function_t *)Mem_Malloc(sizeof(cmd_function_t));
	cmd->name = CopyString(cmd_name); // alloc string, so it will not dissapear on
	                                  // side modules unloading and to maintain
	                                  // the same name during run
	cmd->function = function ? function : Cmd_ForwardToServer;
	cmd->flags = flag;
	
	//mlstCmds.enplace_back(cmd_name, function, flag, asDesc);

	Insert(cmd);
	return cmd;
};

bool CConCmdHandler::Register(IConCmd *apCmd)
{
	char *sName = apCmd->GetName();
	
	for(auto It : mlstCmds)
		if(!Q_stricmp(It->GetName(), sName)
		{
			mpConsole->Printf("%s: Can't register a console comand - already registered! (%s)\n", __FUNCTION__, sName);
			return false;
		};
	
	Insert(apCmd);
	return true;
};

void CConCmdHandler::Insert(IConCmd *cmd)
{
	// Commands list is alphabetically sorted, search where to push
	IConCmd *c = cmd_functions;
	IConCmd **p = &cmd_functions;
	
	for(auto c : mlstCmds)
	{
		if(Q_stricmp(c->GetName(), cmd->GetName()) > 0)
		{
			// Current command name is bigger, insert before it
			cmd->next = c;
			*p = cmd;
			return;
		};
		
		p = &c->next;
		c = c->next;
	};

	// All commands in the list are lower then the new one
	cmd->next = nullptr;
	*p = cmd;
};

/*
============
Cmd_RemoveCommand
============
*/
void EXT_FUNC CConCmdHandler::Remove(const char *asName)
{
	auto prev = FindPrev(asName);
	
	if(prev)
	{
		auto cmd = prev->next;
		prev->next = cmd->next;
		
		// concmds are mostly internally used
		// there is no direct access to register a new cmd via cmd_function_t struct
		// so in case we preserve the same behavior we should free the mem here
		// in case we access IConCmd's from other dlls here we should call
		// the Release method of the IConCmd interface which will delete it in his way
		// (using it's own crt lib, we can't say that the other dll is using the same crt for sure)
		
		//It->Release(); // void CConCmd::Release(){delete this;}
		
		//Z_Free(cmd->name);
		//Mem_Free(cmd);
	};
};

NOXREF IConCmd *CConCmdHandler::Find(char *asName)
{
	NOXREFCHECK;

	for(auto It : mlstCmds)
	{
		if(!Q_stricmp(asName, It->GetName()))
			return It;
	};

	return nullptr;
};

IConCmd *CConCmdHandler::FindPrev(char *asName)
{
	if(!mlstCmds)
		return nullptr;

	for(auto It : mlstCmds)
	{
		if(!Q_stricmp(asName, cmd->GetNext()->GetName()))
			return It;
	};

	return nullptr;
};

bool CConCmdHandler::Exists(const char *asName)
{
	for(auto It : mlstCmds)
	{
		if(!Q_stricmp(asName, It->GetName()))
			return true;
	};

	return false;
};