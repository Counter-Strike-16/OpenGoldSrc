#include "gl_local.h"

void R_InitTextures(void)
{
}

void R_FreeTextures(void)
{
}

void R_NewMap(void)
{
	int i;

	r_worldentity = gEngfuncs.GetEntityByIndex(0);
	r_worldmodel = r_worldentity->model;

	r_viewleaf = NULL;

	GL_BuildLightmaps();

	skytexturenum = -1;
	mirrortexturenum = -1;
	nextupdatereflect = gEngfuncs.GetClientTime() + 0.1;

	for (i = 0; i < r_worldmodel->numtextures; i++)
	{
		if (!r_worldmodel->textures[i])
			continue;

		if (!strncmp(r_worldmodel->textures[i]->name, "sky", 3))
			skytexturenum = i;

		if (!strncmp(r_worldmodel->textures[i]->name, "window02_1", 10))
			mirrortexturenum = i;

 		r_worldmodel->textures[i]->texturechain = NULL;
	}

	gRefFuncs.R_NewMap();
}