#include <SDL2/SDL.h>

#include <FileSystem.h>
#include <keydefs.h>
#include <KeyValues.h>
#include <UtlBuffer.h>

#include <vgui/ISystem.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/QueryBox.h>

#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "igameuifuncs.h"
#include "OptionsSubKeyboard.h"
#include "vcontrolslistpanel.h"

//TODO: refactor so engine versions are in shared file - Solokiller
SDL_Keycode GetSDLKeycodeFromEngineKey( int iKey )
{
	SDL_Scancode code = SDL_SCANCODE_UNKNOWN;

	switch( iKey )
	{
	case K_TAB: code = SDL_SCANCODE_TAB; break;
	case K_ENTER: code = SDL_SCANCODE_RETURN; break;
	case K_ESCAPE: code = SDL_SCANCODE_ESCAPE; break;
	case K_SPACE: code = SDL_SCANCODE_SPACE; break;

	case '\'': code = SDL_SCANCODE_APOSTROPHE; break;
	case ',': code = SDL_SCANCODE_COMMA; break;
	case '-': code = SDL_SCANCODE_MINUS; break;
	case '.': code = SDL_SCANCODE_PERIOD; break;
	case '/': code = SDL_SCANCODE_SLASH; break;
	case '0': code = SDL_SCANCODE_0; break;
	case '1': code = SDL_SCANCODE_1; break;
	case '2': code = SDL_SCANCODE_2; break;
	case '3': code = SDL_SCANCODE_3; break;
	case '4': code = SDL_SCANCODE_4; break;
	case '5': code = SDL_SCANCODE_5; break;
	case '6': code = SDL_SCANCODE_6; break;
	case '7': code = SDL_SCANCODE_7; break;
	case '8': code = SDL_SCANCODE_8; break;
	case '9': code = SDL_SCANCODE_9; break;
	case ';': code = SDL_SCANCODE_SEMICOLON; break;
	case '=': code = SDL_SCANCODE_EQUALS; break;
	case '[': code = SDL_SCANCODE_LEFTBRACKET; break;
	case ']': code = SDL_SCANCODE_RIGHTBRACKET; break;
	case '\\': code = SDL_SCANCODE_BACKSLASH; break;
	case '^': code = SDL_SCANCODE_GRAVE; break;
	case 'a': code = SDL_SCANCODE_A; break;
	case 'b': code = SDL_SCANCODE_B; break;
	case 'c': code = SDL_SCANCODE_C; break;
	case 'd': code = SDL_SCANCODE_D; break;
	case 'e': code = SDL_SCANCODE_E; break;
	case 'f': code = SDL_SCANCODE_F; break;
	case 'g': code = SDL_SCANCODE_G; break;
	case 'h': code = SDL_SCANCODE_H; break;
	case 'i': code = SDL_SCANCODE_I; break;
	case 'j': code = SDL_SCANCODE_J; break;
	case 'k': code = SDL_SCANCODE_K; break;
	case 'l': code = SDL_SCANCODE_L; break;
	case 'm': code = SDL_SCANCODE_M; break;
	case 'n': code = SDL_SCANCODE_N; break;
	case 'o': code = SDL_SCANCODE_O; break;
	case 'p': code = SDL_SCANCODE_P; break;
	case 'q': code = SDL_SCANCODE_Q; break;
	case 'r': code = SDL_SCANCODE_R; break;
	case 's': code = SDL_SCANCODE_S; break;
	case 't': code = SDL_SCANCODE_T; break;
	case 'u': code = SDL_SCANCODE_U; break;
	case 'v': code = SDL_SCANCODE_V; break;
	case 'w': code = SDL_SCANCODE_W; break;
	case 'x': code = SDL_SCANCODE_X; break;
	case 'y': code = SDL_SCANCODE_Y; break;
	case 'z': code = SDL_SCANCODE_Z; break;

	case K_BACKSPACE: code = SDL_SCANCODE_BACKSPACE; break;
	case K_UPARROW: code = SDL_SCANCODE_UP; break;
	case K_DOWNARROW: code = SDL_SCANCODE_DOWN; break;
	case K_LEFTARROW: code = SDL_SCANCODE_LEFT; break;
	case K_RIGHTARROW: code = SDL_SCANCODE_RIGHT; break;

	case K_ALT: code = SDL_SCANCODE_LALT; break;
	case K_CTRL: code = SDL_SCANCODE_LCTRL; break;
	case K_SHIFT: code = SDL_SCANCODE_LSHIFT; break;

	case K_F1: code = SDL_SCANCODE_F1; break;
	case K_F2: code = SDL_SCANCODE_F2; break;
	case K_F3: code = SDL_SCANCODE_F3; break;
	case K_F4: code = SDL_SCANCODE_F4; break;
	case K_F5: code = SDL_SCANCODE_F5; break;
	case K_F6: code = SDL_SCANCODE_F6; break;
	case K_F7: code = SDL_SCANCODE_F7; break;
	case K_F8: code = SDL_SCANCODE_F8; break;
	case K_F9: code = SDL_SCANCODE_F9; break;
	case K_F10: code = SDL_SCANCODE_F10; break;
	case K_F11: code = SDL_SCANCODE_F11; break;
	case K_F12: code = SDL_SCANCODE_F12; break;

	case K_INS: code = SDL_SCANCODE_INSERT; break;
	case K_HOME: code = SDL_SCANCODE_HOME; break;
	case K_PGUP: code = SDL_SCANCODE_PAGEUP; break;
	case K_DEL: code = SDL_SCANCODE_DELETE; break;
	case K_END: code = SDL_SCANCODE_END; break;
	case K_PGDN: code = SDL_SCANCODE_PAGEDOWN; break;

	case K_KP_HOME: code = SDL_SCANCODE_KP_7; break;
	case K_KP_UPARROW: code = SDL_SCANCODE_KP_8; break;
	case K_KP_PGUP: code = SDL_SCANCODE_KP_9; break;
	case K_KP_LEFTARROW: code = SDL_SCANCODE_KP_4; break;
	case K_KP_5: code = SDL_SCANCODE_KP_5; break;
	case K_KP_RIGHTARROW: code = SDL_SCANCODE_KP_6; break;
	case K_KP_END: code = SDL_SCANCODE_KP_1; break;
	case K_KP_DOWNARROW: code = SDL_SCANCODE_KP_2; break;
	case K_KP_PGDN: code = SDL_SCANCODE_KP_3; break;
	case K_KP_ENTER: code = SDL_SCANCODE_KP_ENTER; break;
	case K_KP_INS: code = SDL_SCANCODE_KP_0; break;
	case K_KP_DEL: code = SDL_SCANCODE_KP_PERIOD; break;
	case K_KP_SLASH: code = SDL_SCANCODE_KP_DIVIDE; break;
	case K_KP_MINUS: code = SDL_SCANCODE_KP_MINUS; break;
	case K_KP_PLUS: code = SDL_SCANCODE_KP_PLUS; break;
	case K_CAPSLOCK: code = SDL_SCANCODE_CAPSLOCK; break;
	case K_KP_MUL: code = SDL_SCANCODE_KP_MULTIPLY; break;
	case K_WIN: code = SDL_SCANCODE_LGUI; break;

	case K_PAUSE: code = SDL_SCANCODE_PAUSE; break;

	default: break;
	}

	return SDL_GetKeyFromScancode( code );
}

int GetEngineKeyFromSDLScancode( SDL_Scancode code )
{
	switch( code )
	{
	case SDL_SCANCODE_RETURN: return K_ENTER;
	case SDL_SCANCODE_ESCAPE: return K_ESCAPE;
	case SDL_SCANCODE_BACKSPACE: return K_BACKSPACE;
	case SDL_SCANCODE_TAB: return K_TAB;
	case SDL_SCANCODE_SPACE: return K_SPACE;

	case SDL_SCANCODE_APOSTROPHE: return '\'';
	case SDL_SCANCODE_COMMA: return ',';
	case SDL_SCANCODE_MINUS: return '-';
	case SDL_SCANCODE_PERIOD: return '.';
	case SDL_SCANCODE_SLASH: return '/';
	case SDL_SCANCODE_0: return '0';
	case SDL_SCANCODE_1: return '1';
	case SDL_SCANCODE_2: return '2';
	case SDL_SCANCODE_3: return '3';
	case SDL_SCANCODE_4: return '4';
	case SDL_SCANCODE_5: return '5';
	case SDL_SCANCODE_6: return '6';
	case SDL_SCANCODE_7: return '7';
	case SDL_SCANCODE_8: return '8';
	case SDL_SCANCODE_9: return '9';
	case SDL_SCANCODE_SEMICOLON: return ';';
	case SDL_SCANCODE_EQUALS: return '=';
	case SDL_SCANCODE_LEFTBRACKET: return '[';
	case SDL_SCANCODE_RIGHTBRACKET: return ']';
	case SDL_SCANCODE_BACKSLASH: return '\\';
	case SDL_SCANCODE_GRAVE: return '^';
	case SDL_SCANCODE_A: return 'a';
	case SDL_SCANCODE_B: return 'b';
	case SDL_SCANCODE_C: return 'c';
	case SDL_SCANCODE_D: return 'd';
	case SDL_SCANCODE_E: return 'e';
	case SDL_SCANCODE_F: return 'f';
	case SDL_SCANCODE_G: return 'g';
	case SDL_SCANCODE_H: return 'h';
	case SDL_SCANCODE_I: return 'i';
	case SDL_SCANCODE_J: return 'j';
	case SDL_SCANCODE_K: return 'k';
	case SDL_SCANCODE_L: return 'l';
	case SDL_SCANCODE_M: return 'm';
	case SDL_SCANCODE_N: return 'n';
	case SDL_SCANCODE_O: return 'o';
	case SDL_SCANCODE_P: return 'p';
	case SDL_SCANCODE_Q: return 'q';
	case SDL_SCANCODE_R: return 'r';
	case SDL_SCANCODE_S: return 's';
	case SDL_SCANCODE_T: return 't';
	case SDL_SCANCODE_U: return 'u';
	case SDL_SCANCODE_V: return 'v';
	case SDL_SCANCODE_W: return 'w';
	case SDL_SCANCODE_X: return 'x';
	case SDL_SCANCODE_Y: return 'y';
	case SDL_SCANCODE_Z: return 'z';

	case SDL_SCANCODE_CAPSLOCK: return K_CAPSLOCK;

	case SDL_SCANCODE_F1: return K_F1;
	case SDL_SCANCODE_F2: return K_F2;
	case SDL_SCANCODE_F3: return K_F3;
	case SDL_SCANCODE_F4: return K_F4;
	case SDL_SCANCODE_F5: return K_F5;
	case SDL_SCANCODE_F6: return K_F6;
	case SDL_SCANCODE_F7: return K_F7;
	case SDL_SCANCODE_F8: return K_F8;
	case SDL_SCANCODE_F9: return K_F9;
	case SDL_SCANCODE_F10: return K_F10;
	case SDL_SCANCODE_F11: return K_F11;
	case SDL_SCANCODE_F12: return K_F12;

	case SDL_SCANCODE_PAUSE: return K_PAUSE;
	case SDL_SCANCODE_INSERT: return K_INS;
	case SDL_SCANCODE_HOME: return K_HOME;
	case SDL_SCANCODE_PAGEUP: return K_PGUP;
	case SDL_SCANCODE_DELETE: return K_DEL;
	case SDL_SCANCODE_END: return K_END;
	case SDL_SCANCODE_PAGEDOWN: return K_PGDN;

	case SDL_SCANCODE_RIGHT: return K_RIGHTARROW;
	case SDL_SCANCODE_LEFT: return K_LEFTARROW;
	case SDL_SCANCODE_DOWN: return K_DOWNARROW;
	case SDL_SCANCODE_UP: K_UPARROW;

	case SDL_SCANCODE_KP_DIVIDE: return K_KP_SLASH;
	case SDL_SCANCODE_KP_MULTIPLY: return K_KP_MUL;
	case SDL_SCANCODE_KP_MINUS: return K_KP_MINUS;
	case SDL_SCANCODE_KP_PLUS: return K_KP_PLUS;
	case SDL_SCANCODE_KP_ENTER: return K_KP_ENTER;
	case SDL_SCANCODE_KP_1: return K_KP_END;
	case SDL_SCANCODE_KP_2: return K_KP_DOWNARROW;
	case SDL_SCANCODE_KP_3: return K_KP_PGDN;
	case SDL_SCANCODE_KP_4: return K_KP_LEFTARROW;
	case SDL_SCANCODE_KP_5: return K_KP_5;
	case SDL_SCANCODE_KP_6: return K_KP_RIGHTARROW;
	case SDL_SCANCODE_KP_7: return K_KP_HOME;
	case SDL_SCANCODE_KP_8: return K_KP_UPARROW;
	case SDL_SCANCODE_KP_9: return K_KP_PGUP;
	case SDL_SCANCODE_KP_0: return K_KP_INS;
	case SDL_SCANCODE_KP_PERIOD: return K_KP_DEL;

	case SDL_SCANCODE_LCTRL:
	case SDL_SCANCODE_RCTRL: return K_CTRL;

	case SDL_SCANCODE_LSHIFT:
	case SDL_SCANCODE_RSHIFT: return K_SHIFT;

	case SDL_SCANCODE_LALT:
	case SDL_SCANCODE_RALT: return K_ALT;

	case SDL_SCANCODE_LGUI:
	case SDL_SCANCODE_RGUI: return K_WIN;
	default: return 0;
	}
}

const char* MouseMaskToHumanReadable( int mask )
{
	if( mask & ( 1 << vgui2::MOUSE_LEFT ) )
		return "MOUSE1";
	if( mask & ( 1 << vgui2::MOUSE_RIGHT ) )
		return "MOUSE2";
	if( mask & ( 1 << vgui2::MOUSE_MIDDLE ) )
		return "MOUSE3";
	if( mask & ( 1 << vgui2::MOUSE_4 ) )
		return "MOUSE4";
	if( ( mask & ( 1 << vgui2::MOUSE_5 ) ) )
		return "MOUSE5";

	return "MOUSE1";
}

COptionsSubKeyboard::COptionsSubKeyboard( vgui2::Panel* parent )
	: BaseClass( parent, nullptr )
{
	memset( m_Bindings, 0, sizeof( m_Bindings ) );

	CreateKeyBindingList();

	SaveCurrentBindings();
	ParseActionDescriptions();

	m_pSetBindingButton = new vgui2::Button( this, "ChangeKeyButton", "" );
	m_pClearBindingButton = new vgui2::Button( this, "ClearKeyButton", "" );

	LoadControlSettings( "Resource/OptionsSubKeyboard.res" );

	m_pSetBindingButton->SetEnabled( false );
	m_pClearBindingButton->SetEnabled( false );
}

COptionsSubKeyboard::~COptionsSubKeyboard()
{
	DeleteSavedBindings();
}

void COptionsSubKeyboard::OnMouseDoublePressed( vgui2::MouseCode code )
{
	if( m_pKeyBindList ->IsCapturing() )
	{
		Finish( 0, 1 << code, MouseMaskToHumanReadable( 1 << code ) );
	}
	else
	{
		BaseClass::OnMouseDoublePressed( code );
	}
}

void COptionsSubKeyboard::OnMousePressed( vgui2::MouseCode code )
{
	if( m_pKeyBindList->IsCapturing() )
	{
		Finish( 0, 1 << code, MouseMaskToHumanReadable( 1 << code ) );
	}
	else
	{
		BaseClass::OnMousePressed( code );
	}
}

void COptionsSubKeyboard::OnKeyTyped( wchar_t unichar )
{
	vgui2::Panel::OnKeyTyped( unichar );
}

void COptionsSubKeyboard::ItemSelected( int itemID )
{
	m_pKeyBindList->SetItemOfInterest( itemID );

	if( m_pKeyBindList->IsItemIDValid( itemID ) )
	{
		// find the details, see if we should be enabled/clear/whatever
		m_pSetBindingButton->SetEnabled( true );

		KeyValues *kv = m_pKeyBindList->GetItemData( itemID );
		if( kv )
		{
			const char *key = kv->GetString( "Key", NULL );
			if( key && *key )
			{
				m_pClearBindingButton->SetEnabled( true );
			}
			else
			{
				m_pClearBindingButton->SetEnabled( false );
			}

			if( kv->GetInt( "Header" ) )
			{
				m_pSetBindingButton->SetEnabled( false );
			}
		}
	}
	else
	{
		m_pSetBindingButton->SetEnabled( false );
		m_pClearBindingButton->SetEnabled( false );
	}
}

void COptionsSubKeyboard::OnKeyCodeTyped( vgui2::KeyCode code )
{
	if( m_pKeyBindList->IsCapturing() )
	{
		const auto vKey = vgui2::system()->KeyCode_VGUIToVirtualKey( code );

		const auto scanCode = SDL_GetScancodeFromKey( vKey );

		const auto engineKey = GetEngineKeyFromSDLScancode( scanCode );

		Finish( engineKey, 0, SDL_GetKeyName( vKey ) );
	}
	else
	{
		BaseClass::OnKeyCodeTyped( code );
	}
}

void COptionsSubKeyboard::RemoveKeyFromBindItems( const char* key )
{
	if( !key || !( *key ) )
		return;

	for( int i = 0; i < m_pKeyBindList->GetItemCount(); ++i )
	{
		auto pData = m_pKeyBindList->GetItemData( m_pKeyBindList->GetItemIDFromRow( i ) );

		if( pData )
		{
			if( !stricmp( key, pData->GetString( "AltKey" ) ) )
			{
				pData->SetString( "AltKey", "" );
				pData->SetString( "AltCode", "" );
				//TODO: this may not be the same as the item ID returned from GetItemIDFromRow - Solokiller
				m_pKeyBindList->InvalidateItem( i );
			}

			if( !stricmp( key, pData->GetString( "Key" ) ) )
			{
				pData->SetString( "Key", "" );
				pData->SetString( "Code", "" );
				//TODO: this may not be the same as the item ID returned from GetItemIDFromRow - Solokiller
				m_pKeyBindList->InvalidateItem( i );

				auto pszAltKey = pData->GetString( "AltKey" );

				//If the alt key was bound, move the binding to the main one
				if( pszAltKey && *pszAltKey )
				{
					pData->SetString( "Key", pszAltKey );
					pData->SetString( "Code", pData->GetString( "AltCode" ) );
					pData->SetString( "AltKey", "" );
					pData->SetString( "AltCode", "" );
				}
			}
		}
	}

	m_pKeyBindList->InvalidateLayout();
}

void COptionsSubKeyboard::AddBinding( KeyValues* item, const char* keyname, const char* pchKeyName )
{
	if( !stricmp( item->GetString( "Key" ), keyname ) )
		return;

	if( keyname && *keyname )
		RemoveKeyFromBindItems( keyname );

	//Move the existing key to alt
	item->SetString( "AltKey", item->GetString( "Key" ) );
	item->SetString( "Key", keyname );

	item->SetString( "AltCode", item->GetString( "Code" ) );
	item->SetString( "Code", pchKeyName );
}

void COptionsSubKeyboard::Finish( int key, int button, const char* pchKeyName )
{
	const auto i = m_pKeyBindList->GetItemOfInterest();
	m_pKeyBindList->EndCaptureMode( vgui2::dc_arrow );
	auto pData = m_pKeyBindList->GetItemData( i );

	if( pData )
	{
		if( button )
		{
			const char* keyname = MouseMaskToHumanReadable( button );

			if( stricmp( pData->GetString( "Key" ), keyname ) )
				AddBinding( pData, keyname, pchKeyName );

			PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
		}
		else if( key != K_ESCAPE && key )
		{
			const auto pszName = gameuifuncs->Key_NameForKey( key );

			if( stricmp( pData->GetString( "Key" ), pszName ) )
				AddBinding( pData, pszName, pchKeyName );

			PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
		}

		m_pKeyBindList->InvalidateItem( i );
	}

	m_pSetBindingButton->SetEnabled( true );
	m_pClearBindingButton->SetEnabled( true );
}

void COptionsSubKeyboard::OnKeyCodePressed( vgui2::KeyCode code )
{
	// Enter key pressed and not already trapping next key/button press
	if( !m_pKeyBindList->IsCapturing() )
	{
		// Grab which item was set as interesting
		int r = m_pKeyBindList->GetItemOfInterest();

		// Check that it's visible
		int x, y, w, h;
		bool visible = m_pKeyBindList->GetCellBounds( r, 1, x, y, w, h );
		if( visible )
		{
			if( vgui2::KEY_DELETE == code )
			{
				// find the current binding and remove it
				KeyValues *kv = m_pKeyBindList->GetItemData( r );

				const char *key = kv->GetString( "Key", NULL );
				if( key && *key )
				{
					RemoveKeyFromBindItems( key );
				}

				m_pClearBindingButton->SetEnabled( false );
				m_pKeyBindList->InvalidateItem( r );
				//TODO: From Alien Swarm, needed? - Solokiller
				//PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );

				// message handled, don't pass on
				return;
			}
		}
	}

	// Allow base class to process message instead
	BaseClass::OnKeyCodePressed( code );
}

void COptionsSubKeyboard::OnMouseWheeled( int delta )
{
	if( m_pKeyBindList->IsCapturing() )
	{
		const char* pszBindingToFind = delta <= 0 ? "MWHEELDOWN" : "MWHEELUP";

		Finish( FindKeyForName( pszBindingToFind ), 0, pszBindingToFind );
	}
	else
	{
		BaseClass::OnMouseWheeled( delta );
	}
}

void COptionsSubKeyboard::CreateKeyBindingList()
{
	m_pKeyBindList = new VControlsListPanel( this, "listpanel_keybindlist" );
}

static char token[ 1024 ];

void COptionsSubKeyboard::ParseActionDescriptions()
{
	char szFileName[ MAX_PATH ];
	sprintf( szFileName, "%s/kb_act.lst", "gfx/shell" );

	auto hFile = vgui2::filesystem()->Open( szFileName, "rb" );

	if( FILESYSTEM_INVALID_HANDLE != hFile )
	{
		auto uiSize = vgui2::filesystem()->Size( hFile );

		CUtlBuffer buf( 0, uiSize + 1, CUtlBuffer::TEXT_BUFFER );

		vgui2::filesystem()->Read( buf.Base(), uiSize, hFile );

		vgui2::filesystem()->Close( hFile );

		auto data = reinterpret_cast<char*>( buf.Base() );

		data[ uiSize ] = '\0';

		int iSection = 0;

		char szBinding[ 256 ];
		char szDescription[ 256 ];

		while( 1 )
		{
			data = engine->COM_ParseFile( data, token );
			if( !token[ 0 ] )
				break;
			strcpy( szBinding, token );

			data = engine->COM_ParseFile( data, token );
			if( !token[ 0 ] )
				break;
			strcpy( szDescription, token );

			if( szDescription[ 0 ] != '=' )
			{
				if( !stricmp( szBinding, "blank" ) )
				{
					++iSection;
					m_pKeyBindList->AddSection( iSection, szDescription );
					//TODO: implement fallback fonts in sectionedlistpanel - Solokiller
					m_pKeyBindList->AddColumnToSection( iSection, "Action", szDescription, VControlsListPanel::COLUMN_BRIGHT, 226 );
					m_pKeyBindList->AddColumnToSection( iSection, "Code", "#GameUI_KeyButton", VControlsListPanel::COLUMN_BRIGHT, 128 );
					m_pKeyBindList->AddColumnToSection( iSection, "AltCode", "#GameUI_Alternate", VControlsListPanel::COLUMN_BRIGHT, 128 );
				}
				else
				{
					auto pData = new KeyValues( "Item" );

					pData->SetString( "Action", szDescription );
					pData->SetString( "Binding", szBinding );
					pData->SetString( "Key", "" );
					pData->SetString( "AltKey", "" );
					pData->SetString( "Code", "" );
					pData->SetString( "AltCode", "" );

					m_pKeyBindList->AddItem( iSection, pData );

					pData->deleteThis();
				}
			}
		}
	}
}

KeyValues* COptionsSubKeyboard::GetItemForBinding( const char* binding )
{
	static HKeySymbol bindingSymbol = keyvalues()->GetSymbolForString( "Binding" );

	for( int i = 0; i < m_pKeyBindList->GetItemCount(); ++i )
	{
		auto pData = m_pKeyBindList->GetItemData( m_pKeyBindList->GetItemIDFromRow( i ) );

		if( pData )
		{
			if( !stricmp( pData->FindKey( bindingSymbol )->GetString(), binding ) )
				return pData;
		}
	}

	return nullptr;
}

const char* COptionsSubKeyboard::GetKeyName( int keynum )
{
	return gameuifuncs->Key_NameForKey( keynum );
}

int COptionsSubKeyboard::FindKeyForName( const char* keyname )
{
	for( int i = 0; i < 256; ++i )
	{
		auto pszName = GetKeyName( i );

		if( pszName && *pszName )
		{
			if( strlen( keyname ) == 1 )
			{
				if( !strcmp( keyname, pszName ) )
					return i;
			}
			else if( !stricmp( keyname, pszName ) )
			{
				return i;
			}
		}
	}

	return -1;
}

void COptionsSubKeyboard::ClearBindItems()
{
	m_KeysToUnbind.RemoveAll();

	for( int i = 0; i < m_pKeyBindList->GetItemCount(); ++i )
	{
		auto pData = m_pKeyBindList->GetItemData( m_pKeyBindList->GetItemIDFromRow( i ) );

		if( pData )
		{
			pData->SetString( "Key", "" );
			pData->SetString( "AltKey", "" );
			pData->SetString( "Code", "" );
			pData->SetString( "AltCode", "" );
			
			m_pKeyBindList->InvalidateItem( i );
		}
	}
	
	m_pKeyBindList->InvalidateLayout();
}

void COptionsSubKeyboard::DeleteSavedBindings()
{
	for( auto& binding : m_Bindings )
	{
		if( binding.binding )
		{
			delete[] binding.binding;
			binding.binding = nullptr;
		}
	}
}

void COptionsSubKeyboard::SaveCurrentBindings()
{
	DeleteSavedBindings();

	for( int i = 0; i < 256; ++i )
	{
		auto pszBinding = gameuifuncs->Key_BindingForKey( i );

		if( pszBinding && *pszBinding )
		{
			auto& binding = m_Bindings[ i ];

			binding.binding = new char[ strlen( pszBinding ) + 1 ];
			strcpy( binding.binding, pszBinding );
		}
	}
}

//TODO: this is a copy of SharedVarArgs - Solokiller
char* UTIL_va( char* format, ... )
{
	va_list argptr;
	const int BufLen = 1024;
	const int NumBuffers = 4;
	static char string[ NumBuffers ][ BufLen ];
	static int curstring = 0;

	curstring = ( curstring + 1 ) % NumBuffers;

	va_start( argptr, format );
	vsnprintf( string[ curstring ], BufLen, format, argptr );
	va_end( argptr );

	return string[ curstring ];
}

void COptionsSubKeyboard::BindKey( const char* key, const char* binding )
{
	engine->pfnClientCmd( UTIL_va( "bind \"%s\" \"%s\"\n", key, binding ) );
}

void COptionsSubKeyboard::UnbindKey( const char* key )
{
	engine->pfnClientCmd( UTIL_va( "unbind \"%s\"\n", key ) );
}

void COptionsSubKeyboard::ApplyAllBindings()
{
	for( int i = 0; i < m_KeysToUnbind.Count(); ++i )
	{
		engine->pfnClientCmd( UTIL_va( "unbind \"%s\"\n", m_KeysToUnbind[ i ].String() ) );
	}

	m_KeysToUnbind.RemoveAll();

	DeleteSavedBindings();

	const char* const ppszKeysToCheck[] = 
	{
		"Key",
		"AltKey"
	};

	for( int i = 0; i < m_pKeyBindList->GetItemCount(); ++i )
	{
		auto pData = m_pKeyBindList->GetItemData( m_pKeyBindList->GetItemIDFromRow( i ) );

		if( pData )
		{
			auto pszBinding = pData->GetString( "Binding" );

			if( pszBinding && *pszBinding )
			{
				for( auto pszKeyToCheck : ppszKeysToCheck )
				{
					auto pszKey = pData->GetString( pszKeyToCheck );

					if( pszKey && *pszKey )
					{
						engine->pfnClientCmd( UTIL_va( "bind \"%s\" \"%s\"\n", pszKey, pszBinding ) );

						const auto key = FindKeyForName( pszBinding );

						if( key != -1 )
						{
							auto& binding = m_Bindings[ key ];

							binding.binding = new char[ strlen( pszBinding ) + 1 ];
							strcpy( binding.binding, pszBinding );
						}
					}
				}
			}
		}
	}

	engine->pfnClientCmd( "exec userconfig.cfg\n" );
}

void COptionsSubKeyboard::OnApplyChanges()
{
	ApplyAllBindings();
}

void COptionsSubKeyboard::FillInDefaultBindings()
{
	char szFileName[ MAX_PATH ];
	sprintf( szFileName, "%s/kb_def.lst", "gfx/shell" );

	auto hFile = vgui2::filesystem()->Open( szFileName, "rb" );
	if( hFile )
	{
		const auto uiSize = vgui2::filesystem()->Size( hFile );
		CUtlBuffer buf( 0, uiSize, CUtlBuffer::TEXT_BUFFER );
		vgui2::filesystem()->Read( buf.Base(), uiSize, hFile );
		vgui2::filesystem()->Close( hFile );

		ClearBindItems();

		auto data = reinterpret_cast<char*>( buf.Base() );

		char szKeyName[ 256 ];
		char szBinding[ 256 ];

		while( 1 )
		{
			data = engine->COM_ParseFile( data, token );
			if( !token[ 0 ] )
				break;
			strcpy( szKeyName, token );

			data = engine->COM_ParseFile( data, token );
			if( !token[ 0 ] )
				break;
			strcpy( szBinding, token );

			auto pItem = GetItemForBinding( szBinding );

			if( pItem )
			{
				auto keyNum = gameuifuncs->Key_KeyStringToKeyNum( szKeyName );
				auto sdlKeyCode = GetSDLKeycodeFromEngineKey( keyNum );

				const char* pchKeyName;

				if( K_MOUSE1 <= keyNum && keyNum <= K_MOUSE5 )
				{
					//Convert the mouse code from the engine key code to the vgui2 key code
					pchKeyName = MouseMaskToHumanReadable( 1 << ( ( keyNum - K_MOUSE1 ) + vgui2::MOUSE_LEFT ) );
				}
				else
				{
					pchKeyName = SDL_GetKeyName( sdlKeyCode );
				}

				if( stricmp( pItem->GetString( "Key" ), szKeyName ) )
					AddBinding( pItem, szKeyName, pchKeyName );
			}
		}

		PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );

		auto pToggleConsole = GetItemForBinding( "toggleconsole" );
		if( pToggleConsole )
		{
			if( stricmp( pToggleConsole->GetString( "Key" ), "`" ) )
				AddBinding( pToggleConsole, "`", "`" );
		}

		auto pCancelSelect = GetItemForBinding( "cancelselect" );
		if( pCancelSelect )
		{
			if( stricmp( pCancelSelect->GetString( "Key" ), "ESCAPE" ) )
				AddBinding( pCancelSelect, "ESCAPE", "ESCAPE" );
		}
	}
}

void COptionsSubKeyboard::OnCommand( const char* command )
{
	if( !stricmp( command, "Defaults" ) )
	{
		auto pQuery = new vgui2::QueryBox( "#GameUI_KeyboardSettings", "#GameUI_KeyboardSettingsText" );
		pQuery->AddActionSignalTarget( this );
		pQuery->SetOKCommand( new KeyValues( "Command", "command", "DefaultsOK" ) );

		pQuery->DoModal();
	}
	else if( !stricmp( command, "DefaultsOK" ) )
	{
		FillInDefaultBindings();
		m_pKeyBindList->RequestFocus();
	}
	else if( !m_pKeyBindList->IsCapturing() && !stricmp( command, "ChangeKey" ) )
	{
		m_pKeyBindList->StartCaptureMode( vgui2::dc_none );
	}
	else if( !m_pKeyBindList->IsCapturing() && !stricmp( command, "ClearKey" ) )
	{
		OnKeyCodePressed( vgui2::KEY_DELETE );
		m_pKeyBindList->RequestFocus();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void COptionsSubKeyboard::FillInCurrentBindings()
{
	ClearBindItems();

	for( int i = 0; i < 256; ++i )
	{
		auto pszBinding = gameuifuncs->Key_BindingForKey( i );
		if( pszBinding )
		{
			const char* pchKeyName;

			if( K_MOUSE1 <= i && i <= K_MOUSE5 )
			{
				pchKeyName = MouseMaskToHumanReadable( 1 << ( ( i - K_MOUSE1 ) + vgui2::MOUSE_LEFT ) );
			}
			else if( K_MWHEELDOWN == i )
			{
				pchKeyName = "Mouse Wheel Down";
			}
			else if( K_MWHEELUP == i )
			{
				pchKeyName = "Mouse Wheel Up";
			}
			else
			{
				auto sdlCode = GetSDLKeycodeFromEngineKey( i );
				pchKeyName = SDL_GetKeyName( sdlCode );
			}

			auto pItem = GetItemForBinding( pszBinding );

			if( pItem )
			{
				auto pszName = gameuifuncs->Key_NameForKey( i );
				if( stricmp( pItem->GetString( "Key" ), pszName ) )
					AddBinding( pItem, pszName, pchKeyName );

				m_KeysToUnbind.AddToTail( CUtlSymbol( pszName ) );
			}
		}
	}
}

void COptionsSubKeyboard::OnResetData()
{
	FillInCurrentBindings();
	m_pKeyBindList->SetSelectedItem( 0 );
}
