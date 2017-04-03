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
/// @brief video module impl

#pragma once

#include "video/IVideo.hpp"

class CVideo : public IVideo
{
public:
	// Video module initialization, etc
	// Called at startup to set up translation tables, takes 256 8 bit RGB values
	// the palette data will go away after the call, so it must be copied off if
	// the video driver will need it again
	NOBODY int Init(unsigned short *palette); // was void (uchar*)
	
	// Called at shutdown
	void Shutdown();
	
	// flushes the given rectangles from the view buffer to the screen
	void Update(wrect_t *rects); // was vrect_t
	
	IWindow *OpenWindow(const TWindowCreateProps &aWinProps) const;
	void CloseWindow(const IWindow *&apWindow);
	
	// sets the mode; only used by the engine for resetting to mode 0 
	// (the base mode) on memory allocation failures
	int SetMode(int modenum, unsigned char *palette);
	
	void SetDefaultMode();
private:
	void CheckChanges();
};