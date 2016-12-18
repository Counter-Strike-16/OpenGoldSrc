#ifndef IGAMECONSOLE_H
#define IGAMECONSOLE_H

#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

class Color;

class IGameConsole : public IBaseInterface
{
public:
	virtual void Activate() = 0;
	virtual void Initialize() = 0;
	virtual void Hide() = 0;
	virtual void Clear() = 0;
	virtual bool IsConsoleVisible() = 0;
	
	virtual void Printf(const char *format, ...) = 0;
	virtual void DPrintf(const char *format, ...) = 0;
	
	virtual void ColorPrintf(Color &clr) = 0;
	
	virtual void SetParent(int parent) = 0;
};

#define GAMECONSOLE_INTERFACE_VERSION "GameConsole003"

#endif // IGAMECONSOLE_H