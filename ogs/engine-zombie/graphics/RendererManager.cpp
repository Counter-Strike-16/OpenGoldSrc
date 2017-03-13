/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include "precompiled.hpp"
#include "graphics/RendererManager.hpp"
#include "system/SharedLib.hpp"

// Structure containing functions exported from render DLL
// refexport_t	re;

// Global accessor to rendering module interface
IRender *gpRender = nullptr;

/*
==============
VID_LoadRefresh
==============
*/
//qboolean VID_LoadRefresh(char *name)
bool CRendererManager::LoadRenderer(const char *asName)
{
	refimport_t ri;

	if(reflib_active)
	{
		gpRender->Shutdown();
		UnloadRenderer(); //VID_FreeReflib
	};

	Con_Printf("------- Loading %s -------\n", asName);
	
	CreateInterfaceFn fnRenderFactory;
	
	if(!(fnRenderFactory = Sys_GetFactory(asName)))
	{
		Con_Printf("Can't get a factory func from the %s renderer module\n", asName);
		return false;
	};
	
	gpRender = (IRender*)afnRenderFactory(OGS_RENDER_INTERFACE_VERSION, nullptr);
	
	// Interface didn't exported or has an incompatible version
	if(!gpRender)
	{
		Sys_Error(ERR_FATAL, "Failed to load render dll %s", asName);
		//Sys_Error(ERR_FATAL, "%s has incompatible api_version", asName);
		//Sys_FreeModule();
		UnloadRenderer(); //VID_FreeReflib
		return false;
	};

	ri.Con_Printf = VID_Printf;

	ri.Sys_Error = VID_Error;

	ri.FS_LoadFile = FS_LoadFile;
	ri.FS_FreeFile = FS_FreeFile;
	ri.FS_Gamedir = FS_Gamedir;

	ri.Vid_GetModeInfo = VID_GetModeInfo;
	ri.Vid_MenuInit = VID_MenuInit;
	ri.Vid_NewWindow = VID_NewWindow;
	
	CreateInterfaceFn fnEngineFactory = Sys_GetFactoryThis();

	if(gpRender->Init(fnEngineFactory, global_hInstance, MainWndProc) == -1)
	{
		gpRender->Shutdown();
		UnloadRenderer(); //VID_FreeReflib
		return false;
	};

	Con_Printf("------------------------------------\n");
	reflib_active = true;

	vidref_val = VIDREF_OTHER;
	
	if(vid_ref)
	{
		if(!Q_strcmp(vid_ref->string, "gl"))
			vidref_val = VIDREF_GL;
		else if(!Q_strcmp(vid_ref->string, "soft"))
			vidref_val = VIDREF_SOFT;
	};

	return true;
};

//void VID_FreeReflib()
void CRendererManager::UnloadRenderer()
{
	if(!Sys_FreeModule(mpRendererDLL))
		Sys_Error(ERR_FATAL, "Reflib FreeLibrary failed");
	
	memset(&re, 0, sizeof(re));
	mpRendererDLL = NULL;
	reflib_active = false;
};