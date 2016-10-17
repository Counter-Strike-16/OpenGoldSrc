#pragma once

#include "common/net_api.h"

// TODO: template class locator?

extern CNetAPINull gNetAPINull;

class CNetAPILocator
{
public:
	//static void Init(){}
	
	static void Set(net_api_t *apNewAPI)
	{
		// Reset to null if invalid or null
		if(!apNewAPI)
		{
			mpCurrentAPI = &mNullAPI;
			return;
		};
		
		mpCurrentAPI = apNewAPI;
	};
	
	static net_api_t *Get(){return mpCurrentAPI;}
private:
	static gNetAPINull mNullAPI; // TODO: lazy init
	static net_api_t *mpCurrentAPI;
};