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

struct EXCEPTION_REGISTRATION
{
	EXCEPTION_REGISTRATION *prev;
	FARPROC handler;
};

extern "C" int _except_handler3(PEXCEPTION_RECORD, EXCEPTION_REGISTRATION *, PCONTEXT, PEXCEPTION_RECORD);
extern "C" int _except_handler4(PEXCEPTION_RECORD, EXCEPTION_REGISTRATION *, CONTEXT *, void *);

#define SetupExceptHandler3()                \
	DWORD handler = (DWORD)_except_handler3; \
                                             \
	__asm push handler __asm push FS : [0] __asm mov FS : [0], ESP

#define SetupExceptHandler4()                \
	DWORD handler = (DWORD)_except_handler4; \
                                             \
	__asm push handler __asm push FS : [0] __asm mov FS : [0], ESP