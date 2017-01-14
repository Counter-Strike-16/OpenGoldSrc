//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_PopupMenu.h"

using namespace vgui;

PopupMenu::PopupMenu(int x,int y,int wide,int tall) : Menu(x,y,wide,tall)
{
}

PopupMenu::PopupMenu(int wide,int tall) : Menu(wide,tall)
{
}

void PopupMenu::showModal(Panel* panel)
{
}
