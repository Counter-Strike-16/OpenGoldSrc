#pragma once

#include "public/interface.h"

#define DEMOPLAYER_INTERFACE_VERSION "demoplayer001"

class IDemoPlayer : public IBaseInterface
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	
	virtual void ReadDemoMessage() = 0;
	
	virtual void ExecuteDemoFileCommands() = 0;
};