#ifndef GAMEUI_CVARSLIDER_H
#define GAMEUI_CVARSLIDER_H

#include <vgui_controls/Slider.h>

class CCvarSlider : public vgui2::Slider
{
public:
	DECLARE_CLASS_SIMPLE( CCvarSlider, vgui2::Slider );

public:
	CCvarSlider( vgui2::Panel* parent, const char* panelName, const char* caption, float minValue, float maxValue, const char* cvarname, bool bAllowOutOfRange );
	~CCvarSlider();

	MESSAGE_FUNC( OnSliderMoved, "SliderMoved" );

	void Paint() override;

	void ApplyChanges();

	float GetSliderValue();

	void SetSliderValue( float fValue );

	void Reset();

	bool HasBeenModified();

private:
	bool m_bAllowOutOfRange;
	bool m_bModifiedOnce;

	float m_fStartValue;
	int m_iStartValue;
	int m_iLastSliderValue;
	float m_fCurrentValue;

	char m_szCvarName[ 64 ];

private:
	CCvarSlider( const CCvarSlider& ) = delete;
	CCvarSlider& operator=( const CCvarSlider& ) = delete;
};


#endif //GAMEUI_CVARSLIDER_H
