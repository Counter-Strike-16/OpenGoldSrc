#ifndef GAMEUI_CVARNEGATECHECKBUTTON_H
#define GAMEUI_CVARNEGATECHECKBUTTON_H

#include <vgui_controls/CheckButton.h>

class CCvarNegateCheckButton : vgui2::CheckButton
{
public:
	DECLARE_CLASS_SIMPLE( CCvarNegateCheckButton, vgui2::CheckButton );

public:
	CCvarNegateCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* cvarname );
	~CCvarNegateCheckButton();

	void SetSelected( bool state ) override;

	void Paint() override;

	MESSAGE_FUNC( OnButtonChecked, "CheckButtonChecked" );

	void ApplyChanges();

	void Reset();

	bool HasBeenModified();

private:
	char* m_pszCvarName;
	bool m_bStartState;

private:
	CCvarNegateCheckButton( const CCvarNegateCheckButton& ) = delete;
	CCvarNegateCheckButton& operator=( const CCvarNegateCheckButton& ) = delete;
};


#endif //GAMEUI_CVARNEGATECHECKBUTTON_H
