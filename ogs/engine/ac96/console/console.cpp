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

//int con_ormask;

//console_t con_main;
//console_t con_chat;

//console_t *con; // point to either con_main or con_chat

int con_linewidth = 1;  // characters across screen
int con_totallines = 0; // total lines in console scrollback

//float con_cursorspeed = 4;

cvar_t con_notifytime = { "con_notifytime", "3" }; // seconds

const int NUM_CON_TIMES = 4;

float con_times[NUM_CON_TIMES]; // realtime time the line was generated
                                // for transparent notify lines

//int con_vislines;
int con_notifylines = 0; // scan lines to clear for notify lines

qboolean con_debuglog = false;

const int MAXCMDLINE = 256;

extern char key_lines[32][MAXCMDLINE];
extern int edit_line;
extern int key_linepos;

qboolean con_initialized = false;

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
	//Q_memset(con_main.text, ' ', CON_TEXTSIZE);
	//Q_memset(con_chat.text, ' ', CON_TEXTSIZE);
	
	if( con_text )
		Q_memset( con_text, ' ', CON_TEXTSIZE );

	VGuiWrap2_ClearConsole();
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
	
	if( VGuiWrap2_IsInCareerMatch() == CAREER_NONE )
	{
		key_dest = key_message;

		if( Cmd_Argc() == 2 )
		{
			Q_strncpy( message_type, Cmd_Argv( 1 ), ARRAYSIZE( message_type ) - 1 );
			message_type[ ARRAYSIZE( message_type ) - 1 ] = '\0';
		}
		else
			Q_strcpy( message_type, "say" );
	};
};

/*
================
Con_MessageMode2_f
================
*/
void Con_MessageMode2_f()
{
	if( VGuiWrap2_IsInCareerMatch() == CAREER_NONE )
	{
		key_dest = key_message;
		Q_strcpy( message_type, "say_team" ); //chat_team = true;
	};
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
*/

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

/*
================
Con_Init
================
*/
void Con_Init()
{
	con_debuglog = COM_CheckParm("-condebug");

	con = &con_main;
	con_linewidth = -1; // con.
	Con_CheckResize();

	Con_DPrintf("Console initialized.\n"); // Con_Printf

	//
	// register our commands
	//
	Cvar_RegisterVariable(&con_notifytime);

	Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
	//Cmd_AddCommand("togglechat", Con_ToggleChat_f);
	Cmd_AddCommand("messagemode", Con_MessageMode_f);
	Cmd_AddCommand("messagemode2", Con_MessageMode2_f);
	Cmd_AddCommand("clear", Con_Clear_f);
	Cmd_AddCommand("condump", Con_Dump_f);

#ifdef HOOK_ENGINE
	Cmd_AddCommand("condebug", (xcommand_t)GetOriginalFuncAddrOrDefault("Con_Debug_f", (void *)Con_Debug_f));
#else
	Cmd_AddCommand("condebug", Con_Debug_f);
#endif

	con_initialized = true; // con.initialized
};

void Con_Shutdown()
{
#ifndef SWDS
#endif
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

void EXT_FUNC Con_NPrintf(int idx, const char *fmt, ...)
{
//#ifndef SWDS
	va_list va;

	va_start( va, fmt );

	g_engdstAddrs.Con_NPrintf( &idx, const_cast<char**>( &fmt ) );

	if( 0 <= idx && idx < CON_MAX_DEBUG_AREAS )
	{
		vsnprintf( da_notify[ idx ].szNotify, ARRAYSIZE( da_notify[ idx ].szNotify ), fmt, va );
	
		da_notify[ idx ].expire = realtime + 4.0;
		
		da_notify[ idx ].color[ 0 ] = da_default_color[ 0 ];
		da_notify[ idx ].color[ 1 ] = da_default_color[ 1 ];
		da_notify[ idx ].color[ 2 ] = da_default_color[ 2 ];
	};

	va_end( va );
//#endif
};

/*
================
Con_DPrintf

A Con_Printf that only shows up if the "developer" cvar is set
================
*/
#if defined(REHLDS_FIXES) && defined(REHLDS_FLIGHT_REC)
// Always print debug logs to the flight recorder
void EXT_FUNC Con_DPrintf(const char *fmt, ...)
{
	char Dest[4096];
	va_list argptr;
	va_start(argptr, fmt);
	Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);
	va_end(argptr);

	FR_Log("REHLDS_CONDBG", Dest);

	if(developer.value != 0.0f)
	{
#ifdef _WIN32
		OutputDebugStringA(Dest);
		if(con_debuglog)
			Con_DebugLog("qconsole.log", "%s", Dest);
#else
		vfprintf(stdout, fmt, argptr);
		fflush(stdout);
#endif // _WIN32
	}
}

#else // defined(REHLDS_FIXES) and defined(REHLDS_FLIGHT_REC)

void Con_DPrintf(const char *fmt, ...)
{
/*
	// don't confuse non-developers with techical stuff...
	if(!developer.value)
		return;
	
	va_list argptr;
	va_start(argptr, fmt);
	
#ifdef _WIN32
	char Dest[4096];
	Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);

	OutputDebugStringA(Dest);
	if(con_debuglog)
		Con_DebugLog("qconsole.log", "%s", Dest);
#else
	vfprintf(stdout, fmt, argptr);
	fflush(stdout);
#endif // _WIN32
	
	va_end(argptr);
*/
}

#endif // defined(REHLDS_FIXES) and defined(REHLDS_FLIGHT_REC)

/*
==================
Con_SafePrintf

Okay to call even when the screen can't be updated
==================
*/
void Con_SafePrintf(const char *fmt, ...)
{
	va_list argptr;
	char msg[1024];
	//int temp;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr); // Q_vsnprintf( msg, ARRAYSIZE( msg ), fmt, argptr )
	va_end(argptr);

	//temp = scr_disabled_for_loading;
	//scr_disabled_for_loading = true;
	Con_Printf("%s", msg);
	//scr_disabled_for_loading = temp;
}

/*
void Con_SafePrintf(const char *fmt, ...)
{
	va_start(argptr, fmt);

#ifdef _WIN32
	char Dest[1024];
	Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);
	va_end(argptr);
	Con_Printf("%s", Dest);
#else
	vfprintf(stdout, fmt, argptr);
	va_end(argptr);
	fflush(stdout);
#endif // _WIN32
}
*/

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

void EXT_FUNC Con_Printf(const char *fmt, ...)
{
/*
	char Dest[4096];
	va_list va;

	va_start(va, fmt);
	Q_vsnprintf(Dest, sizeof(Dest), fmt, va);
	va_end(va);

#ifdef REHLDS_FLIGHT_REC
	FR_Log("REHLDS_CON", Dest);
#endif

#ifdef REHLDS_FIXES
	if(sv_redirected == RD_NONE || sv_rcon_condebug.value > 0.0f)
#endif
	{
		Sys_Printf("%s", Dest);
	}

	if(sv_redirected)
	{
		if((Q_strlen(outputbuf) + Q_strlen(Dest)) > sizeof(outputbuf) - 1)
			SV_FlushRedirect();
		Q_strncat(outputbuf, Dest, sizeof(outputbuf) - 1);
	}
	else
	{
		if(con_debuglog)
			Con_DebugLog("qconsole.log", "%s", Dest);
#ifndef SWDS
		if(host_initialized && con_initialized && cls.state)
		{
			if(developer.value != 0.0f)
			{
				Q_strncpy(g_szNotifyAreaString, msg, 255);
				g_szNotifyAreaString[255] = 0;
				*con_times = realtime;
			}
			VGuiWrap2_ConPrintf(msg);
		}
#endif // SWDS
	}
*/
}

/*
void EXT_FUNC Con_DPrintf(const char *fmt, ...)
{
	char msg[MAXPRINTMSG];

	vsprintf(msg, fmt, argptr);

	Con_Printf("%s", msg);
}
*/

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