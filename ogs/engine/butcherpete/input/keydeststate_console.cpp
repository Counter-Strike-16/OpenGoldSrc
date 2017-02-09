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

//#include "precompiled.hpp"
#include "input/KeyDestState.hpp"
#include "IConsole.hpp"

class CKeyDestState_Console : public IKeyDestState
{
public:
	CKeyDestState_Console(IConsole *apConsole) : mpConsole(apConsole){}
	
	/*
	====================
	Key_Console

	Interactive line editing and console scrollback
	====================
	*/
	void HandleKey(int key)
	{
	#ifdef _WIN32
		char *cmd, *s;
		int i;
		HANDLE th;
		char *clipText, *textCopied;
	#endif

		if(key == K_ENTER)
		{ // backslash text are commands, else chat
			if(key_lines[edit_line][1] == '\\' || key_lines[edit_line][1] == '/')
				Cbuf_AddText(key_lines[edit_line] + 2); // skip the >
			else if(CheckForCommand())
				Cbuf_AddText(key_lines[edit_line] + 1); // valid command
			else
			{ // convert to a chat message
				if(cls.state >= ca_connected)
					Cbuf_AddText("say ");
				Cbuf_AddText(key_lines[edit_line] + 1); // skip the >
			}

			Cbuf_AddText("\n");
			Con_Printf("%s\n", key_lines[edit_line]);
			edit_line = (edit_line + 1) & 31;
			history_line = edit_line;
			key_lines[edit_line][0] = ']';
			key_linepos = 1;
			//if(cls.state == ca_disconnected)
				//SCR_UpdateScreen(); // force an update, because the command
									// may take some time
			return;
		}

		if(key == K_TAB)
		{ // command completion
			CompleteCommand();
			return;
		}

		if(key == K_BACKSPACE || key == K_LEFTARROW)
		{
			if(key_linepos > 1)
				key_linepos--;
			return;
		}

		if(key == K_UPARROW)
		{
			do
			{
				history_line = (history_line - 1) & 31;
			} while(history_line != edit_line && !key_lines[history_line][1]);
			if(history_line == edit_line)
				history_line = (edit_line + 1) & 31;
			Q_strcpy(key_lines[edit_line], key_lines[history_line]);
			key_linepos = Q_strlen(key_lines[edit_line]);
			return;
		}

		if(key == K_DOWNARROW)
		{
			if(history_line == edit_line)
				return;
			do
			{
				history_line = (history_line + 1) & 31;
			} while(history_line != edit_line && !key_lines[history_line][1]);
			if(history_line == edit_line)
			{
				key_lines[edit_line][0] = ']';
				key_linepos = 1;
			}
			else
			{
				Q_strcpy(key_lines[edit_line], key_lines[history_line]);
				key_linepos = Q_strlen(key_lines[edit_line]);
			}
			return;
		}

		if(key == K_PGUP || key == K_MWHEELUP)
		{
			con->display -= 2;
			return;
		}

		if(key == K_PGDN || key == K_MWHEELDOWN)
		{
			con->display += 2;
			if(con->display > con->current)
				con->display = con->current;
			return;
		}

		if(key == K_HOME)
		{
			con->display = con->current - con_totallines + 10;
			return;
		}

		if(key == K_END)
		{
			con->display = con->current;
			return;
		}

	#ifdef _WIN32
		if((key == 'V' || key == 'v') && GetKeyState(VK_CONTROL) < 0)
		{
			if(OpenClipboard(NULL))
			{
				th = GetClipboardData(CF_TEXT);
				if(th)
				{
					//clipText = GlobalLock(th);
					if(clipText)
					{
						textCopied = (char*)malloc(GlobalSize(th) + 1);
						strcpy(textCopied, clipText);
						/* Substitutes a NULL for every token */ strtok(textCopied, "\n\r\b");
						i = strlen(textCopied);
						if(i + key_linepos >= MAXCMDLINE)
							i = MAXCMDLINE - key_linepos;
						if(i > 0)
						{
							textCopied[i] = 0;
							strcat(key_lines[edit_line], textCopied);
							key_linepos += i;
							;
						}
						free(textCopied);
					}
					GlobalUnlock(th);
				}
				CloseClipboard();
				return;
			}
		}
	#endif

		if(key < 32 || key > 127)
			return; // non printable

		if(key_linepos < MAXCMDLINE - 1)
		{
			key_lines[edit_line][key_linepos] = key;
			key_linepos++;
			key_lines[edit_line][key_linepos] = 0;
		};
	};
private:
	IConsole *mpConsole;
};