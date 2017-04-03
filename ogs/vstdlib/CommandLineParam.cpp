#include "CommandLineParam.hpp"

CCommandLineParam::CCommandLineParam(const char *asKey, const char *asValue) : msKey(asKey), msValue(asValue)
{
};

CCommandLineParam::~CCommandLineParam()
{
};

bool CCommandLineParam::Exists()
{
	return true;
};

HParam CCommandLineParam::GetHParam()
{
	return 1;
};