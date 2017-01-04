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

#pragma once

// TODO: I think this defines must be in /common/
#define NUM_EDICTS 900
#define MAX_NAME 32
#define MAX_PACKET_ENTITIES 256

#define DEFAULT_SOUND_PACKET_VOLUME 255
#define DEFAULT_SOUND_PACKET_ATTENUATION 1.0f
#define DEFAULT_SOUND_PACKET_PITCH 100

// Sound flags
#define SND_FL_VOLUME BIT(0)       // send volume
#define SND_FL_ATTENUATION BIT(1)  // send attenuation
#define SND_FL_LARGE_INDEX BIT(2)  // send sound number as short instead of byte
#define SND_FL_PITCH BIT(3)        // send pitch
#define SND_FL_SENTENCE BIT(4)     // set if sound num is actually a sentence num
#define SND_FL_STOP BIT(5)         // stop the sound
#define SND_FL_CHANGE_VOL BIT(6)   // change sound vol
#define SND_FL_CHANGE_PITCH BIT(7) // change sound pitch
#define SND_FL_SPAWNING BIT(8)     // we're spawning, used in some cases for ambients (not sent across network)

// Message send destination flags
#define MSG_FL_NONE 0           // No flags
#define MSG_FL_BROADCAST BIT(0) // Broadcast?
#define MSG_FL_PVS BIT(1)       // Send to PVS
#define MSG_FL_PAS BIT(2)       // Send to PAS
#define MSG_FL_ONE BIT(7)       // Send to single client

#define RESOURCE_INDEX_BITS 12
#ifdef REHLDS_FIXES
#define RESOURCE_MAX_COUNT (1 << RESOURCE_INDEX_BITS)
#endif // REHLDS_FIXES

#define HL_SOUND_MAX 512
#define HL_SOUND_HASHLOOKUP_SIZE (HL_SOUND_MAX * 2 - 1)

#define HL_MODEL_MAX 512
#define HL_GENERIC_MAX 512
#define HL_EVENT_MAX 256

#define MAX_RESOURCE_LIST 1280

typedef enum redirect_e {
	RD_NONE   = 0,
	RD_CLIENT = 1,
	RD_PACKET = 2,
} redirect_t;

typedef enum server_state_e {
	ss_dead    = 0,
	ss_loading = 1,
	ss_active  = 2,
} server_state_t;

typedef enum sv_delta_s {
	sv_packet_nodelta,
	sv_packet_delta,
} sv_delta_t;

enum
{
	SND_ANYPLAYER,
	SND_NOTHOST,
};