/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "public/archtypes.h" // DAL

// C functions for external declarations that call the appropriate C++ methods

#ifndef CBASE_DLLEXPORT
#ifdef _WIN32
#define CBASE_DLLEXPORT __declspec(dllexport)
#else
#define CBASE_DLLEXPORT __attribute__((visibility("default")))
#endif
#endif

#define EXPORT CBASE_DLLEXPORT

extern "C" CBASE_DLLEXPORT int GetEntityAPI(DLL_FUNCTIONS *pFunctionTable, int interfaceVersion);
extern "C" CBASE_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion);