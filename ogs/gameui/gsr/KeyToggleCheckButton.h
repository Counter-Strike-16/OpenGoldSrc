#ifndef GAMEUI_KEYTOGGLECHECKBUTTON_H
#define GAMEUI_KEYTOGGLECHECKBUTTON_H

#include <vgui_controls/CheckButton.h>

class CKeyToggleCheckButton : public vgui2::CheckButton
{
public:
	using BaseClass = vgui2::CheckButton;

	CKeyToggleCheckButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* key, const char* cmdname );
	~CKeyToggleCheckButton();

	void Paint() override;

	void Reset();

	void ApplyChanges();

	bool HasBeenModified();

private:
	char* m_pszKeyName;
	char* m_pszCmdName;
	bool m_bStartValue;

private:
	CKeyToggleCheckButton( const CKeyToggleCheckButton& ) = delete;
	CKeyToggleCheckButton& operator=( const CKeyToggleCheckButton& ) = delete;
};


#endif //GAMEUI_KEYTOGGLECHECKBUTTON_H
