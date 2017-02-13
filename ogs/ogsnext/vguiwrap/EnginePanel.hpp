/*
EnginePanel.hpp - engine base vgui panel
Copyright (C) 2015 XashXT Team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef XASH_ENGINEPANEL_HPP
#define XASH_ENGINEPANEL_HPP

#include<VGUI_App.h>
#include<VGUI_SurfaceBase.h>

using namespace vgui;

class CEnginePanel : public Panel
{
public:
	virtual SurfaceBase* getSurfaceBase();
	virtual App* getApp();
};

#endif // XASH_ENGINEPANEL_HPP