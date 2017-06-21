#ifndef GAMEUI_OPTIONSSUBADVANCED_H
#define GAMEUI_OPTIONSSUBADVANCED_H

#include <vgui_controls/PropertyPage.h>

class CContentControlDialog;

class COptionsSubAdvanced : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( COptionsSubAdvanced, vgui2::PropertyPage );

public:
	COptionsSubAdvanced( vgui2::Panel* parent );

	MESSAGE_FUNC( OnContentControlClose, "ContentControlClose" );

	void OnApplyChanges() override;

	void OnResetData() override;

	void OnCommand( const char* command ) override;

	void OnOpenContentControlDialog();

private:
	CContentControlDialog* m_pContentControlDialog;
	vgui2::CheckButton* m_pContentCheckButton;

private:
	COptionsSubAdvanced( const COptionsSubAdvanced& ) = delete;
	COptionsSubAdvanced& operator=( const COptionsSubAdvanced& ) = delete;
};

#endif //GAMEUI_OPTIONSSUBADVANCED_H
