/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#pragma once
#ifndef OGS_GAMEINFO_H
#define OGS_GAMEINFO_H

#include "maintypes.h"

/* <8eef1> ../engine/modinfo.h:7 */
typedef enum
{
	BOTH = 0,
	SINGLEPLAYER_ONLY,
	MULTIPLAYER_ONLY,
} MOD_GAMEPLAY_TYPE_E;

/* <8ef1b> ../engine/modinfo.h:10 */
typedef struct GameInfo_s // modinfo_s
{
	qboolean bIsMod;
	char szInfo[256];
	char szDL[256];
	char szHLVersion[32];
	int version;
	int size;
	qboolean svonly;
	qboolean cldll;
	qboolean secure;
	MOD_GAMEPLAY_TYPE_E type;
	int num_edicts;
	int clientDllCRC;
} GameInfo_t; // modinfo_t

class CGameInfo
{
public:
	CGameInfo();
	~CGameInfo();
	
	void LoadFromFile();
	
	void SetModKey(char *pkey, char *pvalue);
private:
	modinfo_t *mpPODInfo;
};

#endif // OGS_GAMEINFO_H