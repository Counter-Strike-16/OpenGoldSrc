
typedef struct
{
	int left;
	int right;
} portable_samplepair_t;

typedef struct
{
	int  length;
	int  loopstart;
	int  speed;
	int  width;
	int  stereo;
	byte data[1]; // variable sized
} sfxcache_t;

typedef struct
{
	qboolean       gamealive;
	qboolean       soundalive;
	qboolean       splitbuffer;
	int            channels;
	int            samples;          // mono samples in buffer
	int            submission_chunk; // don't mix less than this #
	int            samplepos;        // in mono samples
	int            samplebits;
	int            speed;
	unsigned char *buffer;
} dma_t;

typedef struct
{
	int rate;
	int width;
	int channels;
	int loopstart;
	int samples;
	int dataofs; // chunk starts this many bytes from file start
} wavinfo_t;

// picks a channel based on priorities, empty slots, number of channels
channel_t *SND_PickChannel(int entnum, int entchannel);

// spatializes a channel
void SND_Spatialize(channel_t *ch);

// initializes cycling through a DMA buffer and returns information on it
qboolean SNDDMA_Init(void);

// gets the current DMA position
int SNDDMA_GetDMAPos(void);

// shutdown the DMA xfer.
void SNDDMA_Shutdown(void);

// ====================================================================
// User-setable variables
// ====================================================================

#define MAX_CHANNELS 128
#define MAX_DYNAMIC_CHANNELS 8

extern channel_t channels[MAX_CHANNELS];
// 0 to MAX_DYNAMIC_CHANNELS-1	= normal entity sounds
// MAX_DYNAMIC_CHANNELS to MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS -1 = water, etc
// MAX_DYNAMIC_CHANNELS + NUM_AMBIENTS to total_channels = static sounds

extern int total_channels;

//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

extern qboolean        fakedma;
extern int             fakedma_updates;
extern int             paintedtime;
extern vec3_t          listener_origin;
extern vec3_t          listener_forward;
extern vec3_t          listener_right;
extern vec3_t          listener_up;
extern volatile dma_t *shm;
extern volatile dma_t  sn;
extern vec_t           sound_nominal_clip_dist;

extern cvar_t loadas8bit;
extern cvar_t bgmvolume;
extern cvar_t volume;

extern qboolean snd_initialized;

extern int snd_blocked;

wavinfo_t GetWavinfo(char *name, byte *wav, int wavlength);

void SND_InitScaletable(void);
void SNDDMA_Submit(void);

void S_AmbientOff(void);
void S_AmbientOn(void);