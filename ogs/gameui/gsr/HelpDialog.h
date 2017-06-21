#ifndef GAMEUI_HELPDIALOG_H
#define GAMEUI_HELPDIALOG_H

#include "taskframe.h"

class CHelpDialog : public CTaskFrame
{
public:
	using BaseClass = CTaskFrame;

	CHelpDialog();

	void OnClose() override;

	void OnCommand( const char* command ) override;

private:
	vgui2::Label* m_pInfoText;
	vgui2::Button* m_pClose;
	vgui2::CheckButton* m_pNeverShowAgain;

private:
	CHelpDialog( const CHelpDialog& ) = delete;
	CHelpDialog& operator=( const CHelpDialog& ) = delete;
};


#endif //GAMEUI_HELPDIALOG_H
