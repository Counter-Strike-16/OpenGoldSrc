#pragma once

#include "IWorld.hpp"

class World : public IWorld
{
public:
	void Init();
	void Shutdown();
	
	void UpdatePlayer(int playernum);
	
	void SetMaxClients(int num);
	
	void SetBaselineEntity();
	void SetInstancedBaselineEntity();
	
	void SetLightStyle();
	
	void SetState();
	
	void WriteBaseline();
	void WriteDeltaDescriptions();
	
	void AddFrame();
};