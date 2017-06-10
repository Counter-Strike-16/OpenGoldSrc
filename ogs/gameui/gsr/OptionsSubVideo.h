#ifndef GAMEUI_OPTIONSSUBVIDEO_H
#define GAMEUI_OPTIONSSUBVIDEO_H

#include <vgui_controls/PropertyPage.h>

class CCvarSlider;
class CCvarToggleCheckButton;

class COptionsSubVideo : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE( COptionsSubVideo, vgui2::PropertyPage );

private:
	struct CVidSettings
	{
		int w;
		int h;
		int bpp;
		int windowed;
		int hdmodels;
		int addons_folder;
		int vid_level;
		char renderer[ 128 ];
	};

	enum
	{
		MAX_RENDERERS = 3,
		MAX_ASPECTS = 2,
		MAX_ITEM_NAME = 32,
	};

public:
	COptionsSubVideo( vgui2::Panel* parent );

	MESSAGE_FUNC( OnDataChanged, "ControlModified" );
	MESSAGE_FUNC_PARAMS( OnButtonChecked, "CheckButtonChecked", data );
	MESSAGE_FUNC_PTR_CHARPTR( OnTextChanged, "TextChanged", panel, text );

	void PrepareResolutionList();

	void SetCurrentRendererComboItem();

	void SetCurrentResolutionComboItem();

	void OnResetData() override;

	void GetVidSettings();

	void RevertVidSettings();

	void ApplyVidSettings( bool bForceRefresh );

	void OnApplyChanges() override;

private:
	CVidSettings m_OrigSettings;
	CVidSettings m_CurrentSettings;
	
	vgui2::ComboBox* m_pMode;
	vgui2::ComboBox* m_pRenderer;

	vgui2::CheckButton* m_pWindowed;
	vgui2::ComboBox* m_pAspectRatio;

	CCvarToggleCheckButton* m_pDetailTextures;
	CCvarToggleCheckButton* m_pVsync;

	vgui2::CheckButton* m_pHDModels;
	vgui2::CheckButton* m_pAddonsFolder;
	vgui2::CheckButton* m_pLowVideoDetail;

	CCvarSlider* m_pBrightnessSlider;
	CCvarSlider* m_pGammaSlider;

	char m_pszRenderNames[ MAX_RENDERERS ][ MAX_ITEM_NAME ];
	char m_pszAspectName[ MAX_ASPECTS ][ MAX_ITEM_NAME ];

	int m_iStartRenderer;
	int m_iStartResolution;
	bool m_bStartWidescreen;

private:
	COptionsSubVideo( const COptionsSubVideo& ) = delete;
	COptionsSubVideo& operator=( const COptionsSubVideo& ) = delete;
};

#endif //GAMEUI_OPTIONSSUBVIDEO_H
