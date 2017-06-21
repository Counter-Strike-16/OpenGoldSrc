#ifndef ENGINE_VGUI2_IMOUSECONTROL_H
#define ENGINE_VGUI2_IMOUSECONTROL_H

class IMouseControl
{
public:
	/**
	*	@return Whether VGUI2 has mouse control
	*/
	virtual bool VGUI2MouseControl() = 0;

	/**
	*	Sets whether VGUI2 has mouse control
	*/
	virtual void SetVGUI2MouseControl( bool state ) = 0;
};

#endif //ENGINE_VGUI2_IMOUSECONTROL_H