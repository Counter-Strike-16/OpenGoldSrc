#include "precompiled.hpp"
#include "quakedef.hpp"
#include "vgui_EngineSurface.hpp"

EngineSurface::EngineSurface() : _renderPlat( nullptr )
{
	createRenderPlat();
};