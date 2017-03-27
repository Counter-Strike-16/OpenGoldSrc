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

#include "ui/GameUIFuncs.hpp"

EXPOSE_SINGLE_INTERFACE(CGameUIFuncs, IGameUIFuncs, VENGINE_GAMEUIFUNCS_VERSION);

bool CGameUIFuncs::IsKeyDown(char const *keyname, bool &isdown)
{
	return false;
};

const char *CGameUIFuncs::Key_NameForKey(int keynum)
{
	return "";
};

const char *CGameUIFuncs::Key_BindingForKey(int keynum)
{
	return "";
};

vgui::KeyCode CGameUIFuncs::GetVGUI2KeyCodeForBind(const char *bind)
{
	return 0;
};

void CGameUIFuncs::GetVideoModes(struct vmode_s **liststart, int *count)
{
};

void CGameUIFuncs::GetCurrentVideoMode(int *wide, int *tall, int *bpp)
{
};

void CGameUIFuncs::GetCurrentRenderer(char *name, int namelen, int *windowed)
{
};

bool CGameUIFuncs::IsConnectedToVACSecureServer()
{
	return false;
};

int CGameUIFuncs::Key_KeyStringToKeyNum(const char *string)
{
	return 0;
};