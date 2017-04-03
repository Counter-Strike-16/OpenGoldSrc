#include "stringnormalize.hpp"
#include "tier0/validator.h"

void CStringNormalization::Initialize()
{
};

int CStringNormalization::Normalize(bool, char const *, char *, int)
{
	return 1;
};

int CStringNormalization::Normalize(bool, wchar_t const *, wchar_t *, int)
{
	return 1;
};

void CStringNormalization::ValidateStatics(CValidator &, char *)
{
};

bool CStringNormalization::Test_BValidateStructures()
{
	return true;
};

//private:
//bool m_bInitialized;