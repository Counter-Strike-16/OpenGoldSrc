#pragma once

class CPhysics
{
public:
	CPhysics();
	~CPhysics();
	
	void Init();
	void Shutdown();
	
	void Frame();
	
	bool EntityCheckBottom(edict_t *ent);
	bool EntityMoveStep(edict_t *ent, vec3_t move, qboolean relink);
	
	void CheckVelocity(edict_t *ent);
	void AddGravity(edict_t *ent, float scale);
	bool RunThink(edict_t *ent);
	void Physics_Toss(edict_t *ent);
	
	void EntityImpact(edict_t *e1, edict_t *e2);
};