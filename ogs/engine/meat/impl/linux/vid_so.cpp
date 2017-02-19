// Main windowed and fullscreen graphics interface module. This module
// is used for both the software and OpenGL rendering versions of the
// Quake refresh engine.

#define SO_FILE "/etc/quake2.conf"

#include <assert.h>
#include <dlfcn.h> // ELF dl loader
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../client/client.hpp"

#include "../linux/rw_linux.hpp"

// Structure containing functions exported from refresh DLL
refexport_t re;

// Console variables that we need to access from this module
cvar_t *vid_gamma;
cvar_t *vid_ref;  // Name of Refresh DLL loaded
cvar_t *vid_xpos; // X coordinate of window position
cvar_t *vid_ypos; // Y coordinate of window position
cvar_t *vid_fullscreen;

// Global variables used internally by this module
viddef_t viddef;      // global video state; used by other modules
void *reflib_library; // Handle to refresh DLL
qboolean reflib_active = 0;

#define VID_NUM_MODES (sizeof(vid_modes) / sizeof(vid_modes[0]))

/** KEYBOARD **************************************************************/

void Do_Key_Event(int key, qboolean down);

void (*KBD_Update_fp)();
void (*KBD_Init_fp)(Key_Event_fp_t fp);
void (*KBD_Close_fp)();

/** MOUSE *****************************************************************/

in_state_t in_state;

void (*RW_IN_Init_fp)(in_state_t *in_state_p);
void (*RW_IN_Shutdown_fp)();
void (*RW_IN_Activate_fp)(qboolean active);
void (*RW_IN_Commands_fp)();
void (*RW_IN_Move_fp)(usercmd_t *cmd);
void (*RW_IN_Frame_fp)();

void Real_IN_Init();

/*
==========================================================================

DLL GLUE

==========================================================================
*/

#define MAXPRINTMSG 4096
void VID_Printf(int print_level, char *fmt, ...)
{
	va_list argptr;
	char msg[MAXPRINTMSG];
	static qboolean inupdate;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	if(print_level == PRINT_ALL)
		Com_Printf("%s", msg);
	else
		Com_DPrintf("%s", msg);
}

void VID_Error(int err_level, char *fmt, ...)
{
	va_list argptr;
	char msg[MAXPRINTMSG];
	static qboolean inupdate;

	va_start(argptr, fmt);
	vsprintf(msg, fmt, argptr);
	va_end(argptr);

	Com_Error(err_level, "%s", msg);
}

//==========================================================================







void VID_FreeReflib()
{
	if(reflib_library)
	{
		if(KBD_Close_fp)
			KBD_Close_fp();
		if(RW_IN_Shutdown_fp)
			RW_IN_Shutdown_fp();
		dlclose(reflib_library);
	}

	KBD_Init_fp = NULL;
	KBD_Update_fp = NULL;
	KBD_Close_fp = NULL;
	RW_IN_Init_fp = NULL;
	RW_IN_Shutdown_fp = NULL;
	RW_IN_Activate_fp = NULL;
	RW_IN_Commands_fp = NULL;
	RW_IN_Move_fp = NULL;
	RW_IN_Frame_fp = NULL;

	memset(&re, 0, sizeof(re));
	reflib_library = NULL;
	reflib_active = false;
}

/*
==============
VID_LoadRefresh
==============
*/
qboolean VID_LoadRefresh(char *name)
{
	refimport_t ri;
	GetRefAPI_t GetRefAPI;
	char fn[MAX_OSPATH];
	struct stat st;
	extern uid_t saved_euid;
	FILE *fp;

	if(reflib_active)
	{
		if(KBD_Close_fp)
			KBD_Close_fp();
		if(RW_IN_Shutdown_fp)
			RW_IN_Shutdown_fp();
		KBD_Close_fp = NULL;
		RW_IN_Shutdown_fp = NULL;
		re.Shutdown();
		VID_FreeReflib();
	}

	Com_Printf("------- Loading %s -------\n", name);

	// regain root
	seteuid(saved_euid);

	if((fp = fopen(SO_FILE, "r")) == NULL)
	{
		Com_Printf("LoadLibrary(\"%s\") failed: can't open " SO_FILE
		           " (required for location of ref libraries)\n",
		           name);
		return false;
	}
	fgets(fn, sizeof(fn), fp);
	fclose(fp);
	if(*fn && fn[strlen(fn) - 1] == '\n')
		fn[strlen(fn) - 1] = 0;

	strcat(fn, "/");
	strcat(fn, name);

	// permission checking
	if(strstr(fn, "softx") == NULL)
	{ // softx doesn't require root
		if(stat(fn, &st) == -1)
		{
			Com_Printf("LoadLibrary(\"%s\") failed: %s\n", name, strerror(errno));
			return false;
		}
		if(st.st_uid != 0)
		{
			Com_Printf("LoadLibrary(\"%s\") failed: ref is not owned by root\n",
			           name);
			return false;
		}
#if 0
		if ((st.st_mode & 0777) & ~0700) {
			Com_Printf( "LoadLibrary(\"%s\") failed: invalid permissions, must be 700 for security considerations\n", name);
			return false;
		}
#endif
	}
	else
	{
		// softx requires we give up root now
		setreuid(getuid(), getuid());
		setegid(getgid());
	}

	if((reflib_library = dlopen(fn, RTLD_NOW)) == 0)
	{
		Com_Printf("LoadLibrary(\"%s\") failed: %s\n", name, dlerror());
		return false;
	}

	ri.Cmd_AddCommand = Cmd_AddCommand;
	ri.Cmd_RemoveCommand = Cmd_RemoveCommand;
	ri.Cmd_Argc = Cmd_Argc;
	ri.Cmd_Argv = Cmd_Argv;
	ri.Cmd_ExecuteText = Cbuf_ExecuteText;
	ri.Con_Printf = VID_Printf;
	ri.Sys_Error = VID_Error;
	ri.FS_LoadFile = FS_LoadFile;
	ri.FS_FreeFile = FS_FreeFile;
	ri.FS_Gamedir = FS_Gamedir;
	ri.Cvar_Get = Cvar_Get;
	ri.Cvar_Set = Cvar_Set;
	ri.Cvar_SetValue = Cvar_SetValue;
	ri.Vid_GetModeInfo = VID_GetModeInfo;
	ri.Vid_MenuInit = VID_MenuInit;
	ri.Vid_NewWindow = VID_NewWindow;

	if((GetRefAPI = (void *)dlsym(reflib_library, "GetRefAPI")) == 0)
		Com_Error(ERR_FATAL, "dlsym failed on %s", name);

	re = GetRefAPI(ri);

	if(re.api_version != API_VERSION)
	{
		VID_FreeReflib();
		Com_Error(ERR_FATAL, "%s has incompatible api_version", name);
	}

	/* Init IN (Mouse) */
	in_state.IN_CenterView_fp = IN_CenterView;
	in_state.Key_Event_fp = Do_Key_Event;
	in_state.viewangles = cl.viewangles;
	in_state.in_strafe_state = &in_strafe.state;

	if((RW_IN_Init_fp = dlsym(reflib_library, "RW_IN_Init")) == NULL ||
	   (RW_IN_Shutdown_fp = dlsym(reflib_library, "RW_IN_Shutdown")) == NULL ||
	   (RW_IN_Activate_fp = dlsym(reflib_library, "RW_IN_Activate")) == NULL ||
	   (RW_IN_Commands_fp = dlsym(reflib_library, "RW_IN_Commands")) == NULL ||
	   (RW_IN_Move_fp = dlsym(reflib_library, "RW_IN_Move")) == NULL ||
	   (RW_IN_Frame_fp = dlsym(reflib_library, "RW_IN_Frame")) == NULL)
		Sys_Error("No RW_IN functions in REF.\n");

	Real_IN_Init();

	if(re.Init(0, 0) == -1)
	{
		re.Shutdown();
		VID_FreeReflib();
		return false;
	}

/* Init KBD */
#if 1
	if((KBD_Init_fp = dlsym(reflib_library, "KBD_Init")) == NULL ||
	   (KBD_Update_fp = dlsym(reflib_library, "KBD_Update")) == NULL ||
	   (KBD_Close_fp = dlsym(reflib_library, "KBD_Close")) == NULL)
		Sys_Error("No KBD functions in REF.\n");
#else
	{
		void KBD_Init();
		void KBD_Update();
		void KBD_Close();

		KBD_Init_fp = KBD_Init;
		KBD_Update_fp = KBD_Update;
		KBD_Close_fp = KBD_Close;
	}
#endif
	KBD_Init_fp(Do_Key_Event);

	// give up root now
	setreuid(getuid(), getuid());
	setegid(getgid());

	Com_Printf("------------------------------------\n");
	reflib_active = true;
	return true;
}

/*
============
VID_CheckChanges

This function gets called once just before drawing each frame, and it's sole
purpose in life
is to check to see if any of the video mode parameters have changed, and if they
have to
update the rendering DLL and/or video mode to match.
============
*/
void VID_CheckChanges()
{
	char name[100];
	cvar_t *sw_mode;

	if(vid_ref->modified)
	{
		S_StopAllSounds();
	}

	while(vid_ref->modified)
	{
		/*
    ** refresh has changed
    */
		vid_ref->modified = false;
		vid_fullscreen->modified = true;
		cl.refresh_prepped = false;
		cls.disable_screen = true;

		sprintf(name, "ref_%s.so", vid_ref->string);
		if(!VID_LoadRefresh(name))
		{
			if(strcmp(vid_ref->string, "soft") == 0 ||
			   strcmp(vid_ref->string, "softx") == 0)
			{
				Com_Printf("Refresh failed\n");
				sw_mode = Cvar_Get("sw_mode", "0", 0);
				if(sw_mode->value != 0)
				{
					Com_Printf("Trying mode 0\n");
					Cvar_SetValue("sw_mode", 0);
					if(!VID_LoadRefresh(name))
						Com_Error(ERR_FATAL, "Couldn't fall back to software refresh!");
				}
				else
					Com_Error(ERR_FATAL, "Couldn't fall back to software refresh!");
			}

			Cvar_Set("vid_ref", "soft");

			/*
      ** drop the console if we fail to load a refresh
      */
			if(cls.key_dest != key_console)
			{
				Con_ToggleConsole_f();
			}
		}
		cls.disable_screen = false;
	}
}

/*
============
VID_Init
============
*/
void VID_Init()
{
	/* Create the video variables so we know how to start the graphics drivers */
	// if DISPLAY is defined, try X
	if(getenv("DISPLAY"))
		vid_ref = Cvar_Get("vid_ref", "softx", CVAR_ARCHIVE);
	else
		vid_ref = Cvar_Get("vid_ref", "soft", CVAR_ARCHIVE);
	vid_xpos = Cvar_Get("vid_xpos", "3", CVAR_ARCHIVE);
	vid_ypos = Cvar_Get("vid_ypos", "22", CVAR_ARCHIVE);
	vid_fullscreen = Cvar_Get("vid_fullscreen", "0", CVAR_ARCHIVE);
	vid_gamma = Cvar_Get("vid_gamma", "1", CVAR_ARCHIVE);

	/* Add some console commands that we want to handle */
	Cmd_AddCommand("vid_restart", VID_Restart_f);

	/* Disable the 3Dfx splash screen */
	putenv("FX_GLIDE_NO_SPLASH=0");

	/* Start the graphics mode and load refresh DLL */
	VID_CheckChanges();
}

/*
============
VID_Shutdown
============
*/
void VID_Shutdown()
{
	if(reflib_active)
	{
		if(KBD_Close_fp)
			KBD_Close_fp();
		if(RW_IN_Shutdown_fp)
			RW_IN_Shutdown_fp();
		KBD_Close_fp = NULL;
		RW_IN_Shutdown_fp = NULL;
		re.Shutdown();
		VID_FreeReflib();
	}
}

/*****************************************************************************/
/* INPUT                                                                     */
/*****************************************************************************/

cvar_t *in_joystick;

// This if fake, it's acutally done by the Refresh load
void IN_Init()
{
	in_joystick = Cvar_Get("in_joystick", "0", CVAR_ARCHIVE);
}

void Real_IN_Init()
{
	if(RW_IN_Init_fp)
		RW_IN_Init_fp(&in_state);
}

void IN_Shutdown()
{
	if(RW_IN_Shutdown_fp)
		RW_IN_Shutdown_fp();
}

void IN_Commands()
{
	if(RW_IN_Commands_fp)
		RW_IN_Commands_fp();
}

void IN_Move(usercmd_t *cmd)
{
	if(RW_IN_Move_fp)
		RW_IN_Move_fp(cmd);
}

void IN_Frame()
{
	if(RW_IN_Frame_fp)
		RW_IN_Frame_fp();
}

void IN_Activate(qboolean active)
{
	if(RW_IN_Activate_fp)
		RW_IN_Activate_fp(active);
}

void Do_Key_Event(int key, qboolean down)
{
	Key_Event(key, down, Sys_Milliseconds());
}
