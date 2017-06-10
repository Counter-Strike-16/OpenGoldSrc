#include <tier0/platform.h>

#include "CareerGame.h"
#include "CareerProfile.h"
#include "EngineInterface.h"

const char* CCareerGame::GetSaveGamePlayerName( const char* name )
{
	//TODO: figure out where the max player name length is defined - Solokiller
	static char nameBuf[ 33 ];

	if( name )
	{
		strncpy( nameBuf, name, ARRAYSIZE( nameBuf ) - 1 );
	}
	else
	{
		strncpy( nameBuf, engine->pfnGetCvarString( "name" ), ARRAYSIZE( nameBuf ) - 1 );
	}

	nameBuf[ ARRAYSIZE( nameBuf ) - 1 ] = '\0';

	char test[ 2 ];
	test[ 1 ] = '\0';

	for( int i = 0; i < ARRAYSIZE( nameBuf ); ++i )
	{
		test[ 0 ] = nameBuf[ i ];

		//Replace all other characters with underscores
		if( !strstr( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-0123456789", test ) )
		{
			nameBuf[ i ] = '_';
		}
	}

	return nameBuf;
}

CCareerProfileData* CCareerGame::GetProfileForName( const char* profileName )
{
	//TODO: implement - Solokiller
	return nullptr;
}

void CCareerProfileDifficultyData::Reset()
{
	played = false;
	points = 0;

	hired.clear();
	maps.clear();

	lastMap = "";
	pointsFlashed = 0;
}

void CCareerProfileData::Reset()
{
	name = "";

	sliderPos = 0;

	for( int i = 0; i < MAX_CAREER_DIFFICULTY; ++i )
	{
		medals[ i ] = 0;
		difficulty[ i ].Reset();
	}

	memset( tutorData, 0, sizeof( tutorData ) );
}
