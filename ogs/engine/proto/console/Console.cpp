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

#include "precompiled.hpp"
#include "console/Console.hpp"
#include "system/System.hpp"
#include "system/common.hpp"

/*
================
Con_Init
================
*/
bool CConsole::Init()
{
	//con_debuglog = //COM_CheckParm("-condebug");

	//con = &con_main;
	//con_linewidth = -1; // con.
	//Con_CheckResize();

	DevPrintf("Console initialized.\n"); // Con_Printf

	//
	// register our commands
	//
	//Cvar_RegisterVariable(&con_notifytime);

	//Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
	
	//Cmd_AddCommand("togglechat", Con_ToggleChat_f);
	
	//Cmd_AddCommand("messagemode", [this](){Con_MessageMode_f();});
	//Cmd_AddCommand("messagemode2", Con_MessageMode2_f);
	//Cmd_AddCommand("clear", Con_Clear_f);
	//Cmd_AddCommand("condump", Con_Dump_f);

#ifdef HOOK_ENGINE
	//Cmd_AddCommand("condebug", (xcommand_t)GetOriginalFuncAddrOrDefault("Con_Debug_f", (void *)Con_Debug_f));
#else
	//Cmd_AddCommand("condebug", Con_Debug_f);
#endif

	initialized = true; // con.initialized
	return true;
};

void CConsole::Shutdown()
{
#ifndef SWDS
#endif
};

void EXT_FUNC CConsole::Printf(/*int anPrintLevel,*/ const char *asMsg, ...) // _format(1)
{
	va_list arglist; // was va
	char sText[256] = {'\0'}; // was Dest[4096];

	va_start(arglist, asMsg);
	Q_vsnprintf(sText, charsmax(sText), asMsg, arglist);
	va_end(arglist);
	
#ifdef REHLDS_FLIGHT_REC
	FR_Log("REHLDS_CON", sText);
#endif

#ifdef REHLDS_FIXES // && !defined(_DEBUG)
	if(sv_redirected == RD_NONE || sv_rcon_condebug.value > 0.0f)
#endif
	{
		CSystem::Printf("%s", sText);
	};
	
	//if(sv_redirected)
	{
		//if((Q_strlen(outputbuf) + Q_strlen(sText)) > sizeof(outputbuf) - 1)
			//SV_FlushRedirect();
		
		//Q_strncat(outputbuf, sText, sizeof(outputbuf) - 1);
	}
	else
	{
		//if(con_debuglog)
			//DebugLog("qconsole.log", "%s", sText);
		
#ifndef SWDS
		if(host_initialized && con_initialized && cls.state)
		{
			if(developer.value != 0.0f)
			{
				Q_strncpy(g_szNotifyAreaString, msg, 255);
				g_szNotifyAreaString[255] = 0;
				*con_times = realtime;
			};
			
			VGuiWrap2_ConPrintf(msg);
		};
#endif // SWDS
	};
};

void CConsole::DrawCharacter(int cx, int line, int num)
{
};

void CConsole::CheckResize()
{
};

void CConsole::Draw(int lines /*, qboolean drawinput*/)
{
};

//void Draw(float frac);

void CConsole::Print(const char *txt)
{
};

/*
================
Con_DPrintf

A Con_Printf that only shows up if the "developer" cvar is set
================
*/
#if defined(REHLDS_FIXES) && defined(REHLDS_FLIGHT_REC)

//void EXT_FUNC Con_DPrintf(const char *fmt, ...)

void EXT_FUNC CConsole::DevPrintf(const char *fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);

#ifdef REHLDS_FIXES

#ifdef REHLDS_FLIGHT_REC
	// Always print debug logs to the flight recorder
	FR_Log("REHLDS_CONDBG", Dest);
#endif // REHLDS_FLIGHT_REC
	
	if(developer.value != 0.0f)
	{
#endif // REHLDS_FIXES

#ifdef _WIN32
		char Dest[4096];
		Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);
		
		OutputDebugStringA(Dest);

		//if(con_debuglog)
			//DebugLog("qconsole.log", "%s", Dest);

#else // if not _WIN32
		vfprintf(stdout, fmt, argptr);
		fflush(stdout);
#endif // _WIN32

#ifdef REHLDS_FIXES
	};
#endif // REHLDS_FIXES
	
	va_end(argptr);
};

#else // if not defined(REHLDS_FIXES) and not defined(REHLDS_FLIGHT_REC)


void CConsole::DevPrintf(const char *asMsg, ...) // _format(1);
{
	// don't confuse non-developers with techical stuff...
	//if(!developer.value)
		//return;
	
	va_list arglist;
	char sText[256] = {'\0'}; // char msg[MAXPRINTMSG];

	va_start(arglist, asMsg);
	Q_vsnprintf(sText, charsmax(sText), asMsg, arglist); // vsprintf(msg, fmt, argptr);
	va_end(arglist);

	Printf("[DEV] %s", sText);
};

#endif // defined(REHLDS_FIXES) and defined(REHLDS_FLIGHT_REC)

//void CConsole::NPrintf(int idx, const char *fmt, ...)
//{
//};

//void EXT_FUNC CConsole::NPrintf(int idx, /*const*/ char *fmt, ...) // _format(2);
//{
//#ifndef SWDS
//#endif
//};

//void CConsole::NXPrintf(struct con_nprint_s *info, char *fmt, ...) //_format(2)
//{
//};

/*
==================
Con_SafePrintf

Okay to call even when the screen can't be updated
==================
*/
void CConsole::SafePrintf(const char *fmt, ...)
{
	va_list argptr;
	char msg[1024];
	
	va_start(argptr, fmt);
	
#ifdef _WIN32
	char Dest[1024];
	Q_vsnprintf(Dest, sizeof(Dest), fmt, argptr);
	Printf("%s", Dest);
#else
	vfprintf(stdout, fmt, argptr);
	fflush(stdout);
#endif // _WIN32
	
	va_end(argptr);
};

void CConsole::CenteredPrint(const char *text)
{
};

void CConsole::DrawNotify()
{
};

void CConsole::ClearNotify()
{
};

void CConsole::ShowNotifyBox(const char *text)
{
};

void CConsole::DebugLog(const char *file, const char *fmt, ...)
{
};

//IConVar *CConsole::GetConVar(const char *asName)
//{
//};