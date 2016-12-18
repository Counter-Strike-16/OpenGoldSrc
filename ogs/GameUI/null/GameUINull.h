#pragma once

#include "GameUI/IGameUI.hpp"

class CGameUI : public IGameUI
{
public:
	void Initialize(CreateInterfaceFn *factories, int count);
	void Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system);
	void Shutdown();
	
	int ActivateGameUI();
	int ActivateDemoUI();
	
	int HasExclusiveInput();
	
	void RunFrame();
	
	void ConnectToServer(const char *game, int IP, int port);
	void DisconnectFromServer();
	
	void HideGameUI();
	bool IsGameUIActive();
	
	void LoadingStarted(const char *resourceType, const char *resourceName);
	void LoadingFinished(const char *resourceType, const char *resourceName);
	
	void StartProgressBar(const char *progressType, int progressSteps);
	int ContinueProgressBar(int progressPoint, float progressFraction);
	void StopProgressBar(bool bError, const char *failureReason, const char *extendedReason = NULL);
	
	int SetProgressBarStatusText(const char *statusText);
	
	void SetSecondaryProgressBar(float progress);
	void SetSecondaryProgressBarText(const char *statusText);
private:
	bool mbActive;
};