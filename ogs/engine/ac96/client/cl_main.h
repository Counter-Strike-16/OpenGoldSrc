#pragma once

#include "client.hpp"
#include "common/cl_entity.h"
#include "common/dlight.h"

const int MAX_STARTUP_TIMINGS = 32;

struct startup_timing_t
{
	const char *name;
	float time;
};

extern cl_entity_t *cl_entities;

extern efrag_t cl_efrags[ MAX_EFRAGS ];
extern dlight_t cl_dlights[ MAX_DLIGHTS ];
extern dlight_t cl_elights[ MAX_ELIGHTS ];
extern lightstyle_t cl_lightstyle[ MAX_LIGHTSTYLES ];

extern float g_LastScreenUpdateTime;

extern cvar_t cl_lw;
extern cvar_t fs_perf_warnings;

void SetupStartupTimings();
void AddStartupTiming( const char* name );
void PrintStartupTimings();

dlight_t* CL_AllocDlight( int key );

dlight_t* CL_AllocElight( int key );

model_t* CL_GetModelByIndex( int index );

void CL_GetPlayerHulls();

bool UserIsConnectedOnLoopback();

void SetPal( int i );

void GetPos( vec3_t origin, vec3_t angles );

const char* CL_CleanFileName( const char* filename );

void CL_ClearCaches();

void CL_ClearClientState();

void CL_ClearState( bool bQuiet );

void CL_CreateResourceList();

#endif //ENGINE_CL_MAIN_H