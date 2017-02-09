
/// @brief video driver defs

#include "maintypes.h"
#include "common/commontypes.h"
#include "common/wrect.h"

#define VID_CBITS 6
#define VID_GRADES (1 << VID_CBITS)

#ifdef HOOK_ENGINE
#define r_pixbytes (*pr_pixbytes)
#define gl_vsync (*pgl_vsync)
#define scr_con_current (*pscr_con_current)
#endif //HOOK_ENGINE

// a pixel can be one, two, or four bytes
typedef byte pixel_t;

typedef struct
{
	pixel_t *buffer;            // invisible buffer
	pixel_t *colormap;          // 256 * VID_GRADES size
	unsigned short *colormap16; // 256 * VID_GRADES size
	int fullbright;             // index of first fullbright color
	unsigned rowbytes;          // may be > width if displayed in a window
	unsigned width;
	unsigned height;
	float aspect; // width / height -- < 0 is taller than wide
	int numpages;
	int recalc_refdef; // if true, recalc vid-based stuff
	pixel_t *conbuffer;
	int conrowbytes;
	unsigned conwidth;
	unsigned conheight;
	int maxwarpwidth;
	int maxwarpheight;
	pixel_t *direct; // direct drawing to framebuffer, if not
	                 //  NULL
} viddef_t;

extern viddef_t vid; // global video state

extern unsigned short d_8to16table[256];
extern unsigned d_8to24table[256];

extern void (*vid_menudrawfn)();
extern void (*vid_menukeyfn)(int key);

void VID_SetPalette(unsigned char *palette);
// called at startup and after any gamma correction

void VID_ShiftPalette(unsigned char *palette);
// called for bonus and pain flashes, and for underwater color changes

void VID_Init(unsigned char *palette);
// Called at startup to set up translation tables, takes 256 8 bit RGB values
// the palette data will go away after the call, so it must be copied off if
// the video driver will need it again

void VID_Shutdown();
// Called at shutdown

void VID_Update(wrect_t *rects); // was vrect_t
// flushes the given rectangles from the view buffer to the screen

int VID_SetMode(int modenum, unsigned char *palette);
// sets the mode; only used by the Quake engine for resetting to mode 0 (the
// base mode) on memory allocation failures

void VID_HandlePause(qboolean pause);
// called only on Win32, when pause happens, so the mouse can be released

void VID_LockBuffer();
void VID_UnlockBuffer();

#ifdef GLQUAKE
qboolean VID_Is8bit();
#endif
