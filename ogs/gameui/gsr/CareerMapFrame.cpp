#include <vgui/ILocalize.h>
#include <vgui_controls/Controls.h>

#include "CareerGame.h"
#include "CareerMapFrame.h"

void ShowCareerMapDescription( vgui2::RichText* pText, CCareerMap* pMap, SDK_Color normalColor, SDK_Color friendlyFireColor )
{
	if( !pText )
		return;

	wchar_t str[ 2048 ];
	wchar_t botWStr[ 64 ];
	char botStr[ 64 ];
	wchar_t intWStr[ 6 ];
	char intStr[ 6 ];

	//TODO: the wcsncat calls here are incorrect because they pass the total buffer size, not remaining size. - Solokiller

	if( pMap )
	{
		memset( str, 0, sizeof( str ) );
		memset( intStr, 0, sizeof( intStr ) );

		const int numBots = pMap->GetCurrentEnemyCount();
		snprintf( intStr, ARRAYSIZE( intStr ), "%d", numBots );

		vgui2::localize()->ConvertANSIToUnicode( intStr, intWStr, sizeof( intWStr ) );

		const wchar_t* pszLocalized;
		
		if( numBots == 1
			&& ( ( pszLocalized = vgui2::localize()->Find(
				"#Career_EnemyListSingle" ) ) != 0 ) )
		{
			wcsncat( str, pszLocalized, ARRAYSIZE( str ) );
		}
		else
		{
			wchar_t* pszLocalized = vgui2::localize()->Find( "#Career_EnemyList" );
			vgui2::localize()->ConstructString(
				str, sizeof( str ),
				pszLocalized, 1,
				intWStr
			);
		}

		wcsncat( str, L"\n", ARRAYSIZE( str ) );

		if( !pMap->m_bots.empty() )
		{
			for( int i = 0; i < numBots; ++i )
			{
				snprintf( botStr, ARRAYSIZE( botStr ), " %s", pMap->m_bots[ i ]->GetName() );

				vgui2::localize()->ConvertANSIToUnicode( botStr, botWStr, sizeof( botWStr ) );

				wcsncat( str, botWStr, ARRAYSIZE( str ) );

				if( i + 1 < numBots )
					wcsncat( str, vgui2::localize()->Find( "#Career_EnemyListSeparator" ), ARRAYSIZE( str ) );
			}
		}

		wcsncat( str, L"\n", ARRAYSIZE( str ) );

		wcsncat( str, vgui2::localize()->Find( "#Career_TasksColon" ), ARRAYSIZE( str ) );
		wcsncat( str, L"\n", ARRAYSIZE( str ) );

		for( auto pTask : pMap->m_tasks )
		{
			if( !wcslen( pTask->GetLocalizedTaskNameWithCompletion() ) )
				continue;

			wcsncat( str, pTask->GetLocalizedTaskNameWithCompletion(), ARRAYSIZE( str ) );
			wcsncat( str, L"\n", ARRAYSIZE( str ) );
		}

		pText->SetText( L"" );
		pText->InsertColorChange( normalColor );
		pText->InsertString( str );

		if( pMap->m_allowTeammates )
		{
			if( pMap->IsFriendlyFireOn() )
			{
				pText->InsertColorChange( friendlyFireColor );
				pText->InsertString( "#Career_FriendlyFireOn" );
			}
		}
		else
		{
			pText->InsertColorChange( friendlyFireColor );
			pText->InsertString( "#Career_NoTeammates" );
		}
	}
	else
	{
		pText->SetText( L"" );
	}
}