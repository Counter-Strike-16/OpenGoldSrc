/*
GameUIDraw.h - game ui draw
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

#ifndef XASH_GAMEUIDRAW_H
#define XASH_GAMEUIDRAW_H

#include "System/SystemTypes.hpp"
#include "Graphics/GraphicsTypes.hpp"

typedef struct
{
	int		gl_texturenum;	// this is a real texnum

	// scissor test
	int		scissor_x;
	int		scissor_y;
	int		scissor_width;
	int		scissor_height;
	qboolean		scissor_test;

	// holds text color
	rgba_t		textColor;
} gameui_draw_t;

#endif // XASH_GAMEUIDRAW_H