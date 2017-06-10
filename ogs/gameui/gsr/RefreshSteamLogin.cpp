#include <vgui/IInputInternal.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/MessageBox.h>

#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "RefreshSteamLogin.h"
#include "ienginevgui.h"

CRefreshSteamLogin::CRefreshSteamLogin( vgui2::Panel* parent, const char* username )
	: BaseClass( parent, "RefreshSteamLogin", true )
{
	SetSizeable( false );
	SetSize( 360, 224 );

	SetTitle( "#GameUI_RefreshLogin", true );

	m_pUserName = new vgui2::TextEntry( this, "username" );
	m_pPassword = new vgui2::TextEntry( this, "password" );

	m_pSavePassword = new vgui2::CheckButton( this, "SavePasswordCheck", "" );

	m_pSavePassword->SetSelected( true );

	m_pLoginButton = new vgui2::Button( this, "OKButton", "" );

	LoadControlSettings( "Resource/RefreshLogin.res" );

	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );

	m_pUserName->SetEnabled( false );
	m_pUserName->SetText( username );
}

CRefreshSteamLogin::~CRefreshSteamLogin()
{
}

void CRefreshSteamLogin::Activate()
{
	BaseClass::Activate();

	m_pPassword->RequestFocus();

	m_RestrictedPaintPanel = GetVPanel();
}

void CRefreshSteamLogin::OnThink()
{
	vgui2::surface()->RestrictPaintToSinglePanel( m_RestrictedPaintPanel );

	if( !vgui2::input()->GetFocus() ||
		!vgui2::ipanel()->HasParent( vgui2::input()->GetFocus(), GetVPanel() ) )
	{
		Activate();
	}

	if( !m_bLoggingIn )
	{
		BaseClass::OnThink();
		return;
	}

	bool finished;
	TSteamProgress progress;
	TSteamError steamError;

	if( EngineVGUI()->SteamProcessCall( &finished, &progress, &steamError ) )
	{
		if( finished )
		{
			if( steamError.eSteamError != eSteamErrorNone )
			{
				vgui2::input()->ReleaseAppModalSurface();

				auto pMB = new vgui2::MessageBox( "#GameUI_Login_ErrorTitle", "#GameUI_ErrorLoginFailed", this );
				
				pMB->SetCommand( "errorClose" );
				pMB->DoModal();

				m_RestrictedPaintPanel = pMB->GetVPanel();
			}
			else
			{
				engine->pfnClientCmd( "retry\n" );
				Close();
			}

			m_bLoggingIn = false;
			m_pLoginButton->SetEnabled( true );
			m_pPassword->SetEnabled( true );
		}

		BaseClass::OnThink();
		return;
	}

	m_pPassword->SetEnabled( true );
	m_pLoginButton->SetEnabled( true );

	m_bLoggingIn = false;

	vgui2::input()->ReleaseAppModalSurface();

	auto pMB = new vgui2::MessageBox( "#GameUI_Login_ErrorTitle", "#GameUI_ErrorLoginFailed", this );

	pMB->SetCommand( "errorClose" );
	pMB->DoModal();

	m_RestrictedPaintPanel = pMB->GetVPanel();
}

void CRefreshSteamLogin::OnClose()
{
	vgui2::input()->ReleaseAppModalSurface();

	BaseClass::OnClose();

	MarkForDeletion();
}

void CRefreshSteamLogin::OnCommand( const char* command )
{
	if( !stricmp( command, "Login" ) )
	{
		char szPassword[ 128 ];
		m_pPassword->GetText( szPassword, ARRAYSIZE( szPassword ) );

		if( EngineVGUI()->SteamRefreshLogin(
			szPassword,
			m_pSavePassword->IsSelected() ) )
		{
			m_bLoggingIn = true;
			m_pPassword->SetEnabled( false );
			m_pLoginButton->SetEnabled( false );
		}
		else
		{
			vgui2::input()->ReleaseAppModalSurface();

			auto pMB = new vgui2::MessageBox( "#GameUI_Login_ErrorTitle", "#GameUI_ErrorLoginFailed", this );
			
			pMB->SetCommand( "errorclose" );
			pMB->DoModal();

			m_RestrictedPaintPanel = pMB->GetVPanel();
		}
	}
	else if( !stricmp( command, "Close" ) )
	{
		Close();
	}
	else if( !stricmp( command, "errorclose" ) )
	{
		m_RestrictedPaintPanel = GetVPanel();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}
