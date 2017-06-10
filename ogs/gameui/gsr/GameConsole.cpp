#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>

#include "EngineInterface.h"
#include "GameConsole.h"
#include "GameConsoleDialog.h"
#include "LoadingDialog.h"

static CGameConsole g_GameConsole;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(
	CGameConsole, IGameConsole,
	GAMECONSOLE_INTERFACE_VERSION,
	g_GameConsole
);

CGameConsole::CGameConsole()
{
}

CGameConsole::~CGameConsole()
{
	m_bInitialized = false;
}

void CGameConsole::Activate()
{
	if( m_bInitialized )
	{
		if( !LoadingDialog() )
		{
			vgui2::surface()->RestrictPaintToSinglePanel( NULL_HANDLE );
			m_pConsole->Activate();
		}
	}
}

void CGameConsole::Initialize()
{
	m_pConsole = new CGameConsoleDialog();
	m_pConsole->MakeReadyForUse();

	m_pConsole->SetParent( g_pTaskbar );

	int swide, stall;
	vgui2::surface()->GetScreenSize( swide, stall );

	//Compute console size
	int wide = swide - 80;

	if( wide > 560 )
		wide = 560;

	int tall = stall - 80;

	if( tall > 400 )
		tall = 400;

	m_pConsole->SetBounds( 40, 40, wide, tall );

	m_bInitialized = true;

	gEngfuncs.pfnAddCommand( "condump", &CGameConsole::OnCmdCondump );
}

void CGameConsole::Hide()
{
	if( m_bInitialized )
		m_pConsole->Hide();
}

void CGameConsole::Clear()
{
	if( m_bInitialized )
		m_pConsole->Clear();
}

bool CGameConsole::IsConsoleVisible()
{
	if( !m_bInitialized )
		return false;

	return m_pConsole->IsVisible();
}

void CGameConsole::Printf( const char* format, ... )
{
	char msg[ 4096 ];

	va_list va;

	va_start( va, format );

	if( m_bInitialized )
	{
		vsnprintf( msg, ARRAYSIZE( msg ), format, va );
		msg[ ARRAYSIZE( msg ) - 1 ] = '\0';
		m_pConsole->Print( msg );
	}

	va_end( va );
}

void CGameConsole::DPrintf( const char* format, ... )
{
	char msg[ 4096 ];

	va_list va;

	va_start( va, format );

	if( m_bInitialized )
	{
		vsnprintf( msg, ARRAYSIZE( msg ), format, va );
		msg[ ARRAYSIZE( msg ) - 1 ] = '\0';
		m_pConsole->DPrint( msg );
	}

	va_end( va );
}

void CGameConsole::SetParent( vgui2::VPANEL parent )
{
	if( m_bInitialized )
		m_pConsole->SetParent( parent );
}

void CGameConsole::ActivateDelayed( float time )
{
	if( !m_bInitialized )
		return;

	m_pConsole->PostMessage( m_pConsole, new KeyValues( "Activate" ), time );
}

void CGameConsole::OnCmdCondump()
{
	g_GameConsole.m_pConsole->DumpConsoleTextToFile();
}
