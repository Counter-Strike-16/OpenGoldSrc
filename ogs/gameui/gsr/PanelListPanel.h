//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef GAMEUI_PANELLISTPANEL_H
#define GAMEUI_PANELLISTPANEL_H

//From the Alien Swarm SDK - Solokiller

#include <vgui/VGUI2.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/ScrollBar.h>

class VScrollBarReversedButtons : public vgui2::ScrollBar
{
public:
	using BaseClass = vgui2::ScrollBar;

	VScrollBarReversedButtons( vgui2::Panel* parent, const char* panelName, bool vertical );
	~VScrollBarReversedButtons();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

private:
	VScrollBarReversedButtons( const VScrollBarReversedButtons& ) = delete;
	VScrollBarReversedButtons& operator=( const VScrollBarReversedButtons& ) = delete;
};

//-----------------------------------------------------------------------------
// Purpose: A list of variable height child panels
//-----------------------------------------------------------------------------
class CPanelListPanel : public vgui2::Panel
{
public:
	DECLARE_CLASS_SIMPLE( CPanelListPanel, vgui2::Panel );

public:
	typedef struct dataitem_s
	{
		// Always store a panel pointer
		vgui2::Panel* panel;
	} DATAITEM;

	CPanelListPanel( vgui2::Panel *parent, const char* panelName, bool inverseButtons = false );
	~CPanelListPanel();

	// DATA & ROW HANDLING
	// The list now owns the panel
	virtual int	computeVPixelsNeeded();
	virtual int AddItem( vgui2::Panel *panel );
	virtual int	GetItemCount();
	virtual vgui2::Panel* GetItem( int itemIndex ); // returns pointer to data the row holds
	virtual void RemoveItem( int itemIndex ); // removes an item from the table (changing the indices of all following items)
	virtual void DeleteAllItems(); // clears and deletes all the memory used by the data items

								   // career-mode UI wants to nudge sub-controls around
	void SetSliderYOffset( int pixels );

	// PAINTING
	virtual vgui2::Panel* GetCellRenderer( int row );

	MESSAGE_FUNC_INT( OnSliderMoved, "ScrollBarSliderMoved", position );

	virtual void ApplySchemeSettings( vgui2::IScheme *pScheme );

	vgui2::Panel* GetEmbedded()
	{
		return _embedded;
	}

protected:

	DATAITEM* GetDataItem( int itemIndex );

	virtual void PerformLayout();
	virtual void PaintBackground();
	virtual void OnMouseWheeled( int delta );

private:
	// list of the column headers
	vgui2::Dar<DATAITEM*> _dataItems;
	vgui2::ScrollBar* _vbar;
	vgui2::Panel* _embedded;

	int					_tableStartX;
	int					_tableStartY;
	int					_sliderYOffset;
};

#endif //GAMEUI_PANELLISTPANEL_H
