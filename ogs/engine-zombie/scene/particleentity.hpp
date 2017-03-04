#pragma once

#include "Entity.hpp"

class CParticleEntity : public CEntity
{
public:
	void SetColor(int anColor){mnColor = anColor;}
	int GetColor(){return mnColor;}
	
	void SetAlpha(float afAlpha){mfAlpha = afAlpha;}
	float GetAlpha(){return mfAlpha;}
private:
	float mfAlpha;
	
	int mnColor;
};