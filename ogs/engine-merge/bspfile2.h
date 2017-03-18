

#define	MAX_MAP_PLANES		32767
#define	MAX_MAP_NODES		32767		// because negative shorts are contents
#define	MAX_MAP_CLIPNODES	32767		//
#define	MAX_MAP_LEAFS		8192

#define	MAX_MAP_TEXTURES	512


#define	MAX_MAP_PORTALS		65536

#define	TOOLVERSION	2



//============================================================================

#ifndef QUAKE_GAME

#define	ANGLE_UP	-1
#define	ANGLE_DOWN	-2


// the utilities get to be lazy and just use large static arrays


void DecompressVis (byte *in, byte *decompressed);
int CompressVis (byte *vis, byte *dest);

void	LoadBSPFile (char *filename);
void	WriteBSPFile (char *filename);
void	PrintBSPFileSizes (void);

//===============


typedef struct epair_s
{
	struct epair_s	*next;
	char	*key;
	char	*value;
} epair_t;

typedef struct
{
	vec3_t		origin;
	int			firstbrush;
	int			numbrushes;
	epair_t		*epairs;
} entity_t;

extern	int			num_entities;
extern	entity_t	entities[MAX_MAP_ENTITIES];

void	ParseEntities (void);
void	UnparseEntities (void);

void 	SetKeyValue (entity_t *ent, char *key, char *value);
char 	*ValueForKey (entity_t *ent, char *key);
// will return "" if not present

vec_t	FloatForKey (entity_t *ent, char *key);
void 	GetVectorForKey (entity_t *ent, char *key, vec3_t vec);

epair_t *ParseEpair (void);

#endif
