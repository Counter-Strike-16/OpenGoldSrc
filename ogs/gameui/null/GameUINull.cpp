#include "GameUINull.h"

static CGameUI gGameUI;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION, gGameUI);

void CGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	TRACE("CGameUI::Initialize");
};

void CGameUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system)
{
	TRACE("CGameUI::Start");
};

void CGameUI::Shutdown()
{
	TRACE("CGameUI::Shutdown");
};

int CGameUI::ActivateGameUI()
{
	TRACE("CGameUI::ActivateGameUI");
	mbActive = true;
	return 1;
};

int CGameUI::ActivateDemoUI()
{
	TRACE("CGameUI::ActivateDemoUI");
	return 1;
};

int CGameUI::HasExclusiveInput()
{
	TRACE("CGameUI::HasExclusiveInput");
	return 0;
};

void CGameUI::RunFrame()
{
	TRACE("CGameUI::RunFrame");
};

void CGameUI::ConnectToServer(const char *game, int IP, int port)
{
	TRACE("CGameUI::ConnectToServer(%s, %d, %d)", game, IP, port);
};

void CGameUI::DisconnectFromServer()
{
	TRACE("CGameUI::DisconnectFromServer");
};

void CGameUI::HideGameUI()
{
	TRACE("CGameUI::HideGameUI");
	mbActive = false;
};

bool CGameUI::IsGameUIActive()
{
	TRACE("CGameUI::IsGameUIActive");
	return mbActive;
};

void CGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	TRACE("CGameUI::LoadingStarted");
};

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	TRACE("CGameUI::LoadingFinished");
};

void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	TRACE("CGameUI::StartProgressBar");
};

int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	TRACE("CGameUI::ContinueProgressBar");
	return 1;
};

void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	TRACE("CGameUI::StopProgressBar");
};

int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	TRACE("CGameUI::SetProgressBarStatusText");
};

void CGameUI::SetSecondaryProgressBar(float progress)
{
	TRACE("CGameUI::SetSecondaryProgressBar");
};

void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
	TRACE("CGameUI::SetSecondaryProgressBarText");
};