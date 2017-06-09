/**
*	@file
*
*	NT GL vid component
*/

//Included first so Windows definitions included by SDL & GLEW don't try to override ours. - Solokiller
#ifdef WIN32
#include <winlite.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <GL/glew.h>

#include "quakedef.h"
#include "DetailTexture.h"
#include "qgl.h"
#include "gl_draw.h"
#include "gl_rmisc.h"
#include "gl_vidnt.h"
#include "strtools.h"
#include "sys_getmodes.h"
#include "vid.h"

cvar_t gl_ztrick = { "gl_ztrick", "0" };
cvar_t gl_vsync = { "gl_vsync", "1", FCVAR_ARCHIVE };

bool scr_skipupdate = false;
bool scr_skiponeupdate = false;

Rect_t window_rect = {};

int window_center_x = 0;
int window_center_y = 0;

int gGLHardwareType = 0;

int TEXTURE0_SGIS = GL_ASYNC_READ_PIXELS_SGIX;
int TEXTURE1_SGIS = GL_MAX_ASYNC_TEX_IMAGE_SGIX;
int TEXTURE2_SGIS = GL_MAX_ASYNC_DRAW_PIXELS_SGIX;

int gl_mtexable = 0;

struct FBO_Container_t
{
	GLuint s_hBackBufferFBO;
	GLuint s_hBackBufferCB;
	GLuint s_hBackBufferDB;
	GLuint s_hBackBufferTex;
};

static bool gfMiniDriver = false;
static bool s_bEnforceAspect = false;
static bool bDoMSAAFBO = true;
static bool bDoScaledFBO = true;
static bool s_bSupportsBlitTexturing = false;

static int gl_filter_min = GL_LINEAR;

const char* gl_vendor = nullptr;
const char* gl_renderer = nullptr;
const char* gl_version = nullptr;
const char* gl_extensions = nullptr;

bool g_bSupportsNPOTTextures = false;
bool atismoothing = false;

static FBO_Container_t s_MSAAFBO;
static FBO_Container_t s_BackBufferFBO;

static float s_fXMouseAspectAdjustment = 1.0;
static float s_fYMouseAspectAdjustment = 1.0;

float GetXMouseAspectRatioAdjustment()
{
	return s_fXMouseAspectAdjustment;
}

float GetYMouseAspectRatioAdjustment()
{
	return s_fYMouseAspectAdjustment;
}

void GL_Config()
{
	Cbuf_InsertText( "exec hw/opengl.cfg\n" );

	char lowerCase[ 256 ];
	Q_strncpy( lowerCase, gl_renderer, ARRAYSIZE( lowerCase ) - 1 );
	lowerCase[ ARRAYSIZE( lowerCase ) - 1 ] = '\0';

	if( Q_strstr( gl_vendor, "3Dfx" ) )
	{
		gGLHardwareType = 1;
		if( Q_strstr( gl_renderer, "Voodoo(tm)" ) )
		{
			Cbuf_InsertText( "exec hw/3DfxVoodoo1.cfg\n" );
		}
		else if( Q_strstr( gl_renderer, "Voodoo^2" ) )
		{
			Cbuf_InsertText( "exec hw/3DfxVoodoo2.cfg\n" );
		}
		else
		{
			Cbuf_InsertText( "exec hw/3Dfx.cfg\n" );
		}
	}
	else if( Q_strstr( gl_vendor, "NVIDIA" ) )
	{
		if( Q_strstr( gl_renderer, "RIVA 128" ) )
		{
			gGLHardwareType = 2;
			Cbuf_InsertText( "exec hw/riva128.cfg\n" );
		}
		else if( Q_strstr( gl_renderer, "TNT" ) )
		{
			gGLHardwareType = 10;
			Cbuf_InsertText( "exec hw/rivaTNT.cfg\n" );
		}
		else if( strstr( gl_renderer, "Quadro" ) ||
				 strstr( gl_renderer, "GeForce" ) )
		{
			gGLHardwareType = 11;
			Cbuf_InsertText( "exec hw/geforce.cfg\n" );
		}
	}
	else if( Q_strstr( lowerCase, "riva tnt" ) ||
			 Q_strstr( lowerCase, "velocity 4400" ) )
	{
		Cbuf_InsertText( "exec hw/rivaTNT.cfg\n" );
		gGLHardwareType = 10;
	}
	else if( Q_strstr( gl_vendor, "PCX2" ) )
	{
		Cbuf_InsertText( "exec hw/PowerVRPCX2.cfg\n" );
		gGLHardwareType = 3;
	}
	else if( Q_strstr( gl_vendor, "PowerVR" ) )
	{
		Cbuf_InsertText( "exec hw/PowerVRSG.cfg\n" );
		gGLHardwareType = 4;
	}
	else if( Q_strstr( gl_vendor, "V2200" ) )
	{
		Cbuf_InsertText( "exec hw/V2200.cfg\n" );
		gGLHardwareType = 5;
	}
	else if( Q_strstr( gl_vendor, "3Dlabs" ) )
	{
		Cbuf_InsertText( "exec hw/3Dlabs.cfg\n" );
		gGLHardwareType = 6;
	}
	else if( Q_strstr( gl_vendor, "Matrox" ) )
	{
		gGLHardwareType = 0;
		Cbuf_InsertText( "exec hw/matrox.cfg\n" );
	}
	else if( Q_strstr( gl_vendor, "ATI" )
				&& ( Q_strstr( gl_renderer, "Rage" ) ||
					 Q_strstr( gl_renderer, "RAGE" ) )
				&& Q_strstr( gl_renderer, "128" ) )
	{
		gGLHardwareType = 0;
		Cbuf_InsertText( "exec hw/ATIRage128.cfg\n" );
	}
	else
	{
		gGLHardwareType = 0;
		if( Q_strstr( gl_renderer, "Matrox" ) &&
			Q_strstr( gl_renderer, "G200" ) )
		{
			Cbuf_InsertText( "exec hw/G200d3d.cfg\n" );
		}
		else if( Q_strstr( gl_renderer, "ATI" )
					&& ( Q_strstr( gl_renderer, "Rage" ) ||
						 Q_strstr( gl_renderer, "RAGE" ) )
					&& Q_strstr( gl_renderer, "128" ) )
		{
			Cbuf_InsertText( "exec hw/ATIRage128d3d.cfg\n" );
		}
		else if( Q_strstr( gl_vendor, "NVIDIA" ) )
		{
			Cbuf_InsertText( "exec hw/nvidiad3d.cfg\n" );
		}
	}
}

void CheckTextureExtensions()
{
	if( gl_extensions &&
		( Q_strstr( gl_extensions, "GL_EXT_paletted_texture" ) &&
		  Q_strstr( gl_extensions, "GL_EXT_shared_texture_palette" ) ) )
	{
		qglColorTableEXT = reinterpret_cast<decltype( qglColorTableEXT )>( SDL_GL_GetProcAddress( "glColorTableEXT" ) );
		Con_Printf( "Found paletted texture extension.\n" );
	}
	else
	{
		qglColorTableEXT = nullptr;
	}

	if( gl_extensions && Q_strstr( gl_extensions, "GL_EXT_texture_object " ) )
	{
		qglBindTexture = reinterpret_cast<decltype( qglBindTexture )>( SDL_GL_GetProcAddress( "glBindTextureEXT" ) );
		if( !qglBindTexture )
			Sys_Error( "GetProcAddress for BindTextureEXT failed" );
	}
}

void CheckMultiTextureExtensions()
{
	if( gl_extensions && Q_strstr( gl_extensions, "GL_ARB_multitexture " ) )
	{
		Con_Printf( "ARB Multitexture extensions found.\n" );

		qglMTexCoord2fSGIS = reinterpret_cast<decltype( qglMTexCoord2fSGIS )>( SDL_GL_GetProcAddress( "glMultiTexCoord2fARB" ) );
		qglSelectTextureSGIS = reinterpret_cast<decltype( qglSelectTextureSGIS )>( SDL_GL_GetProcAddress( "glActiveTextureARB" ) );
			
		TEXTURE0_SGIS = GL_TEXTURE0;
		TEXTURE1_SGIS = GL_TEXTURE1;
		TEXTURE2_SGIS = GL_TEXTURE2;

		oldtarget = TEXTURE0_SGIS;
		gl_mtexable = 2;
		GL_SelectTexture( TEXTURE0_SGIS );

		if( Q_strstr( gl_extensions, "GL_ARB_texture_env_combine " ) ||
			Q_strstr( gl_extensions, "GL_EXT_texture_env_combine " ) )
		{
			GLint num;
			qglGetIntegerv( GL_MAX_TEXTURE_UNITS, &num );

			if( num > 2 )
			{
				Con_Printf( "%d texture units.  Detail texture supported.\n", num );
				gl_mtexable = num;
				DT_Initialize();
			}
		}
	}
	else if( gl_extensions && Q_strstr( gl_extensions, "GL_SGIS_multitexture " ) )
	{
		Con_Printf( "Multitexture extensions found.\n" );

		qglMTexCoord2fSGIS = reinterpret_cast<decltype( qglMTexCoord2fSGIS )>( SDL_GL_GetProcAddress( "glMTexCoord2fSGIS" ) );
		qglSelectTextureSGIS = reinterpret_cast<decltype( qglSelectTextureSGIS )>( SDL_GL_GetProcAddress( "glSelectTextureSGIS" ) );

		TEXTURE0_SGIS = QGL_TEXTURE0_SGIS;
		TEXTURE1_SGIS = QGL_TEXTURE1_SGIS;
		TEXTURE2_SGIS = QGL_TEXTURE2_SGIS;

		oldtarget = TEXTURE0_SGIS;
		gl_mtexable = 2;
		GL_SelectTexture( TEXTURE0_SGIS );
	}
	else
	{
		Con_Printf( "NO Multitexture extensions found.\n" );
	}
}

//TODO: check if this is used on Windows - Solokiller
void CheckArrayExtensions()
{
	auto pszExtensions = reinterpret_cast<const char*>( qglGetString( GL_EXTENSIONS ) );

	if( pszExtensions )
	{
		while( *pszExtensions )
		{
			if( !Q_strncmp( pszExtensions, "GL_EXT_vertex_array", Q_strlen( "GL_EXT_vertex_array" ) ) )
				return;

			++pszExtensions;
		}
	}

	Sys_Error( "Vertex array extension not present" );
}

void CheckATINPatchExtensions()
{
	atismoothing = false;
}

void GL_Init()
{
	gl_vendor = reinterpret_cast<const char*>( qglGetString( GL_VENDOR ) );

	if( !gl_vendor )
		Sys_Error( "Failed to query gl vendor string" );

	Con_DPrintf( "GL_VENDOR: %s\n", gl_vendor );

	gl_renderer = reinterpret_cast<const char*>( qglGetString( GL_RENDERER ) );
	Con_DPrintf( "GL_RENDERER: %s\n", gl_renderer );

	gl_version = reinterpret_cast<const char*>( qglGetString( GL_VERSION ) );
	Con_DPrintf( "GL_VERSION: %s\n", gl_version );

	gl_extensions = reinterpret_cast<const char*>( qglGetString( GL_EXTENSIONS ) );
	g_bSupportsNPOTTextures = 0;

	if( COM_CheckParm( "-glext" ) )
		Con_DPrintf( "GL_EXTENSIONS: %s\n", gl_extensions );

	int r, g, b, a, depth;

	if( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &r ) )
	{
		r = 0;
		Con_DPrintf( "Failed to get GL RED size (%s)\n", SDL_GetError() );
	}

	if( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &g ) )
	{
		g = 0;
		Con_DPrintf( "Failed to get GL GREEN size (%s)\n", SDL_GetError() );
	}

	if( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &b ) )
	{
		b = 0;
		Con_DPrintf( "Failed to get GL BLUE size (%s)\n", SDL_GetError() );
	}

	if( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &a ) )
	{
		a = 0;
		Con_DPrintf( "Failed to get GL ALPHA size (%s)\n", SDL_GetError() );
	}

	if( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &depth ) )
	{
		depth = 0;
		Con_DPrintf( "Failed to get GL DEPTH size (%s)\n", SDL_GetError() );
	}

	Con_DPrintf( "GL_SIZES:  r:%d g:%d b:%d a:%d depth:%d\n", r, g, b, a, depth );

	CheckTextureExtensions();
	CheckMultiTextureExtensions();
	CheckATINPatchExtensions();

	if( gl_extensions && Q_strstr( gl_extensions, "GL_ARB_texture_non_power_of_two" ) )
		g_bSupportsNPOTTextures = true;

	qglClearColor( 1.0, 0.0, 0.0, 0.0 );
	qglCullFace( GL_FRONT );
	qglEnable( GL_TEXTURE_2D );
	qglEnable( GL_ALPHA_TEST );
	qglAlphaFunc( GL_NOTEQUAL, 0.0 );
	qglPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	qglShadeModel( GL_FLAT );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	qglTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_ansio.value );
	qglBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	qglTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	GL_Config();
}

void FreeFBOObjects()
{
	if( s_MSAAFBO.s_hBackBufferFBO )
		qglDeleteFramebuffersEXT( 1, &s_MSAAFBO.s_hBackBufferFBO );
	s_MSAAFBO.s_hBackBufferFBO = 0;

	if( s_MSAAFBO.s_hBackBufferCB )
		qglDeleteRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferCB );
	s_MSAAFBO.s_hBackBufferCB = 0;

	if( s_MSAAFBO.s_hBackBufferDB )
		qglDeleteRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferDB );
	s_MSAAFBO.s_hBackBufferDB = 0;

	if( s_MSAAFBO.s_hBackBufferTex )
		qglDeleteTextures( 1, &s_MSAAFBO.s_hBackBufferTex );
	s_MSAAFBO.s_hBackBufferTex = 0;

	if( s_BackBufferFBO.s_hBackBufferFBO )
		qglDeleteFramebuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferFBO );
	s_BackBufferFBO.s_hBackBufferFBO = 0;

	if( s_BackBufferFBO.s_hBackBufferCB )
		qglDeleteRenderbuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferCB );
	s_BackBufferFBO.s_hBackBufferCB = 0;

	if( s_BackBufferFBO.s_hBackBufferDB )
		qglDeleteRenderbuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferDB );
	s_BackBufferFBO.s_hBackBufferDB = 0;

	if( s_BackBufferFBO.s_hBackBufferTex )
		qglDeleteTextures( 1, &s_BackBufferFBO.s_hBackBufferTex );
	s_BackBufferFBO.s_hBackBufferTex = 0;
}

#ifdef WIN32
//Note: this code is obsolete and won't have any effect.
//The cross-platform OpenGL context creation code supersedes this. - Solokiller
bool bSetupPixelFormat( HDC hdc )
{
	static PIXELFORMATDESCRIPTOR ppfd =
	{
		sizeof( PIXELFORMATDESCRIPTOR ),	// size of this pfd
		1,									// version number
		PFD_DRAW_TO_WINDOW 					// support window
		| PFD_SUPPORT_OPENGL 				// support OpenGL
		| PFD_DOUBLEBUFFER ,				// double buffered
		PFD_TYPE_RGBA,						// RGBA type
		32,									// 32-bit color depth
		0, 0, 0, 0, 0, 0,					// color bits ignored
		0,									// no alpha buffer
		0,									// shift bit ignored
		0,									// no accumulation buffer
		0, 0, 0, 0, 						// accum bits ignored
		32,									// 32-bit z-buffer	
		0,									// no stencil buffer
		0,									// no auxiliary buffer
		PFD_MAIN_PLANE,						// main layer
		0,									// reserved
		0, 0, 0								// layer masks ignored
	};

	static bool bInitialized = false;

	if( bInitialized )
		return true;

	bInitialized = true;

	if( gfMiniDriver )
	{
		int index = qwglChoosePixelFormat( hdc, &ppfd );

		if( !index )
		{
			MessageBoxA( NULL, "ChoosePixelFormat failed", "Error", MB_OK );
			return false;
		}

		if( !qwglSetPixelFormat( hdc, index, &ppfd ) )
		{
			MessageBoxA( NULL, "SetPixelFormat failed", "Error", MB_OK );
			return false;
		}

		qwglDescribePixelFormat( hdc, index, sizeof( ppfd ), &ppfd );

		return true;
	}

	const int index = ChoosePixelFormat( hdc, &ppfd );

	if( !index )
	{
		MessageBoxA( NULL, "ChoosePixelFormat failed", "Error", MB_OK );
		return false;
	}

	if( !SetPixelFormat( hdc, index, &ppfd ) )
	{
		MessageBoxA( NULL, "SetPixelFormat failed", "Error", MB_OK );
		return false;
	}

	DescribePixelFormat( hdc, index, sizeof( ppfd ), &ppfd );

	//if not a generic Software driver or a hardware accelerated generic driver, we can use OpenGL.
	if( !( ppfd.dwFlags & PFD_GENERIC_FORMAT ) || ( ppfd.dwFlags & PFD_GENERIC_ACCELERATED ) )
		return true;

	return false;
}
#endif

bool GL_SetMode( SDL_Window* mainwindow, HDC* pmaindc, HGLRC* pbaseRC, const char* pszDriver, const char* pszCmdLine )
{
	gfMiniDriver = false;

	if( pmaindc )
		*pmaindc = 0;

	*pbaseRC = 0;
	*pmaindc = 0;

	//There's an if statement here in the Windows version to see if a global bool is false.
	//It disables the second context creation.
	//The boolean is only ever false, explaining why it's missing in the Linux version. - Solokiller
	//if( !g_bDisableMSAAFBO )
	//{
	s_bEnforceAspect = COM_CheckParm( "-stretchaspect" ) == 0;

	if( COM_CheckParm( "-nomsaa" ) )
		bDoMSAAFBO = false;

	if( COM_CheckParm( "-nofbo" ) )
		bDoScaledFBO = false;

	if( Host_GetVideoLevel() > 0 )
	{
		gl_filter_min = GL_LINEAR_MIPMAP_NEAREST;
		bDoScaledFBO = false;
	}

#ifdef WIN32
	const int freqIndex = COM_CheckParm( "-freq" );

	if( freqIndex )
	{
		if( VideoMode_IsWindowed() )
		{
			Con_Printf( "Setting frequency in windowed mode is unsupported\n" );
		}
		else
		{
			DEVMODEA DevMode;
			DEVMODEA target;

			memset( &DevMode, 0, sizeof( DevMode ) );
			DevMode.dmSize = sizeof( DevMode );

			EnumDisplaySettingsA( nullptr, ENUM_CURRENT_SETTINGS, &DevMode );

			memset( &target, 0, sizeof( DevMode ) );
			target.dmSize = sizeof( target );

			target.dmDisplayFrequency = atoi( com_argv[ freqIndex + 1 ] );
			target.dmFields |= DM_DISPLAYFREQUENCY;

			Con_Printf( "Setting monitor frequency to %d\n", target.dmDisplayFrequency );

			if( ChangeDisplaySettingsExA( nullptr, &target, NULL, CDS_FULLSCREEN, nullptr )
				&& ChangeDisplaySettingsA( &target, CDS_FULLSCREEN ) )
				Con_Printf( "Frequency %d is not supported by your monitor\n", target.dmDisplayFrequency );
		}
	}
#endif

	//TODO: remove obsolete parameters - Solokiller
	QGL_Init( pszDriver, pszCmdLine );
	s_bSupportsBlitTexturing = false;

	gl_extensions = reinterpret_cast<const char*>( qglGetString( GL_EXTENSIONS ) );

	if( gl_extensions && 
		Q_strstr( gl_extensions, "GL_EXT_framebuffer_multisample_blit_scaled" ) )
		s_bSupportsBlitTexturing = true;

	if( COM_CheckParm( "-directblit" ) )
		s_bSupportsBlitTexturing = true;

	if( COM_CheckParm( "-nodirectblit" ) )
		s_bSupportsBlitTexturing = false;

	if( !qglGenFramebuffersEXT || !qglBindFramebufferEXT || !qglBlitFramebufferEXT )
		bDoScaledFBO = false;

	if( gl_extensions
		&& !Q_strstr( gl_extensions, "GL_ARB_texture_rectangle" )
		&& !Q_strstr( gl_extensions, "GL_NV_texture_rectangle" ) )
		bDoScaledFBO = false;

	s_MSAAFBO.s_hBackBufferFBO = 0;

	if( VideoMode_IsWindowed() )
		return true;

	s_MSAAFBO.s_hBackBufferCB = 0;
	s_MSAAFBO.s_hBackBufferDB = 0;
	s_MSAAFBO.s_hBackBufferTex = 0;
	s_BackBufferFBO.s_hBackBufferFBO = 0;
	s_BackBufferFBO.s_hBackBufferCB = 0;
	s_BackBufferFBO.s_hBackBufferDB = 0;
	s_BackBufferFBO.s_hBackBufferTex = 0;

	int wide;
	int tall;

	VideoMode_GetCurrentVideoMode( &wide, &tall, nullptr );

	if( qglRenderbufferStorageMultisampleEXT
		&& bDoMSAAFBO
		&& gl_extensions
		&& Q_strstr( gl_extensions, "GL_EXT_framebuffer_multisample" ) )
	{
		qglGenFramebuffersEXT( 1, &s_MSAAFBO.s_hBackBufferFBO );
		qglBindFramebufferEXT( GL_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO );

		qglGenRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferCB );
		qglBindRenderbufferEXT( GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferCB );

		qglRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER, 4, GL_RGBA8, wide, tall );
		qglFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferCB );

		qglGenRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferDB );
		qglBindRenderbufferEXT( GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferDB );

		qglRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, wide, tall );

		qglFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_MSAAFBO.s_hBackBufferDB );

		if( qglCheckFramebufferStatusEXT( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			if( s_MSAAFBO.s_hBackBufferFBO )
				qglDeleteFramebuffersEXT( 1, &s_MSAAFBO.s_hBackBufferFBO );
			s_MSAAFBO.s_hBackBufferFBO = 0;

			if( s_MSAAFBO.s_hBackBufferCB )
				qglDeleteRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferCB );
			s_MSAAFBO.s_hBackBufferCB = 0;

			if( s_MSAAFBO.s_hBackBufferDB )
				qglDeleteRenderbuffersEXT( 1, &s_MSAAFBO.s_hBackBufferDB );
			s_MSAAFBO.s_hBackBufferDB = 0;

			if( s_MSAAFBO.s_hBackBufferTex )
				qglDeleteTextures( 1, &s_MSAAFBO.s_hBackBufferTex );
			s_MSAAFBO.s_hBackBufferTex = 0;

			Con_Printf( "Error initializing MSAA frame buffer\n" );
		}
	}
	else
	{
		Con_Printf( "MSAA backbuffer rendering disabled.\n" );
		s_MSAAFBO.s_hBackBufferFBO = 0;
	}

	if( bDoScaledFBO )
	{
		if( s_MSAAFBO.s_hBackBufferFBO )
			glEnable( GL_MULTISAMPLE );

		glEnable( GL_TEXTURE_RECTANGLE );
		qglGenFramebuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferFBO );
		qglBindFramebufferEXT( GL_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO );

		if( !s_MSAAFBO.s_hBackBufferFBO )
		{
			qglGenRenderbuffersEXT( 1, &s_BackBufferFBO.s_hBackBufferDB );
			qglBindRenderbufferEXT( GL_RENDERBUFFER, s_BackBufferFBO.s_hBackBufferDB );
			qglRenderbufferStorageEXT( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, wide, tall );
			qglFramebufferRenderbufferEXT( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, s_BackBufferFBO.s_hBackBufferDB );
		}

		glGenTextures( 1, &s_BackBufferFBO.s_hBackBufferTex );

		glBindTexture( GL_TEXTURE_RECTANGLE, s_BackBufferFBO.s_hBackBufferTex );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );

		qglFramebufferTexture2DEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, s_BackBufferFBO.s_hBackBufferTex, 0 );

		glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

		glDisable( GL_TEXTURE_RECTANGLE );
	}

	if( !bDoScaledFBO || qglCheckFramebufferStatusEXT( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		FreeFBOObjects();

		SDL_DisplayMode requestedMode;
		SDL_DisplayMode mode;

		requestedMode.format = 0;
		requestedMode.refresh_rate = 0;
		requestedMode.w = wide;
		requestedMode.h = tall;

		if( !SDL_GetClosestDisplayMode( 0, &requestedMode, &mode ) )
			Sys_Error( "Error initializing Main frame buffer\n" );
		if( bDoScaledFBO )
			Con_Printf( "FBO backbuffer rendering disabled due to create error.\n" );
		else
			Con_Printf( "FBO backbuffer rendering disabled.\n" );
		SDL_SetWindowDisplayMode( mainwindow, &mode );

		bNeedsFullScreenModeSwitch = true;
		VideoMode_RestoreVideo();
	}

	if( bDoScaledFBO )
	{
		qglBindFramebufferEXT( GL_FRAMEBUFFER, 0 );
	}

	return true;
	//} //!g_bDisableMSAAFBO

#ifdef WIN32
	SDL_SysWMinfo wmInfo;

	SDL_VERSION( &wmInfo.version );

	SDL_GetWindowWMInfo( mainwindow, &wmInfo );

	HDC hDC = GetDC( wmInfo.info.win.window );

	*pmaindc = hDC;

	//TODO: why? - Solokiller
	s_BackBufferFBO.s_hBackBufferFBO = 0;

	if( !bSetupPixelFormat( hDC ) )
		return false;

	auto context = qwglCreateContext( hDC );

	*pbaseRC = context;

	if( !context )
		return false;

	return qwglMakeCurrent( hDC, context ) != FALSE;
#else
	return true;
#endif
}

void GL_Shutdown( SDL_Window* mainwindow, HDC hdc, HGLRC hglrc )
{
	FreeFBOObjects();
}

//TODO: obsolete - Solokiller
bool VID_AllocBuffers()
{
	return true;
}

bool VID_Init( unsigned short* palette )
{
	Cvar_RegisterVariable( &gl_ztrick );
	Cvar_RegisterVariable( &gl_vsync );

	if( COM_CheckParm( "-gl_log" ) )
		Cmd_AddCommand( "gl_log", GLimp_EnableLogging );

	return true;
}

void VID_UpdateWindowVars( Rect_t* prc, int x, int y )
{
	window_rect.x = prc->x;
	window_rect.width = prc->width;
	window_rect.y = prc->y;
	window_rect.height = prc->height;

	if( pmainwindow )
	{
		int w, h;
		SDL_GetWindowSize( reinterpret_cast<SDL_Window*>( pmainwindow ), &w, &h );

		int x_0, y_0;
		SDL_GetWindowPosition( reinterpret_cast<SDL_Window*>( pmainwindow ), &x_0, &y_0 );

		prc->x = x_0;
		prc->width = x_0 + w;

		prc->y = y_0;
		prc->height = y_0 + h;
		window_center_x = x_0 + w / 2;
		window_center_y = y_0 + h / 2;
	}
	else
	{
		window_center_x = x;
		window_center_y = y;
	}
}

static qboolean vsync = false;

void GL_BeginRendering( int* x, int* y, int* width, int* height )
{
	*x = 0;
	*y = 0;

	if( VideoMode_IsWindowed() )
	{
		*width = window_rect.width - window_rect.x;
		*height = window_rect.height - window_rect.y;
	}
	else
	{
		VideoMode_GetCurrentVideoMode( width, height, nullptr );
	}

	vid.conwidth = *width;
	vid.width = *width;
	vid.conheight = *height;
	vid.height = *height;

	if( vsync != gl_vsync.value )
	{
		SDL_GL_SetSwapInterval( gl_vsync.value > 0 );
		vsync = gl_vsync.value != 0;
	}

	if( s_BackBufferFBO.s_hBackBufferFBO )
	{
		if( s_MSAAFBO.s_hBackBufferFBO )
		{
			qglBindFramebufferEXT( GL_DRAW_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO );
		}
		else
		{
			qglBindFramebufferEXT( GL_DRAW_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO );
		}

		glClearColor( 0, 0, 0, 1.0 );

		if( gl_clear.value )
		{
			glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
		}
		else
		{
			qglClear( GL_DEPTH_BUFFER_BIT );
		}
	}

	GLimp_LogNewFrame();
}

void GL_EndRendering()
{
	if( s_BackBufferFBO.s_hBackBufferFBO )
	{
		int width, height;

		VideoMode_GetCurrentVideoMode( &width, &height, nullptr );

		if( s_MSAAFBO.s_hBackBufferFBO )
		{
			qglBindFramebufferEXT( GL_DRAW_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO );
			qglBindFramebufferEXT( GL_READ_FRAMEBUFFER, s_MSAAFBO.s_hBackBufferFBO );
			qglBlitFramebufferEXT(
				0, 0, width, height,
				0, 0, width, height,
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR
			);
		}

		qglBindFramebufferEXT( GL_DRAW_FRAMEBUFFER, 0 );
		qglBindFramebufferEXT( GL_READ_FRAMEBUFFER, s_BackBufferFBO.s_hBackBufferFBO );

		glClearColor( 0, 0, 0, 0 );
		glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
		int windowWidth = window_rect.width - window_rect.x;
		int windowHeight = window_rect.height - window_rect.y;

		s_fXMouseAspectAdjustment = 1.0;
		s_fYMouseAspectAdjustment = 1.0;

		int iDestX = 0;
		int iDestY = 0;
		int iDestWidth = windowWidth;
		int iDestHeight = windowHeight;

		const double flOffset = 0;

		if( s_bEnforceAspect )
		{
			double flAspect = width / static_cast<double>( height );
			double flWindowAspect = windowWidth / static_cast<double>( windowHeight );

			if( flWindowAspect <= flAspect )
			{
				if( flWindowAspect < flAspect )
				{
					s_fYMouseAspectAdjustment = flAspect / flWindowAspect;
					
					const double flInvAspectX = 1.0 / flAspect * windowWidth;
					const double flCenter = ( windowHeight - flInvAspectX ) * 0.5;
					
					iDestY = static_cast<int>( floor( flOffset + flCenter ) );
					iDestHeight = static_cast<int>( floor( windowHeight - flCenter ) );
				}
			}
			else
			{
				s_fXMouseAspectAdjustment = flWindowAspect / flAspect;

				const double flCenter = ( windowWidth - windowHeight * flAspect ) * 0.5;

				iDestX = static_cast<int>( floor( flOffset + flCenter ) );
				iDestWidth = static_cast<int>( floor( windowWidth - flCenter ) );
			}
		}

		if( s_bSupportsBlitTexturing )
		{
			qglBlitFramebufferEXT(
				0, 0, width, height,
				iDestX, iDestY, iDestWidth, iDestHeight,
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR
				);
		}
		else
		{
			glDisable( GL_BLEND );
			glDisable( GL_LIGHTING );
			glDisable( GL_DEPTH_TEST );
			glDisable( GL_ALPHA_TEST );
			glDisable( GL_CULL_FACE );

			qglMatrixMode( GL_PROJECTION );
			qglPushMatrix();
			glLoadIdentity();
			glOrtho( 0, windowWidth, windowHeight, 0, -1.0, 1 );

			qglMatrixMode( GL_MODELVIEW );
			qglPushMatrix();
			glLoadIdentity();
			qglViewport( 0, 0, windowWidth, windowHeight );

			glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			qglColor4f( 1, 1, 1, 1 );

			glEnable( GL_TEXTURE_RECTANGLE );
			glBindTexture( GL_TEXTURE_RECTANGLE, s_BackBufferFBO.s_hBackBufferTex );
			
			glBegin( GL_QUADS );

			glTexCoord2f( 0, height );
			glVertex3f( iDestX, iDestY, 0 );

			glTexCoord2f( width, height );
			glVertex3f( iDestWidth, iDestY, 0 );

			glTexCoord2f( width, 0 );
			glVertex3f( iDestWidth, iDestHeight, 0 );

			glTexCoord2f( 0, 0 );
			glVertex3f( iDestX, iDestHeight, 0 );

			glEnd();

			glBindTexture( GL_TEXTURE_RECTANGLE, 0 );

			qglMatrixMode( GL_PROJECTION );
			qglPopMatrix();

			qglMatrixMode( GL_MODELVIEW );
			qglPopMatrix();

			glDisable( GL_TEXTURE_RECTANGLE );
		}

		qglBindFramebufferEXT( GL_READ_FRAMEBUFFER, 0 );
	}

	VID_FlipScreen();
}

void VID_ShiftPalette()
{
	//Nothing
}

void VID_TakeSnapshotRect( const char* pFilename, int x, int y, int w, int h )
{
	//TODO: refactor all BMP file I/O to use a single set of functions, types and constants (or a library) - Solokiller
	const auto iPixels = w * h;

	auto hFile = FS_OpenPathID( pFilename, "wb", "GAMECONFIG" );

	if( !hFile )
		Sys_Error( "Couldn't create file for snapshot.\n" );

	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bi;

	hdr.bfSize = 3 * iPixels + sizeof( hdr ) + sizeof( bi );
	hdr.bfType = 19778;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = sizeof( hdr ) + sizeof( bi );

	if( FS_Write( &hdr, sizeof( hdr ), hFile ) != sizeof( hdr ) )
		Sys_Error( "Couldn't write file header to snapshot.\n" );

	bi.biSize = sizeof( bi );
	bi.biWidth = w;
	bi.biPlanes = 1;
	bi.biHeight = h;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	bi.biBitCount = 24;
	bi.biCompression = 0;

	if( FS_Write( &bi, sizeof( bi ), hFile ) != sizeof( bi ) )
		Sys_Error( "Couldn't write bitmap header to snapshot.\n" );

	auto pRed = reinterpret_cast<byte*>( Mem_Malloc( 3 * iPixels ) );

	if( !pRed )
		Sys_Error( "Couldn't allocate bitmap header to snapshot.\n" );

	qglPixelStorei( GL_PACK_ALIGNMENT, 1 );
	qglReadPixels( x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, pRed );

	//Convert to BGR
	{
		int temp;

		for( int i = 0; i < iPixels; ++i )
		{
			temp = pRed[ ( 3 * iPixels ) + 2 ];
			pRed[ ( 3 * iPixels ) + 2 ] = pRed[ 3 * iPixels ];
			pRed[ 3 * iPixels ] = temp;
		}
	}

	const auto iLineSize = 3 * w + ( -3 * w & 3 );

	//Write to file, one line of pixels at a time
	for( int i = 0; i < h; ++i )
	{
		if( iLineSize != FS_Write( pRed + ( 3 * w * i ), iLineSize, hFile ) )
			Sys_Error( "Couldn't write bitmap data snapshot.\n" );
	}

	Mem_Free( pRed );

	FS_Close( hFile );
}

void VID_TakeSnapshot( const char* pFilename )
{
	int width, height;

	if( bDoScaledFBO || VideoMode_IsWindowed() )
	{
		width = window_rect.width - window_rect.x;
		height = window_rect.height - window_rect.y;
	}
	else
	{
		width = vid.width;
		height = vid.height;
	}

	VID_TakeSnapshotRect( pFilename, 0, 0, width, height );
}

void VID_WriteBuffer( const char* pFilename )
{
	static char basefilename[ MAX_PATH ];
	static int filennumber = 1;

	char filename[ MAX_PATH ];

	if( pFilename )
	{
		Q_strncpy( basefilename, pFilename, ARRAYSIZE( basefilename ) - 1 );
		filename[ ARRAYSIZE( basefilename ) - 1 ] = '\0';
		filennumber = 1;
	}

	snprintf( filename, ARRAYSIZE( filename ) - 1, "%s%05d.bmp", basefilename, filennumber );
	filename[ ARRAYSIZE( filename ) - 1 ] = '\0';
	VID_TakeSnapshot( filename );
	++filennumber;
}