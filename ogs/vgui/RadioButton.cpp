//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_RadioButton.h"

using namespace vgui;

RadioButton::RadioButton(const char* text,int x,int y,int wide,int tall) : ToggleButton(text,x,y,wide,tall)
{
}

RadioButton::RadioButton(const char* text,int x,int y) : ToggleButton(text,x,y)
{
}

void RadioButton::paintBackground()
{
}
