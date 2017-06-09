#include "quakedef.h"
#include "gl_screen.h"
#include "glHud.h"
#include "qgl.h"
#include "vgui2/text_draw.h"

void GLBeginHud()
{
	qglViewport( glx, gly, glwidth, glheight );

	qglMatrixMode( GL_PROJECTION );
	qglPushMatrix();
	qglLoadIdentity();
	//TODO: probably should match the max render distance? - Solokiller
	qglOrtho( 0, glwidth, glheight, 0, -99999, 99999 );

	qglMatrixMode( GL_MODELVIEW );
	qglPushMatrix();
	qglLoadIdentity();

	qglDisable( GL_DEPTH_TEST );
	qglDisable( GL_CULL_FACE );
	qglDisable( GL_BLEND );

	qglEnable( GL_ALPHA_TEST );
	qglColor4f( 1, 1, 1, 1 );

	VGUI2_ResetCurrentTexture();
}

void GLFinishHud()
{
	qglMatrixMode( GL_PROJECTION );
	qglPopMatrix();

	qglMatrixMode( GL_MODELVIEW );
	qglPopMatrix();

	qglEnable( GL_DEPTH_TEST );
	qglEnable( GL_CULL_FACE );
	qglEnable( GL_BLEND );
}