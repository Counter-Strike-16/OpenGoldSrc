//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef GAMEUI_CREATEMULTIPLAYERGAMESERVERPAGE_H
#define GAMEUI_CREATEMULTIPLAYERGAMESERVERPAGE_H

#include <UtlRBTree.h>
#include <UtlSymbol.h>

#include <vgui_controls/PropertyPage.h>
#include "cvartogglecheckbutton.h"

bool CaselessStringLessThan( const CUtlSymbol& lhs, const CUtlSymbol& rhs );

//-----------------------------------------------------------------------------
// Purpose: server options page of the create game server dialog
//-----------------------------------------------------------------------------
class CCreateMultiplayerGameServerPage : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( CCreateMultiplayerGameServerPage, vgui2::PropertyPage );

public:
	CCreateMultiplayerGameServerPage( vgui2::Panel *parent, const char *name );
	~CCreateMultiplayerGameServerPage();

	// returns currently entered information about the server
	void SetMap( const char *name );
	bool IsRandomMapSelected();
	const char *GetMapName();

	// CS Bots
	int GetBotQuota();
	void SetBotQuota( int quota );
	bool GetBotsEnabled();
	void SetBotsEnabled( bool enabled );

protected:
	virtual void OnApplyChanges();
	MESSAGE_FUNC( OnCheckButtonChecked, "CheckButtonChecked" );

private:
	void LoadMapList();
	void LoadMaps( const char *pszPathID );

	vgui2::ComboBox *m_pMapList;
	vgui2::TextEntry* m_pBotQuotaCombo;
	vgui2::CheckButton *m_pEnableBotsCheck;
	CCvarToggleCheckButton *m_pEnableTutorCheck;

	enum { DATA_STR_LENGTH = 64 };
	char m_szHostName[ DATA_STR_LENGTH ];
	char m_szPassword[ DATA_STR_LENGTH ];
	char m_szMapName[ DATA_STR_LENGTH ];

	int m_iMaxPlayers;

	CUtlRBTree<CUtlSymbol, short> m_MapNames;
};

#endif //GAMEUI_CREATEMULTIPLAYERGAMESERVERPAGE_H
