#ifndef ENGINE_GL_MODEL_H
#define ENGINE_GL_MODEL_H

#include "modelgen.h"
#include "spritegn.h"

/*
==============================================================================

SPRITE MODELS

==============================================================================
*/


// FIXME: shorten these?
struct mspriteframe_t
{
	int		width;
	int		height;
	float	up, down, left, right;
	int		gl_texturenum;
};

struct mspritegroup_t
{
	int				numframes;
	float			*intervals;
	mspriteframe_t	*frames[ 1 ];
};

struct mspriteframedesc_t
{
	spriteframetype_t	type;
	mspriteframe_t		*frameptr;
};

struct msprite_t
{
	short				type;
	short				texFormat;
	int					maxwidth;
	int					maxheight;
	int					numframes;
	int					paloffset;
	float				beamlength;		// remove?
	void				*cachespot;		// remove?
	mspriteframedesc_t	frames[ 1 ];
};

/*
==============================================================================

ALIAS MODELS

Alias models are position independent, so the cache manager can move them.
==============================================================================
*/

typedef struct
{
	int					firstpose;
	int					numposes;
	float				interval;
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
	char				name[ 16 ];
} maliasframedesc_t;

typedef struct
{
	trivertx_t			bboxmin;
	trivertx_t			bboxmax;
	int					frame;
} maliasgroupframedesc_t;

typedef struct
{
	int						numframes;
	int						intervals;
	maliasgroupframedesc_t	frames[ 1 ];
} maliasgroup_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct mtriangle_s {
	int					facesfront;
	int					vertindex[ 3 ];
} mtriangle_t;


#define	MAX_SKINS	32
typedef struct
{
	int			ident;
	int			version;
	vec3_t		scale;
	vec3_t		scale_origin;
	float		boundingradius;
	vec3_t		eyeposition;
	int			numskins;
	int			skinwidth;
	int			skinheight;
	int			numverts;
	int			numtris;
	int			numframes;
	synctype_t	synctype;
	int			flags;
	float		size;

	int					numposes;
	int					poseverts;
	int					posedata;	// numposes*poseverts trivert_t
	int					commands;	// gl command list with embedded s/t
	int					gl_texturenum[ MAX_SKINS ][ 4 ];
	int					texels[ MAX_SKINS ];	// only for player skins
	maliasframedesc_t	frames[ 1 ];	// variable sized
} aliashdr_t;

#define	MAXALIASVERTS	1024
#define	MAXALIASFRAMES	256
#define	MAXALIASTRIS	2048
extern	aliashdr_t	*pheader;
extern	stvert_t	stverts[ MAXALIASVERTS ];
extern	mtriangle_t	triangles[ MAXALIASTRIS ];
extern	trivertx_t	*poseverts[ MAXALIASFRAMES ];

void Mod_ClearAll();

#endif //ENGINE_GL_MODEL_H