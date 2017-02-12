#include <gl/gl.h>

typedef struct vrect_s
{
	int x, y, width, height;
	struct vrect_s *pnext;
}
vrect_t;

typedef struct refdef_s
{
	vrect_t vrect;
	vrect_t aliasvrect;
	int vrectright;
	int vrectbottom;
	int aliasvrectright;
	int aliasvrectbottom;
	float vrectrightedge;
	float fvrectx;
	float fvrecty;
	float fvrectx_adj;
	float fvrecty_adj;
	int vrect_x_adj_shift20;
	int vrectright_adj_shift20;
	float fvrectright_adj;
	float fvrectbottom_adj;
	float fvrectright;
	float fvrectbottom;
	float horizontalFieldOfView;
	float xOrigin;
	float yOrigin;
	vec3_t vieworg;
	vec3_t viewangles;
	color24 ambientlight;
	qboolean onlyClientDraws;
}
refdef_t;

typedef struct msurface_s msurface_t;

typedef struct
{
	void (*R_Clear)(void);
	void (*R_ForceCVars)(qboolean mp);
	void (*R_RenderView)(void);
	void (*R_RenderScene)(void);
	void (*R_NewMap)(void);
	void (*R_DrawEntitiesOnList)(void);
	void (*R_DrawWorld)(void);
	void (*R_SetupFrame)(void);
	void (*R_SetupGL)(void);
	void (*R_DrawSkyChain)(msurface_t *s);
	void (*R_DrawSkyBox)(void);
	qboolean (*R_CullBox)(vec3_t mins, vec3_t maxs);
	void (*GL_BuildLightmaps)(void);
	void (*GL_Bind)(int texnum);
	void (*GL_SelectTexture)(GLenum target);
	void (*GL_DisableMultitexture)(void);
	void (*GL_EnableMultitexture)(void);
	void (*GL_StudioDrawShadow)(void);
	void (*GL_BeginRendering)(int *x, int *y, int *width, int *height);
	void (*GL_EndRendering)(void);
	int (*GL_LoadTexture)(char *identifier, int textureType, int width, int height, byte *data, qboolean mipmap, int iType, byte *pPal);
	int (*GL_LoadTexture2)(char *identifier, int textureType, int width, int height, byte *data, qboolean mipmap, int iType, byte *pPal, int filter);
	void (*EmitWaterPolys)(msurface_t *fa, int direction);
	void (*BuildSurfaceDisplayList)(msurface_t *fa);
}
ref_funcs_t;

extern ref_funcs_t gRefFuncs;