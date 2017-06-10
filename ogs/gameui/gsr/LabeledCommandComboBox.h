//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef GAMEUI_LABELEDCOMMANDCOMBOBOX_H
#define GAMEUI_LABELEDCOMMANDCOMBOBOX_H

//From the Alien Swarm SDK - Solokiller

#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Panel.h>
#include "UtlVector.h"

class CLabeledCommandComboBox : public vgui2::ComboBox
{
	DECLARE_CLASS_SIMPLE( CLabeledCommandComboBox, vgui2::ComboBox );

public:
	CLabeledCommandComboBox( vgui2::Panel *parent, const char *panelName );
	~CLabeledCommandComboBox();

	virtual void DeleteAllItems();
	virtual void AddItem( char const *text, char const *engineCommand );
	virtual void ActivateItem( int itemIndex );
	const char *GetActiveItemCommand();

	void SetInitialItem( int itemIndex );

	void			ApplyChanges();
	void			Reset();
	bool			HasBeenModified();

	enum
	{
		MAX_NAME_LEN = 256,
		MAX_COMMAND_LEN = 256
	};

private:
	MESSAGE_FUNC_CHARPTR( OnTextChanged, "TextChanged", text );

	struct COMMANDITEM
	{
		char			name[ MAX_NAME_LEN ];
		char			command[ MAX_COMMAND_LEN ];
		int				comboBoxID;
	};

	CUtlVector< COMMANDITEM >	m_Items;
	int		m_iCurrentSelection;
	int		m_iStartSelection;
};

#endif //GAMEUI_LABELEDCOMMANDCOMBOBOX_H
