#ifndef GAMEUI_CAREERPROFILEFRAME_H
#define GAMEUI_CAREERPROFILEFRAME_H

#include <vgui/Dar.h>

#include "CareerBaseFrame.h"

namespace vgui2
{
class Button;
class CBitmapImagePanel;
class ComboBox;
class Label;
class URLLabel;
}

class CCareerPanelListPanel;
class CProfileButton;

class CCareerProfileFrame : public CCareerBaseFrame
{
public:
	using BaseClass = CCareerBaseFrame;

	CCareerProfileFrame( vgui2::Panel* parent );

	//TODO: implement - Solokiller

private:
	vgui2::Dar<CProfileButton*> m_profiles;
	CCareerPanelListPanel* m_pProfileList;
	vgui2::Label* m_pProfileSelectionBackground;

	vgui2::Button* m_pResumeButton;
	vgui2::Button* m_pDeleteButton;
	vgui2::Button* m_pNewButton;

	char m_name[ 32 ];

	vgui2::Label* m_pMissionPackBackground;
	vgui2::Label* m_pMissionPackAuthor;
	vgui2::Label* m_pMissionPackDescription;
	vgui2::URLLabel* m_pMissionPackURL;
	vgui2::CBitmapImagePanel* m_pMissionPackLogo;
	vgui2::ComboBox* m_pMissionPackComboBox;

	int m_needFocus;

private:
	CCareerProfileFrame( const CCareerProfileFrame& ) = delete;
	CCareerProfileFrame& operator=( const CCareerProfileFrame& ) = delete;
};


#endif //GAMEUI_CAREERPROFILEFRAME_H
