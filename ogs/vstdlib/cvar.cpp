#include "vstdlib/cvar.h"

CreateInterfaceFn gICVarFactory = nullptr;

CreateInterfaceFn VStdLib_GetICVarFactory()
{
	return gICVarFactory;
};