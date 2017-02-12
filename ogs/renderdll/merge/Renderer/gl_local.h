#include <metahook.h>
#include <math.h>
#include <assert.h>
#include <mathlib.h>
#include <archtypes.h>
#include <const.h>
#include <custom.h>
#include <ref_params.h>
#include <cvardef.h>
#include <r_studioint.h>
#include "plugins.h"

typedef struct cache_user_s
{
	void *data;
}
cache_user_t;

#include "bspfile.h"
#include "ref_int.h"
#include "qgl.h"

#include "gl_const.h"
#include "gl_model.h"
#include "gl_util.h"

extern engine_studio_api_t IEngineStudio;

extern refdef_t *r_refdef;
extern ref_params_t *r_params;

extern cl_entity_t *r_worldentity;
extern model_t *r_worldmodel;

extern vec3_t modelorg, r_entorigin;
extern cl_entity_t *currententity;

extern mleaf_t *r_viewleaf, *r_oldviewleaf;
extern texture_t *r_notexture_mip;

extern int mirrortexturenum;
extern qboolean mirror;
extern mplane_t *mirror_plane;

extern float yfov;
extern float screenaspect;
extern float nextupdatereflect;

extern vec_t *vup;
extern vec_t *vpn;
extern vec_t *vright;
extern vec_t *r_origin;

extern float r_world_matrix[16];
extern float r_projection_matrix[16];

extern qboolean gl_mtexable;
extern qboolean gl_program;
extern qboolean gl_framebuffer_object;

extern qboolean bDoMSAAFBO;
extern qboolean bDoScaledFBO;
extern qboolean bDoDirectBlit;

extern int skytexturenum;

extern msurface_t *skychain;
extern msurface_t *waterchain;

extern int programs[MAX_PROGRAMS];
extern int numprograms;

extern qboolean reflect;
extern vec3_t reflect_vecs[MAX_REFLECTS];
extern float reflect_distances[MAX_REFLECTS];
extern int reflect_textures[MAX_REFLECTS];
extern mplane_t reflect_frustum[4];
extern int numreflects;

extern qboolean reflect_water_program;
extern int reflect_water_fp_program;
extern int reflect_water_vp_program;

extern int *gSkyTexNumber;

extern cvar_t *r_bmodelinterp;
extern cvar_t *r_bmodelhighfrac;
extern cvar_t *r_norefresh;
extern cvar_t *r_drawentities;
extern cvar_t *r_drawviewmodel;
extern cvar_t *r_speeds;
extern cvar_t *r_fullbright;
extern cvar_t *r_decals;
extern cvar_t *r_lightmap;
extern cvar_t *r_shadows;
extern cvar_t *r_mirroralpha;
extern cvar_t *r_wateralpha;
extern cvar_t *r_dynamic;
extern cvar_t *r_novis;
extern cvar_t *r_mmx;
extern cvar_t *r_traceglow;
extern cvar_t *r_wadtextures;
extern cvar_t *r_glowshellfreq;

extern cvar_t *gl_vsync;
extern cvar_t *gl_ztrick;
extern cvar_t *gl_finish;
extern cvar_t *gl_clear;
extern cvar_t *gl_cull;
extern cvar_t *gl_texsort;
extern cvar_t *gl_smoothmodels;
extern cvar_t *gl_affinemodels;
extern cvar_t *gl_flashblend;
extern cvar_t *gl_playermip;
extern cvar_t *gl_nocolors;
extern cvar_t *gl_keeptjunctions;
extern cvar_t *gl_reporttjunctions;
extern cvar_t *gl_wateramp;
extern cvar_t *gl_dither;
extern cvar_t *gl_spriteblend;
extern cvar_t *gl_polyoffset;
extern cvar_t *gl_lightholes;
extern cvar_t *gl_zmax;
extern cvar_t *gl_alphamin;
extern cvar_t *gl_overdraw;
extern cvar_t *gl_watersides;
extern cvar_t *gl_overbright;
extern cvar_t *gl_envmapsize;
extern cvar_t *gl_flipmatrix;
extern cvar_t *gl_monolights;
extern cvar_t *gl_fog;
extern cvar_t *gl_wireframe;
extern cvar_t *gl_reflectwater;
extern cvar_t *gl_shaderprogram;
extern cvar_t *gl_ansio;

extern cvar_t *v_lightgamma;
extern cvar_t *v_brightness;
extern cvar_t *v_gamma;

typedef enum
{
	GLT_SYSTEM,
	GLT_DECAL,
	GLT_HUDSPRITE,
	GLT_STUDIO,
	GLT_WORLD,
	GLT_SPRITE
}
GL_TEXTURETYPE;

#define TEX_TYPE_NONE 0
#define TEX_TYPE_ALPHA 1
#define TEX_TYPE_LUM 2
#define TEX_TYPE_ALPHA_GRADIENT 3
#define TEX_TYPE_RGBA 4

#define TEX_IS_ALPHA(type) ((type) == TEX_TYPE_ALPHA || (type) == TEX_TYPE_ALPHA_GRADIENT || (type) == TEX_TYPE_RGBA)

extern GLenum TEXTURE0_SGIS;
extern GLenum TEXTURE1_SGIS;
extern GLenum TEXTURE2_SGIS;
extern GLenum TEXTURE3_SGIS;

void R_InstallHook(void);
void R_UninstallHook(void);
void R_RenderView(void);
void R_RenderScene(void);
qboolean R_CullBox(vec3_t mins, vec3_t maxs);
void R_RotateForEntity(vec_t *origin, cl_entity_t *e);
void R_Clear(void);
void R_ForceCVars(qboolean mp);
void R_UploadLightmaps(void);
void R_NewMap(void);
void R_Init(void);
void R_Shutdown(void);
void R_InitTextures(void);
void R_FreeTextures(void);
void R_InitPrograms(void);
void R_FreePrograms(void);
void R_SetupFrame(void);
void R_SetupGL(void);
void R_CalcRefdef(struct ref_params_s *pparams);
void R_DrawWorld(void);
void R_DrawWaterSurfaces(void);
void R_DrawSkyChain(msurface_t *s);
void R_ClearSkyBox(void);
void R_DrawSkyBox(void);
void R_InitReflect(void);
void R_ShutdownReflect(void);
void R_AddReflect(vec3_t p);
void R_ClearReflect(void);
void R_SetupReflect(int i);
void R_FinishReflect(int i);
void R_UpdateReflect(void);
void R_LoadReflectMatrix(int i);
void R_ClearReflectMatrix(void);

void GL_Init(void);
void GL_BeginRendering(int *x, int *y, int *width, int *height);
void GL_EndRendering(void);
void GL_BuildLightmaps(void);
void GL_InitExtensions(void);
bool GL_Support(int r_ext);
void GL_SetDefaultState(void);
GLuint GL_GenTexture(void);
void GL_DeleteTexture(GLuint tex);
void GL_Bind(int texnum);
void GL_SelectTexture(GLenum target);
void GL_DisableMultitexture(void);
void GL_EnableMultitexture(void);
GLuint GL_CompileProgram(GLuint target, GLenum format, const GLvoid *string);
int GL_LoadTexture(char *identifier, GL_TEXTURETYPE textureType, int width, int height, byte *data, qboolean mipmap, int iType, byte *pPal);
int GL_LoadTexture2(char *identifier, GL_TEXTURETYPE textureType, int width, int height, byte *data, qboolean mipmap, int iType, byte *pPal, int filter);

void Draw_UpdateAnsios(void);
void Draw_Init(void);

void EmitWaterPolys(msurface_t *fa, int direction);
void MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
float CalcFov(float fov_x, float width, float height);
int SignbitsForPlane(mplane_t *out);
void BuildSurfaceDisplayList(msurface_t *fa);