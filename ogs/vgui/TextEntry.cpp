//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_TextEntry.h"

using namespace vgui;

TextEntry::TextEntry(const char* text,int x,int y,int wide,int tall)
{
}

void TextEntry::setText(const char* text,int textLen)
{
}

void TextEntry::getText(int offset,char* buf,int bufLen)
{
}

void TextEntry::resetCursorBlink()
{
}

void TextEntry::doGotoLeft()
{
}

void TextEntry::doGotoRight()
{
}

void TextEntry::doGotoFirstOfLine()
{
}

void TextEntry::doGotoEndOfLine()
{
}

void TextEntry::doInsertChar(char ch)
{
}

void TextEntry::doBackspace()
{
}

void TextEntry::doDelete()
{
}

void TextEntry::doSelectNone()
{
}

void TextEntry::doCopySelected()
{
}

void TextEntry::doPaste()
{
}

void TextEntry::doPasteSelected()
{
}

void TextEntry::doDeleteSelected()
{
}

void TextEntry::addActionSignal(ActionSignal* s)
{
}

void TextEntry::setFont(Font* font)
{
}

void TextEntry::setTextHidden(bool bHideText)
{
}

void TextEntry::paintBackground()
{
}

void TextEntry::setCharAt(char ch,int index)
{
}

void TextEntry::fireActionSignal()
{
}

bool TextEntry::getSelectedRange(int& cx0,int& cx1)
{
	return false;
}

bool TextEntry::getSelectedPixelRange(int& cx0,int& cx1)
{
	return false;
}

int TextEntry::cursorToPixelSpace(int cursorPos)
{
	return 0;
}

void TextEntry::selectCheck()
{
}

void TextEntry::cursorMoved(int x,int y,Panel* panel)
{
}

void TextEntry::cursorEntered(Panel* panel)
{
}

void TextEntry::cursorExited(Panel* panel)
{
}

void TextEntry::mousePressed(MouseCode code,Panel* panel)
{
}

void TextEntry::mouseDoublePressed(MouseCode code,Panel* panel)
{
}

void TextEntry::mouseReleased(MouseCode code,Panel* panel)
{
}

void TextEntry::mouseWheeled(int delta,Panel* panel)
{
}

void TextEntry::keyPressed(KeyCode code,Panel* panel)
{
}

void TextEntry::keyTyped(KeyCode code,Panel* panel)
{
}

void TextEntry::keyReleased(KeyCode code,Panel* panel)
{
}
 
void TextEntry::keyFocusTicked(Panel* panel)
{
}
