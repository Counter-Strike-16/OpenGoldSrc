#pragma once

#include "Entity.hpp"

class CLightEntity : public CEntity
{
public:
	void SetColor(const TColor4 &aColor);
	const TColor4 &GetColor();
	
	void SetIntensity(float afIntensity){mfIntensity = afIntensity;}
	float GetIntensity(){return mfIntensity;}
private:
	// TODO: replace by color struct
	
	float mfIntensity;
	
	float mfRed;
	float mfGreen;
	float mfBlue;
};