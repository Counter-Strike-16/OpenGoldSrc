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

//Needed so POINT is defined as CPoint. - Solokiller
//TODO: change all references to POINT to CPoint, remove this header include. - Solokiller
#include "winheaders.h"

#include "engine/cdll_int.h"
#include "engine/eiface.h"
#include "Sequence.h"

#include "system/client.hpp"
#include "system/textmessage.hpp"

#include "physics/pmovetst.hpp"

#include "world/pr_cmds.hpp"

#include "network/net_api_int.hpp"

#include "graphics/qgl.hpp"
#include "graphics/r_efx.hpp"
#include "graphics/r_triangle.hpp"
#include "graphics/view.hpp"

#include "sound/cdaudio.hpp"
#include "sound/sound.hpp"

#include "voice/voice.hpp"

#include "ui/vgui_int.hpp"

#include "cl_demo.hpp"
#include "cl_draw.hpp"
#include "cl_parsefn.hpp"
#include "cl_spectator.hpp"
#include "cdll_exp.hpp"
#include "eventapi.hpp"
#include "vgui2/text_draw.h"

//TODo: implement functions and add here - Solokiller
cl_enginefunc_t cl_enginefuncs = 
{
	&SPR_Load,
	&SPR_Frames,
	&SPR_Height,
	&SPR_Width,
	&SPR_Set,
	&SPR_Draw,
	&SPR_DrawHoles,
	&SPR_DrawAdditive,
	&SPR_EnableScissor,
	&SPR_DisableScissor,
	&SPR_GetList,
	&Draw_FillRGBA,
	&hudGetScreenInfo,
	&SetCrosshair,
	&hudRegisterVariable,
	&hudGetCvarFloat,
	&hudGetCvarString,
	&hudAddCommand,
	&hudHookUserMsg,
	&hudServerCmd,
	&hudClientCmd,
	&hudGetPlayerInfo,
	&hudPlaySoundByName,
	&hudPlaySoundByIndex,
	&AngleVectors,
	&TextMessageGet,
	&TextMessageDrawCharacter,
	&Draw_String,
	&Draw_SetTextColor,
	&hudDrawConsoleStringLen,
	&hudConsolePrint,
	&hudCenterPrint,
	&hudCenterX,
	&hudCenterY,
	&hudGetViewAngles,
	&hudSetViewAngles,
	&hudGetMaxClients,
	&hudCvar_SetValue,
	&Cmd_Argc,
	&hudCmd_Argv,
	&hudCon_Printf,
	&hudCon_DPrintf,
	&hudCon_NPrintf,
	&hudCon_NXPrintf,
	&hudPhysInfo_ValueForKey,
	&hudServerInfo_ValueForKey,
	&hudGetClientMaxspeed,
	&hudCheckParm,
	&hudKey_Event,
	&hudGetMousePosition,
	&hudIsNoClipping,
	&hudGetLocalPlayer,
	&hudGetViewModel,
	&hudGetEntityByIndex,
	&hudGetClientTime,
	&V_CalcShake,
	&V_ApplyShake,
	&PM_PointContents,
	&PM_WaterEntity,
	&PM_TraceLine,
	&CL_LoadModel,
	&CL_CreateVisibleEntity,
	&hudGetSpritePointer,
	&hudPlaySoundByNameAtLocation,
	&hudPrecacheEvent,
	&hudPlaybackEvent,
	&hudWeaponAnim,
	&RandomFloat,
	&RandomLong,
	&CL_HookEvent,
	&Con_IsVisible,
	&hudGetGameDir,
	&Cvar_FindVar,
	&Key_NameForBinding,
	&hudGetLevelName,
	&hudGetScreenFade,
	&hudSetScreenFade,
	&hudVGuiWrap_GetPanel,
	&VGui_ViewportPaintBackground,
	&hudCOM_LoadFile,
	&COM_ParseFile,
	&COM_FreeFile,
	&tri,
	&efx,
	&eventapi,
	&demoapi,
	&netapi,
	&g_VoiceTweakAPI,
	&CL_IsSpectateOnly,
	&R_LoadMapSprite,
	&COM_AddAppDirectoryToSearchPath,
	&ClientDLL_ExpandFileName,
	&PlayerInfo_ValueForKey,
	&PlayerInfo_SetValueForKey,
	&GetPlayerUniqueID,
	&GetTrackerIDForPlayer,
	&GetPlayerForTrackerID,
	&hudServerCmdUnreliable,
	&SDL_GetMousePos,
	&SDL_SetMousePos,
	&SetMouseEnable,
	&GetFirstCVarPtr,
	&GetFirstCmdFunctionHandle,
	&GetNextCmdFunctionHandle,
	&GetCmdFunctionName,
	&hudGetClientOldTime,
	&hudGetServerGravityValue,
	&hudGetModelByIndex,
	&SetFilterMode,
	&SetFilterColor,
	&SetFilterBrightness,
	&SequenceGet,
	&SPR_DrawGeneric,
	&SequencePickSentence,
	&VGUI2_DrawStringClient,
	&VGUI2_DrawStringReverseClient,
	&LocalPlayerInfo_ValueForKey,
	&VGUI2_Draw_Character,
	&VGUI2_Draw_CharacterAdd,
	&COM_GetApproxWavePlayLength,
	&hudVguiWrap2_GetCareerUI,
	&hudCvar_Set,
	&hudVGuiWrap2_IsInCareerMatch,
	&hudPlaySoundVoiceByName,
	&PrimeMusicStream,
	&Sys_FloatTime,
	&ProcessTutorMessageDecayBuffer,
	&ConstructTutorMessageDecayBuffer,
	&ResetTutorMessageDecayData,
	&hudPlaySoundByNameAtPitch,
	&Draw_FillRGBABlend,
	&hudGetGameAppID,
	&GetAliasesList,
	&VguiWrap2_GetMouseDelta
};

cldll_func_t cl_funcs;

char g_szfullClientName[ 512 ];

bool fClientLoaded = false;

void ClientDLL_Init()
{
};

void ClientDLL_Shutdown()
{
};

void ClientDLL_ActivateMouse()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_ActivateMouse )
			cl_funcs.pIN_ActivateMouse();
	};
};

void ClientDLL_DeactivateMouse()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_DeactivateMouse )
			cl_funcs.pIN_DeactivateMouse();
	};
};

void ClientDLL_MouseEvent( int mstate )
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_MouseEvent )
			cl_funcs.pIN_MouseEvent( mstate );
	};
};

void ClientDLL_ClearStates()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_ClearStates )
			cl_funcs.pIN_ClearStates();
	};
};

int ClientDLL_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	if( !VGui_Key_Event( down, keynum, pszCurrentBinding ) )
		return false;

	if( !cl_funcs.pKeyEvent )
		return true;

	return cl_funcs.pKeyEvent( down, keynum, pszCurrentBinding ) != 0;
};

int hudCenterX()
{
	return 0;
};

int hudCenterY()
{
	return 0;
};

void hudGetViewAngles( float* va )
{
}

void hudSetViewAngles( float* va )
{
}

int hudGetMaxClients()
{
	return 0;
}

const char* hudPhysInfo_ValueForKey( const char* key )
{
	return "";
}

const char* hudServerInfo_ValueForKey( const char* key )
{
	return "";
}

float hudGetClientMaxspeed()
{
	return 0;
}

void hudGetMousePosition( int* mx, int* my )
{
}

int hudIsNoClipping()
{
	return false;
}

cl_entity_t* hudGetLocalPlayer()
{
	return nullptr;
}

cl_entity_t* hudGetViewModel()
{
	return nullptr;
}

cl_entity_t* hudGetEntityByIndex( int idx )
{
	return nullptr;
}

float hudGetClientTime()
{
	return 0;
}

model_t* CL_LoadModel( const char* modelname, int* index )
{
	return nullptr;
}

int CL_CreateVisibleEntity( int type, cl_entity_t* ent )
{
	return 0;
}

const model_t* hudGetSpritePointer( HSPRITE hSprite )
{
	return nullptr;
}

unsigned short hudPrecacheEvent( int type, const char* psz )
{
	return 0;
}

void hudPlaybackEvent( int flags, const edict_t* pInvoker, unsigned short eventindex, float delay,
					   float* origin, float* angles,
					   float fparam1, float fparam2,
					   int iparam1, int iparam2,
					   int bparam1, int bparam2 )
{
	//TODO: implement
}

void hudWeaponAnim( int iAnim, int body )
{
}

const char* hudGetGameDir()
{
	//g_engdstAddrs.pfnGetGameDirectory();
	return com_gamedir;
}

const char *hudGetLevelName()
{
	//g_engdstAddrs.pfnGetLevelName();

	if( cls.state < ca_connected )
		return "";

	return "";
	//return cl.levelname;
}

void hudGetScreenFade( screenfade_t* fade )
{
}

void hudSetScreenFade( screenfade_t* fade )
{
}

char* COM_ParseFile( char* data, char* token )
{
	//g_engdstAddrs.COM_ParseFile();
	char* pszResult = COM_Parse( data );
	Q_strcpy( token, com_token );
	return pszResult;
}

void COM_AddAppDirectoryToSearchPath( const char* pszBaseDir, const char* appName )
{
	//g_engdstAddrs.COM_AddAppDirectoryToSearchPath();
	COM_AddAppDirectory( pszBaseDir );
}

int ClientDLL_ExpandFileName( const char* fileName, char* nameOutBuffer, int nameOutBufferSize )
{
	return false;
}

const char* PlayerInfo_ValueForKey( int playerNum, const char* key )
{
	return "";
}

void PlayerInfo_SetValueForKey( const char* key, const char* value )
{
}

qboolean GetPlayerUniqueID( int iPlayer, char* playerID )
{
	return false;
}

int GetTrackerIDForPlayer( int playerSlot )
{
	//g_engdstAddrs.GetTrackerIDForPlayer();
	return 0;
}

int GetPlayerForTrackerID( int trackerID )
{
	//g_engdstAddrs.GetPlayerForTrackerID();
	return 0;
}

void SDL_GetMousePos( POINT* ppt )
{
}

void SDL_SetMousePos( int x, int y )
{
}

cvar_t* GetFirstCVarPtr()
{
	return cvar_vars;
}

unsigned int GetFirstCmdFunctionHandle()
{
	return reinterpret_cast<unsigned int>( Cmd_GetFirstCmd() );
}

unsigned int GetNextCmdFunctionHandle( unsigned int cmdhandle )
{
	return reinterpret_cast<unsigned int>( reinterpret_cast<cmd_function_t*>( cmdhandle )->next );
}

const char* GetCmdFunctionName( unsigned int cmdhandle )
{
	return reinterpret_cast<cmd_function_t*>( cmdhandle )->name;
}

float hudGetClientOldTime()
{
	return 0;
}

float hudGetServerGravityValue()
{
	return 0;
}

model_t* hudGetModelByIndex( const int index )
{
	return nullptr;
}

const char* LocalPlayerInfo_ValueForKey( const char* key )
{
	return "";
}

cmdalias_t* GetAliasesList()
{
	return Cmd_GetAliasesList();
}