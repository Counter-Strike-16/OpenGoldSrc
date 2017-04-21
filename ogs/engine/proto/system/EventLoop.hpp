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

/*
===============================================================================

	The event loop receives events from the system and dispatches them to
	the various parts of the engine. The event loop also handles journaling.
	The file system copies .cfg files to the journaled file.

===============================================================================
*/

constexpr auto MAX_PUSHED_EVENTS = 64;

class CEventLoop
{
public:
	CEventLoop() = default;
	~CEventLoop() = default;

	void Init();

	// Closes the journal file if needed
	void Shutdown();

	// It is possible to get an event at the beginning of a frame that
	// has a time stamp lower than the last event from the previous frame.
	sysEvent_t GetEvent();

	// Dispatches all pending events and returns the current time
	int	Run( bool bCmdExec = true );

	// Gets the current time in a way that will be journaled properly,
	// as opposed to Sys_Milliseconds(), which always reads a real timer.
	int GetMilliseconds();

	// Returns the journal level, 1 = record, 2 = play back
	int GetJournalLevel() const;

	// Journal file
	CFile *com_journalFile{nullptr};
	CFile *com_journalDataFile{nullptr};
private:
	// All events will have this subtracted from their time
	int	initialTimeOffset{0};

	int				com_pushedEventsHead, com_pushedEventsTail;
	sysEvent_t		com_pushedEvents[MAX_PUSHED_EVENTS];

	static CConVar com_journal;

	sysEvent_t		GetRealEvent();
	void			Process( sysEvent_t ev );
	void			Push( sysEvent_t *event );
};

extern CEventLoop *eventLoop;