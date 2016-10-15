#include "event_api.h"

namespace
{
void EventAPI_PlaySound(int ent, float *origin, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch)
{
};

void EventAPI_StopSound(int ent, int channel, const char *sample)
{
};

int EventAPI_FindModelIndex(const char *pmodel)
{
	return 0;
};

int EventAPI_IsLocal/*Player*/(int playernum)
{
	return 0;
};

int EventAPI_LocalPlayerDucking()
{
	return 0;
};

void EventAPI_LocalPlayerViewheight(float *)
{
};

void EventAPI_LocalPlayerBounds(int hull, float *mins, float *maxs)
{
};

int EventAPI_IndexFromTrace(struct pmtrace_s *pTrace)
{
	return 0;
};

struct physent_s *EventAPI_GetPhysent(int idx)
{
	return NULL;
};

void EventAPI_SetUpPlayerPrediction(int dopred, int bIncludeLocalClient)
{
};

void EventAPI_PushPMStates()
{
};

void EventAPI_PopPMStates()
{
};
	
void EventAPI_SetSolidPlayers(int playernum)
{
};

void EventAPI_SetTraceHull(int hull)
{
};

void EventAPI_PlayerTrace(float *start, float *end, int traceFlags, int ignore_pe, struct pmtrace_s *tr)
{
};

void EventAPI_WeaponAnimation(int sequence, int body)
{
};

unsigned short EventAPI_PrecacheEvent(int type, const char* psz)
{
	return 0;
};

void EventAPI_PlaybackEvent(int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
};

const char *EventAPI_TraceTexture(int ground, float *vstart, float *vend)
{
	return "";
};

void EventAPI_StopAllSounds(int entnum, int entchannel)
{
};

void EventAPI_KillEvents(int entnum, const char *eventname)
{
};
}; // namespace

static event_api_t *gpTriAPINull =
{
	EVENT_API_VERSION,
	
	EventAPI_PlaySound,
	EventAPI_StopSound,
	
	EventAPI_FindModelIndex,
	
	EventAPI_IsLocal/*Player*/,
	EventAPI_LocalPlayerDucking,
	EventAPI_LocalPlayerViewheight,
	EventAPI_LocalPlayerBounds,
	
	EventAPI_IndexFromTrace,
	
	EventAPI_GetPhysent,
	
	EventAPI_SetUpPlayerPrediction,
	
	EventAPI_PushPMStates,
	EventAPI_PopPMStates,
	
	EventAPI_SetSolidPlayers,
	
	EventAPI_SetTraceHull,
	EventAPI_PlayerTrace,
	
	EventAPI_WeaponAnimation,
	
	EventAPI_PrecacheEvent,
	EventAPI_PlaybackEvent,
	
	EventAPI_TraceTexture,
	EventAPI_StopAllSounds,
	EventAPI_KillEvents
};