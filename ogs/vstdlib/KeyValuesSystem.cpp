#include "KeyValuesSystem.hpp"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//-----------------------------------------------------------------------------
// Instance singleton and expose interface to rest of code
//-----------------------------------------------------------------------------
static CKeyValuesSystem g_KeyValuesSystem;

IKeyValuesSystem *KeyValuesSystem()
{
	return &g_KeyValuesSystem;
};

// EXPOSE_SINGLE_INTERFACE(CKeyValuesSystem, IKeyValuesSystem, KEYVALUES_INTERFACE_VERSION);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CKeyValuesSystem::CKeyValuesSystem() : m_SymbolTable(0, 1024, true)
{
	m_pMemPool = NULL;
	m_iMaxKeyValuesSize = sizeof(KeyValues);
};

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CKeyValuesSystem::~CKeyValuesSystem()
{
	delete m_pMemPool;
};

//-----------------------------------------------------------------------------
// Purpose: registers the size of the KeyValues in the specified instance
//			so it can build a properly sized memory pool for the KeyValues objects
//			the sizes will usually never differ but this is for versioning safety
//-----------------------------------------------------------------------------
void CKeyValuesSystem::RegisterSizeofKeyValues(int size)
{
	if(size > m_iMaxKeyValuesSize)
		m_iMaxKeyValuesSize = size;
};

//-----------------------------------------------------------------------------
// Purpose: allocates a KeyValues object from the shared mempool
//-----------------------------------------------------------------------------
void *CKeyValuesSystem::AllocKeyValuesMemory(int size)
{
	// allocate, if we don't have one yet
	if(!m_pMemPool)
		m_pMemPool = new CMemoryPool(m_iMaxKeyValuesSize, 1024);

	return m_pMemPool->Alloc(size);
};

//-----------------------------------------------------------------------------
// Purpose: frees a KeyValues object from the shared mempool
//-----------------------------------------------------------------------------
void CKeyValuesSystem::FreeKeyValuesMemory(void *pMem)
{
	m_pMemPool->Free(pMem);
};

//-----------------------------------------------------------------------------
// Purpose: symbol table access (used for key names)
//-----------------------------------------------------------------------------
HKeySymbol CKeyValuesSystem::GetSymbolForString(const char *name)
{
	return m_SymbolTable.AddString(name);
};

//-----------------------------------------------------------------------------
// Purpose: symbol table access
//-----------------------------------------------------------------------------
const char *CKeyValuesSystem::GetStringForSymbol(HKeySymbol symbol)
{
	return m_SymbolTable.String(symbol);
};

HLocalized CKeyValuesSystem::GetLocalizedFromANSI(const char *string)
{
	return INVALID_LOCALIZED;
};

const char *CKeyValuesSystem::GetANSIFromLocalized(HLocalized l)
{
	return "";
};

void CKeyValuesSystem::AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name)
{
};

void CKeyValuesSystem::RemoveKeyValuesFromMemoryLeakList(void *pMem)
{
};