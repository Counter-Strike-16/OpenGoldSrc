#pragma once

class CScene
{
public:
	CScene();
	~CScene();
	
	dlight_t *AllocDlight(int key);
	void DecayLights();
	
	void InitTEnts();
	void ClearTEnts();
	void UpdateTEnts();
};