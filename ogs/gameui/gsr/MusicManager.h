#ifndef GAMEUI_MUSICMANAGER_H
#define GAMEUI_MUSICMANAGER_H

#include "GameUI/IMusicManager.h"

class MusicManager : public IMusicManager
{
private:
	struct TrackInfo
	{
		char t_path[ MAX_PATH ];
		bool looping;
	};

public:
	MusicManager();
	~MusicManager();

	void StartTrack( TrackType track, bool looping ) override;
	void StopTrack() override;
	void FadeOutTrack() override;
	bool IsPlayingTrack() override;
	bool IsPlayingTrack( TrackType track ) override;

	void Init();

	void Reset();

private:
	TrackType m_currentTrack = TRACKTYPE_INVALID;
	TrackInfo m_tracks[ TRACKTYPE_NUM ];

	bool m_useLoopingInfo = false;

private:
	MusicManager( const MusicManager& ) = delete;
	MusicManager& operator=( const MusicManager& ) = delete;
};

extern MusicManager* TheMusicManager;

void CreateMusicManager();

void ShutdownMusicManager();

#endif //GAMEUI_MUSICMANAGER_H
