// Wrapper class around edict_t to interact with it in OOP way

#pragma once

class CEdictHandle
{
public:
	CEdictHandle(int id, edict_t *apEdict) : mnID(id), mpHandlingEdict(apEdict){}
	~CEdictHandle();
	
	void Clear();
	
	void SaveSpawnParms();
	
	int GetID(){return mnID;} // IndexOfEdict
	entvars_t *GetEntVars();
	void *GetModelPtr();
	
	edict_t &operator->() const {return mpHandlingEdict;} // hm...
	
	edict_t &operator&(){return *mpHandlingEdict;}
	edict_t *operator*() const {return mpHandlingEdict;}
	
	void SetHandlingEdict(edict_t *apHandlingEdict){mpHandlingEdict = apHandlingEdict;}
	edict_t *GetHandlingEdict() const {return mpHandlingEdict;}
private:
	edict_t *mpHandlingEdict;
	int mnID;
};