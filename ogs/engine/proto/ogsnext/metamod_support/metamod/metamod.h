#ifndef METAMOD_H
#define METAMOD_H

// Hook function tables
extern DLLHIDDEN DLL_FUNCTIONS *g_pHookedDllFunctions;
extern DLLHIDDEN NEW_DLL_FUNCTIONS *g_pHookedNewDllFunctions;

// ===== lotsa macros... ======================================================

// These are the meat of the metamod processing, and are as ugly as (or
// uglier) than they look.  This is done via macros, because of the varying
// parameter types (int, void, edict_t*, etc) as well as varying
// function-pointer types and different api tables (dllapi, newapi,
// engine), which just can't be passed to a function.  And, since the
// operation is similar for each api call, I didn't want to keep
// duplicating code all over the place.  Thus the ugly macros.
//
// The basic operation is, for each api call:
//  - iterate through list of plugins
//  - for each plugin, if it provides this api call, then call the
//    function in the plugin
//  - call the "real" function (in the game dll, or from the engine)
//  - for each plugin, check for a "post" version of the function, and call
//    if present
//
//
// Also, for any api call, each plugin has the opportunity to replace the
// real routine, in two ways:
//  - prevent the real routine from being called ("supercede")
//  - allow the real routine to be called, but change the value that's
//    returned ("override")
//
// Thus after each plugin is called, its META_RETURN flag is checked, and
// action taken appropriately.  Note that supercede/override only affects
// the _real_ routine; other plugins will still be called.
//
// In addition to the SUPERCEDE and OVERRIDE flags, there are two
// additional flags a plugin can return:
//  - HANDLED ("I did something here")
//  - IGNORED ("I didn't really do anything")
//
// These aren't used by metamod itself, but could be used by plugins to
// get an idea if a previous plugin did anything.
//
//
// The 5 basic macros are:
//   SETUP
//   CALL_PLUGIN
//   CALL_GAME  and  CALL_ENGINE
//   RETURN
//
// These 5 are actually used to build second level macros for each api type
// (dllapi, newapi, engine), with the CALL_PLUGIN macro being used twice
// (before and after).  Altogether, they end up expanding to approx 150
// lines of code for _each_ api call.  Ack, ugly indeed.
//
// However, due to some functions returning 'void', and others returning an
// actual value, I had to have separate macros for the two, since I
// couldn't seem to generalize the two into a form that the compiler would
// accept.  Thus there are "_void" versions of the 5 macros; these are
// listed first.

// ===== macros for void-returning functions ==================================

// return (void) WTF
#define RETURN_API_void() \
	return;

// ===== macros for type-returning functions ==================================

// return a value
#define RETURN_API(ret_t) \
	{return (GET_RET_CLASS(ret_val, ret_t));}

// ===== end macros ===========================================================

#ifdef META_PERFMON

// ============================================================================
// Api-hook performance monitoring
// ============================================================================

extern long double total_tsc DLLHIDDEN;
extern unsigned long long count_tsc DLLHIDDEN;
extern unsigned long long active_tsc DLLHIDDEN;
extern unsigned long long min_tsc DLLHIDDEN;

inline unsigned long long DLLINTERNAL GET_TSC(void) {
	union { struct { unsigned int eax, edx;	} split; unsigned long long full; } tsc;
#ifdef __GNUC__
	__asm__ __volatile__("rdtsc":"=a"(tsc.split.eax), "=d"(tsc.split.edx));	
#else
	__asm
	{
		rdtsc
		mov tsc.split.eax, eax
		mov tsc.split.edx, edx
	}
#endif
	return(tsc.full);
}

#define API_START_TSC_TRACKING() \
	active_tsc = GET_TSC()

#define API_PAUSE_TSC_TRACKING() \
	total_tsc += GET_TSC() - active_tsc

#define API_UNPAUSE_TSC_TRACKING() \
	active_tsc = GET_TSC()

#define API_END_TSC_TRACKING() { \
		unsigned long long run_tsc = GET_TSC() - active_tsc; \
		total_tsc += run_tsc; \
		count_tsc++; \
		if(min_tsc == 0 || run_tsc < min_tsc) \
			min_tsc = run_tsc; \
	}

// ===== end ==================================================================

#else

// ===== performance monitor disabled =========================================

#define API_START_TSC_TRACKING()
#define API_PAUSE_TSC_TRACKING()
#define API_UNPAUSE_TSC_TRACKING()
#define API_END_TSC_TRACKING()

// ===== end ==================================================================

#endif /*META_PERFMON*/

#endif /* METAMOD_H */