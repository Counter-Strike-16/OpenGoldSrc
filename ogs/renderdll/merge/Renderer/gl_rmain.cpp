#include "gl_local.h"
#include "screen.h"

ref_funcs_t gRefFuncs;

refdef_t *r_refdef;
ref_params_t *r_params;

cl_entity_t *r_worldentity;

vec3_t modelorg, r_entorigin;
cl_entity_t *currententity;

mleaf_t *r_viewleaf, *r_oldviewleaf;
texture_t *r_notexture_mip;

int mirrortexturenum;
qboolean mirror;
mplane_t *mirror_plane;

float yfov;
float screenaspect;
float nextupdatereflect;

vec_t *vup;
vec_t *vpn;
vec_t *vright;
vec_t *r_origin;

float r_world_matrix[16];
float r_projection_matrix[16];

qboolean gl_mtexable;
qboolean gl_program;
qboolean gl_framebuffer_object;

int vsync = -1;

struct FBO_Container_t
{
	GLuint s_hBackBufferFBO;
	GLuint s_hBackBufferCB;
	GLuint s_hBackBufferDB;
	GLuint s_hBackBufferTex;
};

FBO_Container_t s_MSAAFBO;
FBO_Container_t s_BackBufferFBO;

qboolean bDoMSAAFBO = false;
qboolean bDoScaledFBO = false;
qboolean bDoDirectBlit = false;

vec3_t last_reflect_vieworg;
vec3_t last_reflect_viewangles;

qboolean R_CullBox(vec3_t mins, vec3_t maxs)
{
	int i;

	if (reflect)
	{
		for (i = 0; i < 4; i++)
		{
			if (BoxOnPlaneSide(mins, maxs, &reflect_frustum[i]) == 2)
				return true;
		}

		return false;
	}

	return gRefFuncs.R_CullBox(mins, maxs);
}

void R_RotateForEntity(vec_t *origin, cl_entity_t *e)
{
}

void R_Clear(void)
{
	if (r_mirroralpha->value != 1.0)
	{
		if (gl_clear->value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear(GL_DEPTH_BUFFER_BIT);

		gldepthmin = 0;
		gldepthmax = 0.5;
		qglDepthFunc(GL_LEQUAL);
	}
	else if (gl_ztrick->value)
	{
		static int trickframe;

		if (gl_clear->value)
			qglClear(GL_COLOR_BUFFER_BIT);

		trickframe++;

		if (trickframe & 1)
		{
			gldepthmin = 0;
			gldepthmax = 0.49999;
			qglDepthFunc(GL_LEQUAL);
		}
		else
		{
			gldepthmin = 1;
			gldepthmax = 0.5;
			qglDepthFunc(GL_GEQUAL);
		}
	}
	else
	{
		if (gl_clear->value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			qglClear(GL_DEPTH_BUFFER_BIT);

		gldepthmin = 0;
		gldepthmax = 1;
		qglDepthFunc(GL_LEQUAL);
	}

	qglDepthRange(gldepthmin, gldepthmax);
}

void R_RenderView(void)
{
	gRefFuncs.R_RenderView();
}

void R_DrawViewModel(void)
{
}

void R_PreDrawViewModel(void)
{
}

void R_SetFrustum(void)
{
}

void R_ForceCVars(qboolean mp)
{
	if (gRefFuncs.R_ForceCVars)
	{
		gRefFuncs.R_ForceCVars(mp);
		return;
	}

	if (gl_polyoffset->value < 0 && gl_polyoffset->value < -0.001)
		Cvar_DirectSet(gl_polyoffset, "-0.001");
	if (gl_polyoffset->value > 0 && gl_polyoffset->value < 0.001)
		Cvar_DirectSet(gl_polyoffset, "0.001");
	else if (gl_polyoffset->value > 0 && gl_polyoffset->value > 25)
		Cvar_DirectSet(gl_polyoffset, "25");

	if (v_lightgamma->value < 1.8)
	{
		Cvar_DirectSet(v_lightgamma, "1.8");
		GL_BuildLightmaps();
	}
}

int SignbitsForPlane(mplane_t *out)
{
	int bits, j;

	bits = 0;

	for (j = 0; j < 3; j++)
	{
		if (out->normal[j] < 0)
			bits |= 1 << j;
	}

	return bits;
}

void R_SetupFrame(void)
{
	if (!reflect)
		R_ForceCVars(gEngfuncs.GetMaxClients() > 1);

	gRefFuncs.R_SetupFrame();
}

void MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	qglFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

float CalcFov(float fov_x, float width, float height)
{
	float a;
	float x;

	if (fov_x < 1 || fov_x > 179)
		x = 1;
	else
		x = tan(fov_x / 360 * M_PI);

	x = width / x;
	a = atanf(height / x);
	a = a * 360 / M_PI;
	return a;
}

void R_SetupGL(void)
{
	gRefFuncs.R_SetupGL();
}

void R_CalcRefdef(struct ref_params_s *pparams)
{
	r_params = pparams;
}

void R_RenderScene(void)
{
	yfov = CalcFov(scr_fov_value, r_refdef->vrect.width, r_refdef->vrect.height);
	screenaspect = (float)r_refdef->vrect.width / r_refdef->vrect.height;

	R_UploadLightmaps();
	Draw_UpdateAnsios();

	if (gl_reflectwater->value)
	{
		if (gEngfuncs.GetClientTime() > nextupdatereflect)
		{
			if (!VectorCompare(r_refdef->vieworg, last_reflect_vieworg) || !VectorCompare(r_refdef->viewangles, last_reflect_viewangles))
			{
				R_UpdateReflect();

				VectorCopy(r_refdef->vieworg, last_reflect_vieworg);
				VectorCopy(r_refdef->viewangles, last_reflect_viewangles);
			}

			nextupdatereflect = gEngfuncs.GetClientTime() + 0.01;
		}
	}
	else
	{
		R_ClearReflect();
	}

	gRefFuncs.R_RenderScene();
}

void CheckMultiTextureExtensions(void)
{
	if (gl_mtexable)
	{
		TEXTURE0_SGIS = GL_TEXTURE0;
		TEXTURE1_SGIS = GL_TEXTURE1;
		TEXTURE2_SGIS = GL_TEXTURE2;
		TEXTURE3_SGIS = GL_TEXTURE3;
	}
}

void FreeFBOObjects(void)
{
	if (s_MSAAFBO.s_hBackBufferCB)
		qglDeleteRenderbuffersEXT(1, &s_MSAAFBO.s_hBackBufferCB);

	s_MSAAFBO.s_hBackBufferCB = 0;

	if (s_MSAAFBO.s_hBackBufferDB)
		qglDeleteRenderbuffersEXT(1, &s_MSAAFBO.s_hBackBufferDB);

	s_MSAAFBO.s_hBackBufferDB = 0;

	if (s_MSAAFBO.s_hBackBufferTex)
		qglDeleteTextures(1, &s_MSAAFBO.s_hBackBufferTex);

	s_MSAAFBO.s_hBackBufferTex = 0;

	if (s_BackBufferFBO.s_hBackBufferFBO)
		qglDeleteFramebuffersEXT(1, &s_BackBufferFBO.s_hBackBufferFBO);

	s_BackBufferFBO.s_hBackBufferFBO = 0;

	if (s_BackBufferFBO.s_hBackBufferCB)
		qglDeleteRenderbuffersEXT(1, &s_BackBufferFBO.s_hBackBufferCB);

	s_BackBufferFBO.s_hBackBufferCB = 0;

	if (s_BackBufferFBO.s_hBackBufferDB)
		qglDeleteRenderbuffersEXT(1, &s_BackBufferFBO.s_hBackBufferDB);

	s_BackBufferFBO.s_hBackBufferDB = 0;

	if (s_BackBufferFBO.s_hBackBufferTex)
		qglDeleteTextures(1, &s_BackBufferFBO.s_hBackBufferTex);

	s_BackBufferFBO.s_hBackBufferTex = 0;
}

void GL_Init(void)
{
	QGL_Init();

	CheckMultiTextureExtensions();

#if 0
	if (gl_framebuffer_object && g_dwEngineBuildnum < 5971)
	{
		bDoMSAAFBO = true;
		bDoScaledFBO = true;

		if (gEngfuncs.CheckParm("-nomsaa", NULL))
			bDoMSAAFBO = false;

		if (gEngfuncs.CheckParm("-nofbo", NULL))
			bDoScaledFBO = false;

		if (gEngfuncs.CheckParm("-directblit", NULL))
			bDoDirectBlit = true;

		if (gEngfuncs.CheckParm("-nodirectblit", NULL))
			bDoDirectBlit = false;

		if (!qglGenFramebuffersEXT || !qglBindFramebufferEXT || !qglBlitFramebufferEXT)
			bDoScaledFBO = false;

		s_MSAAFBO.s_hBackBufferFBO = 0;
		s_MSAAFBO.s_hBackBufferCB = 0;
		s_MSAAFBO.s_hBackBufferDB = 0;
		s_MSAAFBO.s_hBackBufferTex = 0;
		s_BackBufferFBO.s_hBackBufferFBO = 0;
		s_BackBufferFBO.s_hBackBufferCB = 0;
		s_BackBufferFBO.s_hBackBufferDB = 0;
		s_BackBufferFBO.s_hBackBufferTex = 0;

		if (qglRenderbufferStorageMultisampleEXT && bDoMSAAFBO)
		{
			qglGenFramebuffersEXT(1, &s_MSAAFBO.s_hBackBufferFBO);
			qglBindFramebufferEXT(GL_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO);

			qglGenRenderbuffersEXT(1, &s_MSAAFBO.s_hBackBufferCB);
			qglBindRenderbufferEXT(GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferCB);
			qglRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, 4, GL_RGBA8_EXT, g_iVideoWidth, g_iVideoHeight);
			qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferCB);

			qglGenRenderbuffersEXT(1, &s_MSAAFBO.s_hBackBufferDB);
			qglBindRenderbufferEXT(GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferDB);
			qglRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, g_iVideoWidth, g_iVideoHeight);
			qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferDB);

			if (qglCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				if (s_MSAAFBO.s_hBackBufferCB)
					qglDeleteRenderbuffersEXT(1, &s_MSAAFBO.s_hBackBufferCB);

				s_MSAAFBO.s_hBackBufferCB = 0;

				if (s_MSAAFBO.s_hBackBufferDB)
					qglDeleteRenderbuffersEXT(1, &s_MSAAFBO.s_hBackBufferDB);

				s_MSAAFBO.s_hBackBufferDB = 0;

				if (s_MSAAFBO.s_hBackBufferTex)
					qglDeleteTextures(1, &s_MSAAFBO.s_hBackBufferTex);

				s_MSAAFBO.s_hBackBufferTex = 0;

				gEngfuncs.Con_Printf("Error initializing MSAA frame buffer\n");
				s_MSAAFBO.s_hBackBufferFBO = 0;
			}
		}
		else
		{
			gEngfuncs.Con_Printf("MSAA backbuffer rendering disabled.\n");
			s_MSAAFBO.s_hBackBufferFBO = 0;
		}

		if (bDoScaledFBO)
		{
			if (s_MSAAFBO.s_hBackBufferFBO)
				qglEnable(GL_MULTISAMPLE);

			qglGenFramebuffersEXT(1, &s_BackBufferFBO.s_hBackBufferFBO);
			qglBindFramebufferEXT(GL_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO);

			if (!s_MSAAFBO.s_hBackBufferFBO)
			{
				qglGenRenderbuffersEXT(1, &s_BackBufferFBO.s_hBackBufferDB);
				qglBindRenderbufferEXT(GL_RENDERBUFFER, s_BackBufferFBO.s_hBackBufferDB);
				qglRenderbufferStorageEXT(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, g_iVideoWidth, g_iVideoHeight);
				qglFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_BackBufferFBO.s_hBackBufferDB);
			}

			qglGenTextures(1, &s_BackBufferFBO.s_hBackBufferTex);
			qglBindTexture(GL_TEXTURE_2D, s_BackBufferFBO.s_hBackBufferTex);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			qglTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_iVideoWidth, g_iVideoHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			qglFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_BackBufferFBO.s_hBackBufferTex, 0);
			qglBindTexture(GL_TEXTURE_2D, 0);
			qglDisable(GL_TEXTURE_2D);
		}

		if (!bDoScaledFBO || qglCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			FreeFBOObjects();

			if (bDoScaledFBO)
				gEngfuncs.Con_Printf("FBO backbuffer rendering disabled due to create error.\n");
			else
				gEngfuncs.Con_Printf("FBO backbuffer rendering disabled.\n");
		}

		if (bDoScaledFBO)
			qglBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	}
#endif
}

void GL_Shutdown(void)
{
	FreeFBOObjects();
}

void GL_BeginRendering(int *x, int *y, int *width, int *height)
{
	if (gl_vsync->value != vsync)
	{
		if (qwglSwapIntervalEXT)
		{
			if (gl_vsync->value > 0)
				qwglSwapIntervalEXT(1);
			else
				qwglSwapIntervalEXT(0);
		}

		vsync = gl_vsync->value;
	}

	if (s_BackBufferFBO.s_hBackBufferFBO)
	{
		if (s_MSAAFBO.s_hBackBufferFBO)
			qglBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO);
		else
			qglBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO);

		qglClearColor(0.0, 0.0, 0.0, 1.0);

		if (gl_clear->value)
			qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	gRefFuncs.GL_BeginRendering(x, y, width, height);
}

void GL_EndRendering(void)
{
	if (s_BackBufferFBO.s_hBackBufferFBO)
	{
		if (s_MSAAFBO.s_hBackBufferFBO)
		{
			qglBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO);
			qglBindFramebufferEXT(GL_READ_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO);
			qglBlitFramebufferEXT(0, 0, g_iVideoWidth, g_iVideoHeight, 0, 0, g_iVideoWidth, g_iVideoHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}

		qglBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
		qglBindFramebufferEXT(GL_READ_FRAMEBUFFER_BINDING, s_BackBufferFBO.s_hBackBufferFBO);
		qglClearColor(0.0, 1.0, 0.0, 0.25);
		qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (bDoDirectBlit)
		{
			qglBlitFramebufferEXT(0, 0, g_iVideoWidth, g_iVideoHeight, 0, 0, g_iVideoWidth, g_iVideoWidth, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
		else
		{
			qglDisable(GL_BLEND);
			qglDisable(GL_LIGHTING);
			qglDisable(GL_DEPTH_TEST);
			qglDisable(GL_ALPHA_TEST);
			qglDisable(GL_CULL_FACE);

			qglMatrixMode(GL_PROJECTION);
			qglPushMatrix();
			qglLoadIdentity();

			qglOrtho(0, g_iVideoWidth, g_iVideoHeight, 0, -1.875, 1.875);

			qglMatrixMode(GL_MODELVIEW);
			qglPushMatrix();
			qglLoadIdentity();

			qglViewport(0, 0, g_iVideoWidth, g_iVideoHeight);

			qglTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			qglTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			qglTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			qglColor4f(1, 1, 1, 1);
			qglEnable(GL_TEXTURE_RECTANGLE);
			qglBindTexture(GL_TEXTURE_RECTANGLE, s_BackBufferFBO.s_hBackBufferTex);

			qglBegin(GL_QUADS);
			qglTexCoord2f(0, 0);
			qglVertex3f(0, 100, -1);
			qglTexCoord2f(0, g_iVideoHeight);
			qglVertex3f(0, 0, -1);
			qglTexCoord2f(g_iVideoWidth, g_iVideoHeight);
			qglVertex3f(100, 0, -1);
			qglTexCoord2f(g_iVideoWidth, 0);
			qglVertex3f(100, 100, -1);
			qglEnd();

			qglBindTexture(GL_TEXTURE_RECTANGLE, 0);

			qglMatrixMode(GL_PROJECTION);
			qglPopMatrix();
			qglMatrixMode(GL_MODELVIEW);
			qglPopMatrix();

			qglDisable(GL_TEXTURE_RECTANGLE);
		}

		qglBindFramebufferEXT(GL_READ_FRAMEBUFFER, 0);
	}

	gRefFuncs.GL_EndRendering();
}

void R_InitCvars(void)
{
	static cvar_t s_gl_texsort = { "gl_texsort", "0", 0, 0, 0 };

	r_bmodelinterp = gEngfuncs.pfnGetCvarPointer("r_bmodelinterp");
	r_bmodelhighfrac = gEngfuncs.pfnGetCvarPointer("r_bmodelhighfrac");
	r_norefresh = gEngfuncs.pfnGetCvarPointer("r_norefresh");
	r_drawentities = gEngfuncs.pfnGetCvarPointer("r_drawentities");
	r_drawviewmodel = gEngfuncs.pfnGetCvarPointer("r_drawviewmodel");
	r_speeds = gEngfuncs.pfnGetCvarPointer("r_speeds");
	r_fullbright = gEngfuncs.pfnGetCvarPointer("r_fullbright");
	r_decals = gEngfuncs.pfnGetCvarPointer("r_decals");
	r_lightmap = gEngfuncs.pfnGetCvarPointer("r_lightmap");
	r_shadows = gEngfuncs.pfnGetCvarPointer("r_shadows");
	r_mirroralpha = gEngfuncs.pfnGetCvarPointer("r_mirroralpha");
	r_wateralpha = gEngfuncs.pfnGetCvarPointer("r_wateralpha");
	r_dynamic = gEngfuncs.pfnGetCvarPointer("r_dynamic");
	r_novis = gEngfuncs.pfnGetCvarPointer("r_novis");
	r_mmx = gEngfuncs.pfnGetCvarPointer("r_mmx");
	r_traceglow = gEngfuncs.pfnGetCvarPointer("r_traceglow");
	r_wadtextures = gEngfuncs.pfnGetCvarPointer("r_wadtextures");
	r_glowshellfreq = gEngfuncs.pfnGetCvarPointer("r_glowshellfreq");

	gl_vsync = gEngfuncs.pfnGetCvarPointer("gl_vsync");

	if (!gl_vsync)
		gl_vsync = gEngfuncs.pfnRegisterVariable("gl_vsync", "1", FCVAR_ARCHIVE);

	gl_ztrick = gEngfuncs.pfnGetCvarPointer("gl_ztrick");

	if (!gl_ztrick)
		gl_ztrick = gEngfuncs.pfnGetCvarPointer("gl_ztrick_old");

	gl_finish = gEngfuncs.pfnGetCvarPointer("gl_finish");
	gl_clear = gEngfuncs.pfnGetCvarPointer("gl_clear");
	gl_cull = gEngfuncs.pfnGetCvarPointer("gl_cull");
	gl_texsort = gEngfuncs.pfnGetCvarPointer("gl_texsort");

	if (!gl_texsort)
		gl_texsort = &s_gl_texsort;

	gl_smoothmodels = gEngfuncs.pfnGetCvarPointer("gl_smoothmodels");
	gl_affinemodels = gEngfuncs.pfnGetCvarPointer("gl_affinemodels");
	gl_flashblend = gEngfuncs.pfnGetCvarPointer("gl_flashblend");
	gl_playermip = gEngfuncs.pfnGetCvarPointer("gl_playermip");
	gl_nocolors = gEngfuncs.pfnGetCvarPointer("gl_nocolors");
	gl_keeptjunctions = gEngfuncs.pfnGetCvarPointer("gl_keeptjunctions");
	gl_reporttjunctions = gEngfuncs.pfnGetCvarPointer("gl_reporttjunctions");
	gl_wateramp = gEngfuncs.pfnGetCvarPointer("gl_wateramp");
	gl_dither = gEngfuncs.pfnGetCvarPointer("gl_dither");
	gl_spriteblend = gEngfuncs.pfnGetCvarPointer("gl_spriteblend");
	gl_polyoffset = gEngfuncs.pfnGetCvarPointer("gl_polyoffset");
	gl_lightholes = gEngfuncs.pfnGetCvarPointer("gl_lightholes");
	gl_zmax = gEngfuncs.pfnGetCvarPointer("gl_zmax");
	gl_alphamin = gEngfuncs.pfnGetCvarPointer("gl_alphamin");
	gl_overdraw = gEngfuncs.pfnGetCvarPointer("gl_overdraw");
	gl_watersides = gEngfuncs.pfnGetCvarPointer("gl_watersides");
	gl_overbright = gEngfuncs.pfnGetCvarPointer("gl_overbright");
	gl_envmapsize = gEngfuncs.pfnGetCvarPointer("gl_envmapsize");
	gl_flipmatrix = gEngfuncs.pfnGetCvarPointer("gl_flipmatrix");
	gl_monolights = gEngfuncs.pfnGetCvarPointer("gl_monolights");
	gl_fog = gEngfuncs.pfnGetCvarPointer("gl_fog");
	gl_wireframe = gEngfuncs.pfnGetCvarPointer("gl_wireframe");
	gl_reflectwater = gEngfuncs.pfnRegisterVariable("gl_reflectwater", "0", FCVAR_ARCHIVE);
	gl_ansio = gEngfuncs.pfnGetCvarPointer("gl_ansio");

	if (!gl_ansio)
		gl_ansio = gEngfuncs.pfnRegisterVariable("gl_ansio", "0", FCVAR_ARCHIVE);

	v_lightgamma = gEngfuncs.pfnGetCvarPointer("lightgamma");
	v_brightness = gEngfuncs.pfnGetCvarPointer("brightness");
	v_gamma = gEngfuncs.pfnGetCvarPointer("gamma");
}

void R_InitScreen(void)
{
	r_refdef->vrect.width = g_iVideoWidth;
	r_refdef->vrect.height = g_iVideoHeight;
}

void R_Init(void)
{
	R_InitCvars();
	R_InitScreen();
	R_InitTextures();
	R_InitPrograms();
	R_InitReflect();

	Draw_Init();
}

void R_Shutdown(void)
{
	R_ShutdownReflect();
	R_FreeTextures();
	R_FreePrograms();
	R_UninstallHook();
}