#include <tier0/platform.h>

#include <vgui/ILocalize.h>
#include <vgui_controls/Controls.h>

#include "CareerGameTasks.h"
#include "shared_util.h"

/**
*	Constructs a string with an out-of-order format string
*/
void ConstructStringWithReordering( wchar_t *unicodeOutput, int unicodeBufferSizeInBytes, wchar_t *formatString, int numFormatParameters, ... )
{
	va_list va;

	va_start( va, numFormatParameters );
	if( formatString )
	{
		wchar_t** fragments = new wchar_t*[ numFormatParameters ];

		for( int i = 0; i < numFormatParameters; ++i )
		{
			fragments[ i ] = va_arg( va, wchar_t* );
		}

		wchar_t* tmp = new wchar_t[ wcslen( formatString ) + 1 ];
		wcscpy( tmp, formatString );

		auto pszData = tmp;

		for( int i = 0; i < numFormatParameters; ++i )
		{
			pszData = wcsstr( pszData, L"%" );

			if( !pszData )
				break;

			const auto c = pszData[ 2 ];

			if( c < L'0' || c > L'9' )
				break;

			const auto index = c - L'1';

			if( index < numFormatParameters )
			{
				auto ppszCurrent = &fragments[ i ];
				auto ppszTarget = &fragments[ index ];

				auto pszTemp = *ppszCurrent;
				*ppszCurrent = *ppszTarget;
				*ppszTarget = pszTemp;

				wchar_t unicodePS[ 4 ] = { L'%', L's', static_cast<wchar_t>( i + L'1' ), L'\0' };

				pszData[ 2 ] = unicodePS[ 2 ];
				pszData += 3;

				auto pszReplace = wcsstr( pszData, unicodePS );

				if( pszReplace )
					pszReplace[ 2 ] = c;
			}
		}

		auto uiBufferSpaceLeft = static_cast<unsigned int>( unicodeBufferSizeInBytes / sizeof( wchar_t ) );
		
		char searchStr[ 32 ];
		wchar_t unicodePS[ 4 ];

		int nextFragment = 0;
		wchar_t* pszDest = unicodeOutput;
		const wchar_t* pszNext = tmp;

		for( int i = 0; ; ++i )
		{
			searchStr[ 0 ] = '%';
			searchStr[ 1 ] = 's';
			searchStr[ 2 ] = i + '1';
			searchStr[ 3 ] = '\0';

			vgui2::localize()->ConvertANSIToUnicode( searchStr, unicodePS, sizeof( unicodePS ) );

			auto pszNextParam = wcsstr( pszNext, unicodePS );

			if( !pszNextParam )
			{
				const auto uiToCopy = min( uiBufferSpaceLeft, wcslen( pszNext ) + 1 );

				wcsncpy( pszDest, pszNext, uiToCopy );
				pszDest[ uiBufferSpaceLeft - 1 ] = L'\0';
				break;
			}
		
			{
				const auto uiToCopy = min( uiBufferSpaceLeft, static_cast<size_t>( pszNextParam - pszNext ) );

				wcsncpy( pszDest, pszNext, uiToCopy );
				uiBufferSpaceLeft -= uiToCopy;
				pszDest += uiToCopy;
				pszNext += uiToCopy;
			}

			if( i < numFormatParameters )
			{
				auto s = fragments[ nextFragment ];

				if( !s )
				{
					break;
				}

				++nextFragment;

				const auto uiToCopy = min( uiBufferSpaceLeft, wcslen( s ) );

				//Skip the format parameter
				pszNext += 3;
				wcsncpy( pszDest, s, uiToCopy );
				pszDest += uiToCopy;
				uiBufferSpaceLeft -= uiToCopy;
			}
		}

		if( tmp )
			delete[] tmp;

		if( fragments )
			delete[] fragments;
	}
	else
	{
		*unicodeOutput = L'\0';
	}

	va_end( va );
}

CCareerUITask::CCareerUITask( const char* taskName )
{
	strncpy( m_name, taskName, ARRAYSIZE( m_name ) );
	m_name[ ARRAYSIZE( m_name ) - 1 ] = '\0';

	m_name[ 0 ] = '\0';
	m_weapon[ 0 ] = '\0';

	const char* pszBuffer = taskName;
	const char* s1;

	while( SharedTokenWaiting( pszBuffer ) )
	{
		pszBuffer = SharedParse( pszBuffer );

		s1 = SharedGetToken();

		if( !s1 )
			continue;

		if( !stricmp( s1, "inarow" ) )
		{
			m_inarow = true;
			m_survive = false;
		}
		else if( !stricmp( s1, "survive" ) )
		{
			m_survive = true;
			m_inarow = false;
		}
		else
		{
			const auto iValue = strtol( s1, nullptr, 10 );

			if( iValue )
			{
				m_repeat = iValue;
			}
			else if( !m_name[ 0 ] )
			{
				strncpy( m_name, s1, ARRAYSIZE( m_name ) );
				m_name[ ARRAYSIZE( m_name ) - 1 ] = '\0';
			}
			else if( !m_weapon[ 0 ] )
			{
				strncpy( m_weapon, s1, ARRAYSIZE( m_weapon ) );
				m_weapon[ ARRAYSIZE( m_weapon ) - 1 ] = '\0';
			}
		}
	}

	if( !stricmp( m_weapon, "grenade" ) && m_repeat > 1 )
	{
		strncpy( m_weapon, "hegren", ARRAYSIZE( m_weapon ) );
		m_weapon[ ARRAYSIZE( m_weapon ) - 1 ] = '\0';
	}
	else if( !stricmp( m_weapon, "hegren" ) && m_repeat == 1 )
	{
		strncpy( m_weapon, "grenade", ARRAYSIZE( m_weapon ) );
		m_weapon[ ARRAYSIZE( m_weapon ) - 1 ] = '\0';
	}

	m_localizedName[ 0 ] = '\0';

	char taskIdStr[ 144 ];
	sprintf( taskIdStr, "#Career_Weapon_%s", m_weapon );

	auto pszLocalized = vgui2::localize()->Find( taskIdStr );

	if( !pszLocalized )
		pszLocalized = L"";

	wchar_t numBuf[ 16 ];
	swprintf( numBuf, ARRAYSIZE( numBuf ), L"%d", this->m_repeat );

	if( m_repeat == 1 )
		sprintf( taskIdStr, "#Career_Task_%s", m_name );
	else
		sprintf( taskIdStr, "#Career_Task_%s_multi", m_name );

	if( !stricmp( m_name, "winfast" ) )
		m_isWinFast = true;

	wchar_t tmpTask[ 512 ];
	ConstructStringWithReordering( tmpTask, ARRAYSIZE( tmpTask ), vgui2::localize()->Find( taskIdStr ), 2, pszLocalized, numBuf );
	
	if( m_inarow )
	{
		//TODO: should be sizeof( m_localizedName ) - Solokiller
		vgui2::localize()->ConstructString(
			m_localizedName, ARRAYSIZE( m_localizedName ),
			vgui2::localize()->Find( "#Career_Task_inarow" ), 1,
			tmpTask
		);
	}
	else if( m_survive )
	{
		//TODO: should be sizeof( m_localizedName ) - Solokiller
		vgui2::localize()->ConstructString(
			m_localizedName, ARRAYSIZE( m_localizedName ),
			vgui2::localize()->Find( "#Career_Task_survive" ), 1,
			tmpTask
		);
	}
	else
	{
		wcscpy( m_localizedName, tmpTask );
	}

	vgui2::localize()->ConstructString(
		m_localizedNameCompleted, sizeof( m_localizedNameCompleted ),
		vgui2::localize()->Find( "#Career_Task_Done" ), 1,
		m_localizedName
	);

	m_completedThisRound = false;
	m_isComplete = false;
	m_partials = 0;
}

const wchar_t* CCareerUITask::GetLocalizedTaskNameWithCompletion()
{
	if( m_isComplete )
		return m_localizedNameCompleted;
	else
		return m_localizedName;
}
