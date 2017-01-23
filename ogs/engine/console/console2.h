

#define	NUM_CON_TIMES 4

#define		CON_TEXTSIZE	32768
typedef struct
{
	qboolean	initialized;

	int		ormask;			// high bit mask for colored characters

	int 	linewidth;		// characters across screen
	int		totallines;		// total lines in console scrollback

	float	cursorspeed;

	int		vislines;

	float	times[NUM_CON_TIMES];	// cls.realtime time the line was generated
								// for transparent notify lines
} console_t;

extern	console_t	con;

void Con_DrawConsole (float frac);

void Con_CenteredPrint (char *text);