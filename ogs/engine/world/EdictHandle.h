// Wrapper class around edict_t to interact with it in OOP way

#pragma once

class CEdictHandle
{
public:
	CEdictHandle(int id, edict_t *apEdict);
	~CEdictHandle();
	
	void Clear();
	
	void SaveSpawnParms();
	
	int GetID(); // IndexOfEdict
	entvars_t *GetEntVars();
	void *GetModelPtr();
	
	edict_t *operator->() const {return mpEdict;} // hm...
	
	edict_t& operator&(){return *mpEdict;}
	edict_t* operator*() const{return mpEdict;}
	
	void SetEdict(edict_t *apEdict);
	edict_t *GetEdict() const {return mpEdict;}
private:
	edict_t *mpEdict;
};