
typedef struct
{
	char		name[16];
	qboolean	failedload;		// the name isn't a valid skin
	cache_user_t	cache;
} skin_t;

// player_state_t is the information needed by a player entity
// to do move prediction and to generate a drawable entity
typedef struct
{
	int			messagenum;		// all player's won't be updated each frame

	double		state_time;		// not the same as the packet time,
								// because player commands come asyncronously
	usercmd_t	command;		// last command for prediction

	vec3_t		origin;
	vec3_t		viewangles;		// only for demos, not from server
	vec3_t		velocity;
	int			weaponframe;

	int			modelindex;
	int			frame;
	int			skinnum;
	int			effects;

	int			flags;			// dead, gib, etc

	float		waterjumptime;
	int			onground;		// -1 = in air, else pmove entity number
	int			oldbuttons;
} player_state_t;

typedef struct player_info_s
{
	float	entertime;
	int		frags;
	byte	pl;

	int		_topcolor;
	int		_bottomcolor;

	byte	translations[VID_GRADES*256];
	skin_t	*skin;
} player_info_t;

typedef struct
{
	// generated on client side
	usercmd_t	cmd;		// cmd that generated the frame
	double		senttime;	// time cmd was sent off
	int			delta_sequence;		// sequence number to delta from, -1 = full update

	// received from server
	double		receivedtime;	// time message was received, or -1
	player_state_t	playerstate[MAX_CLIENTS];	// message received that reflects performing
							// the usercmd
	packet_entities_t	packet_entities;
	qboolean	invalid;		// true if the packet_entities delta was invalid
} frame_t;

typedef struct
{
	int		destcolor[3];
	int		percent;		// 0-256
} cshift_t;

#define	CSHIFT_CONTENTS	0
#define	CSHIFT_DAMAGE	1
#define	CSHIFT_BONUS	2
#define	CSHIFT_POWERUP	3
#define	NUM_CSHIFTS		4


//
// client_state_t should hold all pieces of the client state
//

typedef struct
{
	int		key;				// so entities can reuse same entry
} dlight_t;

typedef enum
{
	dl_none,
	dl_model,
	dl_sound,
	dl_skin,
	dl_single
} dltype_t;		// download type

typedef struct
{
	int			qport;

	FILE		*download;		// file transfer from server
	char		downloadtempname[MAX_OSPATH];
	char		downloadname[MAX_OSPATH];
	int			downloadnumber;
	dltype_t	downloadtype;
	int			downloadpercent;

// demo recording info must be here, because record is started before
// entering a map (and clearing client_state_t)
	qboolean	demorecording;

	float		latency;		// rolling average
} client_static_t;


typedef struct
{	
	int			movemessages;	// since connecting to this server
								// throw out the first couple, so the player
								// doesn't accidentally do something the 
								// first frame

	int			spectator;

	double		last_ping_request;	// while showing scoreboard
	double		last_servermessage;
	
	float		item_gettime[32];	// cl.time of aquiring item, for blinking
	float		faceanimtime;		// use anim frame if cl.time < this

	cshift_t	cshifts[NUM_CSHIFTS];	// color shifts for damage, powerups
	cshift_t	prev_cshifts[NUM_CSHIFTS];	// and content types
		
// pitch drifting vars
	float		pitchvel;
	qboolean	nodrift;
	float		driftmove;
	double		laststop;

	float		crouch;			// local amount for smoothing stepups		
		
	int			completed_time;	// latched ffrom time at intermission start
	
//
// information that is static for the entire time connected to a server
//
	char		model_name[MAX_MODELS][MAX_QPATH];
	char		sound_name[MAX_SOUNDS][MAX_QPATH];

	struct model_s		*model_precache[MAX_MODELS];
	struct sfx_s		*sound_precache[MAX_SOUNDS];	

	int			cdtrack;		// cd audio
} client_state_t;


//
// cvars
//
extern  cvar_t	cl_warncmd;

extern	cvar_t	cl_shownet;
extern	cvar_t	cl_sbar;
extern	cvar_t	cl_hudswap;

extern	cvar_t	cl_pitchdriftspeed;

extern cvar_t		_windowed_mouse;

extern	cvar_t	name;




extern	qboolean	nomaster;
extern float	server_version;	// version of server we connected to

//=============================================================================

void CL_NextDemo ();
qboolean CL_DemoBehind();



extern char emodel_name[], pmodel_name[], prespawn_name[], modellist_name[], soundlist_name[];

//
// cl_parse.c
//
#define NET_TIMINGS 256
#define NET_TIMINGSMASK 255
extern int	packet_latency[NET_TIMINGS];
int CL_CalcNet ();
qboolean	CL_CheckOrDownloadFile (char *filename);
qboolean CL_IsUploading();
void CL_NextUpload();
void CL_StartUpload (byte *data, int size);
void CL_StopUpload();

//
// cl_ents.c
//
void CL_SetUpPlayerPrediction(qboolean dopred);
void CL_ClearProjectiles ();
void CL_ParseProjectiles ();
void CL_ParsePacketEntities (qboolean delta);

void CL_ParsePlayerinfo ();


//
// cl_cam.c
//

extern	int		autocam;
extern int spec_track; // player# of who we are tracking

void CL_InitCam();

//
// skin.c
//

typedef struct
{
    char	manufacturer;
    char	version;
    char	encoding;
    char	bits_per_pixel;
    unsigned short	xmin,ymin,xmax,ymax;
    unsigned short	hres,vres;
    unsigned char	palette[48];
    char	reserved;
    char	color_planes;
    unsigned short	bytes_per_line;
    unsigned short	palette_type;
    char	filler[58];
    unsigned char	data;			// unbounded
} pcx_t;


void	Skin_Find (player_info_t *sc);
byte	*Skin_Cache (skin_t *skin);
void	Skin_Skins_f ();
void	Skin_AllSkins_f ();
void	Skin_NextDownload ();

#define RSSHOT_WIDTH 320
#define RSSHOT_HEIGHT 200


// q2


//define	PARANOID			// speed sapping error checking

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "ref.h"

#include "vid.h"
#include "screen.h"
#include "sound.h"
#include "input.h"
#include "keys.h"
#include "console.h"
#include "cdaudio.h"

//=============================================================================

typedef struct
{
	qboolean		valid;			// cleared if delta parsing was invalid
	int				serverframe;
	int				servertime;		// server time the message is valid for (in msec)
	int				deltaframe;
	byte			areabits[MAX_MAP_AREAS/8];		// portalarea visibility bits
	player_state_t	playerstate;
	int				num_entities;
	int				parse_entities;	// non-masked index into cl_parse_entities array
} frame_t;

typedef struct
{
	entity_state_t	baseline;		// delta from this if not from a previous frame
	entity_state_t	current;
	entity_state_t	prev;			// will always be valid, but might just be a copy of current

	int			serverframe;		// if not current, this ent isn't in the frame

	int			trailcount;			// for diminishing grenade trails
	vec3_t		lerp_origin;		// for trails (variable hz)

	int			fly_stoptime;
} centity_t;

#define MAX_CLIENTWEAPONMODELS		20		// PGM -- upped from 16 to fit the chainfist vwep

typedef struct
{
	char	name[MAX_QPATH];
	char	cinfo[MAX_QPATH];
	struct image_s	*skin;
	struct image_s	*icon;
	char	iconname[MAX_QPATH];
	struct model_s	*model;
	struct model_s	*weaponmodel[MAX_CLIENTWEAPONMODELS];
} clientinfo_t;

extern char cl_weaponmodels[MAX_CLIENTWEAPONMODELS][MAX_QPATH];
extern int num_cl_weaponmodels;

#define	CMD_BACKUP		64	// allow a lot of command backups for very fast systems

//
// the client_state_t structure is wiped completely at every
// server map change
//
typedef struct
{
	int			timeoutcount;

	int			timedemo_frames;
	int			timedemo_start;

	qboolean	refresh_prepped;	// false if on new level or new ref dll
	qboolean	sound_prepped;		// ambient sounds can start
	qboolean	force_refdef;		// vid has changed, so we can't use a paused refdef

	int			parse_entities;		// index (not anded off) into cl_parse_entities[]

	usercmd_t	cmd;
	usercmd_t	cmds[CMD_BACKUP];	// each mesage will send several old cmds
	int			cmd_time[CMD_BACKUP];	// time sent, for calculating pings
	short		predicted_origins[CMD_BACKUP][3];	// for debug comparing against server

	float		predicted_step;				// for stair up smoothing
	unsigned	predicted_step_time;

	vec3_t		predicted_origin;	// generated by CL_PredictMovement
	vec3_t		predicted_angles;
	vec3_t		prediction_error;

	frame_t		frame;				// received from server
	int			surpressCount;		// number of messages rate supressed
	frame_t		frames[UPDATE_BACKUP];

	// the client maintains its own idea of view angles, which are
	// sent to the server each frame.  It is cleared to 0 upon entering each level.
	// the server sends a delta each frame which is added to the locally
	// tracked view angles to account for standing on rotating objects,
	// and teleport direction changes
	vec3_t		viewangles;

	int			time;			// this is the time value that the client
								// is rendering at.  always <= cls.realtime
	float		lerpfrac;		// between oldframe and frame

	refdef_t	refdef;

	vec3_t		v_forward, v_right, v_up;	// set when refdef.angles is set

	//
	// transient data from server
	//
	char		layout[1024];		// general 2D overlay
	int			inventory[MAX_ITEMS];

	//
	// non-gameserver infornamtion
	// FIXME: move this cinematic stuff into the cin_t structure
	FILE		*cinematic_file;
	int			cinematictime;		// cls.realtime for first cinematic frame
	int			cinematicframe;
	char		cinematicpalette[768];
	qboolean	cinematicpalette_active;

	//
	// server state information
	//
	qboolean	attractloop;		// running the attract loop, any key will menu
	int			servercount;	// server identification for prespawns
	char		gamedir[MAX_QPATH];
	int			playernum;

	char		configstrings[MAX_CONFIGSTRINGS][MAX_QPATH];

	//
	// locally derived information from server state
	//
	struct model_s	*model_draw[MAX_MODELS];
	struct cmodel_s	*model_clip[MAX_MODELS];

	struct sfx_s	*sound_precache[MAX_SOUNDS];
	struct image_s	*image_precache[MAX_IMAGES];

	clientinfo_t	clientinfo[MAX_CLIENTS];
	clientinfo_t	baseclientinfo;
} client_state_t;

typedef struct
{
	keydest_t	key_dest;

	int			framecount;
	int			realtime;			// always increasing, no clamping, etc
	float		frametime;			// seconds since last frame

// screen rendering information
	float		disable_screen;		// showing loading plaque between levels
									// or changing rendering dlls
									// if time gets > 30 seconds ahead, break it
	int			disable_servercount;	// when we receive a frame and cl.servercount
									// > cls.disable_servercount, clear disable_screen	

	int			quakePort;			// a 16 bit value that allows quake servers
									// to work around address translating routers
	int			serverProtocol;		// in case we are doing some kind of version hack		

	FILE		*download;			// file transfer from server
	char		downloadtempname[MAX_OSPATH];
	char		downloadname[MAX_OSPATH];
	int			downloadnumber;
	dltype_t	downloadtype;
	int			downloadpercent;	
} client_static_t;

//=============================================================================

//
// cvars
//
extern	cvar_t	*cl_stereo_separation;
extern	cvar_t	*cl_stereo;

extern	cvar_t	*cl_add_blend;
extern	cvar_t	*cl_add_lights;
extern	cvar_t	*cl_add_particles;
extern	cvar_t	*cl_add_entities;
extern	cvar_t	*cl_predict;

extern	cvar_t	*cl_anglespeedkey;

extern	cvar_t	*cl_shownet;
extern	cvar_t	*cl_showmiss;
extern	cvar_t	*cl_showclamp;

extern	cvar_t	*freelook;

extern	cvar_t	*cl_paused;
extern	cvar_t	*cl_timedemo;

// the cl_parse_entities must be large enough to hold UPDATE_BACKUP frames of
// entities, so that when a delta compressed message arives from the server
// it can be un-deltad from the original 
#define	MAX_PARSE_ENTITIES	1024
extern	entity_state_t	cl_parse_entities[MAX_PARSE_ENTITIES];

//=============================================================================

extern	netadr_t	net_from;
extern	sizebuf_t	net_message;

void DrawString (int x, int y, char *s);
void DrawAltString (int x, int y, char *s);	// toggle high bit
qboolean	CL_CheckOrDownloadFile (char *filename);

void CL_AddNetgraph ();

//ROGUE
typedef struct cl_sustain
{
	int			id;
	int			type;
	int			endtime;
	int			nextthink;
	int			thinkinterval;
	vec3_t		org;
	vec3_t		dir;
	int			color;
	int			count;
	int			magnitude;
	void		(*think)(struct cl_sustain *self);
} cl_sustain_t;

#define MAX_SUSTAINS		32
void CL_ParticleSteamEffect2(cl_sustain_t *self);

void CL_TeleporterParticles (entity_state_t *ent);
void CL_ParticleEffect (vec3_t org, vec3_t dir, int color, int count);
void CL_ParticleEffect2 (vec3_t org, vec3_t dir, int color, int count);

// RAFAEL
void CL_ParticleEffect3 (vec3_t org, vec3_t dir, int color, int count);


//=================================================

typedef struct particle_s
{
	float		time;

	vec3_t		accel;
	float		color;
	float		colorvel;
	float		alpha;
	float		alphavel;
} cparticle_t;


#define	PARTICLE_GRAVITY	40
#define BLASTER_PARTICLE_COLOR		0xe0

#define INSTANT_PARTICLE	-10000.0

void CL_ClearEffects ();
void CL_ClearTEnts ();
void CL_BlasterTrail (vec3_t start, vec3_t end);
void CL_QuadTrail (vec3_t start, vec3_t end);
void CL_RailTrail (vec3_t start, vec3_t end);
void CL_BubbleTrail (vec3_t start, vec3_t end);
void CL_FlagTrail (vec3_t start, vec3_t end, float color);

// RAFAEL
void CL_IonripperTrail (vec3_t start, vec3_t end);

// ========
// PGM
void CL_BlasterParticles2 (vec3_t org, vec3_t dir, unsigned int color);
void CL_BlasterTrail2 (vec3_t start, vec3_t end);
void CL_DebugTrail (vec3_t start, vec3_t end);
void CL_SmokeTrail (vec3_t start, vec3_t end, int colorStart, int colorRun, int spacing);
void CL_Flashlight (int ent, vec3_t pos);
void CL_ForceWall (vec3_t start, vec3_t end, int color);
void CL_FlameEffects (centity_t *ent, vec3_t origin);
void CL_GenericParticleEffect (vec3_t org, vec3_t dir, int color, int count, int numcolors, int dirspread, float alphavel);
void CL_BubbleTrail2 (vec3_t start, vec3_t end, int dist);
void CL_Heatbeam (vec3_t start, vec3_t end);
void CL_ParticleSteamEffect (vec3_t org, vec3_t dir, int color, int count, int magnitude);
void CL_TrackerTrail (vec3_t start, vec3_t end, int particleColor);
void CL_Tracker_Explode(vec3_t origin);
void CL_TagTrail (vec3_t start, vec3_t end, float color);
void CL_ColorFlash (vec3_t pos, int ent, int intensity, float r, float g, float b);
void CL_Tracker_Shell(vec3_t origin);
void CL_MonsterPlasma_Shell(vec3_t origin);
void CL_ColorExplosionParticles (vec3_t org, int color, int run);
void CL_ParticleSmokeEffect (vec3_t org, vec3_t dir, int color, int count, int magnitude);
void CL_Widowbeamout (cl_sustain_t *self);
void CL_Nukeblast (cl_sustain_t *self);
void CL_WidowSplash (vec3_t org);
// PGM
// ========

int CL_ParseEntityBits (unsigned *bits);
void CL_ParseDelta (entity_state_t *from, entity_state_t *to, int number, int bits);
void CL_ParseFrame ();

void CL_ParseTEnt ();
void CL_ParseConfigString ();
void CL_ParseMuzzleFlash ();
void CL_ParseMuzzleFlash2 ();

void CL_AddEntities ();
void CL_AddDLights ();
void CL_AddTEnts ();
void CL_AddLightStyles ();

//=================================================

void CL_PrepRefresh ();
void CL_RegisterSounds ();

void CL_Quit_f ();

void IN_Accumulate ();

void CL_ParseLayout ();


//
// cl_main
//

void CL_GetChallengePacket ();
void CL_PingServers_f ();
void CL_Snd_Restart_f ();
void CL_RequestNextDownload ();

//
// cl_input
//
typedef struct
{
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame
} kbutton_t;

extern	kbutton_t in_klook;

void CL_InitInput ();

void CL_ClearState ();

void CL_BaseMove (usercmd_t *cmd);

void IN_CenterView ();

float CL_KeyState (kbutton_t *key);

//
// cl_demo.c
//
void CL_WriteDemoMessage ();

//
// cl_parse.c
//
extern	char *svc_strings[256];

void CL_LoadClientinfo (clientinfo_t *ci, char *s);
void SHOWNET(char *s);
void CL_ParseClientinfo (int player);
void CL_Download_f ();

//
// cl_view.c
//
extern	int			gun_frame;
extern	struct model_s	*gun_model;

void V_RenderView( float stereo_separation );
void V_AddEntity (entity_t *ent);

void V_AddLightStyle (int style, float r, float g, float b);

//
// cl_tent.c
//
void CL_RegisterTEntSounds ();
void CL_RegisterTEntModels ();

//
// cl_inv.c
//
void CL_ParseInventory ();
void CL_KeyInventory (int key);
void CL_DrawInventory ();

#if id386
void x86_TimerStart( void );
void x86_TimerStop( void );
void x86_TimerInit( unsigned long smallest, unsigned longest );
unsigned long *x86_TimerGetHistogram( void );
#endif


// netq



typedef struct
{
	char	name[MAX_SCOREBOARDNAME];
	float	entertime;
	int		frags;
	int		colors;			// two 4 bit fields
	byte	translations[VID_GRADES*256];
} scoreboard_t;

typedef struct
{
	int		destcolor[3];
	int		percent;		// 0-256
} cshift_t;

#define	CSHIFT_CONTENTS	0
#define	CSHIFT_DAMAGE	1
#define	CSHIFT_BONUS	2
#define	CSHIFT_POWERUP	3
#define	NUM_CSHIFTS		4

#define	NAME_LENGTH	64


//
// client_state_t should hold all pieces of the client state
//

#define	SIGNONS		4			// signon messages to receive before connected

typedef struct
{
	int		entity;
	struct model_s	*model;
	float	endtime;
	vec3_t	start, end;
} beam_t;

#define	MAX_MAPSTRING	2048

typedef enum {
ca_dedicated, 		// a dedicated server with no ability to start a client
ca_disconnected, 	// full screen console with no connection
ca_connected		// valid netcon, talking to a server
} cactive_t;

typedef struct
{
	int			movemessages;	// since connecting to this server
								// throw out the first couple, so the player
								// doesn't accidentally do something the 
								// first frame
	usercmd_t	cmd;			// last command sent to the server

// information for local display
	int			stats[MAX_CL_STATS];	// health, etc
	int			items;			// inventory bit flags
	float	item_gettime[32];	// cl.time of aquiring item, for blinking
	float		faceanimtime;	// use anim frame if cl.time < this

	cshift_t	cshifts[NUM_CSHIFTS];	// color shifts for damage, powerups
	cshift_t	prev_cshifts[NUM_CSHIFTS];	// and content types

// the client maintains its own idea of view angles, which are
// sent to the server each frame.  The server sets punchangle when
// the view is temporarliy offset, and an angle reset commands at the start
// of each level and after teleporting.
	vec3_t		mviewangles[2];	// during demo playback viewangles is lerped
								// between these
	vec3_t		viewangles;
	
	vec3_t		mvelocity[2];	// update by server, used for lean+bob
								// (0 is newest)
	vec3_t		velocity;		// lerped between mvelocity[0] and [1]

	vec3_t		punchangle;		// temporary offset
	
// pitch drifting vars
	float		idealpitch;
	float		pitchvel;
	qboolean	nodrift;
	float		driftmove;
	double		laststop;

	float		viewheight;
	float		crouch;			// local amount for smoothing stepups

	qboolean	paused;			// send over by server
	qboolean	onground;
	qboolean	inwater;
	
	int			intermission;	// don't change view angle, full screen, etc
	int			completed_time;	// latched at intermission start
	
	double		mtime[2];		// the timestamp of last two messages	
	double		time;			// clients view of time, should be between
								// servertime and oldservertime to generate
								// a lerp point for other data
	double		oldtime;		// previous cl.time, time-oldtime is used
								// to decay light values and smooth step ups
	

	float		last_received_message;	// (realtime) for net trouble icon

//
// information that is static for the entire time connected to a server
//
	struct model_s		*model_precache[MAX_MODELS];
	struct sfx_s		*sound_precache[MAX_SOUNDS];

	char		levelname[40];	// for display on solo scoreboard
	int			viewentity;		// cl_entitites[cl.viewentity] = player
	int			maxclients;
	int			gametype;

// refresh related state
	struct model_s	*worldmodel;	// cl_entitites[0].model
	struct efrag_s	*free_efrags;
	int			num_entities;	// held in cl_entities array
	int			num_statics;	// held in cl_staticentities array
	entity_t	viewent;			// the gun model

	int			cdtrack, looptrack;	// cd audio

// frag scoreboard
	scoreboard_t	*scores;		// [cl.maxclients]

#ifdef QUAKE2
// light level at player's position including dlights
// this is sent back to the server each frame
// architectually ugly but it works
	int			light_level;
#endif
} client_state_t;


//
// cvars
//
extern	cvar_t	cl_color;

extern	cvar_t	cl_autofire;

extern	cvar_t	cl_shownet;
extern	cvar_t	cl_nolerp;

extern	cvar_t	cl_pitchdriftspeed;

//=============================================================================

void CL_NextDemo ();

extern	entity_t		*cl_visedicts[MAX_VISEDICTS];

//
// cl_input
//


extern	kbutton_t	in_klook;

void CL_InitInput ();

void CL_ParseTEnt ();

void CL_ClearState ();

void CL_BaseMove(usercmd_t *cmd);

float CL_KeyState (kbutton_t *key);

void CL_Stop_f ();
void CL_Record_f ();
void CL_PlayDemo_f ();
void CL_TimeDemo_f ();

void CL_NewTranslation (int slot);

void CL_SignonReply ();
