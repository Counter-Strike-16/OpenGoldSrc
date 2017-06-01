#include "EngineInterface.h"
#include "vgui_controls/consoledialog.h"
#include <Color.h>
#include "UtlVector.h"
#include "vgui_controls/Frame.h"

class CGameConsoleDialog : public vgui::CConsoleDialog
{
	DECLARE_CLASS_SIMPLE(CGameConsoleDialog, vgui::CConsoleDialog);

public:
	CGameConsoleDialog(void);

private:
	MESSAGE_FUNC(OnClosedByHittingTilde, "ClosedByHittingTilde");
	MESSAGE_FUNC_CHARPTR(OnCommandSubmitted, "CommandSubmitted", command);

private:
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
	virtual void OnCommand(const char *command);
};