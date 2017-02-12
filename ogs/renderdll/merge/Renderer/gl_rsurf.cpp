#include "gl_local.h"

int skytexturenum;

msurface_t *skychain = NULL;
msurface_t *waterchain = NULL;

float current_lightgamma = -1.0;
float current_brightness = -1.0;
float current_gamma = -1.0;

qboolean lightmap_updateing;

void R_DrawWaterChain(msurface_t *pChain)
{
	msurface_t *s;

	if (!pChain)
		return;

	qglColor4ub(255, 255, 255, 255);

	for (s = pChain; s; s = s->texturechain)
	{
		GL_Bind(s->texinfo->texture->gl_texturenum);
		EmitWaterPolys(s, 0);
	}
}

void R_DrawWaterSurfaces(void)
{
	int i;
	msurface_t *s;
	texture_t *t;

	if (r_wateralpha->value == 1.0 && gl_texsort->value)
		return;

	if (r_wateralpha->value < 1.0)
	{
		qglEnable(GL_BLEND);
		qglColor4f(1, 1, 1, r_wateralpha->value);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	if (!gl_texsort->value)
	{
		if (waterchain)
		{
			for (s = waterchain; s; s = s->texturechain)
			{
				GL_Bind(s->texinfo->texture->gl_texturenum);
				EmitWaterPolys(s, 0);
			}

			waterchain = NULL;
		}
	}
	else
	{
		for (i = 0; i < r_worldmodel->numtextures; i++)
		{
			t = r_worldmodel->textures[i];

			if (!t)
				continue;

			s = t->texturechain;

			if (!s)
				continue;

			if (!(s->flags & SURF_DRAWTURB))
				continue;

			GL_Bind(t->gl_texturenum);

			for ( ; s; s = s->texturechain)
				EmitWaterPolys(s, 0);

			t->texturechain = NULL;
		}
	}

	if (r_wateralpha->value < 1.0)
	{
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		qglColor4f(1, 1, 1, 1);
		qglDisable(GL_BLEND);
	}
}

void R_RecursiveWorldNode(mnode_t *node)
{
}

void R_DrawWorld(void)
{
	gRefFuncs.R_DrawWorld();

	currententity = IEngineStudio.GetCurrentEntity();
}

void R_MarkLeaves(void)
{
}

void R_UploadLightmaps(void)
{
	if (current_lightgamma != v_lightgamma->value || current_brightness != v_brightness->value || current_gamma != v_gamma->value)
	{
		lightmap_updateing = true;
		GL_BuildLightmaps();
		lightmap_updateing = false;
	}
}

void BuildSurfaceDisplayList(msurface_t *fa)
{
	if (lightmap_updateing)
		return;

	gRefFuncs.BuildSurfaceDisplayList(fa);
}

void GL_BuildLightmaps(void)
{
	current_lightgamma = v_lightgamma->value;
	current_brightness = v_brightness->value;
	current_gamma = v_gamma->value;

	gRefFuncs.GL_BuildLightmaps();
}