

extern netadr_t net_local_adr;
extern netadr_t net_from; // address of who sent the packet
extern sizebuf_t net_message;

extern cvar_t hostname;

extern int net_socket;

qboolean NET_GetPacket(void);
void NET_SendPacket(int length, void *data, netadr_t to);

//============================================================================

#define OLD_AVG 0.99 // total = oldtotal*OLD_AVG + new*(1-OLD_AVG)

typedef struct
{
	qboolean fatal_error;

	// the statistics are cleared at each client begin, because
	// the server connecting process gives a bogus picture of the data
	float frame_latency; // rolling average
	float frame_rate;

	int drop_count; // dropped packets, cleared each level
	int good_count; // cleared each level

	int qport;

	// bandwidth estimator
	double cleartime; // if realtime > nc->cleartime, free to go
	double rate;      // seconds / byte

	int incoming_reliable_acknowledged; // single bit

	int incoming_reliable_sequence; // single bit, maintained local

	// reliable staging and holding areas
	sizebuf_t message; // writing buffer to send to server

	// time and size data to calculate bandwidth
	int outgoing_size[MAX_LATENT];
	double outgoing_time[MAX_LATENT];
} netchan_t;

void Netchan_Transmit(netchan_t *chan, int length, byte *data);
void Netchan_OutOfBand(netadr_t adr, int length, byte *data);
void Netchan_OutOfBandPrint(netadr_t adr, char *format, ...);
qboolean Netchan_Process(netchan_t *chan);
void Netchan_Setup(netchan_t *chan, netadr_t adr, int qport);

qboolean Netchan_CanReliable(netchan_t *chan);