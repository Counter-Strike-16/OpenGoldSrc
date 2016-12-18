#include "GameConsoleNull.h"

static CGameConsole gGameConsole;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION, gGameConsole);

void CGameConsole::Activate()
{
	mbVisible = true;
};

void CGameConsole::Initialize()
{
};

void CGameConsole::Hide()
{
	mbVisible = false;
};

void CGameConsole::Clear()
{
};

bool CGameConsole::IsConsoleVisible()
{
	return mbVisible;
};

void CGameConsole::Printf(const char *format, ...)
{
};

void CGameConsole::DPrintf(const char *format, ...)
{
};

void CGameConsole::ColorPrintf(Color &clr)
{
};

void CGameConsole::SetParent(int parent)
{
};