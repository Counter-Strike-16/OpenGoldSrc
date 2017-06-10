#ifndef GAMEUI_NEWGAMELOADING_H
#define GAMEUI_NEWGAMELOADING_H

#include <vgui_controls/RadioButton.h>

#include "taskframe.h"

class CNewGameDialog : public CTaskFrame
{
public:
	using BaseClass = CTaskFrame;

	CNewGameDialog( vgui2::Panel* parent );

	void OnClose() override;

	void OnCommand( const char* command ) override;

private:
	int m_nPlayMode;

	vgui2::RadioButton* m_pTraining;
	vgui2::RadioButton* m_pEasy;
	vgui2::RadioButton* m_pMedium;
	vgui2::RadioButton* m_pHard;

private:
	CNewGameDialog( const CNewGameDialog& ) = delete;
	CNewGameDialog& operator=( const CNewGameDialog& ) = delete;
};


#endif //GAMEUI_NEWGAMELOADING_H
