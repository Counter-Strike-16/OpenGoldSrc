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
/// @brief script command processing component

#pragma once

struct IConsole;
class CSizeBuffer;
class CCmdProcessor;

class CCmdBuffer
{
public:
	CCmdBuffer(IConsole *apConsole, CCmdProcessor *apCmdProcessor) : mpConsole(apConsole), mpCmdProcessor(apCmdProcessor){}
	
	// Allocates an initial text buffer that will grow as needed
	void Init();

	//void Shutdown();
	
	// As new commands are generated from the console or keybindings,
	// the text is added to the end of the command buffer
	void AddText(char *text);

	// When a command wants to issue other commands immediately, the text is
	// inserted at the beginning of the buffer, before any remaining unexecuted
	// commands
	void InsertText(char *text);

	void InsertTextLines(char *text);

	// Pulls off \n terminated lines of text from the command buffer and sends
	// them through Cmd_ExecuteString.  Stops when the buffer is empty.
	// Normally called once per frame, but may be explicitly invoked.
	// Do not call inside a command function!
	void Execute();
	
	// Leave the execution for next frame
	void SetWait(bool abWait){cmd_wait = true;}
	//void SetWait(int anFrames){wait = anFrames;}
private:
	IConsole *mpConsole{nullptr};
	CCmdProcessor *mpCmdProcessor{nullptr};
	
	CSizeBuffer *cmd_text{nullptr}; // sizebuf_t cmd_text;
	
	bool cmd_wait{false}; // int wait; - framenum to wait
};