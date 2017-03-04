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
#include "client/clientdll.hpp"
#include "system/common.hpp"

static dllfunc_t cdll_exports[] =
{
{ "Initialize", (void **)&clgame.dllFuncs.pfnInitialize },
{ "HUD_VidInit", (void **)&clgame.dllFuncs.pfnVidInit },
{ "HUD_Init", (void **)&clgame.dllFuncs.pfnInit },
{ "HUD_Shutdown", (void **)&clgame.dllFuncs.pfnShutdown },
{ "HUD_Redraw", (void **)&clgame.dllFuncs.pfnRedraw },
{ "HUD_UpdateClientData", (void **)&clgame.dllFuncs.pfnUpdateClientData },
{ "HUD_Reset", (void **)&clgame.dllFuncs.pfnReset },
{ "HUD_PlayerMove", (void **)&clgame.dllFuncs.pfnPlayerMove },
{ "HUD_PlayerMoveInit", (void **)&clgame.dllFuncs.pfnPlayerMoveInit },
{ "HUD_PlayerMoveTexture", (void **)&clgame.dllFuncs.pfnPlayerMoveTexture },
{ "HUD_ConnectionlessPacket", (void **)&clgame.dllFuncs.pfnConnectionlessPacket },
{ "HUD_GetHullBounds", (void **)&clgame.dllFuncs.pfnGetHullBounds },
{ "HUD_Frame", (void **)&clgame.dllFuncs.pfnFrame },
{ "HUD_PostRunCmd", (void **)&clgame.dllFuncs.pfnPostRunCmd },
{ "HUD_Key_Event", (void **)&clgame.dllFuncs.pfnKey_Event },
{ "HUD_AddEntity", (void **)&clgame.dllFuncs.pfnAddEntity },
{ "HUD_CreateEntities", (void **)&clgame.dllFuncs.pfnCreateEntities },
{ "HUD_StudioEvent", (void **)&clgame.dllFuncs.pfnStudioEvent },
{ "HUD_TxferLocalOverrides", (void **)&clgame.dllFuncs.pfnTxferLocalOverrides },
{ "HUD_ProcessPlayerState", (void **)&clgame.dllFuncs.pfnProcessPlayerState },
{ "HUD_TxferPredictionData", (void **)&clgame.dllFuncs.pfnTxferPredictionData },
{ "HUD_TempEntUpdate", (void **)&clgame.dllFuncs.pfnTempEntUpdate },
{ "HUD_DrawNormalTriangles", (void **)&clgame.dllFuncs.pfnDrawNormalTriangles },
{ "HUD_DrawTransparentTriangles", (void **)&clgame.dllFuncs.pfnDrawTransparentTriangles },
{ "HUD_GetUserEntity", (void **)&clgame.dllFuncs.pfnGetUserEntity },
{ "Demo_ReadBuffer", (void **)&clgame.dllFuncs.pfnDemo_ReadBuffer },
{ "CAM_Think", (void **)&clgame.dllFuncs.CAM_Think },
{ "CL_IsThirdPerson", (void **)&clgame.dllFuncs.CL_IsThirdPerson },
{ "CL_CameraOffset", (void **)&clgame.dllFuncs.CL_CameraOffset },
{ "CL_CreateMove", (void **)&clgame.dllFuncs.CL_CreateMove },
{ "IN_ActivateMouse", (void **)&clgame.dllFuncs.IN_ActivateMouse },
{ "IN_DeactivateMouse", (void **)&clgame.dllFuncs.IN_DeactivateMouse },
{ "IN_MouseEvent", (void **)&clgame.dllFuncs.IN_MouseEvent },
{ "IN_Accumulate", (void **)&clgame.dllFuncs.IN_Accumulate },
{ "IN_ClearStates", (void **)&clgame.dllFuncs.IN_ClearStates },
{ "V_CalcRefdef", (void **)&clgame.dllFuncs.pfnCalcRefdef },
{ "KB_Find", (void **)&clgame.dllFuncs.KB_Find },
{ NULL, NULL }
};

// Client DLL Loader

bool ClientDLL_Load(const char *asPath)
{
	if(clgame.hInstance)
		ClientDLL_Unload();

	CL_EXPORT_FUNCS F; // export 'F'
	const dllfunc_t *func;

	clgame.hInstance = Com_LoadLibrary(asPath, false);
	if(!clgame.hInstance)
		return false;

	// clear exports
	for(func = cdll_exports; func && func->name; func++)
		*func->func = NULL;

	// trying to get single export named 'F'
	if((F = (void *)Com_GetProcAddress(clgame.hInstance, "F")) != NULL)
	{
		MsgDev(D_NOTE, "CL_LoadProgs: found single callback export\n");

		// trying to fill interface now
		F(&clgame.dllFuncs);

		// check critical functions again
		for(func = cdll_exports; func && func->name; func++)
		{
			if(func->func == NULL)
				break; // BAH critical function was missed
		}

		// because all the exports are loaded through function 'F"
		if(!func || !func->name)
			critical_exports = false;
	}

	return true;
};

bool ClientDLL_Reload()
{
	if(!ClientDLL_IsLoaded())
		return false;

	char sPrevPath[MAX_PATH];
	sPrevPath[0] = '\0';

	Q_strncpy(sPrevPath, ClientDLL_GetPath(), charsmax(MAX_PATH));

	ClientDLL_Unload();

	if(!ClientDLL_Load(sPrevPath))
		return false;

	return true;
};

void ClientDLL_Unload()
{
	if(!ClientDLL_IsLoaded())
		return;
	
	Com_FreeLibrary(clgame.hInstance);
};

bool ClientDLL_IsLoaded()
{
	if(!clgame.hInstance)
		return false;
	
	return true;
};