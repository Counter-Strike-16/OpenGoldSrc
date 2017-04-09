#pragma once

#include "public/interface.h"

#define WORLD_INTERFACE_VERSION "world001"

class IWorld : public IBaseInterface
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
};