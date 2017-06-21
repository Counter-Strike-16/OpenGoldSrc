//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <time.h>

#include "MultiplayerAdvancedDialog.h"

#include <vgui/IInputInternal.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/ListPanel.h>
#include <KeyValues.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>
#include "PanelListPanel.h"
#include <vgui/IInput.h>

#include "FileSystem.h"
#include "ModInfo.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui2;

#define DEFAULT_OPTIONS_FILE "user_default.scr"
#define OPTIONS_FILE "user.scr"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMultiplayerAdvancedDialog::CMultiplayerAdvancedDialog( vgui2::Panel *parent ) : BaseClass( NULL, "MultiplayerAdvancedDialog" )
{
	SetBounds( 0, 0, 372, 160 );
	SetSizeable( false );

	MakePopup();

	g_pTaskbar->AddTask( GetVPanel() );

	SetTitle( "#GameUI_MultiplayerAdvanced", true );

	Button *cancel = new Button( this, "Cancel", "#GameUI_Cancel" );
	cancel->SetCommand( "Close" );

	Button *ok = new Button( this, "OK", "#GameUI_OK" );
	ok->SetCommand( "Ok" );

	m_pListPanel = new CPanelListPanel( this, "PanelListPanel" );

	m_pList = NULL;

	m_pDescription = new CInfoDescription( m_pListPanel );
	//TODO: not in GoldSource - Solokiller
	//m_pDescription->InitFromFile( DEFAULT_OPTIONS_FILE );
	m_pDescription->InitFromFile( OPTIONS_FILE );
	m_pDescription->TransferCurrentValues();

	CreateControls();

	LoadControlSettings( "Resource\\MultiplayerAdvancedDialog.res" );
	//TODO: not in GoldSource - Solokiller
	//MoveToCenterOfScreen();
	//SetSizeable( false );
	//SetDeleteSelfOnClose( true );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CMultiplayerAdvancedDialog::~CMultiplayerAdvancedDialog()
{
	//TODO: not in GoldSource - Solokiller
	//delete m_pDescription;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMultiplayerAdvancedDialog::OnClose()
{
	BaseClass::OnClose();
	MarkForDeletion();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *command - 
//-----------------------------------------------------------------------------
void CMultiplayerAdvancedDialog::OnCommand( const char *command )
{
	if( !stricmp( command, "Ok" ) )
	{
		// OnApplyChanges();
		SaveValues();
		OnClose();
		return;
	}

	BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMultiplayerAdvanced::GatherCurrentValues()
{
	if( !m_pDescription )
		return;

	// OK
	CheckButton *pBox;
	TextEntry *pEdit;
	ComboBox *pCombo;

	mpcontrol_t *pList;

	CScriptObject *pObj;
	CScriptListItem *pItem;

	char szValue[ 256 ];
	char strValue[ 256 ];

	pList = m_pList;
	while( pList )
	{
		pObj = pList->pScrObj;

		if( !pList->pControl )
		{
			pObj->SetCurValue( pObj->defValue );
			pList = pList->next;
			continue;
		}

		switch( pObj->type )
		{
		case O_BOOL:
			pBox = ( CheckButton * ) pList->pControl;
			sprintf( szValue, "%s", pBox->IsSelected() ? "1" : "0" );
			break;
		case O_NUMBER:
			pEdit = ( TextEntry * ) pList->pControl;
			pEdit->GetText( strValue, sizeof( strValue ) );
			sprintf( szValue, "%s", strValue );
			break;
		case O_STRING:
			pEdit = ( TextEntry * ) pList->pControl;
			pEdit->GetText( strValue, sizeof( strValue ) );
			sprintf( szValue, "%s", strValue );
			break;
		case O_LIST:
			pCombo = ( ComboBox * ) pList->pControl;
			// pCombo->GetText( strValue, sizeof( strValue ) );
			int activeItem = pCombo->GetActiveItem();

			pItem = pObj->pListItems;
			//			int n = (int)pObj->fdefValue;

			while( pItem )
			{
				if( !activeItem-- )
					break;

				pItem = pItem->pNext;
			}

			if( pItem )
			{
				sprintf( szValue, "%s", pItem->szValue );
			}
			else  // Couln't find index
			{
				//assert(!("Couldn't find string in list, using default value"));
				sprintf( szValue, "%s", pObj->defValue );
			}
			break;
		}

		// Remove double quotes and % characters
		UTIL_StripInvalidCharacters( szValue, sizeof( szValue ) );

		strcpy( strValue, szValue );

		pObj->SetCurValue( strValue );

		pList = pList->next;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMultiplayerAdvanced::CreateControls()
{
	DestroyControls();

	// Go through desciption creating controls
	CScriptObject *pObj;

	pObj = m_pDescription->pObjList;

	mpcontrol_t	*pCtrl;

	CheckButton *pBox;
	TextEntry *pEdit;
	ComboBox *pCombo;
	CScriptListItem *pListItem;

	Panel *objParent = m_pListPanel;

	while( pObj )
	{
		pCtrl = new mpcontrol_t( objParent, "mpcontrol_t" );
		pCtrl->type = pObj->type;

		switch( pCtrl->type )
		{
		case O_BOOL:
			pBox = new CheckButton( pCtrl, "DescCheckButton", pObj->prompt );
			pBox->SetSelected( pObj->fdefValue != 0.0f ? true : false );

			pCtrl->pControl = ( Panel * ) pBox;
			break;
		case O_STRING:
		case O_NUMBER:
			pEdit = new TextEntry( pCtrl, "DescTextEntry" );
			pEdit->InsertString( pObj->defValue );
			pCtrl->pControl = ( Panel * ) pEdit;
			break;
		case O_LIST:
			pCombo = new ComboBox( pCtrl, "DescComboBox", 5, false );

			pListItem = pObj->pListItems;
			while( pListItem )
			{
				pCombo->AddItem( pListItem->szItemText, NULL );
				pListItem = pListItem->pNext;
			}

			pCombo->ActivateItemByRow( ( int ) pObj->fdefValue );

			pCtrl->pControl = ( Panel * ) pCombo;
			break;
		default:
			break;
		}

		if( pCtrl->type != O_BOOL )
		{
			pCtrl->pPrompt = new vgui2::Label( pCtrl, "DescLabel", "" );
			pCtrl->pPrompt->SetContentAlignment( vgui2::Label::a_west );
			pCtrl->pPrompt->SetTextInset( 5, 0 );
			pCtrl->pPrompt->SetText( pObj->prompt );
		}

		pCtrl->pScrObj = pObj;
		pCtrl->SetSize( 100, 28 );
		//pCtrl->SetBorder( scheme()->GetBorder(1, "DepressedButtonBorder") );
		m_pListPanel->AddItem( pCtrl );

		// Link it in
		if( !m_pList )
		{
			m_pList = pCtrl;
			pCtrl->next = NULL;
		}
		else
		{
			mpcontrol_t *p;
			p = m_pList;
			while( p )
			{
				if( !p->next )
				{
					p->next = pCtrl;
					pCtrl->next = NULL;
					break;
				}
				p = p->next;
			}
		}

		pObj = pObj->pNext;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMultiplayerAdvanced::DestroyControls()
{
	mpcontrol_t *p, *n;

	p = m_pList;
	while( p )
	{
		n = p->next;
		//
		delete p->pControl;
		delete p->pPrompt;
		delete p;
		p = n;
	}

	m_pList = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMultiplayerAdvanced::SaveValues()
{
	// Get the values from the controls:
	GatherCurrentValues();

	// Create the game.cfg file
	if( m_pDescription )
	{
		FileHandle_t fp;

		// Add settings to config.cfg
		m_pDescription->WriteToConfig();

		//TODO: Not in GoldSource - Solokiller
		//vgui2::filesystem()->CreateDirHierarchy( OPTIONS_DIR, nullptr );
		fp = vgui2::filesystem()->Open( OPTIONS_FILE, "wb", "GAMECONFIG" );
		if( fp )
		{
			m_pDescription->WriteToScriptFile( fp );
			vgui2::filesystem()->Close( fp );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor, load/save client settings object
//-----------------------------------------------------------------------------
CInfoDescription::CInfoDescription( CPanelListPanel *panel )
	: CDescription( panel )
{
	setHint( "// NOTE:  THIS FILE IS AUTOMATICALLY REGENERATED, \r\n\
//DO NOT EDIT THIS HEADER, YOUR COMMENTS WILL BE LOST IF YOU DO\r\n\
// User options script\r\n\
//\r\n\
// Format:\r\n\
//  Version [float]\r\n\
//  Options description followed by \r\n\
//  Options defaults\r\n\
//\r\n\
// Option description syntax:\r\n\
//\r\n\
//  \"cvar\" { \"Prompt\" { type [ type info ] } { default } }\r\n\
//\r\n\
//  type = \r\n\
//   BOOL   (a yes/no toggle)\r\n\
//   STRING\r\n\
//   NUMBER\r\n\
//   LIST\r\n\
//\r\n\
// type info:\r\n\
// BOOL                 no type info\r\n\
// NUMBER       min max range, use -1 -1 for no limits\r\n\
// STRING       no type info\r\n\
// LIST         "" delimited list of options value pairs\r\n\
//\r\n\
//\r\n\
// default depends on type\r\n\
// BOOL is \"0\" or \"1\"\r\n\
// NUMBER is \"value\"\r\n\
// STRING is \"value\"\r\n\
// LIST is \"index\", where index \"0\" is the first element of the list\r\n\r\n\r\n" );

	setDescription( "INFO_OPTIONS" );
}

bool CInfoDescription::InitFromFile( char* pszFileName )
{
	bool ret = BaseClass::InitFromFile( pszFileName );

	if( !stricmp( ModInfo().GetGameDescription(), "Condition Zero" ) )
	{
		for( auto i = pObjList; i; i = i->pNext )
		{
			//Old name, convert to new
			if( !stricmp( i->cvarname, "cl_tutor" ) )
			{
				strncpy( i->cvarname, "tutor_enabled", ARRAYSIZE( i->cvarname ) );
				i->cvarname[ ARRAYSIZE( i->cvarname ) - 1 ] = '\0';
			}
		}
	}

	auto tmp = new CInfoDescription( nullptr );

	auto hFile = vgui2::filesystem()->Open( DEFAULT_OPTIONS_FILE, "rb" );

	if( FILESYSTEM_INVALID_HANDLE != hFile )
	{
		auto size = vgui2::filesystem()->Size( hFile );
		//TODO: should be size + 1, followed by null termination - Solokiller
		auto pBuffer = new char[ size ];
		vgui2::filesystem()->Read( pBuffer, size, hFile );
		vgui2::filesystem()->Close( hFile );

		tmp->ReadFromBuffer( &pBuffer );
		delete[] pBuffer;
	}

	for( auto j = tmp->pObjList; j; j = j->pNext )
	{
		bool bExists = false;

		//Don't overwrite existing values
		for( auto k = pObjList; k; k = k->pNext )
		{
			if( !stricmp( k->cvarname, j->cvarname ) )
			{
				bExists = true;
				break;
			}
		}

		if( !bExists )
		{
			auto pItem = new CScriptObject();
			//TODO: this performs a shallow copy of j, which copies over unique Panel data as well.
			//Find a way to copy it without causing such problems - Solokiller
			*pItem = *j;
			pItem->pListItems = nullptr;

			//Copy items
			for( auto k = j->pListItems; k; k = k->pNext )
			{
				pItem->AddItem( new CScriptListItem( *k ) );
			}

			AddObject( pItem );
		}
	}

	delete tmp;

	return ret;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CInfoDescription::WriteScriptHeader( FileHandle_t fp )
{
	char am_pm[] = "AM";
	time_t ltime;
	time( &ltime );
	tm newtime = *localtime( &ltime );

	vgui2::filesystem()->FPrintf( fp, ( char * ) getHint() );

	// Write out the comment and Cvar Info:
	vgui2::filesystem()->FPrintf( fp, "// Half-Life User Info Configuration Layout Script (stores last settings chosen, too)\r\n" );
	vgui2::filesystem()->FPrintf( fp, "// File generated:  %.19s %s\r\n", asctime( &newtime ), am_pm );
	vgui2::filesystem()->FPrintf( fp, "//\r\n//\r\n// Cvar\t-\tSetting\r\n\r\n" );
	vgui2::filesystem()->FPrintf( fp, "VERSION %.1f\r\n\r\n", SCRIPT_VERSION );
	vgui2::filesystem()->FPrintf( fp, "DESCRIPTION INFO_OPTIONS\r\n{\r\n" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CInfoDescription::WriteFileHeader( FileHandle_t fp )
{
	char am_pm[] = "AM";
	time_t ltime;
	time( &ltime );
	tm newtime = *localtime( &ltime );

	vgui2::filesystem()->FPrintf( fp, "// Half-Life User Info Configuration Settings\r\n" );
	vgui2::filesystem()->FPrintf( fp, "// DO NOT EDIT, GENERATED BY HALF-LIFE\r\n" );
	vgui2::filesystem()->FPrintf( fp, "// File generated:  %.19s %s\r\n", asctime( &newtime ), am_pm );
	vgui2::filesystem()->FPrintf( fp, "//\r\n//\r\n// Cvar\t-\tSetting\r\n\r\n" );
}

//-----------------------------------------------------------------------------

CMultiplayerAdvancedPage::CMultiplayerAdvancedPage( vgui2::Panel* parent )
	: BaseClass( parent, "MultiplayerAdvancedDialog" /*, true TODO*/ )
{
	//TODO: implement PaintBorder parameter for PropertyPage - Solokiller
	m_pList = nullptr;
	m_pListPanel = new CPanelListPanel( this, "PanelListPanel" );

	m_pDescription = new CInfoDescription( m_pListPanel );
	m_pDescription->InitFromFile( OPTIONS_FILE );
	m_pDescription->TransferCurrentValues();
	CreateControls();

	LoadControlSettings( "Resource\\MultiplayerAdvancedPage.res" );
}

void CMultiplayerAdvancedPage::OnResetData()
{
	GatherCurrentValues();
}

void CMultiplayerAdvancedPage::OnApplyChanges()
{
	GatherCurrentValues();

	if( m_pDescription )
	{
		m_pDescription->WriteToConfig();

		auto hFile = vgui2::filesystem()->Open( OPTIONS_FILE, "wb", "GAMECONFIG" );
		if( FILESYSTEM_INVALID_HANDLE != hFile )
		{
			m_pDescription->WriteToScriptFile( hFile );
			vgui2::filesystem()->Close( hFile );
		}
	}
}
