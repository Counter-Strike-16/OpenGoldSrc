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

#pragma once

#include "vgui2/IInput.hpp"

namespace vgui
{

class CInput : public IInput
{
public:
	void SetMouseFocus(VPANEL newMouseFocus);
	void SetMouseCapture(VPANEL panel);
	
	void GetKeyCodeText(KeyCode code, char *buf, int buflen);
	
	VPANEL GetFocus();
	VPANEL GetMouseOver();
	
	void SetCursorPos(int x, int y);
	void GetCursorPos(int &x, int &y);
	
	bool WasMousePressed(MouseCode code);
	bool WasMouseDoublePressed(MouseCode code);
	
	bool IsMouseDown(MouseCode code);
	
	void SetCursorOveride(HCursor cursor);
	HCursor GetCursorOveride();
	
	bool WasMouseReleased(MouseCode code);
	bool WasKeyPressed(KeyCode code);
	
	bool IsKeyDown(KeyCode code);
	
	bool WasKeyTyped(KeyCode code);
	bool WasKeyReleased(KeyCode code);
	
	VPANEL GetAppModalSurface();
	void SetAppModalSurface(VPANEL panel);
	void ReleaseAppModalSurface();
	
	void GetCursorPosition(int &x, int &y);
};

}; // namespace vgui