#include "quakedef.h"
#include "cl_main.h"
#include "client.h"
#include "gl_draw.h"
#include "gl_screen.h"
#include "gl_vidnt.h"
#include "glHud.h"
#include "host.h"
#include "HUD.h"
#include "net_chan.h"
#include "qgl.h"
#include "sound.h"
#include "vgui_int.h"
#include "vid.h"
#include "view.h"
#include "wad.h"
#include "vgui2/text_draw.h"

#define SCR_CENTERSTRING_MAX 40

viddef_t	vid;				// global video state

//TODO: check which ones are boolean - Solokiller
int clearnotify = 0;
int scr_center_lines = 0;
int scr_erase_lines = 0;
int scr_erase_center = 0;
int scr_fullupdate = 0;

float scr_centertime_off = 0;
float scr_centertime_start = 0;

char scr_centerstring[ 1024 ] = {};

float scr_con_current = 0;
float scr_fov_value = 90;

bool scr_copytop = false;
bool scr_copyeverything = false;
bool scr_initialized = false;
bool scr_drawloading = false;

int glx = 0;
int gly = 0;
int glwidth = 0;
int glheight = 0;

qpic_t* scr_paused = nullptr;

vrect_t scr_vrect = {};

cvar_t scr_viewsize = { "viewsize", "120", FCVAR_ARCHIVE };
cvar_t scr_showpause = { "showpause", "1" };

cvar_t scr_centertime = { "scr_centertime", "2" };
cvar_t scr_printspeed = { "scr_printspeed", "8" };
cvar_t scr_graphheight = { "graphheight", "64.0" };

cvar_t scr_connectmsg = { "scr_connectmsg", "0" };
cvar_t scr_connectmsg1 = { "scr_connectmsg1", "0" };
cvar_t scr_connectmsg2 = { "scr_connectmsg2", "0" };

void SCR_SizeUp_f();
void SCR_SizeDown_f();

void SCR_Init()
{
	Cvar_RegisterVariable( &scr_viewsize );
	Cvar_RegisterVariable( &scr_showpause );

	Cvar_RegisterVariable( &scr_centertime );
	Cvar_RegisterVariable( &scr_printspeed );
	Cvar_RegisterVariable( &scr_graphheight );

	Cvar_RegisterVariable( &scr_connectmsg );
	Cvar_RegisterVariable( &scr_connectmsg1 );
	Cvar_RegisterVariable( &scr_connectmsg2 );

	//TODO: implement - Solokiller
	Cmd_AddCommand( "sizeup", SCR_SizeUp_f );
	Cmd_AddCommand( "sizedown", SCR_SizeDown_f );
	//TODO: implement - Solokiller
	scr_initialized = true;
}

void GL_BreakOnError()
{
	//Nothing
	//TODO: check if Windows has this - Solokiller
}

void SCR_DrawConsole()
{
	Con_DrawNotify();
}

void SCR_DrawCenterString()
{
	int		l;
	int		j;
	int		x, y;
	int		remaining;

	// the finale prints the characters one at a time
	if( cl.intermission )
	{
		if( VGuiWrap2_IsInCareerMatch() )
			return;

		remaining = scr_printspeed.value * ( cl.time - scr_centertime_start );
	}
	else
		remaining = 9999;

	Draw_GetDefaultColor();

	scr_erase_center = 0;
	const char* start = scr_centerstring;

	if( scr_center_lines <= 4 )
		y = glheight*0.35;
	else
		y = 48;

	char line[ SCR_CENTERSTRING_MAX * 2 ];

	do
	{
		// scan the width of the line
		for( l = 0; l < SCR_CENTERSTRING_MAX; ++l )
		{
			if( start[ l ] == '\n' || !start[ l ] )
				break;
		}

		Q_strncpy( line, start, l );
		line[ l ] = '\0';

		x = ( glwidth - Draw_StringLen( line, VGUI2_GetConsoleFont() ) ) / 2;

		for( j = 0; j < l; ++j )
		{
			x += Draw_Character( x, y, start[ j ], VGUI2_GetConsoleFont() );
			if( !remaining-- )
				return;
		}

		y += 12;

		while( *start && *start != '\n' )
			start++;

		if( !*start )
			break;
		start++;		// skip the \n
	}
	while( 1 );
}

void SCR_CheckDrawCenterString()
{
	scr_copytop = true;

	if( scr_center_lines > scr_erase_lines )
		scr_erase_lines = scr_center_lines;

	scr_centertime_off -= host_frametime;

	if( scr_centertime_off <= 0 && !cl.intermission )
		return;

	if( key_dest != key_game )
		return;

	SCR_DrawCenterString();
}

void SCR_CenterPrint( const char* str )
{
	Q_strncpy( scr_centerstring, str, ARRAYSIZE( scr_centerstring ) - 1 );
	scr_centerstring[ ARRAYSIZE( scr_centerstring ) - 1 ] = '\0';
	scr_center_lines = 1;
	scr_centertime_off = scr_centertime.value;
	scr_centertime_start = cl.time;

	for( auto pszStr = str; *pszStr; ++pszStr )
	{
		if( *pszStr == '\n' )
			++scr_center_lines;
	}
}

void D_FillRect( vrect_t* r, byte* color )
{
	qglDisable( GL_TEXTURE_2D );
	qglEnable( GL_BLEND );
	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	qglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	qglEnable( GL_ALPHA_TEST );

	qglColor4f( color[ 0 ] / 255.0, color[ 1 ] / 255.0, color[ 2 ] / 255.0, 1.0 );

	qglDisable( GL_DEPTH_TEST );

	qglBegin( GL_QUADS );
		qglVertex2f( r->x, r->y );
		qglVertex2f( r->x + r->width, r->y );
		qglVertex2f( r->x + r->width, r->y + r->height );
		qglVertex2f( r->x, r->y + r->height );
	qglEnd();

	qglDisable( GL_ALPHA_TEST );
	qglEnable( GL_DEPTH_TEST );
	qglColor3f( 1.0, 1.0, 1.0 );
	qglEnable( GL_TEXTURE_2D );
	qglDisable( GL_BLEND );
}

void SCR_ConnectMsg()
{
	if( *scr_connectmsg.string == '0' )
		return;

	int wid[ 3 ];
	wid[ 0 ] = Draw_StringLen( scr_connectmsg.string, VGUI2_GetConsoleFont() );
	wid[ 1 ] = Draw_StringLen( scr_connectmsg1.string, VGUI2_GetConsoleFont() );
	int iWidth = Draw_StringLen( scr_connectmsg2.string, VGUI2_GetConsoleFont() );

	if( scr_vrect.width > 270 )
	{
		auto iWidest = -1;

		if( wid[ 0 ] >= 0 )
			iWidest = wid[ 0 ];

		if( iWidest < wid[ 1 ] )
			iWidest = wid[ 1 ];

		if( iWidth < iWidest )
			iWidth = iWidest;

		if( iWidth == -1 )
			return;

		if( iWidth >= scr_vrect.width - 1 )
			iWidth = scr_vrect.width - 2;
	}
	else
	{
		iWidth = scr_vrect.width - 2;
	}

	const int iLineHeight = ( scr_vrect.height <= 14 ) ? ( scr_vrect.height - 2 ) : 14;
	const int iYStart = iLineHeight / -2;

	int v10 = scr_vrect.x + ( scr_vrect.width - iWidth ) / 2;
			
	int iXPos = v10 + 1;
	int iYPos = iYStart + ( -5 * iLineHeight ) + scr_vrect.height;

	vrect_t rcFill;
	rcFill.x = v10 - 4;
	rcFill.y = iYPos + iYStart;
	rcFill.width = iWidth + 10;
	rcFill.height = 4 * iLineHeight;

	byte color[ 3 ] = { 0, 0, 0 };

	D_FillRect( &rcFill, color );

	//TODO: shouldn't these be in the range 0..1? - Solokiller
	Draw_SetTextColor( 192.0, 192.0, 192.0 );
	Draw_String( iXPos, iYPos, scr_connectmsg.string );

	if( *scr_connectmsg1.string != '0' )
	{
		iYPos += iLineHeight;
		Draw_SetTextColor( 192.0, 192.0, 192.0 );
		Draw_String( iXPos, iYPos, scr_connectmsg1.string );
	}

	if( *scr_connectmsg2.string != '0' )
	{
		iYPos += iLineHeight;
		Draw_SetTextColor( 192.0, 192.0, 192.0 );
		Draw_String( iXPos, iYPos, scr_connectmsg2.string );
	}
}

void SCR_DrawDownloadText()
{
	if( cls.state != ca_active && Q_strlen( gDownloadFile ) > 0 )
	{
		char szStatusText[ 128 ];
		snprintf( szStatusText, ARRAYSIZE( szStatusText ), "Downloading %s", gDownloadFile );

		const auto iWidth = Draw_StringLen( szStatusText, VGUI2_GetConsoleFont() );

		vrect_t rcFill;
		rcFill.x = ( vid.width - iWidth ) / 2;
		rcFill.y = max( 22, static_cast<int>( vid.height ) - 40 );
		rcFill.width = iWidth + 2;
		rcFill.height = vid.height <= 10 ? vid.height - 2 : 14;

		byte color[ 3 ] = { 0, 0, 0 };
		D_FillRect( &rcFill, color );

		Draw_SetTextColor( 0.9, 0.9, 0.9 );
		Draw_String( rcFill.x + 1, rcFill.y, szStatusText );
	}
}

void Draw_CenterPic( qpic_t* pPic )
{
	Draw_Pic( glwidth / 2 - pPic->width / 2, glheight / 2 - pPic->height / 2, pPic );
}

void SCR_DrawLoading()
{
	if( scr_drawloading )
		Draw_BeginDisc();
}

void SCR_DrawPause()
{
	if( scr_showpause.value != 0.0 && cl.paused && !VGuiWrap2_IsGameUIVisible() )
		Draw_Pic( glwidth / 2 - scr_paused->width / 2, glheight / 2 - scr_paused->height / 2, scr_paused );
}

void SCR_SetUpToDrawConsole()
{
	Con_CheckResize();
}

void SCR_UpdateScreen()
{
	static bool recursionGuard = false;

	if( recursionGuard )
		return;

	recursionGuard = true;

	V_CheckGamma();

	if( !gfBackground && !scr_skipupdate )
	{
		if( scr_skiponeupdate )
		{
			scr_skiponeupdate = false;
		}
		else
		{
			scr_copytop = false;
			scr_copyeverything = false;

			if( cls.state != ca_dedicated && scr_initialized && con_initialized )
			{
				double fCurrentTime = Sys_FloatTime();

				if( ( cls.state == ca_connecting ||
					cls.state == ca_connected ||
					cls.state == ca_active ) &&
					g_LastScreenUpdateTime > 0 && 
					( fCurrentTime - g_LastScreenUpdateTime ) > 120 )
				{
					Con_Printf( "load failed.\n" );
					//TODO: implement - Solokiller
					/*
					COM_ExplainDisconnection( true, "Connection to server lost during level change." );
					CL_Disconnect();
					*/
				}

				if( g_modfuncs.m_pfnFrameRender1 )
					g_modfuncs.m_pfnFrameRender1();

				//TODO: implement - Solokiller

				GL_BeginRendering( &glx, &gly, &glwidth, &glheight );

				//TODO: implement - Solokiller

				SCR_SetUpToDrawConsole();

				GLBeginHud();

				//Most of these cases are now obsolete,
				//but keep them to maintain the exclusion logic for the working cases - Solokiller
				if( scr_drawloading )
				{
					//Nothing
				}
				else if( cl.intermission == 1 && key_dest == key_game )
				{
					//Nothing
				}
				else if( cl.intermission == 2 && key_dest == key_game )
				{
					SCR_CheckDrawCenterString();
				}
				else if(  cl.intermission == 3 && key_dest == key_game )
				{
					SCR_CheckDrawCenterString();
				}
				else
				{
					//TODO: implement - Solokiller
					/*
					GL_Bind( r_notexture_mip->gl_texturenum );
	
					if( vid.height > scr_con_current )
						Sbar_Draw();

					if( developer.value != 0.0 )
					{
						GL_Bind( r_notexture_mip->gl_texturenum );
						Con_DrawNotify();
					}
					*/
				}

				SCR_ConnectMsg();
				VGui_Paint();

				GLFinishHud();

				GLBeginHud();

				//TODO: implement - Solokiller

				if( !VGuiWrap2_IsGameUIVisible() )
				{
					SCR_DrawPause();
					SCR_CheckDrawCenterString();
				}

				GLFinishHud();

				if( g_modfuncs.m_pfnFrameRender2 )
					g_modfuncs.m_pfnFrameRender2();

				GL_EndRendering();

				g_LastScreenUpdateTime = fCurrentTime;
			}
		}
	}

	recursionGuard = false;
}

void SCR_BeginLoadingPlaque( bool reconnect )
{
	S_StopAllSounds( true );

	if( ( cls.state == ca_connected ||
		  cls.state == ca_uninitialized || 
		  cls.state == ca_active ) &&
		cls.signon == 2 )
	{
		Con_ClearNotify();
		scr_drawloading = true;
		scr_fullupdate = false;
		scr_centertime_off = false;
		scr_con_current = false;

		StartLoadingProgressBar( "Connecting", 12 );
		SetLoadingProgressBarStatusText( "#GameUI_EstablishingConnection" );

		SCR_UpdateScreen();

		scr_fullupdate = false;
		VGuiWrap2_LoadingStarted( "transition", "" );
	}
}

void SCR_EndLoadingPlaque()
{
	scr_fullupdate = false;
	scr_drawloading = false;
	Con_ClearNotify();
	VGuiWrap2_LoadingFinished( "transition", "" );
}

void SCR_SizeUp_f()
{
	HudSizeUp();
	vid.recalc_refdef = true;
}

void SCR_SizeDown_f()
{
	HudSizeDown();
	vid.recalc_refdef = true;
}