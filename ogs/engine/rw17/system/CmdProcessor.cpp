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
#include "system/CmdProcessor.hpp"
#include "console/ConCmdHandler.hpp"
#include "console/ConVarHandler.hpp"

/*
============
idCmdSystemLocal::ExecuteCommandText

Tokenizes, then executes.
============
*/
void CCmdProcessor::ExecCmdText(const char *asText)
{
	//ExecuteTokenizedString(CConCmdArgs(asText, false));
	
	mpCmdBuffer->InsertText(asText);
	mpCmdBuffer->Exec();
};

void CCmdProcessor::AppendCmdText(const char *asText)
{
	mpCmdBuffer->AppendText(asText);
};

void CCmdProcessor::BufferCmdText(cmdExecution_t aExecMode, const char *asText);
{
	switch(aExecMode)
	{
		case CMD_EXEC_NOW:
			ExecCmdText(asText);
			break;
		case CMD_EXEC_INSERT:
			InsertCmdText(asText);
			break;
		case CMD_EXEC_APPEND:
			AppendCmdText(asText);
			break;
		default:
			common->FatalError("idCmdSystemLocal::BufferCommandText: bad exec type");
	};
};

void CCmdProcessor::ExecCmdBuffer()
{
	mpCmdBuffer->Exec();
};

//void CCmdProcessor::ExecuteTokenizedString(const IConCmdArgs &aCmdArgs)
//{
//};

//void CCmdProcessor::InsertCmdText(const char *asText)
//{
//};

void CCmdProcessor::ExecuteString(const char *asText) // CConCmdHandler::ExecCmd
{
	CConCmdArgs CmdArgs;
	TokenizeString(asText, CmdArgs);
	
	// These components are directly checking their internal data
	// so we can't place their methods code here
	// But I think it's better to remove the additional logic of cmd execution
	// from those levels and make them work mostly as containers
	
	/// Solution #1
	
	// mpCmdHandler = mpCmdList = mpCmdContainer
	//IConCmd *pCmd = mpCmdHandler->GetCmd(CmdArgs[0]);
	
	//if(pCmd)
	//{
		//pCmd->Exec(CmdArgs);
		//return;
	//};
	
	/// Solution #2
	
	if(mpCmdHandler->ExecCmd(CmdArgs))
		return;
	
	///
	
	if(mpVarHandler->HandleCommand(CmdArgs)) // that's harder
		return;
	
	if(mpLocalClient->GetState() >= eClientState::Connected)
		mpLocalClient->ForwardCmdToServer(CmdArgs); // mpNetwork
};

/*
============
idCmdSystemLocal::SetupReloadEngine

Setup args that will be applied after the engine restart
============
*/
void CCmdProcessor::SetupReloadEngine(const IConCmdArgs &aCmdArgs)
{
	BufferCommandText(CMD_EXEC_APPEND, "reloadEngine\n");
	mPostReloadArgs = aCmdArgs;
};

/*
============
idCmdSystemLocal::PostReloadEngine

Apply our post-reload args
============
*/
bool CCmdProcessor::PostReloadEngine()
{
	if(!mPostReloadArgs.GetCount())
		return false;
	
	BufferCommandArgs(CMD_EXEC_APPEND, mPostReloadArgs);
	mPostReloadArgs.Clear();
	return true;
};