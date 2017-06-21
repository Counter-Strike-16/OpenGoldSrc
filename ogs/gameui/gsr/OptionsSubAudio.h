#ifndef GAMEUI_OPTIONSSUBAUDIO_H
#define GAMEUI_OPTIONSSUBAUDIO_H

#include <vgui_controls/PropertyPage.h>

class CCvarSlider;
class CLabeledCommandComboBox;

class COptionsSubAudio : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( COptionsSubAudio, vgui2::PropertyPage );

public:
	COptionsSubAudio( vgui2::Panel* parent );

	MESSAGE_FUNC( OnControlModified, "ControlModified" );

	void OnApplyChanges() override;

	void OnResetData() override;

private:
	CCvarSlider* m_pSFXSlider;
	CCvarSlider* m_pHEVSlider;
	CCvarSlider* m_pMP3Slider;
	CLabeledCommandComboBox* m_pSoundQualityCombo;

private:
	COptionsSubAudio( const COptionsSubAudio& ) = delete;
	COptionsSubAudio& operator=( const COptionsSubAudio& ) = delete;
};


#endif //GAMEUI_OPTIONSSUBAUDIO_H
