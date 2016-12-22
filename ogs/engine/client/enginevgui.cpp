#include "system/precompiled.h"
#include "client/enginevgui.h"

IGameUI *gpGameUI = nullptr;
IGameConsole *gpGameConsole = nullptr;

static CEngineVGui gEngineVGuiImp;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEngineVGui, IEngineVGui, VENGINE_VGUI_VERSION, gEngineVGuiImp);

IEngineVGuiInternal *EngineVGui()
{
	return &gEngineVGuiImp;
}

CEngineVGui::CEngineVGui()
{
};

// Setup the base vgui panels
void CEngineVGui::Init()
{
	COM_TimestampedLog("Loading gameui dll");
	
	gpGameUI = (IGameUI*)fnLauncherFactory(GAMEUI_INTERFACE_VERSION, nullptr);
	
	if(!gpGameUI)
		return;
	
	gpGameConsole = (IGameConsole*)fnLauncherFactory(GAMECONSOLE_INTERFACE_VERSION, nullptr);
	
	if(!gpGameConsole)
		return;
	
	gpGameUI->Initialize();
	gpGameConsole->Initialize();
};

void CEngineVGui::Connect()
{
};

void CEngineVGui::Shutdown()
{
	gpGameUI->Shutdown();
};

void CEngineVGui::SetGameUIActive(bool bActive)
{
	if(bActive)
		gpGameUI->ActivateGameUI();
	else
		gpGameUI->HideGameUI();
};

bool CEngineVGui::IsGameUIActive()
{
	//return gpGameUI && gpGameUI->IsGameUIActive();
	return false;
};

// Show/hide the console
void CEngineVGui::SetConsoleVisible(bool bVisible)
{
	if(gpGameConsole)
	{
		if(bVisible)
		{
			SetGameUIActive(true);
			
			gpGameConsole->Activate();
		}
		else
			gpGameConsole->Hide();
	};
};

bool CEngineVGui::IsConsoleVisible()
{
	return gpGameConsole->IsConsoleVisible();
};

// Clear all text from the console
void CEngineVGui::ClearConsole()
{
	if(gpGameConsole)
		gpGameConsole->Clear();
};

void CEngineVGui::Simulate()
{
	gpGameUI->RunFrame();
};