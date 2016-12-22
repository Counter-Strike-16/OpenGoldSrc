


#define MAX_QPATH 64    // Must match value in quakedefs.h
#define MAX_RESOURCE_LIST	1280

/////////////////
// Customization
// passed to pfnPlayerCustomization
// For automatic downloading.
typedef enum
{
	t_sound = 0,
	t_skin,
	t_model,
	t_decal,
	t_generic,
	t_eventscript,
	t_world,		// Fake type for world, is really t_model
	rt_unk,

	rt_max
} resourcetype_t;

typedef struct resourceinfo_s
{
	_resourceinfo_t info[ rt_max ];
} resourceinfo_t;

#define RES_FATALIFMISSING (1<<0)   // Disconnect if we can't get this file.
#define RES_WASMISSING     (1<<1)   // Do we have the file locally, did we get it ok?
#define RES_CUSTOM         (1<<2)   // Is this resource one that corresponds to another player's customization
								    // or is it a server startup resource.
#define RES_REQUESTED	   (1<<3)	// Already requested a download of this one
#define RES_PRECACHED	   (1<<4)	// Already precached
#define RES_ALWAYS		   (1<<5)	// download always even if available on client
#define RES_UNK_6          (1<<6)   // TODO: what is it?
#define RES_CHECKFILE	   (1<<7)	// check file on client


typedef struct resource_s
{
	char              szFileName[MAX_QPATH]; // File name to download/precache.
	resourcetype_t    type;                // t_sound, t_skin, t_model, t_decal.
	int               nIndex;              // For t_decals
	int               nDownloadSize;       // Size in Bytes if this must be downloaded.
	unsigned char     ucFlags;

// For handling client to client resource propagation
	unsigned char     rgucMD5_hash[16];    // To determine if we already have it.
	unsigned char     playernum;           // Which player index this resource is associated with, if it's a custom resource.

	unsigned char	  rguc_reserved[ 32 ]; // For future expansion
	struct resource_s *pNext;              // Next in chain.
	struct resource_s *pPrev;
} resource_t;

