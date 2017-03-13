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
/// @brief client dll interface

#pragma once

#include "public/interface.h"

const char OGS_INTERFACE_VERSION[] = "OGSGameDLL001";

struct IClientDLL : public IBaseInterface
{
	virtual bool Init(CreateInterfaceFn afnEngineFactory) = 0;
	virtual void Shutdown() = 0;
	
	virtual void Frame(double time) = 0;
	
	//virtual void Init();
	virtual void HudInit();
	virtual void HudVidInit();
	
	virtual void UpdateClientData();
	virtual void HudRedraw(int intermission);
	
	virtual void MoveClient(struct playermove_s *ppmove);
	virtual void ClientMoveInit(struct playermove_s *ppmove);
	virtual char ClientTextureType(char *name);

	virtual void CreateMove(float frametime, struct usercmd_s *cmd, int active);
	
	virtual void ClearStates();
	virtual int  IsThirdPerson();
	virtual void GetCameraOffsets(float *ofs);
	virtual int GraphKeyDown();
	virtual struct kbutton_s *FindKey(const char *name);
	virtual void CAM_Think();
	virtual void IN_Accumulate();
	virtual void CalcRefdef(struct ref_params_s *pparams);
	virtual int AddEntity(int type, struct cl_entity_s *ent);
	virtual void CreateEntities();

	virtual void DrawNormalTriangles();
	virtual void DrawTransparentTriangles();
	virtual void StudioEvent(const struct mstudioevent_s *event, const struct cl_entity_s *entity);
	virtual void PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
	virtual void TxferLocalOverrides(struct entity_state_s *state, const struct clientdata_s *client);
	virtual void ProcessPlayerState(struct entity_state_s *dst, const struct entity_state_s *src);
	virtual void TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
	virtual void ReadDemoBuffer(int size, unsigned char *buffer);
	virtual int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
	virtual int GetHullBounds(int hullnumber, float *mins, float *maxs);

	virtual void VGui_ConsolePrint(const char *text);

	virtual int Key_Event(int down, int keynum, const char *pszCurrentBinding);
	virtual void TempEntUpdate(double ft, double ct, double grav, struct tempent_s **ppFreeTE, struct tempent_s **ppActiveTE, int (*addTEntity)(struct cl_entity_s *pEntity), void (*playTESound)(struct tempent_s *pTemp, float damp));
	virtual struct cl_entity_s *GetUserEntity(int index);
	virtual void VoiceStatus(int entindex, qboolean bTalking);
	virtual void DirectorMessage(int iSize, void *pbuf);
	virtual void ChatInputPosition(int *x, int *y);
};