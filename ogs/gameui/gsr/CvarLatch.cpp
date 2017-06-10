#include <tier0/platform.h>

#include "CvarLatch.h"
#include "EngineInterface.h"

CvarLatch::CvarLatch( const char* cvarName, const char* defaultValue )
{
	if( !cvarName )
		cvarName = "";

	m_cvarName = new char[ strlen( cvarName ) + 1 ];
	strcpy( m_cvarName, cvarName );

	if( engine->pfnGetCvarPointer( m_cvarName ) )
	{
		auto pszValue = engine->pfnGetCvarString( m_cvarName );

		if( !pszValue )
			pszValue = "";

		m_oldValue = new char[ strlen( pszValue ) + 1 ];
		strcpy( m_oldValue, pszValue );
	}
	else if( defaultValue )
	{
		m_oldValue = new char[ strlen( defaultValue ) + 1 ];
		strcpy( m_oldValue, defaultValue );
	}
	else
	{
		m_oldValue = nullptr;
	}
}

CvarLatch::~CvarLatch()
{
	if( m_oldValue )
	{
		engine->Cvar_Set( m_cvarName, m_oldValue );

		if( m_oldValue )
			delete[] m_oldValue;
		m_oldValue = nullptr;
	}

	if( m_cvarName )
		delete[] m_cvarName;

	m_cvarName = nullptr;
}
