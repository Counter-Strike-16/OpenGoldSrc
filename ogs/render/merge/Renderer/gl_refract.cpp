#include "gl_local.h"
#include "screen.h"

qboolean refract;
int refracttexture;

void R_SetupRefract(void)
{
	float screenaspect;
	float yfov;
	int i, j, k;
	extern int glwidth, glheight;
	int x, x2, y2, y, w, h;

	pglMatrixMode(GL_PROJECTION);
	pglPushMatrix();
	pglLoadIdentity();

	x = r_refdef->vrect.x;
	x2 = r_refdef->vrect.width + r_refdef->vrect.x;
	y = glheight - r_refdef->vrect.y;
	y2 = glheight - r_refdef->vrect.height - r_refdef->vrect.y;

	if (x > 0)
		x--;

	if (x2 < glwidth)
		x2++;

	if (y2 < 0)
		y2--;

	if (y < glheight)
		y++;

	w = x2 - x;
	h = y - y2;
	w = 500;
	h = 500;

	yfov = CalcFov(scr_fov_value, r_refdef->vrect.width, r_refdef->vrect.height);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(r_refdef->vrect.x + x, r_refdef->vrect.y + y2, w, h);
	screenaspect = (float)r_refdef->vrect.width / r_refdef->vrect.height;
	MYgluPerspective(yfov, screenaspect, 4, 4096);

	pglScalef(1, -1, 1);
	pglCullFace(GL_BACK);

	pglMatrixMode(GL_MODELVIEW);
	pglPushMatrix();
	pglLoadIdentity();

	pglRotatef(-90, 1, 0, 0);
	pglRotatef(90, 0, 0, 1);
	pglRotatef(-r_refdef->viewangles[2], 1, 0, 0);
	pglRotatef(-r_refdef->viewangles[0], 0, 1, 0);
	pglRotatef(-r_refdef->viewangles[1], 0, 0, 1);
	pglTranslatef(-r_refdef->vieworg[0], -r_refdef->vieworg[1], -r_refdef->vieworg[2]);

	if (gl_cull->value)
		pglEnable(GL_CULL_FACE);
	else
		pglDisable(GL_CULL_FACE);

	pglDisable(GL_BLEND);
	pglDisable(GL_ALPHA_TEST);
	pglEnable(GL_DEPTH_TEST);

	refract = true;
}

void R_FinishRefract(void)
{
	GL_Bind(refracttexture);
	pglEnable(GL_TEXTURE_2D);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 500, 500);

	R_Clear();

	pglMatrixMode(GL_PROJECTION);
	pglPopMatrix();
	pglMatrixMode(GL_MODELVIEW);
	pglPopMatrix();

	glPopAttrib();
}

void R_RenderRefract(void)
{
	R_DrawWorld();
}