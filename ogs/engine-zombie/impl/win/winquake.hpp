
// winquake.h: Win32-specific Quake header file

#ifdef _WIN32 
#pragma warning( disable : 4229 )  // mgraph gets this

#include <windows.h>

#ifndef SERVERONLY

#include <ddraw.h>

#ifndef GLQUAKE
	#include <mgraph.h>
#endif

#endif

#define	WINDOW_STYLE	(WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_VISIBLE)

extern	HINSTANCE	global_hInstance;
extern	int			global_nCmdShow;

#ifndef SERVERONLY

extern LPDIRECTDRAW		lpDD;

extern qboolean			DDActive;

extern LPDIRECTDRAWSURFACE	lpPrimary;
extern LPDIRECTDRAWSURFACE	lpFrontBuffer;
extern LPDIRECTDRAWSURFACE	lpBackBuffer;

extern LPDIRECTDRAWPALETTE	lpDDPal;

#endif

typedef enum {MS_WINDOWED,
			MS_FULLSCREEN,
			MS_FULLDIB,
			MS_UNINIT} modestate_t;

extern modestate_t	modestate;

extern qboolean		ActiveApp, Minimized;

extern qboolean	WinNT;

extern cvar_t		_windowed_mouse;

extern HWND		hwnd_dialog;

extern HANDLE	hinput, houtput;


#endif