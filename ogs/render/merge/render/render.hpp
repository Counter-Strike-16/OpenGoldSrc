/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#define MAXCLIPPLANES 11

typedef struct efrag_s
{
	struct mleaf_s *leaf;
	struct efrag_s *leafnext;
	struct entity_s *entity;
	struct efrag_s *entnext;
} efrag_t;

typedef struct entity_s
{
	qboolean forcelink; // model changed

	int update_type;

	entity_state_t baseline; // to fill in defaults in updates

	double msgtime;        // time of last update
	vec3_t msg_origins[2]; // last two updates (0 is newest)
	vec3_t origin;
	vec3_t msg_angles[2]; // last two updates (0 is newest)
	vec3_t angles;
	struct model_s *model; // NULL = no model
	struct efrag_s *efrag; // linked list of efrags
	int frame;
	float syncbase; // for client-side animations
	byte *colormap;
	int effects;  // light, particals, etc
	int skinnum;  // for Alias models
	int visframe; // last frame this entity was
	              //  found in an active leaf

	int dlightframe; // dynamic lighting
	int dlightbits;

	// FIXME: could turn these into a union
	int trivial_accept;
	struct mnode_s *topnode; // for bmodels, first world node
	                         //  that splits bmodel, or NULL if
	                         //  not split
} entity_t;



void R_Init(void);
void R_InitTextures(void);
void R_InitEfrags(void);
void R_RenderView(void); // must set r_refdef first

void R_InitSky(struct texture_s *mt); // called at level load

void R_AddEfrags(entity_t *ent);
void R_RemoveEfrags(entity_t *ent);

void R_NewMap(void);

void R_ParseParticleEffect(void);

#ifdef QUAKE2
void R_DarkFieldParticles(entity_t *ent);
#endif
void R_EntityParticles(entity_t *ent);

void R_ParticleExplosion2(vec3_t org, int colorStart, int colorLength);

void R_PushDlights(void);

void D_FlushCaches(void);
void D_DeleteSurfaceCache(void);
void D_InitCaches(void *buffer, int size);
void R_SetVrect(vrect_t *pvrect, vrect_t *pvrectin, int lineadj);