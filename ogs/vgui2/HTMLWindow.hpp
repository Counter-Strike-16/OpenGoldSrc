#pragma once

#include "vgui2/IHTML.h"

namespace vgui
{

class CHTMLWindow : public IHTML, public IHTMLEvents
{
public:
	void OpenURL(const char *url);
	
	bool StopLoading();
	
	bool Refresh();
	
	bool Show(bool shown);
	
	char *GetOpenedPage();
	
	void OnSize(int x, int y, int w, int h);
	void GetHTMLSize(int &wide, int &tall);
	
	void Clear();
	
	void AddText(const char *text);
	
	void OnMouse(MouseCode code, MOUSE_STATE s, int x, int y);
	void OnChar(wchar_t unichar);
	void OnKeyDown(KeyCode code);
	
	IImage *GetBitmap();
	
	void SetVisible(bool state);
	
	bool OnStartURL(const char *url, const char *target, bool first);
	void OnFinishURL(const char *url);
	void OnProgressURL(long current, long maximum);
	
	void OnSetStatusText(const char *text);
	
	void OnUpdate();
	
	void OnLink();
	void OffLink();
};

}; // namespace vgui