#include "World.hpp"

void World::Init()
{
	if(?)
	{
		printf("World::Init: Not enough memory for caches. Reduce -cachesize.");
	};
	
	printf("World module initialized.");
};

void World::Shutdown()
{
	printf("World module shutdown.");
};

void World::SetMaxClients(int num)
{
	if(num > MAX_CLIENTS) // ?
		printf("WARNING! World::SetMaxClients: Bad maxclients (%u)", num);
	
	mnMaxClients = num;
};

void World::UpdatePlayer(int playernum)
{
	if(playernum > MAX_CLIENTS)
		printf("World::UpdatePlayer: player number %i > MAX_CLIENTS", playernum);
};