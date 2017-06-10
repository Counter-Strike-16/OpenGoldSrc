#include <vgui/ILocalize.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/PropertySheet.h>

#include "GameUI_Interface.h"
#include "ModInfo.h"
#include "OptionsDialog.h"
#include "OptionsSubAdvanced.h"
#include "OptionsSubAudio.h"
#include "OptionsSubKeyboard.h"
#include "OptionsSubMouse.h"
#include "OptionsSubMultiplayer.h"
#include "OptionsSubVideo.h"
#include "OptionsSubVoice.h"
#include "Taskbar.h"

COptionsDialog::COptionsDialog( vgui2::Panel* parent )
	: BaseClass( parent, "OptionsDialog" )
{
	SetBounds( 0, 0, 512, 406 );
	SetSizeable( false );

	g_pTaskbar->AddTask( GetVPanel() );

	SetTitle( "#GameUI_Options", true );

	if( ModInfo().IsMultiplayerOnly() && !ModInfo().IsSinglePlayerOnly() )
	{
		AddPage( new COptionsSubMultiplayer( this ),  "#GameUI_Multiplayer" );
	}

	AddPage( new COptionsSubKeyboard( this ), "#GameUI_Keyboard" );
	AddPage( new COptionsSubMouse( this ), "#GameUI_Mouse" );
	AddPage( new COptionsSubAudio( this ), "#GameUI_Audio" );
	AddPage( new COptionsSubVideo( this ), "#GameUI_Video" );

	if( !ModInfo().IsSinglePlayerOnly() || !stricmp( ModInfo().GetGameDescription(), "Half-Life" ) )
	{
		AddPage( new COptionsSubVoice( this ), "#GameUI_Voice" );
	}

	AddPage( new COptionsSubAdvanced( this ), "#GameUI_Advanced" );

	if( ModInfo().IsSinglePlayerOnly() && stricmp( ModInfo().GetGameDescription(), "Half-Life" ) )
	{
		AddPage( new CMultiplayerAdvancedPage( this ), "#GameUI_AdvancedNoEllipsis" );
	}

	if( !ModInfo().IsMultiplayerOnly() && 
		( !ModInfo().IsSinglePlayerOnly() ||
		  !stricmp( ModInfo().GetGameDescription(), "Half-Life" ) ) )
	{
		AddPage( new COptionsSubMultiplayer( this ), "#GameUI_Multiplayer" );
	}

	SetApplyButtonVisible( true );

	GetPropertySheet()->SetTabWidth( 72 );
}

void COptionsDialog::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}

void COptionsDialog::Activate()
{
	BaseClass::Activate();
	EnableApplyButton( false );
}

void COptionsDialog::SetTitle( const char* title, bool surfaceTitle )
{
	BaseClass::SetTitle( title, surfaceTitle );

	if( g_pTaskbar )
	{
		auto pszTitle = vgui2::localize()->Find( title );

		wchar_t w_szTitle[ 1024 ];

		if( !pszTitle )
		{
			vgui2::localize()->ConvertANSIToUnicode( title, w_szTitle, sizeof( w_szTitle ) );
			pszTitle = w_szTitle;
		}

		g_pTaskbar->SetTitle( GetVPanel(), pszTitle );
	}
}

void COptionsDialog::Run()
{
	SetTitle( "#GameUI_Options", true );

	Activate();
}
