#ifndef GAMEUI_OPTIONSSUBMOUSE_H
#define GAMEUI_OPTIONSSUBMOUSE_H

#include <vgui_controls/PropertyPage.h>

class CCvarNegateCheckButton;
class CCvarSlider;
class CCvarToggleCheckButton;
class CKeyToggleCheckButton;

class COptionsSubMouse : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( COptionsSubMouse, vgui2::PropertyPage );

public:
	COptionsSubMouse( vgui2::Panel* parent );

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	MESSAGE_FUNC_PTR( OnControlModified, "ControlModified", panel );

	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );

	void OnApplyChanges() override;

	void OnResetData() override;

	void UpdateSensitivityLabel();

private:
	CCvarNegateCheckButton* m_pReverseMouseCheckBox;
	CKeyToggleCheckButton* m_pMouseLookCheckBox;
	CCvarToggleCheckButton* m_pMouseFilterCheckBox;
	CCvarToggleCheckButton* m_pJoystickCheckBox;
	CKeyToggleCheckButton* m_pJoystickLookCheckBox;
	CCvarToggleCheckButton* m_pMouseRawInputCheckBox;

	CCvarSlider* m_pMouseSensitivitySlider;
	vgui2::TextEntry* m_pMouseSensitivityLabel;

	CCvarToggleCheckButton* m_pAutoAimCheckBox;

private:
	COptionsSubMouse( const COptionsSubMouse& ) = delete;
	COptionsSubMouse& operator=( const COptionsSubMouse& ) = delete;
};


#endif //GAMEUI_OPTIONSSUBMOUSE_H
