#include "gl_local.h"
#include "cmd.h"

GLenum TEXTURE0_SGIS;
GLenum TEXTURE1_SGIS;
GLenum TEXTURE2_SGIS;
GLenum TEXTURE3_SGIS;

typedef struct
{
	int texnum;
	short servercount;
	short paletteIndex;
	int width;
	int height;
	qboolean mipmap;
	char identifier[64];
}
gltexture_t;

int gl_filter_min = GL_LINEAR_MIPMAP_LINEAR;
int gl_filter_max = GL_LINEAR;

qboolean gl_texture_needansio;
xcommand_t gl_texturemode_function;

float current_ansio = -1.0;

GLuint GL_GenTexture(void)
{
	GLuint tex;
	qglGenTextures(1, &tex);
	return tex;
}

void GL_DeleteTexture(GLuint tex)
{
	qglDeleteTextures(1, &tex);
}

void GL_Bind(int texnum)
{
	gRefFuncs.GL_Bind(texnum);

	if (gl_texture_needansio)
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_ansio->value);
}

void GL_SelectTexture(GLenum target)
{
	gRefFuncs.GL_SelectTexture(target);
}

void GL_DisableMultitexture(void)
{
	gRefFuncs.GL_DisableMultitexture();
}

void GL_EnableMultitexture(void)
{
	gRefFuncs.GL_EnableMultitexture();
}

int GL_LoadTexture2(char *identifier, GL_TEXTURETYPE textureType, int width, int height, byte *data, qboolean mipmap, int iType, byte *pPal, int filter)
{
	int result;

	if (!mipmap || textureType != GLT_WORLD)
		return gRefFuncs.GL_LoadTexture2(identifier, textureType, width, height, data, mipmap, iType, pPal, filter);

	Draw_UpdateAnsios();

	if (g_dwEngineBuildnum < 5971)
		gl_texture_needansio = true;

	result = gRefFuncs.GL_LoadTexture2(identifier, textureType, width, height, data, mipmap, iType, pPal, filter);
	gl_texture_needansio = false;

	return result;
}

typedef struct
{
	char *name;
	int minimize, maximize;
}
glmode_t;

glmode_t modes[] =
{
	{ "GL_NEAREST", GL_NEAREST, GL_NEAREST},
	{ "GL_LINEAR", GL_LINEAR, GL_LINEAR},
	{ "GL_NEAREST_MIPMAP_NEAREST", GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST },
	{ "GL_LINEAR_MIPMAP_NEAREST", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR },
	{ "GL_NEAREST_MIPMAP_LINEAR", GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST },
	{ "GL_LINEAR_MIPMAP_LINEAR", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }
};

void Draw_TextureMode_f(void)
{
	int i;

	if (gEngfuncs.Cmd_Argc() == 1)
	{
		for (i = 0; i < 6; i++)
		{
			if (gl_filter_min == modes[i].minimize)
			{
				gEngfuncs.Con_Printf("%s\n", modes[i].name);
				return;
			}
		}

		gEngfuncs.Con_Printf("current filter is unknown???\n");
		return;
	}

	for (i = 0; i < 6; i++)
	{
		if (!stricmp(modes[i].name, gEngfuncs.Cmd_Argv(1)))
			break;
	}

	if (i == 6)
	{
		gEngfuncs.Con_Printf("bad filter name\n");
		return;
	}

	gl_filter_min = modes[i].minimize;
	gl_filter_max = modes[i].maximize;

	gl_texture_needansio = true;
	gl_texturemode_function();
	gl_texture_needansio = false;
}

void Draw_UpdateAnsios(void)
{
	int i;

	if (gl_ansio->value != current_ansio)
	{
		for (i = 0; i < 6; i++)
		{
			if (gl_filter_min == modes[i].minimize)
			{
				char cmd[64];
				sprintf(cmd, "gl_texturemode %s\n", modes[i].name);
				gEngfuncs.pfnClientCmd(cmd);
				break;
			}
		}

		current_ansio = gl_ansio->value;
	}
}

void Draw_Init(void)
{
	gl_texturemode_function = Cmd_HookCmd("gl_texturemode", Draw_TextureMode_f);
}