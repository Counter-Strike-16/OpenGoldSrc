#include "gl_local.h"
#include "screen.h"

qboolean reflect;

float reflect_texture_width;
float reflect_texture_height;
float reflect_texture_minsize;

vec3_t reflect_vecs[MAX_REFLECTS];
float reflect_distances[MAX_REFLECTS];
int reflect_textures[MAX_REFLECTS];
mplane_t reflect_frustum[4];
byte *reflect_pixels;
int numreflects;

qboolean reflect_water_program;
int reflect_water_fp_program = 0;
int reflect_water_vp_program = 0;

char reflect_water_fp[] =
{
	"!!ARBfp1.0"

	"TEMP distortColor;"
	"TEMP distortColor2;"
	"TXP distortColor, fragment.texcoord[1], texture[1], 2D;"
	"TXP distortColor2, fragment.texcoord[2], texture[1], 2D;"
	"SUB distortColor, distortColor, distortColor2;"

	"TEMP recipW;"
	"TEMP reflectionUV;"
	"RCP recipW, fragment.texcoord[0].w;"
	"MAD reflectionUV, recipW, fragment.texcoord[0], 0.5;"

	"TEMP endColor;"
	"MAD reflectionUV, distortColor, recipW, reflectionUV;"
	"TEX endColor, reflectionUV, texture, 2D;"

	"TEMP normalColor;"
	"TEMP normalColor2;"

	"TXP normalColor, fragment.texcoord[1], texture[2], 2D;"
	"TXP normalColor2, fragment.texcoord[2], texture[2], 2D;"

	"ADD normalColor, normalColor, normalColor2;"
	"SUB normalColor, normalColor, 1.0;"

	"TEMP eyeVec;"
	"TEMP inverseLength;"
	"DP3 inverseLength, fragment.texcoord[3], fragment.texcoord[3];"
	"RSQ inverseLength, inverseLength.x;"
	"MUL eyeVec, fragment.texcoord[3], inverseLength;"

	"TEMP transparency;"
	"DP3 transparency.w, normalColor, eyeVec;"
	"SUB endColor.w, 0.5, transparency.w;"
	"MAX endColor.w, endColor.w, 0.35;"
	"MIN endColor.w, endColor.w, 0.45;"

	"MOV result.color, endColor;"

	"END"
};

char reflect_water_vp[] =
{
	"!!ARBvp1.0"

	"TEMP pos;"

	"DP4 pos.x, state.matrix.mvp.row[0], vertex.position;"
	"DP4 pos.y, state.matrix.mvp.row[1], vertex.position;"
	"DP4 pos.z, state.matrix.mvp.row[2], vertex.position;"
	"DP4 pos.w, state.matrix.mvp.row[3], vertex.position;"
	"MOV result.position, pos;"

	"PARAM scaleFlipReflection = { 0.5, -0.5, 1.0, 1.0 };"
	"MUL result.texcoord[0], pos, scaleFlipReflection;"

	"END"
};

void R_InitReflect(void)
{
	int power;
	int min_texture_size;
	int max_texture_size;

	for (power = 2; power < r_refdef->vrect.height; power *= 2)
		min_texture_size = power;

	qglGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

	if (min_texture_size > max_texture_size)
	{
		for (power = 2; power < max_texture_size; power *= 2)
			min_texture_size = power;
	}

	reflect_texture_width = (r_refdef->vrect.width < min_texture_size) ? r_refdef->vrect.width : min_texture_size;
	reflect_texture_height = (r_refdef->vrect.width < min_texture_size) ? r_refdef->vrect.width : min_texture_size;
	reflect_texture_minsize = min_texture_size;

	if (gl_program && gl_shaderprogram)
	{
		reflect_water_fp_program = GL_CompileProgram(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, reflect_water_fp);
		reflect_water_vp_program = GL_CompileProgram(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, reflect_water_vp);
	}

	reflect_water_program = (reflect_water_fp_program && reflect_water_vp_program);
	reflect_pixels = (byte *)malloc(reflect_texture_width * reflect_texture_height * 3);
}

void R_ShutdownReflect(void)
{
	free(reflect_pixels);
}

void R_AddReflect(vec3_t p)
{
	int i;
	vec3_t v;
	float distance;

	for (i = 0; i < numreflects; i++)
	{
		if (!reflect_textures[i])
		{
			reflect_textures[i] = GL_GenTexture();

			GL_Bind(reflect_textures[i]);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			qglTexImage2D(GL_TEXTURE_2D, 0, 3, reflect_texture_width, reflect_texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, reflect_pixels);
		}

		if (fabs(reflect_vecs[i][2] - p[2]) < 8.0f)
			return;
	}

	VectorSubtract(p, r_refdef->vieworg, v);
	distance = VectorLength(v);

	if (numreflects < MAX_REFLECTS)
	{
		VectorCopy(p, reflect_vecs[numreflects]);
		reflect_distances[numreflects] = distance;
		numreflects++;
	}
	else
	{
		for (i = 0; i < numreflects; i++)
		{
			if (distance < reflect_distances[i])
			{
				VectorCopy(p, reflect_vecs[i]);
				reflect_distances[i] = distance;
				break;
			}
		}
	}
}

void R_ClearReflect(void)
{
	numreflects = 0;
}

void R_SetReflectFrustum(void)
{
	int i;

	if (scr_fov_value == 90) 
	{
		VectorAdd(vpn, vright, reflect_frustum[0].normal);
		VectorSubtract(vpn, vright, reflect_frustum[1].normal);

		VectorAdd(vpn, vup, reflect_frustum[2].normal);
		VectorSubtract(vpn, vup, reflect_frustum[3].normal);
	}
	else
	{
		RotatePointAroundVector(reflect_frustum[0].normal, vup, vpn, -(90 - scr_fov_value / 2));
		RotatePointAroundVector(reflect_frustum[1].normal, vup, vpn, 90 - scr_fov_value / 2);
		RotatePointAroundVector(reflect_frustum[2].normal, vright, vpn, 90 - yfov / 2);
		RotatePointAroundVector(reflect_frustum[3].normal, vright, vpn, -(90 - yfov / 2));
	}

	for (i = 0; i < 4; i++)
	{
		reflect_frustum[i].type = PLANE_ANYZ;
		reflect_frustum[i].dist = DotProduct(r_origin, reflect_frustum[i].normal);
		reflect_frustum[i].signbits = SignbitsForPlane(&reflect_frustum[i]);
	}
}

float save_vieworg_z;

void R_SetupReflect(int i)
{
	int x, x2, y2, y, w, h;

	qglMatrixMode(GL_PROJECTION);
	qglLoadIdentity();

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
	w = reflect_texture_width;
	h = reflect_texture_width;

	qglViewport(r_refdef->vrect.x + x, r_refdef->vrect.y + y2, w, h);

	MYgluPerspective(yfov, screenaspect, 4, 4096);

	qglMatrixMode(GL_MODELVIEW);
	qglLoadIdentity();

	qglRotatef(-90, 1, 0, 0);
	qglRotatef(90, 0, 0, 1);
	qglRotatef(r_refdef->viewangles[2], 1, 0, 0);
	qglRotatef(r_refdef->viewangles[0], 0, 1, 0);
	qglRotatef(-r_refdef->viewangles[1], 0, 0, 1);
	qglTranslatef(-r_refdef->vieworg[0], -r_refdef->vieworg[1], -((2 * reflect_vecs[i][2]) - r_refdef->vieworg[2]));
	qglTranslatef(0, 0, -0);

	if (gl_cull->value)
		qglEnable(GL_CULL_FACE);
	else
		qglDisable(GL_CULL_FACE);

	double clipPlane[] = { 0.0, 0.0, 0.0, 0.0 };

	if (r_refdef->vieworg[2] < reflect_vecs[i][2])
	{
		clipPlane[2] = -1.0;
		clipPlane[3] = reflect_vecs[i][2];
	}
	else
	{
		clipPlane[2] = 1.0;
		clipPlane[3] = -reflect_vecs[i][2];
	}

	qglEnable(GL_CLIP_PLANE0);
	qglClipPlane(GL_CLIP_PLANE0, clipPlane);

	qglDisable(GL_BLEND);
	qglDisable(GL_ALPHA_TEST);
	qglEnable(GL_DEPTH_TEST);

	qglClearColor(0, 0, 0, 0);
	qglClear(GL_COLOR_BUFFER_BIT);

	save_vieworg_z = r_refdef->vieworg[2];
	r_refdef->vieworg[2] = (2 * reflect_vecs[i][2]) - r_refdef->vieworg[2];
	r_refdef->viewangles[0] *= -1.0f;
	r_refdef->onlyClientDraws = true;

	VectorCopy(r_refdef->vieworg, r_origin);
	AngleVectors(r_refdef->viewangles, vpn, vright, vup);

	R_SetReflectFrustum();
}

void R_FinishReflect(int i)
{
	GL_Bind(reflect_textures[i]);

	qglDisable(GL_CLIP_PLANE0);
	qglCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, reflect_texture_width, reflect_texture_height);

	r_refdef->vieworg[2] = save_vieworg_z;
	r_refdef->viewangles[0] *= -1.0f;
	r_refdef->onlyClientDraws = false;
}

void R_UpdateReflect(void)
{
	if (r_refdef->onlyClientDraws)
		return;

	reflect = true;

	for (int i = 0; i < numreflects; i++)
	{
		R_SetupReflect(i);
		R_ClearSkyBox();
		R_DrawWorld();
		R_FinishReflect(i);
		R_Clear();
	}

	GLenum error = qglGetError();

	if (error)
		assert(qglGetError() == GL_NO_ERROR);

	reflect = false;
}

void R_LoadReflectMatrix(int i)
{
	qglMatrixMode(GL_TEXTURE);
	qglLoadIdentity();

	qglTranslatef(0.5f, 0.5f, 0.0f);
	qglScalef(0.5f * (reflect_texture_width / reflect_texture_minsize), 0.5f * (reflect_texture_height / reflect_texture_minsize), 1.0f);

	MYgluPerspective(yfov, screenaspect, 4, 4096);

	qglRotatef(-90, 1, 0, 0);
	qglRotatef(90, 0, 0, 1);
	qglRotatef(r_refdef->viewangles[2], 1, 0, 0);
	qglRotatef(r_refdef->viewangles[0], 0, 1, 0);
	qglRotatef(-r_refdef->viewangles[1], 0, 0, 1);
	qglTranslatef(-r_refdef->vieworg[0], -r_refdef->vieworg[1], -((2 * reflect_vecs[i][2]) - r_refdef->vieworg[2]));
	qglTranslatef(0.0f, 0.0f, 0.0f);
	qglMatrixMode(GL_MODELVIEW);
}

void R_ClearReflectMatrix(void)
{
	qglMatrixMode(GL_TEXTURE);
	qglLoadIdentity();
	qglMatrixMode(GL_MODELVIEW);
}