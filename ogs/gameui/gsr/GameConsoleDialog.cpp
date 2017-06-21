#include <FileSystem.h>
#include <KeyValues.h>
#include <vgui/IInputInternal.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Menu.h>

#include "EngineInterface.h"
#include "igameuifuncs.h"
#include "LoadingDialog.h"
#include "BaseUI/IBaseUI.h"

#include "GameConsoleDialog.h"

TabCatchingTextEntry::TabCatchingTextEntry( vgui2::Panel* parent, const char* panelName, vgui2::VPANEL hCompletionList )
	: BaseClass( parent, panelName )
	, m_pCompletionList( hCompletionList )
{
	SetAllowNonAsciiCharacters( true );
}

void TabCatchingTextEntry::IgnoreNextTextInput( bool bIgnore )
{
	m_bIgnoreKeyTyped = bIgnore;
}

void TabCatchingTextEntry::OnKeyTyped( wchar_t key )
{
	if( !m_bIgnoreKeyTyped )
		BaseClass::OnKeyTyped( key );

	m_bIgnoreKeyTyped = false;
}

void TabCatchingTextEntry::OnKeyCodeTyped( vgui2::KeyCode code )
{
	if( code == vgui2::KEY_PAD_ENTER )
	{
		code = vgui2::KEY_ENTER;
	}
	else if( code != vgui2::KEY_DOWN &&
			 code != vgui2::KEY_UP &&
			 code != vgui2::KEY_ENTER )
	{
		GetParent()->OnKeyCodeTyped( code );
	}

	BaseClass::OnKeyCodeTyped( code );
}

void TabCatchingTextEntry::OnKillFocus()
{
	if( vgui2::input()->GetFocus() != m_pCompletionList )
	{
		PostMessage( GetParent(), new KeyValues( "CloseCompletionList" ) );
	}
}

CNoKeyboardInputRichText::CNoKeyboardInputRichText( vgui2::Panel* parent, const char* panelName, vgui2::Panel* pFocusPanel )
	: BaseClass( parent, panelName )
	, m_pFocusPanel( pFocusPanel )
{
	//TODO: implement m_bTrustInput in RichText if needed. - Solokiller
}

void CNoKeyboardInputRichText::OnRequestFocus( vgui2::VPANEL subFocus, vgui2::VPANEL defaultPanel )
{
	m_pFocusPanel->RequestFocus();
}

void CNoKeyboardInputRichText::OnKeyCodeTyped( vgui2::KeyCode code )
{
	BaseClass::OnKeyCodeTyped( code );
}

void CNoKeyboardInputRichText::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	if( gEngfuncs.pfnGetCvarFloat( "con_mono" ) > 0 )
	{
		SetFont( pScheme->GetFont( "GameConsole_Mono", IsProportional() ) );
	}
}

CHistoryItem::CHistoryItem()
{
}

CHistoryItem::CHistoryItem( const char* text, const char* extra )
{
	SetText( text, extra );
}

CHistoryItem::CHistoryItem( const CHistoryItem& src )
{
	SetText( src.m_text ? src.m_text : "", src.m_extraText );
}

CHistoryItem::~CHistoryItem()
{
	if( m_text )
		delete[] m_text;

	if( m_extraText )
		delete[] m_extraText;

	m_text = nullptr;
}

const char* CHistoryItem::GetText()
{
	return m_text;
}

const char* CHistoryItem::GetExtra()
{
	return m_extraText;
}

bool CHistoryItem::HasExtra()
{
	return m_bHasExtra;
}

void CHistoryItem::SetText( const char* text, const char* extra )
{
	if( m_text )
		delete[] m_text;

	auto size = strlen( text ) + 1;

	m_text = new char[ size ];

	memset( m_text, 0, size * sizeof( char ) );

	strcpy( m_text, text );

	if( extra )
	{
		m_bHasExtra = true;

		if( m_extraText )
			delete[] m_extraText;

		auto extraSize = strlen( extra ) + 1;

		m_extraText = new char[ extraSize ];

		memset( m_extraText, 0, extraSize * sizeof( char ) );

		strcpy( m_extraText, extra );
	}
	else
	{
		m_bHasExtra = false;
	}
}

CGameConsoleDialog::CompletionItem::CompletionItem()
	: iscommand( true )
{
	cmd.cmd = 0;
	cmd.cvar = nullptr;
	m_text = nullptr;
}

CGameConsoleDialog::CompletionItem::CompletionItem( const CompletionItem& src )
	: iscommand( src.iscommand )
{
	cmd = src.cmd;

	if( src.m_text )
	{
		m_text = new CHistoryItem( *src.m_text );
	}
	else
	{
		m_text = nullptr;
	}
}

CGameConsoleDialog::CompletionItem& CGameConsoleDialog::CompletionItem::operator=( const CompletionItem& src )
{
	if( this != &src )
	{
		iscommand = src.iscommand;
		cmd = src.cmd;

		//TODO: This appears to be missing from the original - Solokiller
		//if( m_text )
		//	delete m_text;

		if( src.m_text )
		{
			m_text = new CHistoryItem( *src.m_text );
		}
		else
			m_text = nullptr;
	}

	return *this;
}

CGameConsoleDialog::CompletionItem::~CompletionItem()
{
	if( m_text )
		delete m_text;
}

const char* CGameConsoleDialog::CompletionItem::GetItemText()
{
	static char text[ 256 ];

	text[ 0 ] = '\0';

	if( m_text )
	{
		auto pszText = m_text->GetText();

		if( !pszText )
			pszText = "";

		if( m_text->HasExtra() )
		{
			snprintf(
				text, ARRAYSIZE( text ),
				"%s %s",
				pszText, m_text->GetExtra()
			);
		}
		else
		{
			strcpy( text, pszText );
		}
	}

	return text;
}

const char* CGameConsoleDialog::CompletionItem::GetCommand() const
{
	static char text[ 256 ];

	text[ 0 ] = '\0';

	if( m_text )
	{
		auto pszText = m_text->GetText();

		if( !pszText )
			pszText = "";

		strcpy( text, pszText );
	}

	return text;
}

const char* CGameConsoleDialog::CompletionItem::GetName() const
{
	if( !iscommand )
		return GetCommand();

	return cmd.cvar ? cmd.cvar->name : engine->GetCmdFunctionName( cmd.cmd );
}

CGameConsoleDialog::CGameConsoleDialog()
	: BaseClass( nullptr, "GameConsole", true )
{
	MakePopup();
	SetVisible( false );
	SetMinimumSize( 100, 100 );

	g_pTaskbar->AddTask( GetVPanel() );

	SetTitle( "#GameUI_Console", true );

	m_pSubmit = new vgui2::Button( this, "ConsoleSubmit", "#GameUI_Submit" );
	m_pSubmit->SetCommand( "submit" );

	m_pCompletionList = new vgui2::Menu( this, "CompletionList" );
	m_pCompletionList->MakePopup();
	m_pCompletionList->SetVisible( false );

	m_pEntry = new TabCatchingTextEntry( this, "ConsoleEntry", m_pCompletionList->GetVPanel() );
	m_pEntry->AddActionSignalTarget( this );
	m_pEntry->SendNewLine( true );
	m_pEntry->SetTabPosition( 1 );

	m_pHistory = new CNoKeyboardInputRichText( this, "ConsoleHistory", m_pEntry );
	m_pHistory->SetVerticalScrollbar( true );
}

CGameConsoleDialog::~CGameConsoleDialog()
{
}

void CGameConsoleDialog::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_PrintColor = GetFgColor();
	m_DPrintColor = GetSchemeColor( "BrightControlText", pScheme );

	InvalidateLayout();
}

void CGameConsoleDialog::Activate()
{
	BaseClass::Activate();

	m_pEntry->RequestFocus();
	m_pEntry->IgnoreNextTextInput( false );
	m_pHistory->InvalidateLayout( false, true );
}

void CGameConsoleDialog::OnTextChanged( vgui2::Panel* panel )
{
	if( m_pEntry != panel )
		return;

	m_pEntry->GetText( m_szPartialText, ARRAYSIZE( m_szPartialText ) );

	auto length = strlen( m_szPartialText );

	if( !m_bAutoCompleteMode && length > 0 && m_szPartialText[ length - 1 ] == ' ' )
	{
		m_iNextCompletion = 0;
		m_bAutoCompleteMode = true;

		if( m_CompletionList.Count() > 0 )
			OnAutoComplete();
	}

	m_bAutoCompleteMode = false;

	RebuildCompletionList( m_szPartialText );

	if( m_CompletionList.Count() <= 0 )
	{
		m_pCompletionList->SetVisible( false );
	}
	else
	{
		m_pCompletionList->SetVisible( true );

		m_pCompletionList->DeleteAllItems();

		char dest[ 256 ];

		const int maxCompletions = min( m_CompletionList.Count(), MAX_COMPLETION_ITEMS + 1 );

		for( int i = 0; i < maxCompletions; ++i )
		{
			dest[ 0 ] = '\0';

			auto& completion = m_CompletionList[ i ];

			strcpy( dest, completion.GetItemText() );
			dest[ ARRAYSIZE( dest ) - 1 ] = '\0';


			m_pCompletionList->AddMenuItem( dest, new KeyValues( "CompletionCommand", "command", dest ), this );
		}

		if( m_CompletionList.Count() > ( MAX_COMPLETION_ITEMS + 1 ) )
		{
			strcpy( dest, "..." );
			dest[ ARRAYSIZE( dest ) - 1 ] = '\0';

			m_pCompletionList->AddMenuItem( dest, new KeyValues( "CompletionCommand", "command", dest ), this );
		}

		UpdateCompletionListPosition();
	}

	RequestFocus();
	m_pEntry->RequestFocus();
}

void CGameConsoleDialog::CloseCompletionList()
{
	m_pCompletionList->SetVisible( false );
}

void CGameConsoleDialog::OnMenuItemSelected( const char* command )
{
	if( strstr( command, "..." ) )
	{
		m_pCompletionList->SetVisible( true );
	}
	else
	{
		m_pEntry->SetText( command );
		m_pEntry->GotoTextEnd();
		m_pEntry->InsertChar( L' ' );
		m_pEntry->GotoTextEnd();
	}
}

void CGameConsoleDialog::OnAutoComplete()
{
	auto& completion = m_CompletionList[ m_iNextCompletion ];

	char completedText[ 256 ];

	strncpy( completedText, completion.iscommand ? completion.GetCommand() : completion.GetItemText(), ARRAYSIZE( completedText ) - 2 );

	if( !strchr( completedText, ' ' ) )
	{
		const auto length = strlen( completedText );
		completedText[ length ] = ' ';
		completedText[ length + 1 ] = '\0';
	}

	m_pEntry->SetText( completedText );
	m_pEntry->SelectNone();
	m_pEntry->GotoTextEnd();

	++m_iNextCompletion;
}

void CGameConsoleDialog::OnKeyCodeTyped( vgui2::KeyCode code )
{
	BaseClass::OnKeyCodeTyped( code );

	const auto toggleKey = gameuifuncs->GetVGUI2KeyCodeForBind( "toggleconsole" );

	const bool bShiftDown = vgui2::input()->IsKeyDown( vgui2::KEY_LSHIFT ) ||
		vgui2::input()->IsKeyDown( vgui2::KEY_RSHIFT );

	if( vgui2::input()->GetFocus() == m_pEntry->GetVPanel() )
	{
		if( code == vgui2::KEY_TAB )
		{
			OnAutoComplete( bShiftDown );
			m_pEntry->RequestFocus();
		}
		else if( code == vgui2::KEY_DOWN )
		{
			OnAutoComplete( false );
			m_pEntry->RequestFocus();
		}
		else if( code == vgui2::KEY_UP )
		{
			OnAutoComplete( true );
			m_pEntry->RequestFocus();
		}
	}

	m_pEntry->GetText( m_szPartialText, ARRAYSIZE( m_szPartialText ) );

	const bool bCtrlDown = vgui2::input()->IsKeyDown( vgui2::KEY_LCONTROL ) ||
		vgui2::input()->IsKeyDown( vgui2::KEY_RCONTROL );

	if( !m_szPartialText[ 0 ] && bCtrlDown )
	{
		if( code == vgui2::KEY_X || code == vgui2::KEY_C )
		{
			m_pHistory->CopySelected();
		}

		if( code == vgui2::KEY_A )
		{
			m_pHistory->SelectAllText();
		}
	}

	if( !bShiftDown && code == toggleKey )
	{
		if( toggleKey )
		{
			m_pEntry->SetText( "" );
			m_pEntry->IgnoreNextTextInput( true );

			PostMessage( this, new KeyValues( "Close" ) );

			if( baseuifuncs )
			{
				if( LoadingDialog() )
				{
					vgui2::surface()->RestrictPaintToSinglePanel( LoadingDialog()->GetVPanel() );
				}
				else
				{
					baseuifuncs->HideGameUI();
				}
			}
		}
	}
}

void CGameConsoleDialog::Clear()
{
	m_pHistory->SetText( "" );
}

void CGameConsoleDialog::Print(  const char* msg )
{
	m_pHistory->InsertColorChange( m_PrintColor );
	m_pHistory->InsertString( msg );
}

void CGameConsoleDialog::DPrint( const char* msg )
{
	m_pHistory->InsertColorChange( m_DPrintColor );
	m_pHistory->InsertString( msg );
}

void CGameConsoleDialog::OnAutoComplete( bool reverse )
{
	if( !m_bAutoCompleteMode )
	{
		m_iNextCompletion = 0;
		m_bAutoCompleteMode = true;
	}

	if( reverse )
	{
		m_iNextCompletion -= 2;

		if( m_iNextCompletion < 0 )
		{
			m_iNextCompletion = m_CompletionList.Count() - 1;
		}
	}

	if( m_iNextCompletion < 0 || m_iNextCompletion >= m_CompletionList.Count() )
		m_iNextCompletion = 0;

	if( m_iNextCompletion >= 0 && m_iNextCompletion < m_CompletionList.Count() )
		OnAutoComplete();
}

void CGameConsoleDialog::UpdateCompletionListPosition()
{
	int x, y;
	GetPos( x, y );

	int ex, ey;
	m_pEntry->GetPos( ex, ey );

	m_pCompletionList->SetPos( ex + x, ey + y + 32 );

	m_pEntry->RequestFocus();
	MoveToFront();
}

void CGameConsoleDialog::PerformLayout()
{
	BaseClass::PerformLayout();

	GetFocusNavGroup().SetDefaultButton( m_pSubmit );

	auto pScheme = vgui2::scheme()->GetIScheme( GetScheme() );

	m_pEntry->SetBorder( pScheme->GetBorder( "DepressedButtonBorder" ) );
	m_pHistory->SetBorder( pScheme->GetBorder( "DepressedButtonBorder" ) );

	int wide, tall;
	GetSize( wide, tall );

	m_pHistory->SetPos( 8, 36 );
	m_pHistory->SetSize( wide - 16, tall - 76 );
	m_pEntry->SetPos( 8, tall - 32 );
	m_pEntry->SetSize( wide - 88, 24 );
	m_pSubmit->SetPos( wide - 72, tall - 32 );
	m_pSubmit->SetSize( 64, 24 );

	UpdateCompletionListPosition();
}

void CGameConsoleDialog::Hide()
{
	BaseClass::Close();
	m_iNextCompletion = 0;
}

void CGameConsoleDialog::DumpConsoleTextToFile()
{
	char szfile[ 512 ];

	int i;

	for( i = 0; i < 1000; ++i )
	{
		snprintf( szfile, ARRAYSIZE( szfile ), "condump%03d.txt", i );

		if( !vgui2::filesystem()->FileExists( szfile ) )
			break;
	}

	if( i == 1000 )
	{
		Print( "Can't condump! Too many existing condump output files in the gamedir!\n" );
		return;
	}

	auto hFile = vgui2::filesystem()->Open( szfile, "wb" );

	if( hFile )
	{
		int pos = 0;

		wchar_t buf[ 512 ];
		char ansi[ 512 ];

		while( true )
		{
			m_pHistory->GetText( pos, buf, sizeof( buf ) );

			pos += ARRAYSIZE( buf );

			if( !buf[ 0 ] )
				break;

			vgui2::localize()->ConvertUnicodeToANSI( buf, ansi, ARRAYSIZE( ansi ) );

			auto length = strlen( ansi );

			for( decltype( length ) i = 0; i < length; ++i )
			{
				if( ansi[ i ] == '\n' )
				{
					char ret = '\r';

					vgui2::filesystem()->Write( &ret, 1, hFile );
				}

				vgui2::filesystem()->Write( &ansi[ i ], 1, hFile );
			}
		}

		vgui2::filesystem()->Close( hFile );

		Print( "console dumped to " );
		Print( szfile );
	}
	else
	{
		Print( "Unable to condump to " );
		Print( szfile );
	}

	Print( "\n" );
}

void CGameConsoleDialog::RebuildCompletionList( const char* text )
{
	m_CompletionList.RemoveAll();

	const auto length = strlen( text );

	if( length <= 0 )
	{
		for( int i = 0; i < m_CommandHistory.Count(); ++i )
		{
			auto& completion = m_CompletionList[ m_CompletionList.AddToTail() ];

			completion.iscommand = false;
			completion.m_text = new CHistoryItem( m_CommandHistory[ i ] );
		}
	}
	else if( !strchr( text, ' ' ) )
	{
		for( auto handle = engine->GetFirstCmdFunctionHandle(); handle; handle = engine->GetNextCmdFunctionHandle( handle ) )
		{
			if( !strnicmp( text, engine->GetCmdFunctionName( handle ), length ) )
			{
				auto& completion = m_CompletionList[ m_CompletionList.AddToTail() ];

				completion.iscommand = true;
				completion.cmd.cmd = handle;
				completion.cmd.cvar = nullptr;
				completion.m_text = new CHistoryItem( engine->GetCmdFunctionName( handle ), nullptr );
			}
		}

		for( auto pCVar = engine->GetFirstCvarPtr(); pCVar; pCVar = pCVar->next )
		{
			if( !strnicmp( text, pCVar->name, length ) )
			{
				auto& completion = m_CompletionList[ m_CompletionList.AddToTail() ];

				completion.iscommand = true;
				completion.cmd.cmd = 0;
				completion.cmd.cvar = pCVar;
				completion.m_text = new CHistoryItem( pCVar->name, pCVar->string );
			}
		}
	}
}

void CGameConsoleDialog::AddToHistory( const char* commandText, const char* extraText )
{
	while( m_CommandHistory.Count() >= MAX_HISTORY_ITEMS )
	{
		m_CommandHistory.Remove( 0 );
	}

	const auto commandLength = strlen( commandText );
	char* command = reinterpret_cast<char*>( stackalloc( sizeof( char ) * ( commandLength + 1 ) ) );
	memset( command, 0, sizeof( char ) * ( commandLength + 1 ) );
	strncpy( command, commandText, commandLength );

	if( command[ commandLength - 1 ] == ' ' )
		command[ commandLength - 1 ] = '\0';

	char* extra = nullptr;

	if( extraText )
	{
		const int extraLength = strlen( extraText );
		extra = reinterpret_cast<char*>( malloc( sizeof( char ) * ( extraLength + 1 ) ) );
		memset( extra, 0, sizeof( char ) * ( extraLength + 1 ) );
		strncpy( extra, extraText, extraLength );

		for( auto index = extraLength - 1; index >= 0 && extra[ index ] == ' '; --index )
		{
			extra[ index ] = '\0';
		}
	}

	for( int i = m_CommandHistory.Count() - 1; i >= 0; --i )
	{
		auto& item = m_CommandHistory[ i ];

		auto pszText = "";

		if( item.GetText() )
			pszText = item.GetText();

		if( !stricmp( pszText, command ) &&
			( !extra || ( item.GetExtra() && !stricmp( item.GetExtra(), extra ) ) ) )
		{
			m_CommandHistory.Remove( i );
		}
	}

	auto& item = m_CommandHistory[ m_CommandHistory.AddToTail() ];
	item.SetText( command, extra );

	m_iNextCompletion = 0;
	RebuildCompletionList( m_szPartialText );
	free( extra );
}

void CGameConsoleDialog::OnCommand( const char* command )
{
	if( !stricmp( command, "Submit" ) )
	{
		char text[ 256 ];

		m_pEntry->GetText( text, ARRAYSIZE( text ) );

		engine->pfnClientCmd( text );

		m_pHistory->InsertColorChange( m_PrintColor );
		m_pHistory->InsertString( "] " );
		m_pHistory->InsertColorChange( m_PrintColor );
		m_pHistory->InsertString( text );
		m_pHistory->InsertColorChange( m_PrintColor );
		m_pHistory->InsertString( "\n" );

		m_pEntry->SetText( "" );
		OnTextChanged( m_pEntry );

		m_pHistory->GotoTextEnd();

		auto pszSpace = strchr( text, ' ' );
		if( pszSpace )
			*pszSpace++ = '\0';

		AddToHistory( text, pszSpace );

		m_pCompletionList->SetVisible( false );
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}