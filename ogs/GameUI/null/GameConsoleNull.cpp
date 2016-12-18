#include "GameConsoleNull.h"

static CGameConsole gGameConsole;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameConsole, IGameConsole, GAMECONSOLE_INTERFACE_VERSION, gGameConsole);

void CGameConsole::Activate()
{
	TRACE("CGameConsole::Activate");
	mbVisible = true;
};

void CGameConsole::Initialize()
{
	TRACE("CGameConsole::Initialize");
};

void CGameConsole::Hide()
{
	TRACE("CGameConsole::Hide");
	mbVisible = false;
};

void CGameConsole::Clear()
{
	TRACE("CGameConsole::Clear");
};

bool CGameConsole::IsConsoleVisible()
{
	TRACE("CGameConsole::IsConsoleVisible");
	return mbVisible;
};

void CGameConsole::Printf(const char *format, ...)
{
	TRACE("CGameConsole::Printf");
};

void CGameConsole::DPrintf(const char *format, ...)
{
	TRACE("CGameConsole::DPrintf");
};

void CGameConsole::ColorPrintf(Color &clr)
{
	TRACE("CGameConsole::ColorPrintf");
};

void CGameConsole::SetParent(int parent)
{
	TRACE("CGameConsole::SetParent(%d)", parent);
};