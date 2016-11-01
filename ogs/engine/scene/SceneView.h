/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

// view.h

#pragma once

extern	cvar_t		v_gamma;
extern	cvar_t		lcd_x;
#ifdef GLQUAKE
extern float v_blend[4];
#endif

class CView
{
public:
	CView();
	~CView();
	
	void Init();
	
	void StartPitchDrift();
	void StopPitchDrift();
	
	void RenderView();
	void UpdatePalette();
	void Register();
	void ParseDamage();
	void SetContentsColor(int contents);
	void CalcBlend();
	
	float CalcRoll(vec3_t angles, vec3_t velocity);
};