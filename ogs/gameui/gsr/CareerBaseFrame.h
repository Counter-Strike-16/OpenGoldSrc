#ifndef GAMEUI_CAREERBASEFRAME_H
#define GAMEUI_CAREERBASEFRAME_H

#include <SDK_Color.h>
#include <vgui/Dar.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Frame.h>

namespace vgui2
{
class Button;
class Label;
}

class CCareerButton : public vgui2::Button
{
public:
	using BaseClass = vgui2::Button;

	CCareerButton( vgui2::Panel* parent, const char* buttonName, const char* buttonText, const char* image, bool textFirst );

	void SetArmedBorder( vgui2::IBorder* border );

	void Paint() override;

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	vgui2::IBorder* GetBorder( bool depressed, bool armed, bool selected, bool keyfocus ) override;

private:
	vgui2::IBorder* m_armedBorder = nullptr;
	vgui2::IImage* m_image;
	vgui2::TextImage* m_textImage;

	bool m_textFirst;

	int m_textPad;
	int m_imagePad;

	SDK_Color m_textNormalColor = SDK_Color( 0, 0, 0, 0 );
	SDK_Color m_textDisabledColor = SDK_Color( 0, 0, 0, 0 );

private:
	CCareerButton( const CCareerButton& ) = delete;
	CCareerButton& operator=( const CCareerButton& ) = delete;
};

struct ColorFade
{
	enum FadeDir
	{
		up = 0,
		down = 1,
		left = 2,
		right = 3
	};

	SDK_Color m_startColor;
	SDK_Color m_endColor;
	float m_fadePercent;
	FadeDir m_direction;

	//TODO: implement - Solokiller
};


class CCareerBaseFrame : public vgui2::Frame
{
private:
	enum
	{
		BACKGROUND_ROWS = 3,
		BACKGROUND_COLUMNS = 4,
	};

	struct bimage_t
	{
		int imageID;
		int width;
		int height;
	};

public:
	using BaseClass = vgui2::Frame;

	CCareerBaseFrame( vgui2::Panel* parent, const char* panelName, bool showTaskbarIcon );

	//TODO: implement - Solokiller

private:
	vgui2::Dar<vgui2::Button*> m_buttons;

	ColorFade m_bgFade;

	vgui2::Label* m_pTitleBackground;
	vgui2::Label* m_pPlayerName;
	vgui2::Label* m_pMissionPackName;
	vgui2::Label* m_pAutosaveNotice;

	float m_autosaveHideTime;
	bool m_showImageBackground;

	bimage_t m_ImageID[ BACKGROUND_ROWS ][ BACKGROUND_COLUMNS ];
};


#endif //GAMEUI_CAREERBASEFRAME_H
