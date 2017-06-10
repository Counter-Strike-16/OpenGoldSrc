#include "CvarTextEntry.h"
#include "TextEntryBox.h"

CTextEntryBox::CTextEntryBox( const char* title, const char* queryText, const char* entryText, bool isCvar, vgui2::Panel* parent )
	: BaseClass( title, queryText, parent )
{
	if( isCvar )
	{
		m_pCvarEntry = new CCvarTextEntry( this, "TextEntry", entryText );
		m_pEntry = m_pCvarEntry;
	}
	else
	{
		m_pEntry = new vgui2::TextEntry( this, "TextEntry" );
		m_pCvarEntry = nullptr;
	}

	m_pEntry->SetTabPosition( 3 );
	m_pEntry->RequestFocus();
	m_pEntry->GotoTextEnd();
}

CTextEntryBox::~CTextEntryBox()
{
	//TODO: is this even necessary? - Solokliler
	if( m_pEntry )
		delete m_pEntry;
}

void CTextEntryBox::OnCommand( const char* command )
{
	if( !stricmp( command, "Ok" ) )
	{
		if( m_pCvarEntry )
			m_pCvarEntry->ApplyChanges( true );
	}

	BaseClass::OnCommand( command );
}

void CTextEntryBox::PerformLayout()
{
	BaseClass::PerformLayout();

	int x, y, wide, tall;
	GetClientArea( x, y, wide, tall );

	wide += x;
	tall += y;

	int labelW, labelH;
	m_pMessageLabel->GetSize( labelW, labelH );

	int entryWidth = wide - 30 - labelW;
	if( entryWidth < 120 )
		entryWidth = 120;

	int entryHeight = 24;
	if( labelH >= 24 )
		entryHeight = labelH;

	m_pEntry->SetSize( entryWidth, entryHeight );

	int boxWidth, boxTall;
	GetSize( boxWidth, boxTall );

	const int contentWidth = labelW + entryWidth + 30;
	if( contentWidth > boxWidth )
	{
		SetSize( contentWidth, boxTall );
	}

	m_pMessageLabel->GetPos( x, y );
	m_pMessageLabel->SetPos( 10, y + ( entryHeight - labelH ) / -2 );
	m_pEntry->SetPos( m_pMessageLabel->GetWide() + 20, y + labelH - entryHeight );
}

void CTextEntryBox::ShowWindow( vgui2::Frame* pFrameOver )
{
	BaseClass::ShowWindow( pFrameOver );

	m_pEntry->RequestFocus();
	InvalidateLayout();
}

void CTextEntryBox::OnKeyCodeTyped( vgui2::KeyCode code )
{
	if( code == vgui2::KEY_ESCAPE )
	{
		OnCommand( "Cancel" );
	}
	else if( code == vgui2::KEY_ENTER )
	{
		OnCommand( "Ok" );
	}
	else
	{
		BaseClass::OnKeyCodeTyped( code );
	}
}
