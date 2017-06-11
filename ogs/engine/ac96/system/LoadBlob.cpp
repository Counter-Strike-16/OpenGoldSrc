#include "precompiled.hpp"

#ifdef WIN32
	#include "winheaders.h"
#else
	#include <dlfcn.h>
#endif

#include "commondef.hpp"
#include "system/LoadBlob.hpp"

int FIsBlob( const char* pstFileName )
{
	return false;
}

void FreeBlob( BlobFootprint_t* pblobfootprint )
{
	if( pblobfootprint->m_hDll )
	{
#ifdef WIN32
		FreeLibrary( reinterpret_cast<HMODULE>( pblobfootprint->m_hDll ) );
#else
		dlclose( ( void * ) pblobfootprint->m_hDll );
#endif
	}
}

void* NLoadBlobFile( const char* pstFileName, BlobFootprint_t* pblobfootprint, void* pv, char fLoadAsDll )
{
	void* pFile = nullptr;

	if( fLoadAsDll )
	{
		char rgchNew[ 2048 ];
		strcpy( rgchNew, pstFileName );

		char rgchLocalPath[ 2048 ];
		g_pFileSystem->GetLocalPath( rgchNew, rgchLocalPath, ARRAYSIZE( rgchLocalPath ) );
		
		pFile = FS_LoadLibrary( rgchLocalPath );
		pblobfootprint->m_hDll = reinterpret_cast<int32>( pFile );

		if( pFile )
		{
			auto pFunc = reinterpret_cast<FFunction>( Sys_GetProcAddress( pFile, "F" ) );
			
			if( pFunc )
			{
				pFunc( pv );
			}
			else
			{
				Sys_UnloadModule( reinterpret_cast<CSysModule*>( pFile ) );
				pFile = nullptr;
			}
		}
	}

	return pFile;
}