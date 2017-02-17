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

#define CONST_INTEGER_AS_STRING(x) #x // Wraps the integer in quotes, allowing us to form constant strings with it
#define __HACK_LINE_AS_STRING__(x) CONST_INTEGER_AS_STRING(x) // __LINE__ can only be converted to an actual number by going through this, otherwise the output is literally "__LINE__"
#define __LINE__AS_STRING __HACK_LINE_AS_STRING__(__LINE__) // Gives you the line number in constant string form

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
	#define NOXREFCHECK		   __asm { push [ebp + 4] } Sys_Error("[NOXREFCHECK]:" __FUNCTION__ " (" __FILE__ ":"__LINE__AS_STRING") NOXREF, but called from 0x%.08x")
#else
	#define NOXREFCHECK			const char* noxref_msg = "[NOXREFCHECK]:" __FUNCTION__ " (" __FILE__ ":"__LINE__AS_STRING") NOXREF, but called from 0x%.08x"; \
								asm volatile (				\
								"pushl 4(%%ebp)\n\t"	\
								"pushl %0\n\t"			\
								"call $Sys_Error\n\t"	\
								"addl %%esp, $8\n\t"	\
								::						\
								"m" (noxref_msg)			\
								);
#endif

// clang-format off

#ifndef REHLDS_FIXES
	#define __BUILD_TIME__ __TIME__
	#define __BUILD_DATE__ __DATE__
#else // REHLDS_FIXES
	#define __BUILD_TIME__ APP_COMMIT_TIME
	#define __BUILD_DATE__ APP_COMMIT_DATE
#endif // REHLDS_FIXES

// clang-format on

const int MAX_DISCONNECT_REASON = 256;

// Note: move these below to memorytypes?

const int FIFTEEN_MB = 15 * 1024 * 1024;

const int DEFAULT_MEMORY = 0x2800000;
const int WARNING_MEMORY = 0x0200000;

const int MINIMUM_WIN_MEMORY = 0x0e00000;
const int MAXIMUM_WIN_MEMORY = 0x8000000; // Ask for 128 MB max

const int MAXIMUM_DEDICATED_MEMORY = 0x8000000; // Ask for 128 MB max