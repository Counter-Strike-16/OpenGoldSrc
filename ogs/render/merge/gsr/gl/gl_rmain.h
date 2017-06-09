#ifndef ENGINE_GL_RMAIN_H
#define ENGINE_GL_RMAIN_H

#include "render.h"

extern int currenttexture;

extern int cnttextures[ 2 ];

extern vec3_t r_origin;

extern refdef_t r_refdef;

void AllowFog( bool allowed );

#endif //ENGINE_GL_RMAIN_H