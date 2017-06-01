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

#include "public/archtypes.h" // DAL
#include "engine/progdefs.h"
#include "engine/edict.h"

//
// Misc utility code
//

#ifndef ENGINECALLBACK_H
#include "enginecallback.h"
#endif
inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float *pOrigin, entvars_t *ent); // implementation later in this file

extern globalvars_t *gpGlobals;

// Use this instead of ALLOC_STRING on constant strings
#define STRING(offset) ((const char *)(gpGlobals->pStringBase + (unsigned int)(offset)))
#define MAKE_STRING(str) ((uint64)(str) - (uint64)(STRING(0)))

// Makes these more explicit, and easier to find
#define FILE_GLOBAL static
#define DLL_GLOBAL

// Until we figure out why "const" gives the compiler problems, we'll just have to use
// this bogus "empty" define to mark things as constant.
#define CONSTANT

// More explicit than "int"
typedef int EOFFSET;

// In case it's not alread defined
typedef int BOOL;

// In case this ever changes
#define M_PI 3.14159265358979323846

// Keeps clutter down a bit, when declaring external entity/global method prototypes
#define DECLARE_GLOBAL_METHOD(MethodName) extern void UTIL_DLLEXPORT MethodName(void)
#define GLOBAL_METHOD(funcname) void UTIL_DLLEXPORT funcname(void)

#ifndef UTIL_DLLEXPORT
#ifdef _WIN32
#define UTIL_DLLEXPORT __declspec(dllexport)
#else
#define UTIL_DLLEXPORT __attribute__((visibility("default")))
#endif
#endif

// This is the glue that hooks .MAP entity class names to our CPP classes
// The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress()
// The function is used to intialize / allocate the object for the entity
#define LINK_ENTITY_TO_CLASS(mapClassName, DLLClassName)         \
	extern "C" UTIL_DLLEXPORT void mapClassName(entvars_t *pev); \
	void mapClassName(entvars_t *pev) { GetClassPtr((DLLClassName *)pev); }
//
// Conversion among the three types of "entity", including identity-conversions.
//
#ifdef DEBUG
extern edict_t *DBG_EntOfVars(const entvars_t *pev);
inline edict_t *ENT(const entvars_t *pev)
{
	return DBG_EntOfVars(pev);
}
#else
inline edict_t *ENT(const entvars_t *pev)
{
	return pev->pContainingEntity;
}
#endif
inline edict_t *ENT(edict_t *pent)
{
	return pent;
}
inline edict_t *ENT(EOFFSET eoffset)
{
	return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset);
}
inline EOFFSET OFFSET(EOFFSET eoffset)
{
	return eoffset;
}
inline EOFFSET OFFSET(const edict_t *pent)
{
#if _DEBUG
	if(!pent)
		ALERT(at_error, "Bad ent in OFFSET()\n");
#endif
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent);
}
inline EOFFSET OFFSET(entvars_t *pev)
{
#if _DEBUG
	if(!pev)
		ALERT(at_error, "Bad pev in OFFSET()\n");
#endif
	return OFFSET(ENT(pev));
}
inline entvars_t *VARS(entvars_t *pev)
{
	return pev;
}

inline entvars_t *VARS(edict_t *pent)
{
	if(!pent)
		return NULL;

	return &pent->v;
}

inline entvars_t *VARS(EOFFSET eoffset)
{
	return VARS(ENT(eoffset));
}
inline int ENTINDEX(edict_t *pEdict)
{
	return (*g_engfuncs.pfnIndexOfEdict)(pEdict);
}
inline edict_t *INDEXENT(int iEdictNum)
{
	return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum);
}
inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float *pOrigin, entvars_t *ent)
{
	(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ENT(ent));
}

// Testing the three types of "entity" for nullity
#define eoNullEntity 0
inline BOOL FNullEnt(EOFFSET eoffset)
{
	return eoffset == 0;
}
inline BOOL FNullEnt(const edict_t *pent)
{
	return pent == NULL || FNullEnt(OFFSET(pent));
}
inline BOOL FNullEnt(entvars_t *pev)
{
	return pev == NULL || FNullEnt(OFFSET(pev));
}

// Testing strings for nullity
#define iStringNull 0
inline BOOL FStringNull(int iString)
{
	return iString == iStringNull;
}

#define cchMapNameMost 32

// Dot products for view cone checking
#define VIEW_FIELD_FULL (float)-1.0        // +-180 degrees
#define VIEW_FIELD_WIDE (float)-0.7        // +-135 degrees 0.1 // +-85 degrees, used for full FOV checks
#define VIEW_FIELD_NARROW (float)0.7       // +-45 degrees, more narrow check used to set up ranged attacks
#define VIEW_FIELD_ULTRA_NARROW (float)0.9 // +-25 degrees, more narrow check used to set up ranged attacks

//
// How did I ever live without ASSERT?
//
#ifdef DEBUG
void DBG_AssertFunction(BOOL fExpr, const char *szExpr, const char *szFile, int szLine, const char *szMessage);
#define ASSERT(f) DBG_AssertFunction(f, #f, __FILE__, __LINE__, NULL)
#define ASSERTSZ(f, sz) DBG_AssertFunction(f, #f, __FILE__, __LINE__, sz)
#else // !DEBUG
#define ASSERT(f)
#define ASSERTSZ(f, sz)
#endif // !DEBUG

//
// Constants that were used only by QC (maybe not used at all now)
//
// Un-comment only as needed
//
#define LANGUAGE_ENGLISH 0
#define LANGUAGE_GERMAN 1
#define LANGUAGE_FRENCH 2
#define LANGUAGE_BRITISH 3

extern DLL_GLOBAL int g_Language;

#define AMBIENT_SOUND_STATIC 0 // medium radius attenuation
#define AMBIENT_SOUND_EVERYWHERE 1
#define AMBIENT_SOUND_SMALLRADIUS 2
#define AMBIENT_SOUND_MEDIUMRADIUS 4
#define AMBIENT_SOUND_LARGERADIUS 8
#define AMBIENT_SOUND_START_SILENT 16
#define AMBIENT_SOUND_NOT_LOOPING 32

#define SPEAKER_START_SILENT 1 // wait for trigger 'on' to start announcements

#define SND_SPAWNING (1 << 8)     // duplicated in protocol.h we're spawing, used in some cases for ambients
#define SND_STOP (1 << 5)         // duplicated in protocol.h stop sound
#define SND_CHANGE_VOL (1 << 6)   // duplicated in protocol.h change sound vol
#define SND_CHANGE_PITCH (1 << 7) // duplicated in protocol.h change sound pitch

#define LFO_SQUARE 1
#define LFO_TRIANGLE 2
#define LFO_RANDOM 3

// func_rotating
#define SF_BRUSH_ROTATE_Y_AXIS 0
#define SF_BRUSH_ROTATE_INSTANT 1
#define SF_BRUSH_ROTATE_BACKWARDS 2
#define SF_BRUSH_ROTATE_Z_AXIS 4
#define SF_BRUSH_ROTATE_X_AXIS 8
#define SF_PENDULUM_AUTO_RETURN 16
#define SF_PENDULUM_PASSABLE 32

#define SF_BRUSH_ROTATE_SMALLRADIUS 128
#define SF_BRUSH_ROTATE_MEDIUMRADIUS 256
#define SF_BRUSH_ROTATE_LARGERADIUS 512

#define PUSH_BLOCK_ONLY_X 1
#define PUSH_BLOCK_ONLY_Y 2

#define VEC_HULL_MIN Vector(-16, -16, -36)
#define VEC_HULL_MAX Vector(16, 16, 36)
#define VEC_HUMAN_HULL_MIN Vector(-16, -16, 0)
#define VEC_HUMAN_HULL_MAX Vector(16, 16, 72)
#define VEC_HUMAN_HULL_DUCK Vector(16, 16, 36)

#define VEC_VIEW Vector(0, 0, 28)

#define VEC_DUCK_HULL_MIN Vector(-16, -16, -18)
#define VEC_DUCK_HULL_MAX Vector(16, 16, 18)
#define VEC_DUCK_VIEW Vector(0, 0, 12)

#define SVC_TEMPENTITY 23
#define SVC_INTERMISSION 30
#define SVC_CDTRACK 32
#define SVC_WEAPONANIM 35
#define SVC_ROOMTYPE 37
#define SVC_DIRECTOR 51

// triggers
#define SF_TRIGGER_ALLOWMONSTERS 1 // monsters allowed to fire this trigger
#define SF_TRIGGER_NOCLIENTS 2     // players not allowed to fire this trigger
#define SF_TRIGGER_PUSHABLES 4     // only pushables can fire this trigger

// func breakable
#define SF_BREAK_TRIGGER_ONLY 1 // may only be broken by trigger
#define SF_BREAK_TOUCH 2        // can be 'crashed through' by running player (plate glass)
#define SF_BREAK_PRESSURE 4     // can be broken by a player standing on it
#define SF_BREAK_CROWBAR 256    // instant break if hit with crowbar

// func_pushable (it's also func_breakable, so don't collide with those flags)
#define SF_PUSH_BREAKABLE 128

#define SF_LIGHT_START_OFF 1

#define SPAWNFLAG_NOMESSAGE 1
#define SPAWNFLAG_NOTOUCH 1
#define SPAWNFLAG_DROIDONLY 4

#define SPAWNFLAG_USEONLY 1 // can't be touched, must be used (buttons)

#define TELE_PLAYER_ONLY 1
#define TELE_SILENT 2

#define SF_TRIG_PUSH_ONCE 1

// Sound Utilities

// sentence groups
#define CBSENTENCENAME_MAX 16
#define CVOXFILESENTENCEMAX 1536 // max number of sentences in game. NOTE: this must match
                                 // CVOXFILESENTENCEMAX in engine\sound.h!!!

extern char gszallsentencenames[CVOXFILESENTENCEMAX][CBSENTENCENAME_MAX];
extern int gcallsentences;

int USENTENCEG_Pick(int isentenceg, char *szfound);
int USENTENCEG_PickSequential(int isentenceg, char *szfound, int ipick, int freset);
void USENTENCEG_InitLRU(unsigned char *plru, int count);

void SENTENCEG_Init();
void SENTENCEG_Stop(edict_t *entity, int isentenceg, int ipick);
int SENTENCEG_PlayRndI(edict_t *entity, int isentenceg, float volume, float attenuation, int flags, int pitch);
int SENTENCEG_PlayRndSz(edict_t *entity, const char *szrootname, float volume, float attenuation, int flags, int pitch);
int SENTENCEG_PlaySequentialSz(edict_t *entity, const char *szrootname, float volume, float attenuation, int flags, int pitch, int ipick, int freset);
int SENTENCEG_GetIndex(const char *szrootname);
int SENTENCEG_Lookup(const char *sample, char *sentencenum);

// NOTE: use EMIT_SOUND_DYN to set the pitch of a sound. Pitch of 100
// is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
// down to 1 is a lower pitch.   150 to 70 is the realistic range.
// EMIT_SOUND_DYN with pitch != 100 should be used sparingly, as it's not quite as
// fast as EMIT_SOUND (the pitchshift mixer is not native coded).

void EMIT_SOUND_DYN(edict_t *entity, int channel, const char *sample, float volume, float attenuation,
                    int flags, int pitch);

inline void EMIT_SOUND(edict_t *entity, int channel, const char *sample, float volume, float attenuation)
{
	EMIT_SOUND_DYN(entity, channel, sample, volume, attenuation, 0, PITCH_NORM);
}

inline void STOP_SOUND(edict_t *entity, int channel, const char *sample)
{
	EMIT_SOUND_DYN(entity, channel, sample, 0, 0, SND_STOP, PITCH_NORM);
}

void EMIT_SOUND_SUIT(edict_t *entity, const char *sample);
void EMIT_GROUPID_SUIT(edict_t *entity, int isentenceg);
void EMIT_GROUPNAME_SUIT(edict_t *entity, const char *groupname);

#define PRECACHE_SOUND_ARRAY(a)               \
	{                                         \
		for(int i = 0; i < ARRAYSIZE(a); i++) \
			PRECACHE_SOUND((char *)a[i]);     \
	}

#define EMIT_SOUND_ARRAY_DYN(chan, array) \
	EMIT_SOUND_DYN(ENT(pev), chan, array[RANDOM_LONG(0, ARRAYSIZE(array) - 1)], 1.0, ATTN_NORM, 0, RANDOM_LONG(95, 105));

#define RANDOM_SOUND_ARRAY(array) (array)[RANDOM_LONG(0, ARRAYSIZE((array)) - 1)]

#define PLAYBACK_EVENT(flags, who, index) PLAYBACK_EVENT_FULL(flags, who, index, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0);
#define PLAYBACK_EVENT_DELAY(flags, who, index, delay) PLAYBACK_EVENT_FULL(flags, who, index, delay, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0);

#define GROUP_OP_AND 0
#define GROUP_OP_NAND 1

class UTIL_GroupTrace
{
public:
	UTIL_GroupTrace(int groupmask, int op);
	~UTIL_GroupTrace(void);

private:
	int m_oldgroupmask, m_oldgroupop;
};