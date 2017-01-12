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
/// @brief system module types

#pragma once

// clang-format off

#if defined(_WIN32) && !defined(__MINGW32__)
	#define NOXREFCHECK       \
		__asm { push [ebp + 4]} \
		Sys_Error(__FUNCTION__ " NOXREF, but called from 0x%.08x")
#else
	#define NOXREFCHECK \
		int *a = 0;     \
		*a     = 0
#endif

// clang-format on

const int MAX_DISCONNECT_REASON = 256;

// Note: move these below to memorytypes?

const int FIFTEEN_MB = 15 * 1024 * 1024;

const int DEFAULT_MEMORY = 0x2800000;
const int WARNING_MEMORY = 0x0200000;

const int MINIMUM_WIN_MEMORY = 0x0e00000;
const int MAXIMUM_WIN_MEMORY = 0x8000000; // Ask for 128 MB max

const int MAXIMUM_DEDICATED_MEMORY = 0x8000000; // Ask for 128 MB max