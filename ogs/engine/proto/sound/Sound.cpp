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
#include "sound/Sound.hpp"
#include "sound/ISound.hpp" // ISoundImpl

CSound *gpSound = nullptr;

void S_Init()
{
	gpSound->Init();
};

void S_Shutdown()
{
	gpSound->Shutdown();
};

void S_Update()
{
	gpSound->Update();
};

/*
================
S_Init
================
*/
bool CSound::Init()
{
	mpConsole->Printf("\nSound Initialization\n");
	
	if(COM_CheckParm("-nosound"))
		return true;
	
	if(COM_CheckParm("-simsound"))
		fakedma = true;
	
	if(!mpSound->Init())
		return false;
	
	gpSound = this;
	return true;
};

void CSound::Shutdown()
{
	mpSound->Shutdown();
};

void CSound::Update()
{
	mpSound->Update();
	
	CL_UpdateSoundFade();
};