#pragma once

#include "vstdlib/vstdlib.h"

class CValidator;

class VSTDLIB_CLASS CStringNormalization
{
public:
	void Initialize();
	
	int Normalize(bool, char const *, char *, int);
	int Normalize(bool, wchar_t const *, wchar_t *, int);
	
	void ValidateStatics(CValidator &, char *);
	
	bool Test_BValidateStructures();
private:
	bool m_bInitialized;
};