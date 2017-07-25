#pragma once

struct IMouseControl
{
	/// @return Whether VGUI2 has mouse control
	virtual bool VGUI2MouseControl() = 0;

	/// Sets whether VGUI2 has mouse control
	virtual void SetVGUI2MouseControl(bool state) = 0;
};