/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

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
	
	const char *GetMapName();
private:
	char mapname[64]; // map name
	char startspot[64];

	struct model_s *worldmodel{nullptr};

	char *lightstyles[MAX_LIGHTSTYLES];

	int num_edicts; // increases towards MAX_EDICTS
	int max_edicts;
	edict_t *edicts; // can NOT be array indexed, because
	                 // edict_t is variable sized, but can
	                 // be used to reference the world ent
};