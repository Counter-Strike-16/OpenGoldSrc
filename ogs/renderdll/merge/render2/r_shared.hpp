
extern float pixelAspect;

typedef struct surf_s
{
	struct surf_s *next;   // active surface stack in r_edge.c
	struct surf_s *prev;   // used in r_edge.c for active surf stack
	struct espan_s *spans; // pointer to linked list of spans to draw
	int key;               // sorting key (BSP order)
	int last_u;            // set during tracing
	int spanstate;         // 0 = not in span
	                       // 1 = in span
	                       // -1 = in inverted span (end before
	                       //  start)
	int flags;             // currentface flags
	void *data;            // associated data like msurface_t
	cl_entity_t *entity;
	float nearzi; // nearest 1/z on surface, for mipmapping
	qboolean insubmodel;
	float d_ziorigin, d_zistepu, d_zistepv;

	int pad[2]; // to 64 bytes
} surf_t;

extern vec3_t base_modelorg;

extern int d_lightstylevalue[256]; // 8.8 frac of base light value

extern int r_skymade;
extern void R_MakeSky();

// !!! if this is changed, it must be changed in d_ifacea.h too !!!
#define ALIAS_ONSEAM 0x0020 // also defined in modelgen.h;
                            //  must be kept in sync

#define MAXVERTS 16                    // max points in a surface polygon
#define MAXWORKINGVERTS (MAXVERTS + 4) // max points in an intermediate
                                       //  polygon (while processing)

extern void R_DrawLine(polyvert_t *polyvert0, polyvert_t *polyvert1);