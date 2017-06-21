#pragma once

#include "cdll_int.h"
#include "ienginevgui.h"
#include "BaseUI/IBaseUI.h"

class BaseUISurface;
class ICareerUI;
class IClientVGUI;
class IGameConsole;
class IGameUI;
class IMouseControl;

class CBaseUI : public IBaseUI
{
private:
	enum
	{
		MAX_NUM_FACTORIES = 6,
	};

public:
	CBaseUI();
	~CBaseUI();

	void Initialize( CreateInterfaceFn* factories, int count ) override;

	void Start( cl_enginefunc_t* engineFuncs, int interfaceVersion ) override;

	void Shutdown() override;

	int Key_Event( int down, int keynum, const char* pszCurrentBinding ) override;

	void CallEngineSurfaceAppHandler( void* pEvent, void* pUserData ) override;

	void Paint( int x, int y, int right, int bottom ) override;

	void HideGameUI() override;

	void ActivateGameUI() override;

	void HideConsole() override;

	void ShowConsole() override;

private:
	CreateInterfaceFn m_FactoryList[ MAX_NUM_FACTORIES ] = {};
	int m_iNumFactories = 0;

	CSysModule* m_hVGuiModule = nullptr;
	CSysModule* m_hStaticGameUIModule = nullptr;
	CSysModule* m_hChromeModule = nullptr;
	bool m_bHidingGameUI = false;

private:
	CBaseUI( const CBaseUI& ) = delete;
	CBaseUI& operator=( const CBaseUI& ) = delete;
};

class CEngineVGui : public IEngineVGui
{
public:
	CEngineVGui() = default;
	virtual ~CEngineVGui() = default;

	vgui2::VPANEL GetPanel( VGUIPANEL type ) override;
	bool SteamRefreshLogin( const char* password, bool isSecure ) override;
	bool SteamProcessCall( bool* finished, TSteamProgress* progress, TSteamError* steamError ) override;

	void SetEngineVisible( bool state );

private:
	bool m_bVisible = true;
	SteamHandle_t m_hRefreshLoginHandle = 0;

private:
	CEngineVGui( const CEngineVGui& ) = delete;
	CEngineVGui& operator=( const CEngineVGui& ) = delete;
};

CBaseUI& BaseUI();

extern IGameUI* staticGameUIFuncs;

extern IGameConsole* staticGameConsole;

extern ICareerUI* staticCareerUI;

extern IClientVGUI* staticClient;

extern cl_enginefunc_t gEngfuncs;

extern BaseUISurface* staticSurface;

extern IMouseControl* mousecontrol;