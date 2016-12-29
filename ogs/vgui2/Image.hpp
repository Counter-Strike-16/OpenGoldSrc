#pragma once

#include "vgui2/IImage.h"

namespace vgui
{

class CImage : public IImage
{
public:
	void Paint();
	
	void SetPos(int x, int y);
	
	void GetContentSize(int &wide, int &tall);
	
	void GetSize(int &wide, int &tall);
	void SetSize(int wide, int tall);
	
	void SetColor(Color color);
};

}; // namespace vgui