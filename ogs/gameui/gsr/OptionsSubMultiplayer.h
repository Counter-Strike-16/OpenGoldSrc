#ifndef GAMEUI_OPTIONSSUBMULTIPLAYER_H
#define GAMEUI_OPTIONSSUBMULTIPLAYER_H

#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/PropertyPage.h>

#include "MultiplayerAdvancedDialog.h"

class CBitmapImagePanel;
class CCvarSlider;
class CCvarTextEntry;
class CCvarToggleCheckButton;
class CLabeledCommandComboBox;

class CrosshairImagePanel : public vgui2::ImagePanel
{
	using BaseClass = vgui2::ImagePanel;

public:
	CrosshairImagePanel( vgui2::Panel* parent, const char* name, CCvarToggleCheckButton* pAdditive );

	void UpdateCrosshair( int r, int g, int b, int size );

	void Paint() override;

private:
	int m_R;
	int m_G;
	int m_B;
	int m_barSize;
	int m_barGap;
	CCvarToggleCheckButton* m_pAdditive;

private:
	CrosshairImagePanel( const CrosshairImagePanel& ) = delete;
	CrosshairImagePanel& operator=( const CrosshairImagePanel& ) = delete;
};


class COptionsSubMultiplayer : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( COptionsSubMultiplayer, vgui2::PropertyPage );

public:
	COptionsSubMultiplayer( vgui2::Panel* parent );

	MESSAGE_FUNC_PTR( OnTextChanged, "TextChanged", panel );
	MESSAGE_FUNC_PARAMS( OnSliderMoved, "SliderMoved", data );
	MESSAGE_FUNC( OnApplyButtonEnable, "ControlModified" );

	void OnResetData() override;

	void OnCommand( const char* command ) override;

	void OnApplyChanges() override;

	void InitModelList( CLabeledCommandComboBox* cb );

	void RedrawCrosshairImage();

	void InitCrosshairColorEntries();

	void InitCrosshairSizeList( CLabeledCommandComboBox* cb );

	void RemapPalette( char* filename, int topcolor, int bottomcolor );

	void RemapModel();

	void RemapLogoPalette( char* filename, int r, int g, int b );

	void RemapLogo();

	void ColorForName( const char* pszColorName, int& r, int& g, int& b );

	void ApplyCrosshairColorChanges();

	void InitLogoList( CLabeledCommandComboBox* cb );

private:
	CBitmapImagePanel* m_pModelImage;
	CLabeledCommandComboBox* m_pModelList;

	CBitmapImagePanel* m_pLogoImage;
	CLabeledCommandComboBox* m_pLogoList;
	char m_LogoName[ 32 ];

	CLabeledCommandComboBox* m_pCrosshairSize;
	CCvarToggleCheckButton* m_pCrosshairTranslucencyCheckbox;
	vgui2::ComboBox* m_pCrosshairColorComboBox;

	int m_displayedCrosshairColorRed;
	int m_displayedCrosshairColorGreen;
	int m_displayedCrosshairColorBlue;

	CLabeledCommandComboBox* m_pColorList;

	CCvarTextEntry* m_pNameTextEntry;

	CCvarSlider* m_pPrimaryColorSlider;
	CCvarSlider* m_pSecondaryColorSlider;

	CCvarToggleCheckButton* m_pHighQualityModelCheckBox;

	CrosshairImagePanel* m_pCrosshairImage;

	int m_nTopColor;
	int m_nBottomColor;

	int m_nLogoR;
	int m_nLogoG;
	int m_nLogoB;

	vgui2::DHANDLE<CMultiplayerAdvancedDialog> m_hMultiplayerAdvancedDialog;

private:
	COptionsSubMultiplayer( const COptionsSubMultiplayer& ) = delete;
	COptionsSubMultiplayer& operator=( const COptionsSubMultiplayer& ) = delete;
};


#endif //GAMEUI_OPTIONSSUBMULTIPLAYER_H
