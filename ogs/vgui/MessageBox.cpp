//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_MessageBox.h"
#include "VGUI_Frame.h"

using namespace vgui;

MessageBox::MessageBox(const char* title,const char* text,int x,int y) : Frame(x,y,64,64)
{
}

void MessageBox::performLayout()
{
}

void MessageBox::addActionSignal(ActionSignal* s)
{
}

void MessageBox::fireActionSignal()
{
}
