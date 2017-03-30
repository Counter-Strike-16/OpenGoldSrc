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

bool CConsole::Init()
{
	return true;
};

void CConsole::Shutdown()
{
};

void CConsole::Printf(/*int anPrintLevel,*/ const char *asMsg, ...)
{
	//Con_Print();

#ifdef _DEBUG
	va_list arglist;
	char sText[256] = {'\0'};

	va_start(arglist, asMsg);
	Q_vsnprintf(sText, charsmax(sText), asMsg, arglist);
	va_end(arglist);

	CSystem::Printf(sText);
#endif // _DEBUG
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

//void CConsole::Printf(const char *fmt, ...) // _format(1);
//{
//};

void CConsole::DevPrintf(const char *asMsg, ...) // _format(1);
{
#ifdef _DEBUG
	va_list arglist;
	char sText[256] = {'\0'};

	va_start(arglist, asMsg);
	Q_vsnprintf(sText, charsmax(sText), asMsg, arglist);
	va_end(arglist);

	CSystem::Printf("[DEV] %s", sText);
#endif // _DEBUG
};

//void CConsole::NPrintf(int idx, const char *fmt, ...)
//{
//};

//void CConsole::NPrintf( int idx, char *fmt, ... ) // _format(2);
//{
//};

//void CConsole::NXPrintf( struct con_nprint_s *info, char *fmt, ... ) //_format(2)
//{
//};

void CConsole::SafePrintf(const char *fmt, ...)
{
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