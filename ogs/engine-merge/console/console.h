
extern int con_backscroll;
extern	qboolean con_forcedup;	// because no entities to refresh

void Con_DrawConsole (int lines, qboolean drawinput);

void Con_NotifyBox (char *text);	// during startup for sound / cd warnings
