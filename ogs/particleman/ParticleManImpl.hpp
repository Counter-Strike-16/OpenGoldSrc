/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file

#pragma once

#include "public/particleman/particleman.h"

class CParticleMan : public IParticleMan
{
public:
	CParticleMan();
	~CParticleMan();
	
	void SetUp(cl_enginefunc_t *apEngineFuncs);
	
	void Update();
	
	void SetVariables(float afGravity, Vector avViewAngles);
	
	void ResetParticles();
	
	void ApplyForce(Vector avOrigin, Vector avDirection, float afRadius, float afStrength, float afDuration);
	
	void AddCustomParticleClassSize(unsigned long anSize);
	
	CBaseParticle *CreateParticle(Vector avOrg, Vector avNormal, model_s *apSprite, float afSize, float afBrightness, const char *classname);

	char *RequestNewMemBlock(int anSize);

	void CoreInitializeSprite(CCoreTriangleEffect *pParticle, Vector org, Vector normal, model_s *sprite, float size, float brightness);
	void CoreThink(CCoreTriangleEffect *pParticle, float time);
	void CoreDraw(CCoreTriangleEffect *pParticle);
	void CoreAnimate(CCoreTriangleEffect *pParticle, float time);
	void CoreAnimateAndDie(CCoreTriangleEffect *pParticle, float time);
	void CoreExpand(CCoreTriangleEffect *pParticle, float time);
	void CoreContract(CCoreTriangleEffect *pParticle, float time);
	void CoreFade(CCoreTriangleEffect *pParticle, float time);
	void CoreSpin(CCoreTriangleEffect *pParticle, float time);
	void CoreCalculateVelocity(CCoreTriangleEffect *pParticle, float time);
	void CoreCheckCollision(CCoreTriangleEffect *pParticle, float time);
	void CoreTouch(CCoreTriangleEffect *pParticle, Vector pos, Vector normal, int index);
	void CoreDie(CCoreTriangleEffect *pParticle);
	void CoreForce(CCoreTriangleEffect *pParticle);
	bool CoreCheckVisibility(CCoreTriangleEffect *pParticle);
	
	void SetRender(int iRender);
};