/// @file

#include "precompiled.hpp"
#include "commondef.hpp"
#include "sv_secure.hpp"

// ModuleS_i386.so
char *SV_GetFileNameModuleS()
{
	char szModule[ 20 ];

#ifdef WIN32
	//ModuleS.dll
	szModule[ 4 ] = 'l';
	szModule[ 9 ] = 'l';
	szModule[ 10 ] = 'l';
	szModule[ 0 ] = 'M';
	szModule[ 1 ] = 'o';
	szModule[ 2 ] = 'd';
	szModule[ 3 ] = 'u';
	szModule[ 5 ] = 'e';
	szModule[ 6 ] = 'S';
	szModule[ 7 ] = '.';
	szModule[ 8 ] = 'd';
	szModule[ 11 ] = '\0';
#else
	//ModuleS_i386.so
	szModule[ 0 ] = 'M';
	szModule[ 4 ] = 'l';
	szModule[ 5 ] = 'e';
	szModule[ 6 ] = 'S';
	szModule[ 7 ] = '_';
	szModule[ 8 ] = 'i';
	szModule[ 9 ] = '3';
	szModule[ 3 ] = 'u';
	szModule[ 13 ] = 's';
	szModule[ 1 ] = 'o';
	szModule[ 2 ] = 'd';
	szModule[ 12 ] = '.';
	szModule[ 14 ] = 'o';
	szModule[ 10 ] = '8';
	szModule[ 11 ] = '6';
	szModule[ 15 ] = '\0';
#endif

	char gamedir[ MAX_PATH ];
	COM_FileBase( com_gamedir, gamedir );

	static char moduleFileName[ MAX_PATH ];

	snprintf( moduleFileName, ARRAYSIZE( moduleFileName ) - 1, "%s/%s", gamedir, szModule );
	moduleFileName[ ARRAYSIZE( moduleFileName ) - 1 ] = '\0';

	return moduleFileName;
};

char *SV_GetFileNameModuleC()
{
	char szModule[ 20 ];

#ifdef WIN32
	//ModuleC.dll
	szModule[ 4 ] = 'l';
	szModule[ 9 ] = 'l';
	szModule[ 10 ] = 'l';
	szModule[ 0 ] = 'M';
	szModule[ 1 ] = 'o';
	szModule[ 2 ] = 'd';
	szModule[ 3 ] = 'u';
	szModule[ 5 ] = 'e';
	szModule[ 6 ] = 'C';
	szModule[ 7 ] = '.';
	szModule[ 8 ] = 'd';
	szModule[ 11 ] = '\0';
#else
	// ModuleC.dll
	szModule[ 0 ] = 'M';
	szModule[ 4 ] = 'l';
	szModule[ 5 ] = 'e';
	szModule[ 6 ] = 'C';
	szModule[ 7 ] = '.';
	szModule[ 1 ] = 'o';
	szModule[ 2 ] = 'd';
	szModule[ 3 ] = 'u';
	szModule[ 8 ] = 'd';
	szModule[ 9 ] = 'l';
	szModule[ 10 ] = 'l';
	szModule[ 11 ] = '\0';
#endif

	char gamedir[ MAX_PATH ];
	COM_FileBase( com_gamedir, gamedir );

	static char moduleFileName[ MAX_PATH ];

	snprintf( moduleFileName, ARRAYSIZE( moduleFileName ) - 1, "%s/%s", gamedir, szModule );
	moduleFileName[ ARRAYSIZE( moduleFileName ) - 1 ] = '\0';

	return moduleFileName;
};