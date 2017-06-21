#ifndef GAMEUI_CDKEYENTRYDIALOG_H
#define GAMEUI_CDKEYENTRYDIALOG_H

#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/TextEntry.h>

#include "taskframe.h"

/**
*	Text entry that converts all alphabetical characters to upper case.
*	Will only accept alphanumeric characters as input.
*/
class CUpperCaseTextEntry : public vgui2::TextEntry
{
public:
	using BaseClass = vgui2::TextEntry;

	using BaseClass::BaseClass;

	void OnKeyTyped( wchar_t unichar ) override;

private:
	CUpperCaseTextEntry( const CUpperCaseTextEntry& ) = delete;
	CUpperCaseTextEntry& operator=( const CUpperCaseTextEntry& ) = delete;
};

/**
*	Message box that issues a quit command when it is closed.
*/
class CloseMessageBox : public vgui2::MessageBox
{
public:
	using BaseClass = vgui2::MessageBox;

	using BaseClass::BaseClass;

	void OnClose() override;

private:
	CloseMessageBox( const CloseMessageBox& ) = delete;
	CloseMessageBox& operator=( const CloseMessageBox& ) = delete;
};

class CCDKeyEntryDialog : public CTaskFrame
{
private:
	enum
	{
		MAX_CDKEY_ERRORS = 5
	};

	static const int CHARS_PER_ENTRY = 5;
	static const int NUM_ENTRIES = 5;

public:
	DECLARE_CLASS_SIMPLE( CCDKeyEntryDialog, CTaskFrame );

public:
	CCDKeyEntryDialog( vgui2::Panel* parent, bool inConnect );

	void Activate() override;

	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );

	void OnThink() override;

	void OnClose() override;

	void OnCommand( const char* command ) override;

	bool IsEnteredKeyValid();

private:
	vgui2::Button* m_pOK;
	vgui2::Button* m_pQuitGame;

	vgui2::TextEntry* m_pEntry1;
	vgui2::TextEntry* m_pEntry2;
	vgui2::TextEntry* m_pEntry3;
	vgui2::TextEntry* m_pEntry4;
	vgui2::TextEntry* m_pEntry5;

	bool m_bIsValveCDKey;
	bool m_bInConnect;
	int m_iErrCount;

private:
	CCDKeyEntryDialog( const CCDKeyEntryDialog& ) = delete;
	CCDKeyEntryDialog& operator=( const CCDKeyEntryDialog& ) = delete;
};

#endif //GAMEUI_CDKEYENTRYDIALOG_H
