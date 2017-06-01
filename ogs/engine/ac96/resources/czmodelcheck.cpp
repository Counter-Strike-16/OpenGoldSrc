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
#include "common/commontypes.h"
#include "public/archtypes.h"
#include "system/common.hpp"

qboolean IsCZPlayerModel(uint32 crc, const char *filename)
{
	if(crc == 0x27FB4D2F)
		return Q_stricmp(filename, "models/player/spetsnaz/spetsnaz.mdl") ? 0 : 1;

	if(crc == 0xEC43F76D || crc == 0x270FB2D7)
		return Q_stricmp(filename, "models/player/terror/terror.mdl") ? 0 : 1;

	if(crc == 0x1AAA3360 || crc == 0x35AC6FED)
		return Q_stricmp(filename, "models/player/gign/gign.mdl") ? 0 : 1;

	if(crc == 0x02B95E5F || crc == 0x72DB74E4)
		return Q_stricmp(filename, "models/player/vip/vip.mdl") ? 0 : 1;

	if(crc == 0x1F3CD80B || crc == 0x1B6C4115)
		return Q_stricmp(filename, "models/player/guerilla/guerilla.mdl") ? 0 : 1;

	if(crc == 0x3BCAA016)
		return Q_stricmp(filename, "models/player/militia/militia.mdl") ? 0 : 1;

	if(crc == 0x43E67FF3 || crc == 0xF141AE3F)
		return Q_stricmp(filename, "models/player/sas/sas.mdl") ? 0 : 1;

	if(crc == 0xDA8922A || crc == 0x56DD2D02)
		return Q_stricmp(filename, "models/player/gsg9/gsg9.mdl") ? 0 : 1;

	if(crc == 0xA37D8680 || crc == 0x4986827B)
		return Q_stricmp(filename, "models/player/arctic/arctic.mdl") ? 0 : 1;

	if(crc == 0xC37369F6 || crc == 0x29FE156C)
		return Q_stricmp(filename, "models/player/leet/leet.mdl") ? 0 : 1;

	if(crc == 0xC7F0DBF3 || crc == 0x068168DB)
		return Q_stricmp(filename, "models/player/urban/urban.mdl") ? 0 : 1;

	return 0;
}