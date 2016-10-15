#pragma once

#include "IClientState.h"

class CClientState_Base : public IClientState
{
public:
	CClientState_Base();
	~CClientState_Base();
	
	void OnDisconnect()
	{
		byte sFinal[10];
		
		sFinal[0] = clc_stringcmd;
		strcpy(sFinal + 1, "drop");
		Netchan_Transmit (&cls.netchan, 6, sFinal);
		Netchan_Transmit (&cls.netchan, 6, sFinal);
		Netchan_Transmit (&cls.netchan, 6, sFinal);
		
		cls.demoplayback = cls.demorecording = cls.timedemo = false;
	};
};