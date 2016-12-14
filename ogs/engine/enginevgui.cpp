#include "enginevgui.h"

IGameUI *gpGameUI = nullptr;
IGameConsole *gpGameConsole = nullptr;

CEngineVGui::CEngineVGui()
{
};

CEngineVGui::~CEngineVGui()
{
};

void CEngineVGui::Init()
{
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

void CEngineVGui::Update()
{
	gpGameUI->RunFrame();
};

void CEngineVGui::SetMenuActive(bool bActive)
{
	if(bActive)
		gpGameUI->ActivateGameUI();
	else
		gpGameUI->HideGameUI();
};

bool CEngineVGui::IsMenuActive()
{
	return gpGameUI->IsGameUIActive();
};

void CEngineVGui::SetConsoleVisible(bool bVisible)
{
	gpGameUI->ActivateGameUI();
	
	if(bVisible)
		gpGameConsole->Activate();
	else
		gpGameConsole->Hide();
};

bool CEngineVGui::IsConsoleVisible()
{
	return gpGameConsole->IsConsoleVisible();
};

void CEngineVGui::ClearConsole()
{
	gpGameConsole->Clear();
};