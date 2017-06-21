#ifndef GAMEUI_LABELSLIDERCOMBO_H
#define GAMEUI_LABELSLIDERCOMBO_H

#include <vgui_controls/Slider.h>
#include <vgui_controls/TextEntry.h>

class LabelSliderCombo : public vgui2::Slider
{
public:
	using BaseClass = vgui2::Slider;

	LabelSliderCombo( vgui2::Panel* parent, const char* sliderName, const char* textEntryName );
	virtual ~LabelSliderCombo();

	void SetVisible( bool state ) override;

	void UpdateTextEntry( bool bTriggerChangeMessage = true );

	void SetValue( int value, bool bTriggerChangeMessage = true ) override;

	void OnCursorMoved( int x, int y ) override;

private:
	vgui2::TextEntry *m_pTextEntry;
	int m_currentValueShown;

private:
	LabelSliderCombo( const LabelSliderCombo& ) = delete;
	LabelSliderCombo& operator=( const LabelSliderCombo& ) = delete;
};


#endif //GAMEUI_LABELSLIDERCOMBO_H
