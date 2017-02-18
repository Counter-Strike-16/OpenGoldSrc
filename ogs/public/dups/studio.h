
#pragma once

#define MAXSTUDIOTRIANGLES	20000	// TODO: tune this
#define MAXSTUDIOVERTS		2048	// TODO: tune this
#define MAXSTUDIOSEQUENCES	2048	// total animation sequences
#define MAXSTUDIOSKINS		100		// total textures
#define MAXSTUDIOSRCBONES	512		// bones allowed at source movement
#define MAXSTUDIOBONES		128		// total bones actually used
#define MAXSTUDIOMODELS		32		// sub-models per model
#define MAXSTUDIOBODYPARTS	32
#define MAXSTUDIOGROUPS		16
#define MAXSTUDIOANIMATIONS	2048	// per sequence
#define MAXSTUDIOMESHES		256
#define MAXSTUDIOEVENTS		1024
#define MAXSTUDIOPIVOTS		256
#define MAXSTUDIOCONTROLLERS 8

// demand loaded sequence groups
/*
typedef struct
{
	char				label[32];	// textual name
	char				name[64];	// file name
	int32				unused1;    // was "cache"  - index pointer
	int					unused2;    // was "data" -  hack for group 0
} mstudioseqgroup_t;
*/

// skin info
/*
typedef struct
{
	char					name[64];
	int						flags;
	int						width;
	int						height;
	int						index;
} mstudiotexture_t;
*/

#define STUDIO_DYNAMIC_LIGHT		0x0100	// dynamically get lighting from floor or ceil (flying monsters)
#define STUDIO_TRACE_HITBOX		0x0200	// always use hitbox trace instead of bbox

// lighting options
#define STUDIO_NF_FLATSHADE		0x0001
#define STUDIO_NF_CHROME		0x0002
#define STUDIO_NF_FULLBRIGHT	0x0004
#define STUDIO_NF_NOMIPS		0x0008
#define STUDIO_NF_ALPHA			0x0010
#define STUDIO_NF_ADDITIVE		0x0020
#define STUDIO_NF_MASKED		0x0040

#define RAD_TO_STUDIO		(32768.0/M_PI)
#define STUDIO_TO_RAD		(M_PI/32768.0)


#define STUDIO_NUM_HULLS 128
#define STUDIO_NUM_PLANES (STUDIO_NUM_HULLS * 6)
#define STUDIO_CACHE_SIZE 16
