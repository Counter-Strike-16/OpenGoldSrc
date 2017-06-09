#ifndef ENGINE_GL_SCREEN_H
#define ENGINE_GL_SCREEN_H

#include "wad.h"

extern int clearnotify;

extern float scr_con_current;
extern float scr_fov_value;

extern bool scr_copytop;

extern int glx;
extern int gly;
extern int glwidth;
extern int glheight;

extern cvar_t scr_viewsize;

void SCR_Init();

void SCR_DrawConsole();

void SCR_CenterPrint( const char* str );

void Draw_CenterPic( qpic_t* pPic );

void SCR_DrawLoading();

void SCR_UpdateScreen();

void SCR_BeginLoadingPlaque( bool reconnect );
void SCR_EndLoadingPlaque();

#endif //ENGINE_GL_SCREEN_H