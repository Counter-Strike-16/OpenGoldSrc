/*
UI.hpp - ui class
Copyright (C) 2015 Xash3D Dev Team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef XASH_UI_HPP
#define XASH_UI_HPP

#include "ui/GameUIDraw.h"

class CUI
{
public:
	CUI();
	~CUI();
	
	void Init();
	
	void Update(float afTimeStep);
	
	void SetActive(bool abActive);
	bool IsActive();
private:
	bool mbActive;
};

class IGameUI
{
public:
	virtual ~IGameUI(){};
	
	virtual void Init() = 0;
	
	virtual void Update(float afTimeStep) = 0;
	
	virtual void SetActive(bool abActive) = 0;
	virtual bool IsActive() = 0;
};

typedef struct
{
	void		*hInstance;		// pointer to client.dll
	UI_FUNCTIONS	dllFuncs;			// dll exported funcs
	byte		*mempool;			// client edicts pool

	cl_entity_t	playermodel;		// uiPlayerSetup drawing model
	player_info_t	playerinfo;		// local playerinfo

	gameui_draw_t	ds;			// draw2d stuff (menu images)
	GAMEINFO		gameInfo;			// current gameInfo
	GAMEINFO		*modsInfo[MAX_MODS];	// simplified gameInfo for MainUI

	ui_globalvars_t	*globals;

	qboolean		drawLogo;			// set to TRUE if logo.avi missed or corrupted
	long		logo_xres;
	long		logo_yres;
	float		logo_length;
} menu_static_t;

void UI_UnloadProgs();
qboolean UI_LoadProgs();

void UI_UpdateMenu( float realtime );
void UI_KeyEvent( int key, qboolean down );
void UI_MouseMove( int x, int y );

void UI_AddServerToList( netadr_t adr, const char *info );
void UI_GetCursorPos( int *pos_x, int *pos_y );
void UI_SetCursorPos( int pos_x, int pos_y );
void UI_ShowCursor( qboolean show );
qboolean UI_CreditsActive( void );
void UI_CharEvent( int key );
qboolean UI_MouseInRect( void );
qboolean UI_IsVisible( void );

void UI_NPrintf( int idx, char *fmt, ... );
void UI_NXPrintf( struct con_nprint_s *info, char *fmt, ... );

qboolean UI_IsCreditsActive();

void pfnPIC_Set( HIMAGE hPic, int r, int g, int b, int a );
void pfnPIC_Draw( int x, int y, int width, int height, const wrect_t *prc );
void pfnPIC_DrawTrans( int x, int y, int width, int height, const wrect_t *prc );
void pfnPIC_DrawHoles( int x, int y, int width, int height, const wrect_t *prc );
void pfnPIC_DrawAdditive( int x, int y, int width, int height, const wrect_t *prc );

#endif // XASH_MENU_H