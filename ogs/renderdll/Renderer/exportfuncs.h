int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion);
void HUD_Init(void);
int HUD_VidInit(void);
void HUD_Reset(void);
void V_CalcRefdef(struct ref_params_s *pparams);
void HUD_DrawTransparentTriangles(void);
int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio);
int HUD_UpdateClientData(client_data_t *pcldata, float flTime);
void HUD_Shutdown(void);