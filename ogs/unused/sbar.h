

// the status bar is only redrawn if something has changed, but if anything
// does, the entire thing will be redrawn for the next vid.numpages frames.

#define	SBAR_HEIGHT		24

extern	int			sb_lines;			// scan lines to draw

void Sbar_Init (void);

void Sbar_Changed (void);
// call whenever any of the client stats represented on the sbar changes

void Sbar_Draw (void);
// called every frame by screen

void Sbar_IntermissionOverlay (void);
// called each frame after the level has been completed

void Sbar_FinaleOverlay (void);
