#pragma once

#include "vgui2/IScheme.h"

namespace vgui
{

class CSchemeManager : public ISchemeManager
{
public:
	HScheme LoadSchemeFromFile(const char *fileName, const char *tag);
	
	void ReloadSchemes();
	
	HScheme GetDefaultScheme();
	HScheme GetScheme(const char *tag);
	
	IImage *GetImage(const char *imageName, bool hardwareFiltered);
	HTexture GetImageID(const char *imageName, bool hardwareFiltered);
	
	IScheme *GetIScheme(HScheme scheme);
	
	void Shutdown(bool full = true);
	
	int GetProportionalScaledValue(int normalizedValue);
	int GetProportionalNormalizedValue(int scaledValue);
};

}; // namespace vgui