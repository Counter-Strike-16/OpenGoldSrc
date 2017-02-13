/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== util.cpp ========================================================

  Utility code.  Really not optional after all.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include <time.h>

static unsigned int glSeed = 0;

unsigned int seed_table[256] =
{
  28985, 27138, 26457, 9451, 17764, 10909, 28790, 8716, 6361, 4853, 17798, 21977, 19643, 20662, 10834, 20103,
  27067, 28634, 18623, 25849, 8576, 26234, 23887, 18228, 32587, 4836, 3306, 1811, 3035, 24559, 18399, 315,
  26766, 907, 24102, 12370, 9674, 2972, 10472, 16492, 22683, 11529, 27968, 30406, 13213, 2319, 23620, 16823,
  10013, 23772, 21567, 1251, 19579, 20313, 18241, 30130, 8402, 20807, 27354, 7169, 21211, 17293, 5410, 19223,
  10255, 22480, 27388, 9946, 15628, 24389, 17308, 2370, 9530, 31683, 25927, 23567, 11694, 26397, 32602, 15031,
  18255, 17582, 1422, 28835, 23607, 12597, 20602, 10138, 5212, 1252, 10074, 23166, 19823, 31667, 5902, 24630,
  18948, 14330, 14950, 8939, 23540, 21311, 22428, 22391, 3583, 29004, 30498, 18714, 4278, 2437, 22430, 3439,
  28313, 23161, 25396, 13471, 19324, 15287, 2563, 18901, 13103, 16867, 9714, 14322, 15197, 26889, 19372, 26241,
  31925, 14640, 11497, 8941, 10056, 6451, 28656, 10737, 13874, 17356, 8281, 25937, 1661, 4850, 7448, 12744,
  21826, 5477, 10167, 16705, 26897, 8839, 30947, 27978, 27283, 24685, 32298, 3525, 12398, 28726, 9475, 10208,
  617, 13467, 22287, 2376, 6097, 26312, 2974, 9114, 21787, 28010, 4725, 15387, 3274, 10762, 31695, 17320,
  18324, 12441, 16801, 27376, 22464, 7500, 5666, 18144, 15314, 31914, 31627, 6495, 5226, 31203, 2331, 4668,
  12650, 18275, 351, 7268, 31319, 30119, 7600, 2905, 13826, 11343, 13053, 15583, 30055, 31093, 5067, 761,
  9685, 11070, 21369, 27155, 3663, 26542, 20169, 12161, 15411, 30401, 7580, 31784, 8985, 29367, 20989, 14203,
  29694, 21167, 10337, 1706, 28578, 887, 3373, 19477, 14382, 675, 7033, 15111, 26138, 12252, 30996, 21409,
  25678, 18555, 13256, 23316, 22407, 16727, 991, 9236, 5373, 29402, 6117, 15241, 27715, 19291, 19888, 19847
};

TYPEDESCRIPTION gEntvarsDescription[] =
{
  DEFINE_ENTITY_FIELD(classname, FIELD_STRING),
  DEFINE_ENTITY_GLOBAL_FIELD(globalname, FIELD_STRING),

  DEFINE_ENTITY_FIELD(origin, FIELD_POSITION_VECTOR),
  DEFINE_ENTITY_FIELD(oldorigin, FIELD_POSITION_VECTOR),
  DEFINE_ENTITY_FIELD(velocity, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(basevelocity, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(movedir, FIELD_VECTOR),

  DEFINE_ENTITY_FIELD(angles, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(avelocity, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(punchangle, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(v_angle, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(fixangle, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(idealpitch, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(pitch_speed, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(ideal_yaw, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(yaw_speed, FIELD_FLOAT),

  DEFINE_ENTITY_FIELD(modelindex, FIELD_INTEGER),
  DEFINE_ENTITY_GLOBAL_FIELD(model, FIELD_MODELNAME),

  DEFINE_ENTITY_FIELD(viewmodel, FIELD_MODELNAME),
  DEFINE_ENTITY_FIELD(weaponmodel, FIELD_MODELNAME),

  DEFINE_ENTITY_FIELD(absmin, FIELD_POSITION_VECTOR),
  DEFINE_ENTITY_FIELD(absmax, FIELD_POSITION_VECTOR),
  DEFINE_ENTITY_GLOBAL_FIELD(mins, FIELD_VECTOR),
  DEFINE_ENTITY_GLOBAL_FIELD(maxs, FIELD_VECTOR),
  DEFINE_ENTITY_GLOBAL_FIELD(size, FIELD_VECTOR),

  DEFINE_ENTITY_FIELD(ltime, FIELD_TIME),
  DEFINE_ENTITY_FIELD(nextthink, FIELD_TIME),

  DEFINE_ENTITY_FIELD(solid, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(movetype, FIELD_INTEGER),

  DEFINE_ENTITY_FIELD(skin, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(body, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(effects, FIELD_INTEGER),

  DEFINE_ENTITY_FIELD(gravity, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(friction, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(light_level, FIELD_FLOAT),

  DEFINE_ENTITY_FIELD(frame, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(scale, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(sequence, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(animtime, FIELD_TIME),
  DEFINE_ENTITY_FIELD(framerate, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(controller, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(blending, FIELD_INTEGER),

  DEFINE_ENTITY_FIELD(rendermode, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(renderamt, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(rendercolor, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(renderfx, FIELD_INTEGER),

  DEFINE_ENTITY_FIELD(health, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(frags, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(weapons, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(takedamage, FIELD_FLOAT),

  DEFINE_ENTITY_FIELD(deadflag, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(view_ofs, FIELD_VECTOR),
  DEFINE_ENTITY_FIELD(button, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(impulse, FIELD_INTEGER),

  DEFINE_ENTITY_FIELD(chain, FIELD_EDICT),
  DEFINE_ENTITY_FIELD(dmg_inflictor, FIELD_EDICT),
  DEFINE_ENTITY_FIELD(enemy, FIELD_EDICT),
  DEFINE_ENTITY_FIELD(aiment, FIELD_EDICT),
  DEFINE_ENTITY_FIELD(owner, FIELD_EDICT),
  DEFINE_ENTITY_FIELD(groundentity, FIELD_EDICT),

  DEFINE_ENTITY_FIELD(spawnflags, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(flags, FIELD_FLOAT),

  DEFINE_ENTITY_FIELD(colormap, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(team, FIELD_INTEGER),

  DEFINE_ENTITY_FIELD(max_health, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(teleport_time, FIELD_TIME),
  DEFINE_ENTITY_FIELD(armortype, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(armorvalue, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(waterlevel, FIELD_INTEGER),
  DEFINE_ENTITY_FIELD(watertype, FIELD_INTEGER),

  // Having these fields be local to the individual levels makes it easier to test those levels individually.
  DEFINE_ENTITY_GLOBAL_FIELD(target, FIELD_STRING),
  DEFINE_ENTITY_GLOBAL_FIELD(targetname, FIELD_STRING),
  DEFINE_ENTITY_FIELD(netname, FIELD_STRING),
  DEFINE_ENTITY_FIELD(message, FIELD_STRING),

  DEFINE_ENTITY_FIELD(dmg_take, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(dmg_save, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(dmg, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(dmgtime, FIELD_TIME),

  DEFINE_ENTITY_FIELD(noise, FIELD_SOUNDNAME),
  DEFINE_ENTITY_FIELD(noise1, FIELD_SOUNDNAME),
  DEFINE_ENTITY_FIELD(noise2, FIELD_SOUNDNAME),
  DEFINE_ENTITY_FIELD(noise3, FIELD_SOUNDNAME),
  DEFINE_ENTITY_FIELD(speed, FIELD_FLOAT),
  DEFINE_ENTITY_FIELD(air_finished, FIELD_TIME),
  DEFINE_ENTITY_FIELD(pain_finished, FIELD_TIME),
  DEFINE_ENTITY_FIELD(radsuit_finished, FIELD_TIME),
};

#define ENTVARS_COUNT (sizeof(gEntvarsDescription) / sizeof(gEntvarsDescription[0]))

#ifdef DEBUG
edict_t *DBG_EntOfVars(const entvars_t *pev)
{
	if(pev->pContainingEntity != NULL)
		return pev->pContainingEntity;
	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine");
	edict_t *pent = (*g_engfuncs.pfnFindEntityByVars)((entvars_t *)pev);
	if(pent == NULL)
		ALERT(at_console, "DAMN!  Even the engine couldn't FindEntityByVars!");
	((entvars_t *)pev)->pContainingEntity = pent;
	return pent;
}
#endif //DEBUG

#ifdef DEBUG
void DBG_AssertFunction(
BOOL fExpr,
const char *szExpr,
const char *szFile,
int szLine,
const char *szMessage)
{
	if(fExpr)
		return;
	char szOut[512];
	if(szMessage != NULL)
		sprintf(szOut, "ASSERT FAILED:\n %s \n(%s@%d)\n%s", szExpr, szFile, szLine, szMessage);
	else
		sprintf(szOut, "ASSERT FAILED:\n %s \n(%s@%d)", szExpr, szFile, szLine);
	ALERT(at_console, szOut);
}
#endif // DEBUG

#define SWAP(a, b, temp) ((temp) = (a), (a) = (b), (b) = (temp))

static unsigned short FixedUnsigned16(float value, float scale)
{
	int output;

	output = value * scale;
	if(output < 0)
		output = 0;
	if(output > 0xFFFF)
		output = 0xFFFF;

	return (unsigned short)output;
}

static short FixedSigned16(float value, float scale)
{
	int output;

	output = value * scale;

	if(output > 32767)
		output = 32767;

	if(output < -32768)
		output = -32768;

	return (short)output;
}

char *UTIL_dtos1(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}

char *UTIL_dtos2(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}

char *UTIL_dtos3(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}

char *UTIL_dtos4(int d)
{
	static char buf[8];
	sprintf(buf, "%d", d);
	return buf;
}

char *UTIL_VarArgs(char *format, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;
}

extern DLL_GLOBAL short g_sModelIndexBubbles; // holds the index for the bubbles model

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf(char *fmt, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	// Print to server console
	ALERT(at_logged, "%s", string);
}

#ifndef _WIN32
extern "C" {
unsigned _rotr(unsigned val, int shift)
{
	register unsigned lobit;     /* non-zero means lo bit set */
	register unsigned num = val; /* number to rotate */

	shift &= 0x1f; /* modulo 32 -- this will also make
                                           negative shifts work */

	while(shift--)
	{
		lobit = num & 1; /* get high bit */
		num >>= 1;       /* shift right one bit */
		if(lobit)
			num |= 0x80000000; /* set hi bit if lo bit was set */
	}

	return num;
}
}
#endif