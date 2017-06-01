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
	
	/*virtual*/ void RegisterSizeofKeyValues(int size);
	
	/*virtual*/ void *AllocKeyValuesMemory(int size);
	/*virtual*/ void FreeKeyValuesMemory(void *pMem);
	
	/*virtual*/ HKeySymbol GetSymbolForString(const char *name);
	/*virtual*/ const char *GetStringForSymbol(HKeySymbol symbol);
	
	/*virtual*/ HLocalized GetLocalizedFromANSI(const char *string);
	/*virtual*/ const char *GetANSIFromLocalized(HLocalized l);
	
	/*virtual*/ void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name);
	/*virtual*/ void RemoveKeyValuesFromMemoryLeakList(void *pMem);
private:
	CMemoryPool *m_pMemPool;
	CUtlSymbolTable m_SymbolTable;

	int m_iMaxKeyValuesSize;
};