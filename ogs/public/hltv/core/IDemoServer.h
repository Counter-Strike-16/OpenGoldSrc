#pragma once

#include "public/interface.h"

#define SERVER_INTERFACE_VERSION "server001"

class IServer : public IBaseInterface
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
};