#pragma once

#include "client/ClientState.h"

class CClientState_Disconnected : public CClientState_Base
{
public:
	CClientState_Disconnected();
	~CClientState_Disconnected();
	
	// Do nothing, already disconnected
	void OnDisconnect(){}
};