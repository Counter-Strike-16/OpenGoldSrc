#include "gl_local.h"

#define R_CLEAR_SIG "\xD9\x05\x2A\x2A\x2A\x2A\xDC\x1D\x2A\x2A\x2A\x2A\xDF\xE0\xF6\xC4\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\xD8\x1D\x2A\x2A\x2A\x2A\xDF\xE0"
#define R_CLEAR_SIG2 "\x8B\x15\x2A\x2A\x2A\x2A\x33\xC0\x83\xFA\x01\x0F\x9F\xC0\x50\xE8\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\xDC\x1D\x2A\x2A\x2A\x2A\x83\xC4\x04\xDF\xE0"
#define R_CLEAR_SIG_NEW "\x8B\x15\x2A\x2A\x2A\x2A\x33\xC0\x83\xFA\x01\x0F\x9F\xC0\x50\xE8\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\xDC\x1D\x2A\x2A\x2A\x2A\x83\xC4\x04"
#define R_RENDERSCENE_SIG "\x51\xDB\x05\x2A\x2A\x2A\x2A\xD9\x5C\x24\x00\xD9\x05\x2A\x2A\x2A\x2A\xD8\x5C\x24\x00\xDF\xE0\xF6\xC4\x2A\x2A\x2A\xA1"
#define R_RENDERSCENE_SIG_NEW "\xE8\x2A\x2A\x2A\x2A\x85\xC0\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\xE8\x2A\x2A\x2A\x2A\xE8"
#define R_NEWMAP_SIG "\x55\x8B\xEC\x83\xEC\x0C\xC7\x45\xFC\x00\x00\x00\x00\x2A\x2A\x8B\x45\xFC\x83\xC0\x01\x89\x45\xFC"
#define R_NEWMAP_SIG_NEW "\x55\x8B\xEC\x83\xEC\x08\xC7\x45\xFC\x00\x00\x00\x00\x2A\x2A\x8B\x45\xFC\x83\xC0\x01\x89\x45\xFC\x81\x7D\xFC\x00\x01\x00\x00\x2A\x2A\x8B\x4D\xFC"
#define R_DRAWWORLD_SIG "\x81\xEC\xB8\x0B\x00\x00\x68\xB8\x0B\x00\x00\x8D\x44\x24\x04\x6A\x00\x50\xE8"
#define R_DRAWWORLD_SIG_NEW "\x55\x8B\xEC\x81\xEC\xB8\x0B\x00\x00\x68\xB8\x0B\x00\x00\x8D\x85\x48\xF4\xFF\xFF\x6A\x00\x50\xE8\x2A\x2A\x2A\x2A\x8B\x0D"
#define R_CULLBOX_SIG "\x53\x8B\x5C\x24\x08\x56\x57\x8B\x7C\x24\x14\xBE\x2A\x2A\x2A\x2A\x56\x57\x53\xE8"
#define R_CULLBOX_SIG_NEW "\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x57\x8B\x7D\x0C\xBE\x2A\x2A\x2A\x2A\x56\x57\x53"
#define R_SETUPFRAME_SIG "\xA1\x2A\x2A\x2A\x2A\x83\xEC\x18\x83\xF8\x01\x0F\x8E\x2A\x2A\x2A\x2A\xD9\x05\x2A\x2A\x2A\x2A\xD8\x1D\x2A\x2A\x2A\x2A\xDF\xE0\xF6\xC4\x2A\x2A\x2A\x68"
#define R_SETUPFRAME_SIG2 "\x8B\x0D\x2A\x2A\x2A\x2A\x83\xEC\x18\x33\xC0\x83\xF9\x01\x0F\x9F\xC0\x50\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xA1"
#define R_SETUPFRAME_SIG_NEW "\x55\x8B\xEC\x83\xEC\x18\x8B\x0D\x2A\x2A\x2A\x2A\x33\xC0\x83\xF9\x01\x0F\x9F\xC0\x50\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xA1"
#define R_DRAWSKYCHAIN_SIG "\x81\xEC\x00\x03\x00\x00\xE8\x2A\x2A\x2A\x2A\x85\xC0\x0F\x85\x2A\x2A\x2A\x2A\xA3\x2A\x2A\x2A\x2A\x8B\x84\x24\x04\x03\x00\x00\x57\x8B\xF8\x85\xC0"
#define R_DRAWSKYCHAIN_SIG_NEW "\x55\x8B\xEC\x81\xEC\x00\x03\x00\x00\xE8\x2A\x2A\x2A\x2A\x85\xC0\x0F\x85\x2A\x2A\x2A\x2A\xA3\x2A\x2A\x2A\x2A\x8B\x45\x08\x57\x8B\xF8\x85\xC0"
#define GL_BUILDLIGHTMAPS_SIG "\x53\x56\x57\x68\x00\x80\x00\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x83\xC4\x0C\xBB\x01\x00\x00\x00\x85\xC0\x89\x1D"
#define GL_BUILDLIGHTMAPS_SIG2 "\x51\x68\x00\x80\x00\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x83\xC4\x0C\xB9\x01\x00\x00\x00\x85\xC0\x89\x0D"
#define GL_BUILDLIGHTMAPS_SIG_NEW "\x55\x8B\xEC\x51\x53\x56\x57\x68\x00\x80\x00\x00\x6A\x00\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x83\xC4\x0C"
#define GL_BIND_SIG "\x8B\x44\x24\x04\x8B\x0D\x2A\x2A\x2A\x2A\x56\x8B\xF0\xC1\xFE\x10\x25\xFF\xFF\x00\x00\x4E\x3B\xC8"
#define GL_BIND_SIG_NEW "\x55\x8B\xEC\x8B\x45\x08\x8B\x0D\x2A\x2A\x2A\x2A\x56\x8B\xF0\xC1\xFE\x10\x25\xFF\xFF\x00\x00\x4E\x3B\xC8"
#define GL_SELECTTEXTURE_SIG "\xA1\x2A\x2A\x2A\x2A\x56\x85\xC0\x2A\x2A\x8B\x74\x24\x08\x56\xFF\x15\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x3B\xF0"
#define GL_SELECTTEXTURE_SIG_NEW "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x56\x85\xC0\x2A\x2A\x8B\x75\x08\x56\xFF\x15\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x3B\xF0"
#define GL_DISABLEMULTITEXTURE_SIG "\xA1\x2A\x2A\x2A\x2A\x85\xC0\x2A\x2A\x68\xE1\x0D\x00\x00\xFF\x15\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x50\xE8"
#define GL_DISABLEMULTITEXTURE_SIG_NEW "\xA1\x2A\x2A\x2A\x2A\x85\xC0\x2A\x2A\x68\xE1\x0D\x00\x00\xFF\x15\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x50\xE8"
#define GL_ENABLEMULTITEXTURE_SIG "\xA1\x2A\x2A\x2A\x2A\x85\xC0\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x50\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x68\xE1\x0D\x00\x00\xFF\x15"
#define GL_ENABLEMULTITEXTURE_SIG_NEW "\xA1\x2A\x2A\x2A\x2A\x85\xC0\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x50\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x68\xE1\x0D\x00\x00\xFF\x15"
#define GL_LOADTEXTURE_SIG "\xA1\x2A\x2A\x2A\x2A\x8B\x4C\x24\x20\x8B\x54\x24\x1C\x50\x8B\x44\x24\x1C\x51\x8B\x4C\x24\x1C\x52\x8B\x54\x24\x1C\x50\x8B\x44\x24\x1C"
#define GL_LOADTEXTURE_SIG_NEW "\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x8B\x4D\x24\x8B\x55\x20\x50\x8B\x45\x1C\x51\x8B\x4D\x18\x52\x8B\x55\x14\x50\x8B\x45\x10\x51"
#define GL_LOADTEXTURE2_SIG "\xB8\x0C\x40\x00\x00\xE8\x2A\x2A\x2A\x2A\x53\x8B\x9C\x24\x14\x40\x00\x00\x55\x56\x8A\x03\x33\xF6"
#define GL_LOADTEXTURE2_SIG2 "\xB8\x0C\x40\x00\x00\xE8\x2A\x2A\x2A\x2A\x8B\x84\x24\x10\x40\x00\x00\x53\x55\x33\xDB\x8A\x08\x56\x84\xC9\x57\x89\x5C\x24\x14"
#define GL_LOADTEXTURE2_SIG_NEW "\x55\x8B\xEC\xB8\x0C\x40\x00\x00\xE8\x2A\x2A\x2A\x2A\x8B\x45\x08\x53\x33\xDB\x56\x8A\x08\x57\x84\xC9\x89\x5D\xF4\x74\x2A\x33\xF6"
#define GL_BEGINRENDERING_SIG "\x8B\x44\x24\x08\x8B\x4C\x24\x04\x8B\x54\x24\x0C\xC7\x00\x00\x00\x00\x00\xA1\x2A\x2A\x2A\x2A\xC7\x01\x00\x00\x00\x00\x8B\x0D\x2A\x2A\x2A\x2A\x2B\xC1"
#define GL_ENDRENDERING_SIG "\xFF\x25\x2A\x2A\x2A\x2A\x90"
#define EMITWATERPOLYS_SIG "\x83\xEC\x1C\x33\xD2\x55\x56\x8B\x74\x24\x28\x57\x8B\x46\x2C\x8B\x48\x24\x8B\x41\x44\x33\xC9\x8A\x50\x0C\x8A\x48\x0B\x52"
#define EMITWATERPOLYS_SIG_NEW "\x55\x8B\xEC\x83\xEC\x20\x56\x8B\x75\x08\x33\xD2\x57\x8B\x46\x2C\x8B\x48\x24\x8B\x41\x44\x33\xC9\x8A\x50\x0C\x8A\x48\x0B\x52\x51"
#define BUILDSURFACEDISPLAYLIST_SIG "\x83\xEC\x48\x53\x55\x56\x8B\x74\x24\x58\xA1\x2A\x2A\x2A\x2A\x57\x8B\x6E\x10\x8B\x98\x9C\x00\x00\x00\x8D\x0C\xED\x00\x00\x00\x00"
#define BUILDSURFACEDISPLAYLIST_SIG2 "\x83\xEC\x48\x53\x55\x56\x8B\x74\x24\x58\xA1\x2A\x2A\x2A\x2A\x57\x8B\x7E\x10\x8B\xA8\x9C\x00\x00\x00\x8D\x0C\xFD\x00\x00\x00\x00"
#define BUILDSURFACEDISPLAYLIST_SIG_NEW "\x55\x8B\xEC\x83\xEC\x48\x53\x56\x8B\x75\x08\xA1\x2A\x2A\x2A\x2A\x57\x8B\x5E\x10\x8B\x88\x9C\x00\x00\x00\x89\x4D\xF0"

void R_InstallHook(void)
{
	if (g_dwEngineBuildnum >= 5971)
	{
		gRefFuncs.GL_Bind = (void (*)(int))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, GL_BIND_SIG_NEW, sizeof(GL_BIND_SIG_NEW) - 1);
		gRefFuncs.GL_SelectTexture = (void (*)(GLenum))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_Bind, g_dwEngineSize - ((DWORD)gRefFuncs.GL_Bind - g_dwEngineBase), GL_SELECTTEXTURE_SIG_NEW, sizeof(GL_SELECTTEXTURE_SIG_NEW) - 1);
		gRefFuncs.GL_LoadTexture2 = (int (*)(char *, int, int, int, byte *, qboolean, int, byte *, int))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_SelectTexture, g_dwEngineSize - ((DWORD)gRefFuncs.GL_SelectTexture - g_dwEngineBase), GL_LOADTEXTURE2_SIG_NEW, sizeof(GL_LOADTEXTURE2_SIG_NEW) - 1);
		gRefFuncs.R_CullBox = (qboolean (*)(vec3_t, vec3_t))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_LoadTexture2, g_dwEngineSize - ((DWORD)gRefFuncs.GL_LoadTexture2 - g_dwEngineBase), R_CULLBOX_SIG_NEW, sizeof(R_CULLBOX_SIG_NEW) - 1);
		gRefFuncs.R_SetupFrame = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_CullBox, g_dwEngineSize - ((DWORD)gRefFuncs.R_CullBox - g_dwEngineBase), R_SETUPFRAME_SIG_NEW, sizeof(R_SETUPFRAME_SIG_NEW) - 1);
		gRefFuncs.R_Clear = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_SetupFrame, g_dwEngineSize - ((DWORD)gRefFuncs.R_SetupFrame - g_dwEngineBase), R_CLEAR_SIG_NEW, sizeof(R_CLEAR_SIG_NEW) - 1);
		gRefFuncs.R_RenderScene = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_Clear, g_dwEngineSize - ((DWORD)gRefFuncs.R_Clear - g_dwEngineBase), R_RENDERSCENE_SIG_NEW, sizeof(R_RENDERSCENE_SIG_NEW) - 1);
		gRefFuncs.R_NewMap = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_RenderScene, g_dwEngineSize - ((DWORD)gRefFuncs.R_RenderScene - g_dwEngineBase), R_NEWMAP_SIG_NEW, sizeof(R_NEWMAP_SIG_NEW) - 1);
		gRefFuncs.GL_DisableMultitexture = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_NewMap, g_dwEngineSize - ((DWORD)gRefFuncs.R_NewMap - g_dwEngineBase), GL_DISABLEMULTITEXTURE_SIG_NEW, sizeof(GL_DISABLEMULTITEXTURE_SIG_NEW) - 1);
		gRefFuncs.GL_EnableMultitexture = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_DisableMultitexture, g_dwEngineSize - ((DWORD)gRefFuncs.GL_DisableMultitexture - g_dwEngineBase), GL_ENABLEMULTITEXTURE_SIG_NEW, sizeof(GL_ENABLEMULTITEXTURE_SIG_NEW) - 1);
		gRefFuncs.R_DrawWorld = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_EnableMultitexture, g_dwEngineSize - ((DWORD)gRefFuncs.GL_EnableMultitexture - g_dwEngineBase), R_DRAWWORLD_SIG_NEW, sizeof(R_DRAWWORLD_SIG_NEW) - 1);
		gRefFuncs.GL_BuildLightmaps = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_DrawWorld, g_dwEngineSize - ((DWORD)gRefFuncs.R_DrawWorld - g_dwEngineBase), GL_BUILDLIGHTMAPS_SIG_NEW, sizeof(GL_BUILDLIGHTMAPS_SIG_NEW) - 1);
		gRefFuncs.EmitWaterPolys = (void (*)(msurface_t *, int))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_BuildLightmaps, g_dwEngineSize - ((DWORD)gRefFuncs.GL_BuildLightmaps - g_dwEngineBase), EMITWATERPOLYS_SIG_NEW, sizeof(EMITWATERPOLYS_SIG_NEW) - 1);
		gRefFuncs.R_DrawSkyChain = (void (*)(msurface_t *))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.EmitWaterPolys, g_dwEngineSize - ((DWORD)gRefFuncs.EmitWaterPolys - g_dwEngineBase), R_DRAWSKYCHAIN_SIG_NEW, sizeof(R_DRAWSKYCHAIN_SIG_NEW) - 1);

		gSkyTexNumber = *(int **)g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.EmitWaterPolys, 0x1000, "\x2A\x2A\x2A\x2A\x51\xE8\x2A\x2A\x2A\x2A\x8B\x55\xF0\x83\xC4\x04\x52\x68\x01\x14\x00\x00\x68\x00\x19\x00\x00\x6A\x00\x68\x00\x01\x00\x00\x68\x00\x01\x00\x00\x68\xE5\x80\x00\x00\x6A\x00\x68\xE1\x0D\x00\x00", 51);
	}
	else
	{
		gRefFuncs.GL_Bind = (void (*)(int))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, GL_BIND_SIG, sizeof(GL_BIND_SIG) - 1);
		gRefFuncs.GL_SelectTexture = (void (*)(GLenum))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_Bind, g_dwEngineSize - ((DWORD)gRefFuncs.GL_Bind - g_dwEngineBase), GL_SELECTTEXTURE_SIG, sizeof(GL_SELECTTEXTURE_SIG) - 1);
		gRefFuncs.GL_LoadTexture2 = (int (*)(char *, int, int, int, byte *, qboolean, int, byte *, int))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_SelectTexture, g_dwEngineSize - ((DWORD)gRefFuncs.GL_SelectTexture - g_dwEngineBase), GL_LOADTEXTURE2_SIG, sizeof(GL_LOADTEXTURE2_SIG) - 1);

		if (!gRefFuncs.GL_LoadTexture2)
			gRefFuncs.GL_LoadTexture2 = (int(*)(char *, int, int, int, byte *, qboolean, int, byte *, int))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_SelectTexture, g_dwEngineSize - ((DWORD)gRefFuncs.GL_SelectTexture - g_dwEngineBase), GL_LOADTEXTURE2_SIG2, sizeof(GL_LOADTEXTURE2_SIG2) - 1);

		gRefFuncs.R_CullBox = (qboolean (*)(vec3_t, vec3_t))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_LoadTexture2, g_dwEngineSize - ((DWORD)gRefFuncs.GL_LoadTexture2 - g_dwEngineBase), R_CULLBOX_SIG, sizeof(R_CULLBOX_SIG) - 1);
		gRefFuncs.R_SetupFrame = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_CullBox, g_dwEngineSize - ((DWORD)gRefFuncs.R_CullBox - g_dwEngineBase), R_SETUPFRAME_SIG, sizeof(R_SETUPFRAME_SIG) - 1);

		if (!gRefFuncs.R_SetupFrame)
			gRefFuncs.R_SetupFrame = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_CullBox, g_dwEngineSize - ((DWORD)gRefFuncs.R_CullBox - g_dwEngineBase), R_SETUPFRAME_SIG2, sizeof(R_SETUPFRAME_SIG2) - 1);

		gRefFuncs.R_Clear = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_SetupFrame, g_dwEngineSize - ((DWORD)gRefFuncs.R_SetupFrame - g_dwEngineBase), R_CLEAR_SIG, sizeof(R_CLEAR_SIG) - 1);

		if (!gRefFuncs.R_Clear)
			gRefFuncs.R_Clear = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_SetupFrame, g_dwEngineSize - ((DWORD)gRefFuncs.R_SetupFrame - g_dwEngineBase), R_CLEAR_SIG2, sizeof(R_CLEAR_SIG2) - 1);

		gRefFuncs.R_RenderScene = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_Clear, g_dwEngineSize - ((DWORD)gRefFuncs.R_Clear - g_dwEngineBase), R_RENDERSCENE_SIG, sizeof(R_RENDERSCENE_SIG) - 1);
		gRefFuncs.R_NewMap = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_RenderScene, g_dwEngineSize - ((DWORD)gRefFuncs.R_RenderScene - g_dwEngineBase), R_NEWMAP_SIG, sizeof(R_NEWMAP_SIG) - 1);
		gRefFuncs.GL_DisableMultitexture = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_NewMap, g_dwEngineSize - ((DWORD)gRefFuncs.R_NewMap - g_dwEngineBase), GL_DISABLEMULTITEXTURE_SIG, sizeof(GL_DISABLEMULTITEXTURE_SIG) - 1);
		gRefFuncs.GL_EnableMultitexture = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_DisableMultitexture, g_dwEngineSize - ((DWORD)gRefFuncs.GL_DisableMultitexture - g_dwEngineBase), GL_ENABLEMULTITEXTURE_SIG, sizeof(GL_ENABLEMULTITEXTURE_SIG) - 1);
		gRefFuncs.R_DrawWorld = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_EnableMultitexture, g_dwEngineSize - ((DWORD)gRefFuncs.GL_EnableMultitexture - g_dwEngineBase), R_DRAWWORLD_SIG, sizeof(R_DRAWWORLD_SIG) - 1);
		gRefFuncs.BuildSurfaceDisplayList = (void (*)(msurface_t *))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_DrawWorld, g_dwEngineSize - ((DWORD)gRefFuncs.R_DrawWorld - g_dwEngineBase), BUILDSURFACEDISPLAYLIST_SIG, sizeof(BUILDSURFACEDISPLAYLIST_SIG) - 1);

		if (!gRefFuncs.BuildSurfaceDisplayList)
			gRefFuncs.BuildSurfaceDisplayList = (void(*)(msurface_t *))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_DrawWorld, g_dwEngineSize - ((DWORD)gRefFuncs.R_DrawWorld - g_dwEngineBase), BUILDSURFACEDISPLAYLIST_SIG2, sizeof(BUILDSURFACEDISPLAYLIST_SIG2) - 1);

		gRefFuncs.GL_BuildLightmaps = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.BuildSurfaceDisplayList, g_dwEngineSize - ((DWORD)gRefFuncs.BuildSurfaceDisplayList - g_dwEngineBase), GL_BUILDLIGHTMAPS_SIG, sizeof(GL_BUILDLIGHTMAPS_SIG) - 1);

		if (!gRefFuncs.GL_BuildLightmaps)
			gRefFuncs.GL_BuildLightmaps = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_DrawWorld, g_dwEngineSize - ((DWORD)gRefFuncs.R_DrawWorld - g_dwEngineBase), GL_BUILDLIGHTMAPS_SIG2, sizeof(GL_BUILDLIGHTMAPS_SIG2) - 1);;

		gRefFuncs.GL_BeginRendering = (void (*)(int *, int *, int *, int *))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_BuildLightmaps, g_dwEngineSize - ((DWORD)gRefFuncs.GL_BuildLightmaps - g_dwEngineBase), GL_BEGINRENDERING_SIG, sizeof(GL_BEGINRENDERING_SIG) - 1);
		gRefFuncs.GL_EndRendering = (void (*)(void))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_BeginRendering, g_dwEngineSize - ((DWORD)gRefFuncs.GL_BeginRendering - g_dwEngineBase), GL_ENDRENDERING_SIG, sizeof(GL_ENDRENDERING_SIG) - 1);
		gRefFuncs.EmitWaterPolys = (void (*)(msurface_t *, int))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_EndRendering, g_dwEngineSize - ((DWORD)gRefFuncs.GL_EndRendering - g_dwEngineBase), EMITWATERPOLYS_SIG, sizeof(EMITWATERPOLYS_SIG) - 1);
		gRefFuncs.R_DrawSkyChain = (void (*)(msurface_t *))g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.GL_EndRendering, g_dwEngineSize - ((DWORD)gRefFuncs.GL_EndRendering - g_dwEngineBase), R_DRAWSKYCHAIN_SIG, sizeof(R_DRAWSKYCHAIN_SIG) - 1);
	}

	if (gRefFuncs.R_SetupFrame)
	{
		DWORD addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gRefFuncs.R_SetupFrame, 0x300, "\x40\x68\x2A\x2A\x2A\x2A\xA3\x2A\x2A\x2A\x2A\xA1", 12);

		if (addr)
		{
			addr += 0x2;
			vright = (vec_t *)(*(DWORD *)addr);
			addr += 0xA;
			addr += 0x5;
			vpn = (vec_t *)(*(DWORD *)addr);
			addr += 0x5;
			vup = (vec_t *)(*(DWORD *)addr);
			addr += 0x5;
			r_refdef = (refdef_t *)(*(DWORD *)addr - 0x7C);
			addr += 0x5;

			if (*(BYTE *)addr == 0xD)
				addr += 0x1;

			r_origin = (vec_t *)(*(DWORD *)addr);
		}
	}

	if (g_dwEngineBuildnum < 5971)
	{
		g_pMetaHookAPI->InlineHook(gRefFuncs.GL_BeginRendering, GL_BeginRendering, (void *&)gRefFuncs.GL_BeginRendering);
		g_pMetaHookAPI->InlineHook(gRefFuncs.GL_EndRendering, GL_EndRendering, (void *&)gRefFuncs.GL_EndRendering);
	}

	g_pMetaHookAPI->InlineHook(gRefFuncs.R_RenderScene, R_RenderScene, (void *&)gRefFuncs.R_RenderScene);
	g_pMetaHookAPI->InlineHook(gRefFuncs.R_NewMap, R_NewMap, (void *&)gRefFuncs.R_NewMap);
	g_pMetaHookAPI->InlineHook(gRefFuncs.R_DrawWorld, R_DrawWorld, (void *&)gRefFuncs.R_DrawWorld);
	g_pMetaHookAPI->InlineHook(gRefFuncs.R_SetupFrame, R_SetupFrame, (void *&)gRefFuncs.R_SetupFrame);
	g_pMetaHookAPI->InlineHook(gRefFuncs.R_DrawSkyChain, R_DrawSkyChain, (void *&)gRefFuncs.R_DrawSkyChain);
	g_pMetaHookAPI->InlineHook(gRefFuncs.R_CullBox, R_CullBox, (void *&)gRefFuncs.R_CullBox);
	g_pMetaHookAPI->InlineHook(gRefFuncs.GL_Bind, GL_Bind, (void *&)gRefFuncs.GL_Bind);
	g_pMetaHookAPI->InlineHook(gRefFuncs.GL_BuildLightmaps, GL_BuildLightmaps, (void *&)gRefFuncs.GL_BuildLightmaps);
	g_pMetaHookAPI->InlineHook(gRefFuncs.GL_LoadTexture2, GL_LoadTexture2, (void *&)gRefFuncs.GL_LoadTexture2);
	g_pMetaHookAPI->InlineHook(gRefFuncs.EmitWaterPolys, EmitWaterPolys, (void *&)gRefFuncs.EmitWaterPolys);
	g_pMetaHookAPI->InlineHook(gRefFuncs.BuildSurfaceDisplayList, BuildSurfaceDisplayList, (void *&)gRefFuncs.BuildSurfaceDisplayList);
}

void R_UninstallHook(void)
{
}