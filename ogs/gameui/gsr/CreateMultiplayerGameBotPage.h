//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef GAMEUI_CREATEMULTIPLAYERGAMEBOTPAGE_H
#define GAMEUI_CREATEMULTIPLAYERGAMEBOTPAGE_H

#include <vgui_controls/PropertyPage.h>

class CPanelListPanel;
class CDescription;
class mpcontrol_t;
class CCvarToggleCheckButton;

//-----------------------------------------------------------------------------
// Purpose: advanced bot properties page of the create game server dialog
//-----------------------------------------------------------------------------
class CCreateMultiplayerGameBotPage : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( CCreateMultiplayerGameBotPage, vgui2::PropertyPage );

public:
	CCreateMultiplayerGameBotPage( vgui2::Panel *parent, const char *name, KeyValues *botKeys );
	~CCreateMultiplayerGameBotPage();

	void UpdateKeys( KeyValues* botKeys );

protected:
	virtual void OnResetChanges();
	virtual void OnApplyChanges();

private:
	CCvarToggleCheckButton *m_joinAfterPlayer;

	CCvarToggleCheckButton *m_allowRogues;

	CCvarToggleCheckButton *m_allowPistols;
	CCvarToggleCheckButton *m_allowShotguns;
	CCvarToggleCheckButton *m_allowSubmachineGuns;
	CCvarToggleCheckButton *m_allowMachineGuns;
	CCvarToggleCheckButton *m_allowRifles;
	CCvarToggleCheckButton *m_allowGrenades;
#ifdef CS_SHIELD_ENABLED
	CCvarToggleCheckButton *m_allowShields;
#endif // CS_SHIELD_ENABLED
	CCvarToggleCheckButton *m_allowSnipers;

	CCvarToggleCheckButton *m_deferToHuman;

	vgui2::ComboBox *m_joinTeamCombo;
	void SetJoinTeamCombo( const char *team );

	vgui2::ComboBox *m_chatterCombo;
	void SetChatterCombo( const char *team );

	vgui2::TextEntry *m_quotaEntry;

	vgui2::TextEntry *m_prefixEntry;
};

#endif //GAMEUI_CREATEMULTIPLAYERGAMEBOTPAGE_H
