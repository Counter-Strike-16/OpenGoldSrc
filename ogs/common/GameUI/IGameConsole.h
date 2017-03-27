#ifndef IGAMECONSOLE_H
#define IGAMECONSOLE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier1/interface.h"

class Color;

//-----------------------------------------------------------------------------
// Purpose: interface to game/dev console
// Phil: Only one difference between Source and GoldSrc versions.
//-----------------------------------------------------------------------------
class IGameConsole : public IBaseInterface
{
public:
	// activates the console, makes it visible and brings it to the foreground
	virtual void Activate() = 0;
	
	virtual void Initialize() = 0;
	
	// hides the console
	virtual void Hide() = 0;
	
	// clears the console
	virtual void Clear() = 0;
	
	// return true if the console has focus
	virtual bool IsConsoleVisible() = 0;
	
	// prints a message to the console
	virtual void Printf(const char *format, ...) = 0;
	
	// prints a debug message to the console
	virtual void DPrintf(const char *format, ...) = 0;
	
	// prints a debug message to the console
	// Phil: This function is not present in GoldSrc
	//virtual void ColorPrintf(Color &clr) = 0; // ( Color& clr, const char *format, ...)
	
	virtual void SetParent(int parent) = 0; // int = VPANEL
};

#define GAMECONSOLE_INTERFACE_VERSION "GameConsole003"

#endif // IGAMECONSOLE_H