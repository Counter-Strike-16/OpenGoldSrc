#include "baseui.hpp"

void CBaseUI::Initialize(CreateInterfaceFn *factories, int count)
{
	if (!gConfigs.bInitialied)
		Config_Init();

	g_pfnCBaseUI_Initialize(this, 0, factories, count);
	g_hVGUI2 = (HINTERFACEMODULE)GetModuleHandle("vgui2.dll");

	if (g_hVGUI2)
	{
		CreateInterfaceFn fnVGUI2CreateInterface = Sys_GetFactory(g_hVGUI2);
		CreateInterfaceFn fnEngineCreateInterface = g_pMetaHookAPI->GetEngineFactory();

		IKeyValuesSystem *pKeyValuesSystem = (IKeyValuesSystem *)fnVGUI2CreateInterface(KEYVALUESSYSTEM_INTERFACE_VERSION, NULL);

		vgui::IVGui *pVGui = (vgui::IVGui *)fnVGUI2CreateInterface(VGUI_IVGUI_INTERFACE_VERSION, NULL);
		vgui::ISurface *pSurface = (vgui::ISurface *)fnEngineCreateInterface(VGUI_SURFACE_INTERFACE_VERSION, NULL);
		vgui::ISchemeManager *pSchemeManager = (vgui::ISchemeManager *)fnVGUI2CreateInterface(VGUI_SCHEME_INTERFACE_VERSION, NULL);
		vgui::ILocalize *pLocalize = (vgui::ILocalize *)fnVGUI2CreateInterface(VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
		vgui::IInputInternal *pInput = (vgui::IInputInternal *)fnVGUI2CreateInterface(VGUI_INPUT_INTERFACE_VERSION, NULL);
		vgui::IPanel *pPanel = (vgui::IPanel *)fnVGUI2CreateInterface(VGUI_PANEL_INTERFACE_VERSION, NULL);

		KeyCode_InitKeyTranslationTable();
		Panel_InstallHook(pPanel);
		Surface_InstallHook(pSurface);
		SchemeManager_InstallHook(pSchemeManager);
		Input_InstallHook(pInput);
		KeyValuesSystem_InstallHook(pKeyValuesSystem);

		vgui::VGui_LoadEngineInterfaces(fnVGUI2CreateInterface, fnEngineCreateInterface);
	}

	vgui_stencil_test = gEngfuncs.pfnRegisterVariable("vgui_stencil_test", "0", FCVAR_ARCHIVE);
	vgui_message_dialog_modal = engine->pfnRegisterVariable("vgui_message_dialog_modal", "1", FCVAR_ARCHIVE);
	vgui_emulatemouse = engine->pfnGetCvarPointer("vgui_emulatemouse");
};

void CBaseUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion)
{
	CreateInterfaceFn factories[2];
	factories[0] = g_pMetaHookAPI->GetEngineFactory();
	factories[1] = Sys_GetFactory(g_hVGUI2);

	vgui::g_pInput->SetIMEWindow(g_hMainWnd);

	g_pVGuiLocalize = (vgui::ILocalize *)(Sys_GetFactoryThis())(VGUI_LOCALIZE_INTERFACE_VERSION, NULL);
	g_pVGuiLocalize->AddFile("resource/valve_%language%.txt", "GAME");
	g_pVGuiLocalize->AddFile("resource/csbte_%language%.txt", "GAME");

	g_pfnCBaseUI_Start(this, FALSE, engineFuncs, interfaceVersion);
	g_pFileSystem->CreateDirHierarchy("cfg", "DEFAULT_WRITE_PATH");

	if (1)
	{
		engineFuncs->pfnAddCommand("_setlanguage", SetLanguage_f);
		engineFuncs->pfnAddCommand("_dump_fonttexture", DumpFontTexture_f);
	};
};

void CBaseUI::Shutdown()
{
	//g_pfnCBaseUI_Shutdown(this, 0);
};

int CBaseUI::Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	return 0; //g_pfnCBaseUI_Key_Event(this, 0, down, keynum, pszCurrentBinding);
};

void CBaseUI::CallEngineSurfaceProc(void *hwnd, unsigned int msg, unsigned int wparam, long lparam)
{
	//g_pfnCBaseUI_CallEngineSurfaceProc(this, 0, hwnd, msg, wparam, lparam);
};

void CBaseUI::Paint(int x, int y, int right, int bottom)
{
	//g_pfnCBaseUI_Paint(this, 0, x, y, right, bottom);
};

void CBaseUI::HideGameUI()
{
	//g_pfnCBaseUI_HideGameUI(this, 0);
};

void CBaseUI::ActivateGameUI()
{
	//g_pfnCBaseUI_ActivateGameUI(this, 0);
};

bool CBaseUI::IsGameUIVisible()
{
	return false; //g_pfnCBaseUI_IsGameUIVisible(this, 0);
};

void CBaseUI::HideConsole()
{
	//g_pfnCBaseUI_HideConsole(this, 0);
};

void CBaseUI::ShowConsole()
{
	//g_pfnCBaseUI_ShowConsole(this, 0);
};