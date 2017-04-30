
//
// vregset.h: header file for video register-setting interpreter
//

//
// registers & subregisters
//
#define MISC_OUTPUT	0x3C2

#define SC_INDEX		0x3C4
#define SC_DATA			0x3C5
#define SYNC_RESET		0
#define MAP_MASK		2
#define MEMORY_MODE		4

#define GC_INDEX		0x3CE
#define GC_DATA			0x3CF
#define READ_MAP		4
#define GRAPHICS_MODE	5
#define MISCELLANOUS	6

#define CRTC_INDEX		0x3D4
#define CRTC_DATA		0x3D5
#define MAX_SCAN_LINE	9
#define UNDERLINE		0x14
#define	MODE_CONTROL	0x17

//
// register-set commands
//
#define VRS_END			0
#define VRS_BYTE_OUT	1
#define VRS_BYTE_RMW	2
#define VRS_WORD_OUT	3

void VideoRegisterSet (int *pregset);

