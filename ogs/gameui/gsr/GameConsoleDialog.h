#ifndef GAMEUI_GAMECONSOLEDIALOG_H
#define GAMEUI_GAMECONSOLEDIALOG_H

#include <SDK_Color.h>
#include <UtlVector.h>

#include <vgui_controls/RichText.h>
#include <vgui_controls/TextEntry.h>

#include "taskframe.h"

class CNoKeyboardInputRichText;

namespace vgui2
{
class Button;
class Menu;
}

typedef struct cvar_s cvar_t;

class TabCatchingTextEntry : public vgui2::TextEntry
{
public:
	using BaseClass = vgui2::TextEntry;

	TabCatchingTextEntry( vgui2::Panel* parent, const char* panelName, vgui2::VPANEL hCompletionList );

	virtual void IgnoreNextTextInput( bool bIgnore );

	void OnKeyTyped( wchar_t key ) override;

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

	void OnKillFocus() override;

public:
	vgui2::VPANEL m_pCompletionList;
	bool m_bIgnoreKeyTyped;

private:
	TabCatchingTextEntry( const TabCatchingTextEntry& ) = delete;
	TabCatchingTextEntry& operator=( const TabCatchingTextEntry& ) = delete;
};

class CNoKeyboardInputRichText : public vgui2::RichText
{
public:
	using BaseClass = vgui2::RichText;

	CNoKeyboardInputRichText( vgui2::Panel* parent, const char* panelName, vgui2::Panel* pFocusPanel );

	void OnRequestFocus( vgui2::VPANEL subFocus, vgui2::VPANEL defaultPanel ) override;

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

private:
	vgui2::Panel* m_pFocusPanel;

private:
	CNoKeyboardInputRichText( const CNoKeyboardInputRichText& ) = delete;
	CNoKeyboardInputRichText& operator=( const CNoKeyboardInputRichText& ) = delete;
};


struct CHistoryItem
{
public:
	CHistoryItem();
	CHistoryItem( const char* text, const char* extra );
	CHistoryItem( const CHistoryItem& src );
	~CHistoryItem();

	const char* GetText();

	const char* GetExtra();

	bool HasExtra();

	void SetText( const char* text, const char* extra );

private:
	char* m_text = nullptr;
	char* m_extraText = nullptr;
	bool m_bHasExtra = false;
};

class CGameConsoleDialog : public CTaskFrame
{
private:
	enum
	{
		MAX_HISTORY_ITEMS = 100
	};

	static const int MAX_COMPLETION_ITEMS = 5;

	struct cmdnode_t
	{
		unsigned int cmd;
		cvar_t* cvar;
	};

	struct CompletionItem
	{
		CompletionItem();
		CompletionItem( const CompletionItem& src );
		CompletionItem& operator=( const CompletionItem& src );
		~CompletionItem();
		const char* GetItemText();
		const char* GetCommand() const;
		const char* GetName() const;

		bool iscommand;
		CGameConsoleDialog::cmdnode_t cmd;
		CHistoryItem* m_text;
	};

public:
	DECLARE_CLASS_SIMPLE( CGameConsoleDialog, CTaskFrame );

public:
	CGameConsoleDialog();
	~CGameConsoleDialog();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	// brings dialog to the fore
	MESSAGE_FUNC( Activate, "Activate" );

	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );

	MESSAGE_FUNC( CloseCompletionList, "CloseCompletionList" );

	MESSAGE_FUNC_CHARPTR( OnMenuItemSelected, "CompletionCommand", command );

	void OnAutoComplete();

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

	void Clear();

	void Print( const char* msg );
	void DPrint( const char* msg );

	void OnAutoComplete( bool reverse );

	void UpdateCompletionListPosition();

	void PerformLayout() override;

	void Hide();

	void DumpConsoleTextToFile();

	void RebuildCompletionList( const char* text );

	void AddToHistory( const char* commandText, const char* extraText );

	void OnCommand( const char* command ) override;

private:
	CNoKeyboardInputRichText* m_pHistory;
	TabCatchingTextEntry* m_pEntry;
	vgui2::Button* m_pSubmit;
	vgui2::Menu* m_pCompletionList;

	SDK_Color m_PrintColor;
	SDK_Color m_DPrintColor;

	bool m_bAutoCompleteMode = false;
	int m_iNextCompletion = 0;
	char m_szPartialText[ 256 ] = { '\0' };

	CUtlVector<CompletionItem> m_CompletionList;
	CUtlVector<CHistoryItem> m_CommandHistory;

private:
	CGameConsoleDialog( const CGameConsoleDialog& ) = delete;
	CGameConsoleDialog& operator=( const CGameConsoleDialog& ) = delete;
};


#endif //GAMEUI_GAMECONSOLEDIALOG_H
