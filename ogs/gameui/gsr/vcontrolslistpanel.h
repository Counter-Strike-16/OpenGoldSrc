#ifndef GAMEUI_VCONTROLSLISTPANEL_H
#define GAMEUI_VCONTROLSLISTPANEL_H

#include <vgui_controls/Panel.h>
#include <vgui_controls/SectionedListPanel.h>

class CInlineEditPanel : public vgui2::Panel
{
public:
	using BaseClass = vgui2::Panel;

	using BaseClass::BaseClass;

	~CInlineEditPanel();

	void OnKeyCodeTyped( vgui2::KeyCode code ) override;

	void OnMousePressed( vgui2::MouseCode code ) override;

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void Paint() override;

private:
	CInlineEditPanel( const CInlineEditPanel& ) = delete;
	CInlineEditPanel& operator=( const CInlineEditPanel& ) = delete;
};

class VControlsListPanel : public vgui2::SectionedListPanel
{
public:
	using BaseClass = vgui2::SectionedListPanel;

	VControlsListPanel( vgui2::Panel* parent, const char* listName );
	~VControlsListPanel();

	virtual int GetItemOfInterest();

	virtual void SetItemOfInterest( int itemID );

	virtual bool IsCapturing();

	virtual void StartCaptureMode( vgui2::HCursor hCursor );
	virtual void EndCaptureMode( vgui2::HCursor hCursor );

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void OnMouseDoublePressed( vgui2::MouseCode code ) override;

	void OnMousePressed( vgui2::MouseCode code ) override;

private:
	bool m_bCaptureMode = false;
	int m_nClickRow = 0;
	vgui2::HFont m_hFont = vgui2::INVALID_FONT;
	CInlineEditPanel* m_pInlineEditPanel;
	int m_iMouseX;
	int m_iMouseY;

private:
	VControlsListPanel( const VControlsListPanel& ) = delete;
	VControlsListPanel& operator=( const VControlsListPanel& ) = delete;
};


#endif //GAMEUI_VCONTROLSLISTPANEL_H
