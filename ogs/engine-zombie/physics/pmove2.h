

typedef struct
{
	int		sequence;	// just for debugging prints

	// player state
	
	// world state
	physent_t	physents[MAX_PHYSENTS];	// 0 should be the world
} playermove_t;

extern	playermove_t	pmove;
extern	int		onground;
extern	int		waterlevel;
extern	int		watertype;

void PlayerMove (void);

int PM_HullPointContents (hull_t *hull, int num, vec3_t p);

int PM_PointContents (vec3_t point);
qboolean PM_TestPlayerPosition (vec3_t point);
pmtrace_t PM_PlayerMove (vec3_t start, vec3_t stop);
