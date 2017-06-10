#ifndef GAMEUI_REFRESHSTEAMLOGIN_H
#define GAMEUI_REFRESHSTEAMLOGIN_H

#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/TextEntry.h>

#include "taskframe.h"

class CRefreshSteamLogin : public CTaskFrame
{
public:
	DECLARE_CLASS_SIMPLE( CRefreshSteamLogin, CTaskFrame );

public:
	CRefreshSteamLogin( vgui2::Panel* parent, const char* username );
	virtual ~CRefreshSteamLogin();

	void Activate() override;

	void OnThink() override;

	void OnClose() override;

	void OnCommand( const char* command ) override;

private:
	vgui2::TextEntry* m_pUserName;
	vgui2::TextEntry* m_pPassword;

	vgui2::CheckButton* m_pSavePassword;
	vgui2::Button* m_pLoginButton;

	bool m_bLoggingIn = false;

	vgui2::VPANEL m_RestrictedPaintPanel;

private:
	CRefreshSteamLogin( const CRefreshSteamLogin& ) = delete;
	CRefreshSteamLogin& operator=( const CRefreshSteamLogin& ) = delete;
};


#endif //GAMEUI_REFRESHSTEAMLOGIN_H
