//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
#include "valobject.h"

#ifdef WIN32//DBGFLAG_VALIDATE
CValObject::~CValObject()
{
	//TODO: implement - Solokiller
}

void CValObject::Init( tchar* pchType, void* pvObj, tchar* pchName, CValObject* pValObjectParent,
		   CValObject* pValObjectPrev )
{
	//TODO: implement - Solokiller
}

void CValObject::ClaimMemoryBlock( void* pvMem )
{
	//TODO: implement - Solokiller
}

void CValObject::ClaimChildMemoryBlock( int cubUser )
{
	//TODO: implement - Solokiller
}
#endif //DBGFLAG_VALIDATE
