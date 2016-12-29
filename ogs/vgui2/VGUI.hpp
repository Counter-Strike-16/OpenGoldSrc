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
	
	void PostMessage(VPANEL target, KeyValues *params, VPANEL from, float delaySeconds = 0.0);
	
	HContext CreateContext();
	void DestroyContext(HContext context);
	
	void AssociatePanelWithContext(HContext context, VPANEL pRoot);
	void ActivateContext(HContext context);
	
	void SetSleep(bool state);
	bool GetShouldVGuiControlSleep();
};

}; // namespace vgui