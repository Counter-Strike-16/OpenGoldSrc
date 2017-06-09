/*
 *	This file is part of OGS Engine
 *	Copyright (C) 1996-1997 Id Software, Inc.
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
/// @brief player eye positioning

#include "precompiled.hpp"
//#include "commondef.hpp"
#include "graphics/view.hpp"
//#include "server/server.hpp"

/*

The view is allowed to move slightly from it's true position for bobbing,
but if it exceeds 8 pixels linear distance (spherical, not box), the list of
entities sent from the server may not include everything in the pvs, especially
when crossing a water boudnary.

*/

cvar_t lcd_x = { "lcd_x", "0" }; // FIXME: make this work sometime...

cvar_t cl_rollspeed = { "cl_rollspeed", "200" };
cvar_t cl_rollangle = { "cl_rollangle", "2.0" };

cvar_t v_kicktime = { "v_kicktime", "0.5", false };
cvar_t v_kickroll = { "v_kickroll", "0.6", false };
cvar_t v_kickpitch = { "v_kickpitch", "0.6", false };

cvar_t v_idlescale = { "v_idlescale", "0", false };

cvar_t crosshair = { "crosshair", "0", true };
cvar_t crosshaircolor = { "crosshaircolor", "79", true };

cvar_t cl_crossx = { "cl_crossx", "0", true };
cvar_t cl_crossy = { "cl_crossy", "0", true };

#ifdef GLQUAKE
cvar_t gl_cshiftpercent = { "gl_cshiftpercent", "100", false };
#endif

cvar_t v_contentblend = { "v_contentblend", "1", false };

float v_dmg_time, v_dmg_roll, v_dmg_pitch;

extern int in_forward, in_forward2, in_back;

frame_t *view_frame;
player_state_t *view_message;

//=============================================================================

/*
==============================================================================

PALETTE FLASHES

==============================================================================
*/

cshift_t cshift_empty = { { 130, 80, 50 }, 0 };
cshift_t cshift_water = { { 130, 80, 50 }, 128 };
cshift_t cshift_slime = { { 0, 25, 5 }, 150 };
cshift_t cshift_lava = { { 255, 80, 0 }, 150 };

cvar_t v_gamma = { "gamma", "1", true };

byte gammatable[256]; // palette is sent through this

#ifdef GLQUAKE
byte ramps[3][256];
float v_blend[4]; // rgba 0.0 - 1.0
#endif            // GLQUAKE

void BuildGammaTable(float g)
{
	int i, inf;

	if(g == 1.0)
	{
		for(i = 0; i < 256; i++)
			gammatable[i] = i;
		return;
	};

	for(i = 0; i < 256; i++)
	{
		inf = 255 * pow((i + 0.5) / 255.5, g) + 0.5;
		if(inf < 0)
			inf = 0;
		if(inf > 255)
			inf = 255;
		gammatable[i] = inf;
	};
};

/*
=================
V_CheckGamma
=================
*/
qboolean V_CheckGamma()
{
	static float oldgammavalue;

	if(v_gamma.value == oldgammavalue)
		return false;

	oldgammavalue = v_gamma.value;

	BuildGammaTable(v_gamma.value);
	vid.recalc_refdef = 1; // force a surface cache flush

	return true;
};

/*
===============
V_ParseDamage
===============
*/
void V_ParseDamage()
{
	// Deprecated
	// Moved to client dll (CHud::MsgFunc_Damage) and unused

	vec3_t from;
	vec3_t forward, right, up;

	int armor = 0;
	int blood = 0;
	
	for(int i = 0; i < 3; i++)
		from[i] = 0;

	float count = blood * 0.5 + armor * 0.5;
	
	if(count < 10)
		count = 10;

	cl.faceanimtime = cl.time + 0.2; // but sbar face into pain frame

	//
	// calculate view angle kicks
	//
	VectorSubtract(from, cl.simorg, from);
	VectorNormalize(from);

	AngleVectors(cl.simangles, forward, right, up);

	float side = DotProduct(from, right);
	v_dmg_roll = count * side * v_kickroll.value;

	side = DotProduct(from, forward);
	v_dmg_pitch = count * side * v_kickpitch.value;

	v_dmg_time = v_kicktime.value;
}

/*
==================
V_cshift_f
==================
*/
void V_cshift_f()
{
	cshift_empty.destcolor[0] = Q_atoi(Cmd_Argv(1));
	cshift_empty.destcolor[1] = Q_atoi(Cmd_Argv(2));
	cshift_empty.destcolor[2] = Q_atoi(Cmd_Argv(3));
	cshift_empty.percent = Q_atoi(Cmd_Argv(4));
}

/*
=============
V_Viewpos_f
=============
*/
void V_Viewpos_f ()
{
	Con_Printf ("(%i %i %i) : %i\n", (int)cl.refdef.vieworg[0],
		(int)cl.refdef.vieworg[1], (int)cl.refdef.vieworg[2], 
		(int)cl.refdef.viewangles[YAW]);
}

/*
=============
V_SetContentsColor

Underwater, lava, etc each has a color shift
=============
*/
void V_SetContentsColor(int contents)
{
	if(!v_contentblend.value)
		return;

	switch(contents)
	{
	case CONTENTS_EMPTY:
		break;
	case CONTENTS_LAVA:
		break;
	case CONTENTS_SOLID:
	case CONTENTS_SLIME:
		break;
	default:
	}
}

/*
=============
V_CalcPowerupCshift
=============
*/
void V_CalcPowerupCshift()
{
	if(cl.stats[STAT_ITEMS] & IT_QUAD)
	{
	}
	else if(cl.stats[STAT_ITEMS] & IT_SUIT)
	{
	}
	else if(cl.stats[STAT_ITEMS] & IT_INVISIBILITY)
	{
	}
	else if(cl.stats[STAT_ITEMS] & IT_INVULNERABILITY)
	{
	}
}

/*
=============
V_CalcBlend
=============
*/
#ifdef GLQUAKE
void V_CalcBlend()
{
	float r, g, b, a, a2;
	int j;

	r = 0;
	g = 0;
	b = 0;
	a = 0;

	for(j = 0; j < NUM_CSHIFTS; j++)
	{
		if(!gl_cshiftpercent.value)
			continue;

		a2 = ((cl.cshifts[j].percent * gl_cshiftpercent.value) / 100.0) / 255.0;

		//		a2 = (cl.cshifts[j].percent/2)/255.0;
		if(!a2)
			continue;
		a = a + a2 * (1 - a);
		// Con_Printf ("j:%i a:%f\n", j, a);
		a2 = a2 / a;
		r = r * (1 - a2) + cl.cshifts[j].destcolor[0] * a2;
		g = g * (1 - a2) + cl.cshifts[j].destcolor[1] * a2;
		b = b * (1 - a2) + cl.cshifts[j].destcolor[2] * a2;
	}

	v_blend[0] = r / 255.0;
	v_blend[1] = g / 255.0;
	v_blend[2] = b / 255.0;
	v_blend[3] = a;
	if(v_blend[3] > 1)
		v_blend[3] = 1;
	if(v_blend[3] < 0)
		v_blend[3] = 0;
}
#endif

/*
=============
V_UpdatePalette
=============
*/
#ifdef GLQUAKE
void V_UpdatePalette()
{
	int i, j;
	qboolean bnew;
	byte *basepal, *newpal;
	byte pal[768];
	float r, g, b, a;
	int ir, ig, ib;
	qboolean force;

	V_CalcPowerupCshift();

	bnew = false;

	for(i = 0; i < NUM_CSHIFTS; i++)
	{
		if(cl.cshifts[i].percent != cl.prev_cshifts[i].percent)
		{
			bnew = true;
			cl.prev_cshifts[i].percent = cl.cshifts[i].percent;
		}
		for(j = 0; j < 3; j++)
			if(cl.cshifts[i].destcolor[j] != cl.prev_cshifts[i].destcolor[j])
			{
				bnew = true;
				cl.prev_cshifts[i].destcolor[j] = cl.cshifts[i].destcolor[j];
			}
	}

	// drop the damage value
	cl.cshifts[CSHIFT_DAMAGE].percent -= host_frametime * 150;
	if(cl.cshifts[CSHIFT_DAMAGE].percent <= 0)
		cl.cshifts[CSHIFT_DAMAGE].percent = 0;

	// drop the bonus value
	cl.cshifts[CSHIFT_BONUS].percent -= host_frametime * 100;
	if(cl.cshifts[CSHIFT_BONUS].percent <= 0)
		cl.cshifts[CSHIFT_BONUS].percent = 0;

	force = V_CheckGamma();
	if(!bnew && !force)
		return;

	V_CalcBlend();

	// Con_Printf("b: %4.2f %4.2f %4.2f %4.6f\n", v_blend[0],	v_blend[1],
	// v_blend[2],	v_blend[3]);

	a = v_blend[3];
	r = 255 * v_blend[0] * a;
	g = 255 * v_blend[1] * a;
	b = 255 * v_blend[2] * a;

	a = 1 - a;
	for(i = 0; i < 256; i++)
	{
		ir = i * a + r;
		ig = i * a + g;
		ib = i * a + b;
		if(ir > 255)
			ir = 255;
		if(ig > 255)
			ig = 255;
		if(ib > 255)
			ib = 255;

		ramps[0][i] = gammatable[ir];
		ramps[1][i] = gammatable[ig];
		ramps[2][i] = gammatable[ib];
	}

	basepal = host_basepal;
	newpal = pal;

	for(i = 0; i < 256; i++)
	{
		ir = basepal[0];
		ig = basepal[1];
		ib = basepal[2];
		basepal += 3;

		newpal[0] = ramps[0][ir];
		newpal[1] = ramps[1][ig];
		newpal[2] = ramps[2][ib];
		newpal += 3;
	}

	VID_ShiftPalette(pal);
}
#else // !GLQUAKE
/*
=============
V_UpdatePalette
=============
*/
void V_UpdatePalette()
{
	int i, j;
	qboolean bnew;
	byte *basepal, *newpal;
	byte pal[768];
	int r, g, b;
	qboolean force;

	V_CalcPowerupCshift();

	bnew = false;

	for(i = 0; i < NUM_CSHIFTS; i++)
	{
		if(cl.cshifts[i].percent != cl.prev_cshifts[i].percent)
		{
			bnew = true;
			cl.prev_cshifts[i].percent = cl.cshifts[i].percent;
		}
		for(j = 0; j < 3; j++)
			if(cl.cshifts[i].destcolor[j] != cl.prev_cshifts[i].destcolor[j])
			{
				bnew = true;
				cl.prev_cshifts[i].destcolor[j] = cl.cshifts[i].destcolor[j];
			}
	}

	// drop the damage value
	cl.cshifts[CSHIFT_DAMAGE].percent -= host_frametime * 150;
	if(cl.cshifts[CSHIFT_DAMAGE].percent <= 0)
		cl.cshifts[CSHIFT_DAMAGE].percent = 0;

	// drop the bonus value
	cl.cshifts[CSHIFT_BONUS].percent -= host_frametime * 100;
	if(cl.cshifts[CSHIFT_BONUS].percent <= 0)
		cl.cshifts[CSHIFT_BONUS].percent = 0;

	force = V_CheckGamma();
	if(!bnew && !force)
		return;

	basepal = host_basepal;
	newpal = pal;

	for(i = 0; i < 256; i++)
	{
		r = basepal[0];
		g = basepal[1];
		b = basepal[2];
		basepal += 3;

		for(j = 0; j < NUM_CSHIFTS; j++)
		{
			r += (cl.cshifts[j].percent * (cl.cshifts[j].destcolor[0] - r)) >> 8;
			g += (cl.cshifts[j].percent * (cl.cshifts[j].destcolor[1] - g)) >> 8;
			b += (cl.cshifts[j].percent * (cl.cshifts[j].destcolor[2] - b)) >> 8;
		}

		newpal[0] = gammatable[r];
		newpal[1] = gammatable[g];
		newpal[2] = gammatable[b];
		newpal += 3;
	}

	VID_ShiftPalette(pal);
}

#endif // !GLQUAKE

/*
==============================================================================

                                                VIEW RENDERING

==============================================================================
*/

float angledelta(float a)
{
	a = anglemod(a);
	if(a > 180)
		a -= 360;
	return a;
}

/*
==============
V_BoundOffsets
==============
*/
void V_BoundOffsets()
{
	// absolutely bound refresh reletive to entity clipping hull
	// so the view can never be inside a solid wall

	if(r_refdef.vieworg[0] < cl.simorg[0] - 14)
		r_refdef.vieworg[0] = cl.simorg[0] - 14;
	else if(r_refdef.vieworg[0] > cl.simorg[0] + 14)
		r_refdef.vieworg[0] = cl.simorg[0] + 14;
	if(r_refdef.vieworg[1] < cl.simorg[1] - 14)
		r_refdef.vieworg[1] = cl.simorg[1] - 14;
	else if(r_refdef.vieworg[1] > cl.simorg[1] + 14)
		r_refdef.vieworg[1] = cl.simorg[1] + 14;
	if(r_refdef.vieworg[2] < cl.simorg[2] - 22)
		r_refdef.vieworg[2] = cl.simorg[2] - 22;
	else if(r_refdef.vieworg[2] > cl.simorg[2] + 30)
		r_refdef.vieworg[2] = cl.simorg[2] + 30;
}

/*
==================
V_RenderView

The player's clipping box goes from (-16 -16 -24) to (16 16 32) from
the entity origin, so any view position inside that will be valid
==================
*/
extern vrect_t scr_vrect;

void V_RenderView()
{
	//	if (cl.simangles[ROLL])
	//		Sys_Error ("cl.simangles[ROLL]");	// DEBUG
	cl.simangles[ROLL] = 0; // FIXME @@@

	if(cls.state != ca_active)
		return;

	view_frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	view_message = &view_frame->playerstate[cl.playernum];

	DropPunchAngle();
	
	if(cl.intermission)
	{
		// intermission / finale rendering
		V_CalcIntermissionRefdef();
	}
	else
		V_CalcRefdef();

	R_PushDlights();
	R_RenderView();

#ifndef GLQUAKE
	if(crosshair.value)
		Draw_Crosshair();
#endif
}

//============================================================================

// Looks like below are only needed funcs

/*
=============
V_Init
=============
*/
void V_Init()
{
	//Cmd_AddCommand ("gun_next", V_Gun_Next_f);
	//Cmd_AddCommand ("gun_prev", V_Gun_Prev_f);
	//Cmd_AddCommand ("gun_model", V_Gun_Model_f);
	
	Cmd_AddCommand ("viewpos", V_Viewpos_f);
	
	Cmd_AddCommand("v_cshift", V_cshift_f);
	
	/*
	crosshair = Cvar_Get ("crosshair", "0", CVAR_ARCHIVE);

	cl_testblend = Cvar_Get ("cl_testblend", "0", 0);
	cl_testparticles = Cvar_Get ("cl_testparticles", "0", 0);
	cl_testentities = Cvar_Get ("cl_testentities", "0", 0);
	cl_testlights = Cvar_Get ("cl_testlights", "0", 0);

	cl_stats = Cvar_Get ("cl_stats", "0", 0);
	*/

	Cvar_RegisterVariable(&v_contentblend);

	Cvar_RegisterVariable(&v_idlescale);
	Cvar_RegisterVariable(&crosshaircolor);
	Cvar_RegisterVariable(&crosshair);
	Cvar_RegisterVariable(&cl_crossx);
	Cvar_RegisterVariable(&cl_crossy);
#ifdef GLQUAKE
	Cvar_RegisterVariable(&gl_cshiftpercent);
#endif

	Cvar_RegisterVariable(&cl_rollspeed);
	Cvar_RegisterVariable(&cl_rollangle);

	Cvar_RegisterVariable(&v_kicktime);
	Cvar_RegisterVariable(&v_kickroll);
	Cvar_RegisterVariable(&v_kickpitch);

	BuildGammaTable(1.0f); // no gamma yet
	Cvar_RegisterVariable(&v_gamma);
}

/*
=============
pfnCalcShake

=============
*/
void V_CalcShake()
{
	int i;
	float fraction, freq;
	float localAmp;

	if(clgame.shake.time == 0)
		return;

	if((cl.time > clgame.shake.time) || clgame.shake.amplitude <= 0 ||
	   clgame.shake.frequency <= 0)
	{
		Q_memset(&clgame.shake, 0, sizeof(clgame.shake));
		return;
	}

	if(cl.time > clgame.shake.next_shake)
	{
		// higher frequency means we recalc the extents more often and perturb the
		// display again
		clgame.shake.next_shake = cl.time + (1.0f / clgame.shake.frequency);

		// compute random shake extents (the shake will settle down from this)
		for(i = 0; i < 3; i++)
			clgame.shake.offset[i] =
			Com_RandomFloat(-clgame.shake.amplitude, clgame.shake.amplitude);
		clgame.shake.angle = Com_RandomFloat(-clgame.shake.amplitude * 0.25f,
		                                     clgame.shake.amplitude * 0.25f);
	}

	// ramp down amplitude over duration (fraction goes from 1 to 0 linearly with
	// slope 1/duration)
	fraction = (clgame.shake.time - cl.time) / clgame.shake.duration;

	// ramp up frequency over duration
	if(fraction)
		freq = (clgame.shake.frequency / fraction);
	else
		freq = 0;

	// square fraction to approach zero more quickly
	fraction *= fraction;

	// Sine wave that slowly settles to zero
	fraction = fraction * sin(cl.time * freq);

	// add to view origin
	VectorScale(clgame.shake.offset, fraction, clgame.shake.applied_offset);

	// add to roll
	clgame.shake.applied_angle = clgame.shake.angle * fraction;

	// drop amplitude a bit, less for higher frequency shakes
	localAmp = clgame.shake.amplitude * (host.frametime / (clgame.shake.duration * clgame.shake.frequency));
	clgame.shake.amplitude -= localAmp;
};

/*
=============
pfnApplyShake

=============
*/
void V_ApplyShake(float *origin, float *angles, float factor)
{
	if(origin)
		VectorMA(origin, factor, clgame.shake.applied_offset, origin);

	if(angles)
		angles[ROLL] += clgame.shake.applied_angle * factor;
};

int V_ScreenShake( const char* pszName, int iSize, void* pbuf )
{
	// TODO
	return 0;
};

int V_ScreenFade( const char* pszName, int iSize, void* pbuf )
{
	// TODO
	return 0;
};