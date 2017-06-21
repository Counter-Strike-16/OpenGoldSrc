#ifndef GAMEUI_CAREERGAME_H
#define GAMEUI_CAREERGAME_H

#include <vector>

#include "GameUI/ICareerUI.h"

class BotProfile;
class BufferWriter;
class CCareerCharacter;
class CCareerMap;
class CCareerProfileData;
class CvarLatch;

typedef std::vector<CCareerCharacter*> CharacterVec;
typedef std::vector<int> CharCostVector;
typedef std::vector<CvarLatch*> CvarLatchVector;
typedef std::vector<std::vector<CCareerMap*>> MapsVec;

class CCareerCharacter
{
public:
	CCareerCharacter( const BotProfile* pProfile );

	void Print();

	char* Save( char* buf, int& len, int hired );

	int GetPoints() const;

	const char* GetName() const;

	bool operator<( const CCareerCharacter& other ) const;

	int GetRange( float val ) const;

	const char* GetSkinThumbnailFname() const;

	const char* GetWeaponPreferenceAsString( int i ) const;

	int GetWeaponPreferenceCount() const;

	float GetSkill() const;

	float GetTeamwork() const;

	float GetAggression() const;

public:
	const BotProfile* m_pProfile;
};

class CCareerMap
{
public:
	CCareerMap( const char* name, const CharacterVec& bots, bool friendlyFire );
	~CCareerMap();

	virtual int GetNumBotNames();

	virtual const char* GetBotName( int index );

	void SetExtraInitialization( const char* extra );

	bool IsFriendlyFireOn();

	int GetCurrentEnemyCount() const;

	void ResetTaskCompletion();

public:
	char m_name[ 64 ];
	bool m_defeated = false;
	bool m_friendlyFire;
	CharacterVec m_bots;
	ITaskVec m_tasks;
	int m_minEnemyCount;
	int m_threshold;
	char* m_extra = nullptr;
	bool m_allowTeammates = true;

private:
	CCareerMap( const CCareerMap& ) = delete;
	CCareerMap& operator=( const CCareerMap& ) = delete;
};

class CCareerGame : public ICareerUI
{
public:
	CCareerGame();
	~CCareerGame();

	bool IsPlayingMatch() override;

	ITaskVec* GetCurrentTaskVec() override;

	bool PlayAsCT() override { return m_isCT != 0; }
	int GetReputationGained() override { return m_matchReputationGain; }
	int GetNumMapsUnlocked() override { return m_matchMapsUnlocked; }
	bool DoesWinUnlockAll() override;
	int GetRoundTimeLength() override;
	int GetWinfastLength() override { return m_winfastLength; }

	CareerDifficultyType GetDifficulty() const override { return m_difficulty; }
	int GetCurrentMapTriplet( MapInfo* maps ) override;

	void OnRoundEndMenuOpen( bool didWin ) override;
	void OnMatchEndMenuOpen( bool didWin ) override;

	void OnRoundEndMenuClose( bool stillPlaying ) override;
	void OnMatchEndMenuClose( bool stillPlaying ) override;

	static void Create();

	void Reset();

	bool LoadProfile( CareerDifficultyType difficulty );

	void Update();

	void FinishMatchStart();

	void StopMatch( int ctWins, int tWins );

	const char* GetLastMap();

	CCareerMap* GetLastMapPtr();

	static const char* GetSaveGamePlayerName( const char* name );

	CCareerProfileData* GetProfileForName( const char* profileName );

public:
	CCareerProfileData* m_currentProfile;
	BufferWriter* m_writer;

	int m_reputationPoints = 0;
	int m_reputationPointsFlashed = 0;

	CharCostVector m_costAvailability;

	bool m_queuedLoadMenu = false;
	char m_queuedName[ 32 ] = { '\0' };

	bool m_wasMapDefeatedAtStart;

	bool m_needAutosave;

	CharacterVec m_hiredCharacters;
	CharacterVec m_availableCharacters;

	MapsVec m_maps;

	bool m_isPlaying;
	char m_lastMapName[ 64 ] = { '\0' };
	CCareerMap* m_pLastMap = nullptr;
	int m_basePoints;
	int m_roundTime;
	int m_bombCountdownTime;
	int m_freezeTime;
	int m_isCT = 1;
	bool m_initDone = false;
	int m_botPoolSliderPosition;
	int m_matchNumWins;
	int m_matchWinBy;
	int m_matchReputationGain;
	int m_matchMapsUnlocked;
	int m_roundTimeLength;
	int m_winfastLength;

	CvarLatchVector m_savedCvars;
	CareerDifficultyType m_difficulty;

	bool m_medals[ 4 ];

	bool m_canUnpause;

	int m_ctWins;
	int m_tWins;

private:
	CCareerGame( const CCareerGame& ) = delete;
	CCareerGame& operator=( const CCareerGame& ) = delete;
};

extern CCareerGame* TheCareerGame;

void Career_Printf( const char* fmt, ... );

#endif //GAMEUI_CAREERGAME_H
