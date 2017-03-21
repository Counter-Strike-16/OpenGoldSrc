/// @file

#pragma once

struct IWorld;

class CScene
{
public:
	CScene();
	~CScene();
	
	void Init();
	
	void Update(float afTimeStep);
	
	IWorld *CreateWorld();
private:
	IWorld *mpWorld;
};