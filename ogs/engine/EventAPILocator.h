#pragma once

#include "common/event_api.h"

extern CEventAPINull gEventAPINull;

class CEventAPILocator
{
public:
	//static void Init(){}
	
	static void Set(event_api_t *apNewAPI)
	{
		// Reset to null if invalid or null
		if(!apNewAPI)
		{
			mpCurrentAPI = &mNullAPI;
			return;
		};
		
		mpCurrentAPI = apNewAPI;
	};
	
	static event_api_t *Get(){return mpCurrentAPI;}
private:
	static gEventAPINull mNullAPI; // TODO: lazy init
	static event_api_t *mpCurrentAPI;
};