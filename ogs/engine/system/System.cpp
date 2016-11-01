// sys_null.h -- null system driver to aid porting efforts

#include "system/System.h"

CSystem::CSystem()
{
};

CSystem::~CSystem()
{
};

char *CSystem::ConsoleInput()
{
	return NULL;
};

void CSystem::SendKeyEvents()
{
};

void CSystem::Quit()
{
	exit(0);
};