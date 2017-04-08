#include "precompiled.hpp"
#include "world/pr_dlls.hpp"

DISPATCHFUNCTION GetDispatch(char *pname)
{
	DISPATCHFUNCTION pDispatch;

	for(int i = 0; i < g_iextdllMac; i++)
	{
		pDispatch = (DISPATCHFUNCTION)GetProcAddress((HMODULE)g_rgextdll[i].lDLLHandle, pname);
		if(pDispatch)
			return pDispatch;
	};

	return NULL;
};

ENTITYINIT GetEntityInit(char *pClassName)
{
	return (ENTITYINIT)GetDispatch(pClassName);
};

FIELDIOFUNCTION GetIOFunction(char *pName)
{
	return (FIELDIOFUNCTION)GetDispatch(pName);
};