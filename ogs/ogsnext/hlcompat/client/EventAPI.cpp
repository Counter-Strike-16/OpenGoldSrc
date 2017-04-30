event_api_t gEventApi =
{
	EVENT_API_VERSION,
	pfnPlaySound,
	//S_StopSound,
	CL_FindModelIndex,
	pfnIsLocal,
	pfnLocalPlayerDucking,
	pfnLocalPlayerViewheight,
	pfnLocalPlayerBounds,
	pfnIndexFromTrace,
	pfnGetPhysent,
	pfnSetUpPlayerPrediction,
	pfnPushPMStates,
	pfnPopPMStates,
	//CL_SetSolidPlayers,
	CL_SetTraceHull,
	CL_PlayerTrace,
	//CL_WeaponAnim,
	//pfnPrecacheEvent,
	//CL_PlaybackEvent,
	pfnTraceTexture,
	pfnStopAllSounds,
	pfnKillEvents,
	//CL_EventIndex,
	//CL_IndexEvent,
	CL_PlayerTraceExt,
	//CL_SoundFromIndex,
	pfnTraceSurface
};

/*
=============
pfnPlaySound

=============
*/
void pfnPlaySound(int ent, float *org, int chan, const char *samp, float vol, float attn, int flags, int pitch)
{
	S_StartSound(org, ent, chan, S_RegisterSound(samp), vol, attn, pitch, flags);
}

/*
=============
CL_FindModelIndex

=============
*/
int CL_FindModelIndex(const char *m)
{
	int	i;

	if(!m || !m[0])
		return 0;

	for(i = 1; i < MAX_MODELS && cl.model_precache[i][0]; i++)
	{
		if(!Q_stricmp(cl.model_precache[i], m))
			return i;
	}

	if(cls.state == ca_active && Q_strnicmp(m, "models/player/", 14))
	{
		// tell user about problem(but don't spam console about playermodel)
		MsgDev(D_NOTE, "CL_ModelIndex: %s not precached", m);
	}
	return 0;
}

/*
=============
pfnIsLocal

=============
*/
int pfnIsLocal(int playernum)
{
	if(playernum == cl.playernum)
		return true;
	return false;
}

/*
=============
pfnLocalPlayerDucking

=============
*/
int pfnLocalPlayerDucking()
{
	return cl.frame.local.client.bInDuck;
}

void pfnLocalPlayerViewheight(float *view_ofs)
{
	// predicted or smoothed
	if(view_ofs) VectorCopy(cl.frame.local.client.view_ofs, view_ofs);
}

void pfnLocalPlayerBounds(int hull, float *mins, float *maxs)
{
	if(hull >= 0 && hull < 4)
	{
		if(mins) VectorCopy(clgame.pmove->player_mins[hull], mins);
		if(maxs) VectorCopy(clgame.pmove->player_maxs[hull], maxs);
	}
}

int pfnIndexFromTrace(struct pmtrace_s *pTrace)
{
	if(pTrace->ent >= 0 && pTrace->ent < clgame.pmove->numphysent)
	{
		// return cl.entities number
		return clgame.pmove->physents[pTrace->ent].info;
	}
	return -1;
}

physent_t *pfnGetPhysent(int idx)
{
	if(idx >= 0 && idx < clgame.pmove->numphysent)
	{
		// return physent
		return &clgame.pmove->physents[idx];
	}
	return NULL;
}

void pfnSetUpPlayerPrediction(int dopred, int bIncludeLocalClient)
{
	// TODO: implement
}

void pfnPushPMStates()
{
	clgame.oldcount = clgame.pmove->numphysent;
}

void pfnPopPMStates()
{
	clgame.pmove->numphysent = clgame.oldcount;
}

void CL_SetTraceHull(int hull)
{
	clgame.old_trace_hull = clgame.pmove->usehull;
	clgame.pmove->usehull = bound(0, hull, 3);

}

void CL_PlayerTrace(float *start, float *end, int traceFlags, int ignore_pe, pmtrace_t *tr)
{
	if(!tr) return;
	*tr = PM_PlayerTraceExt(clgame.pmove, start, end, traceFlags, clgame.pmove->numphysent, clgame.pmove->physents, ignore_pe, NULL);
	clgame.pmove->usehull = clgame.old_trace_hull;	// restore old trace hull 
}

static const char *pfnTraceTexture(int ground, float *vstart, float *vend)
{
	physent_t *pe;

	if(ground < 0 || ground >= clgame.pmove->numphysent)
		return NULL; // bad ground

	pe = &clgame.pmove->physents[ground];
	return PM_TraceTexture(pe, vstart, vend);
}

void pfnStopAllSounds(int ent, int entchannel)
{
	S_StopSound(ent, entchannel, NULL);
}

static void pfnKillEvents(int entnum, const char *eventname)
{
	int		i;
	event_state_t	*es;
	event_info_t	*ei;
	int		eventIndex = CL_EventIndex(eventname);

	if(eventIndex < 0 || eventIndex >= MAX_EVENTS)
		return;

	if(entnum < 0 || entnum > clgame.maxEntities)
		return;

	es = &cl.events;

	// find all events with specified index and kill it
	for(i = 0; i < MAX_EVENT_QUEUE; i++)
	{
		ei = &es->ei[i];

		if(ei->index == eventIndex && ei->entity_index == entnum)
		{
			CL_ResetEvent(ei);
			break;
		}
	}
}

void CL_PlayerTraceExt(float *start, float *end, int traceFlags, int(*pfnIgnore)(physent_t *pe), pmtrace_t *tr)
{
	if(!tr) return;
	*tr = PM_PlayerTraceExt(clgame.pmove, start, end, traceFlags, clgame.pmove->numphysent, clgame.pmove->physents, -1, pfnIgnore);
	clgame.pmove->usehull = clgame.old_trace_hull;	// restore old trace hull 
}

static struct msurface_s *pfnTraceSurface(int ground, float *vstart, float *vend)
{
	physent_t *pe;

	if(ground < 0 || ground >= clgame.pmove->numphysent)
		return NULL; // bad ground

	pe = &clgame.pmove->physents[ground];
	return PM_TraceSurface(pe, vstart, vend);
}