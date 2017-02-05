#include <metahook.h>
#include "gl_local.h"
#include "screen.h"

cl_enginefunc_t gEngfuncs;
engine_studio_api_t IEngineStudio;

int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(gEngfuncs));

	return gExportfuncs.Initialize(pEnginefuncs, iVersion);
}

void HUD_Init(void)
{
	SCR_Init();
	GL_Init();
	R_Init();

	return gExportfuncs.HUD_Init();
}

int HUD_VidInit(void)
{
	SCR_VidInit();

	return gExportfuncs.HUD_VidInit();
}

void HUD_Reset(void)
{
	return gExportfuncs.HUD_Reset();
}

void V_CalcRefdef(struct ref_params_s *pparams)
{
	gExportfuncs.V_CalcRefdef(pparams);

	R_CalcRefdef(pparams);
}

void HUD_DrawTransparentTriangles(void)
{
	gExportfuncs.HUD_DrawTransparentTriangles();
}

int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio)
{
	if (!gExportfuncs.HUD_GetStudioModelInterface(version, ppinterface, pstudio))
		return 0;

	memcpy(&IEngineStudio, pstudio, sizeof(IEngineStudio));
	return 1;
}

int HUD_UpdateClientData(client_data_t *pcldata, float flTime)
{
	int r = gExportfuncs.HUD_UpdateClientData(pcldata, flTime);

	scr_fov_value = pcldata->fov;
	return r;
}

void HUD_Shutdown(void)
{
	R_Shutdown();

	return gExportfuncs.HUD_Shutdown();
}