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

#include "precompiled.hpp"
#include "console/console.hpp"
#include "console/cmd.hpp"
#include "console/cvar.hpp"
#include "system/system.hpp"
#include "filesystem/filesystem_internal.hpp"
#include "system/client.hpp"
#include "input/keys.hpp"

console_t con_main;
console_t con_chat;

console_t *con; // point to either con_main or con_chat

float con_cursorspeed = 4;

cvar_t con_notifytime = { "con_notifytime", "3" }; // seconds

const int NUM_CON_TIMES = 4;

float con_times[NUM_CON_TIMES]; // realtime time the line was generated
                                // for transparent notify lines

qboolean con_debuglog;

const int MAXCMDLINE = 256;

extern char key_lines[32][MAXCMDLINE];
extern int edit_line;
extern int key_linepos;

void Key_ClearTyping()
{
	key_lines[edit_line][1] = 0; // clear any typing
	key_linepos = 1;
}

/*
================
Con_ToggleConsole_f
================
*/
void Con_ToggleConsole_f()
{
	/*
	//Key_ClearTyping();

	if(EngineVGui()->IsConsoleVisible())
	{
		Con_HideConsole_f();
		
		// If we hide the console, we also hide the game UI
		EngineVGui()->HideGameUI();
	}
	else
		Con_ShowConsole_f();
	
	//Con_ClearNotify();
	*/
}

/*
================
Con_ToggleChat_f
================
*/
void Con_ToggleChat_f()
{
/*
	Key_ClearTyping();

	if(key_dest == key_console)
	{
		if(cls.state == ca_active)
		{
			//M_ForceMenuOff ();
			key_dest = key_game;
		};
	}
	else
		key_dest = key_console;

	Con_ClearNotify();
*/
}

/*
================
Con_Clear_f
================
*/
void Con_Clear_f()
{
	Q_memset(con_main.text, ' ', CON_TEXTSIZE);
	Q_memset(con_chat.text, ' ', CON_TEXTSIZE);
}

/*
================
Con_ClearNotify
================
*/
void Con_ClearNotify()
{
	for(int i = 0; i < NUM_CON_TIMES; ++i)
		con_times[i] = 0;
};

/*
================
Con_MessageMode_f
================
*/
void Con_MessageMode_f()
{
	//chat_team = false;
	//key_dest = key_message;
};

/*
================
Con_MessageMode2_f
================
*/
void Con_MessageMode2_f()
{
	//chat_team = true;
	//key_dest = key_message;
};

/*
================
Con_Dump_f

Save the console contents out to a file
================
*/
void Con_Dump_f ()
{
	int		l, x;
	char	*line;
	FILE	*f;
	char	buffer[1024];
	char	name[MAX_OSPATH];

	if (Cmd_Argc() != 2)
	{
		Con_Printf("usage: condump <filename>\n");
		return;
	}
/*
	Q_sprintf (name, sizeof(name), "%s/%s.txt", COM_GetGameDir(), Cmd_Argv(1));

	Con_Printf ("Dumped console text to %s.\n", name);
	FS_CreatePath (name);
	f = fopen (name, "w");
	if (!f)
	{
		Con_Printf ("ERROR: couldn't open.\n");
		return;
	}

	// skip empty lines
	for (l = con->current - con->totallines + 1 ; l <= con->current ; l++)
	{
		line = con->text + (l%con->totallines)*con->linewidth;
		for (x=0 ; x<con->linewidth ; x++)
			if (line[x] != ' ')
				break;
		if (x != con->linewidth)
			break;
	}

	// write the remaining lines
	buffer[con->linewidth] = 0;
	for ( ; l <= con->current ; l++)
	{
		line = con->text + (l%con->totallines)*con->linewidth;
		strncpy (buffer, line, con->linewidth);
		for (x=con.linewidth-1 ; x>=0 ; x--)
		{
			if (buffer[x] == ' ')
				buffer[x] = 0;
			else
				break;
		}
		for (x=0; buffer[x]; x++)
			buffer[x] &= 0x7f;

		fprintf (f, "%s\n", buffer);
	}

	fclose (f);
};

void Con_Debug_f()
{
	if(con_debuglog)
	{
		Con_Printf("condebug disabled\n");
		con_debuglog = FALSE;
	}
	else
	{
		con_debuglog = TRUE;
		Con_Printf("condebug enabled\n");
	};
*/
};

/*
================
Con_Resize

================
*/
void Con_Resize(console_t *con)
{
/*
	int i, j, width, oldwidth, oldtotallines, numlines, numchars;
	char tbuf[CON_TEXTSIZE];

	width = (vid.width >> 3) - 2;

	if(width == con_linewidth)
		return;

	if(width < 1) // video hasn't been initialized yet
	{
		width = 38;
		con_linewidth = width;
		con_totallines = CON_TEXTSIZE / con_linewidth;
		Q_memset(con->text, ' ', CON_TEXTSIZE);
	}
	else
	{
		oldwidth = con_linewidth;
		con_linewidth = width;
		oldtotallines = con_totallines;
		con_totallines = CON_TEXTSIZE / con_linewidth;
		numlines = oldtotallines;

		if(con_totallines < numlines)
			numlines = con_totallines;

		numchars = oldwidth;

		if(con_linewidth < numchars)
			numchars = con_linewidth;

		Q_memcpy(tbuf, con->text, CON_TEXTSIZE);
		Q_memset(con->text, ' ', CON_TEXTSIZE);

		for(i = 0; i < numlines; i++)
		{
			for(j = 0; j < numchars; j++)
			{
				con->text[(con_totallines - 1 - i) * con_linewidth + j] =
				tbuf[((con->current - i + oldtotallines) % oldtotallines) *
				     oldwidth +
				     j];
			}
		}

		Con_ClearNotify();
	}

	con->current = con_totallines - 1;
	con->display = con->current;
*/
}

/*
================
Con_CheckResize

If the line width has changed, reformat the buffer.
================
*/
void Con_CheckResize()
{
	Con_Resize(&con_main);
	Con_Resize(&con_chat);
}

void Con_Init()
{
	
};

void Con_Shutdown()
{

};

/*
===============
Con_Linefeed
===============
*/
void Con_Linefeed()
{
	con->x = 0;
	if(con->display == con->current)
		con->display++;
	con->current++;
	Q_memset(&con->text[(con->current % con_totallines) * con_linewidth], ' ', con_linewidth);
}

/*
================
Con_Print

Handles cursor positioning, line wrapping, etc
All console printing must go through this in order to be logged to disk
If no console is visible, the notify window will pop up.
================
*/
void Con_Print(char *txt)
{
/*
	int y;
	int c, l;
	static int cr;
	int mask = 0;

	if(txt[0] == 1 || txt[0] == 2)
	{
		mask = 128; // go to colored text
		txt++;
	};

	while((c = *txt))
	{
		// count word length
		for(l = 0; l < con_linewidth; l++)
			if(txt[l] <= ' ')
				break;

		// word wrap
		if(l != con_linewidth && (con->x + l > con_linewidth))
			con->x = 0;

		txt++;

		if(cr)
		{
			con->current--;
			cr = false;
		}

		if(!con->x)
		{
			Con_Linefeed();
			// mark time for transparent overlay
			if(con->current >= 0)
				con_times[con->current % NUM_CON_TIMES] = realtime;
		}

		switch(c)
		{
		case '\n':
			con->x = 0;
			break;

		case '\r':
			con->x = 0;
			cr = 1;
			break;

		default: // display character and advance
			y = con->current % con_totallines;
			con->text[y * con_linewidth + con->x] = c | mask | con_ormask;
			con->x++;
			if(con->x >= con_linewidth)
				con->x = 0;
			break;
		}
	}
*/
}

/*
================
Con_Printf

Handles cursor positioning, line wrapping, etc
================
*/
#define MAXPRINTMSG 4096
// FIXME: make a buffer size safe vsprintf?
void Con_Printf(char *fmt, ...)
{
/*
	va_list argptr;
	char msg[MAXPRINTMSG];
	static qboolean inupdate;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	// also echo to debugging console
	Sys_Printf("%s", msg); // also echo to debugging console

	// log all messages to file
	if(con_debuglog)
		Sys_DebugLog(va("%s/qconsole.log", com_gamedir), "%s", msg);

	if(!con_initialized)
		return;

	// write it to the scrollable buffer
	Con_Print(msg);

	// update the screen immediately if the console is displayed
	if(cls.state != ca_active)
	{
		// protect against infinite loop if something in SCR_UpdateScreen calls
		// Con_Printf
		if(!inupdate)
		{
			inupdate = true;
			SCR_UpdateScreen();
			inupdate = false;
		}
	}
*/
}


void Con_SafePrintf(const char *fmt, ...)
{
/*
	
	int temp;

	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	temp = scr_disabled_for_loading;
	scr_disabled_for_loading = true;
	Con_Printf("%s", msg);
	scr_disabled_for_loading = temp;
*/
}

////////////////////////////////////////////////////////////////////////////

/*
================
Con_DebugLog
================
*/
void Con_DebugLog(const char *file, const char *fmt, ...)
{
	va_list argptr;
	static char data[8192]; // 1024

	va_start(argptr, fmt);
	Q_vsnprintf(data, sizeof(data), fmt, argptr);
	va_end(argptr);

	data[sizeof(data) - 1] = 0;
/*
#ifdef _WIN32

	int fd = _open(file, _O_WRONLY | _O_APPEND | _O_CREAT, _S_IREAD | _S_IWRITE);
	int len = Q_strlen(data);
	_write(fd, data, len);
	_close(fd);

#else // _WIN32

	FILE *fd = FS_Open(file, "at");
	FS_FPrintf(fd, "%s", data);
	FS_Close(fd);

#endif // _WIN32
*/
}

/*
==============================================================================

DRAWING

==============================================================================
*/

/*
================
Con_DrawInput

The input line scrolls horizontally if typing goes beyond the right edge
================
*/
/*
void Con_DrawInput()
{
	int y;
	int i;
	char *text;

	if(key_dest != key_console && cls.state == ca_active)
		return; // don't draw anything (allways draw if not active)

	text = key_lines[edit_line];

	// add the cursor frame
	text[key_linepos] = 10 + ((int)(realtime * con_cursorspeed) & 1);

	// fill out remainder with spaces
	for(i = key_linepos + 1; i < con_linewidth; i++)
		text[i] = ' ';

	//	prestep if horizontally scrolling
	if(key_linepos >= con_linewidth)
		text += 1 + key_linepos - con_linewidth;

	// draw it
	y = con_vislines - 22;

	for(i = 0; i < con_linewidth; i++)
		Draw_Character((i + 1) << 3, con_vislines - 22, text[i]);

	// remove cursor
	key_lines[edit_line][key_linepos] = 0;
}
*/

/*
================
Con_DrawNotify

Draws the last few lines of output transparently over the game top
================
*/
/*
void Con_DrawNotify()
{
	int x, v;
	char *text;
	int i;
	float time;
	char *s;
	int skip;

	v = 0;
	for(i = con->current - NUM_CON_TIMES + 1; i <= con->current; i++)
	{
		if(i < 0)
			continue;
		time = con_times[i % NUM_CON_TIMES];
		if(time == 0)
			continue;
		time = realtime - time;
		if(time > con_notifytime.value)
			continue;
		text = con->text + (i % con_totallines) * con_linewidth;

		clearnotify = 0;
		scr_copytop = 1;

		for(x = 0; x < con_linewidth; x++)
			Draw_Character((x + 1) << 3, v, text[x]);

		v += 8;
	}

	if(key_dest == key_message)
	{
		clearnotify = 0;
		scr_copytop = 1;

		if(chat_team)
		{
			Draw_String(8, v, "say_team:");
			skip = 11;
		}
		else
		{
			Draw_String(8, v, "say:");
			skip = 5;
		}

		s = chat_buffer;
		if(chat_bufferlen > (vid.width >> 3) - (skip + 1))
			s += chat_bufferlen - ((vid.width >> 3) - (skip + 1));
		x = 0;
		while(s[x])
		{
			Draw_Character((x + skip) << 3, v, s[x]);
			x++;
		}
		Draw_Character((x + skip) << 3, v, 10 + ((int)(realtime * con_cursorspeed) & 1));
		v += 8;
	}

	if(v > con_notifylines)
		con_notifylines = v;
}
*/

/*
================
Con_DrawConsole

Draws the console with the solid background
================
*/
/*
void Con_DrawConsole(int lines)
{
	int i, j, x, y, n;
	int rows;
	char *text;
	int row;
	char dlbar[1024];

	if(lines <= 0)
		return;

	// draw the background
	Draw_ConsoleBackground(lines);

	// draw the text
	con_vislines = lines;

	// changed to line things up better
	rows = (lines - 22) >> 3; // rows of text to draw

	y = lines - 30;

	// draw from the bottom up
	if(con->display != con->current)
	{
		// draw arrows to show the buffer is backscrolled
		for(x = 0; x < con_linewidth; x += 4)
			Draw_Character((x + 1) << 3, y, '^');

		y -= 8;
		rows--;
	}

	row = con->display;
	for(i = 0; i < rows; i++, y -= 8, row--)
	{
		if(row < 0)
			break;
		if(con->current - row >= con_totallines)
			break; // past scrollback wrap point

		text = con->text + (row % con_totallines) * con_linewidth;

		for(x = 0; x < con_linewidth; x++)
			Draw_Character((x + 1) << 3, y, text[x]);
	}

	// draw the download bar
	// figure out width
	if(cls.download)
	{
		if((text = strrchr(cls.downloadname, '/')) != NULL)
			text++;
		else
			text = cls.downloadname;

		x = con_linewidth - ((con_linewidth * 7) / 40);
		y = x - strlen(text) - 8;
		i = con_linewidth / 3;
		if(strlen(text) > i)
		{
			y = x - i - 11;
			strncpy(dlbar, text, i);
			dlbar[i] = 0;
			strcat(dlbar, "...");
		}
		else
			strcpy(dlbar, text);
		strcat(dlbar, ": ");
		i = strlen(dlbar);
		dlbar[i++] = '\x80';
		// where's the dot go?
		if(cls.downloadpercent == 0)
			n = 0;
		else
			n = y * cls.downloadpercent / 100;

		for(j = 0; j < y; j++)
			if(j == n)
				dlbar[i++] = '\x83';
			else
				dlbar[i++] = '\x81';
		dlbar[i++] = '\x82';
		dlbar[i] = 0;

		sprintf(dlbar + strlen(dlbar), " %02d%%", cls.downloadpercent);

		// draw it
		y = con_vislines - 22 + 8;
		for(i = 0; i < strlen(dlbar); i++)
			Draw_Character((i + 1) << 3, y, dlbar[i]);
	}

	// draw the input prompt, user text, and cursor if desired
	Con_DrawInput();
}
*/

/*
==================
Con_NotifyBox
==================
*/
/*
void Con_NotifyBox(char *text)
{
	double t1, t2;

	// during startup for sound / cd warnings
	Con_Printf("\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36"
	           "\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n");

	Con_Printf(text);

	Con_Printf("Press a key.\n");
	Con_Printf("\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36"
	           "\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n");

	key_count = -2; // wait for a key down and up
	key_dest = key_console;

	do
	{
		t1 = Sys_FloatTime();
		SCR_UpdateScreen();
		Sys_SendKeyEvents();
		t2 = Sys_FloatTime();
		realtime += t2 - t1; // make the cursor blink
	} while(key_count < 0);

	Con_Printf("\n");
	key_dest = key_game;
	realtime = 0; // put the cursor back to invisible
}
*/