#ifndef GAMEUI_CBACKGROUNDMENUBUTON_H
#define GAMEUI_CBACKGROUNDMENUBUTON_H

#include <vgui_controls/Button.h>
#include <vgui_controls/Image.h>
#include <vgui_controls/Menu.h>

class CBackgroundMenuButton : public vgui2::Button
{
public:
	using BaseClass = vgui2::Button;

	CBackgroundMenuButton( vgui2::Panel* parent, const char* name );
	~CBackgroundMenuButton();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void OnCommand( const char* command ) override;

	void SetArmed( bool state ) override
	{
		if( state )
			SetImageAtIndex( 0, m_pMouseOverImage, 0 );
		else
			SetImageAtIndex( 0, m_pImage, 0 );

		BaseClass::SetArmed( state );
	}

public:
	//TODO: unused? - Solokiller
	vgui2::Menu* m_pMenu;
	vgui2::IImage* m_pImage = nullptr;
	vgui2::IImage* m_pMouseOverImage = nullptr;

private:
	CBackgroundMenuButton( const CBackgroundMenuButton& ) = delete;
	CBackgroundMenuButton& operator=( const CBackgroundMenuButton& ) = delete;
};

#endif //GAMEUI_CBACKGROUNDMENUBUTON_H
