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
#include "ui/vgui_int.hpp"

// SDL defines this
#undef main
#include <VGUI_App.h>

#include "engine/cdll_int.h"
#include "public/interface.h"

#include "public/IEngineSurface.h"
#include "sound/sound.hpp"
#include "graphics/VideoMode.hpp"
#include "ui/VGUI_EngineSurfaceWrap.hpp"

static EngineSurfaceWrap* staticEngineSurface = nullptr;

static vgui::Panel* staticPanel = nullptr;

void VGuiWrap_SetRootPanelSize()
{
	auto pRoot = VGuiWrap_GetPanel();

	if( pRoot )
	{
		int x = 0, y = 0;
		Rect_t rect;

		rect.y = 0;

		if( VideoMode_IsWindowed() )
		{
			SDL_GetWindowPosition( pmainwindow, &x, &y );
			SDL_GetWindowSize( pmainwindow, &rect.width, &rect.height );
		}
		else
			VideoMode_GetCurrentVideoMode( &rect.width, &rect.height, nullptr );

		rect.height += rect.y;

		pRoot->setBounds( x, y, rect.width, rect.height );
	}
}

void VGuiWrap_Startup()
{
	if( staticEngineSurface )
		return;

	auto pApp = vgui::App::getInstance();

	pApp->reset();

	staticPanel = new vgui::Panel( 0, 0, 320, 240 ); // NO! ABSOLUTELY NO!

	auto pScheme = pApp->getScheme();

	staticPanel->setPaintBorderEnabled( false );
	staticPanel->setPaintBackgroundEnabled( false );
	staticPanel->setPaintEnabled( false );
	staticPanel->setCursor( pScheme->getCursor( vgui::Scheme::scu_none ) );

	auto factoryFn = Sys_GetFactoryThis();

	auto pSurface = static_cast<IEngineSurface*>( factoryFn( ENGINESURFACE_INTERFACE_VERSION, nullptr ) );

	staticEngineSurface = new EngineSurfaceWrap( staticPanel, pSurface );

	VGuiWrap_SetRootPanelSize();
}

void VGuiWrap_Shutdown()
{
	delete staticPanel;
	staticPanel = nullptr;

	if( staticEngineSurface )
		delete staticEngineSurface;

	staticEngineSurface = nullptr;
}

bool VGuiWrap_CallEngineSurfaceAppHandler( void* event, void* userData )
{
	if( staticEngineSurface )
		staticEngineSurface->AppHandler( event, userData );

	return false;
}

vgui::Panel* VGuiWrap_GetPanel()
{
	//g_engdstAddrs.VGui_GetPanel();
	return staticPanel;
}

void VGuiWrap_ReleaseMouse()
{
	if( vgui::App::getInstance() && staticEngineSurface )
	{
		if( VGuiWrap2_UseVGUI1() )
		{
			staticEngineSurface->setCursor( 
				vgui::App::getInstance()->
					getScheme()->
						getCursor( vgui::Scheme::scu_arrow ) );

			staticEngineSurface->lockCursor();
		}

		ClientDLL_DeactivateMouse();
		SetMouseEnable( false );
	}
}

void VGuiWrap_GetMouse()
{
	if( staticEngineSurface )
	{
		if( VGuiWrap2_UseVGUI1() )
			staticEngineSurface->unlockCursor();

		ClientDLL_ActivateMouse();
		SetMouseEnable( true );
	}
}

void VGuiWrap_SetVisible( bool state )
{
	if( staticPanel )
	{
		staticPanel->setVisible( state );
	}
}

void VGuiWrap_Paint( bool paintAll )
{
	auto pRoot = VGuiWrap_GetPanel();

	if( pRoot )
	{
		VGuiWrap_SetRootPanelSize();

		vgui::App::getInstance()->externalTick();

		if( paintAll )
		{
			pRoot->paintTraverse();
		}
		else
		{
			int extents[ 4 ];

			pRoot->getAbsExtents(
				extents[ 0 ],
				extents[ 1 ],
				extents[ 2 ],
				extents[ 3 ]
			);

			VGui_ViewportPaintBackground( extents );
		}
	}
}

class CDummyApp : public vgui::App
{
public:
	void main( int argc, char* argv[] ) override;

protected:
	void platTick() override;
};

void CDummyApp::main( int argc, char* argv[] )
{
	//Nothing
}

void CDummyApp::platTick()
{
	//Nothing
}

//App for VGUI programs, globally accessed through vgui::App::getInstance().
static CDummyApp theApp;
