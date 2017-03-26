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
/// @brief shared engine interface for dedicated server/client

#pragma once

// This interface isn't oriented on export
// (has no version define and isn't inherited from IBaseInterface)
// Only for internal use (in other words can be freely changed)

class IEngine
{
public:
	enum
	{
		QUIT_NOTQUITTING = 0,
		QUIT_TODESKTOP,
		QUIT_RESTART
	};

	virtual ~IEngine(){}
	
	/// Load the engine
	virtual bool Load(bool dedicated, char *basedir, const char *cmdline) = 0;
	
	/// Unload the engine
	virtual void Unload() = 0;
	
	/// Set the new engine state
	virtual void SetState(int iState) = 0;
	
	/// Get current engine state
	virtual int GetState() = 0;
	
	/// Set new new engine sub-state
	virtual void SetSubState(int iSubState) = 0;
	
	/// Get current engine sub-state
	virtual int GetSubState() = 0;
	
	/// Run a single frame
	virtual int Frame() = 0;
	
	/// Get frame time value
	virtual double GetFrameTime() = 0;
	
	/// Get current time value
	virtual double GetCurTime() = 0;
	
	/// Event for keyboard when in trapping mode
	virtual void TrapKey_Event(int key, bool down) = 0;
	
	/// Event for mouse when in trapping mode
	virtual void TrapMouse_Event(int buttons, bool down) = 0;
	
	/// Start the input trapping mode
	virtual void StartTrapMode() = 0;
	
	/// @return true if currently in trapping mode
	virtual bool IsTrapping() = 0;
	
	/// @return true if the trapping mode is done
	virtual bool CheckDoneTrapping(int &buttons, int &key) = 0;
	
	/// Should the engine be closed
	virtual int GetQuitting() = 0;
	
	/// Mark engine for quit
	virtual void SetQuitting(int quittype) = 0;
	
	// OGS extensions
	
	/// Add the text to console command buffer
	virtual void AddCommandText(const char *asText) = 0;
	
	/// Get the info about current player count and map
	virtual void GetHostInfo(float *fps, int *nActive, int *unused, int *nMaxPlayers, char *pszMap) = 0;
};

extern IEngine *eng;