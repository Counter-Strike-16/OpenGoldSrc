#pragma once

/*
game world is a sort of proxy for local client world

class IWorld // shared interface between client and server. on the server - send messages to clients. on the clients - perform operations (spawn entities, particles, etc)

class CGameWorld // aka server world; global state of world, authoritative to other clients

class CClientWorld/CLocalWorld // local client world

mb they should be merged into single class? but then there are should be some mech to differentiate client from server (bool isclient or something)

void CGameWorld::SpawnParticle(...)
{
	// send message to all clients except local?
	
	//
	
	LocalWorld->SpawnParticle(); // == nullptr for dedicated mode
	
	// or
	
	if(LocalClient->IsActive())
	
	// draw particles, play sounds, animate them and etc
};

or

ClientProtocol::ReceiveParticle(...)
{
	// read data
	
	mpLocalWorld->SpawnParticle();
};

in both cases, client protocol should handle that message
in the first variant there should be some sort of prevention from double exec (like for prediction system)
*/

class CGameWorld : public IWorld
{
public:
	CGameWorld();
	~CGameWorld();
	
	void Update();
	
	void SetActive(bool active); // for disabling simulation if needed (server pause)
	bool IsActive();
	
	void CreateEntity();
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