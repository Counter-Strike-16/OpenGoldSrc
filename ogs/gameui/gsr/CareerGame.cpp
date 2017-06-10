#include <FileSystem.h>
#include <vgui/IScheme.h>
#include <vgui_controls/Controls.h>

#include "CareerGame.h"
#include "EngineInterface.h"
#include "shared_util.h"
#include "bot_profile.h"

CCareerGame* TheCareerGame = nullptr;

EXPOSE_SINGLE_INTERFACE( CCareerGame, ICareerUI, CAREERUI_INTERFACE_VERSION );

CCareerCharacter::CCareerCharacter( const BotProfile* pProfile )
	: m_pProfile( pProfile )
{
}

void CCareerCharacter::Print()
{
	//Nothing
}

char* CCareerCharacter::Save( char* buf, int& len, int hired )
{
	if( !hired )
		return buf;

	return BufPrintf( buf, len, "hire \"%s\"\n", m_pProfile->GetName() );
}

int CCareerCharacter::GetPoints() const
{
	if( !m_pProfile )
		return 0;

	return m_pProfile->GetCost();
}

const char* CCareerCharacter::GetName() const
{
	if( !m_pProfile )
		return "";

	return m_pProfile->GetName();
}

bool CCareerCharacter::operator<( const CCareerCharacter& other ) const
{
#define GET_PROFILE_VALUE( name ) ( pOther ? pOther->Get##name() : 0 )

#define CHECK_PROFILE_VALUE( name )										\
	do																	\
	{																	\
		if( m_pProfile->Get##name() != GET_PROFILE_VALUE( name ) )		\
		{																\
			return m_pProfile->Get##name() < GET_PROFILE_VALUE( name );	\
		}																\
	}																	\
	while( false )

#define CHECK_PROFILE_RANGED_VALUE( name )								\
	do																	\
	{																	\
		if( GetRange( m_pProfile->Get##name() ) != 						\
			other.GetRange( GET_PROFILE_VALUE( name ) ) )				\
		{																\
			return m_pProfile->Get##name() < GET_PROFILE_VALUE( name );	\
		}																\
	}																	\
	while( false )

	if( !m_pProfile )
		return false;

	auto pOther = other.m_pProfile;

	CHECK_PROFILE_VALUE( Cost );
	CHECK_PROFILE_RANGED_VALUE( Skill );
	CHECK_PROFILE_RANGED_VALUE( Teamwork );
	CHECK_PROFILE_RANGED_VALUE( Aggression );

	return strcmp( m_pProfile->GetName(), pOther ? pOther->GetName() : "" ) < 0;

#undef GET_PROFILE_VALUE
#undef CHECK_PROFILE_VALUE
#undef CHECK_PROFILE_RANGED_VALUE
}

int CCareerCharacter::GetRange( float val ) const
{
	if( val >= 0.9 )
		return 4;

	if( val >= 0.65 )
		return 3;

	if( val >= 0.35 )
		return 2;

	if( val >= 0.1 )
		return 1;

	return 0;
}

const char* CCareerCharacter::GetSkinThumbnailFname() const
{
	static char imageFnameconst[ 4096 ];

	if( TheBotProfiles->GetCustomSkinModelname( m_pProfile->GetSkin() ) )
	{
		snprintf(
			imageFnameconst, ARRAYSIZE( imageFnameconst ),
			"gfx/thumbnails/skins/%s.tga",
			TheBotProfiles->GetCustomSkinModelname( m_pProfile->GetSkin() )
		);

		if( vgui2::filesystem()->FileExists( imageFnameconst ) )
		{
			//Strip extension
			imageFnameconst[ strlen( imageFnameconst ) - 4 ] = '\0';
			return imageFnameconst;
		}
	}

	if( TheCareerGame->PlayAsCT() )
	{
		switch( m_pProfile->GetSkin() )
		{
		case 1:		return "gfx/thumbnails/skins/urban";
		case 2:		return "gfx/thumbnails/skins/gsg9";
		case 3:		return "gfx/thumbnails/skins/sas";
		case 4:		return "gfx/thumbnails/skins/gign";
		case 5:		return "gfx/thumbnails/skins/spetsnaz";
		default:	return "gfx/thumbnails/skins/ct_random";
		}
	}
	else
	{
		switch( m_pProfile->GetSkin() )
		{
		case 1:		return "gfx/thumbnails/skins/terror";
		case 2:		return "gfx/thumbnails/skins/leet";
		case 3:		return "gfx/thumbnails/skins/arctic";
		case 4:		return "gfx/thumbnails/skins/guerilla";
		case 5:		return "gfx/thumbnails/skins/militia";
		default:	return "gfx/thumbnails/skins/t_random";
		}
	}
}

const char* CCareerCharacter::GetWeaponPreferenceAsString( int i ) const
{
	if( !m_pProfile )
		return nullptr;

	return m_pProfile->GetWeaponPreferenceAsString( i );
}

int CCareerCharacter::GetWeaponPreferenceCount() const
{
	if( !m_pProfile )
		return 0;

	return m_pProfile->GetWeaponPreferenceCount();
}

float CCareerCharacter::GetSkill() const
{
	if( !m_pProfile )
		return 0;

	return m_pProfile->GetSkill();
}

float CCareerCharacter::GetTeamwork() const
{
	if( !m_pProfile )
		return 0;

	return m_pProfile->GetTeamwork();
}

float CCareerCharacter::GetAggression() const
{
	if( !m_pProfile )
		return 0;

	return m_pProfile->GetAggression();
}

CCareerMap::CCareerMap( const char* name, const CharacterVec& bots, bool friendlyFire )
{
	strncpy( m_name, name, ARRAYSIZE( m_name ) );
	m_name[ ARRAYSIZE( m_name ) - 1 ] = '\0';

	m_bots = bots;
	m_friendlyFire = friendlyFire;
}

CCareerMap::~CCareerMap()
{
	for( auto pBot : m_bots )
	{
		delete pBot;
	}

	if( m_extra )
		delete[] m_extra;
}

int CCareerMap::GetNumBotNames()
{
	return m_bots.size();
}

const char* CCareerMap::GetBotName( int index )
{
	if( index < 0 || static_cast<size_t>( index ) >= m_bots.size() )
		return nullptr;

	auto pProfile = m_bots[ index ]->m_pProfile;

	if( pProfile )
		return pProfile->GetName();

	return "";
}

void CCareerMap::SetExtraInitialization( const char* extra )
{
	if( m_extra )
	{
		delete[] m_extra ;
		m_extra = nullptr;
	}

	if( extra )
	{
		m_extra = new char[ strlen( extra ) + 1 ];
		strcpy( m_extra, extra );
	}
}

bool CCareerMap::IsFriendlyFireOn()
{
	return m_friendlyFire;
}

int CCareerMap::GetCurrentEnemyCount() const
{
	int result = m_minEnemyCount;

	if( TheCareerGame->m_reputationPoints > m_threshold )
		result += TheCareerGame->m_reputationPoints - m_threshold;

	if( static_cast<size_t>( result ) > m_bots.size() )
		return m_bots.size();

	return result;
}

void CCareerMap::ResetTaskCompletion()
{
	for( auto pTask : m_tasks )
	{
		pTask->Reset();
	}
}

CCareerGame::CCareerGame()
{
}

CCareerGame::~CCareerGame()
{
}

bool CCareerGame::IsPlayingMatch()
{
	return engine->pfnIsCareerMatch() != 0;
}

ITaskVec* CCareerGame::GetCurrentTaskVec()
{
	if( m_pLastMap )
		return &m_pLastMap->m_tasks;

	return nullptr;
}

bool CCareerGame::DoesWinUnlockAll()
{
	//TODO: implement - Solokiller
	return false;
}

int CCareerGame::GetRoundTimeLength()
{
	return m_roundTimeLength;
}

int CCareerGame::GetCurrentMapTriplet( MapInfo* maps )
{
	//TODO: implement - Solokiller
	return 0;
}

void CCareerGame::OnRoundEndMenuOpen( bool didWin )
{
	//TODO: implement - Solokiller
}

void CCareerGame::OnMatchEndMenuOpen( bool didWin )
{
	//TODO: implement - Solokiller
}

void CCareerGame::OnRoundEndMenuClose( bool stillPlaying )
{
	//TODO: implement - Solokiller
}

void CCareerGame::OnMatchEndMenuClose( bool stillPlaying )
{
	//TODO: implement - Solokiller
}

void CCareerGame::Create()
{
	//TODO: implement - Solokiller
	if( !TheCareerGame )
	{
		vgui2::scheme()->LoadSchemeFromFile( "Resource/CareerScheme.res", "CareerScheme" );
		vgui2::scheme()->LoadSchemeFromFile( "Resource/CareerBoxScheme.res", "CareerBoxScheme" );
		//engine->pfnHookUserMsg( "CZCareer", __MsgFunc_CZCareer );
	}

	TheCareerGame = &__g_CCareerGame_singleton;

	/*
	CareerDefinitionManager::Create();

	TheBotProfiles = nullptr;
	if( TheCareerDefinitions->m_activeCareer )
	{
		TheBotProfiles = CareerDefinition::GetBotProfileManager( TheCareerDefinitions->m_activeCareer, TheCareerGame->GetDifficulty() );
	}
	*/
}

void CCareerGame::Reset()
{
	//TODO: implement - Solokiller
}

bool CCareerGame::LoadProfile( CareerDifficultyType difficulty )
{
	//TODO: implement - Solokiller
	return false;
}

void CCareerGame::Update()
{
	//TODO: implement - Solokiller
}

void CCareerGame::FinishMatchStart()
{
	//TODO: implement - Solokiller
}

void CCareerGame::StopMatch( int ctWins, int tWins )
{
	//TODO: implement - Solokiller
}

const char* CCareerGame::GetLastMap()
{
	return m_lastMapName;
}

CCareerMap* CCareerGame::GetLastMapPtr()
{
	return m_pLastMap;
}

//Dummy implementation for code shared with CStrike/CZero
void Career_Printf( const char* fmt, ... )
{
	//Nothing
}
