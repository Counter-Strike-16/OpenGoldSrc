#include "EngineInterface.h"
#include "GameConsoleDialog.h"
#include "GameUI_Interface.h"
#include "vgui/iinput.h"
#include "vgui/isurface.h"
#include "vgui/keycode.h"
#include "LoadingDialog.h"
#include "igameuifuncs.h"
#include <keydefs.h>

using namespace vgui;

CGameConsoleDialog::CGameConsoleDialog(void)
    : BaseClass(NULL, "GameConsole", false)
{
	AddActionSignalTarget(this);
}

void CGameConsoleDialog::OnCommand(const char *command)
{
	BaseClass::OnCommand(command);
}

void CGameConsoleDialog::OnKeyCodeTyped(KeyCode code)
{
	BaseClass::OnKeyCodeTyped(code);

	if(m_pConsolePanel->TextEntryHasFocus())
	{
#ifdef _DEBUG
		if(engine->pfnGetCvarFloat("developer") >= 1)
		{
			switch(code)
			{
			case KEY_F1:
			{
				engine->pfnClientCmd("connect 127.1:27015\n");
				break;
			}

			case KEY_F2:
			{
				engine->pfnClientCmd("connect 127.1:4242\n");
				break;
			}
			}
		}
#else
		if(code >= KEY_F1 && code <= KEY_F12)
		{
			const char *binding = gameuifuncs->Key_BindingForKey(K_F1 + (code - KEY_F1));

			if(binding && binding[0])
			{
				char szCommand[256];
				Q_strncpy(szCommand, binding, sizeof(szCommand));
				engine->pfnClientCmd(szCommand);
			}
		}
#endif
	}
}

void CGameConsoleDialog::OnCommandSubmitted(const char *pCommand)
{
	engine->pfnClientCmd((char *)pCommand);
}

void CGameConsoleDialog::OnClosedByHittingTilde(void)
{
}