#include "quakedef.h"
#include "qgl.h"
#include "gl_rmain.h"

int currenttexture = -1;	// to avoid unnecessary texture sets

int cnttextures[ 2 ] = { -1, -1 };     // cached

vec3_t r_origin = { 0, 0, 0 };

refdef_t r_refdef = {};

model_t* R_LoadMapSprite( const char *szFilename )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void AllowFog( bool allowed )
{
	//TODO: implement - Solokiller
}