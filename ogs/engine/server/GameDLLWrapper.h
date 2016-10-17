// This class should be responsible for interacting with game dll functions in a OOP way
// Contains both handling for DLL_FUNCTIONS and NEW_DLL_FUNCTIONS
// Conceptually, function pointers of DLL_FUNCTIONS and NEW_DLL_FUNCTIONS will always be valid
// They will be pointing to game dll functions or local null impl of this functions

class CGameDLLWrapper // CGameDLLHandle
{
public:
	CGameDLLWrapper(DLL_FUNCTIONS *apPrimaryFuncs, NEW_DLL_FUNCTIONS *apOptionalFuncs);
	~CGameDLLWrapper();
	
	void Invalidate(DLL_FUNCTIONS *apPrimaryFuncs, NEW_DLL_FUNCTIONS *apOptionalFuncs); // api provider pass pointers here; if they differs from local - update them
	
	void GameInit();
	void GameShutdown();
private:
	DLL_FUNCTIONS *mpPrimaryFuncs;
	NEW_DLL_FUNCTIONS *mpOptionalFuncs;
};