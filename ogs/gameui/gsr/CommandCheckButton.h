#ifndef GAMEUI_COMMANDCHECKBUTTON_H
#define GAMEUI_COMMANDCHECKBUTTON_H

#include <vgui_controls/CheckButton.h>

class CCommandCheckButton : public vgui2::CheckButton
{
public:
	using BaseClass = vgui2::CheckButton;

	CCommandCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* downcmd, const char* upcmd );
	~CCommandCheckButton();

	void SetSelected( bool state ) override;

private:
	char* m_pszDown;
	char* m_pszUp;

private:
	CCommandCheckButton( const CCommandCheckButton& ) = delete;
	CCommandCheckButton& operator=( const CCommandCheckButton& ) = delete;
};


#endif //GAMEUI_COMMANDCHECKBUTTON_H
