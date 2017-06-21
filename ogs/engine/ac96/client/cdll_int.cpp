//Needed so POINT is defined as CPoint. - Solokiller
//TODO: change all references to POINT to CPoint, remove this header include. - Solokiller
#include "winheaders.h"

#include "quakedef.h"
#include "cdaudio.h"
#include "client.h"
#include "cl_demo.h"
#include "cl_draw.h"
#include "cl_main.h"
#include "cl_parse.h"
#include "cl_parsefn.h"
#include "cl_spectator.h"
#include "cdll_int.h"
#include "cdll_exp.h"
#include "eiface.h"
#include "eventapi.h"
#include "FilePaths.h"
#include "gl_draw.h"
#include "gl_screen.h"
#include "gl_vidnt.h"
#include "kbutton.h"
#include "LoadBlob.h"
#include "net_api_int.h"
#include "pmovetst.h"
#include "pr_cmds.h"
#include "qgl.h"
#include "r_efx.h"
#include "r_studio.h"
#include "r_triangle.h"
#include "Sequence.h"
#include "sound.h"
#include "sv_phys.h"
#include "sys_getmodes.h"
#include "tmessage.h"
#include "vgui_int.h"
#include "view.h"
#include "voice.h"
#include "vgui2/text_draw.h"

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

cldll_func_t cl_funcs = {};

cldll_func_dst_t g_cldstAddrs = k_cldstNull;

char g_szfullClientName[ 512 ];

bool fClientLoaded = false;

CSysModule* hClientDLL = nullptr;

BlobFootprint_t g_blobfootprintClient = {};

bool LoadSecureClient( const char* pszDllName )
{
	cl_funcs.pInitFunc = ( INITIALIZE_FUNC ) &g_modfuncs;
	cl_funcs.pHudVidInitFunc = ( HUD_VIDINIT_FUNC ) &g_cldstAddrs;

	auto pModule = reinterpret_cast<CSysModule*>( NLoadBlobFile( pszDllName, &g_blobfootprintClient, &cl_funcs, true ) );

	if( pModule )
	{
		hClientDLL = pModule;
		fClientLoaded = true;
	}

	return pModule != nullptr;
}

#define LOAD_IFACE_FUNC( func, pszName, bRequired )												\
	func = reinterpret_cast<decltype( func )>( Sys_GetProcAddress( hClientDLL, pszName ) );		\
																								\
	if( bRequired && !cl_funcs.pInitFunc )														\
		Sys_Error( "could not link client.dll function " pszName "\n" )

void LoadInsecureClient( const char* pszFullPathClientDLLName )
{
	hClientDLL = FS_LoadLibrary( pszFullPathClientDLLName );

	if( !hClientDLL )
		Sys_Error( "could not load library %s", pszFullPathClientDLLName );

	fClientLoaded = true;

	LOAD_IFACE_FUNC( cl_funcs.pInitFunc, "Initialize", true );
	LOAD_IFACE_FUNC( cl_funcs.pHudVidInitFunc, "HUD_VidInit", true );
	LOAD_IFACE_FUNC( cl_funcs.pHudInitFunc, "HUD_Init", true );
	LOAD_IFACE_FUNC( cl_funcs.pHudRedrawFunc, "HUD_Redraw", true );
	LOAD_IFACE_FUNC( cl_funcs.pHudUpdateClientDataFunc, "HUD_UpdateClientData", true );
	LOAD_IFACE_FUNC( cl_funcs.pHudResetFunc, "HUD_Reset", true );
	LOAD_IFACE_FUNC( cl_funcs.pClientMove, "HUD_PlayerMove", true );
	LOAD_IFACE_FUNC( cl_funcs.pClientMoveInit, "HUD_PlayerMoveInit", true );
	LOAD_IFACE_FUNC( cl_funcs.pClientTextureType, "HUD_PlayerMoveTexture", true );
	LOAD_IFACE_FUNC( cl_funcs.pIN_ActivateMouse, "IN_ActivateMouse", true );
	LOAD_IFACE_FUNC( cl_funcs.pIN_DeactivateMouse, "IN_DeactivateMouse", true );
	LOAD_IFACE_FUNC( cl_funcs.pIN_MouseEvent, "IN_MouseEvent", true );
	LOAD_IFACE_FUNC( cl_funcs.pIN_ClearStates, "IN_ClearStates", true );
	LOAD_IFACE_FUNC( cl_funcs.pIN_Accumulate, "IN_Accumulate", true );
	LOAD_IFACE_FUNC( cl_funcs.pCL_CreateMove, "CL_CreateMove", true );
	LOAD_IFACE_FUNC( cl_funcs.pCL_IsThirdPerson, "CL_IsThirdPerson", true );
	LOAD_IFACE_FUNC( cl_funcs.pCL_GetCameraOffsets, "CL_CameraOffset", true );
	LOAD_IFACE_FUNC( cl_funcs.pCamThink, "CAM_Think", true );
	LOAD_IFACE_FUNC( cl_funcs.pFindKey, "KB_Find", true );
	LOAD_IFACE_FUNC( cl_funcs.pCalcRefdef, "V_CalcRefdef", true );
	LOAD_IFACE_FUNC( cl_funcs.pAddEntity, "HUD_AddEntity", true );
	LOAD_IFACE_FUNC( cl_funcs.pCreateEntities, "HUD_CreateEntities", true );
	LOAD_IFACE_FUNC( cl_funcs.pDrawNormalTriangles, "HUD_DrawNormalTriangles", true );
	LOAD_IFACE_FUNC( cl_funcs.pDrawTransparentTriangles, "HUD_DrawTransparentTriangles", true );
	LOAD_IFACE_FUNC( cl_funcs.pStudioEvent, "HUD_StudioEvent", true );
	LOAD_IFACE_FUNC( cl_funcs.pShutdown, "HUD_Shutdown", true );
	LOAD_IFACE_FUNC( cl_funcs.pTxferLocalOverrides, "HUD_TxferLocalOverrides", true );
	LOAD_IFACE_FUNC( cl_funcs.pProcessPlayerState, "HUD_ProcessPlayerState", true );
	LOAD_IFACE_FUNC( cl_funcs.pTxferPredictionData, "HUD_TxferPredictionData", true );
	LOAD_IFACE_FUNC( cl_funcs.pReadDemoBuffer, "Demo_ReadBuffer", true );
	LOAD_IFACE_FUNC( cl_funcs.pConnectionlessPacket, "HUD_ConnectionlessPacket", true );
	LOAD_IFACE_FUNC( cl_funcs.pGetHullBounds, "HUD_GetHullBounds", true );
	LOAD_IFACE_FUNC( cl_funcs.pHudFrame, "HUD_Frame", true );
	LOAD_IFACE_FUNC( cl_funcs.pKeyEvent, "HUD_Key_Event", true );
	LOAD_IFACE_FUNC( cl_funcs.pPostRunCmd, "HUD_PostRunCmd", true );
	LOAD_IFACE_FUNC( cl_funcs.pTempEntUpdate, "HUD_TempEntUpdate", true );
	LOAD_IFACE_FUNC( cl_funcs.pGetUserEntity, "HUD_GetUserEntity", true );
	LOAD_IFACE_FUNC( cl_funcs.pVoiceStatus, "HUD_VoiceStatus", false );
	LOAD_IFACE_FUNC( cl_funcs.pDirectorMessage, "HUD_DirectorMessage", false );
	LOAD_IFACE_FUNC( cl_funcs.pStudioInterface, "HUD_GetStudioModelInterface", false );
	LOAD_IFACE_FUNC( cl_funcs.pChatInputPosition, "HUD_ChatInputPosition", false );
	LOAD_IFACE_FUNC( cl_funcs.pClientFactory, "ClientFactory", false );
	LOAD_IFACE_FUNC( cl_funcs.pGetPlayerTeam, "HUD_GetPlayerTeam", false );
}

void ClientDLL_Init()
{
	char szDllName[ 512 ];
	snprintf( szDllName, ARRAYSIZE( szDllName ), "cl_dlls/client" DEFAULT_SO_EXT );

	ClientDLL_Shutdown();

	COM_FixSlashes( szDllName );
	strcpy( g_szfullClientName, szDllName );
	COM_ExpandFilename( g_szfullClientName );
	COM_FixSlashes( g_szfullClientName );

	cls.fSecureClient = false;

	if( !LoadSecureClient( g_szfullClientName ) && !fClientLoaded )
	{
		LoadInsecureClient( g_szfullClientName );
	}

	HookServerMsg( "ScreenShake", &V_ScreenShake );
	HookServerMsg( "ScreenFade", &V_ScreenFade );

	//TODO: can't init the client yet because it'll crash due to missing engine functions - Solokiller
	/*
	cl_funcs.pInitFunc( &cl_enginefuncs, CLDLL_INTERFACE_VERSION );

	if( cl_funcs.pClientMoveInit )
		cl_funcs.pClientMoveInit( &g_clmove );

	CL_GetPlayerHulls();
	*/
}

void ClientDLL_Shutdown()
{
	Net_APIShutDown();
	SPR_Shutdown();

	if( cl_funcs.pShutdown )
		cl_funcs.pShutdown();

	if( cls.fSecureClient )
	{
		FreeBlob( &g_blobfootprintClient );
	}
	else
	{
		Sys_UnloadModule( hClientDLL );
		hClientDLL = nullptr;
	}

	Q_memset( &cl_funcs, 0, sizeof( cl_funcs ) );

	Cvar_RemoveHudCvars();
	Cmd_RemoveHudCmds();
	CL_ShutDownUsrMessages();

	fClientLoaded = false;

	if( hClientDLL )
		Sys_UnloadModule( hClientDLL );

	hClientDLL = nullptr;
}

//Obsolete function used to call blob dummy functions
int NCallDummy( int ijump, int cnArg, ... )
{
	return 0;
}

void ClientDLL_CheckStudioInterface( CSysModule* hClientDLL )
{
	R_ResetStudio();

	if( fClientLoaded )
	{
		//TODO: doesn't seem to actually be used - Solokiller
		Sys_GetProcAddress( hClientDLL, "HUD_GetStudioModelInterface" );

		if( cl_funcs.pStudioInterface )
		{
			if( !cl_funcs.pStudioInterface( STUDIO_INTERFACE_VERSION, &pStudioAPI, &engine_studio_api ) )
			{
				Con_DPrintf( "Couldn't get client .dll studio model rendering interface.  Version mismatch?\n" );
				R_ResetStudio();
			}
		}
	}
}

void ClientDLL_HudInit()
{
	if( !cl_funcs.pHudInitFunc )
		Sys_Error( "../engine/cdll_int.c, line %d: could not link client DLL for HUD initialization", __LINE__ );
	
	cl_funcs.pHudInitFunc();

	ClientDLL_CheckStudioInterface( hClientDLL );

	cl_righthand = Cvar_FindVar( "cl_righthand" );
}

void ClientDLL_HudVidInit()
{
	if( !cl_funcs.pHudVidInitFunc )
		Sys_Error( "../engine/cdll_int.c, line %d: could not link client DLL for HUD Vid initialization", __LINE__ );
	
	SPR_Init();

	cl_funcs.pHudVidInitFunc();
}

void ClientDLL_UpdateClientData()
{
	if( cls.state == ca_active && !cls.demoplayback )
	{
		client_data_t cdat;
		Q_memset( &cdat, 0, sizeof( cdat ) );

		cdat.viewangles[ 0 ] = cl.viewangles[ 0 ];
		cdat.viewangles[ 1 ] = cl.viewangles[ 1 ];
		cdat.viewangles[ 2 ] = cl.viewangles[ 2 ];

		auto pViewEnt = &cl_entities[ cl.viewentity ];

		cdat.origin[ 0 ] = pViewEnt->origin[ 0 ];
		cdat.origin[ 1 ] = pViewEnt->origin[ 1 ];
		cdat.origin[ 2 ] = pViewEnt->origin[ 2 ];

		cdat.fov = scr_fov_value;
		cdat.iWeaponBits = cl.weapons;

		client_data_t oldcdat;

		if( cls.demorecording )
		{
			memcpy( &oldcdat, &cdat, sizeof( oldcdat ) );
		}

		if( cl_funcs.pHudUpdateClientDataFunc )
		{
			if( cl_funcs.pHudUpdateClientDataFunc( &cdat, cl.time ) )
			{
				cl.viewangles[ 0 ] = cdat.viewangles[ 0 ];
				cl.viewangles[ 1 ] = cdat.viewangles[ 1 ];
				cl.viewangles[ 2 ] = cdat.viewangles[ 2 ];
				scr_fov_value = cdat.fov;
			}
		}

		if( cls.demorecording )
			CL_WriteDLLUpdate( &oldcdat );
	}
}

void ClientDLL_Frame( double time )
{
	if( fClientLoaded )
	{
		if( cl_funcs.pHudFrame )
			cl_funcs.pHudFrame( time );
	}
}

void ClientDLL_HudRedraw( int intermission )
{
	if( !VGuiWrap2_IsGameUIVisible() )
	{
		if( cl_funcs.pHudRedrawFunc )
		{
			cl_funcs.pHudRedrawFunc( cl.time, intermission );
		}
	}
}

void ClientDLL_MoveClient( playermove_t* ppmove )
{
	if( cl_funcs.pClientMove )
		cl_funcs.pClientMove( ppmove, false );
}

void ClientDLL_ClientMoveInit( playermove_t* ppmove )
{
	if( cl_funcs.pClientMoveInit )
		cl_funcs.pClientMoveInit( ppmove );
}

char ClientDLL_ClientTextureType( char* name )
{
	if( cl_funcs.pClientTextureType )
		return cl_funcs.pClientTextureType( name );

	return 0;
}

void ClientDLL_CreateMove( float frametime, usercmd_t* cmd, int active )
{
	if( cl_funcs.pCL_CreateMove )
		cl_funcs.pCL_CreateMove( frametime, cmd, active );
}

void ClientDLL_ActivateMouse()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_ActivateMouse )
			cl_funcs.pIN_ActivateMouse();
	}
}

void ClientDLL_DeactivateMouse()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_DeactivateMouse )
			cl_funcs.pIN_DeactivateMouse();
	}
}

void ClientDLL_MouseEvent( int mstate )
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_MouseEvent )
			cl_funcs.pIN_MouseEvent( mstate );
	}
}

void ClientDLL_ClearStates()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_ClearStates )
			cl_funcs.pIN_ClearStates();
	}
}

int ClientDLL_IsThirdPerson()
{
	if( cl_funcs.pCL_IsThirdPerson )
		return cl_funcs.pCL_IsThirdPerson();
	
	return 0;
}

void ClientDLL_GetCameraOffsets( float* ofs )
{
	if( cl_funcs.pCL_GetCameraOffsets )
		cl_funcs.pCL_GetCameraOffsets( ofs );
}

kbutton_t* ClientDLL_FindKey( const char* name )
{
	if( !cl_funcs.pFindKey )
		return nullptr;

	return cl_funcs.pFindKey( name );
}

void ClientDLL_CAM_Think()
{
	if( cls.state < 0 || cls.state > ca_disconnected )
	{
		if( cl_funcs.pCamThink )
			cl_funcs.pCamThink();
	}
}

void ClientDLL_IN_Accumulate()
{
	if( fClientLoaded )
	{
		if( cl_funcs.pIN_Accumulate )
			cl_funcs.pIN_Accumulate();
	}
}

void ClientDLL_CalcRefdef( ref_params_t* pparams )
{
	if( cl_funcs.pCalcRefdef )
		cl_funcs.pCalcRefdef( pparams );
}

void ClientDLL_CreateEntities()
{
	if( cl_funcs.pCreateEntities )
		cl_funcs.pCreateEntities();
}

void ClientDLL_DrawNormalTriangles()
{
	if( cl_funcs.pDrawNormalTriangles )
		cl_funcs.pDrawNormalTriangles();

	tri.RenderMode( kRenderNormal );
}

void ClientDLL_DrawTransparentTriangles()
{
	if( cl_funcs.pDrawTransparentTriangles )
		cl_funcs.pDrawTransparentTriangles();

	tri.RenderMode( kRenderNormal );
}

void ClientDLL_StudioEvent( const mstudioevent_t* event, const cl_entity_t* entity )
{
	if( cl_funcs.pStudioEvent )
		cl_funcs.pStudioEvent( event, entity );
}

void ClientDLL_PostRunCmd( local_state_t* from, local_state_t* to, usercmd_t* cmd, int runfuncs, double time, unsigned int random_seed )
{
	if( cl_funcs.pPostRunCmd )
	{
		cl_funcs.pPostRunCmd( from, to, cmd, runfuncs, time, random_seed );
	}
	else if( cl_lw.value )
	{
		Cvar_SetValue( "cl_lw", 0.0 );
	}
}

void ClientDLL_TxferLocalOverrides( entity_state_t* state, const clientdata_t* client )
{
	if( cl_funcs.pTxferLocalOverrides )
		cl_funcs.pTxferLocalOverrides( state, client );
}

void ClientDLL_ProcessPlayerState( entity_state_t* dst, const entity_state_t* src )
{
	if( cl_funcs.pProcessPlayerState )
		cl_funcs.pProcessPlayerState( dst, src );
}

void ClientDLL_TxferPredictionData( entity_state_t* ps, const entity_state_t* pps, clientdata_t* pcd, const clientdata_t* ppcd, weapon_data_t* wd, const weapon_data_t* pwd )
{
	if( cl_funcs.pTxferPredictionData )
		cl_funcs.pTxferPredictionData( ps, pps, pcd, ppcd, wd, pwd );
}

void ClientDLL_ReadDemoBuffer( int size, unsigned char* buffer )
{
	if( cl_funcs.pReadDemoBuffer )
		cl_funcs.pReadDemoBuffer( size, buffer );
}

int ClientDLL_ConnectionlessPacket( const netadr_t* net_from, const char* args, char* response_buffer, int* response_buffer_size )
{
	if( cl_funcs.pConnectionlessPacket )
		return cl_funcs.pConnectionlessPacket( net_from, args, response_buffer, response_buffer_size );

	return 0;
}

int ClientDLL_GetHullBounds( int hullnumber, float* mins, float* maxs )
{
	if( cl_funcs.pGetHullBounds )
		return cl_funcs.pGetHullBounds( hullnumber, mins, maxs );

	return 0;
}

int ClientDLL_Key_Event( int down, int keynum, const char* pszCurrentBinding )
{
	if( !VGui_Key_Event( down, keynum, pszCurrentBinding ) )
		return false;

	if( !cl_funcs.pKeyEvent )
		return true;

	return cl_funcs.pKeyEvent( down, keynum, pszCurrentBinding ) != 0;
}

void ClientDLL_TempEntUpdate( double ft, double ct, double grav, TEMPENTITY** ppFreeTE, TEMPENTITY** ppActiveTE, int( *addTEntity )( cl_entity_t* pEntity ), void( *playTESound )( TEMPENTITY* pTemp, float damp ) )
{
	if( cl_funcs.pTempEntUpdate )
		cl_funcs.pTempEntUpdate( ft, ct, grav, ppFreeTE, ppActiveTE, addTEntity, playTESound );
}

cl_entity_t* ClientDLL_GetUserEntity( int index )
{
	if( cl_funcs.pGetUserEntity )
		return cl_funcs.pGetUserEntity( index );

		return nullptr;
}

void ClientDLL_VoiceStatus( int entindex, qboolean bTalking )
{
	if( cl_funcs.pVoiceStatus )
		cl_funcs.pVoiceStatus( entindex, bTalking );
}

void ClientDLL_DirectorMessage( int iSize, void* pbuf )
{
	if( cl_funcs.pDirectorMessage )
		cl_funcs.pDirectorMessage( iSize, pbuf );
}

void ClientDLL_ChatInputPosition( int* x, int* y )
{
	if( cl_funcs.pChatInputPosition )
		cl_funcs.pChatInputPosition( x, y );
}

int hudCenterX()
{
	g_engdstAddrs.GetWindowCenterX();

	return window_center_x;
}

int hudCenterY()
{
	g_engdstAddrs.GetWindowCenterY();

	return window_center_y;
}

void hudGetViewAngles( float* va )
{
	g_engdstAddrs.GetViewAngles( &va );

	va[ 0 ] = cl.viewangles[ 0 ];
	va[ 1 ] = cl.viewangles[ 1 ];
	va[ 2 ] = cl.viewangles[ 2 ];
}

void hudSetViewAngles( float* va )
{
	g_engdstAddrs.SetViewAngles( &va );

	cl.viewangles[ 0 ] = va[ 0 ];
	cl.viewangles[ 1 ] = va[ 1 ];
	cl.viewangles[ 2 ] = va[ 2 ];
}

int hudGetMaxClients()
{
	g_engdstAddrs.GetMaxClients();

	return cl.maxclients;
}

const char* hudPhysInfo_ValueForKey( const char* key )
{
	g_engdstAddrs.PhysInfo_ValueForKey( &key );

	return Info_ValueForKey( cls.physinfo, key );
}

const char* hudServerInfo_ValueForKey( const char* key )
{
	g_engdstAddrs.PhysInfo_ValueForKey( &key );

	return Info_ValueForKey( cls.physinfo, key );
}

float hudGetClientMaxspeed()
{
	g_engdstAddrs.GetClientMaxspeed();

	return cl.maxspeed;
}

void hudGetMousePosition( int* mx, int* my )
{
	g_engdstAddrs.GetMousePosition( &mx, &my );

	SDL_GetMouseState( mx, my );

	if( !VideoMode_IsWindowed() )
	{
		int wW, wT;
		SDL_GetWindowSize( pmainwindow, &wW, &wT );

		int vW, vT;
		VideoMode_GetCurrentVideoMode( &vW, &vT, nullptr );

		*mx = static_cast<int>( *mx * ( static_cast<double>( vW ) / wW ) );
		*my = static_cast<int>( *my * ( static_cast<double>( vT ) / wT ) );

		*mx = static_cast<int>( *mx + static_cast<double>( *mx - vW / 2 ) * ( GetXMouseAspectRatioAdjustment() - 1.0 ) );
		*my = static_cast<int>( *my + static_cast<double>( *my - vT / 2 ) * ( GetYMouseAspectRatioAdjustment() - 1.0 ) );
	}
}

int hudIsNoClipping()
{
	g_engdstAddrs.IsNoClipping();

	return cl.frames[ cl.parsecountmod ].playerstate[ cl.playernum ].movetype == MOVETYPE_NOCLIP;
}

cl_entity_t* hudGetLocalPlayer()
{
	g_engdstAddrs.GetLocalPlayer();

	//TODO: if cl_entities is null this returns garbage - Solokiller
	return &cl_entities[ cl.playernum + 1 ];
}

cl_entity_t* hudGetViewModel()
{
	g_engdstAddrs.GetViewModel();

	return &cl.viewent;
}

cl_entity_t* hudGetEntityByIndex( int idx )
{
	g_engdstAddrs.GetEntityByIndex( &idx );

	if( idx >= 0 && idx < cl.max_edicts )
		return &cl_entities[ idx ];

	return nullptr;
}

float hudGetClientTime()
{
	g_engdstAddrs.GetClientTime();

	return cl.time;
}

model_t* CL_LoadModel( const char* modelname, int* index )
{
	//TODO: implement - Solokiller
	return nullptr;
}

int CL_CreateVisibleEntity( int type, cl_entity_t* ent )
{
	//TODO: implement - Solokiller
	return 0;
}

const model_t* hudGetSpritePointer( HSPRITE hSprite )
{
	g_engdstAddrs.GetSpritePointer( &hSprite );

	return SPR_GetModelPointer( hSprite );
}

unsigned short hudPrecacheEvent( int type, const char* psz )
{
	g_engdstAddrs.pfnPrecacheEvent( &type, &psz );

	for( int i = 1; cl.event_precache[ i ].filename; ++i )
	{
		if( !Q_stricmp( cl.event_precache[ i ].filename, psz ) )
			return i;
	}

	return 0;
}

void hudPlaybackEvent( int flags, const edict_t* pInvoker, unsigned short eventindex, float delay,
					   float* origin, float* angles,
					   float fparam1, float fparam2,
					   int iparam1, int iparam2,
					   int bparam1, int bparam2 )
{
	g_engdstAddrs.pfnPlaybackEvent(
		&flags, &pInvoker, &eventindex, &delay,
		&origin, &angles,
		&fparam1, &fparam2,
		&iparam1, &iparam2,
		&bparam1, &bparam2
	);

	if( !( flags & FEV_SERVER ) )
	{
		event_args_t eargs;
		Q_memset( &eargs, 0, sizeof( eargs ) );

		eargs.origin[ 0 ] = origin[ 0 ];
		eargs.origin[ 1 ] = origin[ 1 ];
		eargs.origin[ 2 ] = origin[ 2 ];

		eargs.angles[ 0 ] = angles[ 0 ];
		eargs.angles[ 1 ] = angles[ 1 ];
		eargs.angles[ 2 ] = angles[ 2 ];

		eargs.velocity[ 0 ] = cl.simvel[ 0 ];
		eargs.velocity[ 1 ] = cl.simvel[ 1 ];
		eargs.velocity[ 2 ] = cl.simvel[ 2 ];
		eargs.fparam1 = fparam1;
		eargs.fparam2 = fparam2;
		eargs.iparam1 = iparam1;
		eargs.iparam2 = iparam2;
		eargs.bparam1 = bparam1;
		eargs.bparam2 = bparam2;

		eargs.entindex = cl.playernum + 1;
		//TODO: define constant - Solokiller
		eargs.ducking = cl.usehull == 1;

		CL_QueueEvent( flags, eventindex, delay, &eargs );

		if( cls.demorecording )
			CL_DemoEvent( flags, eventindex, delay, &eargs );
	}
}

void hudWeaponAnim( int iAnim, int body )
{
	g_engdstAddrs.pfnWeaponAnim( &iAnim, &body );

	cl.weaponstarttime = 0;
	cl.weaponsequence = iAnim;
	cl.viewent.curstate.body = body;

	if( cls.demorecording )
		CL_DemoAnim( iAnim, body );
}

const char* hudGetGameDir()
{
	g_engdstAddrs.pfnGetGameDirectory();

	return com_gamedir;
}

const char *hudGetLevelName()
{
	g_engdstAddrs.pfnGetLevelName();

	if( cls.state < ca_connected )
		return "";

	return cl.levelname;
}

void hudGetScreenFade( screenfade_t* fade )
{
	g_engdstAddrs.pfnGetScreenFade( &fade );

	*fade = cl.sf;
}

void hudSetScreenFade( screenfade_t* fade )
{
	g_engdstAddrs.pfnSetScreenFade( &fade );

	cl.sf = *fade;
}

char* COM_ParseFile( char* data, char* token )
{
	g_engdstAddrs.COM_ParseFile( &data, &token );

	char* pszResult = COM_Parse( data );
	Q_strcpy( token, com_token );

	return pszResult;
}

void COM_AddAppDirectoryToSearchPath( const char* pszBaseDir, const char* appName )
{
	g_engdstAddrs.COM_AddAppDirectoryToSearchPath( &pszBaseDir, &appName );

	COM_AddAppDirectory( pszBaseDir );
}

int ClientDLL_ExpandFileName( const char* fileName, char* nameOutBuffer, int nameOutBufferSize )
{
	g_engdstAddrs.COM_ExpandFilename( &fileName, &nameOutBuffer, &nameOutBufferSize );

	char buf[ 512 ];
	Q_strncpy( buf, fileName, ARRAYSIZE( buf ) );

	if( COM_ExpandFilename( buf ) )
	{
		COM_FixSlashes( buf );
		Q_strncpy( nameOutBuffer, buf, nameOutBufferSize );
		nameOutBuffer[ nameOutBufferSize - 1 ] = '\0';
		return true;
	}

	return false;
}

const char* PlayerInfo_ValueForKey( int playerNum, const char* key )
{
	g_engdstAddrs.PlayerInfo_ValueForKey( &playerNum, &key );

	if( cl.maxclients >= playerNum && playerNum > 0 )
	{
		if( cl.players[ playerNum - 1 ].name[ 0 ] )
			return Info_ValueForKey( cl.players[ playerNum - 1 ].userinfo, key );
	}

	return nullptr;
}

void PlayerInfo_SetValueForKey( const char* key, const char* value )
{
	if( strcmp( Info_ValueForKey( cls.userinfo, key ), value ) )
	{
		g_engdstAddrs.PlayerInfo_SetValueForKey( &key, &value );

		Info_SetValueForStarKey( cls.userinfo, key, value, MAX_INFO_STRING );

		if( cls.state > ca_connecting )
		{
			//TODO: implement - Solokiller
			/*
			MSG_WriteByte( &cls.netchan.message, 3 );
			SZ_Print( &cls.netchan.message, va( "setinfo \"%s\" \"%s\"\n", key, value ) );
			*/
		}
	}
}

qboolean GetPlayerUniqueID( int iPlayer, char* playerID )
{
	g_engdstAddrs.GetPlayerUniqueID( &iPlayer, &playerID );

	if( iPlayer >= 0 && iPlayer < cl.maxclients )
	{
		if( cl.players[ iPlayer ].userinfo[ 0 ] && cl.players[ iPlayer ].name[ 0 ] )
		{
			strncpy( playerID, cl.players[ iPlayer ].hashedcdkey, 16 );
			return true;
		}
	}

	return false;
}

int GetTrackerIDForPlayer( int playerSlot )
{
	g_engdstAddrs.GetTrackerIDForPlayer( &playerSlot );

	return 0;
}

int GetPlayerForTrackerID( int trackerID )
{
	g_engdstAddrs.GetPlayerForTrackerID( &trackerID );

	return 0;
}

void SDL_GetMousePos( POINT* ppt )
{
	g_engdstAddrs.pfnGetMousePos( &ppt );

	//TODO: this code is all over the place, consider refactoring - Solokiller

	int x, y;

	SDL_GetMouseState( &x, &y );

	if( !VideoMode_IsWindowed() )
	{
		int wW, wT;
		SDL_GetWindowSize( pmainwindow, &wW, &wT );

		int vW, vT;
		VideoMode_GetCurrentVideoMode( &vW, &vT, nullptr );

		x = static_cast<int>( x * ( static_cast<double>( vW ) / wW ) );
		y = static_cast<int>( y * ( static_cast<double>( vT ) / wT ) );

		x = static_cast<int>( x + static_cast<double>( x - vW / 2 ) * ( GetXMouseAspectRatioAdjustment() - 1.0 ) );
		y = static_cast<int>( y + static_cast<double>( y - vT / 2 ) * ( GetYMouseAspectRatioAdjustment() - 1.0 ) );
	}

	ppt->x = x;
	ppt->y = y;
}

void SDL_SetMousePos( int x, int y )
{
	SDL_WarpMouseInWindow( pmainwindow, x, y );
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
	return cl.oldtime;
}

float hudGetServerGravityValue()
{
	return sv_gravity.value;
}

model_t* hudGetModelByIndex( const int index )
{
	return CL_GetModelByIndex( index );
}

const char* LocalPlayerInfo_ValueForKey( const char* key )
{
	g_engdstAddrs.LocalPlayerInfo_ValueForKey( &key );

	return Info_ValueForKey( cls.userinfo, key );
}

cmdalias_t* GetAliasesList()
{
	return Cmd_GetAliasesList();
}

void ClientDLL_DemoUpdateClientData( client_data_t* cdat )
{
	if( cl_funcs.pHudUpdateClientDataFunc )
	{
		if( cl_funcs.pHudUpdateClientDataFunc( cdat, cl.time ) )
		{
			if( !cls.spectator )
			{
				cl.viewangles[ 0 ] = cdat->viewangles[ 0 ];
				cl.viewangles[ 1 ] = cdat->viewangles[ 1 ];
				cl.viewangles[ 2 ] = cdat->viewangles[ 2 ];

				scr_fov_value = cdat->fov;
			}
		}
	}
}