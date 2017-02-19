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
/// @brief Render module interface

#pragma once

#include "public/interface.h"

// If requested version is different, the dll cannot be used
const char OGS_RENDER_INTERFACE_VERSION[] = "OGSRender001";

// These are the methods exported by the render module
struct IRender : public IBaseInterface
{
	/// Called when the library is loaded
	virtual bool Init(CreateInterfaceFn afnEngineFactory, void *ahInstance, void *apWndProc) = 0;
	
	/// Called before the library is unloaded
	virtual void Shutdown() = 0;
	
	///
	virtual void AppActivate(bool abActivate) = 0;
	
	// All data that will be used in a level should be
	// registered before rendering any frames to prevent disk hits,
	// but they can still be registered at a later time
	// if necessary.
	//
	// EndRegistration will free any remaining data that wasn't registered.
	// Any model_s or skin_s pointers from before the BeginRegistration
	// are no longer valid after EndRegistration.
	//
	// Skins and images need to be differentiated, because skins
	// are flood filled to eliminate mip map edge errors, and pics have
	// an implicit "pics/" prepended to the name. (a pic name that starts with a
	// slash will not use the "pics/" prefix or the ".pcx" postfix)
	virtual void BeginRegistration(const char *asMap) = 0;
	
	///
	virtual void EndRegistration() = 0;
	
	///
	virtual struct model_s *RegisterModel(const char *asName) = 0;
	
	///
	virtual struct image_s *RegisterSkin(const char *asName) = 0;
	
	///
	virtual struct image_s *RegisterPic(const char *asName) = 0;
	
	///
	virtual void SetSky(const char *asName, float afRotate, vec3_t avAxis) = 0;
	
	///
	virtual void BeginFrame(float afCameraSeparation) = 0;
	
	///
	virtual void RenderFrame(refdef_t *apRenderSettings) = 0;
	
	///
	virtual void EndFrame() = 0;
	
	///
	virtual void DrawGetPicSize(int *w, int *h, char *name) = 0; // will return 0 0 if not found
	
	///
	virtual void DrawPic(int x, int y, char *name) = 0;
	
	///
	virtual void DrawStretchPic(int x, int y, int w, int h, char *name) = 0;
	
	///
	virtual void DrawChar(int x, int y, int c) = 0;
	
	///
	virtual void DrawTileClear(int x, int y, int w, int h, char *name) = 0;
	
	///
	virtual void DrawFill(int x, int y, int w, int h, int c) = 0;
	
	///
	virtual void DrawFadeScreen() = 0;
	
	/// Draw images for cinematic rendering (which can have a different palette). Note that calls
	virtual void DrawStretchRaw(int x, int y, int w, int h, int cols, int rows, byte *data) = 0;
	
	virtual void DrawLine(polyvert_t *polyvert0, polyvert_t *polyvert1) = 0;
	
	/*
	** Video mode and refresh state management entry points
	*/
	virtual void CinematicSetPalette(const byte *palette) = 0; // NULL = game palette
	
	///
	virtual ITexture *CreateTexture() = 0;
};