#pragma once

class CEntity
{
public:
	void SetPosition(const vec3_t &avPos){mvPos = avPos;}
	const vec3_t &GetPosition(){return mvPos;}
private:
	vec3_t mvPos;
};