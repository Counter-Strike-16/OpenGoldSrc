#pragma once

#include "vgui2/IPanel.h"

namespace vgui
{

class CPanel : public IPanel
{
public:
	void Init(VPANEL vguiPanel, IClientPanel *panel);
	
	void SetPos(VPANEL vguiPanel, int x, int y);
	void GetPos(VPANEL vguiPanel, int &x, int &y);
	
	void SetSize(VPANEL vguiPanel, int wide, int tall);
	void GetSize(VPANEL vguiPanel, int &wide, int &tall);
	
	void SetMinimumSize(VPANEL vguiPanel, int wide, int tall);
	void GetMinimumSize(VPANEL vguiPanel, int &wide, int &tall);
	
	void SetZPos(VPANEL vguiPanel, int z);
	int GetZPos(VPANEL vguiPanel);
	
	void GetAbsPos(VPANEL vguiPanel, int &x, int &y);
	
	void GetClipRect(VPANEL vguiPanel, int &x0, int &y0, int &x1, int &y1);
	
	void SetInset(VPANEL vguiPanel, int left, int top, int right, int bottom);
	void GetInset(VPANEL vguiPanel, int &left, int &top, int &right, int &bottom);
	
	void SetVisible(VPANEL vguiPanel, bool state);
	bool IsVisible(VPANEL vguiPanel);
	
	void SetParent(VPANEL vguiPanel, VPANEL newParent);
	
	int GetChildCount(VPANEL vguiPanel);
	
	VPANEL GetChild(VPANEL vguiPanel, int index);
	VPANEL GetParent(VPANEL vguiPanel);
	
	void MoveToFront(VPANEL vguiPanel);
	void MoveToBack(VPANEL vguiPanel);
	
	bool HasParent(VPANEL vguiPanel, VPANEL potentialParent);
	
	bool IsPopup(VPANEL vguiPanel);
	void SetPopup(VPANEL vguiPanel, bool state);
	
	bool Render_GetPopupVisible(VPANEL vguiPanel);
	void Render_SetPopupVisible(VPANEL vguiPanel, bool state);
	
	HScheme GetScheme(VPANEL vguiPanel);
	
	bool IsProportional(VPANEL vguiPanel);
	bool IsAutoDeleteSet(VPANEL vguiPanel);
	
	void DeletePanel(VPANEL vguiPanel);
	
	void SetKeyBoardInputEnabled(VPANEL vguiPanel, bool state);
	void SetMouseInputEnabled(VPANEL vguiPanel, bool state);
	
	bool IsKeyBoardInputEnabled(VPANEL vguiPanel);
	bool IsMouseInputEnabled(VPANEL vguiPanel);
	
	void Solve(VPANEL vguiPanel);
	
	const char *GetName(VPANEL vguiPanel);
	const char *GetClassName(VPANEL vguiPanel);
	
	void SendMessage(VPANEL vguiPanel, KeyValues *params, VPANEL ifromPanel);
	
	void Think(VPANEL vguiPanel);
	
	void PerformApplySchemeSettings(VPANEL vguiPanel);
	
	void PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce = true);
	void Repaint(VPANEL vguiPanel);
	
	VPANEL IsWithinTraverse(VPANEL vguiPanel, int x, int y, bool traversePopups);
	
	void OnChildAdded(VPANEL vguiPanel, VPANEL child);
	void OnSizeChanged(VPANEL vguiPanel, int newWide, int newTall);
	
	void InternalFocusChanged(VPANEL vguiPanel, bool lost);
	
	bool RequestInfo(VPANEL vguiPanel, KeyValues *outputData);
	void RequestFocus(VPANEL vguiPanel, int direction = 0);
	bool RequestFocusPrev(VPANEL vguiPanel, VPANEL existingPanel);
	bool RequestFocusNext(VPANEL vguiPanel, VPANEL existingPanel);
	
	VPANEL GetCurrentKeyFocus(VPANEL vguiPanel);
	
	int GetTabPosition(VPANEL vguiPanel);
	
	SurfacePlat *Plat(VPANEL vguiPanel);
	void SetPlat(VPANEL vguiPanel, SurfacePlat *Plat);
	
	Panel *GetPanel(VPANEL vguiPanel, const char *destinationModule);
	
	bool IsEnabled(VPANEL vguiPanel);
	void SetEnabled(VPANEL vguiPanel, bool state);
	
	void *Client(VPANEL vguiPanel);
	
	const char *GetModuleName(VPANEL vguiPanel);
};

}; // namespace vgui