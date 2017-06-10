#ifndef GAMEUI_SAVEGAMEDIALOG_H
#define GAMEUI_SAVEGAMEDIALOG_H

#include <vgui_controls/ListPanel.h>

#include "taskframe.h"

#define SAVEGAME_MAPNAME_LEN 32
#define SAVEGAME_COMMENT_LEN 80
#define SAVEGAME_ELAPSED_LEN 32

class KeyValues;

int TimeStampSortFunc( vgui2::ListPanel* pPanel, const vgui2::ListPanelItem& p1, const vgui2::ListPanelItem& p2 );

int SaveReadNameAndComment( FileHandle_t f, char *name, char *comment );

class CSaveGameDialog : public CTaskFrame
{
public:
	using BaseClass = CTaskFrame;

	CSaveGameDialog( vgui2::Panel* parent );

	void OnClose() override;

	void OnCommand( const char* command ) override;

private:
	void CreateSavedGamesList();
	bool ParseSaveData( const char* pszFileName, const char* pszShortName, KeyValues* kv );
	void ScanSavedGames();

	const char* FindSaveSlot();

private:
	vgui2::ListPanel* m_pGameList;

private:
	CSaveGameDialog( const CSaveGameDialog& ) = delete;
	CSaveGameDialog& operator=( const CSaveGameDialog& ) = delete;
};

#endif //GAMEUI_SAVEGAMEDIALOG_H
