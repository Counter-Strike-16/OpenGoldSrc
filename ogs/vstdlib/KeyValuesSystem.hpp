//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#pragma once

#include "vstdlib/IKeyValuesSystem.h"
#include "UtlSymbol.h"

class CMemoryPool;

//-----------------------------------------------------------------------------
// Purpose: Central storage point for KeyValues memory and symbols
//-----------------------------------------------------------------------------
class CKeyValuesSystem : public IKeyValuesSystem
{
public:
	CKeyValuesSystem();
	~CKeyValuesSystem();
	
	void RegisterSizeofKeyValues(int size);
	
	void *AllocKeyValuesMemory(int size);
	void FreeKeyValuesMemory(void *pMem);
	
	HKeySymbol GetSymbolForString(const char *name);
	const char *GetStringForSymbol(HKeySymbol symbol);
	
	HLocalized GetLocalizedFromANSI(const char *string);
	const char *GetANSIFromLocalized(HLocalized l);
	
	void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name);
	void RemoveKeyValuesFromMemoryLeakList(void *pMem);
private:
	CMemoryPool *m_pMemPool;
	CUtlSymbolTable m_SymbolTable;

	int m_iMaxKeyValuesSize;
};