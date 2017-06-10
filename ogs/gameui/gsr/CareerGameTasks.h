#ifndef GAMEUI_CAREERGAMETASKS_H
#define GAMEUI_CAREERGAMETASKS_H

#include "GameUI/CareerDefs.h"

class CCareerUITask : public ICareerTask
{
public:
	CCareerUITask( const char* taskName );

	bool IsComplete() override { return m_isComplete; }
	bool IsCompletedThisRound() override { return m_completedThisRound; }
	bool IsCompletedThisMatch() override { return m_isComplete; }

	const char* GetMap() override { return ""; }

	const char* GetTaskName() override { return m_name; }
	const wchar_t* GetLocalizedTaskName() override { return m_localizedName; }
	const wchar_t* GetLocalizedTaskNameWithCompletion() override;

	void Reset() override
	{
		m_completedThisRound = false;
		m_isComplete = false;
		m_partials = 0;
	}

	void Set( bool thisRound ) override
	{
		m_completedThisRound = true;
		m_isComplete = true;
		m_partials = 0;
	}

	void StartRound() override
	{
		m_completedThisRound = false;
	}

	const char* GetWeaponName() const override { return m_weapon; }

	int GetRepeat() const override { return m_repeat; }
	bool MustSurvive() const override { return m_survive; }
	bool InARow() const override { return m_inarow; }
	int GetPartial() const override { return m_partials; }
	void SetPartial( int num ) override { m_partials = num; }
	bool IsMaxRoundTime() const override { return m_isWinFast; }

private:
	bool m_isComplete;
	bool m_completedThisRound;

	char m_name[ 128 ];
	char m_weapon[ 128 ];

	wchar_t m_localizedName[ 512 ];
	wchar_t m_localizedNameCompleted[ 512 ];

	int m_repeat = 1;
	bool m_survive = false;
	bool m_inarow = false;
	int m_partials;
	bool m_isWinFast = false;

private:
	CCareerUITask( const CCareerUITask& ) = delete;
	CCareerUITask& operator=( const CCareerUITask& ) = delete;
};


#endif //GAMEUI_CAREERGAMETASKS_H
