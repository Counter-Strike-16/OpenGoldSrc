//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_ImagePanel.h"
#include "VGUI_Image.h"

using namespace vgui;

ImagePanel::ImagePanel(Image* image) : Panel(0,0,64,64)
{
	_image=null;
	setImage(image);
}

void ImagePanel::setImage(Image* image)
{
	_image=image;
	if(_image)
	{
		int wide,tall;
		_image->getSize(wide,tall);
		setSize(wide,tall);
	}
	repaint();
}

void ImagePanel::paintBackground()
{
	if(_image)
	{
		drawSetColor(Scheme::sc_white);
		//_image->paint(this);
	}
}
