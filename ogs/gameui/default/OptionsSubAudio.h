#ifndef OPTIONS_SUB_AUDIO_H
#define OPTIONS_SUB_AUDIO_H

#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/PropertyPage.h"

class CLabeledCommandComboBox;
class CCvarSlider;
class CCvarToggleCheckButton;

class COptionsSubAudio : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsSubAudio, vgui::PropertyPage);

public:
	COptionsSubAudio(vgui::Panel *parent);
	~COptionsSubAudio(void);

public:
	virtual void OnPageShow(void);
	virtual void OnResetData(void);
	virtual void OnApplyChanges(void);
	virtual void OnCommand(const char *command);

private:
	MESSAGE_FUNC(OnControlModified, "ControlModified");

	MESSAGE_FUNC(OnTextChanged, "TextChanged")
	{
		OnControlModified();
	}

private:
	CCvarToggleCheckButton *m_pEAXCheckButton;
	CCvarToggleCheckButton *m_pA3DCheckButton;

	vgui::ComboBox *         m_pSpeakerSetupCombo;
	CLabeledCommandComboBox *m_pSoundQualityCombo;

	CCvarSlider *m_pSFXSlider;
	CCvarSlider *m_pMusicSlider;

private:
	MESSAGE_FUNC(OpenThirdPartySoundCreditsDialog, "OpenThirdPartySoundCreditsDialog");
	vgui::DHANDLE<class COptionsSubAudioThirdPartyCreditsDlg> m_OptionsSubAudioThirdPartyCreditsDlg;
};

#endif
