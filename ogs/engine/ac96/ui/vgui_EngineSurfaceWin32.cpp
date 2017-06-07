#include "quakedef.h"
#include "vgui_EngineSurface.h"

EngineSurface::EngineSurface()
	: _renderPlat( nullptr )
{
	createRenderPlat();
}