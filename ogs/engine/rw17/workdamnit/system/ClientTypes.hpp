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
/// @brief client module types - constants, macros, typedefs and enums

#pragma once

/// Camera types
enum
{
	CAM_NONE = 0,
	CAM_TRACK
};

const int MAX_BEAMS = 64;
const int MAX_STYLESTRING = 32;
const int MAX_TEMP_ENTITIES = 64; ///< lightning bolts, etc
const int MAX_STATIC_ENTITIES = 128; ///< torches, etc
const int MAX_DLIGHTS = 32;
const int MAX_EFRAGS = 512; // 640

/// Maximum number of recorded demo files
const int MAX_DEMOS = 32;

/// Maximum length of demo file name
const int MAX_DEMONAME = 16;

/// Local client states
typedef enum cactive_e {
	ca_dedicated,     ///< Dedicated server mode, client is unused
	ca_disconnected,  ///< Not talking to a server
	ca_connecting,    ///< Sending request packets to the server
	ca_connected,     ///< Netchan_t established, waiting for svc_serverdata
	ca_uninitialized, ///< Not initialized yet
	ca_active,        ///< Game views should be displayed
} cactive_t;

/// Career states
typedef enum CareerStateType_e {
	CAREER_NONE = 0,
	CAREER_LOADING = 1,
	CAREER_PLAYING = 2,
} CareerStateType;