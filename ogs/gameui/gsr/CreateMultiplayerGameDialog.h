#ifndef GAMEUI_CREATEMULTIPLAYERGAMEDIALOG_H
#define GAMEUI_CREATEMULTIPLAYERGAMEDIALOG_H

#include <vgui_controls/PropertyDialog.h>

class CCreateMultiplayerGameServerPage;
class CCreateMultiplayerGameGameplayPage;
class CCreateMultiplayerGameBotPage;

//-----------------------------------------------------------------------------
// Purpose: dialog for launching a listenserver
//-----------------------------------------------------------------------------
class CCreateMultiplayerGameDialog : public vgui2::PropertyDialog
{
	DECLARE_CLASS_SIMPLE( CCreateMultiplayerGameDialog, vgui2::PropertyDialog );

public:
	CCreateMultiplayerGameDialog( vgui2::Panel *parent );
	~CCreateMultiplayerGameDialog();

protected:
	void SetTitle( const char* title, bool surfaceTitle ) override;

	virtual bool OnOK( bool applyOnly );

	void OnClose() override;

private:
	CCreateMultiplayerGameServerPage *m_pServerPage;
	CCreateMultiplayerGameGameplayPage *m_pGameplayPage;
	CCreateMultiplayerGameBotPage *m_pBotPage;

	bool m_bBotsEnabled;

	// for loading/saving game config
	KeyValues *m_pBotSavedData;
};

#endif //GAMEUI_CREATEMULTIPLAYERGAMEDIALOG_H
