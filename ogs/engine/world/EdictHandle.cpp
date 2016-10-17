#include "world/EdictHandle.h"

/*
=================
ED_ClearEdict

Sets everything to NULL
=================
*/
void CEdictHandle::Clear()
{
	Q_memset(&mpHandlingEdict->v, 0, sizeof(mpHandlingEdict->v));
	mpHandlingEdict->free = FALSE;
	ReleaseEntityDLLFields(mpHandlingEdict);
	InitEntityDLLFields(mpHandlingEdict);
};

entvars_t *CEdictHandle::GetEntVars()
{
	if(mpHandlingEdict)
		return &mpHandlingEdict->v;
	
	return NULL;
};

void *CEdictHandle::GetModelPtr()
{
	if(!mpHandlingEdict)
		return NULL;
	
	return Mod_Extradata(g_psv.models[mpHandlingEdict->v.modelindex]);
}

void *CEdictHandle::GetPrivateData()
{
	if(!mpHandlingEdict)
		return NULL;
	
	return mpHandlingEdict->pvPrivateData;
};

void CEdictHandle::FreePrivateData()
{
	if(mpHandlingEdict->pvPrivateData)
	{
		if(gNewDLLFunctions.pfnOnFreeEntPrivateData)
			gNewDLLFunctions.pfnOnFreeEntPrivateData(mpHandlingEdict);
		
#ifdef REHLDS_FLIGHT_REC
		if(rehlds_flrec_pvdata.string[0] != '0')
			FR_FreeEntPrivateData(mpHandlingEdict->pvPrivateData);
#endif //REHLDS_FLIGHT_REC

		Mem_Free(mpHandlingEdict->pvPrivateData);
		mpHandlingEdict->pvPrivateData = 0;
	};
};