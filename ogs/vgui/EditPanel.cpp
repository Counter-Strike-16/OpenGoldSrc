//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <stdio.h>
#include <stdarg.h>
#include "VGUI.h"
#include "VGUI_EditPanel.h"
#include "VGUI_App.h"
#include "VGUI_InputSignal.h"
#include "VGUI_Font.h"

using namespace vgui;

class FooDefaultEditPanelSignal : public InputSignal
{
public:
	FooDefaultEditPanelSignal(EditPanel* editPanel)
	{
		_editPanel=editPanel;
	}
	void cursorMoved(int x,int y,Panel* panel)
	{
	}
	void cursorEntered(Panel* panel)
	{
	}
	void cursorExited(Panel* panel)
	{
	}
	void mousePressed(MouseCode code,Panel* panel)
	{
		_editPanel->requestFocus();
		_editPanel->repaint();
	}
	void mouseDoublePressed(MouseCode code,Panel* panel)
	{
	}
	void mouseReleased(MouseCode code,Panel* panel)
	{
	}
	void mouseWheeled(int delta,Panel* panel)
	{
	}
	void keyPressed(KeyCode code,Panel* panel)
	{
	}
	void keyTyped(KeyCode code,Panel* panel)
	{
		bool shift=(_editPanel->isKeyDown(KEY_LSHIFT)||_editPanel->isKeyDown(KEY_RSHIFT));
		switch(code)
		{
		case KEY_UP:
			_editPanel->doCursorUp();
			break;
		case KEY_DOWN:
			_editPanel->doCursorDown();
			break;
		case KEY_LEFT:
			_editPanel->doCursorLeft();
			break;
		case KEY_RIGHT:
			_editPanel->doCursorRight();
			break;
		case KEY_HOME:
			_editPanel->doCursorToStartOfLine();
			break;
		case KEY_END:
			_editPanel->doCursorToEndOfLine();
			break;
		case KEY_BACKSPACE:
			_editPanel->doCursorBackspace();
			break;
		case KEY_DELETE:
			_editPanel->doCursorDelete();
			break;
		case KEY_ENTER:
			_editPanel->doCursorNewLine();
			break;
		default:
			_editPanel->doCursorInsertChar(_editPanel->getApp()->getKeyCodeChar(code,shift));
			break;
		case KEY_LSHIFT:
		case KEY_RSHIFT:
		case KEY_LALT:
		case KEY_RALT:
		case KEY_LCONTROL:
		case KEY_RCONTROL:
			break;
		}
	}
	void keyReleased(KeyCode code,Panel* panel)
	{
	}
	void keyFocusTicked(Panel* panel)
	{
		bool blink;
		int nextBlinkTime;
		_editPanel->getCursorBlink(blink,nextBlinkTime);
		if(_editPanel->getApp()->getTimeMillis()>nextBlinkTime)
			_editPanel->setCursorBlink(!blink);
	}
private:
	EditPanel* _editPanel;
};

EditPanel::EditPanel(int x,int y,int wide,int tall)
{
	_cursor[0]=0;
	_cursor[1]=0;
	_font=null;
	setCursorBlink(true);
	addInputSignal(new FooDefaultEditPanelSignal(this));
	getLine(0);
}

void EditPanel::doCursorUp()
{
	_cursor[0]=spatialCharOffsetBetweenTwoLines(getLine(_cursor[1]),getLine(_cursor[1]-1),_cursor[0]);
	_cursor[1]--;

	if(_cursor[1]<0)
		_cursor[1]=0;

	setCursorBlink(true);
	repaint();
}

void EditPanel::doCursorDown()
{
	int visibleCount=getVisibleLineCount();
	_cursor[0]=spatialCharOffsetBetweenTwoLines(getLine(_cursor[1]),getLine(_cursor[1]+1),_cursor[0]);

	if(visibleCount>getLineCount())
		visibleCount=getLineCount();

	if(_cursor[1]+1>=visibleCount)
		return;

	_cursor[1]++;
	setCursorBlink(true);
	repaint();
}

void EditPanel::doCursorLeft()
{
	_cursor[0]--;

	if(_cursor[0]<0)
		_cursor[0]=0;

	setCursorBlink(true);
	repaint();
}

void EditPanel::doCursorRight()
{
	_cursor[0]++;

	setCursorBlink(true);
	repaint();
}

void EditPanel::doCursorToStartOfLine()
{
	_cursor[0]=0;

	setCursorBlink(true);
	repaint();
}

void EditPanel::doCursorToEndOfLine()
{
	Dar<char>* lineDar=getLine(_cursor[1]);

	if(!lineDar)
		return;

	_cursor[0]=lineDar->getCount();

	setCursorBlink(true);
	repaint();
}

void EditPanel::doCursorInsertChar(char ch)
{
	Dar<char>* lineDar=getLine(_cursor[1]);

	if(!lineDar)
		return;

	shiftLineRight(lineDar,_cursor[0],1);
	setChar(lineDar,_cursor[0],ch);
	doCursorRight();
	repaint();
}

void EditPanel::doCursorBackspace()
{
	// dummy
}

void EditPanel::doCursorNewLine()
{
	// dummy
}

void EditPanel::doCursorDelete()
{
	Dar<char>* lineDar=getLine(_cursor[1]);

	if(!lineDar)
		return;

	doCursorRight();
	shiftLineRight(lineDar,_cursor[0],1);
	doCursorLeft();
	repaint();
}

void EditPanel::doCursorPrintf(char* format,...)
{
	char    buf[8192];
	va_list argList;

	va_start(argList,format);
	vsprintf(buf,format,argList);
	va_end(argList);

	for(int i=0;i<sizeof(buf);i++)
	{
		if(buf[i]==0)
			break;

		if(buf[i]=='\n')
		{
			doCursorNewLine();
			break;
		}

		doCursorInsertChar(buf[i]);
	}
	repaint();
}

int EditPanel::getLineCount()
{
	return _lineDarDar.getCount();
}

int EditPanel::getVisibleLineCount()
{
	int wide,tall;
	getPaintSize(wide,tall);

	if(_font)
		return tall/_font->getTall();

	return tall/getApp()->getScheme()->getFont(Scheme::sf_primary1)->getTall();
}

void EditPanel::setCursorBlink(bool state)
{
	_cursorBlink=state;
	_cursorNextBlinkTime=getApp()->getTimeMillis()+400;
	repaint();
}

void EditPanel::setFont(Font* font)
{
	_font=font;
	repaint();
}

void EditPanel::getText(int lineIndex, int offset,char* buf,int bufLen)
{
	if((lineIndex<0)||(lineIndex>=_lineDarDar.getCount()))
		return;

	int len=0;
	Dar<char>* lineDar=_lineDarDar[lineIndex];
	for(int i=offset;i<lineDar->getCount();i++)
	{
		if(len>bufLen-1)
			break;
		buf[i-offset]=(*lineDar)[i]; // ?
		len++;
	}
	buf[len]=0;
}

void EditPanel::getCursorBlink(bool& blink,int& nextBlinkTime)
{
	blink=_cursorBlink;
	nextBlinkTime=_cursorNextBlinkTime;
}

void EditPanel::paintBackground()
{
	int wide,tall;
	getPaintSize(wide,tall);
	drawSetColor(Scheme::sc_white);
	drawFilledRect(0,0,wide,tall);
}

void EditPanel::paint()
{
	// dummy
}

void EditPanel::addLine()
{
}

Dar<char>* EditPanel::getLine(int lineIndex)
{
	if(lineIndex<0)
		return null;

	if((lineIndex==0)&&(_lineDarDar.getCount()==0))
	{
		_lineDarDar.addElement(new Dar<char>);
		return _lineDarDar[0];
	}

	if(lineIndex>=_lineDarDar.getCount())
		return null;

	return _lineDarDar[lineIndex];
}

void EditPanel::setChar(Dar<char>* lineDar,int x,char ch,char fill)
{
	if(!lineDar)
		return;

	if(x<0)
		return;

	int count=lineDar->getCount();
	if(x>=count)
	{
		lineDar->ensureCapacity(x+1);
		for(int i=count;i<x;i++) // ?
		{
			lineDar[i]=ch;
		}
	}

	lineDar[x]=ch;
}

void EditPanel::setChar(Dar<char>* lineDar,int x,char ch)
{
	setChar(lineDar,x,ch,' ');
}

void EditPanel::shiftLineLeft(Dar<char>* lineDar,int x,int count)
{
	if((x<0)||(x>=lineDar->getCount()))
		return;

	if(count<0)
		return;

	for(int i=x,j=x-count;i<lineDar->getCount();i++,j++)
	{
		if(j>=0)
			setChar(lineDar,j,(*lineDar)[i]);
	}

	lineDar->setCount(lineDar->getCount()-count);
}

void EditPanel::shiftLineRight(Dar<char>* lineDar,int x,int count)
{
	if(x<0)
		return;

	if(count<0)
		return;

	for(int i=lineDar->getCount()-1;i>=x;i--)
	{
		setChar(lineDar,i,(*lineDar)[i]);
	}
}

int EditPanel::spatialCharOffsetBetweenTwoLines(Dar<char>* srcDar,Dar<char>* dstDar,int x)
{
	// dummy
	return 0;
}
