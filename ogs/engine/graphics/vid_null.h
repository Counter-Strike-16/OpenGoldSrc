





extern int r_pixbytes;
extern cvar_t gl_vsync;
extern float scr_con_current;

void VID_WriteBuffer(const char *pFilename);
NOBODY int VID_Init(unsigned short *palette);

void D_FlushCaches();
void R_SetStackBase();
void SCR_UpdateScreen();
void V_Init();
void Draw_Init();
void SCR_Init();
void R_Init();
void R_ForceCVars(qboolean multiplayer);

void SCR_BeginLoadingPlaque(qboolean reconnect);
void SCR_EndLoadingPlaque();

void R_InitSky();
void R_MarkLeaves();
void R_InitTextures();

void StartLoadingProgressBar(const char *loadingType, int numProgressPoints);
void ContinueLoadingProgressBar(const char *loadingType, int progressPoint, float progressFraction);
void SetLoadingProgressBarStatusText(const char *statusText);
