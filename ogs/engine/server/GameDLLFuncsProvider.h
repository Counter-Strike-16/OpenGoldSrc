// GameDLLFuncsProvider.h - provider class for game dll functions. Should provide null impl by default
// Provides real funcs from game dll if it was successfully loaded

#pragma once

extern CGameDLLFuncsNull gGameDLLFuncsNull;

class CGameDLLFuncsProvider
{
public:
	//static void Init(){}
	
	static void Set(DLL_FUNCTIONS *apNewAPI)
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
	static gGameDLLFuncsNull mNullAPI; // TODO: lazy init
	static event_api_t *mpCurrentAPI;
};