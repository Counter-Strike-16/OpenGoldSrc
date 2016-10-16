#pragma once

#include "common/demo_api.h"

extern CDemoAPINull gDemoAPINull;

class CDemoAPILocator
{
public:
	//static void Init(){}
	
	static void Set(demo_api_t *apNewAPI)
	{
		// Reset to null if invalid or null
		if(!apNewAPI)
		{
			mpCurrentAPI = &mNullAPI;
			return;
		};
		
		mpCurrentAPI = apNewAPI;
	};
	
	static demo_api_t *Get(){return mpCurrentAPI;}
private:
	static gDemoAPINull mNullAPI; // TODO: lazy init
	static demo_api_t *mpCurrentAPI;
};