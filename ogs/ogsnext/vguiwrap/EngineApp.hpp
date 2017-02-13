/*
EngineApp.hpp - engine vgui application
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

#ifndef XASH_ENGINEAPP_HPP
#define XASH_ENGINEAPP_HPP

#include<VGUI_App.h>

using namespace vgui;

// initialize VGUI::App as external (part of engine)
class CEngineApp : public App
{
public:
	CEngineApp( bool externalMain = true ) : App( externalMain ) { }
	virtual void main( int argc, char* argv[] ) { } // stub
	virtual void setCursorPos( int x, int y ); // we need to recompute abs position to window
	virtual void getCursorPos( int &x,int &y );
};

#endif // XASH_ENGINEAPP_HPP