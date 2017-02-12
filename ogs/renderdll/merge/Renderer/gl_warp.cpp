#include "gl_local.h"

float turbsin[] =
{
	#include "gl_warp_sin.h"
};

#define TURBSCALE (256.0 / (2 * M_PI))

colorVec gWaterColor;
colorVec cshift_water;

void EmitWireFrameWaterPolys(msurface_t *fa, int direction)
{
	glpoly_t *p;
	float *v;
	int i;
	float s;
	float scale;
	float tempVert[3];

	if (gl_wireframe->value)
	{
		if (fa->polys->verts[0][2] >= r_refdef->vieworg[2])
			scale = currententity->curstate.scale;
		else
			scale = -currententity->curstate.scale;

		if (gl_wireframe->value == 2)
			qglDisable(GL_DEPTH_TEST);

		qglColor3f(1, 1, 1);

		for (p = fa->polys; p; p = p->next)
		{
			qglBegin(GL_LINE_LOOP);

			if (direction)
				v = p->verts[p->numverts - 1];
			else
				v = p->verts[0];

			if (direction)
			{
				for (i = 0; i < p->numverts; i++, v -= VERTEXSIZE)
				{
					s = turbsin[(int)((gEngfuncs.GetClientTime() * 160) + v[0] + v[1]) & 255];
					s += 8;
					s += turbsin[(int)((gEngfuncs.GetClientTime() * 171) + v[0] * 5 - v[1]) & 255];
					s *= 8;
					s *= scale * 0.8;

					tempVert[0] = v[0];
					tempVert[1] = v[1];
					tempVert[2] = v[2];
					tempVert[2] += s;

					qglVertex3fv(tempVert);
				}
			}
			else
			{
				for (i = 0; i < p->numverts; i++, v += VERTEXSIZE)
				{
					s = turbsin[(int)((gEngfuncs.GetClientTime() * 160) + v[0] + v[1]) & 255];
					s += 8;
					s += turbsin[(int)((gEngfuncs.GetClientTime() * 171) + v[0] * 5 - v[1]) & 255];
					s *= 8;
					s *= scale * 0.8;

					tempVert[0] = v[0];
					tempVert[1] = v[1];
					tempVert[2] = v[2];
					tempVert[2] += s;

					qglVertex3fv(tempVert);
				}
			}

			qglEnd();
		}

		if (gl_wireframe->value == 2)
			qglEnable(GL_DEPTH_TEST);
	}
}

void EmitReflectWaterPolys(msurface_t *fa, int direction, float z)
{
	glpoly_t *p;
	float *v;
	int i;
	float s, t, os, ot;
	float scale;
	float tempVert[3];
	float tempAngle[3];
	qboolean useProgram;
	float alphaRate;

	if (!gl_reflectwater->value)
		return;

	if (fa->polys->verts[0][2] >= r_refdef->vieworg[2])
		scale = currententity->curstate.scale;
	else
		scale = -currententity->curstate.scale;

	tempVert[0] = fa->polys->verts[0][0];
	tempVert[1] = fa->polys->verts[0][1];
	tempVert[2] = z;

	R_AddReflect(tempVert);

	for (i = 0; i < numreflects; i++)
	{
		if (fabs(reflect_vecs[i][2] - z) < 8.0f)
			break;
	}

	if (i != numreflects)
	{
		useProgram = reflect_water_program && gl_shaderprogram->value;
		alphaRate = (cshift_water.a / 255.0) * 0.7f;

		if (useProgram)
		{
			qglEnable(GL_VERTEX_PROGRAM_ARB);
			qglBindProgramARB(GL_VERTEX_PROGRAM_ARB, reflect_water_vp_program);
			qglEnable(GL_FRAGMENT_PROGRAM_ARB);
			qglBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, reflect_water_fp_program);
		}

		GL_Bind(reflect_textures[i]);

		if (useProgram)
		{
		}
		else
		{
			R_LoadReflectMatrix(i);
		}

		qglEnable(GL_BLEND);
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE);
		qglShadeModel(GL_SMOOTH);

		for (p = fa->polys; p; p = p->next)
		{
			qglBegin(GL_POLYGON);

			if (direction)
				v = p->verts[p->numverts - 1];
			else
				v = p->verts[0];

			for (i = 0; i < p->numverts; i++)
			{
				os = v[3];
				ot = v[4];

				s = turbsin[(int)((gEngfuncs.GetClientTime() * 160) + v[0] + v[1]) & 255];
				s += 8;
				s += turbsin[(int)((gEngfuncs.GetClientTime() * 171) + v[0] * 5 - v[1]) & 255];
				s *= 8;
				s *= scale * 0.8;

				tempVert[0] = v[0];
				tempVert[1] = v[1];
				tempVert[2] = v[2];
				tempVert[2] += s;

				s = turbsin[(int)(cos(ot * 0.125 + gEngfuncs.GetClientTime()) * TURBSCALE) & 255];
				s *= (1.0 / 8);

				t = turbsin[(int)(sin(os * 0.125 + gEngfuncs.GetClientTime()) * TURBSCALE) & 255];
				t *= (1.0 / 8);

				VectorSubtract(v, r_refdef->vieworg, tempAngle);
				VectorNormalize(tempAngle);

				qglColor4f(1.0f, 1.0f, 1.0f, (0.2f - alphaRate * (1.0f - tempAngle[2]) * (1.0f - tempAngle[2])));
				qglTexCoord3f(tempVert[0] + s, tempVert[1] + t, tempVert[2]);
				qglVertex3fv(tempVert);

				if (direction)
					v -= VERTEXSIZE;
				else
					v += VERTEXSIZE;
			}

			qglEnd();
		}

		qglDisable(GL_BLEND);

		if (useProgram)
		{
			qglDisable(GL_FRAGMENT_PROGRAM_ARB);
			qglDisable(GL_VERTEX_PROGRAM_ARB);
		}
		else
		{
			R_ClearReflectMatrix();
		}
	}
}

void EmitWaterPolys(msurface_t *fa, int direction)
{
	glpoly_t *p;
	float *v;
	int i;
	float s, t, os, ot;
	float scale;
	float tempVert[3];
	unsigned char *pSourcePalette;
	qboolean zValue = 0.0f;
	qboolean waterNotFlat = false;

	if (reflect)
		return;

	pSourcePalette = fa->texinfo->texture->pPal;
	gWaterColor.r = pSourcePalette[9];
	gWaterColor.g = pSourcePalette[10];
	gWaterColor.b = pSourcePalette[11];
	cshift_water.r = pSourcePalette[9];
	cshift_water.g = pSourcePalette[10];
	cshift_water.b = pSourcePalette[11];
	cshift_water.a = pSourcePalette[12];

	if (fa->polys->verts[0][2] >= r_refdef->vieworg[2])
		scale = currententity->curstate.scale;
	else
		scale = -currententity->curstate.scale;

	for (p = fa->polys; p; p = p->next)
	{
		qglBegin(GL_POLYGON);

		if (direction)
			v = p->verts[p->numverts - 1];
		else
			v = p->verts[0];

		for (i = 0; i < p->numverts; i++)
		{
			os = v[3];
			ot = v[4];

			s = turbsin[(int)((gEngfuncs.GetClientTime() * 160) + v[0] + v[1]) & 255];
			s += 8;
			s += turbsin[(int)((gEngfuncs.GetClientTime() * 171) + v[0] * 5 - v[1]) & 255];
			s *= 8;
			s *= scale * 0.8;

			tempVert[0] = v[0];
			tempVert[1] = v[1];
			tempVert[2] = v[2];
			tempVert[2] += s;

			s = os + turbsin[(int)((ot * 0.125 + gEngfuncs.GetClientTime()) * TURBSCALE) & 255];
			s *= (1.0 / 64);

			t = ot + turbsin[(int)((os * 0.125 + gEngfuncs.GetClientTime()) * TURBSCALE) & 255];
			t *= (1.0 / 64);

			if (zValue == 0.0f)
				zValue = tempVert[2];
			else if (fabs(zValue - tempVert[2]) > 0.1f)
				waterNotFlat = true;

			qglTexCoord2f(s, t);
			qglVertex3fv(tempVert);

			if (direction)
				v -= VERTEXSIZE;
			else
				v += VERTEXSIZE;
		}

		qglEnd();
	}

	if (!waterNotFlat)
		EmitReflectWaterPolys(fa, direction, zValue);

	EmitWireFrameWaterPolys(fa, direction);
}

#define SKY_TEX 5800

int *gSkyTexNumber;

vec3_t skyclip[6] =
{
	{ 1, 1, 0 },
	{ 1, -1, 0 },
	{ 0, -1, 1 },
	{ 0, 1, 1 },
	{ 1, 0, 1 },
	{ -1, 0, 1 } 
};

int c_sky;

int st_to_vec[6][3] =
{
	{ 3, -1, 2 },
	{ -3, 1, 2 },

	{ 1, 3, 2 },
	{ -1, -3, 2 },

	{ -2, -1, 3 },
	{ 2, -1, -3 }
};

int vec_to_st[6][3] =
{
	{ -2, 3, 1 },
	{ 2, 3, -1 },

	{ 1, 3, 2 },
	{ -1, 3, -2 },

	{ -2, -1, 3 },
	{ -2, 1, -3 }
};

float skymins[2][6], skymaxs[2][6];

void DrawSkyPolygon(int nump, vec3_t vecs)
{
	int i, j;
	vec3_t v, av;
	float s, t, dv;
	int axis;
	float *vp;

	c_sky++;

	VectorCopy(vec3_origin, v);

	for (i = 0, vp = vecs; i < nump; i++, vp += 3)
	{
		VectorAdd(vp, v, v);
	}

	av[0] = fabs(v[0]);
	av[1] = fabs(v[1]);
	av[2] = fabs(v[2]);

	if (av[0] > av[1] && av[0] > av[2])
	{
		if (v[0] < 0)
			axis = 1;
		else
			axis = 0;
	}
	else if (av[1] > av[2] && av[1] > av[0])
	{
		if (v[1] < 0)
			axis = 3;
		else
			axis = 2;
	}
	else
	{
		if (v[2] < 0)
			axis = 5;
		else
			axis = 4;
	}

	for (i = 0; i < nump; i++, vecs += 3)
	{
		j = vec_to_st[axis][2];

		if (j > 0)
			dv = vecs[j - 1];
		else
			dv = -vecs[-j - 1];

		j = vec_to_st[axis][0];

		if (j < 0)
			s = -vecs[-j -1] / dv;
		else
			s = vecs[j - 1] / dv;

		j = vec_to_st[axis][1];

		if (j < 0)
			t = -vecs[-j -1] / dv;
		else
			t = vecs[j - 1] / dv;

		if (s < skymins[0][axis])
			skymins[0][axis] = s;

		if (t < skymins[1][axis])
			skymins[1][axis] = t;

		if (s > skymaxs[0][axis])
			skymaxs[0][axis] = s;

		if (t > skymaxs[1][axis])
			skymaxs[1][axis] = t;
	}
}

#define MAX_CLIP_VERTS 64
#define ON_EPSILON 0.1

void ClipSkyPolygon(int nump, vec3_t vecs, int stage)
{
	float *norm;
	float *v;
	qboolean front, back;
	float d, e;
	float dists[MAX_CLIP_VERTS];
	int sides[MAX_CLIP_VERTS];
	vec3_t newv[2][MAX_CLIP_VERTS];
	int newc[2];
	int i, j;

	if (nump > MAX_CLIP_VERTS - 2)
	{
		gEngfuncs.Con_Printf("ClipSkyPolygon: MAX_CLIP_VERTS");
	}

	if (stage == 6)
	{
		DrawSkyPolygon(nump, vecs);
		return;
	}

	front = back = false;
	norm = skyclip[stage];

	for (i = 0, v = vecs; i < nump; i++, v += 3)
	{
		d = DotProduct(v, norm);

		if (d > ON_EPSILON)
		{
			front = true;
			sides[i] = SIDE_FRONT;
		}
		else if (d < ON_EPSILON)
		{
			back = true;
			sides[i] = SIDE_BACK;
		}
		else
			sides[i] = SIDE_ON;

		dists[i] = d;
	}

	if (!front || !back)
	{
		ClipSkyPolygon(nump, vecs, stage + 1);
		return;
	}

	sides[i] = sides[0];
	dists[i] = dists[0];
	VectorCopy(vecs, (vecs + (i * 3)));
	newc[0] = newc[1] = 0;

	for (i = 0, v = vecs; i < nump; i++, v += 3)
	{
		switch (sides[i])
		{
			case SIDE_FRONT:
			{
				VectorCopy(v, newv[0][newc[0]]);
				newc[0]++;
				break;
			}

			case SIDE_BACK:
			{
				VectorCopy(v, newv[1][newc[1]]);
				newc[1]++;
				break;
			}

			case SIDE_ON:
			{
				VectorCopy(v, newv[0][newc[0]]);
				newc[0]++;
				VectorCopy(v, newv[1][newc[1]]);
				newc[1]++;
				break;
			}
		}

		if (sides[i] == SIDE_ON || sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
			continue;

		d = dists[i] / (dists[i] - dists[i + 1]);

		for (j = 0; j < 3; j++)
		{
			e = v[j] + d*(v[j + 3] - v[j]);
			newv[0][newc[0]][j] = e;
			newv[1][newc[1]][j] = e;
		}

		newc[0]++;
		newc[1]++;
	}

	ClipSkyPolygon(newc[0], newv[0][0], stage + 1);
	ClipSkyPolygon(newc[1], newv[1][0], stage + 1);
}

void R_DrawSkyChain(msurface_t *s)
{
	msurface_t *fa;

	int i;
	vec3_t verts[MAX_CLIP_VERTS];
	glpoly_t *p;

	if (!reflect)
	{
		return gRefFuncs.R_DrawSkyChain(s);
	}

	c_sky = 0;

	for (fa = s; fa; fa = fa->texturechain)
	{
		for (p = fa->polys; p; p = p->next)
		{
			for (i = 0; i < p->numverts; i++)
			{
				VectorSubtract(p->verts[i], r_origin, verts[i]);
			}

			ClipSkyPolygon(p->numverts, verts[0], 0);
		}
	}

	R_DrawSkyBox();
}

void R_ClearSkyBox(void)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		skymins[0][i] = skymins[1][i] = 9999;
		skymaxs[0][i] = skymaxs[1][i] = -9999;
	}
}

void MakeSkyVec(float s, float t, int axis)
{
	vec3_t v, b;
	int j, k;

	b[0] = s * 2048;
	b[1] = t * 2048;
	b[2] = 2048;

	for (j = 0; j < 3; j++)
	{
		k = st_to_vec[axis][j];

		if (k < 0)
			v[j] = -b[-k - 1];
		else
			v[j] = b[k - 1];

		v[j] += r_origin[j];
	}

	s = (s + 1) * 0.5f;
	t = (t + 1) * 0.5f;

	if (s < 1.0 / 512)
		s = 1.0 / 512;
	else if (s > 511.0 / 512)
		s = 511.0 / 512;
	if (t < 1.0 / 512)
		t = 1.0 / 512;
	else if (t > 511.0 / 512)
		t = 511.0 / 512;

	t = 1.0f - t;

	qglTexCoord2f(s, t);
	qglVertex3fv(v);
}

int skytexorder[6] = { 0, 2, 1, 3, 4, 5 };

void R_DrawSkyBox(void)
{
	for (int i = 0; i < 6; i++)
	{
		if (skymins[0][i] >= skymaxs[0][i] || skymins[1][i] >= skymaxs[1][i])
			continue;

		if (g_dwEngineBuildnum > 5971)
			GL_Bind(gSkyTexNumber[skytexorder[i]]);
		else
			GL_Bind(SKY_TEX + skytexorder[i]);

		qglBegin(GL_QUADS);
		MakeSkyVec(skymins[0][i], skymins[1][i], i);
		MakeSkyVec(skymins[0][i], skymaxs[1][i], i);
		MakeSkyVec(skymaxs[0][i], skymaxs[1][i], i);
		MakeSkyVec(skymaxs[0][i], skymins[1][i], i);
		qglEnd();
	}
}