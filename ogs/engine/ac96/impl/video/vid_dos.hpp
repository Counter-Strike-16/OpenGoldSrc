
// vid_dos.h: header file for DOS-specific video stuff

typedef struct vmode_s {
	struct vmode_s	*pnext;
	char		*name;
	char		*header;
	unsigned	width;
	unsigned	height;
	float		aspect;
	unsigned	rowbytes;
	int			planar;
	int			numpages;
	void		*pextradata;
	int			(*setmode)(viddef_t *vid, struct vmode_s *pcurrentmode);
	void		(*swapbuffers)(viddef_t *vid, struct vmode_s *pcurrentmode,
							   vrect_t *rects);
	void		(*setpalette)(viddef_t *vid, struct vmode_s *pcurrentmode,
							  unsigned char *palette);
	void		(*begindirectrect)(viddef_t *vid, struct vmode_s *pcurrentmode,
								   int x, int y, byte *pbitmap, int width,
								   int height);
	void		(*enddirectrect)(viddef_t *vid, struct vmode_s *pcurrentmode,
								 int x, int y, int width, int height);
} vmode_t;

// vid_wait settings
#define VID_WAIT_NONE			0
#define VID_WAIT_VSYNC			1
#define VID_WAIT_DISPLAY_ENABLE	2

extern int		numvidmodes;
extern vmode_t	*pvidmodes;

extern int		VGA_width, VGA_height, VGA_rowbytes, VGA_bufferrowbytes;
extern byte		*VGA_pagebase;
extern vmode_t	*VGA_pcurmode;

extern cvar_t	vid_wait;
extern cvar_t	vid_nopageflip;
extern cvar_t	_vid_wait_override;

extern unsigned char colormap256[32][256];

extern void	*vid_surfcache;
extern int	vid_surfcachesize;

void VGA_Init (void);
void VID_InitVESA (void);
void VID_InitExtra (void);
void VGA_WaitVsync (void);
void VGA_ClearVideoMem (int planar);
void VGA_SetPalette(viddef_t *vid, vmode_t *pcurrentmode, unsigned char *pal);
void VGA_SwapBuffersCopy (viddef_t *vid, vmode_t *pcurrentmode,
	vrect_t *rects);
qboolean VGA_FreeAndAllocVidbuffer (viddef_t *vid, int allocnewbuffer);
qboolean VGA_CheckAdequateMem (int width, int height, int rowbytes,
	int allocnewbuffer);
void VGA_BeginDirectRect (viddef_t *vid, struct vmode_s *pcurrentmode, int x,
	int y, byte *pbitmap, int width, int height);
void VGA_EndDirectRect (viddef_t *vid, struct vmode_s *pcurrentmode, int x,
	int y, int width, int height);
void VGA_UpdateLinearScreen (void *srcptr, void *destptr, int width,
	int height, int srcrowbytes, int destrowbytes);

