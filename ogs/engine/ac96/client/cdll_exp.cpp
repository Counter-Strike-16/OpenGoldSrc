#include <cstdarg>

#include "winheaders.h"

#include "quakedef.h"
#include "cl_main.h"
#include "cl_parse.h"
#include "client.h"
#include "cdll_exp.h"
#include "const.h"
#include "gl_draw.h"
#include "gl_rmain.h"
#include "gl_screen.h"
#include "r_studio.h"
#include "sound.h"
#include "vgui_int.h"
#include "vid.h"
#include "vgui2/text_draw.h"

int hudGetScreenInfo( SCREENINFO* pscrinfo )
{
	g_engdstAddrs.pfnGetScreenInfo( &pscrinfo );

	if( pscrinfo && pscrinfo->iSize == sizeof( SCREENINFO ) )
	{
		pscrinfo->iWidth = vid.width;
		pscrinfo->iHeight = vid.height;
		pscrinfo->iFlags = SCRINFO_SCREENFLASH;
		pscrinfo->iCharHeight = VGUI2_MessageFontInfo( pscrinfo->charWidths, VGUI2_GetCreditsFont() );
		return sizeof( SCREENINFO );
	}

	return 0;
}

cvar_t* hudRegisterVariable( char* szName, char* szValue, int flags )
{
	g_engdstAddrs.pfnRegisterVariable( &szName, &szValue, &flags );

	auto pCVar = reinterpret_cast<cvar_t*>( Z_Malloc( sizeof( cvar_t ) ) );
	pCVar->name = szName;
	pCVar->string = szValue;
	pCVar->flags = flags | FCVAR_CLIENTDLL;

	Cvar_RegisterVariable( pCVar );

	return pCVar;
}

float hudGetCvarFloat( char* szName )
{
	if( szName )
	{
		g_engdstAddrs.pfnGetCvarFloat( &szName );
		auto pCVar = Cvar_FindVar( szName );

		if( pCVar )
			return pCVar->value;
	}

	return 0;
}

char* hudGetCvarString( char* szName )
{
	if( szName )
	{
		g_engdstAddrs.pfnGetCvarString( &szName );

		auto pCVar = Cvar_FindVar( szName );

		if( pCVar )
			return pCVar->string;
	}

	//TODO: should return null, but crashes due to missing cvars - Solokiller
	return "";
}

int hudAddCommand( char* cmd_name, void( *function )( ) )
{
	g_engdstAddrs.pfnAddCommand( &cmd_name, &function );
	Cmd_AddHUDCommand( cmd_name, function );
	return true;
}

int hudHookUserMsg( char* szMsgName, pfnUserMsgHook pfn )
{
	g_engdstAddrs.pfnHookUserMsg( &szMsgName, &pfn );
	return HookServerMsg( szMsgName, pfn ) != nullptr;
}

int hudServerCmd( char* pszCmdString )
{
	char buf[ 2048 ];

	g_engdstAddrs.pfnServerCmd( &pszCmdString );

	snprintf( buf, ARRAYSIZE( buf ), "cmd %s", pszCmdString );
	Cmd_TokenizeString( buf );
	Cmd_ForwardToServer();

	return false;
}

int hudClientCmd( char* pszCmdString )
{
	g_engdstAddrs.pfnClientCmd( &pszCmdString );

	if( pszCmdString )
	{
		Cbuf_AddText( pszCmdString );
		Cbuf_AddText( "\n" );
		return true;
	}

	return false;
}

void hudGetPlayerInfo( int ent_num, hud_player_info_t* pinfo )
{
	g_engdstAddrs.pfnGetPlayerInfo( &ent_num, &pinfo );

	if( cl.maxclients >= ent_num && ent_num > 0 && 
		cl.players[ ent_num - 1 ].name[ 0 ] )
	{
		//Adjust to 0 based
		--ent_num;

		pinfo->name = cl.players[ ent_num ].name;
		pinfo->ping = cl.players[ ent_num ].ping;
		pinfo->spectator = cl.players[ ent_num ].spectator;
		pinfo->packetloss = cl.players[ ent_num ].packet_loss;
		pinfo->thisplayer = ent_num == cl.playernum;
		pinfo->topcolor = cl.players[ ent_num ].topcolor;
		pinfo->model = cl.players[ ent_num ].model;
		pinfo->bottomcolor = cl.players[ ent_num ].bottomcolor;

		if( g_bIsCStrike || g_bIsCZero )
		{
			pinfo->m_nSteamID = cl.players[ ent_num ].m_nSteamID;
		}
	}
	else
	{
		pinfo->name = nullptr;
		pinfo->thisplayer = false;
	}
}

void hudPlaySoundByName( char* szSound, float volume )
{
	g_engdstAddrs.pfnPlaySoundByName( &szSound, &volume );

	volume = clamp( volume, 0.0f, 1.0f );

	auto pSound = S_PrecacheSound( szSound );

	if( pSound )
		S_StartDynamicSound( cl.viewentity, CHAN_ITEM, pSound, r_origin, volume, 1.0, 0, PITCH_NORM );
	else
		Con_DPrintf( "invalid sound %s\n", szSound );
}

void hudPlaySoundByIndex( int iSound, float volume )
{
	g_engdstAddrs.pfnPlaySoundByIndex( &iSound, &volume );

	volume = clamp( volume, 0.0f, 1.0f );

	//TODO: shouldn't be <= MAX_SOUNDS, since there are only MAX_SOUNDS sounds - Solokiller
	if( 0 <= iSound && iSound <= MAX_SOUNDS )
	{
		S_StartDynamicSound( cl.viewentity, CHAN_ITEM, cl.sound_precache[ iSound ], r_origin, volume, 1.0, 0, PITCH_NORM );
	}
	else
	{
		Con_DPrintf( "invalid sound %i\n", iSound );
	}
}

void hudDrawConsoleStringLen( const char* string, int* width, int* height )
{
	g_engdstAddrs.pfnDrawConsoleStringLen( &string, &width, &height );

	*width = Draw_StringLen( string, VGUI2_GetConsoleFont() );
	*height = VGUI2_GetFontTall( VGUI2_GetConsoleFont() );
}

void hudConsolePrint( const char* string )
{
	g_engdstAddrs.pfnConsolePrint( &string );

	Con_Printf( "%s", string );
}

void hudCenterPrint( const char* string )
{
	g_engdstAddrs.pfnConsolePrint( &string );

	SCR_CenterPrint( string );
}

void hudCvar_SetValue( char* var_name, float value )
{
	Cvar_SetValue( var_name, value );
}

char* hudCmd_Argv( int arg )
{
	return const_cast<char*>( Cmd_Argv( arg ) );
}

void hudCon_Printf( char* fmt, ... )
{
	char buffer[ 1024 ];

	va_list va;

	va_start( va, fmt );
	vsnprintf( buffer, ARRAYSIZE( buffer ), fmt, va );
	va_end( va );

	Con_Printf( "%s", buffer );
}

void hudCon_DPrintf( char* fmt, ... )
{
	char buffer[ 1024 ];

	va_list va;

	va_start( va, fmt );
	vsnprintf( buffer, ARRAYSIZE( buffer ), fmt, va );
	va_end( va );

	Con_DPrintf( "%s", buffer );
}

void hudCon_NPrintf( int idx, char* fmt, ... )
{
	//TODO: new function to deal with const correctness issue,
	//see if we can get away with a function pointer cast - Solokiller
	char szBuf[ CON_MAX_NOTIFY_STRING ];

	va_list va;

	va_start( va, fmt );
	vsnprintf( szBuf, ARRAYSIZE( szBuf ), fmt, va );
	va_end( va );

	Con_NPrintf( idx, "%s", szBuf );
}

void hudCon_NXPrintf( con_nprint_t* info, char* fmt, ... )
{
	//TODO: new function to deal with const correctness issue,
	//see if we can get away with a function pointer cast - Solokiller
	char szBuf[ CON_MAX_NOTIFY_STRING ];

	va_list va;

	va_start( va, fmt );
	vsnprintf( szBuf, ARRAYSIZE( szBuf ), fmt, va );
	va_end( va );

	Con_NXPrintf( info, "%s", szBuf );
}

void hudKey_Event( int key, int down )
{
	Key_Event( key, down != 0 );
}

void hudPlaySoundByNameAtLocation( char* szSound, float volume, float* origin )
{
	g_engdstAddrs.pfnPlaySoundByNameAtLocation( &szSound, &volume, &origin );

	volume = clamp( volume, 0.0f, 1.0f );

	auto pSound = CL_LookupSound( szSound );

	if( pSound )
		S_StartDynamicSound( cl.viewentity, CHAN_AUTO, pSound, origin, volume, 1.0, 0, PITCH_NORM );
}

void* hudVGuiWrap_GetPanel()
{
	return VGuiWrap_GetPanel();
}

byte* hudCOM_LoadFile( char* path, int usehunk, int* pLength )
{
	return COM_LoadFile( path, usehunk, pLength );
}

int hudServerCmdUnreliable( char* pszCmdString )
{
	char buf[ 2048 ];

	g_engdstAddrs.pfnServerCmdUnreliable( &pszCmdString );

	snprintf( buf, ARRAYSIZE( buf ), "cmd %s", pszCmdString );
	Cmd_TokenizeString( buf );

	return Cmd_ForwardToServerUnreliable();
}

void* hudVguiWrap2_GetCareerUI()
{
	return VguiWrap2_GetCareerUI();
}

void hudCvar_Set( char* var_name, char* value )
{
	Cvar_Set( var_name, value );
}

int hudVGuiWrap2_IsInCareerMatch()
{
	return VGuiWrap2_IsInCareerMatch();
}

void hudPlaySoundVoiceByName( char* szSound, float volume, int pitch )
{
	g_engdstAddrs.pfnPlaySoundVoiceByName( &szSound, &volume );

	volume = clamp( volume, 0.0f, 1.0f );

	auto pSound = S_PrecacheSound( szSound );

	if( pSound )
	{
		pitch = max( 0, pitch );

		S_StartDynamicSound( cl.viewentity, CHAN_BOT, pSound, r_origin, volume, 1.0, 0, pitch );
	}
	else
	{
		Con_DPrintf( "invalid sound %s\n", szSound );
	}
}

void hudPlaySoundByNameAtPitch( char* szSound, float volume, int pitch )
{
	g_engdstAddrs.pfnPlaySoundByName( &szSound, &volume );

	volume = clamp( volume, 0.0f, 1.0f );

	auto pSound = S_PrecacheSound( szSound );

	if( pSound )
	{
		pitch = max( 0, pitch );

		S_StartDynamicSound( cl.viewentity, CHAN_ITEM, pSound, r_origin, volume, 1.0, 0, pitch );
	}
	else
	{
		Con_DPrintf( "invalid sound %s\n", szSound );
	}
}

int hudGetGameAppID()
{
	return GetGameAppID();
}

void hudGetClientOrigin( vec3_t origin )
{
	cl_entity_t* pViewEnt = &cl_entities[ cl.viewentity ];

	if( pViewEnt->index - 1 == cl.playernum )
	{
		origin[ 0 ] = cl.simorg[ 0 ];
		origin[ 1 ] = cl.simorg[ 1 ];
		origin[ 2 ] = cl.simorg[ 2 ];
	}
	else
	{
		origin[ 0 ] = pViewEnt->origin[ 0 ];
		origin[ 1 ] = pViewEnt->origin[ 1 ];
		origin[ 2 ] = pViewEnt->origin[ 2 ];
	}
}