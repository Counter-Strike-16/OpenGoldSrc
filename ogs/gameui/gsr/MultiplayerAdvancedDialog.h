//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef GAMEUI_MULTIPLAYERADVANCEDDIALOG_H
#define GAMEUI_MULTIPLAYERADVANCEDDIALOG_H

#include <vgui_controls/Frame.h>
#include <vgui_controls/PropertyPage.h>
#include "ScriptObject.h"
#include <vgui/KeyCode.h>

#include "taskframe.h"

class CMultiplayerAdvanced
{
public:
	void CreateControls();
	void DestroyControls();
	void GatherCurrentValues();
	void SaveValues();

protected:
	CInfoDescription* m_pDescription;
	mpcontrol_t* m_pList;
	CPanelListPanel* m_pListPanel;
};

//-----------------------------------------------------------------------------
// Purpose: Displays a game-specific list of options
//-----------------------------------------------------------------------------
class CMultiplayerAdvancedDialog : public CTaskFrame, public CMultiplayerAdvanced
{
	DECLARE_CLASS_SIMPLE( CMultiplayerAdvancedDialog, CTaskFrame );

public:
	CMultiplayerAdvancedDialog( vgui2::Panel *parent );
	~CMultiplayerAdvancedDialog();

private:
	virtual void OnCommand( const char *command );
	virtual void OnClose();
};

class CMultiplayerAdvancedPage : public vgui2::PropertyPage, public CMultiplayerAdvanced
{
public:
	using BaseClass = vgui2::PropertyPage;

	CMultiplayerAdvancedPage( vgui2::Panel* parent );

	void OnResetData() override;

	void OnApplyChanges() override;

private:
	CMultiplayerAdvancedPage( const CMultiplayerAdvancedPage& ) = delete;
	CMultiplayerAdvancedPage& operator=( const CMultiplayerAdvancedPage& ) = delete;
};

#endif // GAMEUI_MULTIPLAYERADVANCEDDIALOG_H
