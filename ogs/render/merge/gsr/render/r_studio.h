#ifndef ENGINE_R_STUDIO_H
#define ENGINE_R_STUDIO_H

#include "r_studioint.h"
#include "sound.h"

extern engine_studio_api_t engine_studio_api;
extern r_studio_interface_t studio;
extern r_studio_interface_t* pStudioAPI;

extern cvar_t* cl_righthand;

void R_ResetStudio();

sfx_t* CL_LookupSound( const char* pName );

#endif //ENGINE_R_STUDIO_H