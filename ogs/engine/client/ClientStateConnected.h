#pragma once

#include "ClientStateBase.h"

class CClientState_Connected : public CClientState_Base
{
public:
	CClientState_Connected(CLocalClient *apClient) : IClientState(apClient){}
	~CClientState_Connected();
	
	void OnDisconnect()
	{
		if(cls.demorecording)
			CL_Stop_f();
		
		Con_DPrintf ("Sending clc_disconnect\n");
		SZ_Clear (&cls.message);
		MSG_WriteByte (&cls.message, clc_disconnect);
		NET_SendUnreliableMessage (cls.netcon, &cls.message);
		SZ_Clear (&cls.message);
		NET_Close(cls.netcon);
		
		mpClient->SetState(ca_disconnected);
		
		// Local client cause local server to shutdown. Hm...
		// Should this thing be placed somewhere in host class?
		// Or send event?
		if(sv->IsActive()) // check for local server, bad place i think
			mpHost->ShutdownServer(false); // this construction will shutdown the local server
	};
};