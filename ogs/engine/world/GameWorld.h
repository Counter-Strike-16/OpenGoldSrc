#pragma once

class CGameWorld
{
public:
	CGameWorld();
	~CGameWorld();
	
	void Simulate();
	
	void SetActive(bool active); // for disabling simulation if needed (server pause)
	bool IsActive();
private:
	char mapname[64]; // map name
	char startspot[64];
	
	struct model_s *worldmodel;
	
	char *lightstyles[MAX_LIGHTSTYLES];
	
	int num_edicts; // increases towards MAX_EDICTS
	int max_edicts;
	edict_t *edicts;			// can NOT be array indexed, because
									// edict_t is variable sized, but can
									// be used to reference the world ent
};