#pragma once

#include "vgui2/IInputInternal.h"

namespace vgui
{

class CInputInternal : public IInputInternal
{
public:
	void RunFrame();
	
	void UpdateMouseFocus(int x, int y);
	
	void PanelDeleted(VPANEL panel);
	
	bool InternalCursorMoved(int x, int y);
	
	bool InternalMousePressed(MouseCode code);
	bool InternalMouseDoublePressed(MouseCode code);
	bool InternalMouseReleased(MouseCode code);
	bool InternalMouseWheeled(int delta);
	
	bool InternalKeyCodePressed(KeyCode code);
	void InternalKeyCodeTyped(KeyCode code);
	void InternalKeyTyped(wchar_t unichar);
	bool InternalKeyCodeReleased(KeyCode code);
	
	HInputContext CreateInputContext();
	void DestroyInputContext(HInputContext context);
	
	void AssociatePanelWithInputContext(HInputContext context, VPANEL pRoot);
	void ActivateInputContext(HInputContext context);
	
	VPANEL GetMouseCapture();
	
	bool IsChildOfModalPanel(VPANEL panel);
	
	void ResetInputContext(HInputContext context);
};

}; // namespace vgui