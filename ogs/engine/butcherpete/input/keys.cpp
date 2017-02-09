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
#include "input/keys.hpp"
#include "memory/zone.hpp"
#include "system/system.hpp"
//#include "system/common.hpp"
#include "console/console.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "client/client.hpp"
#include "public/keydefs.h"

#ifdef _WINDOWS
#include <windows.h>
#endif

// key up events are sent even if in console mode

const int MAXCMDLINE = 256;

char key_lines[32][MAXCMDLINE];
int key_linepos;
int shift_down = 0;
int key_lastpress;

int edit_line = 0;
int history_line = 0;

keydest_t key_dest;

int key_count; // incremented every key event

char *keybindings[256];

qboolean consolekeys[256]; // if true, can't be rebound while in console
qboolean menubound[256];   // if true, can't be rebound while in menu

int keyshift[256];    // key to map to if shift held down in console
int key_repeats[256]; // if > 1, it is autorepeating

qboolean keydown[256];

typedef struct
{
	char *name;
	int keynum;
} keyname_t;

keyname_t keynames[] =
{
	{ "TAB", K_TAB },
	{ "ENTER", K_ENTER },
	{ "ESCAPE", K_ESCAPE },
	{ "SPACE", K_SPACE },
	{ "BACKSPACE", K_BACKSPACE },
	{ "UPARROW", K_UPARROW },
	{ "DOWNARROW", K_DOWNARROW },
	{ "LEFTARROW", K_LEFTARROW },
	{ "RIGHTARROW", K_RIGHTARROW },

	{ "ALT", K_ALT },
	{ "CTRL", K_CTRL },
	{ "SHIFT", K_SHIFT },

	{ "F1", K_F1 },
	{ "F2", K_F2 },
	{ "F3", K_F3 },
	{ "F4", K_F4 },
	{ "F5", K_F5 },
	{ "F6", K_F6 },
	{ "F7", K_F7 },
	{ "F8", K_F8 },
	{ "F9", K_F9 },
	{ "F10", K_F10 },
	{ "F11", K_F11 },
	{ "F12", K_F12 },

	{ "INS", K_INS },
	{ "DEL", K_DEL },
	{ "PGDN", K_PGDN },
	{ "PGUP", K_PGUP },
	{ "HOME", K_HOME },
	{ "END", K_END },

	{ "MOUSE1", K_MOUSE1 },
	{ "MOUSE2", K_MOUSE2 },
	{ "MOUSE3", K_MOUSE3 },

	{ "JOY1", K_JOY1 },
	{ "JOY2", K_JOY2 },
	{ "JOY3", K_JOY3 },
	{ "JOY4", K_JOY4 },

	{ "AUX1", K_AUX1 },
	{ "AUX2", K_AUX2 },
	{ "AUX3", K_AUX3 },
	{ "AUX4", K_AUX4 },
	{ "AUX5", K_AUX5 },
	{ "AUX6", K_AUX6 },
	{ "AUX7", K_AUX7 },
	{ "AUX8", K_AUX8 },
	{ "AUX9", K_AUX9 },
	{ "AUX10", K_AUX10 },
	{ "AUX11", K_AUX11 },
	{ "AUX12", K_AUX12 },
	{ "AUX13", K_AUX13 },
	{ "AUX14", K_AUX14 },
	{ "AUX15", K_AUX15 },
	{ "AUX16", K_AUX16 },
	{ "AUX17", K_AUX17 },
	{ "AUX18", K_AUX18 },
	{ "AUX19", K_AUX19 },
	{ "AUX20", K_AUX20 },
	{ "AUX21", K_AUX21 },
	{ "AUX22", K_AUX22 },
	{ "AUX23", K_AUX23 },
	{ "AUX24", K_AUX24 },
	{ "AUX25", K_AUX25 },
	{ "AUX26", K_AUX26 },
	{ "AUX27", K_AUX27 },
	{ "AUX28", K_AUX28 },
	{ "AUX29", K_AUX29 },
	{ "AUX30", K_AUX30 },
	{ "AUX31", K_AUX31 },
	{ "AUX32", K_AUX32 },
	
	{"KP_HOME",			K_KP_HOME },
	{"KP_UPARROW",		K_KP_UPARROW },
	{"KP_PGUP",			K_KP_PGUP },
	{"KP_LEFTARROW",	K_KP_LEFTARROW },
	{"KP_5",			K_KP_5 },
	{"KP_RIGHTARROW",	K_KP_RIGHTARROW },
	{"KP_END",			K_KP_END },
	{"KP_DOWNARROW",	K_KP_DOWNARROW },
	{"KP_PGDN",			K_KP_PGDN },
	{"KP_ENTER",		K_KP_ENTER },
	{"KP_INS",			K_KP_INS },
	{"KP_DEL",			K_KP_DEL },
	{"KP_SLASH",		K_KP_SLASH },
	{"KP_MINUS",		K_KP_MINUS },
	{"KP_PLUS",			K_KP_PLUS },

	{ "PAUSE", K_PAUSE },

	{ "MWHEELUP", K_MWHEELUP },
	{ "MWHEELDOWN", K_MWHEELDOWN },

	{ "SEMICOLON", ';' }, // because a raw semicolon seperates commands

	{ NULL, 0 }
};

/*
==============================================================================

            LINE TYPING INTO THE CONSOLE

==============================================================================
*/

qboolean CheckForCommand()
{
	char command[128];
	char *cmd, *s;
	int i;

	s = key_lines[edit_line] + 1;

	for(i = 0; i < 127; i++)
		if(s[i] <= ' ')
			break;
		else
			command[i] = s[i];
	command[i] = 0;

	//cmd = Cmd_CompleteCommand(command);
	
	//if(!cmd || strcmp(cmd, command))
		//cmd = Cvar_CompleteVariable(command);
	
	if(!cmd || strcmp(cmd, command))
		return false; // just a chat message
	
	return true;
}

void CompleteCommand()
{
	char *s = key_lines[edit_line] + 1;

	if(*s == '\\' || *s == '/')
		s++;

	char *cmd = nullptr; //Cmd_CompleteCommand(s);

	//if(!cmd)
		//cmd = Cvar_CompleteVariable(s);

	if(cmd)
	{
		key_lines[edit_line][1] = '/';
		Q_strcpy(key_lines[edit_line] + 2, cmd);
		key_linepos = Q_strlen(cmd) + 2;
		key_lines[edit_line][key_linepos] = ' ';
		key_linepos++;
		key_lines[edit_line][key_linepos] = 0;
		return;
	}
}

/*
===================
Key_StringToKeynum

Returns a key number to be used to index keybindings[] by looking at
the given string.  Single ascii characters return themselves, while
the K_* names are matched up.
===================
*/
int Key_StringToKeynum(char *str)
{
	if(!str || !str[0])
		return -1;

	if(!str[1])
		return str[0];

	for(keyname_t *kn = keynames; kn->name; kn++)
	{
		//if(!Q_strcasecmp(str, kn->name))
			//return kn->keynum;
	};

	return -1;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, or a K_* name) for the
given keynum.
FIXME: handle quote special (general escape sequence?)
===================
*/
const char *Key_KeynumToString(int keynum)
{
	keyname_t *kn;
	static char tinystr[2];

	if(keynum == -1)
		return "<KEY NOT FOUND>";
	if(keynum > 32 && keynum < 127)
	{ // printable ascii
		tinystr[0] = keynum;
		tinystr[1] = 0;
		return tinystr;
	}

	for(kn = keynames; kn->name; kn++)
		if(keynum == kn->keynum)
			return kn->name;

	return "<UNKNOWN KEYNUM>";
}

/*
===================
Key_SetBinding
===================
*/
void Key_SetBinding(int keynum, char *binding)
{
	char *pnew;
	int l;

	if(keynum == -1)
		return;

	// free old bindings
	if(keybindings[keynum])
	{
		Z_Free(keybindings[keynum]);
		keybindings[keynum] = NULL;
	}

	// allocate memory for new binding
	l = Q_strlen(binding);
	pnew = (char*)Z_Malloc(l + 1);
	Q_strcpy(pnew, binding);
	pnew[l] = 0;
	keybindings[keynum] = pnew;
}

/*
===================
Key_Unbind_f
===================
*/
void Key_Unbind_f()
{
	if(Cmd_Argc() != 2)
	{
		Con_Printf("unbind <key> : remove commands from a key\n");
		return;
	}

	int b = Key_StringToKeynum((char*)Cmd_Argv(1));
	if(b == -1)
	{
		Con_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding(b, "");
}

void Key_Unbindall_f()
{
	for(int i = 0; i < 256; i++)
		if(keybindings[i])
			Key_SetBinding(i, "");
}

/*
===================
Key_Bind_f
===================
*/
void Key_Bind_f()
{
	int i, c, b;
	char cmd[1024];

	c = Cmd_Argc();

	if(c < 2)
	{
		Con_Printf("bind <key> [command] : attach a command to a key\n");
		return;
	};
	
	b = Key_StringToKeynum((char*)Cmd_Argv(1));
	
	if(b == -1)
	{
		Con_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if(c == 2)
	{
		if(keybindings[b])
			Con_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), keybindings[b]);
		else
			Con_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
		return;
	}

	// copy the rest of the command line
	cmd[0] = 0; // start out with a null string
	for(i = 2; i < c; i++)
	{
		strcat(cmd, Cmd_Argv(i));
		if(i != (c - 1))
			strcat(cmd, " ");
	}

	Key_SetBinding(b, cmd);
}

/*
============
Key_Bindlist_f

============
*/
void Key_Bindlist_f()
{
	for(int i = 0; i < 256; ++i)
		if(keybindings[i] && keybindings[i][0])
			Con_Printf("%s \"%s\"\n", Key_KeynumToString(i), keybindings[i]);
};

/*
============
Key_WriteBindings

Writes lines containing "bind key value"
============
*/
void Key_WriteBindings(FILE *f)
{
	for(int i = 0; i < 256; i++)
		if(keybindings[i]) // if (keybindings[i] && keybindings[i][0])
			fprintf(f, "bind %s \"%s\"\n", Key_KeynumToString(i), keybindings[i]);
}

/*
===================
Key_Init
===================
*/
void Key_Init()
{
	int i;

	for(i = 0; i < 32; i++)
	{
		key_lines[i][0] = ']';
		key_lines[i][1] = 0;
	}
	key_linepos = 1;

	//
	// init ascii characters in console mode
	//
	for(i = 32; i < 128; i++)
		consolekeys[i] = true;
	
	consolekeys[K_ENTER] = true;
	consolekeys[K_TAB] = true;
	consolekeys[K_LEFTARROW] = true;
	consolekeys[K_RIGHTARROW] = true;
	consolekeys[K_UPARROW] = true;
	consolekeys[K_DOWNARROW] = true;
	consolekeys[K_BACKSPACE] = true;
	consolekeys[K_HOME] = true;
	consolekeys[K_END] = true;
	consolekeys[K_PGUP] = true;
	consolekeys[K_PGDN] = true;
	consolekeys[K_SHIFT] = true;
	
	consolekeys[K_INS] = true;
	
	consolekeys[K_MWHEELUP] = true;
	consolekeys[K_MWHEELDOWN] = true;
	
	consolekeys[K_KP_ENTER] = true;
	consolekeys[K_KP_LEFTARROW] = true;
	consolekeys[K_KP_RIGHTARROW] = true;
	consolekeys[K_KP_UPARROW] = true;
	consolekeys[K_KP_DOWNARROW] = true;
	consolekeys[K_KP_HOME] = true;
	consolekeys[K_KP_END] = true;
	consolekeys[K_KP_PGUP] = true;
	consolekeys[K_KP_PGDN] = true;
	consolekeys[K_KP_INS] = true;
	consolekeys[K_KP_DEL] = true;
	consolekeys[K_KP_SLASH] = true;
	consolekeys[K_KP_PLUS] = true;
	consolekeys[K_KP_MINUS] = true;
	consolekeys[K_KP_5] = true;
	
	consolekeys['`'] = false;
	consolekeys['~'] = false;

	for(i = 0; i < 256; i++)
		keyshift[i] = i;
	
	for(i = 'a'; i <= 'z'; i++)
		keyshift[i] = i - 'a' + 'A';
	
	keyshift['1'] = '!';
	keyshift['2'] = '@';
	keyshift['3'] = '#';
	keyshift['4'] = '$';
	keyshift['5'] = '%';
	keyshift['6'] = '^';
	keyshift['7'] = '&';
	keyshift['8'] = '*';
	keyshift['9'] = '(';
	keyshift['0'] = ')';
	
	keyshift['-'] = '_';
	keyshift['='] = '+';
	keyshift[','] = '<';
	keyshift['.'] = '>';
	keyshift['/'] = '?';
	keyshift[';'] = ':';
	keyshift['\''] = '"';
	keyshift['['] = '{';
	keyshift[']'] = '}';
	keyshift['`'] = '~';
	keyshift['\\'] = '|';

	menubound[K_ESCAPE] = true;
	for(i = 0; i < 12; i++)
		menubound[K_F1 + i] = true;

	//
	// register our functions
	//
	Cmd_AddCommand("bind", Key_Bind_f);
	Cmd_AddCommand("unbind", Key_Unbind_f);
	Cmd_AddCommand("unbindall", Key_Unbindall_f);
	//Cmd_AddCommand("bindlist", Key_Bindlist_f);
}

/*
===================
Key_Event

Called by the system between frames for both key up and key down events
Should NOT be called during an interrupt!
===================
*/
void Key_Event(int key, qboolean down)
{
	char *kb;
	char cmd[1024];

	//	Con_Printf ("%i : %i\n", key, down); //@@@

	keydown[key] = down;

	if(!down)
		key_repeats[key] = 0;

	key_lastpress = key;
	key_count++;
	if(key_count <= 0)
	{
		return; // just catching keys for Con_NotifyBox
	}

	// update auto-repeat status
	if(down)
	{
		key_repeats[key]++;
		if(key != K_BACKSPACE && key != K_PAUSE && key != K_PGUP &&
		   key != K_PGDN && key_repeats[key] > 1)
			return; // ignore most autorepeats

		if(key >= 200 && !keybindings[key])
			Con_Printf("%s is unbound, hit F4 to set.\n", Key_KeynumToString(key));
	}

	if(key == K_SHIFT)
		shift_down = down;

	//
	// handle escape specialy, so the user can never unbind it
	//
	if(key == K_ESCAPE)
	{
		if(!down)
			return;
		switch(key_dest)
		{
		case key_message:
			Key_Message(key);
			break;
		case key_menu:
			//M_Keydown(key);
			break;
		case key_game:
			//M_ToggleMenu_f();
			break;
		default:
			Sys_Error("Bad key_dest");
		}
		return;
	}

	//
	// key up events only generate commands if the game key binding is
	// a button command (leading + sign).  These will occur even in console mode,
	// to keep the character from continuing an action started before a console
	// switch.  Button commands include the kenum as a parameter, so multiple
	// downs can be matched with ups
	//
	if(!down)
	{
		kb = keybindings[key];
		if(kb && kb[0] == '+')
		{
			sprintf(cmd, "-%s %i\n", kb + 1, key);
			Cbuf_AddText(cmd);
		}
		if(keyshift[key] != key)
		{
			kb = keybindings[keyshift[key]];
			if(kb && kb[0] == '+')
			{
				sprintf(cmd, "-%s %i\n", kb + 1, key);
				Cbuf_AddText(cmd);
			}
		}
		return;
	}

	//
	// during demo playback, most keys bring up the main menu
	//
	if(cls.demoplayback && down && consolekeys[key] && key_dest == key_game)
	{
		//M_ToggleMenu_f();
		return;
	}

	//
	// if not a consolekey, send to the interpreter no matter what mode is
	//
	if((key_dest == key_menu && menubound[key]) ||
	   //(key_dest == key_console && !consolekeys[key]) ||
	   (key_dest == key_game && (cls.state == ca_active || !consolekeys[key])))
	{
		kb = keybindings[key];
		if(kb)
		{
			if(kb[0] == '+')
			{ // button commands add keynum as a parm
				sprintf(cmd, "%s %i\n", kb, key);
				Cbuf_AddText(cmd);
			}
			else
			{
				Cbuf_AddText(kb);
				Cbuf_AddText("\n");
			}
		}
		return;
	}

	if(!down)
		return; // other systems only care about key down events

	if(shift_down)
		key = keyshift[key];

	switch(key_dest)
	{
	case key_message:
		Key_Message(key);
		break;
	case key_menu:
		//M_Keydown(key); // Key_Menu
		break;
	case key_game:
		Key_Console(key); // Key_Game
		break;
	default:
		Sys_Error("Bad key_dest");
	}
}

/*
===================
Key_ClearStates
===================
*/
void Key_ClearStates()
{
	//anykeydown = false;
	
	for(int i = 0; i < 256; ++i)
	{
		//if(keydown[i] || key_repeats[i])
			//Key_Event(i, false, 0);
		
		keydown[i] = 0;
		key_repeats[i] = 0;
	};
};

/*
const char *EngFunc_Key_LookupBinding(const char *pBinding)
{
        return Key_KeynumToString( Key_GetKey( pBinding ));
};
*/