// TriApi implementation

// shared between client and server			
triangleapi_t gTriApi =
{
	TRI_API_VERSION,	
	TriRenderMode,
	TriBegin,
	TriEnd,
	TriColor4f,
	TriColor4ub,
	TriTexCoord2f,
	TriVertex3fv,
	TriVertex3f,
	TriBrightness,
	TriCullFace,
	TriSpriteTexture,
	R_WorldToScreen,	// NOTE: XPROJECT, YPROJECT should be done in client.dll
	TriFog,
	R_ScreenToWorld,
	TriGetMatrix,
	TriBoxInPVS,
	TriLightAtPoint,
	TriColor4fRendermode,
	TriFogParams,
};

/*
=============
TriRenderMode

set rendermode
=============
*/
void TriRenderMode(int mode)
{
	switch(mode)
	{
	case kRenderNormal:
	default:	pglDisable(GL_BLEND);
		pglDisable(GL_ALPHA_TEST);
		pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	case kRenderTransColor:
	case kRenderTransAlpha:
	case kRenderTransTexture:
		// NOTE: TriAPI doesn't have 'solid' mode
		pglEnable(GL_BLEND);
		pglDisable(GL_ALPHA_TEST);
		pglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	case kRenderGlow:
	case kRenderTransAdd:
		pglEnable(GL_BLEND);
		pglDisable(GL_ALPHA_TEST);
		pglBlendFunc(GL_SRC_ALPHA, GL_ONE);
		pglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;
	}
}

/*
=============
TriBegin

begin triangle sequence
=============
*/
void TriBegin(int mode)
{
	switch(mode)
	{
	case TRI_POINTS:
		mode = GL_POINTS;
		break;
	case TRI_TRIANGLES:
		mode = GL_TRIANGLES;
		break;
	case TRI_TRIANGLE_FAN:
		mode = GL_TRIANGLE_FAN;
		break;
	case TRI_QUADS:
		mode = GL_QUADS;
		break;
	case TRI_LINES:
		mode = GL_LINES;
		break;
	case TRI_TRIANGLE_STRIP:
		mode = GL_TRIANGLE_STRIP;
		break;
	case TRI_QUAD_STRIP:
		mode = GL_QUAD_STRIP;
		break;
	case TRI_POLYGON:
	default:	mode = GL_POLYGON;
		break;
	}

	pglBegin(mode);
}

/*
=============
TriEnd

draw triangle sequence
=============
*/
void TriEnd()
{
	pglEnd();
	pglDisable(GL_ALPHA_TEST);
}

/*
=============
TriColor4f

=============
*/
void TriColor4f(float r, float g, float b, float a)
{
	clgame.ds.triColor[0] =(byte)bound(0,(r * 255.0f), 255);
	clgame.ds.triColor[1] =(byte)bound(0,(g * 255.0f), 255);
	clgame.ds.triColor[2] =(byte)bound(0,(b * 255.0f), 255);
	clgame.ds.triColor[3] =(byte)bound(0,(a * 255.0f), 255);
	pglColor4ub(clgame.ds.triColor[0], clgame.ds.triColor[1], clgame.ds.triColor[2], clgame.ds.triColor[3]);
}

/*
=============
TriColor4ub

=============
*/
void TriColor4ub(byte r, byte g, byte b, byte a)
{
	clgame.ds.triColor[0] = r;
	clgame.ds.triColor[1] = g;
	clgame.ds.triColor[2] = b;
	clgame.ds.triColor[3] = a;
	pglColor4ub(r, g, b, a);
}

/*
=============
TriTexCoord2f

=============
*/
void TriTexCoord2f(float u, float v)
{
	pglTexCoord2f(u, v);
}

/*
=============
TriVertex3fv

=============
*/
void TriVertex3fv(const float *v)
{
	pglVertex3fv(v);
}

/*
=============
TriVertex3f

=============
*/
void TriVertex3f(float x, float y, float z)
{
	pglVertex3f(x, y, z);
}

/*
=============
TriBrightness

=============
*/
void TriBrightness(float brightness)
{
	rgba_t	rgba;

	brightness = max(0.0f, brightness);
	rgba[0] = clgame.ds.triColor[0] * brightness;
	rgba[1] = clgame.ds.triColor[1] * brightness;
	rgba[2] = clgame.ds.triColor[2] * brightness;

	pglColor3ub(rgba[0], rgba[1], rgba[2]);
}

/*
=============
TriCullFace

=============
*/
void TriCullFace(TRICULLSTYLE mode)
{
	switch(mode)
	{
	case TRI_FRONT:
		clgame.ds.cullMode = GL_FRONT;
		break;
	default:
		clgame.ds.cullMode = GL_NONE;
		break;
	}
	GL_Cull(clgame.ds.cullMode);
}

/*
=============
TriSpriteTexture

bind current texture
=============
*/
int TriSpriteTexture(model_t *pSpriteModel, int frame)
{
	int	gl_texturenum;
	msprite_t	*psprite;

	if((gl_texturenum = R_GetSpriteTexture(pSpriteModel, frame)) == 0)
		return 0;

	if(gl_texturenum <= 0 || gl_texturenum > MAX_TEXTURES)
	{
		MsgDev(D_ERROR, "TriSpriteTexture: bad index %i\n", gl_texturenum);
		gl_texturenum = tr.defaultTexture;
	}

	psprite = pSpriteModel->cache.data;
	if(psprite->texFormat == SPR_ALPHTEST)
	{
		pglEnable(GL_ALPHA_TEST);
		pglAlphaFunc(GL_GREATER, 0.0f);
	}

	GL_Bind(GL_TEXTURE0, gl_texturenum);

	return 1;
}

/*
=============
TriWorldToScreen

convert world coordinates(x,y,z) into screen(x, y)
=============
*/
int TriWorldToScreen(float *world, float *screen)
{
	int	retval;

	retval = R_WorldToScreen(world, screen);

	screen[0] =  0.5f * screen[0] *(float)cl.refdef.viewport[2];
	screen[1] = -0.5f * screen[1] *(float)cl.refdef.viewport[3];
	screen[0] += 0.5f *(float)cl.refdef.viewport[2];
	screen[1] += 0.5f *(float)cl.refdef.viewport[3];

	return retval;
}

/*
=============
TriFog

enables global fog on the level
=============
*/
void TriFog(float flFogColor[3], float flStart, float flEnd, int bOn)
{
	if(RI.fogEnabled) return;
	RI.fogCustom = true;

	if(!bOn)
	{
		pglDisable(GL_FOG);
		RI.fogCustom = false;
		return;
	}

	// copy fog params
	RI.fogColor[0] = flFogColor[0] / 255.0f;
	RI.fogColor[1] = flFogColor[1] / 255.0f;
	RI.fogColor[2] = flFogColor[2] / 255.0f;
	RI.fogStart = flStart;
	RI.fogDensity = 0.0f;
	RI.fogEnd = flEnd;

	if(VectorIsNull(RI.fogColor))
	{
		pglDisable(GL_FOG);
		return;	
	}

	pglEnable(GL_FOG);
	pglFogi(GL_FOG_MODE, GL_LINEAR);
	pglFogf(GL_FOG_START, RI.fogStart);
	pglFogf(GL_FOG_END, RI.fogEnd);
	pglFogfv(GL_FOG_COLOR, RI.fogColor);
	pglHint(GL_FOG_HINT, GL_NICEST);
}

/*
=============
TriGetMatrix

very strange export
=============
*/
void TriGetMatrix(const int pname, float *matrix)
{
	pglGetFloatv(pname, matrix);
}

/*
=============
TriBoxInPVS

check box in pvs(absmin, absmax)
=============
*/
int TriBoxInPVS(float *mins, float *maxs)
{
	return Mod_BoxVisible(mins, maxs, Mod_GetCurrentVis());
}

/*
=============
TriLightAtPoint

NOTE: dlights are ignored
=============
*/
void TriLightAtPoint(float *pos, float *value)
{
	color24	ambient;

	if(!pos || !value)
		return;

	R_LightForPoint(pos, &ambient, false, false, 0.0f);

	value[0] =(float)ambient.r * 255.0f;
	value[1] =(float)ambient.g * 255.0f;
	value[2] =(float)ambient.b * 255.0f;
}

/*
=============
TriColor4fRendermode

Heavy legacy of Quake...
=============
*/
void TriColor4fRendermode(float r, float g, float b, float a, int rendermode)
{
	if(rendermode == kRenderTransAlpha)
		pglColor4f(r, g, b, a);
	else pglColor4f(r * a, g * a, b * a, 1.0f);
}

/*
=============
TriForParams

=============
*/
void TriFogParams(float flDensity, int iFogSkybox)
{
	// TODO: implement
}