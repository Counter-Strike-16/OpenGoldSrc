#ifndef ENGINE_CL_ENTS_H
#define ENGINE_CL_ENTS_H

#include "pm_defs.h"

void CL_SetSolidPlayers( int playernum );

void CL_SetUpPlayerPrediction( bool dopred, bool bIncludeLocalClient );

void CL_AddStateToEntlist( physent_t* pe, entity_state_t* state );

#endif //ENGINE_CL_ENTS_H