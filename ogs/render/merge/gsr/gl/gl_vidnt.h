#ifndef ENGINE_GL_VIDNT_H
#define ENGINE_GL_VIDNT_H

// Multitexture
#define		QGL_TEXTURE0_SGIS	0x835E
#define		QGL_TEXTURE1_SGIS	0x835F
#define		QGL_TEXTURE2_SGIS	0x8360

extern int window_center_x;
extern int window_center_y;

extern int TEXTURE0_SGIS;
extern int TEXTURE1_SGIS;
extern int TEXTURE2_SGIS;

extern int gl_mtexable;

extern const char* gl_renderer;

float GetXMouseAspectRatioAdjustment();

float GetYMouseAspectRatioAdjustment();

void VID_UpdateWindowVars( Rect_t* prc, int x, int y );

void GL_BeginRendering( int* x, int* y, int* width, int* height );

void GL_EndRendering();

void VID_TakeSnapshotRect( const char* pFilename, int x, int y, int w, int h );

void VID_TakeSnapshot( const char* pFilename );

void VID_WriteBuffer( const char* pFilename );

#endif //ENGINE_GL_VIDNT_H