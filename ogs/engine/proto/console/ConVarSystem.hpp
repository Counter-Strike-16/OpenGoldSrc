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

#include "console/IConVarSystem.hpp"

constexpr auto NUM_COLUMNS = 77; // 78 - 1
constexpr auto NUM_NAME_CHARS = 33;
constexpr auto NUM_DESCRIPTION_CHARS = ( NUM_COLUMNS - NUM_NAME_CHARS );
constexpr auto FORMAT_STRING = "%-32s ";

class CConVarSystem : public IConVarSystem
{
public:
	CConVarSystem();
	virtual ~CConVarSystem() {}

	virtual void			Init();
	virtual void			Shutdown();
	virtual bool			IsInitialized() const;

	virtual void			Register( IConVar *cvar );

	virtual IConVar *		Find( const char *name );

	virtual void			SetCVarString( const char *name, const char *value, int flags = 0 );
	virtual void			SetCVarBool( const char *name, const bool value, int flags = 0 );
	virtual void			SetCVarInteger( const char *name, const int value, int flags = 0 );
	virtual void			SetCVarFloat( const char *name, const float value, int flags = 0 );

	virtual const char *	GetCVarString( const char *name ) const;
	virtual bool			GetCVarBool( const char *name ) const;
	virtual int				GetCVarInteger( const char *name ) const;
	virtual float			GetCVarFloat( const char *name ) const;

	virtual bool			HandleCommand( const IConCmdArgs &args );

	virtual void			CommandCompletion( void(*callback)( const char *s ) );
	virtual void			ArgCompletion( const char *cmdString, void(*callback)( const char *s ) );

	virtual void			SetModifiedFlags( int flags );
	virtual int				GetModifiedFlags() const;
	virtual void			ClearModifiedFlags( int flags );

	virtual void			ResetFlaggedVariables( int flags );
	virtual void			RemoveFlaggedAutoCompletion( int flags );
	virtual void			WriteFlaggedVariables( int flags, const char *setCmd, idFile *f ) const;

	virtual void			MoveCVarsToDict( int flags, idDict & dict, bool onlyModified ) const;
	virtual void			SetCVarsFromDict( const idDict &dict );

	void					RegisterInternal( CConVar *cvar );
	idInternalCVar *		FindInternal( const char *name ) const;
	void					SetInternal( const char *name, const char *value, int flags );

private:
	bool					initialized;
	idList<idInternalCVar*, TAG_CVAR>	cvars;
	idHashIndex				cvarHash;
	int						modifiedFlags;

private:
	static void				Toggle_f( const idCmdArgs &args );
	static void				Set_f( const idCmdArgs &args );
	static void				Reset_f( const idCmdArgs &args );
	static void				ListByFlags( const idCmdArgs &args, cvarFlags_t flags );
	static void				List_f( const idCmdArgs &args );
	static void				Restart_f( const idCmdArgs &args );
	static void				CvarAdd_f( const idCmdArgs &args );
};