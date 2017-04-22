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
#include "console/ConCmdSystem.hpp"

#ifdef ID_RETAIL
idCVar net_allowCheats( "net_allowCheats", "0", CVAR_BOOL | CVAR_ROM, "Allow cheats in multiplayer" );
#else
idCVar net_allowCheats( "net_allowCheats", "0", CVAR_BOOL | CVAR_NOCHEAT, "Allow cheats in multiplayer" );
#endif

/*
===============================================================================

	idCmdSystemLocal

===============================================================================
*/

CConCmdSystem gConCmdSystem;
IConCmdSystem *cmdSystem = &gConCmdSystemL;

/*
================================================
idSort_CommandDef 
================================================
*/
class idSort_CommandDef : public idSort_Quick< CConCmd, idSort_CommandDef > {
public:
	int Compare( const CConCmd & a, const CConCmd & b ) const { return idStr::Icmp( a.name, b.name ); }
};

/*
============
idCmdSystemLocal::ListByFlags
============
*/
void idCmdSystemLocal::ListByFlags( const idCmdArgs &args, cmdFlags_t flags ) {
	int i;
	idStr match;
	const CConCmd *cmd;
	idList<const CConCmd *> cmdList;

	if ( args.Argc() > 1 ) {
		match = args.Args( 1, -1 );
		match.Replace( " ", "" );
	} else {
		match = "";
	}

	for ( cmd = cmdSystemLocal.GetCommands(); cmd; cmd = cmd->next ) {
		if ( !( cmd->flags & flags ) ) {
			continue;
		}
		if ( match.Length() && idStr( cmd->name ).Filter( match, false ) == 0 ) {
			continue;
		}

		cmdList.Append( cmd );
	}

	//cmdList.SortWithTemplate( idSort_CommandDef() );

	for ( i = 0; i < cmdList.Num(); i++ ) {
		cmd = cmdList[i];

		common->Printf( "  %-21s %s\n", cmd->name, cmd->description );
	}

	common->Printf( "%i commands\n", cmdList.Num() );
}

/*
============
idCmdSystemLocal::List_f
============
*/
void idCmdSystemLocal::List_f( const idCmdArgs &args ) {
	idCmdSystemLocal::ListByFlags( args, CMD_FL_ALL );
}

/*
============
idCmdSystemLocal::SystemList_f
============
*/
void idCmdSystemLocal::SystemList_f( const idCmdArgs &args ) {
	idCmdSystemLocal::ListByFlags( args, CMD_FL_SYSTEM );
}

/*
============
idCmdSystemLocal::RendererList_f
============
*/
void idCmdSystemLocal::RendererList_f( const idCmdArgs &args ) {
	idCmdSystemLocal::ListByFlags( args, CMD_FL_RENDERER );
}

/*
============
idCmdSystemLocal::SoundList_f
============
*/
void idCmdSystemLocal::SoundList_f( const idCmdArgs &args ) {
	idCmdSystemLocal::ListByFlags( args, CMD_FL_SOUND );
}

/*
============
idCmdSystemLocal::GameList_f
============
*/
void idCmdSystemLocal::GameList_f( const idCmdArgs &args ) {
	idCmdSystemLocal::ListByFlags( args, CMD_FL_GAME );
}

/*
============
idCmdSystemLocal::ToolList_f
============
*/
void idCmdSystemLocal::ToolList_f( const idCmdArgs &args ) {
	idCmdSystemLocal::ListByFlags( args, CMD_FL_TOOL );
}

/*
===============
idCmdSystemLocal::Exec_f
===============
*/
void idCmdSystemLocal::Exec_f( const idCmdArgs &args ) {
	char *	f;
	int		len;
	idStr	filename;

	if ( args.Argc () != 2 ) {
		common->Printf( "exec <filename> : execute a script file\n" );
		return;
	}

	filename = args.Argv(1);
	filename.DefaultFileExtension( ".cfg" );
	len = fileSystem->ReadFile( filename, reinterpret_cast<void **>(&f), NULL );
	if ( !f ) {
		common->Printf( "couldn't exec %s\n", args.Argv(1) );
		return;
	}
	common->Printf( "execing %s\n", args.Argv(1) );
	
	cmdSystemLocal.BufferCommandText( CMD_EXEC_INSERT, f );

	fileSystem->FreeFile( f );
}

/*
===============
idCmdSystemLocal::Vstr_f

Inserts the current value of a cvar as command text
===============
*/
void idCmdSystemLocal::Vstr_f( const idCmdArgs &args ) {
	const char *v;

	if ( args.Argc () != 2 ) {
		common->Printf( "vstr <variablename> : execute a variable command\n" );
		return;
	}

	v = cvarSystem->GetCVarString( args.Argv( 1 ) );

	cmdSystemLocal.BufferCommandText( CMD_EXEC_APPEND, va( "%s\n", v ) );
}

/*
===============
idCmdSystemLocal::Echo_f

Just prints the rest of the line to the console
===============
*/
void idCmdSystemLocal::Echo_f( const idCmdArgs &args ) {
	int		i;
	
	for ( i = 1; i < args.Argc(); i++ ) {
		common->Printf( "%s ", args.Argv( i ) );
	}
	common->Printf( "\n" );
}

/*
============
idCmdSystemLocal::Wait_f

Causes execution of the remainder of the command buffer to be delayed until next frame.
============
*/
void idCmdSystemLocal::Wait_f( const idCmdArgs &args ) {
	if ( args.Argc() == 2 ) {
		cmdSystemLocal.SetWait( atoi( args.Argv( 1 ) ) );
	} else {
		cmdSystemLocal.SetWait( 1 );
	}
}

/*
============
idCmdSystemLocal::Parse_f

This just prints out how the rest of the line was parsed, as a debugging tool.
============
*/
void idCmdSystemLocal::Parse_f( const idCmdArgs &args ) {
	int		i;

	for ( i = 0; i < args.Argc(); i++ ) {
		common->Printf( "%i: %s\n", i, args.Argv(i) );
	}
}

/*
============
idCmdSystemLocal::Init
============
*/
void idCmdSystemLocal::Init() {

	AddCommand( "listCmds", List_f, CMD_FL_SYSTEM, "lists commands" );
	AddCommand( "listSystemCmds", SystemList_f, CMD_FL_SYSTEM, "lists system commands" );
	AddCommand( "listRendererCmds", RendererList_f, CMD_FL_SYSTEM, "lists renderer commands" );
	AddCommand( "listSoundCmds", SoundList_f, CMD_FL_SYSTEM, "lists sound commands" );
	AddCommand( "listGameCmds", GameList_f, CMD_FL_SYSTEM, "lists game commands" );
	AddCommand( "listToolCmds", ToolList_f, CMD_FL_SYSTEM, "lists tool commands" );
	AddCommand( "exec", Exec_f, CMD_FL_SYSTEM, "executes a config file", ArgCompletion_ConfigName );
	AddCommand( "vstr", Vstr_f, CMD_FL_SYSTEM, "inserts the current value of a cvar as command text" );
	AddCommand( "echo", Echo_f, CMD_FL_SYSTEM, "prints text" );
	AddCommand( "parse", Parse_f, CMD_FL_SYSTEM, "prints tokenized string" );
	AddCommand( "wait", Wait_f, CMD_FL_SYSTEM, "delays remaining buffered commands one or more frames" );

	// link in all the commands declared with static idCommandLink variables or CONSOLE_COMMAND macros
	for ( idCommandLink * link = CommandLinks(); link != NULL; link = link->next ) {
		AddCommand( link->cmdName_, link->function_, CMD_FL_SYSTEM, link->description_, link->argCompletion_ );
	}

	completionString = "*";

	textLength = 0;
}

/*
============
idCmdSystemLocal::Shutdown
============
*/
void idCmdSystemLocal::Shutdown() {
	for (CConCmd *cmd = commands; cmd; cmd = commands ) {
		commands = commands->next;
		Mem_Free( cmd->name );
		Mem_Free( cmd->description );
		delete cmd;
	}

	completionString.Clear();
	completionParms.Clear();
	tokenizedCmds.Clear();
	postReload.Clear();
}

/*
============
idCmdSystemLocal::AddCommand
============
*/
void idCmdSystemLocal::AddCommand( const char *cmdName, cmdFunction_t function, int flags, const char *description, argCompletion_t argCompletion ) {
	CConCmd *cmd;
	
	// fail if the command already exists
	for ( cmd = commands; cmd; cmd = cmd->next ) {
		if ( idStr::Cmp( cmdName, cmd->name ) == 0 ) {
			if ( function != cmd->function ) {
				common->Printf( "idCmdSystemLocal::AddCommand: %s already defined\n", cmdName );
			}
			return;
		}
	}

	cmd = new (TAG_SYSTEM) CConCmd;
	cmd->name = Mem_CopyString( cmdName );
	cmd->function = function;
	cmd->argCompletion = argCompletion;
	cmd->flags = flags;
	cmd->description = Mem_CopyString( description );
	cmd->next = commands;
	commands = cmd;
}

/*
============
idCmdSystemLocal::RemoveCommand
============
*/
void idCmdSystemLocal::RemoveCommand( const char *cmdName ) {
	CConCmd *cmd, **last;

	for ( last = &commands, cmd = *last; cmd; cmd = *last ) {
		if ( idStr::Cmp( cmdName, cmd->name ) == 0 ) {
			*last = cmd->next;
			Mem_Free( cmd->name );
			Mem_Free( cmd->description );
			delete cmd;
			return;
		}
		last = &cmd->next;
	}
}

/*
============
idCmdSystemLocal::RemoveFlaggedCommands
============
*/
void idCmdSystemLocal::RemoveFlaggedCommands( int flags ) {
	CConCmd *cmd, **last;

	for ( last = &commands, cmd = *last; cmd; cmd = *last ) {
		if ( cmd->flags & flags ) {
			*last = cmd->next;
			Mem_Free( cmd->name );
			Mem_Free( cmd->description );
			delete cmd;
			continue;
		}
		last = &cmd->next;
	}
}

/*
============
idCmdSystemLocal::CommandCompletion
============
*/
void idCmdSystemLocal::CommandCompletion( void(*callback)( const char *s ) ) {
	for (CConCmd *cmd = commands; cmd; cmd = cmd->next ) {
		callback( cmd->name );
	}
}

/*
============
idCmdSystemLocal::ArgCompletion
============
*/
void idCmdSystemLocal::ArgCompletion( const char *cmdString, void(*callback)( const char *s ) ) {
	CConCmd *cmd;
	idCmdArgs args;

	args.TokenizeString( cmdString, false );

	for ( cmd = commands; cmd; cmd = cmd->next ) {
		if ( !cmd->argCompletion ) {
			continue;
		}
		if ( idStr::Icmp( args.Argv( 0 ), cmd->name ) == 0 ) {
			cmd->argCompletion( args, callback );
			break;
		}
	}
}

/*
============
idCmdSystemLocal::ExecuteTokenizedString
============
*/
void idCmdSystemLocal::ExecuteTokenizedString( const idCmdArgs &args ) {	
	CConCmd *cmd, **prev;
	
	// execute the command line
	if ( !args.Argc() ) {
		return;		// no tokens
	}

	// check registered command functions	
	for ( prev = &commands; *prev; prev = &cmd->next ) {
		cmd = *prev;
		if ( idStr::Icmp( args.Argv( 0 ), cmd->name ) == 0 ) {
			// rearrange the links so that the command will be
			// near the head of the list next time it is used
			*prev = cmd->next;
			cmd->next = commands;
			commands = cmd;

			if ( ( cmd->flags & (CMD_FL_CHEAT|CMD_FL_TOOL) ) && common->IsMultiplayer() && !net_allowCheats.GetBool() ) {
				common->Printf( "Command '%s' not valid in multiplayer mode.\n", cmd->name );
				return;
			}
			// perform the action
			if ( !cmd->function ) {
				break;
			} else {
				cmd->function( args );
			}
			return;
		}
	}
	
	// check cvars
	if ( cvarSystem->Command( args ) ) {
		return;
	}

	common->Printf( "Unknown command '%s'\n", args.Argv( 0 ) );
}

void idCmdSystemLocal::InsertCommandText( const char *text ) {
	int		len;
	int		i;

	len = strlen( text ) + 1;
	if ( len + textLength > (int)sizeof( textBuf ) ) {
		common->Printf( "idCmdSystemLocal::InsertText: buffer overflow\n" );
		return;
	}

	// move the existing command text
	for ( i = textLength - 1; i >= 0; i-- ) {
		textBuf[ i + len ] = textBuf[ i ];
	}

	// copy the new text in
	memcpy( textBuf, text, len - 1 );

	// add a \n
	textBuf[ len - 1 ] = '\n';

	textLength += len;
}

/*
============
idCmdSystemLocal::BufferCommandArgs
============
*/
void idCmdSystemLocal::BufferCommandArgs( cmdExecution_t exec, const idCmdArgs &args ) {
	switch ( exec ) {
		case CMD_EXEC_NOW: {
			ExecuteTokenizedString( args );
			break;
		}
		case CMD_EXEC_APPEND: {
			AppendCommandText( "_execTokenized\n" );
			tokenizedCmds.Append( args );
			break;
		}
		default: {
			common->FatalError( "idCmdSystemLocal::BufferCommandArgs: bad exec type" );
		}
	}
}

void idCmdSystemLocal::ExecuteCommandBuffer() {
	
	idCmdArgs	args;

	while( textLength ) {

		if ( wait )	{
			// skip out while text still remains in buffer, leaving it for next frame
			wait--;
			break;
		}

		for ( i = 0; i < textLength; i++ ) {
			
		}
			
		text[i] = 0;

		if ( !idStr::Cmp( text, "_execTokenized" ) ) {
			args = tokenizedCmds[ 0 ];
			tokenizedCmds.RemoveIndex( 0 );
		} else {
			args.TokenizeString( text, false );
		}

		// execute the command line that we have already tokenized
		ExecuteTokenizedString( args );
	}
}

/*
============
idCmdSystemLocal::ArgCompletion_FolderExtension
============
*/
void idCmdSystemLocal::ArgCompletion_FolderExtension( const idCmdArgs &args, void(*callback)( const char *s ), const char *folder, bool stripFolder, ... ) {
	int i;
	idStr string;
	const char *extension;
	va_list argPtr;

	string = args.Argv( 0 );
	string += " ";
	string += args.Argv( 1 );

	if ( string.Icmp( completionString ) != 0 ) {
		idStr parm, path;
		idFileList *names;

		completionString = string;
		completionParms.Clear();

		parm = args.Argv( 1 );
		parm.ExtractFilePath( path );
		if ( stripFolder || path.Length() == 0 ) {
			path = folder + path;
		}
		path.StripTrailing( '/' );

		// list folders
		names = fileSystem->ListFiles( path, "/", true, true );
		for ( i = 0; i < names->GetNumFiles(); i++ ) {
			idStr name = names->GetFile( i );
			if ( stripFolder ) {
				name.Strip( folder );
			} else {
				name.Strip( "/" );
			}
			name = args.Argv( 0 ) + ( " " + name ) + "/";
			completionParms.Append( name );
		}
		fileSystem->FreeFileList( names );

		// list files
		va_start( argPtr, stripFolder );
		for ( extension = va_arg( argPtr, const char * ); extension; extension = va_arg( argPtr, const char * ) ) {
			names = fileSystem->ListFiles( path, extension, true, true );
			for ( i = 0; i < names->GetNumFiles(); i++ ) {
				idStr name = names->GetFile( i );
				if ( stripFolder ) {
					name.Strip( folder );
				} else {
					name.Strip( "/" );
				}
				name = args.Argv( 0 ) + ( " " + name );
				completionParms.Append( name );
			}
			fileSystem->FreeFileList( names );
		}
		va_end( argPtr );
	}
	for ( i = 0; i < completionParms.Num(); i++ ) {
		callback( completionParms[i] );
	}
}

/*
============
idCmdSystemLocal::ArgCompletion_DeclName
============
*/
void idCmdSystemLocal::ArgCompletion_DeclName( const idCmdArgs &args, void(*callback)( const char *s ), int type ) {
	int i, num;

	if ( declManager == NULL ) {
		return;
	}
	num = declManager->GetNumDecls( (declType_t)type );
	for ( i = 0; i < num; i++ ) {
		callback( idStr( args.Argv( 0 ) ) + " " + declManager->DeclByIndex( (declType_t)type, i , false )->GetName() );
	}
}
