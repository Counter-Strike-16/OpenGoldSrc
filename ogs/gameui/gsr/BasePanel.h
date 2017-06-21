#ifndef GAMEUI_BASEPANEL_H
#define GAMEUI_BASEPANEL_H

#include <UtlVector.h>

#include <vgui_controls/Panel.h>


class CBasePanel : public vgui2::Panel
{
public:
	enum EBackgroundState
	{
		BACKGROUND_NONE = 0,
		BACKGROUND_BLACK,
		BACKGROUND_DESKTOPIMAGE,
		BACKGROUND_LOADING,
		BACKGROUND_LOADINGTRANSITION,
		BACKGROUND_CAREERLOAD
	};

	struct bimage_t
	{
		int imageID;

		int x;
		int y;

		int width;
		int height;

		bool scaled;
	};

public:
	using BaseClass = vgui2::Panel;

	CBasePanel();
	~CBasePanel() = default;

	vgui2::VPANEL IsWithinTraverse( int x, int y, bool traversePopups ) override;

	void OnChildAdded( vgui2::VPANEL child ) override;

	void PaintBackground() override;

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void SetBackgroundRenderState( EBackgroundState state );

private:
	void DrawBackgroundImage();

private:
	EBackgroundState m_eBackgroundState = BACKGROUND_NONE;

	CUtlVector<bimage_t> m_ImageID;

	int m_iBaseResX;
	int m_iBaseResY;
};

#endif //GAMEUI_BASEPANEL_H
