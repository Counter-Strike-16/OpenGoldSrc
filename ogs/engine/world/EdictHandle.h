// Wrapper class around edict_t to interact with it in OOP way

#pragma once

/*
class CEdict : edict_t
{
	void SetSize(...){Edict_SetSize(this, ...)}
	void SetModel(...);
};

class CEdictHandle // reference to edict object; semantically it would work as EHANDLE in hlsdk
{
CEdictHandle(edict_t *aggr);
edict_t *privedict;
};
*/

class CEdictHandle
{
public:
	CEdictHandle(int id, edict_t *apEdict) : mnID(id), mpHandlingEdict(apEdict){}
	~CEdictHandle();
	
	void Clear();
	
	void SaveSpawnParms();
	
	void InitGameDLLFields();
	void ReleaseGameDLLFields();
	
	int GetID(){return mnID;} // IndexOfEdict
	entvars_t *GetEntVars();
	void *GetModelPtr();
	
	edict_t *operator->() const {return mpHandlingEdict;}
	
	//edict_t *operator&(){return *mpHandlingEdict;} // potential UB?
	
	const edict_t &operator*() const {return mpHandlingEdict;}
	edict_t &operator*() const {return mpHandlingEdict;}
	
	void SetHandlingEdict(edict_t *apHandlingEdict){mpHandlingEdict = apHandlingEdict;}
	edict_t *GetHandlingEdict() const {return mpHandlingEdict;}
private:
	edict_t *mpHandlingEdict;
	int mnID;
};