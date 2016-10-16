#include "world/GameWorld.h"

CGameWorld::CGameWorld()
{
};

CGameWorld::~CGameWorld()
{
};

void CGameWorld::Init()
{
	Con_DPrintf("World module initialized.\n");
};

void CGameWorld::Shutdown()
{
	Con_DPrintf("World module shutdown.\n");
};