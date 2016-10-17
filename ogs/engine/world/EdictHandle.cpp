void CEdictHandle::Clear()
{
	Q_memset(&mpHoldingEdict->v, 0, sizeof(mpHoldingEdict->v));
	mpHoldingEdict->free = FALSE;
	ReleaseEntityDLLFields(mpHoldingEdict);
	InitEntityDLLFields(mpHoldingEdict);
};

entvars_t *CEdictHandle::GetEntVars()
{
	if(mpHoldingEdict)
		return &mpHoldingEdict->v;
	
	return NULL;
};

void *CEdictHandle::GetModelPtr()
{
	if(!mpHoldingEdict)
		return NULL;
	
	return Mod_Extradata(g_psv.models[mpHoldingEdict->v.modelindex]);
}

void *CEdictHandle::GetPrivateData()
{
	if(!mpHoldingEdict)
		return NULL;
	
	return mpHoldingEdict->pvPrivateData;
};

void CEdictHandle::FreePrivateData()
{
	if(mpHoldingEdict->pvPrivateData)
	{
		if(gNewDLLFunctions.pfnOnFreeEntPrivateData)
			gNewDLLFunctions.pfnOnFreeEntPrivateData(mpHoldingEdict);
		
#ifdef REHLDS_FLIGHT_REC
		if(rehlds_flrec_pvdata.string[0] != '0')
			FR_FreeEntPrivateData(mpHoldingEdict->pvPrivateData);
#endif //REHLDS_FLIGHT_REC

		Mem_Free(mpHoldingEdict->pvPrivateData);
		mpHoldingEdict->pvPrivateData = 0;
	};
};