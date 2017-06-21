#ifndef GAMEUI_TEXTENTRYBOX_H
#define GAMEUI_TEXTENTRYBOX_H

#include <vgui_controls/QueryBox.h>
#include <vgui_controls/TextEntry.h>

class CCvarTextEntry;

/**
*	Query box that contains either a regular text entry or a cvar entry
*/
class CTextEntryBox : public vgui2::QueryBox
{
public:
	using BaseClass = vgui2::QueryBox;

	CTextEntryBox( const char* title, const char* queryText, const char* entryText, bool isCvar, vgui2::Panel* parent );
	~CTextEntryBox();

	void OnCommand( const char* command ) override;

	void PerformLayout() override;

	void ShowWindow( vgui2::Frame* pFrameOver ) override;

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

private:
	CCvarTextEntry* m_pCvarEntry;
	vgui2::TextEntry* m_pEntry;

private:
	CTextEntryBox( const CTextEntryBox& ) = delete;
	CTextEntryBox& operator=( const CTextEntryBox& ) = delete;
};

#endif //GAMEUI_TEXTENTRYBOX_H
