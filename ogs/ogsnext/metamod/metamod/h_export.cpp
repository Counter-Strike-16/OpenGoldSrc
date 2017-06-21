

// h_export.cpp - main exported DLL functionality

// From SDK dlls/h_export.cpp:

#include <extdll.h>			// always
#include <time.h>

#include "h_export.h"		// me
#include "metamod.h"		// engine_t, etc
#include "log_meta.h"		// META_DEV, etc
#include "osdep_p.h"		// get_module_handle_of_memptr

// From SDK dlls/h_export.cpp:

#ifdef _WIN32
	//! Required DLL entry point
	extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
	{
		if (fdwReason == DLL_PROCESS_ATTACH)
			metamod_handle = hinstDLL;
		
		return(TRUE);
	};
#endif