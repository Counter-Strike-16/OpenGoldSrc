
void	SCR_CenterPrint (char *str);
void	SCR_BeginLoadingPlaque (void);
void	SCR_EndLoadingPlaque (void);

void	SCR_DebugGraph (float value, int color);

void	SCR_TouchPics (void);

void	SCR_RunConsole (void);

extern	cvar_t		*scr_viewsize;
extern	cvar_t		*crosshair;

extern	vrect_t		scr_vrect;		// position of render window

extern	char		crosshair_pic[MAX_QPATH];
extern	int			crosshair_width, crosshair_height;

void SCR_AddDirtyPoint (int x, int y);
void SCR_DirtyScreen (void);

//
// scr_cin.c
//
void SCR_PlayCinematic (char *name);
qboolean SCR_DrawCinematic (void);
void SCR_RunCinematic (void);
void SCR_StopCinematic (void);
void SCR_FinishCinematic (void);

