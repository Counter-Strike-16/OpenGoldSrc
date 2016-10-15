#pragma once

class IAbstractClient
{
public:
	virtual ~IAbstractClient(){};
	
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	
	virtual void Disconnect() = 0;
	
	virtual void Frame() = 0;
};