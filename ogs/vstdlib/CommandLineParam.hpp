#pragma once

#include "vstdlib/vstdlib.h"

typedef int HParam;
#define INVALID_PARAM (-1)

class VSTDLIB_CLASS CCommandLineParam
{
public:
	CCommandLineParam(const char *asKey, const char *asValue);
	~CCommandLineParam();
	
	//CCommandLineParam &operator=(const CCommandLineParam&);
	
	bool Exists();
	
	HParam GetHParam();
private:
	const char *msKey;
	const char *msValue;
};