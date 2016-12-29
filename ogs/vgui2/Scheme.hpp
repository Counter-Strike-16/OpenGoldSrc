#pragma once

#include "vgui2/IScheme.h"

namespace vgui
{

class CScheme : public IScheme
{
public:
	const char *GetResourceString(const char *stringName);
	IBorder *GetBorder(const char *borderName);
	HFont GetFont(const char *fontName, bool proportional = false);
	Color GetColor(const char *colorName, Color defaultColor);
};

}; // namespace vgui