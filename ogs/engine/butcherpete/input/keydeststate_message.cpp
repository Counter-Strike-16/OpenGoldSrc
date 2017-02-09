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
#include "console/cmd.hpp"

class CKeyDestState_Message : public IKeyDestState
{
public:
	//CKeyDestState_Message();
	
	void HandleKey(int key)
	{
		if(key == K_ENTER || key == K_KP_ENTER)
		{
			if(chat_team)
				Cbuf_AddText("say_team \"");
			else
				Cbuf_AddText("say \"");
			
			Cbuf_AddText(chat_buffer);
			Cbuf_AddText("\"\n");

			key_dest = key_game;
			chat_bufferlen = 0;
			chat_buffer[0] = 0;
			return;
		}

		if(key == K_ESCAPE)
		{
			key_dest = key_game;
			chat_bufferlen = 0;
			chat_buffer[0] = 0;
			return;
		}

		if(key < 32 || key > 127)
			return; // non printable

		if(key == K_BACKSPACE)
		{
			if(chat_bufferlen)
			{
				chat_bufferlen--;
				chat_buffer[chat_bufferlen] = 0;
			}
			return;
		}

		if(chat_bufferlen == sizeof(chat_buffer) - 1)
			return; // all full

		chat_buffer[chat_bufferlen++] = key;
		chat_buffer[chat_bufferlen] = 0;
	};
private:
	char chat_buffer[MAXCMDLINE];
	int chat_bufferlen = 0;
	qboolean chat_team;
};