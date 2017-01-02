/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file

#pragma once

#include "vgui2/IVGUI.h"

namespace vgui
{

class CVGui : public IVGui
{
public:
	bool Init(CreateInterfaceFn *factoryList, int numFactories);
	void Shutdown();
	
	void Start();
	void Stop();
	
	bool IsRunning();
	
	void RunFrame();
	
	void ShutdownMessage(unsigned int shutdownID);
	
	VPANEL AllocPanel();
	void FreePanel(VPANEL panel);
	
	void DPrintf(const char *format, ...);
	void DPrintf2(const char *format, ...);
	
	void SpewAllActivePanelNames();
	
	HPanel PanelToHandle(VPANEL panel);
	VPANEL HandleToPanel(HPanel index);
	
	void MarkPanelForDeletion(VPANEL panel);
	
	void AddTickSignal(VPANEL panel, int intervalMilliseconds = 0);
	void RemoveTickSignal(VPANEL panekl);
	
	void PostMessage(VPANEL target, KeyValues *params, VPANEL from, float delaySeconds = 0.0f);
	
	HContext CreateContext();
	void DestroyContext(HContext context);
	
	void AssociatePanelWithContext(HContext context, VPANEL pRoot);
	void ActivateContext(HContext context);
	
	void SetSleep(bool state);
	bool GetShouldVGuiControlSleep();
};

}; // namespace vgui