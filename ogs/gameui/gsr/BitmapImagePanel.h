#ifndef GAMEUI_CBITMAPIMAGEPANEL_H
#define GAMEUI_CBITMAPIMAGEPANEL_H

#include <vgui_controls/Panel.h>

/**
*	Simple bitmap panel.
*/
class CBitmapImagePanel : public vgui2::Panel
{
public:
	using BaseClass = vgui2::Panel;

	CBitmapImagePanel( vgui2::Panel* parent, const char* panelName, const char* filename );

	void PaintBackground() override;

	virtual void setTexture( const char* filename );

	void forceUpload();

	virtual void forceReload();

private:
	bool m_bUploaded = false;
	int m_nTextureId = -1;
	char m_szTexture[ 128 ] = { '\0' };

private:
	CBitmapImagePanel( const CBitmapImagePanel& ) = delete;
	CBitmapImagePanel& operator=( const CBitmapImagePanel& ) = delete;
};

#endif //GAMEUI_CBITMAPIMAGEPANEL_H
