#include <tier0/platform.h>
#include <checksum_md5.h>
#include <KeyValues.h>

#include <vgui/IInputInternal.h>
#include <vgui/ISystem.h>
#include <vgui_controls/TextEntry.h>

#include "ContentControlDialog.h"
#include "EngineInterface.h"
#include "Taskbar.h"

CContentControlDialog::CContentControlDialog( vgui2::Panel* parent )
	: BaseClass( parent, "ContentControlDialog", true )
{
	SetBounds( 0, 0, 372, 160 );
	SetSizeable( false );

	g_pTaskbar->AddTask( GetVPanel() );
	SetTitle( "#GameUI_ContentLock", true );

	m_pStatus = new vgui2::Label( this, "ContentStatus", "" );
	m_pPasswordLabel = new vgui2::Label( this, "PasswordPrompt", "#GameUI_PasswordPrompt" );
	m_pPassword2Label = new vgui2::Label( this, "PasswordReentryPrompt", "#GameUI_PasswordReentryPrompt" );
	m_pExplain = new vgui2::Label( this, "ContentControlExplain", "" );
	m_pPassword = new vgui2::TextEntry( this, "Password" );
	m_pPassword2 = new vgui2::TextEntry( this, "Password2" );

	m_pOK = new vgui2::Button( this, "Ok", "#GameUI_OK" );
	m_pOK->SetCommand( "Ok" );

	auto pCancel = new vgui2::Button( this, "Cancel", "#GameUI_Cancel" );
	pCancel->SetCommand( "Cancel" );

	m_szGorePW[ 0 ] = '\0';

	ResetPassword();

	LoadControlSettings( "Resource\\ContentControlDialog.res" );
}

void CContentControlDialog::OnClose()
{
	BaseClass::OnClose();

	PostActionSignal( new KeyValues( "ContentControlClose" ) );
}

void CContentControlDialog::ResetPassword()
{
	if( !vgui2::system()->GetRegistryString(
		"Software\\Valve\\Half-Life\\Settings\\User Token 2",
		m_szGorePW,
		ARRAYSIZE( m_szGorePW ) ) )
	{
		m_szGorePW[ 0 ] = '\0';
	}
}

void CContentControlDialog::Explain( const char* fmt, ... )
{
	char text[ 1024 ];
	va_list va;

	va_start( va, fmt );

	if( m_pExplain )
	{
		vsnprintf( text, ARRAYSIZE( text ), fmt, va );
		m_pExplain->SetText( text );
	}

	va_end( va );
}

char* CContentControlDialog::BinPrintf( byte* buf, int nLen )
{
	static char szReturn[ 1024 ];

	memset( szReturn, 0, sizeof( szReturn ) );

	char szChunk[ 10 ];

	for( int i = 0; i < nLen; ++i )
	{
		snprintf( szChunk, ARRAYSIZE( szChunk ), "%02x", buf[ i ] );

		strncat( szReturn, szChunk, ARRAYSIZE( szReturn ) - 1 - strlen( szReturn ) );
	}

	return szReturn;
}

void CContentControlDialog::HashPassword( const char* newPW, char* hashBuffer )
{
	MD5Context_t ctx;

	byte md5_hash[ MD5_DIGEST_LENGTH ];

	MD5Init( &ctx );
	MD5Update( &ctx, reinterpret_cast<const unsigned char*>( newPW ), strlen( newPW ) );
	MD5Final( md5_hash, &ctx );

	auto pszResult = BinPrintf( md5_hash, sizeof( md5_hash ) );

	strcpy( hashBuffer, pszResult );
}

bool CContentControlDialog::EnablePassword( const char* newPW )
{
	if( *newPW )
	{
		char digestedPW[ 128 ];
		HashPassword( newPW, digestedPW );

		engine->Cvar_SetValue( "violence_hblood", 0 );
		engine->Cvar_SetValue( "violence_hgibs", 0 );
		engine->Cvar_SetValue( "violence_ablood", 0 );
		engine->Cvar_SetValue( "violence_agibs", 0 );

		strcpy( m_szGorePW, digestedPW );

		return true;
	}
	else
	{
		Explain( "#GameUI_MustEnterPassword" );
		return false;
	}
}

bool CContentControlDialog::DisablePassword( const char* oldPW )
{
	if( *oldPW )
	{
		char digestedPW[ 128 ];

		HashPassword( oldPW, digestedPW );

		if( !stricmp( this->m_szGorePW, digestedPW ) )
		{
			m_szGorePW[ 0 ] = '\0';
			engine->Cvar_SetValue( "violence_hblood", 1 );
			engine->Cvar_SetValue( "violence_hgibs", 1 );
			engine->Cvar_SetValue( "violence_ablood", 1 );
			engine->Cvar_SetValue( "violence_agibs", 1 );
			
			return true;
		}
		else
		{
			Explain( "#GameUI_IncorrectPassword" );
		}
	}
	else
	{
		Explain( "#GameUI_MustEnterPassword" );
	}

	return false;
}

void CContentControlDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "Ok" ) )
	{
		char pw1[ 256 ];
		char pw2[ 256 ];

		m_pPassword->GetText( pw1, ARRAYSIZE( pw1 ) );
		m_pPassword2->GetText( pw2, ARRAYSIZE( pw2 ) );

		bool bChanged;

		if( m_szGorePW[ 0 ] )
		{
			bChanged = DisablePassword( pw1 );
		}
		else
		{
			if( strcmp( pw1, pw2 ) )
			{
				Explain( "#GameUI_PasswordsDontMatch" );
				return;
			}

			bChanged = EnablePassword( pw1 );
		}

		if( bChanged )
			OnClose();
	}
	else if( !stricmp( command, "Cancel" ) )
	{
		OnClose();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

bool CContentControlDialog::IsPasswordEnabledInDialog()
{
	return m_szGorePW[ 0 ] != '\0';
}

void CContentControlDialog::UpdateContentControlStatus()
{
	if( m_szGorePW[ 0 ] )
	{
		m_pStatus->SetText( "#GameUI_ContentStatusEnabled" );
		m_pPasswordLabel->SetText( "#GameUI_PasswordDisablePrompt" );
	}
	else
	{
		m_pStatus->SetText( "#GameUI_ContentStatusDisabled" );
		m_pPasswordLabel->SetText( "#GameUI_PasswordPrompt" );
	}

	m_pPassword2Label->SetVisible( !IsPasswordEnabledInDialog() );
	m_pPassword2->SetVisible( !IsPasswordEnabledInDialog() );
}

void CContentControlDialog::WriteToken( const char* str )
{
	vgui2::system()->SetRegistryString( "Software\\Valve\\Half-Life\\Settings\\User Token 2", str );
	strcpy( m_szGorePW, str );

	UpdateContentControlStatus();
}

void CContentControlDialog::Activate()
{
	BaseClass::Activate();

	m_pPassword->SetText( L"" );
	m_pPassword2->SetText( L"" );

	Explain( "" );
	UpdateContentControlStatus();

	vgui2::input()->SetAppModalSurface( GetVPanel() );
}

void CContentControlDialog::ApplyPassword()
{
	vgui2::system()->SetRegistryString( "Software\\Valve\\Half-Life\\Settings\\User Token 2", m_szGorePW );
	UpdateContentControlStatus();
}
