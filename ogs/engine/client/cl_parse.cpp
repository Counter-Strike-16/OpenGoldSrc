/*
 * This file is part of OGS Engine
 * Copyright (C) 2016-2017 OGS Dev Team
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the author gives permission to
 * link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 * Engine") and Modified Game Libraries ("MODs") developed by Valve,
 * L.L.C ("Valve").  You must obey the GNU General Public License in all
 * respects for all of the code used other than the GoldSrc Engine and MODs
 * from Valve.  If you modify this file, you may extend this exception
 * to your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from your
 * version.
 */

/// @file
/// @brief parse a message received from the server

#include "system/precompiled.h"
#include "client/client.h"

char *svc_strings[] =
    {
        "svc_bad",
        "svc_nop",
        "svc_disconnect",
        "svc_event",
        "svc_version",   // [long] server version
        "svc_setview",   // [short] entity number
        "svc_sound",     // <see code>
        "svc_time",      // [float] server time
        "svc_print",     // [string] null terminated string
        "svc_stufftext", // [string] stuffed into client's console buffer
                         // the string should be \n terminated
        "svc_setangle",  // [vec3] set the view angle to this absolute value

        "svc_serverinfo",       // [long] version ...
        "svc_lightstyle",       // [byte] [string]
        "svc_updateuserinfo",   // [byte] [string]
        "svc_deltadescription", // [byte] [short]
        "svc_clientdata",       // <shortbits + data>
        "svc_stopsound",        // <see code>
        "svc_pings",            // [byte] [byte]
        "svc_particle",         // [vec3] <variable>
        "svc_damage",           // deprecated

        "svc_spawnstatic",
        "svc_event_reliable",
        "svc_spawnbaseline",

        "svc_temp_entity", // <variable>
        "svc_setpause",
        "svc_signonnum",
        "svc_centerprint",
        "svc_killedmonster",
        "svc_foundsecret",
        "svc_spawnstaticsound",
        "svc_intermission",
        "svc_finale",

        "svc_cdtrack",
        "svc_restore",

        "svc_cutscene",
        "svc_weaponanim",

        "svc_decalnam",
        "svc_roomtype",

        "svc_addangle",
        "svc_newusermsg",
        "svc_packetentities",
        "svc_deltapacketentities",
        "svc_choke",
        "svc_resourcelist",
        "svc_newmovevars",
        "svc_resourcerequest",
        "svc_customization",
        "svc_crosshairangle",
        "svc_soundfade",
        "svc_filetxferfailed",
        "svc_hltv",
        "svc_director",

        "svc_voiceinit",
        "svc_voicedata",

        "svc_sendextrainfo",
        "svc_timescale",
        "svc_resourcelocation",

        "svc_sendcvarvalue",
        "svc_sendcvarvalue2",
};

int    oldparsecountmod;
int    parsecountmod;
double parsecounttime;

int cl_spikeindex, cl_playerindex, cl_flagindex;

//=============================================================================

int packet_latency[NET_TIMINGS];

int CL_CalcNet()
{
	int      a, i;
	frame_t *frame;
	int      lost;
	char     st[80];

	for(i = cls.netchan.outgoing_sequence - UPDATE_BACKUP + 1; i <= cls.netchan.outgoing_sequence; i++)
	{
		frame = &cl.frames[i & UPDATE_MASK];
		if(frame->receivedtime == -1)
			packet_latency[i & NET_TIMINGSMASK] = 9999; // dropped
		else if(frame->receivedtime == -2)
			packet_latency[i & NET_TIMINGSMASK] = 10000; // choked
		else if(frame->invalid)
			packet_latency[i & NET_TIMINGSMASK] = 9998; // invalid delta
		else
			packet_latency[i & NET_TIMINGSMASK] = (frame->receivedtime - frame->senttime) * 20;
	}

	lost = 0;
	for(a = 0; a < NET_TIMINGS; a++)
	{
		i = (cls.netchan.outgoing_sequence - a) & NET_TIMINGSMASK;
		if(packet_latency[i] == 9999)
			lost++;
	}
	return lost * 100 / NET_TIMINGS;
}

//=============================================================================

/*
===============
CL_CheckOrDownloadFile

Returns true if the file exists, otherwise it attempts
to start a download from the server.
===============
*/
qboolean CL_CheckOrDownloadFile(char *filename)
{
	FILE *f;

	if(strstr(filename, ".."))
	{
		Con_Printf("Refusing to download a path with ..\n");
		return true;
	}

	COM_FOpenFile(filename, &f);
	if(f)
	{ // it exists, no need to download
		fclose(f);
		return true;
	}

	//ZOID - can't download when recording
	if(cls.demorecording)
	{
		Con_Printf("Unable to download %s in record mode.\n", cls.downloadname);
		return true;
	}
	//ZOID - can't download when playback
	if(cls.demoplayback)
		return true;

	strcpy(cls.downloadname, filename);
	Con_Printf("Downloading %s...\n", cls.downloadname);

	// download to a temp name, and only rename
	// to the real name when done, so if interrupted
	// a runt file wont be left
	COM_StripExtension(cls.downloadname, cls.downloadtempname);
	strcat(cls.downloadtempname, ".tmp");

	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	MSG_WriteString(&cls.netchan.message, va("download %s", cls.downloadname));

	cls.downloadnumber++;

	return false;
}

/*
=================
Model_NextDownload
=================
*/
void Model_NextDownload()
{
	char *      s;
	int         i;
	extern char gamedirfile[];

	if(cls.downloadnumber == 0)
	{
		Con_Printf("Checking models...\n");
		cls.downloadnumber = 1;
	}

	cls.downloadtype = dl_model;
	for(
	    ; cl.model_name[cls.downloadnumber][0]; cls.downloadnumber++)
	{
		s = cl.model_name[cls.downloadnumber];
		if(s[0] == '*')
			continue; // inline brush model
		if(!CL_CheckOrDownloadFile(s))
			return; // started a download
	}

	for(i = 1; i < MAX_MODELS; i++)
	{
		if(!cl.model_name[i][0])
			break;

		cl.model_precache[i] = Mod_ForName(cl.model_name[i], false);

		if(!cl.model_precache[i])
		{
			Con_Printf("\nThe required model file '%s' could not be found or downloaded.\n\n", cl.model_name[i]);
			Con_Printf("You may need to download or purchase a %s client "
			           "pack in order to play on this server.\n\n",
			           gamedirfile);
			CL_Disconnect();
			return;
		}
	}

	// all done
	cl.worldmodel = cl.model_precache[1];
	R_NewMap();
	Hunk_Check(); // make sure nothing is hurt

	// done with modellist, request first of static signon messages
	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	//	MSG_WriteString (&cls.netchan.message, va("prespawn %i 0 %i", cl.servercount, cl.worldmodel->checksum2));
	MSG_WriteString(&cls.netchan.message, va(prespawn_name, cl.servercount, cl.worldmodel->checksum2));
}

/*
=================
Sound_NextDownload
=================
*/
void Sound_NextDownload()
{
	char *s;
	int   i;

	if(cls.downloadnumber == 0)
	{
		Con_Printf("Checking sounds...\n");
		cls.downloadnumber = 1;
	}

	cls.downloadtype = dl_sound;
	for(
	    ; cl.sound_name[cls.downloadnumber][0]; cls.downloadnumber++)
	{
		s = cl.sound_name[cls.downloadnumber];
		if(!CL_CheckOrDownloadFile(va("sound/%s", s)))
			return; // started a download
	}

	for(i = 1; i < MAX_SOUNDS; i++)
	{
		if(!cl.sound_name[i][0])
			break;
		cl.sound_precache[i] = S_PrecacheSound(cl.sound_name[i]);
	}

	// done with sounds, request models now
	memset(cl.model_precache, 0, sizeof(cl.model_precache));
	cl_playerindex = -1;
	cl_spikeindex  = -1;
	cl_flagindex   = -1;
	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	//	MSG_WriteString (&cls.netchan.message, va("modellist %i 0", cl.servercount));
	MSG_WriteString(&cls.netchan.message, va(modellist_name, cl.servercount, 0));
}

/*
======================
CL_RequestNextDownload
======================
*/
void CL_RequestNextDownload()
{
	switch(cls.downloadtype)
	{
	case dl_single:
		break;
	case dl_skin:
		Skin_NextDownload();
		break;
	case dl_model:
		Model_NextDownload();
		break;
	case dl_sound:
		Sound_NextDownload();
		break;
	case dl_none:
	default:
		Con_DPrintf("Unknown download type.\n");
	}
}

/*
=====================
CL_ParseDownload

A download message has been received from the server
=====================
*/
void CL_ParseDownload()
{
	int  size, percent;
	byte name[1024];
	int  r;

	// read the data
	size    = MSG_ReadShort();
	percent = MSG_ReadByte();

	if(cls.demoplayback)
	{
		if(size > 0)
			msg_readcount += size;
		return; // not in demo playback
	}

	if(size == -1)
	{
		Con_Printf("File not found.\n");
		if(cls.download)
		{
			Con_Printf("cls.download shouldn't have been set\n");
			fclose(cls.download);
			cls.download = NULL;
		}
		CL_RequestNextDownload();
		return;
	}

	// open the file if not opened yet
	if(!cls.download)
	{
		if(strncmp(cls.downloadtempname, "skins/", 6))
			sprintf(name, "%s/%s", com_gamedir, cls.downloadtempname);
		else
			sprintf(name, "qw/%s", cls.downloadtempname);

		COM_CreatePath(name);

		cls.download = fopen(name, "wb");
		if(!cls.download)
		{
			msg_readcount += size;
			Con_Printf("Failed to open %s\n", cls.downloadtempname);
			CL_RequestNextDownload();
			return;
		}
	}

	fwrite(net_message.data + msg_readcount, 1, size, cls.download);
	msg_readcount += size;

	if(percent != 100)
	{
// change display routines by zoid
// request next block
#if 0
		Con_Printf (".");
		if (10*(percent/10) != cls.downloadpercent)
		{
			cls.downloadpercent = 10*(percent/10);
			Con_Printf ("%i%%", cls.downloadpercent);
		}
#endif
		cls.downloadpercent = percent;

		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		SZ_Print(&cls.netchan.message, "nextdl");
	}
	else
	{
		char oldn[MAX_OSPATH];
		char newn[MAX_OSPATH];

#if 0
		Con_Printf ("100%%\n");
#endif

		fclose(cls.download);

		// rename the temp file to it's final name
		if(strcmp(cls.downloadtempname, cls.downloadname))
		{
			if(strncmp(cls.downloadtempname, "skins/", 6))
			{
				sprintf(oldn, "%s/%s", com_gamedir, cls.downloadtempname);
				sprintf(newn, "%s/%s", com_gamedir, cls.downloadname);
			}
			else
			{
				sprintf(oldn, "qw/%s", cls.downloadtempname);
				sprintf(newn, "qw/%s", cls.downloadname);
			}
			r = rename(oldn, newn);
			if(r)
				Con_Printf("failed to rename.\n");
		}

		cls.download        = NULL;
		cls.downloadpercent = 0;

		// get another file if needed

		CL_RequestNextDownload();
	}
}

static byte *upload_data;
static int   upload_pos;
static int   upload_size;

void CL_NextUpload()
{
	byte buffer[1024];
	int  r;
	int  percent;
	int  size;

	if(!upload_data)
		return;

	r = upload_size - upload_pos;
	if(r > 768)
		r = 768;
	memcpy(buffer, upload_data + upload_pos, r);
	MSG_WriteByte(&cls.netchan.message, clc_upload);
	MSG_WriteShort(&cls.netchan.message, r);

	upload_pos += r;
	size = upload_size;
	if(!size)
		size = 1;
	percent  = upload_pos * 100 / size;
	MSG_WriteByte(&cls.netchan.message, percent);
	SZ_Write(&cls.netchan.message, buffer, r);

	Con_DPrintf("UPLOAD: %6d: %d written\n", upload_pos - r, r);

	if(upload_pos != upload_size)
		return;

	Con_Printf("Upload completed\n");

	free(upload_data);
	upload_data = 0;
	upload_pos = upload_size = 0;
}

void CL_StartUpload(byte *data, int size)
{
	if(cls.state < ca_onserver)
		return; // gotta be connected

	// override
	if(upload_data)
		free(upload_data);

	Con_DPrintf("Upload starting of %d...\n", size);

	upload_data = malloc(size);
	memcpy(upload_data, data, size);
	upload_size = size;
	upload_pos  = 0;

	CL_NextUpload();
}

qboolean CL_IsUploading()
{
	if(upload_data)
		return true;
	return false;
}

void CL_StopUpload()
{
	if(upload_data)
		free(upload_data);
	upload_data = NULL;
}

/*
=====================================================================

  SERVER CONNECTING MESSAGES

=====================================================================
*/

/*
==================
CL_ParseServerData
==================
*/
void CL_ParseServerData()
{
	char *      str;
	FILE *      f;
	char        fn[MAX_OSPATH];
	qboolean    cflag = false;
	extern char gamedirfile[MAX_OSPATH];
	int         protover;

	Con_DPrintf("Serverdata packet received.\n");
	//
	// wipe the client_state_t struct
	//
	CL_ClearState();

	// parse protocol version number
	// allow 2.2 and 2.29 demos to play
	protover = MSG_ReadLong();
	if(protover != PROTOCOL_VERSION &&
	   !(cls.demoplayback && (protover == 26 || protover == 27 || protover == 28)))
		Host_EndGame("Server returned version %i, not %i\nYou probably need to upgrade.\nCheck http://www.quakeworld.net/", protover, PROTOCOL_VERSION);

	cl.servercount = MSG_ReadLong();

	// game directory
	str = MSG_ReadString();

	if(stricmp(gamedirfile, str))
	{
		// save current config
		Host_WriteConfiguration();
		cflag = true;
	}

	COM_Gamedir(str);

	//ZOID--run the autoexec.cfg in the gamedir
	//if it exists
	if(cflag)
	{
		sprintf(fn, "%s/%s", com_gamedir, "config.cfg");
		if((f = fopen(fn, "r")) != NULL)
		{
			fclose(f);
			Cbuf_AddText("cl_warncmd 0\n");
			Cbuf_AddText("exec config.cfg\n");
			Cbuf_AddText("exec frontend.cfg\n");
			Cbuf_AddText("cl_warncmd 1\n");
		}
	}

	// parse player slot, high bit means spectator
	cl.playernum = MSG_ReadByte();
	if(cl.playernum & 128)
	{
		cl.spectator = true;
		cl.playernum &= ~128;
	}

	// get the full level name
	str = MSG_ReadString();
	strncpy(cl.levelname, str, sizeof(cl.levelname) - 1);

	// get the movevars
	movevars.gravity           = MSG_ReadFloat();
	movevars.stopspeed         = MSG_ReadFloat();
	movevars.maxspeed          = MSG_ReadFloat();
	movevars.spectatormaxspeed = MSG_ReadFloat();
	movevars.accelerate        = MSG_ReadFloat();
	movevars.airaccelerate     = MSG_ReadFloat();
	movevars.wateraccelerate   = MSG_ReadFloat();
	movevars.friction          = MSG_ReadFloat();
	movevars.waterfriction     = MSG_ReadFloat();
	movevars.entgravity        = MSG_ReadFloat();

	// seperate the printfs so the server message can have a color
	Con_Printf("\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n\n");
	Con_Printf("%c%s\n", 2, str);

	// ask for the sound list next
	memset(cl.sound_name, 0, sizeof(cl.sound_name));
	MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
	//	MSG_WriteString (&cls.netchan.message, va("soundlist %i 0", cl.servercount));
	MSG_WriteString(&cls.netchan.message, va(soundlist_name, cl.servercount, 0));

	// now waiting for downloads, etc
	cls.state = ca_onserver;
}

/*
==================
CL_ParseSoundlist
==================
*/
void CL_ParseSoundlist()
{
	int   numsounds;
	char *str;
	int   n;

	// precache sounds
	//	memset (cl.sound_precache, 0, sizeof(cl.sound_precache));

	numsounds = MSG_ReadByte();

	for(;;)
	{
		str = MSG_ReadString();
		if(!str[0])
			break;
		numsounds++;
		if(numsounds == MAX_SOUNDS)
			Host_EndGame("Server sent too many sound_precache");
		strcpy(cl.sound_name[numsounds], str);
	}

	n = MSG_ReadByte();

	if(n)
	{
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		//		MSG_WriteString (&cls.netchan.message, va("soundlist %i %i", cl.servercount, n));
		MSG_WriteString(&cls.netchan.message, va(soundlist_name, cl.servercount, n));
		return;
	}

	cls.downloadnumber = 0;
	cls.downloadtype   = dl_sound;
	Sound_NextDownload();
}

/*
==================
CL_ParseModellist
==================
*/
void CL_ParseModellist()
{
	int   nummodels;
	char *str;
	int   n;

	// precache models and note certain default indexes
	nummodels = MSG_ReadByte();

	for(;;)
	{
		str = MSG_ReadString();
		if(!str[0])
			break;
		nummodels++;
		if(nummodels == MAX_MODELS)
			Host_EndGame("Server sent too many model_precache");
		strcpy(cl.model_name[nummodels], str);

		if(!strcmp(cl.model_name[nummodels], "progs/spike.mdl"))
			cl_spikeindex = nummodels;
		if(!strcmp(cl.model_name[nummodels], "progs/player.mdl"))
			cl_playerindex = nummodels;
		if(!strcmp(cl.model_name[nummodels], "progs/flag.mdl"))
			cl_flagindex = nummodels;
	}

	n = MSG_ReadByte();

	if(n)
	{
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		//		MSG_WriteString (&cls.netchan.message, va("modellist %i %i", cl.servercount, n));
		MSG_WriteString(&cls.netchan.message, va(modellist_name, cl.servercount, n));
		return;
	}

	cls.downloadnumber = 0;
	cls.downloadtype   = dl_model;
	Model_NextDownload();
}

/*
=====================================================================

ACTION MESSAGES

=====================================================================
*/

void CL_HandleDisconnect()
{
	char *sReason = MSG_ReadString();

	if(cls.state == ca_connected)
		Host_EndGame("Server disconnected\n"
		             "Server version may not be compatible");
	else
		Host_EndGame("Server disconnected");
};

void CL_ParseEventData(bool bReliable)
{
	/*
	Note: This message can be dropped if the client already has too much content in its unreliable buffer.
	Note: Events can be precached using pfnPrecacheEvent routine.
	Note: Events are queued and grouped together every frame, if there's any.
	Note: EventArgs are always inherited from "null" event args.
	Note: Only a max of 31 events can be queued and subsequently sent this way.
	Note: This message has its arguments in bit-packed form.
*/

	if(bReliable)
	{
	};
};

void CL_ParseVersion(){
    // Seems to be unused

    //long nServerProtocol = MSG_ReadLong();
    //if(PROTOCOL_VERSION != nServerProtocol)
    //Host_Error("CL_Parse_Version: Server is protocol %i instead of %i\n", nServerProtocol, PROTOCOL_VERSION);
};

void CL_ParseView()
{
	cl.refdef.viewentity = MSG_ReadWord();
};

/*
==================
CL_ParseStartSoundPacket
==================
*/
void CL_ParseStartSoundPacket()
{
	vec3_t pos;

	int flags = MSG_ReadWord();

	int volume = DEFAULT_SOUND_PACKET_VOLUME; // VOL_NORM

	if(flags & SND_FL_VOLUME)
		volume = MSG_ReadByte();

	float attenuation = DEFAULT_SOUND_PACKET_ATTENUATION; // ATTN_NONE

	if(flags & SND_FL_ATTENUATION)
		attenuation = MSG_ReadByte() / 64.0f;

	int channel   = MSG_ReadShort();
	int ent       = MSG_ReadByte();
	int sound_num = MSG_ReadByte();

	for(int i  = 0; i < 3; i++)
		pos[i] = MSG_ReadCoord();

	int pitch = DEFAULT_SOUND_PACKET_PITCH; // PITCH_NORM

	if(flags & SND_FL_PITCH)
		pitch = MSG_ReadByte();

	ent = (channel >> 3) & 1023;
	channel &= 7;

	if(ent > MAX_EDICTS)
		Host_EndGame("CL_ParseStartSoundPacket: ent = %i", ent);

	S_StartSound(ent, channel, cl.sound_precache[sound_num], pos, volume / 255.0, attenuation);
};

void CL_ParseTime()
{
	/*
	Notifies clients about the current server time

	Note: This message is sent every frame by the server
*/

	// shuffle timestamps
	cl.mtime[1] = cl.mtime[0];
	cl.mtime[0] = MSG_ReadFloat();
};

void CL_ParsePrint()
{
	i = MSG_ReadByte();

	if(i == PRINT_CHAT)
	{
		S_LocalSound("misc/talk.wav");
		con_ormask = 128;
	};

	Con_Printf("%s", MSG_ReadString());
	con_ormask = 0;
};

void CL_ParseStuffText()
{
	s = MSG_ReadString();
	Con_DPrintf("stufftext: %s\n", s);
	Cbuf_AddText(s);
};

void CL_ParseSetAngle()
{
	for(i                = 0; i < 3; i++)
		cl.viewangles[i] = MSG_ReadAngle();

	//cl.viewangles[PITCH] = cl.viewangles[ROLL] = 0;
};

/*
==============
CL_ServerInfo
==============
*/
void CL_ParseServerInfo()
{
	int            slot;
	player_info_t *player;
	char           key[MAX_MSGLEN];
	char           value[MAX_MSGLEN];

	strncpy(key, MSG_ReadString(), sizeof(key) - 1);
	key[sizeof(key) - 1] = 0;
	strncpy(value, MSG_ReadString(), sizeof(value) - 1);
	key[sizeof(value) - 1] = 0;

	Con_DPrintf("SERVERINFO: %s=%s\n", key, value);

	Info_SetValueForKey(cl.serverinfo, key, value, MAX_SERVERINFO_STRING);
};

void CL_ParseLightStyle()
{
	i = MSG_ReadByte();

	if(i >= MAX_LIGHTSTYLES)
		Sys_Error("svc_lightstyle > MAX_LIGHTSTYLES");

	Q_strcpy(cl_lightstyle[i].map, MSG_ReadString());
	cl_lightstyle[i].length = Q_strlen(cl_lightstyle[i].map);
};

void CL_ParseUserinfo(){};

void CL_ParseDeltaDescription(){};

/*
==================
CL_ParseClientdata

Server information pertaining to this client only, sent every frame
==================
*/
void CL_ParseClientdata()
{
	int      i;
	float    latency;
	frame_t *frame;

	// calculate simulated time of message
	oldparsecountmod = parsecountmod;

	i             = cls.netchan.incoming_acknowledged;
	cl.parsecount = i;
	i &= UPDATE_MASK;
	parsecountmod  = i;
	frame          = &cl.frames[i];
	parsecounttime = cl.frames[i].senttime;

	frame->receivedtime = realtime;

	// calculate latency
	latency = frame->receivedtime - frame->senttime;

	if(latency < 0 || latency > 1.0f)
	{
		//		Con_Printf ("Odd latency: %5.2f\n", latency);
	}
	else
	{
		// drift the average latency towards the observed latency
		if(latency < cls.latency)
			cls.latency = latency;
		else
			cls.latency += 0.001f; // drift up, so correction are needed
	};
};

void CL_HandleStopSound()
{
	i = MSG_ReadShort();
	S_StopSound(i >> 3, i & 7);
};

void CL_ParsePings()
{
	i = MSG_ReadByte();

	if(i >= MAX_CLIENTS)
		Host_EndGame("CL_ParseServerMessage: svc_updateping > MAX_SCOREBOARD");

	cl.players[i].ping = MSG_ReadShort();
};

void CL_ParseParticle(){};

/*
=====================
CL_ParseStatic

Static entities are non-interactive world objects
like torches
=====================
*/
void CL_ParseStatic()
{
	cl_entity_t *  ent;
	int            i;
	entity_state_t es;

	CL_ParseBaseline(&es);

	i = cl.num_statics;

	if(i >= MAX_STATIC_ENTITIES)
		Host_EndGame("Too many static entities");

	ent = &cl_static_entities[i];
	cl.num_statics++;

	// copy it to the current state
	ent->model    = cl.model_precache[es.modelindex];
	ent->frame    = es.frame;
	ent->colormap = vid.colormap;
	ent->skinnum  = es.skinnum;

	VectorCopy(es.origin, ent->origin);
	VectorCopy(es.angles, ent->angles);

	R_AddEfrags(ent);
};

/*
==================
CL_ParseBaseline
==================
*/
void CL_ParseBaseline(entity_state_t *es)
{
	es->modelindex = MSG_ReadByte();
	es->frame      = MSG_ReadByte();
	es->colormap   = MSG_ReadByte();
	es->skinnum    = MSG_ReadByte();

	for(int i = 0; i < 3; i++)
	{
		es->origin[i] = MSG_ReadCoord();
		es->angles[i] = MSG_ReadAngle();
	};
};

void CL_ParseTempEntity()
{
	byte nEntType = MSG_ReadByte();

	vec3_t vStartPos = vec3_origin;
	vec3_t vEndPos   = vec3_origin;

	switch(nEntType)
	{
	case TE_BEAMPOINTS:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoise      = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMENTPOINT:
		short nStartEnt = MSG_ReadShort();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoise      = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_GUNSHOT:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		break;
	case TE_EXPLOSION:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nScale     = MSG_ReadByte();
		byte nFrameRate = MSG_ReadByte();
		byte nFlags     = MSG_ReadByte();
		break;
	case TE_TAREXPLOSION:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		break;
	case TE_SMOKE:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nScale     = MSG_ReadByte();
		byte nFrameRate = MSG_ReadByte();
		break;
	case TE_TRACER:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();
		break;
	case TE_LIGHTNING:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		byte nLifeTime  = MSG_ReadByte();
		byte nWidth     = MSG_ReadByte();
		byte nAmplitude = MSG_ReadByte();

		short nModelIndex = MSG_ReadShort();
		break;
	case TE_BEAMENTS:
		short nStartEnt = MSG_ReadShort();
		short nEndEnt   = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_SPARKS:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		break;
	case TE_LAVASPLASH:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		break;
	case TE_TELEPORT:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();
		break;
	case TE_EXPLOSION2:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		byte nStartColor = MSG_ReadByte();
		byte nColorCount = MSG_ReadByte();
		break;
	case TE_BSPDECAL:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nTextureIndex  = MSG_ReadShort();
		short nEntIndex      = MSG_ReadShort();
		short nEntModelIndex = 0;

		if(nEntIndex)
			nEntModelIndex = MSG_ReadShort();
		break;
	case TE_IMPLOSION:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		byte nRadius   = MSG_ReadByte();
		byte nCount    = MSG_ReadByte();
		byte nLifeTime = MSG_ReadByte();
		break;
	case TE_SPRITETRAIL:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nCount      = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nScale      = MSG_ReadByte();
		byte nVelocity   = MSG_ReadByte();
		byte nRandomness = MSG_ReadByte();
		break;
	case TE_BEAM:
		// obsolete, unused
		break;
	case TE_SPRITE:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nScale      = MSG_ReadByte();
		byte nBrightness = MSG_ReadByte();
		break;
	case TE_BEAMSPRITE:
		for(int i        = 0; i < 3; ++i)
			vStartPos[i] = MSG_ReadCoord();

		for(int i      = 0; i < 3; ++i)
			vEndPos[i] = MSG_ReadCoord();

		short nStartSprIndex = MSG_ReadShort();
		short nEndSprIndex   = MSG_ReadShort();
		break;
	case TE_BEAMTORUS:
		coord coord coord(center position)
		    coord coord coord(axis and radius)

		        short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMDISK:
		coord coord coord(center position)
		    coord coord coord(axis and radius)

		        short nSprIndex = MSG_ReadShort();

		byte nStartingFrame = MSG_ReadByte();
		byte nFrameRate     = MSG_ReadByte();
		byte nLifeTime      = MSG_ReadByte();
		byte nLineWidth     = MSG_ReadByte();
		byte nNoiseAmpl     = MSG_ReadByte();

		byte nColor[3] = {0};
		for(int i     = 0; i < 3; ++i)
			nColor[i] = MSG_ReadByte();

		byte nBrightness  = MSG_ReadByte();
		byte nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMCYLINDER:
		coord coord coord(center position)
		    coord coord coord(axis and radius)

		        short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameTate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();

		byte, byte, byte(color)

		                byte nBrightness  = MSG_ReadByte();
		byte                 nScrollSpeed = MSG_ReadByte();
		break;
	case TE_BEAMFOLLOW:
		short(entity
		      : attachment to follow) short nSprIndex = MSG_ReadShort();

		byte nLifeTime  = MSG_ReadByte();
		byte nLineWidth = MSG_ReadByte();

		byte, byte, byte(color)

		                byte nBrightness = MSG_ReadByte();
		break;
	case TE_GLOWSPRITE:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte(scale / 10) = MSG_ReadByte();
		break;
	case TE_BEAMRING:
		short nStartEnt = MSG_ReadShort();
		short nEndEnt   = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		byte nStartFrame = MSG_ReadByte();
		byte nFrameRate  = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nLineWidth  = MSG_ReadByte();
		byte nNoiseAmpl  = MSG_ReadByte();

		byte, byte, byte(color)

		                byte nBrightness  = MSG_ReadByte();
		byte                 nScrollSpeed = MSG_ReadByte();
		break;
	case TE_STREAK_SPLASH:
		coord coord coord(start position)
		    coord coord
		    coord(direction vector)

		        byte nColor = MSG_ReadByte();

		short nCount     = MSG_ReadShort();
		short nBaseSpeed = MSG_ReadShort();
		short nVelocity  = MSG_ReadShort();
		break;
	case TE_BEAMHOSE:
		// obsolete, unused
		break;
	case TE_DLIGHT:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		byte(radius in 10's) = MSG_ReadByte();

		byte byte byte(color) = MSG_ReadByte();

		byte nBrightness = MSG_ReadByte();
		byte nLifeTime   = MSG_ReadByte();
		byte nDecayRate  = MSG_ReadByte();
		break;
	case TE_ELIGHT:
		short(entity
		      : attachment to follow)

		    float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fRadius = MSG_ReadCoord();

		byte byte byte(color)

		    byte nLifeTime = MSG_ReadByte();

		coord(decay rate) = MSG_ReadCoord();
		break;
	case TE_TEXTMESSAGE:
		short 1.2.13 x(-1 = center) short 1.2.13 y(-1 = center)

		    byte nEffect = MSG_ReadByte();
		// 0 = fade in/fade out
		// 1 is flickery credits
		// 2 is write out (training room)
		// 4 bytes r,g,b,a color1	(text color)
		// 4 bytes r,g,b,a color2	(effect color)

		ushort nFadeInTime
		    ushort nFadeOutTime
		        ushort nHoldTime

		            optional ushort
		            nFXTime(time the highlight lags behing the leading text in effect 2)

		                string text message(512 chars max sz string) break;
	case TE_LINE:
		coord, coord, coord startpos
		                  coord,
		    coord, coord endpos

		    short nLifeTime = MSG_ReadShort();

		3 bytes r, g, b break;
	case TE_BOX:
		coord, coord, coord boxmins
		                  coord,
		    coord, coord boxmaxs

		    short nLifeTime = MSG_ReadShort();

		3 bytes r, g, b break;
	case TE_KILLBEAM:
		short nEnt = MSG_ReadShort();
		break;
	case TE_LARGEFUNNEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();
		short nFlags    = MSG_ReadShort();
		break;
	case TE_BLOODSTREAM:
		coord coord coord(start position)
		    coord coord
		    coord(spray vector)

		        byte nColor = MSG_ReadByte();
		byte         nSpeed = MSG_ReadByte();
		break;
	case TE_SHOWLINE:
		coord coord coord(start position)
		    coord coord
		    coord(end position) break;
	case TE_BLOOD:
		coord coord coord(start position)
		    coord coord
		    coord(spray vector)

		        byte nColor = MSG_ReadByte();
		byte         nSpeed = MSG_ReadByte();
		break;
	case TE_DECAL:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte(texture index of precached decal texture name)

		                                      short nEntIndex = MSG_ReadShort();
		break;
	case TE_FIZZ:
		short nEntity   = MSG_ReadShort();
		short nSprIndex = MSG_ReadShort();

		byte nDensity = MSG_ReadByte();
		break;
	case TE_MODEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		angle(initial yaw)

		    short nModelIndex = MSG_ReadShort();

		byte(bounce sound type)     = MSG_ReadByte();
		byte              nLifeTime = MSG_ReadByte();
		break;
	case TE_EXPLODEMODEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();
		short nCount      = MSG_ReadShort();

		byte nLifeTime = MSG_ReadByte();
		break;
	case TE_BREAKMODEL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fSize[3] = {0};
		for(int i    = 0; i < 3; ++i)
			fSize[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		byte nVelocity = MSG_ReadByte();

		short nModelIndex = MSG_ReadShort();

		byte nCount    = MSG_ReadByte();
		byte nLifeTime = MSG_ReadByte();
		byte nFlags    = MSG_ReadByte();
		break;
	case TE_GUNSHOTDECAL:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short nEntIndex = MSG_ReadShort();

		byte (decal???) = MSG_ReadByte();
		break;
	case TE_SPRITE_SPRAY:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		short nSprIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();
		byte nSpeed = MSG_ReadByte();
		byte nNoise = MSG_ReadByte();
		break;
	case TE_ARMOR_RICOCHET:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		byte nScale = MSG_ReadByte();
		break;
	case TE_PLAYERDECAL:
		byte nPlayerIndex = MSG_ReadByte();

		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		short (entity???) = MSG_ReadShort();

		byte nDecalNum = MSG_ReadByte();

		[optional] short (model index???)
		break;
	case TE_BUBBLES:
		float fMinPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMinPos[i] = MSG_ReadCoord();

		float fMaxPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMaxPos[i] = MSG_ReadCoord();

		float fHeight = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();

		float fSpeed = MSG_ReadCoord();
		break;
	case TE_BUBBLETRAIL:
		float fMinPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMinPos[i] = MSG_ReadCoord();

		float fMaxPos[3] = {0};
		for(int i      = 0; i < 3; ++i)
			fMaxPos[i] = MSG_ReadCoord();

		float fHeight = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();

		float fSpeed = MSG_ReadCoord();
		break;
	case TE_BLOODSPRITE:
		float fPosition[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fPosition[i] = MSG_ReadCoord();

		short nSpr1Index = MSG_ReadShort();
		short nSpr2Index = MSG_ReadShort();

		byte nColor = MSG_ReadByte();
		byte nScale = MSG_ReadByte();
		break;
	case TE_WORLDDECAL:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte nTextureIndex = MSG_ReadByte();
		break;
	case TE_WORLDDECALHIGH:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte(texture index of precached decal texture name - 256) break;
	case TE_DECALHIGH:
		coord, coord, coord(x, y, z), decal position(center of texture in world)

		                                  byte nTextureIndex = MSG_ReadByte();

		short nEntIndex = MSG_ReadShort();
		break;
	case TE_PROJECTILE:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nLife  = MSG_ReadByte();
		byte nOwner = MSG_ReadByte();
		break;
	case TE_SPRAY:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fDir[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fDir[i] = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();

		byte nCount      = MSG_ReadByte();
		byte nSpeed      = MSG_ReadByte();
		byte nNoise      = MSG_ReadByte();
		byte nRenderMode = MSG_ReadByte();
		break;
	case TE_PLAYERSPRITES:
		byte nPlayerNum = MSG_ReadByte();

		short nSprModelIndex = MSG_ReadShort();

		byte nCount = MSG_ReadByte();
		byte nVariance(0 = no variance in size)(10 = 10 % variance in size) break;
	case TE_PARTICLEBURST:
		float fOrigin = MSG_ReadCoord();

		short nRadius = MSG_ReadShort();

		byte nParticleColor = MSG_ReadByte();
		byte nDuration      = MSG_ReadByte();
		break;
	case TE_FIREFIELD:
		float fOrigin = MSG_ReadCoord();

		short nRadius     = MSG_ReadShort();
		short nModelIndex = MSG_ReadShort();

		byte nCount    = MSG_ReadByte();
		byte nFlags    = MSG_ReadByte();
		byte nDuration = MSG_ReadByte();
		break;
	case TE_PLAYERATTACHMENT:
		byte nPlayerEntIndex = MSG_ReadByte();

		float fVerticalOffset = MSG_ReadCoord();

		short nModelIndex = MSG_ReadShort();
		short nLifeTime   = MSG_ReadShort();
		break;
	case TE_KILLPLAYERATTACHMENTS:
		byte nPlayerEntIndex = MSG_ReadByte();
		break;
	case TE_MULTIGUNSHOT:
		// This message is used to make a client approximate a 'spray' of gunfire.
		// Any weapon that fires more than one bullet per frame and fires in a bit of a spread is
		// a good candidate for MULTIGUNSHOT use. (shotguns)
		//
		// NOTE: This effect makes the client do traces for each bullet, these client traces ignore
		//		 entities that have studio models.Traces are 4096 long.
		//
		// coord (origin)
		// coord (origin)
		// coord (origin)
		// coord (direction)
		// coord (direction)
		// coord (direction)
		// coord (x noise * 100)
		// coord (y noise * 100)
		// byte (count)
		// byte (bullethole decal texture index)
		break;
	case TE_USERTRACER:
		float fPos[3] = {0};
		for(int i   = 0; i < 3; ++i)
			fPos[i] = MSG_ReadCoord();

		float fVelocity[3] = {0};
		for(int i        = 0; i < 3; ++i)
			fVelocity[i] = MSG_ReadCoord();

		byte nLifeTime   = MSG_ReadByte();
		byte nColorIndex = MSG_ReadByte();
		byte nLength     = MSG_ReadByte();
		break;
	default:
		break;
	};
};

void CL_HandlePause()
{
	cl.refdef.paused = (MSG_ReadOneBit() != 0);
};

void CL_ParseSignOnNum()
{
	// Called just after client_putinserver
	// Signals the client that the server has marked it as "active"

	byte nSignOn = MSG_ReadByte();
};

void CL_ParseCenterPrint()
{
	SCR_CenterPrint(MSG_ReadString());
};

void CL_ParseKilledMonster()
{
	// Deprecated

	cl.stats[STAT_MONSTERS]++;
};

void CL_ParseFoundSecret()
{
	// Deprecated

	cl.stats[STAT_SECRETS]++;
};

/*
===================
CL_ParseStaticSound
===================
*/
void CL_ParseStaticSound()
{
	vec3_t org;
	int    sound_num, vol, atten;
	int    i;

	for(i      = 0; i < 3; i++)
		org[i] = MSG_ReadCoord();
	sound_num  = MSG_ReadByte();
	vol        = MSG_ReadByte();
	atten      = MSG_ReadByte();

	S_StaticSound(cl.sound_precache[sound_num], org, vol, atten);
};

void CL_ParseIntermission()
{
	cl.intermission   = 1;
	cl.completed_time = realtime;
	vid.recalc_refdef = true; // go to full screen

	for(i            = 0; i < 3; i++)
		cl.simorg[i] = MSG_ReadCoord();

	for(i               = 0; i < 3; i++)
		cl.simangles[i] = MSG_ReadAngle();

	VectorCopy(vec3_origin, cl.simvel);
};

void CL_ParseFinale()
{
	cl.intermission   = 2;
	cl.completed_time = realtime;
	vid.recalc_refdef = true; // go to full screen
	SCR_CenterPrint(MSG_ReadString());
};

void CL_ParseCDTrack()
{
	cl.cdtrack   = MSG_ReadByte();
	cl.looptrack = MSG_ReadByte();

	CDAudio_Play((byte)cl.cdtrack, true);
};

void CL_ParseRestore(){};

void CL_ParseCutscene()
{
	/*
	Shows the intermission camera view, and writes-out text passed in first parameter.

	Note: Intermission mode 3
	Note: This text will keep showing on clients in future intermissions
*/

	char *sText = MSG_ReadString();
};

void CL_ParseWeaponAnim()
{
	/*
	Sended only if client weapon is disabled
*/

	param1 = MSG_ReadByte(); // SequenceNumber
	param2 = MSG_ReadByte(); // WeaponmodelBodygroup
	CL_WeaponAnim(param1, param2);
};

void CL_ParseDecalName()
{
	/*
	Allows to set, into the client's decals array and at specific position index (0->511), a decal name.
	E.g: let's say you send a message to set a decal "{break" at index 200.
	As result, when a message TE_ will be used to show a decal at index 200, we will see "{break".

	Note: If there is already an existing decal at the provided index, it will be overwritten.
	Note: It appears we can play only with decals from decals.wad. 
*/

	byte nPosIndex = MSG_ReadByte();

	char *sDecalName = MSG_ReadString();
};

void CL_ParseRoomType()
{
	short nRoomType = MSG_ReadShort();
	Cvar_SetFloat("room_type", nRoomType);
};

void CL_ParseAddAngle()
{
	/*
	Note: When pev->fixangle is set to 2, this message is called with pev->avelocity[1] as a value.
	Note: The value needs to be scaled by (65536 / 360).
*/

	short nAngle = MSG_ReadShort();
};

void CL_ParseNewUserMsg()
{
	/*
	Note: Sent every time a new message is registered on the server, but most games do this only once on the map change or server startup.
	Note: Name can be represented as an array of 4 "longs".
*/

	byte  nIndex = MSG_ReadByte();
	byte  nSize  = MSG_ReadByte();
	char *sName  = MSG_ReadString(); // 16 bits or bytes? (4 longs == 16 bytes on x86)
	                                 //MSG_ReadLong(msg, *(int *)&pMsg->szName[0]);
	                                 //MSG_ReadLong(msg, *(int *)&pMsg->szName[4]);
	                                 //MSG_ReadLong(msg, *(int *)&pMsg->szName[8]);
	                                 //MSG_ReadLong(msg, *(int *)&pMsg->szName[12]);
};

void CL_ParsePacketEntities(bool bDelta)
{
	if(bDelta)
	{
	};
};

void CL_HandleChoke(){
    // Notify the client that some outgoing datagrams were not transmitted due to exceeding bandwidth rate limits

    //i = MSG_ReadByte ();
    //for (j=0 ; j<i ; j++)
    //	cl.frames[ (cls.netchan.incoming_acknowledged-1-j)&UPDATE_MASK ].receivedtime = -2;
};

void CL_ParseResourceList()
{
	CL_ParseModellist();
	CL_ParseSoundlist();
};

void CL_ParseNewMoveVars()
{
	movevars.maxspeed   = MSG_ReadFloat();
	movevars.entgravity = MSG_ReadFloat();
};

void CL_ParseResourceRequest(){};

void CL_ParseCustomization(){};

void CL_ParseCrosshairAngle(){};

void CL_ParseSoundFade(){};

void CL_HandleFileTransferFailed(){};

void CL_ParseHLTVMode()
{
	byte nMode = MSG_ReadByte();
};

void CL_ParseDirectorMsg(){};

void CL_HandleVoiceInit(){};

void CL_ParseVoiceData(){};

void CL_ParseExtraInfo(){};

void CL_ParseTimeScale()
{
	float fTimeScale = MSG_ReadFloat();
};

void CL_ParseResouceLocation()
{
	// sv_downloadurl
	char *sURL = MSG_ReadString();
};

void CL_ParseCvarValue(){};

void CL_ParseCvarValue2(){};

/*
=====================
CL_NewTranslation
=====================
*/
void CL_NewTranslation(int slot)
{
#ifdef GLQUAKE
	if(slot > MAX_CLIENTS)
		Sys_Error("CL_NewTranslation: slot > MAX_CLIENTS");

	R_TranslatePlayerSkin(slot);
#else

	int            i, j;
	int            top, bottom;
	byte *         dest, *source;
	player_info_t *player;
	char           s[512];

	if(slot > MAX_CLIENTS)
		Sys_Error("CL_NewTranslation: slot > MAX_CLIENTS");

	player = &cl.players[slot];

	strcpy(s, Info_ValueForKey(player->userinfo, "skin"));
	COM_StripExtension(s, s);
	if(player->skin && !stricmp(s, player->skin->name))
		player->skin = NULL;

	if(player->_topcolor != player->topcolor ||
	   player->_bottomcolor != player->bottomcolor || !player->skin)
	{
		player->_topcolor    = player->topcolor;
		player->_bottomcolor = player->bottomcolor;

		dest   = player->translations;
		source = vid.colormap;
		memcpy(dest, vid.colormap, sizeof(player->translations));
		top = player->topcolor;
		if(top > 13 || top < 0)
			top = 13;
		top *= 16;
		bottom = player->bottomcolor;
		if(bottom > 13 || bottom < 0)
			bottom = 13;
		bottom *= 16;

		for(i = 0; i < VID_GRADES; i++, dest += 256, source += 256)
		{
			if(top < 128) // the artists made some backwards ranges.  sigh.
				memcpy(dest + TOP_RANGE, source + top, 16);
			else
				for(j                   = 0; j < 16; j++)
					dest[TOP_RANGE + j] = source[top + 15 - j];

			if(bottom < 128)
				memcpy(dest + BOTTOM_RANGE, source + bottom, 16);
			else
				for(j                      = 0; j < 16; j++)
					dest[BOTTOM_RANGE + j] = source[bottom + 15 - j];
		}
	}
#endif
}

/*
==============
CL_UpdateUserinfo
==============
*/
void CL_ProcessUserInfo(int slot, player_info_t *player)
{
	strncpy(player->name, Info_ValueForKey(player->userinfo, "name"), sizeof(player->name) - 1);
	player->topcolor    = atoi(Info_ValueForKey(player->userinfo, "topcolor"));
	player->bottomcolor = atoi(Info_ValueForKey(player->userinfo, "bottomcolor"));
	if(Info_ValueForKey(player->userinfo, "*spectator")[0])
		player->spectator = true;
	else
		player->spectator = false;

	if(cls.state == ca_active)
		Skin_Find(player);

	Sbar_Changed();
	CL_NewTranslation(slot);
}

/*
==============
CL_UpdateUserinfo
==============
*/
void CL_UpdateUserinfo()
{
	int            slot;
	player_info_t *player;

	slot = MSG_ReadByte();
	if(slot >= MAX_CLIENTS)
		Host_EndGame("CL_ParseServerMessage: svc_updateuserinfo > MAX_SCOREBOARD");

	player         = &cl.players[slot];
	player->userid = MSG_ReadLong();
	strncpy(player->userinfo, MSG_ReadString(), sizeof(player->userinfo) - 1);

	CL_ProcessUserInfo(slot, player);
}

/*
==============
CL_SetInfo
==============
*/
void CL_SetInfo()
{
	int            slot;
	player_info_t *player;
	char           key[MAX_MSGLEN];
	char           value[MAX_MSGLEN];

	slot = MSG_ReadByte();
	if(slot >= MAX_CLIENTS)
		Host_EndGame("CL_ParseServerMessage: svc_setinfo > MAX_SCOREBOARD");

	player = &cl.players[slot];

	strncpy(key, MSG_ReadString(), sizeof(key) - 1);
	key[sizeof(key) - 1] = 0;
	strncpy(value, MSG_ReadString(), sizeof(value) - 1);
	key[sizeof(value) - 1] = 0;

	Con_DPrintf("SETINFO %s: %s=%s\n", player->name, key, value);

	Info_SetValueForKey(player->userinfo, key, value, MAX_INFO_STRING);

	CL_ProcessUserInfo(slot, player);
}

/*
=====================
CL_SetStat
=====================
*/
void CL_SetStat(int stat, int value)
{
	int j;
	if(stat < 0 || stat >= MAX_CL_STATS)
		Sys_Error("CL_SetStat: %i is invalid", stat);

	Sbar_Changed();

	if(stat == STAT_ITEMS)
	{ // set flash times
		Sbar_Changed();
		for(j = 0; j < 32; j++)
			if((value & (1 << j)) && !(cl.stats[stat] & (1 << j)))
				cl.item_gettime[j] = cl.time;
	}

	cl.stats[stat] = value;
}

/*
==============
CL_MuzzleFlash
==============
*/
void CL_MuzzleFlash()
{
	vec3_t          fv, rv, uv;
	dlight_t *      dl;
	int             i;
	player_state_t *pl;

	i = MSG_ReadShort();

	if((unsigned)(i - 1) >= MAX_CLIENTS)
		return;

#ifdef GLQUAKE
	// don't draw our own muzzle flash in gl if flashblending
	if(i - 1 == cl.playernum && gl_flashblend.value)
		return;
#endif

	pl = &cl.frames[parsecountmod].playerstate[i - 1];

	dl = CL_AllocDlight(i);
	VectorCopy(pl->origin, dl->origin);
	AngleVectors(pl->viewangles, fv, rv, uv);

	VectorMA(dl->origin, 18, fv, dl->origin);
	dl->radius   = 200 + (rand() & 31);
	dl->minlight = 32;
	dl->die      = cl.time + 0.1;
	dl->color[0] = 0.2;
	dl->color[1] = 0.1;
	dl->color[2] = 0.05;
	dl->color[3] = 0.7;
}

#define SHOWNET(x)            \
	if(cl_shownet.value == 2) \
		Con_Printf("%3i:%s\n", msg_readcount - 1, x);
/*
=====================
CL_ParseServerMessage
=====================
*/
int  received_framecount;
void CL_ParseServerMessage()
{
	int   cmd;
	char *s;
	int   i, j;

	received_framecount   = host_framecount;
	cl.last_servermessage = realtime;
	CL_ClearProjectiles();

	//
	// if recording demos, copy the message out
	//
	if(cl_shownet.value == 1)
		Con_Printf("%i ", net_message.cursize);
	else if(cl_shownet.value == 2)
		Con_Printf("------------------\n");

	CL_ParseClientdata();

	//
	// parse the message
	//
	while(1)
	{
		if(msg_badread)
		{
			Host_EndGame("CL_ParseServerMessage: Bad server message");
			break;
		}

		cmd = MSG_ReadByte();

		if(cmd == -1)
		{
			msg_readcount++; // so the EOM showner has the right value
			SHOWNET("END OF MESSAGE");
			break;
		}

		SHOWNET(svc_strings[cmd]);

		// other commands
		switch(cmd)
		{
		// GS protocol entries
		case svc_bad:
			//Host_Error( "svc_bad\n" );
			break;
		case svc_nop:
			// this does nothing
			//Con_Printf("svc_nop\n");
			break;
		case svc_disconnect:
			CL_HandleDisconnect();
			break;
		case svc_event:
			CL_ParseEventData(false);
			break;
		case svc_version:
			CL_ParseVersion();
			break;
		case svc_setview:
			CL_ParseView();
			break;
		case svc_sound:
			CL_ParseStartSoundPacket();
			break;
		case svc_time:
			CL_ParseTime();
			break;
		case svc_print:
			CL_ParsePrint();
			break;
		case svc_stufftext:
			CL_ParseStuffText();
			break;
		case svc_setangle:
			CL_ParseSetAngle();
			break;
		case svc_serverinfo:
			CL_ParseServerInfo();

			//from svc_serverdata
			//Cbuf_Execute ();		// make sure any stuffed commands are done
			//CL_ParseServerData ();
			//vid.recalc_refdef = true;	// leave full screen intermission
			break;
		case svc_lightstyle:
			CL_ParseLightStyle();
			break;
		case svc_updateuserinfo:
			CL_ParseUserinfo();
			break;
		case svc_deltadescription:
			CL_ParseDeltaDescription();
			break;
		case svc_clientdata:
			CL_ParseClientData();
			break;
		case svc_stopsound:
			CL_HandleStopSound();
			break;
		case svc_pings:
			CL_ParsePings();
			break;
		case svc_particle:
			CL_ParseParticle();
			break;
		case svc_damage:
			V_ParseDamage();
			break;
		case svc_spawnstatic:
			CL_ParseStatic();
			break;
		case svc_event_reliable:
			CL_ParseEventData(true);
			break;
		case svc_spawnbaseline:
			i = MSG_ReadShort();
			CL_ParseBaseline(&cl_baselines[i]);
			break;
		case svc_temp_entity:
			CL_ParseTempEntity();
			break;
		case svc_setpause:
			CL_HandlePause();
			break;
		case svc_signonnum:
			CL_ParseSignOnNum();
			break;
		case svc_centerprint:
			CL_ParseCenterPrint();
			break;
		case svc_killedmonster:
			CL_ParseKilledMonster();
			break;
		case svc_foundsecret:
			CL_ParseFoundSecret();
			break;
		case svc_spawnstaticsound:
			CL_ParseStaticSound();
			break;
		case svc_intermission:
			CL_ParseIntermission();
			break;
		case svc_finale:
			CL_ParseFinale();
			break;
		case svc_cdtrack:
			CL_ParseCDTrack();
			break;
		case svc_restore:
			CL_ParseRestore();
			break;
		case svc_cutscene:
			CL_ParseCutscene();
			break;
		case svc_weaponanim:
			CL_ParseWeaponAnim();
			break;
		case svc_decalname:
			CL_ParseDecalName();
			break;
		case svc_roomtype:
			CL_ParseRoomType();
			break;
		case svc_addangle:
			CL_ParseAddAngle();
			break;
		case svc_newusermsg:
			CL_ParseNewUserMsg();
			break;
		case svc_packetentities:
			CL_ParsePacketEntities(false);
			break;
		case svc_deltapacketentities:
			CL_ParsePacketEntities(true);
			break;
		case svc_choke: // some preceding packets were choked
			CL_HandleChoke();
			break;
		case svc_resourcelist:
			CL_ParseResourceList();
			break;
		case svc_newmovevars:
			CL_ParseNewMoveVars();
			break;
		case svc_resourcerequest:
			CL_ParseResourceRequest();
			break;
		case svc_customization:
			CL_ParseCustomization();
			break;
		case svc_crosshairangle:
			CL_ParseCrosshairAngle();
			break;
		case svc_soundfade:
			CL_ParseSoundFade();
			break;
		case svc_filetxferfailed:
			CL_HandleFileTransferFailed();
			break;
		case svc_hltv:
			CL_ParseHLTVMode();
			break;
		case svc_director:
			CL_ParseDirectorMsg();
			break;
		case svc_voiceinit:
			CL_HandleVoiceInit();
			break;
		case svc_voicedata:
			CL_ParseVoiceData();
			break;
		case svc_sendextrainfo:
			CL_ParseExtraInfo();
			break;
		case svc_timescale:
			CL_ParseTimeScale();
			break;
		case svc_resourcelocation:
			CL_ParseResouceLocation();
			break;
		case svc_sendcvarvalue:
			CL_ParseCvarValue();
			break;
		case svc_sendcvarvalue2:
			CL_ParseCvarValue2();
			break;
		default:
			Host_EndGame("CL_ParseServerMessage: Illegible server message");
			break;
			// Old QW protocol
			/*
		case svc_updatefrags:
			Sbar_Changed ();
			i = MSG_ReadByte ();
			if (i >= MAX_CLIENTS)
				Host_EndGame ("CL_ParseServerMessage: svc_updatefrags > MAX_SCOREBOARD");
			cl.players[i].frags = MSG_ReadShort ();
			break;			
			
		case svc_updatepl:
			i = MSG_ReadByte ();
			if (i >= MAX_CLIENTS)
				Host_EndGame ("CL_ParseServerMessage: svc_updatepl > MAX_SCOREBOARD");
			cl.players[i].pl = MSG_ReadByte ();
			break;
			
		case svc_updateentertime:
		// time is sent over as seconds ago
			i = MSG_ReadByte ();
			if (i >= MAX_CLIENTS)
				Host_EndGame ("CL_ParseServerMessage: svc_updateentertime > MAX_SCOREBOARD");
			cl.players[i].entertime = realtime - MSG_ReadFloat ();
			break;

		case svc_updatestat:
			i = MSG_ReadByte ();
			j = MSG_ReadByte ();
			CL_SetStat (i, j);
			break;
		case svc_updatestatlong:
			i = MSG_ReadByte ();
			j = MSG_ReadLong ();
			CL_SetStat (i, j);
			break;

		case svc_smallkick:
			cl.punchangle = -2;
			break;
		case svc_bigkick:
			cl.punchangle = -4;
			break;

		case svc_setinfo:
			CL_SetInfo ();
			break;

		case svc_download:
			CL_ParseDownload ();
			break;

		case svc_playerinfo:
			CL_ParsePlayerinfo ();
			break;

		case svc_nails:
			CL_ParseProjectiles ();
			break;

		case svc_setpause:
			cl.paused = MSG_ReadByte ();
			if (cl.paused)
				CDAudio_Pause ();
			else
				CDAudio_Resume ();
			break;
		*/
		}
	}

	CL_SetSolidEntities();
}