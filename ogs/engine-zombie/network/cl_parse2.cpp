



int oldparsecountmod;
int parsecountmod;
double parsecounttime;

int cl_spikeindex, cl_playerindex, cl_flagindex;

//=============================================================================

int packet_latency[NET_TIMINGS];

int CL_CalcNet()
{
	int a, i;
	frame_t *frame;
	int lost;
	char st[80];

	for(i = cls.netchan.outgoing_sequence - UPDATE_BACKUP + 1;
	    i <= cls.netchan.outgoing_sequence;
	    i++)
	{
		frame = &cl.frames[i & UPDATE_MASK];
		if(frame->receivedtime == -1)
			packet_latency[i & NET_TIMINGSMASK] = 9999; // dropped
		else if(frame->receivedtime == -2)
			packet_latency[i & NET_TIMINGSMASK] = 10000; // choked
		else if(frame->invalid)
			packet_latency[i & NET_TIMINGSMASK] = 9998; // invalid delta
		else
			packet_latency[i & NET_TIMINGSMASK] =
			(frame->receivedtime - frame->senttime) * 20;
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

	// Can't download when recording
	if(cls.demorecording)
	{
		Con_Printf("Unable to download %s in record mode.\n", cls.downloadname);
		return true;
	}
	// Can't download when playback
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
	char *s;
	int i;
	extern char gamedirfile[];

	if(cls.downloadnumber == 0)
	{
		Con_Printf("Checking models...\n");
		cls.downloadnumber = 1;
	}

	cls.downloadtype = dl_model;
	for(; cl.model_name[cls.downloadnumber][0]; cls.downloadnumber++)
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
			Con_Printf("\nThe required model file '%s' could not be found or "
			           "downloaded.\n\n",
			           cl.model_name[i]);
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
	//	MSG_WriteString (&cls.netchan.message, va("prespawn %i 0 %i",
	//cl.servercount, cl.worldmodel->checksum2));
	MSG_WriteString(&cls.netchan.message,
	                va(prespawn_name, cl.servercount, cl.worldmodel->checksum2));
}

/*
=================
Sound_NextDownload
=================
*/
void Sound_NextDownload()
{
	char *s;
	int i;

	if(cls.downloadnumber == 0)
	{
		Con_Printf("Checking sounds...\n");
		cls.downloadnumber = 1;
	}

	cls.downloadtype = dl_sound;
	for(; cl.sound_name[cls.downloadnumber][0]; cls.downloadnumber++)
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
	cl_spikeindex = -1;
	cl_flagindex = -1;
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
	int size, percent;
	char name[1024]; // MAX_OSPATH
	int r;

	// read the data
	size = MSG_ReadShort();
	percent = MSG_ReadByte();

	if(cls.demoplayback)
	{
		if(size > 0)
			msg_readcount += size;
		return; // not in demo playback
	}

	if(size == -1)
	{
		Con_Printf("File not found.\n"); // "Server does not have this file.\n"
		if(cls.download)
		{
			// if here, we tried to resume a file but the server said no
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

		cls.download = NULL;
		cls.downloadpercent = 0;

		// get another file if needed

		CL_RequestNextDownload();
	}
}

static byte *upload_data;
static int upload_pos;
static int upload_size;

void CL_NextUpload()
{
	byte buffer[1024];
	int percent;
	int size;

	if(!upload_data)
		return;

	int r = upload_size - upload_pos;

	if(r > 768)
		r = 768;

	memcpy(buffer, upload_data + upload_pos, r);
	MSG_WriteByte(&cls.netchan.message, clc_upload);
	MSG_WriteShort(&cls.netchan.message, r);

	upload_pos += r;
	size = upload_size;
	if(!size)
		size = 1;
	percent = upload_pos * 100 / size;
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
	upload_pos = 0;

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
	char *str;
	FILE *f;
	char fn[MAX_OSPATH];
	qboolean cflag = false;
	extern char gamedirfile[MAX_OSPATH];
	int protover;

	Con_DPrintf("Serverdata packet received.\n");
	//
	// wipe the client_state_t struct
	//
	CL_ClearState();

	// parse protocol version number
	// allow 2.2 and 2.29 demos to play
	protover = MSG_ReadLong();
	if(protover != PROTOCOL_VERSION &&
	   !(cls.demoplayback &&
	     (protover == 26 || protover == 27 || protover == 28)))
		Host_EndGame("Server returned version %i, not %i\nYou probably need to upgrade.", // reinstall the OS
		             protover,
		             PROTOCOL_VERSION);

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

	// Run the autoexec.cfg in the gamedir if it exists
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
	movevars.gravity = MSG_ReadFloat();
	movevars.stopspeed = MSG_ReadFloat();
	movevars.maxspeed = MSG_ReadFloat();
	movevars.spectatormaxspeed = MSG_ReadFloat();
	movevars.accelerate = MSG_ReadFloat();
	movevars.airaccelerate = MSG_ReadFloat();
	movevars.wateraccelerate = MSG_ReadFloat();
	movevars.friction = MSG_ReadFloat();
	movevars.waterfriction = MSG_ReadFloat();
	movevars.entgravity = MSG_ReadFloat();

	// seperate the printfs so the server message can have a color
	Con_Printf("\n\n\35\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36"
	           "\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\36\37\n\n");
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
	char *str;

	// precache sounds
	//	memset (cl.sound_precache, 0, sizeof(cl.sound_precache));

	int numsounds = MSG_ReadByte();

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

	int n = MSG_ReadByte();

	if(n)
	{
		MSG_WriteByte(&cls.netchan.message, clc_stringcmd);
		//		MSG_WriteString (&cls.netchan.message, va("soundlist %i %i",
		//cl.servercount, n));
		MSG_WriteString(&cls.netchan.message,
		                va(soundlist_name, cl.servercount, n));
		return;
	}

	cls.downloadnumber = 0;
	cls.downloadtype = dl_sound;
	Sound_NextDownload();
}

/*
==================
CL_ParseModellist
==================
*/
void CL_ParseModellist()
{
	int nummodels;
	char *str;
	int n;

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
		//		MSG_WriteString (&cls.netchan.message, va("modellist %i %i",
		//cl.servercount, n));
		MSG_WriteString(&cls.netchan.message,
		                va(modellist_name, cl.servercount, n));
		return;
	}

	cls.downloadnumber = 0;
	cls.downloadtype = dl_model;
	Model_NextDownload();
}





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

	int i, j;
	int top, bottom;
	byte *dest, *source;
	player_info_t *player;
	char s[512];

	if(slot > MAX_CLIENTS) // cl.maxclients
		Sys_Error("CL_NewTranslation: slot > MAX_CLIENTS"); // cl.maxclients

	player = &cl.players[slot];

	strcpy(s, Info_ValueForKey(player->userinfo, "skin"));
	COM_StripExtension(s, s);
	
	if(player->skin && !stricmp(s, player->skin->name))
		player->skin = NULL;

	if(player->topcolor != player->topcolor ||
	   player->bottomcolor != player->bottomcolor || !player->skin)
	{
		player->topcolor = player->topcolor;
		player->bottomcolor = player->bottomcolor;

		dest = player->translations;
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
				for(j = 0; j < 16; j++)
					dest[TOP_RANGE + j] = source[top + 15 - j];

			if(bottom < 128)
				memcpy(dest + BOTTOM_RANGE, source + bottom, 16);
			else
				for(j = 0; j < 16; j++)
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
	player->topcolor = atoi(Info_ValueForKey(player->userinfo, "topcolor"));
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
	player_info_t *player;

	int slot = MSG_ReadByte();
	if(slot >= MAX_CLIENTS)
		Host_EndGame("CL_ParseServerMessage: svc_updateuserinfo > MAX_SCOREBOARD");

	player = &cl.players[slot];
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
	player_info_t *player;
	char key[MAX_MSGLEN];
	char value[MAX_MSGLEN];

	int slot = MSG_ReadByte();
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
	vec3_t fv, rv, uv;
	dlight_t *dl;
	int i;
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
	dl->radius = 200 + (rand() & 31);
	dl->minlight = 32;
	dl->die = cl.time + 0.1;
	dl->color[0] = 0.2;
	dl->color[1] = 0.1;
	dl->color[2] = 0.05;
	dl->color[3] = 0.7;
}