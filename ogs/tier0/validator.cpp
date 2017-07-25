//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
#include "validator.h"

#ifdef WIN32//DBGFLAG_VALIDATE
CValidator::CValidator()
	: m_pValObjectFirst( nullptr )
	, m_pValObjectLast( nullptr )
	, m_pValObjectCur( nullptr )
	, m_cpvOwned( 0 )
	, m_cubLeaked( 0 )
	, m_cpubLeaked( 0 )
	, m_bMemLeaks( false )
{
	//TODO: implement - Solokiller
}

CValidator::~CValidator()
{
	//TODO: implement - Solokiller
}

void CValidator::Push( const tchar* pchType, void* pvObj, const tchar* pchName )
{
	//TODO: implement - Solokiller
}

void CValidator::Pop()
{
	//TODO: implement - Solokiller
}

void CValidator::ClaimMemory( void* pvMem )
{
	//TODO: implement - Solokiller
}

void CValidator::Finalize()
{
	//TODO: implement - Solokiller
}

void CValidator::RenderObjects( int cubThreshold )
{
	//TODO: implement - Solokiller
}

void CValidator::RenderLeaks()
{
	//TODO: implement - Solokiller
}

CValObject* CValidator::FindObject( void* pvObj )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void CValidator::DiffAgainst( CValidator* pOtherValidator )
{
	//TODO: implement - Solokiller
}

void CValidator::Validate( CValidator& validator, tchar* pchName )
{
	//TODO: implement - Solokiller
}
#endif //DBGFLAG_VALIDATE
