#include "KeyValuesSystem.hpp"

void CKeyValuesSystem::RegisterSizeofKeyValues(int size)
{
};

void *CKeyValuesSystem::AllocKeyValuesMemory(int size)
{
	return nullptr;
};

void CKeyValuesSystem::FreeKeyValuesMemory(void *pMem)
{
};

HKeySymbol CKeyValuesSystem::GetSymbolForString(const char *name)
{
	return INVALID_KEY_SYMBOL;
};

const char *CKeyValuesSystem::GetStringForSymbol(HKeySymbol symbol)
{
	return "";
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