#ifndef GAMEUI_IMAGEBUTTON_H
#define GAMEUI_IMAGEBUTTON_H

#include <SDK_Color.h>

#include <vgui_controls/Button.h>

class CImageButton : public vgui2::Button
{
public:
	using BaseClass = vgui2::Button;

	CImageButton( vgui2::Panel* parent, const char* panelName, const char* text );
	virtual ~CImageButton();

	virtual void SetTexture( const char* filename );

	virtual void ForceReload();

	virtual SDK_Color GetImageColor();

	virtual void ForceUpload();

	void PaintBackground() override;

	void SetImagePadding( int pad );

	void SetFillPadding( int pad );

private:
	bool m_bUploaded = false;
	int m_nTextureId = -1;
	char m_szTexture[ 128 ] = { '\0' };
	int m_padImage = 1;
	int m_padFill = 1;

private:
	CImageButton( const CImageButton& ) = delete;
	CImageButton& operator=( const CImageButton& ) = delete;
};


#endif //GAMEUI_IMAGEBUTTON_H
