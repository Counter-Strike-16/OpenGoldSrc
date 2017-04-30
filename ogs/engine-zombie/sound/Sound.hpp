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

#pragma once

struct ISound;
class CSoundChannel;

class CSound
{
public:
	CSound(ISound *apImpl)
	: mpImpl(apImpl), mbInitialized(false), mnBlocked(0), mnStarted(0){}
	
	bool Init(int anDesiredSpeed, int anDesiredBits);
	void Shutdown();
	
	void Update();
	void ExtraUpdate();
	
	void SetListenerParams(vec_t *origin, vec_t *v_forward, vec_t *v_right, vec_t *v_up);
	
	void StopAllSounds(bool abClear);
	void StopAllSoundsC();
	
	void SetAmbientActive(bool abActive){mbAmbient = abActive;}
	bool IsAmbientActive() const {return mbAmbient;}
	
	void SetBlocked(bool abBlocked){mnBlocked = abBlocked ? 1 : 0;}
	bool IsBlocked() const {return mnBlocked;}
	
	bool IsInitialized() const {return mbInitialized;}
private:
	void UpdateAmbientSounds();
	
	//=======================================================================
	// Internal sound data & structures
	//=======================================================================
	
	// 0 to MAX_DYNAMIC_CHANNELS-1	= normal entity sounds
	// MAX_DYNAMIC_CHANNELS to MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS -1 = water, etc
	// MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS to total_channels = static sounds
	CSoundChannel channels[MAX_CHANNELS];
	
	int total_channels;
	
	sfx_t *ambient_sfx[NUM_AMBIENTS];
	
	ISound *mpImpl;
	
	bool mbInitialized;
	bool mbAmbient;
	
	int mnBlocked;
	int mnStarted;
};