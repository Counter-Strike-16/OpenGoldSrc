#include <tier0/platform.h>
#include <KeyValues.h>

#include <vgui_controls/Controls.h>

#include "CareerGame.h"
#include "EngineInterface.h"
#include "MusicManager.h"

MusicManager* TheMusicManager = nullptr;

EXPOSE_SINGLE_INTERFACE( MusicManager, IMusicManager, MUSICMANAGER_INTERFACE_VERSION );

MusicManager::MusicManager()
{
	memset( m_tracks, 0, sizeof( m_tracks ) );
}

MusicManager::~MusicManager()
{
}

void MusicManager::StartTrack( TrackType track, bool looping )
{
	if( m_currentTrack != track && TRACKTYPE_INVALID < track && track < TRACKTYPE_NUM )
	{
		auto& info = m_tracks[ track - 1 ];

		if( info.t_path[ 0 ] )
		{
			if( m_useLoopingInfo )
				looping = info.looping;

			Career_Printf( "MusicManager::StartTrack - playing track %d %s\n", track, looping ? "as looping" : "" );

			engine->pfnPrimeMusicStream( info.t_path, looping );

			char s[ 140 ];
			snprintf( s, ARRAYSIZE( s ), "cd %s %s\n", looping ? "loopfile" : "playfile", info.t_path );

			engine->pfnClientCmd( s );

			m_currentTrack = track;
		}
	}
}

void MusicManager::StopTrack()
{
	if( m_currentTrack != TRACKTYPE_INVALID )
	{
		engine->pfnClientCmd( "cd stop" );
		m_currentTrack = TRACKTYPE_INVALID;
	}
}

void MusicManager::FadeOutTrack()
{
	if( m_currentTrack != TRACKTYPE_INVALID )
	{
		engine->pfnClientCmd( "cd fadeout" );
		m_currentTrack = TRACKTYPE_INVALID;
	}
}

bool MusicManager::IsPlayingTrack()
{
	return m_currentTrack != TRACKTYPE_INVALID;
}

bool MusicManager::IsPlayingTrack( TrackType track )
{
	return m_currentTrack == track;
}

void MusicManager::Init()
{
	Reset();

	auto pKV = new KeyValues( "Playlist" );

	if( pKV->LoadFromFile( vgui2::filesystem(), "playlist.txt" ) )
	{
		if( pKV->GetInt( "Version", 1 ) > 1 )
			m_useLoopingInfo = true;

#define INIT_TRACKINFO( type, name, defaultLooping )								\
		strncpy(																	\
			m_tracks[ type ].t_path,												\
			pKV->GetString( name ),													\
			ARRAYSIZE( m_tracks[ type ].t_path )									\
		);																			\
																					\
		m_tracks[ type ].looping = pKV->GetInt( name "Loop", defaultLooping ) != 0

		INIT_TRACKINFO( TRACKTYPE_SPLASH, "Splash", true );
		INIT_TRACKINFO( TRACKTYPE_ROUNDWIN, "RoundWin", false );
		INIT_TRACKINFO( TRACKTYPE_ROUNDLOSE, "RoundLose", false );
		INIT_TRACKINFO( TRACKTYPE_MATCHWIN, "MatchWin", false );
		INIT_TRACKINFO( TRACKTYPE_MATCHLOSE, "MatchLose", false );
		INIT_TRACKINFO( TRACKTYPE_CREDITS, "Credits", true );

#undef INIT_TRACKINFO
	}

	pKV->deleteThis();
}

void MusicManager::Reset()
{
	if( m_currentTrack != TRACKTYPE_INVALID )
	{
		StopTrack();
		m_currentTrack = TRACKTYPE_INVALID;
	}
}

void CreateMusicManager()
{
	TheMusicManager = &__g_MusicManager_singleton;
	TheMusicManager->Init();
}

void ShutdownMusicManager()
{
	TheMusicManager = nullptr;
}
