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

#include "precompiled.hpp"
//#include "commondef.hpp"
#include "voice/voicetweak.hpp"

/*
=================

IVoiceTweak implementation

=================
*/

namespace
{

//VoiceTweak_StartVoiceTweakMode
int Voice_StartVoiceTweakMode()
{
	// TODO: implement
	return 0;
};

//VoiceTweak_EndVoiceTweakMode
void Voice_EndVoiceTweakMode()
{
	// TODO: implement
};

//VoiceTweak_SetControlFloat
void Voice_SetControlFloat(VoiceTweakControl iControl, float value)
{
	// TODO: implement
};

//VoiceTweak_GetControlFloat
float Voice_GetControlFloat(VoiceTweakControl iControl)
{
	// TODO: implement
	return 1.0f; // 0.0f
};

//VoiceTweak_GetSpeakingVolume
int Voice_GetSpeakingVolume()
{
	// TODO: implement
	return 255; // 0
};

}; // namespace

//g_VoiceTweakAPI
IVoiceTweak gVoiceAPI = 
{
	Voice_StartVoiceTweakMode,
	Voice_EndVoiceTweakMode,
	
	Voice_SetControlFloat,
	Voice_GetControlFloat,
	
	Voice_GetSpeakingVolume
};

/*
IVoiceTweak g_VoiceTweakAPI = 
{
	&VoiceTweak_StartVoiceTweakMode,
	&VoiceTweak_EndVoiceTweakMode,
	&VoiceTweak_SetControlFloat,
	&VoiceTweak_GetControlFloat,
	&VoiceTweak_GetSpeakingVolume
};
*/