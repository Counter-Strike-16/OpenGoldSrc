#pragma once

#include "GameUI/IGameConsole.h"

class CGameConsole : public IGameConsole
{
public:
	void Activate();
	void Initialize();
	void Hide();
	void Clear();
	bool IsConsoleVisible();
	
	void Printf(const char *format, ...);
	void DPrintf(const char *format, ...);
	
	void ColorPrintf(Color &clr);
	
	void SetParent(int parent);
private:
	bool mbVisible;
};