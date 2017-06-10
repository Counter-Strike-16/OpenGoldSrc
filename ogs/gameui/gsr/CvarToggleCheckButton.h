#ifndef GAMEUI_CVARTOGGLECHECKBUTTON_H
#define GAMEUI_CVARTOGGLECHECKBUTTON_H

#include <vgui_controls/CheckButton.h>

class CCvarToggleCheckButton : public vgui2::CheckButton
{
public:
	DECLARE_CLASS_SIMPLE( CCvarToggleCheckButton, vgui2::CheckButton );

public:
	CCvarToggleCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* cvarname );
	~CCvarToggleCheckButton();

	void SetSelected( bool state ) override;

	void Paint() override;

	MESSAGE_FUNC( OnButtonChecked, "CheckButtonChecked" );

	void ApplyChanges();

	void Reset();

	bool HasBeenModified();

private:
	char* m_pszCvarName;
	bool m_bStartValue;

private:
	CCvarToggleCheckButton( const CCvarToggleCheckButton& ) = delete;
	CCvarToggleCheckButton& operator=( const CCvarToggleCheckButton& ) = delete;
};


#endif //GAMEUI_CVARTOGGLECHECKBUTTON_H
