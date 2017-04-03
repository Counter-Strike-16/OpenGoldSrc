/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
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

#include "VGUI.hpp"

namespace vgui
{

bool CVGui::Init(CreateInterfaceFn *factoryList, int numFactories)
{
	return false;
};

void CVGui::Shutdown()
{
};

void CVGui::Start()
{
};

void CVGui::Stop()
{
};

bool CVGui::IsRunning()
{
	return false;
};

void CVGui::RunFrame()
{
};

void CVGui::ShutdownMessage(unsigned int shutdownID)
{
};

VPANEL CVGui::AllocPanel()
{
	return 0;
};

void CVGui::FreePanel(VPANEL panel)
{
};

void CVGui::DPrintf(const char *format, ...)
{
};

void CVGui::DPrintf2(const char *format, ...)
{
};

void CVGui::SpewAllActivePanelNames()
{
};

HPanel CVGui::PanelToHandle(VPANEL panel)
{
	return 0;
};

VPANEL CVGui::HandleToPanel(HPanel index)
{
	return 0;
};

void CVGui::MarkPanelForDeletion(VPANEL panel)
{
};

void CVGui::AddTickSignal(VPANEL panel, int intervalMilliseconds)
{
};

void CVGui::RemoveTickSignal(VPANEL panekl)
{
};

void CVGui::PostMessage(VPANEL target, KeyValues *params, VPANEL from, float delaySeconds)
{
};

HContext CVGui::CreateContext()
{
	return 0;
};

void CVGui::DestroyContext(HContext context)
{
};

void CVGui::AssociatePanelWithContext(HContext context, VPANEL pRoot)
{
};

void CVGui::ActivateContext(HContext context)
{
};

void CVGui::SetSleep(bool state)
{
};

bool CVGui::GetShouldVGuiControlSleep()
{
	return false;
};

}; // namespace vgui