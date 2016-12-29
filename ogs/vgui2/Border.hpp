#pragma once

#include "vgui2/IBorder.h"

namespace vgui
{

class CBorder : public IBorder
{
public:
	void Paint(VPANEL panel);
	void Paint(int x0, int y0, int x1, int y1);
	void Paint(int x0, int y0, int x1, int y1, int breakSide, int breakStart, int breakStop);
	
	void SetInset(int left, int top, int right, int bottom);
	void GetInset(int &left, int &top, int &right, int &bottom);
	
	void AddLine(sides_e side, Color color, int startOffset, int endOffset);
	
	void ApplySchemeSettings(IScheme *pScheme, KeyValues *inResourceData);
	
	void ParseSideSettings(int side_index, KeyValues *inResourceData, IScheme *pScheme);
	
	const char *GetName();
	void SetName(const char *name);
};

}; // namespace vgui