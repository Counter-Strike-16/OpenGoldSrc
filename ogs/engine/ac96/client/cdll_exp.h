#ifndef ENGINE_CDLL_EXP_H
#define ENGINE_CDLL_EXP_H

#include "screenfade.h"

//Replacement for POINT, which is a Windows type.
//TODO: find a better place for this. - Solokiller
struct CPoint
{
	int x;
	int y;
};

#ifndef _WINDOWS_
#define POINT CPoint
#endif

int hudGetScreenInfo( SCREENINFO* pscrinfo );

cvar_t* hudRegisterVariable( char* szName, char* szValue, int flags );

float hudGetCvarFloat( char* szName );

char* hudGetCvarString( char* szName );

int hudAddCommand( char* cmd_name, void ( *function )() );

int hudHookUserMsg( char* szMsgName, pfnUserMsgHook pfn );

int hudServerCmd( char* pszCmdString );

int hudClientCmd( char* pszCmdString );

void hudGetPlayerInfo( int ent_num, hud_player_info_t* pinfo );

void hudPlaySoundByName( char* szSound, float volume );

void hudPlaySoundByIndex( int iSound, float volume );

void hudDrawConsoleStringLen( const char* string, int* width, int* height );

void hudConsolePrint( const char* string );

void hudCenterPrint( const char* string );

void hudCvar_SetValue( char* var_name, float value );

char* hudCmd_Argv( int arg );

void hudCon_Printf( char* fmt, ... );

void hudCon_DPrintf( char* fmt, ... );

void hudCon_NPrintf( int idx, char* fmt, ... );

void hudCon_NXPrintf( con_nprint_t* info, char* fmt, ... );

void hudKey_Event( int key, int down );

void hudPlaySoundByNameAtLocation( char* szSound, float volume, float* origin );

void* hudVGuiWrap_GetPanel();

byte* hudCOM_LoadFile( char* path, int usehunk, int* pLength );

int hudServerCmdUnreliable( char* pszCmdString );

void* hudVguiWrap2_GetCareerUI();

void hudCvar_Set( char* var_name, char* value );

int hudVGuiWrap2_IsInCareerMatch();

void hudPlaySoundVoiceByName( char* szSound, float volume, int pitch );

void hudPlaySoundByNameAtPitch( char* szSound, float volume, int pitch );

int hudGetGameAppID();

//cdll_int.cpp

int hudCenterX();

int hudCenterY();

void hudGetViewAngles( float* va );

void hudSetViewAngles( float* va );

int hudGetMaxClients();

const char* hudPhysInfo_ValueForKey( const char* key );

const char* hudServerInfo_ValueForKey( const char* key );

float hudGetClientMaxspeed();

void hudGetMousePosition( int* mx, int* my );

int hudIsNoClipping();

cl_entity_t* hudGetLocalPlayer();

cl_entity_t* hudGetViewModel();

cl_entity_t* hudGetEntityByIndex( int idx );

float hudGetClientTime();

model_t* CL_LoadModel( const char* modelname, int* index );

int CL_CreateVisibleEntity( int type, cl_entity_t* ent );

const model_t* hudGetSpritePointer( HSPRITE hSprite );

unsigned short hudPrecacheEvent( int type, const char* psz );

void hudPlaybackEvent( int flags, const edict_t* pInvoker, unsigned short eventindex, float delay, 
					   float* origin, float* angles,
					   float fparam1, float fparam2, 
					   int iparam1, int iparam2, 
					   int bparam1, int bparam2 );

void hudWeaponAnim( int iAnim, int body );

const char* hudGetGameDir();

const char *hudGetLevelName();

void hudGetScreenFade( screenfade_t* fade );

void hudSetScreenFade( screenfade_t* fade );

char* COM_ParseFile( char* data, char* token );

void COM_AddAppDirectoryToSearchPath( const char* pszBaseDir, const char* appName );

int ClientDLL_ExpandFileName( const char* fileName, char* nameOutBuffer, int nameOutBufferSize );

const char* PlayerInfo_ValueForKey( int playerNum, const char* key );

void PlayerInfo_SetValueForKey( const char* key, const char* value );

qboolean GetPlayerUniqueID( int iPlayer, char* playerID );

int GetTrackerIDForPlayer( int playerSlot );

int GetPlayerForTrackerID( int trackerID );

void SDL_GetMousePos( POINT* ppt );

void SDL_SetMousePos( int x, int y );

cvar_t* GetFirstCVarPtr();

unsigned int GetFirstCmdFunctionHandle();

unsigned int GetNextCmdFunctionHandle( unsigned int cmdhandle );

const char* GetCmdFunctionName( unsigned int cmdhandle );

float hudGetClientOldTime();

float hudGetServerGravityValue();

model_t* hudGetModelByIndex( const int index );

const char* LocalPlayerInfo_ValueForKey( const char* key );

cmdalias_t* GetAliasesList();

void ClientDLL_DemoUpdateClientData( client_data_t* cdat );

#endif //ENGINE_CDLL_EXP_H
