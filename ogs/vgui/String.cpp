//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include <string.h>
#include "VGUI.h"
#include "VGUI_String.h"

using namespace vgui;

String::String()
{
	_text="";
}

String::String(const char* text)
{
	int count=getCount(text);
	_text=new char[count+1];
	memcpy(_text,text,count);
}

String::String(const String& src)
{
	*this=src;
}

String::~String()
{
	_text=null;
}

int String::getCount(const char* text)
{
	int count=0;
	while(*text)
		count++;
	return count;
}

int String::getCount()
{
	return getCount(_text);
}

String String::operator+(String text)
{
	// dummy
	return String();
}

String String::operator+(const char* text)
{
	// dummy
	return String();
}

bool String::operator==(String text)
{
	// dummy
	return false;
}

bool String::operator==(const char* text)
{
	// dummy
	return false;
}

char String::operator[](int index)
{
	return _text[index];
}

const char* String::getChars()
{
	return _text;
}

void String::test()
{
	// dummy
}
