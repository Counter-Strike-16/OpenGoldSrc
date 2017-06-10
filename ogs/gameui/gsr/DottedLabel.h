#ifndef GAMEUI_DOTTEDLABEL_H
#define GAMEUI_DOTTEDLABEL_H

#include <SDK_Color.h>

#include <vgui_controls/Label.h>

class CDottedBgLabel : public vgui2::Label
{
public:
	using BaseClass = vgui2::Label;

	CDottedBgLabel( vgui2::Panel* parent, const char* name, const char* text, int dash, int gap, int corner );

protected:
	void PaintBackground() override;

private:
	int m_dash;
	int m_gap;
	int m_corner;
	SDK_Color m_color;

private:
	CDottedBgLabel( const CDottedBgLabel& ) = delete;
	CDottedBgLabel& operator=( const CDottedBgLabel& ) = delete;
};


#endif //GAMEUI_DOTTEDLABEL_H
