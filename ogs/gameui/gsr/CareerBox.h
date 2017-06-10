#ifndef GAMEUI_CAREERBOX_H
#define GAMEUI_CAREERBOX_H

#include <SDK_Color.h>
#include <vgui/Dar.h>
#include <vgui_controls/BitmapImagePanel.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Label.h>

class CCvarTextEntry;
class CCvarToggleCheckButton;

class CCareerBaseBox : public vgui2::Frame
{
public:
	using BaseClass = vgui2::Frame;

	CCareerBaseBox( vgui2::Panel* parent, const char* panelName, bool loadResources );

	virtual void ShowWindow();

	void OnCommand( const char* command ) override;

	void DoModal() override;

	void PerformLayout() override;

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

	void SetCancelButtonAsDefault();

	void SetLabelText( const wchar_t* text );

	void AddButton( vgui2::Button* pButton );

protected:
	void PaintBackground() override;

protected:
	vgui2::Button* m_pOkButton;
	vgui2::Button* m_pCancelButton;
	vgui2::Dar<vgui2::Button*> m_buttons;

	vgui2::CBitmapImagePanel* m_pTopLeftPanel;
	vgui2::CBitmapImagePanel* m_pTopRightPanel;
	vgui2::CBitmapImagePanel* m_pBottomLeftPanel;
	vgui2::CBitmapImagePanel* m_pBottomRightPanel;

	SDK_Color m_bgColor;

	vgui2::Label* m_pTextLabel;

	bool m_cancelFocus = false;

private:
	CCareerBaseBox( const CCareerBaseBox& ) = delete;
	CCareerBaseBox& operator=( const CCareerBaseBox& ) = delete;
};

class CCareerQueryBox : public CCareerBaseBox
{
public:
	using BaseClass = CCareerBaseBox;

	CCareerQueryBox( const char* title, const char* labelText, const char* panelName, vgui2::Panel* parent );
	CCareerQueryBox( const wchar_t* title, const wchar_t* labelText, const char* panelName, vgui2::Panel* parent );
	~CCareerQueryBox();

private:
	CCareerQueryBox( const CCareerQueryBox& ) = delete;
	CCareerQueryBox& operator=( const CCareerQueryBox& ) = delete;
};

class CCareerDifficultyBox : public CCareerBaseBox
{
public:
	using BaseClass = CCareerBaseBox;

	CCareerDifficultyBox( const char* panelName, vgui2::Panel* parent );

	void ShowWindow() override;

	void OnCommand( const char* command ) override;

private:
	CCareerButton* m_pFocusButton;
	CCvarToggleCheckButton* m_pEnableTutorCheck;

private:
	CCareerDifficultyBox( const CCareerDifficultyBox& ) = delete;
	CCareerDifficultyBox& operator=( const CCareerDifficultyBox& ) = delete;
};

class CCareerNewProfileBox : public CCareerBaseBox
{
public:
	using BaseClass = CCareerBaseBox;

	CCareerNewProfileBox( const char* panelName, vgui2::Panel* parent );

	void ShowWindow() override;

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

	void OnCommand( const char* command ) override;

private:
	CCvarTextEntry* m_pEntry;

private:
	CCareerNewProfileBox( const CCareerNewProfileBox& ) = delete;
	CCareerNewProfileBox& operator=( const CCareerNewProfileBox& ) = delete;
};

#endif //GAMEUI_CAREERBOX_H
