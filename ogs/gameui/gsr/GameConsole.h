#ifndef GAMEUI_GAMECONSOLE_H
#define GAMEUI_GAMECONSOLE_H

#include "GameUI/IGameConsole.h"

class CGameConsoleDialog;

class CGameConsole : public IGameConsole
{
public:
	CGameConsole();
	~CGameConsole();

	void Activate() override;
	void Initialize() override;
	void Hide() override;
	void Clear() override;

	bool IsConsoleVisible() override;

	void Printf( const char* format, ... ) override;
	void DPrintf( const char* format, ... ) override;

	void SetParent( vgui2::VPANEL parent ) override;

	void ActivateDelayed( float time );

	static void OnCmdCondump();

private:
	bool m_bInitialized = false;
	CGameConsoleDialog* m_pConsole = nullptr;

private:
	CGameConsole( const CGameConsole& ) = delete;
	CGameConsole& operator=( const CGameConsole& ) = delete;
};

#endif //GAMEUI_GAMECONSOLE_H
