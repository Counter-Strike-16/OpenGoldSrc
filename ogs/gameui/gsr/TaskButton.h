#ifndef GAMEUI_CTASKBUTTON_H
#define GAMEUI_CTASKBUTTON_H

#include <vgui_controls/Image.h>
#include <vgui_controls/Label.h>

enum
{
	WINDOW_STATE_FOCUS = 0,
	WINDOW_STATE_OUTOFFOCUS,
	WINDOW_STATE_MINIMIZED,
};


class CWindowStateImage : public vgui2::Image
{
public:
	using BaseClass = vgui2::Image;

	CWindowStateImage();

	void Paint() override;

	void SetTall( int tall );

	void SetFocused();

	void SetOutOfFocus();

	void SetMinimized();

private:
	int m_iState;

private:
	CWindowStateImage( const CWindowStateImage& ) = delete;
	CWindowStateImage& operator=( const CWindowStateImage& ) = delete;
};


class CTaskButton : public vgui2::Label
{
public:
	using BaseClass = vgui2::Label;

	CTaskButton( vgui2::Panel* parent, vgui2::VPANEL task );

	void OnMouseDoublePressed( vgui2::MouseCode mouseCode ) override;

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void OnMousePressed( vgui2::MouseCode mouseCode ) override;

	void SetTitle( const wchar_t* title );

	void SetSelected( bool state );

	bool ShouldDisplay();

	vgui2::VPANEL GetTaskPanel();

private:
	vgui2::HPanel m_hTask;

	bool m_bHasTitle = false;
	bool m_bSelected;

	SDK_Color m_SelectedColor;
	SDK_Color m_LabelColor;

	CWindowStateImage m_WindowStateImage;

private:
	CTaskButton( const CTaskButton& ) = delete;
	CTaskButton& operator=( const CTaskButton& ) = delete;
};

#endif //GAMEUI_CTASKBUTTON_H
