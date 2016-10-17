// ClientDLLFuncsProvider.h - provider class for client dll functions. Should provide null impl by default
// Provides real funcs from client dll if it was successfully loaded

#pragma once

class CClientDLLFuncsProvider
{
public:
	//static void Init(){}
	
	static void Set(cldll_export_t *apNewAPI)
	{
		// Reset to null if invalid or null
		if(!apNewAPI)
		{
			mpCurrentAPI = &mNullAPI;
			return;
		};
		
		mpCurrentAPI = apNewAPI;
	};
	
	static DLL_FUNCTIONS *Get(){return mpCurrentAPI;}
private:
	static gEventAPINull mNullAPI; // TODO: lazy init
	static event_api_t *mpCurrentAPI;
};