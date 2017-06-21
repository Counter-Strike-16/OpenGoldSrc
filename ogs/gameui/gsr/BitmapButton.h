#ifndef GAMEUI_BITMAPBUTTON_H
#define GAMEUI_BITMAPBUTTON_H

#include <vgui_controls/Button.h>

class CBitmapButton : public vgui2::Button
{
private:
	struct bimage_t
	{
		int imageID;
		int width;
		int height;
	};

	enum
	{
		IMAGE_NORMAL = 0,
		IMAGE_ARMED,
		IMAGE_CLICKED,
		IMAGE_DISABLED,
		IMAGE_MAX
	};

public:
	using BaseClass = vgui2::Button;

	CBitmapButton( vgui2::Panel* parent, const char* panelName, const char* text, const char* normalImage, const char* armedImage, const char* clickedImage, const char* disabledImage );

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void PaintBackground() override;

	void OnThink() override;

	void Flash();

	void DrawFocusBorder( int tx0, int ty0, int tx1, int ty1 ) override
	{
		//Nothing
	}

private:
	bimage_t m_ImageID[ IMAGE_MAX ];

	float m_interval;
	float m_lastTransition;
	int m_transitionsLeft;
	bool m_shouldFlash;
	bool m_isFlashColor;
	int m_totalTransitions;

private:
	CBitmapButton( const CBitmapButton& ) = delete;
	CBitmapButton& operator=( const CBitmapButton& ) = delete;
};


#endif //GAMEUI_BITMAPBUTTON_H
