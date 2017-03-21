/*
vgui_main.h - vgui main header
Copyright (C) 2011 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef VGUI_MAIN_H
#define VGUI_MAIN_H

#include "utlvector.h"
#include "utlrbtree.h"

#include<VGUI.h>
#include<VGUI_Cursor.h>
#include<VGUI_InputSignal.h>
#include<VGUI_MouseCode.h>
#include<VGUI_KeyCode.h>

//
// vgui_input.cpp
//
void VGUI_InitCursors();
void VGUI_CursorSelect(Cursor *cursor);
void VGUI_ActivateCurrentCursor();

#endif//VGUI_MAIN_H