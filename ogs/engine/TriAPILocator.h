#pragma once

#include "common/triangleapi.h"

extern CTriAPINull gTriAPINull;

class CTriAPILocator
{
public:
	//static void Init(){}
	
	static void Set(triangleapi_t *apNewAPI)
	{
		// Reset to null if invalid or null
		if(!apNewAPI)
		{
			mpCurrentAPI = &mNullAPI;
			return;
		};
		
		mpCurrentAPI = apNewAPI;
	};
	
	static triangleapi_t *Get(){return mpCurrentAPI;}
private:
	static gTriAPINull mNullAPI; // TODO: lazy init
	static triangleapi_t *mpCurrentAPI;
};