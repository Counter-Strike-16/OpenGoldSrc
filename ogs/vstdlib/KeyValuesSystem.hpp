#pragma once

#include "vstdlib/IKeyValuesSystem.h"

class CKeyValuesSystem : public IKeyValuesSystem
{
public:
	void RegisterSizeofKeyValues(int size);
	
	void *AllocKeyValuesMemory(int size);
	void FreeKeyValuesMemory(void *pMem);
	
	HKeySymbol GetSymbolForString(const char *name);
	const char *GetStringForSymbol(HKeySymbol symbol);
	
	HLocalized GetLocalizedFromANSI(const char *string);
	const char *GetANSIFromLocalized(HLocalized l);
	
	void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name);
	void RemoveKeyValuesFromMemoryLeakList(void *pMem);
};