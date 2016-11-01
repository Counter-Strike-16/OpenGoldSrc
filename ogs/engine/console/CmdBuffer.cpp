/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "CmdBuffer.h"

/*
=============================================================================

						COMMAND BUFFER

=============================================================================
*/

CCmdBuffer::CCmdBuffer()
{
};

CCmdBuffer::~CCmdBuffer()
{
};

/*
============
Cbuf_Init
============
*/
void CCmdBuffer::Init()
{
	SZ_Alloc (&cmd_text, 8192);		// space for commands and script files
}

/*
============
Cbuf_AddText

Adds command text at the end of the buffer
============
*/
void CCmdBuffer::AddText(char *text)
{
	int l = Q_strlen (text);

	if (cmd_text.cursize + l >= cmd_text.maxsize)
	{
		Con_Printf ("Cbuf_AddText: overflow\n");
		return;
	}

	SZ_Write (&cmd_text, text, Q_strlen (text));
}

/*
============
Cbuf_InsertText

Adds command text immediately after the current command
Adds a \n to the text
FIXME: actually change the command buffer to do less copying
============
*/
void CCmdBuffer::InsertText(char *text)
{
	char *temp = NULL;
	int templen;

	// copy off any commands still remaining in the exec buffer
	templen = cmd_text.cursize;
	if (templen)
	{
		temp = Z_Malloc (templen);
		Q_memcpy (temp, cmd_text.data, templen);
		SZ_Clear (&cmd_text);
	};
		
	// add the entire text of the file
	AddText(text);
	//SZ_Write(&cmd_text, "\n", 1);
	
	// add the copied off data
	if (templen)
	{
		SZ_Write (&cmd_text, temp, templen);
		Z_Free (temp);
	}
}

/*
============
Cbuf_Execute
============
*/
void CCmdBuffer::Execute()
{
	int		i;
	char	*text;
	char	line[1024];
	int		quotes;
	
	while (cmd_text.cursize)
	{
// find a \n or ; line break
		text = (char *)cmd_text.data;

		quotes = 0;
		
		for (i=0 ; i< cmd_text.cursize ; i++)
		{
			if (text[i] == '"')
				quotes++;
			if ( !(quotes&1) &&  text[i] == ';')
				break;	// don't break if inside a quoted string
			if (text[i] == '\n')
				break;
		};
		
		memcpy (line, text, i);
		line[i] = 0;
		
// delete the text from the command buffer and move remaining commands down
// this is necessary because commands (exec, alias) can insert data at the
// beginning of the text buffer

		if (i == cmd_text.cursize)
			cmd_text.cursize = 0;
		else
		{
			i++;
			cmd_text.cursize -= i;
			Q_memcpy (text, text+i, cmd_text.cursize);
		}

// execute the command line
		Cmd_ExecuteString (line, src_command);
		
		if (cmd_wait)
		{	// skip out while text still remains in buffer, leaving it
			// for next frame
			cmd_wait = false;
			break;
		}
	}
}