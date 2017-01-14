//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_MenuItem.h"
#include "VGUI_Button.h"

using namespace vgui;

MenuItem::MenuItem(const char* text) : Button(text,0,0)
{
}

MenuItem::MenuItem(const char* text,Menu* subMenu) : Button(text,0,0)
{
}
