#pragma once

class CLocalClient;

class IClientState
{
public:
	IClientState(CLocalClient *apClient) : mpClient(apClient){}
	virtual ~IClientState(){};
	
	virtual void OnFrame(){}
	
	virtual void OnDisconnect(){}
protected:
	CLocalClient *mpClient;
};