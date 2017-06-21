#ifndef GAMEUI_OPTIONSDIALOG_H
#define GAMEUI_OPTIONSDIALOG_H

#include <vgui_controls/PropertyDialog.h>

class COptionsDialog : public vgui2::PropertyDialog
{
	using BaseClass = vgui2::PropertyDialog;

public:
	COptionsDialog( vgui2::Panel* parent );

	void OnClose() override;

	void Activate() override;

	void SetTitle( const char* title, bool surfaceTitle ) override;

	void Run();

private:
	COptionsDialog( const COptionsDialog& ) = delete;
	COptionsDialog& operator=( const COptionsDialog& ) = delete;
};

#endif //GAMEUI_OPTIONSDIALOG_H
