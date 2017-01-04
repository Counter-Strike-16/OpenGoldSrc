/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
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
 
 //#include "precompiled.hpp"
 #include "console/cmd.hpp"
 
 sizebuf_t cmd_text;
 
 void Cbuf_Init()
{
	SZ_Alloc("cmd_text", &cmd_text, MAX_CMD_BUFFER);
}

// As new commands are generated from the console or keybindings,
// the text is added to the end of the command buffer.
void Cbuf_AddText(char *text)
{
	int len = Q_strlen(text);

	if(cmd_text.cursize + len >= cmd_text.maxsize)
	{
		Con_Printf(__FUNCTION__ ": overflow\n");
		return;
	}

	SZ_Write(&cmd_text, text, len);
}

// When a command wants to issue other commands immediately, the text is
// inserted at the beginning of the buffer, before any remaining unexecuted
// commands.
void Cbuf_InsertText(char *text)
{
	int addLen  = Q_strlen(text);
	int currLen = cmd_text.cursize;

	if(cmd_text.cursize + addLen >= cmd_text.maxsize)
	{
		Con_Printf(__FUNCTION__ ": overflow\n");
		return;
	}

#ifdef REHLDS_FIXES
	if(currLen)
		Q_memmove(cmd_text.data + addLen, cmd_text.data, currLen);

	Q_memcpy(cmd_text.data, text, addLen);
	cmd_text.cursize += addLen;

#else
	char *temp = NULL;
	if(currLen)
	{
		temp = (char *)Z_Malloc(currLen); // TODO: Optimize: better use memmove without need for a temp buffer
		Q_memcpy(temp, cmd_text.data, currLen);
		SZ_Clear(&cmd_text);
	}

	Cbuf_AddText(text);

	if(currLen)
	{
		SZ_Write(&cmd_text, temp, currLen);
		Z_Free(temp);
	}
#endif // REHLDS_FIXES
}

void Cbuf_InsertTextLines(char *text)
{
	int addLen  = Q_strlen(text);
	int currLen = cmd_text.cursize;

	if(cmd_text.cursize + addLen + 2 >= cmd_text.maxsize)
	{
		Con_Printf(__FUNCTION__ ": overflow\n");
		return;
	}

#ifdef REHLDS_FIXES
	if(currLen)
		Q_memmove(cmd_text.data + addLen + 2, cmd_text.data, currLen);

	cmd_text.data[0] = '\n'; // TODO: Why we need leading \n, if there is no commands in the start?
	Q_memcpy(&cmd_text.data[1], text, addLen);
	cmd_text.data[addLen + 1] = '\n';

	cmd_text.cursize += addLen + 2;

#else

	char *temp = NULL;
	if(currLen)
	{
		temp = (char *)Z_Malloc(currLen);
		Q_memcpy(temp, cmd_text.data, currLen);
		SZ_Clear(&cmd_text);
	}

	Cbuf_AddText("\n"); // TODO: Why we need leading \n, if there is no commands in the start?
	Cbuf_AddText(text);
	Cbuf_AddText("\n");

	if(currLen)
	{
		SZ_Write(&cmd_text, temp, currLen);
		Z_Free(temp);
	}
#endif // REHLDS_FIXES
}

// Pulls off \n terminated lines of text from the command buffer and sends
// them through Cmd_ExecuteString.  Stops when the buffer is empty.
// Normally called once per frame, but may be explicitly invoked.
// Do not call inside a command function!
void Cbuf_Execute()
{
	int   i;
	char *text;
	char  line[MAX_CMD_LINE];
	int   quotes;

	while(cmd_text.cursize)
	{
		// find a \n or ; line break
		text = (char *)cmd_text.data;

		quotes = 0;
		for(i = 0; i < cmd_text.cursize; i++)
		{
			if(text[i] == '"')
				quotes++;
			if(!(quotes & 1) && text[i] == ';')
				break; // don't break if inside a quoted string
			if(text[i] == '\n')
				break;
		}

#ifdef REHLDS_FIXES
		// save `i` if we truncate command
		int len;

		if(i > MAX_CMD_LINE - 1)
			len = MAX_CMD_LINE - 1;
		else
			len = i;

		Q_memcpy(line, text, len);
		line[len] = 0;
#else  // REHLDS_FIXES
		if(i > MAX_CMD_LINE - 1)
			i = MAX_CMD_LINE - 1;

		Q_memcpy(line, text, i);
		line[i] = 0;
#endif // REHLDS_FIXES

		// delete the text from the command buffer and move remaining commands down
		// this is necessary because commands (exec, alias) can insert data at the
		// beginning of the text buffer

		if(i == cmd_text.cursize)
			cmd_text.cursize = 0;
		else
		{
			i++;
			cmd_text.cursize -= i;
#ifdef REHLDS_FIXES
			// dst overlaps src
			Q_memmove(text, text + i, cmd_text.cursize);
#else  // REHLDS_FIXES
			Q_memcpy(text, text + i, cmd_text.cursize);
#endif // REHLDS_FIXES
		}

		// execute the command line
		Cmd_ExecuteString(line, src_command);

		if(cmd_wait)
		{
			// skip out while text still remains in buffer, leaving it
			// for next frame
			cmd_wait = FALSE;
			break;
		}
	}
}