#include "vstdlib/osversion.h"

const char *GetNameFromOSType(EOSType eOSType)
{
	switch(eOSType)
	{
	};
	
	return "";
};

const char *GetOSDetailString(char *pchOutBuf, int cchOutBuf)
{
	return "";
};

EOSType GetOSType()
{
	return 0;
};

bool OSTypesAreCompatible(EOSType eOSTypeDetected, EOSType eOSTypeRequired)
{
	return false;
};

const char *GetPlatformName(bool *pbIs64Bit)
{
	if(pbIs64Bit)
	{
		*pbIs64Bit = false;
		
		if(Is64Bit())
			*pbIs64Bit = true;
	};
	
	return "";
};