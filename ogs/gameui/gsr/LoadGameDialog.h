#ifndef GAMEUI_LOADGAMEDIALOG_H
#define GAMEUI_LOADGAMEDIALOG_H

#include "taskframe.h"

class CLoadGameDialog : public CTaskFrame
{
public:
	using BaseClass = CTaskFrame;

	CLoadGameDialog( vgui2::Panel* parent );

	void OnClose() override;

	void OnCommand( const char* command ) override;

private:
	void CreateSavedGamesList();
	bool ParseSaveData( char const* pszFileName, char const* pszShortName, KeyValues* kv );
	void ScanSavedGames();

private:
	vgui2::ListPanel* m_pGameList;

private:
	CLoadGameDialog( const CLoadGameDialog& ) = delete;
	CLoadGameDialog& operator=( const CLoadGameDialog& ) = delete;
};

#endif //GAMEUI_LOADGAMEDIALOG_H
