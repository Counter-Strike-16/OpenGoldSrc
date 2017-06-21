#include <cwctype>

#include <vgui/IInputInternal.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/Controls.h>

#include "CDKeyEntryDialog.h"
#include "EngineInterface.h"

void CUpperCaseTextEntry::OnKeyTyped( wchar_t unichar )
{
	if( iswalnum( unichar ) )
	{
		if( L'a' <= unichar && unichar <= L'z' )
			unichar -= ( L'a' - L'A' );

		BaseClass::OnKeyTyped( unichar );
	}
}

void CloseMessageBox::OnClose()
{
	engine->pfnClientCmd( "quit\n" );
}

CCDKeyEntryDialog::CCDKeyEntryDialog( vgui2::Panel* parent, bool inConnect )
	: BaseClass( parent, "CDKeyEntryDialog", true )
{
	m_bIsValveCDKey = true;
	m_bInConnect = inConnect;
	m_iErrCount = 0;

	char cdKeyType[ 64 ];
	if( engine->CheckParm( "-forcevalve", nullptr ) ||
		vgui2::system()->GetRegistryString( "HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\ValveKey", cdKeyType, ARRAYSIZE( cdKeyType ) ) )
		m_bIsValveCDKey = true;

	m_pOK = new vgui2::Button( this, "OKButton", "#GameUI_OK" );
	m_pQuitGame = new vgui2::Button( this, "CancelButton", "#GameUI_Quit" );

	m_pEntry1 = new CUpperCaseTextEntry( this, "Entry1" );
	m_pEntry2 = new CUpperCaseTextEntry( this, "Entry2" );
	m_pEntry3 = new CUpperCaseTextEntry( this, "Entry3" );
	m_pEntry4 = new CUpperCaseTextEntry( this, "Entry4" );
	m_pEntry5 = new CUpperCaseTextEntry( this, "Entry5" );

	if( m_bIsValveCDKey )
	{
		m_pEntry1->SetMaximumCharCount( CHARS_PER_ENTRY );
		m_pEntry2->SetMaximumCharCount( CHARS_PER_ENTRY );
		m_pEntry3->SetMaximumCharCount( CHARS_PER_ENTRY );
		m_pEntry4->SetMaximumCharCount( CHARS_PER_ENTRY );
		m_pEntry5->SetMaximumCharCount( CHARS_PER_ENTRY );
	}

	SetSizeable( false );
	SetSize( 360, 224 );
	SetTitle( "#GameUI_CDKey", true );

	if( m_bIsValveCDKey )
		LoadControlSettings(  "Resource/ValveCDKeyEntryDialog.res" );
	else
		LoadControlSettings( "Resource/CDKeyEntryDialog.res" );

	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );

	m_pOK->SetEnabled( false );
}

void CCDKeyEntryDialog::Activate()
{
	BaseClass::Activate();
	m_pEntry1->RequestFocus();
}

void CCDKeyEntryDialog::OnTextChanged( vgui2::Panel* entry )
{
	char cdkey[ 32 ];

	if( m_bIsValveCDKey )
	{
		m_pEntry1->GetText( cdkey, CHARS_PER_ENTRY + 1 );
		m_pEntry2->GetText( &cdkey[ 6 ], CHARS_PER_ENTRY + 1 );
		m_pEntry3->GetText( &cdkey[ 12 ], CHARS_PER_ENTRY + 1 );
		m_pEntry4->GetText( &cdkey[ 18 ], CHARS_PER_ENTRY + 1 );
		m_pEntry5->GetText( &cdkey[ 24 ], CHARS_PER_ENTRY + 1 );

		//Convert null terminators between entries to dashes
		for( int i = 0; i < NUM_ENTRIES - 1; ++i )
		{
			cdkey[ ( CHARS_PER_ENTRY * ( i + 1 ) ) + i ] = '-';
		}
	}

	if( m_bIsValveCDKey && ( strlen( cdkey ) == ( ( ( CHARS_PER_ENTRY + 1 ) * NUM_ENTRIES ) - 1 ) ) )
	{
		m_pOK->SetEnabled( true );
	}
	else
	{
		memset( cdkey, 0, sizeof( cdkey ) );

		if( m_pEntry1 == entry )
		{
			m_pEntry1->GetText( cdkey, CHARS_PER_ENTRY + 1 );

			if( strlen( cdkey ) >= static_cast<size_t>( m_bIsValveCDKey ? CHARS_PER_ENTRY : CHARS_PER_ENTRY - 1 ) )
				m_pEntry2->RequestFocus();
		}
		else if( m_pEntry2 == entry )
		{
			m_pEntry2->GetText( cdkey, CHARS_PER_ENTRY + 1 );

			if( strlen( cdkey ) >= ( CHARS_PER_ENTRY - 1 ) )
				m_pEntry3->RequestFocus();
		}
		else if( m_bIsValveCDKey )
		{
			if( m_pEntry3 == entry )
			{
				m_pEntry3->GetText( cdkey, CHARS_PER_ENTRY + 1 );

				if( strlen( cdkey ) >= ( CHARS_PER_ENTRY - 1 ) )
					m_pEntry4->RequestFocus();
			}
			else if( m_pEntry4 == entry )
			{
				m_pEntry4->GetText( cdkey, CHARS_PER_ENTRY + 1 );

				if( strlen( cdkey ) >= ( CHARS_PER_ENTRY - 1 ) )
					m_pEntry5->RequestFocus();
			}
		}
		
		m_pOK->SetEnabled( false );
	}
}

void CCDKeyEntryDialog::OnThink()
{
	vgui2::surface()->RestrictPaintToSinglePanel( GetVPanel() );

	if( !vgui2::input()->GetFocus() ||
		!vgui2::ipanel()->HasParent( vgui2::input()->GetFocus(), GetVPanel() ) )
		Activate();

	BaseClass::OnThink();
}

void CCDKeyEntryDialog::OnClose()
{
	engine->pfnClientCmd( "quit\n" );

	BaseClass::OnClose();

	MarkForDeletion();
}

void CCDKeyEntryDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "OK" ) )
	{
		auto pMB = new vgui2::MessageBox( "#GameUI_CDKey_Invalid_Title", "#GameUI_CDKey_Invalid_Text" );

		pMB->ShowWindow( this );
	}
	else if( !stricmp( command, "Cancel" ) || !stricmp( command, "Close" ) )
	{
		BaseClass::OnCommand( "Close" );
	}
	else
	{
		BaseClass::OnCommand( command );
	}

	++m_iErrCount;

	if( m_iErrCount >= MAX_CDKEY_ERRORS )
	{
		auto pMB = new CloseMessageBox( "#GameUI_CDKey_Invalid_Title", "#GameUI_CDKey_TooManyTries", this );
		
		pMB->ShowWindow( this );
	}
}

bool CCDKeyEntryDialog::IsEnteredKeyValid()
{
	return false;
}
