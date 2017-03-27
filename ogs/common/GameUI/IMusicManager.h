// original Valve music manager interface (exported from gameui module)
// looks like it's used to play the cdtrack entries...

#pragma once

#include "public/interface.h"

const char MUSICMANAGER_INTERFACE_VERSION[] = "MusicManager001";

//-----------------------------------------------------------------------------
// Purpose: ???
//-----------------------------------------------------------------------------
class IMusicManager : public IBaseInterface
{
public:
	// virtual destructor from IBaseInterface

	// param1 must be one of [2, 3, 4, 5, 6, 7]
	// param2 is "looping" if non-zero
	#define MUSICMANAGER_NOT_LOOPING 0
	#define MUSICMANAGER_LOOPING 1
	virtual void StartTrack( int track, char looping ) = 0;

	virtual void StopTrack() = 0;

	virtual void FadeoutTrack() = 0;

	virtual bool IsPlayingTrack( int track ) = 0;

	// Returns whether current track is number 1 ???
	virtual bool Unknown1() = 0;
};