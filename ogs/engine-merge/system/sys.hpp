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
/// non-portable functions

#pragma once

//
// file IO
//

// returns the file size
// return -1 if file is not present
// the file should be in BINARY mode for stupid OSs that care
int Sys_FileOpenRead(char *path, int *hndl);

int Sys_FileOpenWrite(char *path);
void Sys_FileClose(int handle);
void Sys_FileSeek(int handle, int position);
int Sys_FileRead(int handle, void *dest, int count);
int Sys_FileWrite(int handle, void *data, int count);
int Sys_FileTime(char *path);
void Sys_mkdir(char *path);

//
// memory protection
//
void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length);

//
// system IO
//
void Sys_DebugLog(char *file, char *fmt, ...);

void Sys_Printf(char *fmt, ...);
// send text to the console

double Sys_DoubleTime(void);

void Sys_Sleep(void);
// called to yield for a little bit so as
// not to hog cpu when paused or debugging

void Sys_LowFPPrecision(void);
void Sys_HighFPPrecision(void);
void Sys_SetFPCW(void);

char *Sys_ConsoleInput (void);