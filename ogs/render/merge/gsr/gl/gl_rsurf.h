#ifndef ENGINE_GL_RSURF_H
#define ENGINE_GL_RSURF_H

void R_DecalShoot( int textureIndex, int entity, int modelIndex, vec_t* position, int flags );

void R_DecalRemoveNonPermanent( int textureIndex );

void R_FireCustomDecal( int textureIndex, int entity, int modelIndex, vec_t* position, int flags, float scale );

#endif //ENGINE_GL_RSURF_H
