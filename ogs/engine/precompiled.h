#pragma once

//#include "version/appversion.h"

#include "public/rehlds/osconfig.h"

#include "public/archtypes.h"
//#include "asmlib.h"
#include "math/sse_mathfun.h"
//#include "mathlib.h"

//#include "sys_shared.h"
//#include "crc32c.h"
//#include "static_map.h"

//#include "ed_strpool.h"

#include "memory.h"
//#include "engine.h"
#include "public/tier0/platform.h"
//#include "RehldsRuntimeConfig.h"
//#include "rehlds_debug.h"

#ifdef HOOK_ENGINE
#include "hooker.h"
#endif

// valve libs stuff
#include "tier0/platform.h"
#include "dbg/dbg.h"

#include "public/interface.h"
#include "public/iregistry.h"

#include "public/utlbuffer.h"
#include "public/utlrbtree.h"

#include "public/iosutil.h"

// testsuite
/*
#include "testsuite.h"
#include "funccalls.h"
#include "recorder.h"
#include "player.h"
#include "anonymizer.h"
*/

//#include "bzip2/bzlib.h"
#include "system/igame.h"
#include "system/sys_linuxwnd.h"

#include "system/iengine.h"
/*
#include "hookchains_impl.h"
#include "rehlds_interfaces.h"
#include "rehlds_interfaces_impl.h"
#include "rehlds_api.h"
#include "rehlds_api_impl.h"
#include "FlightRecorderImpl.h"
#include "flight_recorder.h"
#include "rehlds_security.h"
*/

#include "dlls/cdll_dll.h"