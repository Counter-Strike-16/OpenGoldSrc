//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <stdarg.h>
#include <stdio.h>

#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <KeyValues.h>

#include <vgui_controls/Image.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/TextImage.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui2;

DECLARE_BUILD_FACTORY_DEFAULT_TEXT( CheckButton, CheckButton );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CheckButton::CheckButton(Panel *parent, const char *panelName, const char *text) : ToggleButton(parent, panelName, text)
{
 	SetContentAlignment(a_west);
	m_bCheckButtonCheckable = true;

	// create the images
	m_pSelected = vgui2::scheme()->GetImage( "resource/icon_checked", false );
	m_pDefault = vgui2::scheme()->GetImage( "resource/icon_emptybox", false );

	_selectedFgColor = SDK_Color( 196, 181, 80, 255 );
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CheckButton::~CheckButton()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CheckButton::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetDefaultColor( GetSchemeColor("FgColor", pScheme), GetBgColor() );
	_selectedFgColor = GetSchemeColor("BrightControlText", GetSchemeColor("ControlText", pScheme), pScheme);

	SetPaintBackgroundEnabled( true );

	SetTextImageIndex( 1 );

	auto pImg = IsSelected() ? m_pSelected : m_pDefault;

	if( pImg )
		SetImageAtIndex( 0, pImg, 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
IBorder *CheckButton::GetBorder(bool depressed, bool armed, bool selected, bool keyfocus)
{
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Check the button
//-----------------------------------------------------------------------------
void CheckButton::SetSelected(bool state)
{
	if (m_bCheckButtonCheckable)
	{
		// send a message saying we've been checked
		KeyValues *msg = new KeyValues("CheckButtonChecked", "state", (int)state);
		PostActionSignal(msg);

		BaseClass::SetSelected(state);

		auto pImg = state ? m_pSelected : m_pDefault;

		if( pImg )
			SetImageAtIndex( 0, pImg, 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: sets whether or not the state of the check can be changed
//-----------------------------------------------------------------------------
void CheckButton::SetCheckButtonCheckable(bool state)
{
	m_bCheckButtonCheckable = state;
	Repaint();
}

//-----------------------------------------------------------------------------
// Purpose: Gets a different foreground text color if we are selected
//-----------------------------------------------------------------------------
SDK_Color CheckButton::GetButtonFgColor()
{
	if (IsSelected())
	{
		return _selectedFgColor;
	}

	return BaseClass::GetButtonFgColor();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CheckButton::OnCheckButtonChecked(Panel *panel)
{
}