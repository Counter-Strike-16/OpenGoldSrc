// StringPool.h - string pool class used mostly for edicts

#pragma once

#include "osconfig.h"
#include "static_map.h"

extern char *Ed_StrPool_Alloc(const char* str);
extern void Ed_StrPool_Reset();
extern void Ed_StrPool_Init();

class CStringPool
{
public:
	CStringPool();
	~CStringPool();
	
	void Init();
	void Reset();
	
	char *Alloc(const char *str);
};