
int	anykeydown;

int		key_waiting;

void Key_Console (int key)
{

	switch ( key )
	{
	case K_KP_SLASH:
		key = '/';
		break;
	case K_KP_MINUS:
		key = '-';
		break;
	case K_KP_PLUS:
		key = '+';
		break;
	case K_KP_HOME:
		key = '7';
		break;
	case K_KP_UPARROW:
		key = '8';
		break;
	case K_KP_PGUP:
		key = '9';
		break;
	case K_KP_LEFTARROW:
		key = '4';
		break;
	case K_KP_5:
		key = '5';
		break;
	case K_KP_RIGHTARROW:
		key = '6';
		break;
	case K_KP_END:
		key = '1';
		break;
	case K_KP_DOWNARROW:
		key = '2';
		break;
	case K_KP_PGDN:
		key = '3';
		break;
	case K_KP_INS:
		key = '0';
		break;
	case K_KP_DEL:
		key = '.';
		break;
	}

	if ( ( toupper( key ) == 'V' && keydown[K_CTRL] ) ||
		 ( ( ( key == K_INS ) || ( key == K_KP_INS ) ) && keydown[K_SHIFT] ) )
	{
		char *cbd;
		
		if ( ( cbd = Sys_GetClipboardData() ) != 0 )
		{
			int i;

			strtok( cbd, "\n\r\b" );

			i = strlen( cbd );
			if ( i + key_linepos >= MAXCMDLINE)
				i= MAXCMDLINE - key_linepos;

			if ( i > 0 )
			{
				cbd[i]=0;
				strcat( key_lines[edit_line], cbd );
				key_linepos += i;
			}
			free( cbd );
		}

		return;
	}

	if ( key == 'l' ) 
	{
		if ( keydown[K_CTRL] )
		{
			Cbuf_AddText ("clear\n");
			return;
		}
	}

	if ( key == K_ENTER || key == K_KP_ENTER )
	{	// backslash text are commands, else chat
		if (key_lines[edit_line][1] == '\\' || key_lines[edit_line][1] == '/')
			Cbuf_AddText (key_lines[edit_line]+2);	// skip the >
		else
			Cbuf_AddText (key_lines[edit_line]+1);	// valid command

		Cbuf_AddText ("\n");
		Com_Printf ("%s\n",key_lines[edit_line]);
		edit_line = (edit_line + 1) & 31;
		history_line = edit_line;
		key_lines[edit_line][0] = ']';
		key_linepos = 1;
		if (cls.state == ca_disconnected)
			SCR_UpdateScreen ();	// force an update, because the command
									// may take some time
		return;
	}

	if (key == K_TAB)
	{	// command completion
		CompleteCommand ();
		return;
	}
	
	if ( ( key == K_BACKSPACE ) || ( key == K_LEFTARROW ) || ( key == K_KP_LEFTARROW ) || ( ( key == 'h' ) && ( keydown[K_CTRL] ) ) )
	{
		if (key_linepos > 1)
			key_linepos--;
		return;
	}

	if ( ( key == K_UPARROW ) || ( key == K_KP_UPARROW ) ||
		 ( ( key == 'p' ) && keydown[K_CTRL] ) )
	{
		do
		{
			history_line = (history_line - 1) & 31;
		} while (history_line != edit_line
				&& !key_lines[history_line][1]);
		if (history_line == edit_line)
			history_line = (edit_line+1)&31;
		strcpy(key_lines[edit_line], key_lines[history_line]);
		key_linepos = strlen(key_lines[edit_line]);
		return;
	}

	if ( ( key == K_DOWNARROW ) || ( key == K_KP_DOWNARROW ) ||
		 ( ( key == 'n' ) && keydown[K_CTRL] ) )
	{
		if (history_line == edit_line) return;
		do
		{
			history_line = (history_line + 1) & 31;
		}
		while (history_line != edit_line
			&& !key_lines[history_line][1]);
		if (history_line == edit_line)
		{
			key_lines[edit_line][0] = ']';
			key_linepos = 1;
		}
		else
		{
			strcpy(key_lines[edit_line], key_lines[history_line]);
			key_linepos = strlen(key_lines[edit_line]);
		}
		return;
	}

	if (key == K_PGUP || key == K_KP_PGUP )
	{
		con.display -= 2;
		return;
	}

	if (key == K_PGDN || key == K_KP_PGDN ) 
	{
		con.display += 2;
		if (con.display > con.current)
			con.display = con.current;
		return;
	}

	if (key == K_HOME || key == K_KP_HOME )
	{
		con.display = con.current - con.totallines + 10;
		return;
	}

	if (key == K_END || key == K_KP_END )
	{
		con.display = con.current;
		return;
	}
	
	if (key < 32 || key > 127)
		return;	// non printable
		
	if (key_linepos < MAXCMDLINE-1)
	{
		key_lines[edit_line][key_linepos] = key;
		key_linepos++;
		key_lines[edit_line][key_linepos] = 0;
	}

}

void Key_Event (int key, qboolean down, unsigned time)
{

	// hack for modal presses
	if (key_waiting == -1)
	{
		if (down)
			key_waiting = key;
		return;
	}

	// update auto-repeat status
	if (down)
	{
		key_repeats[key]++;
		if (key != K_BACKSPACE 
			&& key != K_PAUSE 
			&& key != K_PGUP 
			&& key != K_KP_PGUP 
			&& key != K_PGDN
			&& key != K_KP_PGDN
			&& key_repeats[key] > 1)
			return;	// ignore most autorepeats
			
		if (key >= 200 && !keybindings[key])
			Com_Printf ("%s is unbound, hit F4 to set.\n", Key_KeynumToString (key) );
	}
	else
	{
		key_repeats[key] = 0;
	}

	if (key == K_SHIFT)
		shift_down = down;

	// console key is hardcoded, so the user can never unbind it
	if (key == '`' || key == '~')
	{
		if (!down)
			return;
		Con_ToggleConsole_f ();
		return;
	}

	// any key during the attract mode will bring up the menu
	if (cl.attractloop && cls.key_dest != key_menu)
		key = K_ESCAPE;

	// menu key is hardcoded, so the user can never unbind it
	if (key == K_ESCAPE)
	{
		if (!down)
			return;

		if (cl.frame.playerstate.stats[STAT_LAYOUTS] && cls.key_dest == key_game)
		{	// put away help computer / inventory
			Cbuf_AddText ("cmd putaway\n");
			return;
		}
		switch (cls.key_dest)
		{
		case key_message:
			Key_Message (key);
			break;
		case key_menu:
			M_Keydown (key);
			break;
		case key_game:
		case key_console:
			M_Menu_Main_f ();
			break;
		default:
			Com_Error (ERR_FATAL, "Bad cls.key_dest");
		}
		return;
	}

	// track if any key is down for BUTTON_ANY
	keydown[key] = down;
	if (down)
	{
		if (key_repeats[key] == 1)
			anykeydown++;
	}
	else
	{
		anykeydown--;
		if (anykeydown < 0)
			anykeydown = 0;
	}

//
// key up events only generate commands if the game key binding is
// a button command (leading + sign).  These will occur even in console mode,
// to keep the character from continuing an action started before a console
// switch.  Button commands include the kenum as a parameter, so multiple
// downs can be matched with ups
//
	if (!down)
	{
		kb = keybindings[key];
		if (kb && kb[0] == '+')
		{
			Com_sprintf (cmd, sizeof(cmd), "-%s %i %i\n", kb+1, key, time);
			Cbuf_AddText (cmd);
		}
		if (keyshift[key] != key)
		{
			kb = keybindings[keyshift[key]];
			if (kb && kb[0] == '+')
			{
				Com_sprintf (cmd, sizeof(cmd), "-%s %i %i\n", kb+1, key, time);
				Cbuf_AddText (cmd);
			}
		}
		return;
	}

//
// if not a consolekey, send to the interpreter no matter what mode is
//
	if ( (cls.key_dest == key_menu && menubound[key])
	|| (cls.key_dest == key_console && !consolekeys[key])
	|| (cls.key_dest == key_game && ( cls.state == ca_active || !consolekeys[key] ) ) )
	{
		kb = keybindings[key];
		if (kb)
		{
			if (kb[0] == '+')
			{	// button commands add keynum and time as a parm
				Com_sprintf (cmd, sizeof(cmd), "%s %i %i\n", kb, key, time);
				Cbuf_AddText (cmd);
			}
			else
			{
				Cbuf_AddText (kb);
				Cbuf_AddText ("\n");
			}
		}
		return;
	}

	if (!down)
		return;		// other systems only care about key down events

	if (shift_down)
		key = keyshift[key];

	switch (cls.key_dest)
	{
	case key_message:
		Key_Message (key);
		break;
	case key_menu:
		M_Keydown (key);
		break;

	case key_game:
	case key_console:
		Key_Console (key);
		break;
	default:
		Com_Error (ERR_FATAL, "Bad cls.key_dest");
	}
}

/*
===================
Key_GetKey
===================
*/
int Key_GetKey (void)
{
	key_waiting = -1;

	while (key_waiting == -1)
		Sys_SendKeyEvents ();

	return key_waiting;
}