#include "event_api.h"

namespace
{
}; // namespace

static event_api_t *gpTriAPINull =
{
	EVENT_API_VERSION,
	
	EventAPI_PlaySound,
	EventAPI_StopSound,
	
	EventAPI_FindModelIndex,
	
	EventAPI_IsLocalPlayer,
	EventAPI_LocalPlayerDucking,
	EventAPI_LocalPlayerViewheight,
	EventAPI_LocalPlayerBounds,
};