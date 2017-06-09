#ifndef ENGINE_GL_DRAW_H
#define ENGINE_GL_DRAW_H

#include "qgl.h"
#include "wad.h"

extern cvar_t gl_ansio;

extern GLenum oldtarget;

void Draw_Init();

void Draw_FillRGBA( int x, int y, int w, int h, int r, int g, int b, int a );

int Draw_Character( int x, int y, int num, unsigned int font );

int Draw_MessageCharacterAdd( int x, int y, int num, int rr, int gg, int bb, unsigned int font );

int Draw_String( int x, int y, char* str );

int Draw_StringLen( const char* psz, unsigned int font );

void Draw_SetTextColor( float r, float g, float b );

void Draw_GetDefaultColor();

void Draw_ResetTextColor();

void Draw_FillRGBABlend( int x, int y, int w, int h, int r, int g, int b, int a );

GLuint GL_GenTexture();

void GL_SelectTexture( GLenum target );

void Draw_Pic( int x, int y, qpic_t* pic );

void Draw_BeginDisc();

#endif //ENGINE_GL_DRAW_H