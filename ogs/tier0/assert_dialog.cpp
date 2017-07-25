//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
#include "dbg.h"
#include "strtools.h"
#include "threadtools.h"

struct CAssertDisable
{
	tchar m_Filename[ 512 ];
	int m_LineMin;
	int m_LineMax;
	int m_nIgnoreTimes;
	CAssertDisable* m_pNext;
};

 static CAssertDisable* g_pAssertDisables = nullptr;

static CAssertDisable* CreateNewAssertDisable( const tchar* pFilename )
{
	auto pAssertDisable = new CAssertDisable;

	_tcsncpy( pAssertDisable->m_Filename, pFilename, sizeof( pAssertDisable->m_Filename ) - 1 );
	pAssertDisable->m_Filename[ sizeof( pAssertDisable->m_Filename ) - 1 ] = _T( '\0' );

	pAssertDisable->m_LineMin = -1;
	pAssertDisable->m_LineMax = -1;
	pAssertDisable->m_nIgnoreTimes = -1;

	pAssertDisable->m_pNext = g_pAssertDisables;
	g_pAssertDisables = pAssertDisable;

	return pAssertDisable;
}

struct CDialogInitInfo
{
	const tchar* m_pFilename;
	int m_iLine;
	const tchar* m_pExpression;
};

static CDialogInitInfo g_Info;

static bool s_bInAssert = false;
static bool g_bBreak = false;

PLATFORM_INTERFACE bool DoNewAssertDialog( const tchar *pFilename, int line, const tchar *pExpression )
{
	bool bDoAssert = false;

	if( !_tcsstr( Plat_GetCommandLine(), _T( "-noassert" ) ) )
	{
		if( !_tcsstr( Plat_GetCommandLine(), _T( "-nocrashdialog" ) ) )
		{
			bDoAssert = true;

			if( ThreadInMainThread() )
			{
				if( !_tcsstr( Plat_GetCommandLine(), _T( "-debugbreak" ) ) )
				{
					for( CAssertDisable* pNext = g_pAssertDisables, * pPrev = nullptr; pNext; )
					{
						if( !_tcsicmp( pFilename, pNext->m_Filename ) )
						{
							const bool bCheckRange = pNext->m_LineMin != -1 || pNext->m_LineMax != -1;

							if( line >= pNext->m_LineMin && line <= pNext->m_LineMax || !bCheckRange )
							{
								if( pNext->m_nIgnoreTimes <= 0 )
									return false;

								--pNext->m_nIgnoreTimes;

								if( pNext->m_nIgnoreTimes )
									return false;

								if( pPrev )
									pPrev->m_pNext = pNext->m_pNext;

								delete pNext;

								//Skip iterator increment.
								continue;
							}
						}

						pPrev = pNext, pNext = pNext->m_pNext;
					}

					g_Info.m_pFilename = pFilename;
					g_Info.m_iLine = line;
					g_bBreak = false;
					g_Info.m_pExpression = pExpression;
					_ftprintf( stderr, _T( "%s %i %s" ), pFilename, line, pExpression );
					return g_bBreak;
				}
			}
		}
	}

	return bDoAssert;
}

void IgnoreAssertsInCurrentFile()
{
	CreateNewAssertDisable( g_Info.m_pFilename );
}

CAssertDisable* IgnoreAssertsNearby( int nRange )
{
	auto pAssertDisable = CreateNewAssertDisable( g_Info.m_pFilename );

	//TODO: does this really need to be both min and max? - Solokiller
	pAssertDisable->m_LineMin = g_Info.m_iLine - nRange;
	pAssertDisable->m_LineMax = g_Info.m_iLine - nRange;

	return pAssertDisable;
}

PLATFORM_INTERFACE bool IsInAssert()
{
	return s_bInAssert;
}

PLATFORM_INTERFACE void SetInAssert( bool bState )
{
	s_bInAssert = bState;
}

DBG_INTERFACE bool ShouldUseNewAssertDialog()
{
	Plat_GetCommandLine();

	return false;
}
