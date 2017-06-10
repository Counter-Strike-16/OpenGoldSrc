#ifndef GAMEUI_GAMEUI_INTERFACE_H
#define GAMEUI_GAMEUI_INTERFACE_H

#include <vgui_controls/PHandle.h>

#include "GameUI/IGameUI.h"
#include "interface.h"

class CLoadingDialog;
class CSocket;
class CTaskbar;
class IBaseUI;
class IEngineVGui;
class IGameClientExports;
class IGameUIFuncs;

namespace vgui2
{
class TestScriptController;
}

class CGameUI : public IGameUI
{
private:
	enum
	{
		MAX_NUM_FACTORIES = 5
	};

public:
	CGameUI();
	~CGameUI();

	void Initialize( CreateInterfaceFn* factories, int count ) override;

	void Start( cl_enginefunc_t* engineFuncs, int interfaceVersion, IBaseSystem* system ) override;

	void Shutdown() override;

	int ActivateGameUI() override;

	int ActivateDemoUI() override;

	int HasExclusiveInput() override;

	void RunFrame() override;

	void ConnectToServer( const char* game, int IP, int port ) override;

	void DisconnectFromServer() override;

	void HideGameUI() override;

	int IsGameUIActive() override;

	void LoadingStarted( const char* resourceType, const char* resourceName ) override;

	void LoadingFinished( const char* resourceType, const char* resourceName ) override;

	void StartProgressBar( const char* progressType, int progressSteps ) override;

	int ContinueProgressBar( int progressPoint, float progressFraction ) override;

	void StopProgressBar( bool bError, const char* failureReason, const char* extendedReason ) override;

	int SetProgressBarStatusText( const char* statusText ) override;

	void SetSecondaryProgressBar( float progress ) override;

	void SetSecondaryProgressBarText( const char* statusText ) override;

	void ValidateCDKey( bool force, bool inConnect ) override;

	void OnDisconnectFromServer( int eSteamLoginFailure, const char* username ) override;

	void FinishCareerLoad();

	void RunVguiTestScript( const char* script );

private:
	bool FindPlatformDirectory( char* platformDir, int bufferSize );

private:
	bool m_bTryingToLoadTracker = false;

	char m_szPlatformDir[ MAX_PATH ];

	int m_iNumFactories;
	CreateInterfaceFn m_FactoryList[ MAX_NUM_FACTORIES ];

	int m_iGameIP = 0;
	int m_iGamePort = 0;

	bool m_bActivatedUI = false;

	float m_flRetryPlatformReleaseMessageTime;

	const char* m_pszCurrentProgressType = "";
	float m_flProgressStartTime = 0;

	CSocket* m_pMaster;

	vgui2::DHANDLE<vgui2::TestScriptController> m_hTestScriptController;

private:
	CGameUI( const CGameUI& ) = delete;
	CGameUI& operator=( const CGameUI& ) = delete;
};

IEngineVGui* EngineVGUI();

extern IGameUIFuncs* gameuifuncs;

extern IBaseUI* baseuifuncs;

IGameClientExports* GameClientExports();

extern IBaseSystem* g_pSystemWrapper;

extern CTaskbar* g_pTaskbar;

extern vgui2::PHandle g_hLoadingDialog;

extern CGameUI* g_pGameUI;

CGameUI& GameUI();

void RunVguiTestScript();

#endif //GAMEUI_GAMEUI_INTERFACE_H
