extern int    r_pixbytes;
extern cvar_t gl_vsync;

void D_FlushCaches();
void R_SetStackBase();
void R_ForceCVars(qboolean multiplayer);

void StartLoadingProgressBar(const char *loadingType, int numProgressPoints);
void ContinueLoadingProgressBar(const char *loadingType, int progressPoint, float progressFraction);
void SetLoadingProgressBarStatusText(const char *statusText);