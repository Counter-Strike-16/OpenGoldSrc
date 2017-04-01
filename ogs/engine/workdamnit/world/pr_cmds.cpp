/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2016-2017 OGS Dev Team
 *
 *	OGS Engine is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	OGS Engine is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OGS Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	In addition, as a special exception, the author gives permission to
 *	link the code of OGS Engine with the Half-Life Game Engine ("GoldSrc/GS
 *	Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *	L.L.C ("Valve").  You must obey the GNU General Public License in all
 *	respects for all of the code used other than the GoldSrc Engine and MODs
 *	from Valve.  If you modify this file, you may extend this exception
 *	to your version of the file, but you are not obligated to do so.  If
 *	you do not wish to do so, delete this exception statement from your
 *	version.
 */

/// @file

#include "precompiled.hpp"
#include "world/pr_cmds.hpp"
#include "system/server.hpp"
#include "system/common.hpp"
#include "system/host.hpp"
#include "system/system.hpp"
#include "system/sizebuf.hpp"
#include "filesystem/filesystem_internal.hpp"
#include "resources/decal.hpp"
#include "resources/cmodel.hpp"
#include "resources/model_rehlds.hpp"
#include "resources/studio_rehlds.hpp"
#include "network/net_msg.hpp"
#include "console/cmd.hpp"
#include "console/console.hpp"
#include "physics/pmove.hpp"
#include "physics/sv_move.hpp"
#include "physics/sv_phys.hpp"
#include "world/world.hpp"
#include "world/pr_edict.hpp"
#include "network/sv_user.hpp"
#include "sound/sound.hpp"
#include "system/client.hpp"

vec_t gHullMins[4][3] = {
	{ 0.0f, 0.0f, 0.0f },
	{ -16.0f, -16.0f, -36.0f },
	{ -32.0f, -32.0f, -32.0f },
	{ -16.0f, -16.0f, -18.0f },
};

vec_t gHullMaxs[4][3] = {
	{ 0.0f, 0.0f, 0.0f },
	{ 16.0f, 16.0f, 36.0f },
	{ 32.0f, 32.0f, 32.0f },
	{ 16.0f, 16.0f, 18.0f },
};

unsigned char gMsgData[512];

edict_t *gMsgEntity;
int gMsgDest;
int gMsgType;
qboolean gMsgStarted;
vec3_t gMsgOrigin;
int32 idum;
int g_groupop;
int g_groupmask;
unsigned char checkpvs[1024];
int c_invis;
int c_notvis;

// TODO: Move to sv_phys.cpp
vec3_t vec_origin;
int r_visframecount;

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

sizebuf_t gMsgBuffer = { "MessageBegin/End", 0, gMsgData, sizeof(gMsgData), 0 };

#else // HOOK_ENGINE

sizebuf_t gMsgBuffer;

#endif // HOOK_ENGINE

void EXT_FUNC SetMinMaxSize(edict_t *e, const float *min, const float *max, qboolean rotate)
{
	for(int i = 0; i < 3; i++)
	{
		if(min[i] > max[i])
			Host_Error("backwards mins/maxs");
	}

	e->v.mins[0] = min[0];
	e->v.mins[1] = min[1];
	e->v.mins[2] = min[2];

	e->v.maxs[0] = max[0];
	e->v.maxs[1] = max[1];
	e->v.maxs[2] = max[2];

	e->v.size[0] = max[0] - min[0];
	e->v.size[1] = max[1] - min[1];
	e->v.size[2] = max[2] - min[2];
	SV_LinkEdict(e, 0);
}

void EXT_FUNC PF_bprint(char *s)
{
	SV_BroadcastPrintf("%s", s);
}

void EXT_FUNC PF_sprint(char *s, int entnum)
{
	if(entnum <= 0 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to sprint to a non-client\n");
		return;
	}

	client_t *client = &g_psvs.clients[entnum - 1];
	if(!client->fakeclient)
	{
#ifdef REHLDS_FIXES
		MSG_WriteByte(&client->netchan.message, svc_print);
#else  // REHLDS_FIXES
		MSG_WriteChar(&client->netchan.message, svc_print);
#endif // REHLDS_FIXES
		MSG_WriteString(&client->netchan.message, s);
	}
}

void EXT_FUNC PF_traceline_Shared(const float *v1, const float *v2, int nomonsters, edict_t *ent)
{
#ifdef REHLDS_OPT_PEDANTIC
	trace_t trace = SV_Move_Point(v1, v2, nomonsters, ent);
#else  // REHLDS_OPT_PEDANTIC
	trace_t trace = SV_Move(v1, vec3_origin, vec3_origin, v2, nomonsters, ent, 0);
#endif // REHLDS_OPT_PEDANTIC

	gGlobalVariables.trace_flags = 0;
	SV_SetGlobalTrace(&trace);
}

msurface_t *EXT_FUNC SurfaceAtPoint(model_t *pModel, mnode_t *node, vec_t *start, vec_t *end)
{
	mplane_t *plane;
	int s;
	int t;
	msurface_t *surf;
	mtexinfo_t *tex;
	int ds;
	int dt;
	vec3_t mid;
	float back;
	float front;
	float frac;

	if(node->contents < 0)
		return 0;

	plane = node->plane;
	front = _DotProduct(start, plane->normal) - plane->dist;
	back = _DotProduct(end, plane->normal) - plane->dist;
	s = (front < 0.0f) ? 1 : 0;
	t = (back < 0.0f) ? 1 : 0;
	if(t == s)
		return SurfaceAtPoint(pModel, node->children[s], start, end);

	frac = front / (front - back);
	mid[0] = (end[0] - start[0]) * frac + start[0];
	mid[1] = (end[1] - start[1]) * frac + start[1];
	mid[2] = (end[2] - start[2]) * frac + start[2];
	surf = SurfaceAtPoint(pModel, node->children[s], start, mid);
	if(surf)
		return surf;

	/* Unreachable code
  if (t == s)
          return NULL;
  */

	for(int i = 0; i < node->numsurfaces; i++)
	{
		surf = &pModel->surfaces[node->firstsurface + i];
		tex = surf->texinfo;
		ds = (int)(_DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3]);
		dt = (int)(_DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3]);
		if(ds >= surf->texturemins[0])
		{
			if(dt >= surf->texturemins[1])
			{
				if(ds - surf->texturemins[0] <= surf->extents[0] &&
				   dt - surf->texturemins[1] <= surf->extents[1])
					return surf;
			}
		}
	}

	return SurfaceAtPoint(pModel, node->children[s ^ 1], mid, end);
}



void EXT_FUNC PF_TraceToss_Shared(edict_t *ent, edict_t *ignore)
{
	trace_t trace = SV_Trace_Toss(ent, ignore);
	SV_SetGlobalTrace(&trace);
}

void EXT_FUNC SV_SetGlobalTrace(trace_t *ptrace)
{
	gGlobalVariables.trace_fraction = ptrace->fraction;
	gGlobalVariables.trace_allsolid = (float)ptrace->allsolid;
	gGlobalVariables.trace_startsolid = (float)ptrace->startsolid;
	gGlobalVariables.trace_endpos[0] = ptrace->endpos[0];
	gGlobalVariables.trace_endpos[1] = ptrace->endpos[1];
	gGlobalVariables.trace_endpos[2] = ptrace->endpos[2];
	gGlobalVariables.trace_plane_normal[0] = ptrace->plane.normal[0];
	gGlobalVariables.trace_plane_normal[2] = ptrace->plane.normal[2];
	gGlobalVariables.trace_plane_normal[1] = ptrace->plane.normal[1];
	gGlobalVariables.trace_inwater = (float)ptrace->inwater;
	gGlobalVariables.trace_inopen = (float)ptrace->inopen;
	gGlobalVariables.trace_plane_dist = ptrace->plane.dist;
	if(ptrace->ent)
	{
		gGlobalVariables.trace_ent = ptrace->ent;
		gGlobalVariables.trace_hitgroup = ptrace->hitgroup;
	}
	else
	{
		gGlobalVariables.trace_hitgroup = ptrace->hitgroup;
		gGlobalVariables.trace_ent = &g_psv.edicts[0];
	}
}





int EXT_FUNC PF_newcheckclient(int check)
{
	int i;
	unsigned char *pvs;
	edict_t *ent;
	mleaf_t *leaf;
	vec3_t org;

	if(check < 1)
		check = 1;
	if(check > g_psvs.maxclients)
		check = g_psvs.maxclients;
	i = 1;
	if(check != g_psvs.maxclients)
		i = check + 1;
	while(1)
	{
		if(i == g_psvs.maxclients + 1)
			i = 1;

		ent = &g_psv.edicts[i];
		if(i == check)
			break;
		if(!ent->free && ent->pvPrivateData && !(ent->v.flags & FL_NOTARGET))
			break;
		++i;
	}
	org[0] = ent->v.view_ofs[0] + ent->v.origin[0];
	org[1] = ent->v.view_ofs[1] + ent->v.origin[1];
	org[2] = ent->v.view_ofs[2] + ent->v.origin[2];
	leaf = Mod_PointInLeaf(org, g_psv.worldmodel);
	pvs = Mod_LeafPVS(leaf, g_psv.worldmodel);
	Q_memcpy(checkpvs, pvs, (g_psv.worldmodel->numleafs + 7) >> 3);
	return i;
}



mnode_t *EXT_FUNC PVSNode(mnode_t *node, vec_t *emins, vec_t *emaxs)
{
	mplane_t *splitplane;
	int sides;
	mnode_t *splitNode;

	if(node->visframe != r_visframecount)
		return NULL;

	if(node->contents < 0)
		return node->contents != CONTENT_SOLID ? node : NULL;

	splitplane = node->plane;
	if(splitplane->type >= 3u)
	{
		sides = BoxOnPlaneSide(emins, emaxs, splitplane);
	}
	else
	{
		if(splitplane->dist > emins[splitplane->type])
		{
			if(splitplane->dist < emaxs[splitplane->type])
				sides = 3;
			else
				sides = 2;
		}
		else
		{
			sides = 1;
		}
	}

	if(sides & 1)
	{
		splitNode = PVSNode(node->children[0], emins, emaxs);
		if(splitNode)
			return splitNode;
	}

	if(sides & 2)
		return PVSNode(node->children[1], emins, emaxs);

	return NULL;
}

void EXT_FUNC PVSMark(model_t *pmodel, unsigned char *ppvs)
{
	++r_visframecount;
	for(int i = 0; i < pmodel->numleafs; i++)
	{
		if((1 << (i & 7)) & ppvs[i >> 3])
		{
			mnode_t *node = (mnode_t *)&pmodel->leafs[i + 1];
			do
			{
				if(node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while(node);
		}
	}
}



qboolean EXT_FUNC ValidCmd(const char *pCmd)
{
	int len = Q_strlen(pCmd);
	return len && (pCmd[len - 1] == '\n' || pCmd[len - 1] == ';');
}



void EXT_FUNC PF_Remove_I_internal(edict_t *ed)
{
	ED_Free(ed);
}

edict_t *EXT_FUNC PF_find_Shared(int eStartSearchAfter, int iFieldToMatch, const char *szValueToFind)
{
	for(int e = eStartSearchAfter + 1; e < g_psv.num_edicts; e++)
	{
		edict_t *ed = &g_psv.edicts[e];
		if(ed->free)
			continue;

		char *t = &pr_strings[*(string_t *)((size_t)&ed->v + iFieldToMatch)];
		if(t == 0 || t == &pr_strings[0])
			continue;

		if(!Q_strcmp(t, szValueToFind))
			return ed;
	}
	return &g_psv.edicts[0];
}

int EXT_FUNC iGetIndex(const char *pszField)
{
	char sz[512];

	Q_strncpy(sz, pszField, sizeof(sz) - 1);
	sz[sizeof(sz) - 1] = 0;
	Q_strlwr(sz);

#define IGETINDEX_CHECK_FIELD(f) \
	if(!Q_strcmp(sz, #f))        \
		return offsetof(entvars_t, f);

	IGETINDEX_CHECK_FIELD(classname);
	IGETINDEX_CHECK_FIELD(model);
	IGETINDEX_CHECK_FIELD(viewmodel);
	IGETINDEX_CHECK_FIELD(weaponmodel);
	IGETINDEX_CHECK_FIELD(netname);
	IGETINDEX_CHECK_FIELD(target);
	IGETINDEX_CHECK_FIELD(targetname);
	IGETINDEX_CHECK_FIELD(message);
	IGETINDEX_CHECK_FIELD(noise);
	IGETINDEX_CHECK_FIELD(noise1);
	IGETINDEX_CHECK_FIELD(noise2);
	IGETINDEX_CHECK_FIELD(noise3);
	IGETINDEX_CHECK_FIELD(globalname);

	return -1;
}



qboolean EXT_FUNC PR_IsEmptyString(const char *s)
{
	return s[0] < ' ';
}

unsigned short EXT_FUNC EV_Precache(int type, const char *psz)
{
	if(!psz)
		Host_Error("%s: NULL pointer", __FUNCTION__);

	if(PR_IsEmptyString(psz))
		Host_Error("%s: Bad string '%s'", __FUNCTION__, psz);

	if(g_psv.state == ss_loading)
	{
		for(int i = 1; i < MAX_EVENTS; i++)
		{
			struct event_s *ev = &g_psv.event_precache[i];
			if(!ev->filename)
			{
				if(type != 1)
					Host_Error("%s:  only file type 1 supported currently\n", __FUNCTION__);

				char szpath[MAX_PATH];
				Q_snprintf(szpath, sizeof(szpath), "%s", psz);
				COM_FixSlashes(szpath);

				int scriptSize = 0;
				char *evScript = (char *)COM_LoadFile(szpath, 5, &scriptSize);
				if(!evScript)
					Host_Error("%s:  file %s missing from server\n", __FUNCTION__, psz);
#ifdef REHLDS_FIXES
				// Many modders don't know that the resource names passed to precache
				// functions must be a static strings.
				// Also some metamod modules can be unloaded during the server running.
				// Thereafter the pointers to resource names, precached by this modules
				// becomes invalid and access to them will cause segfault error.
				// Reallocating strings in the rehlds temporary hunk memory helps to
				// avoid these problems.
				g_psv.event_precache[i].filename = ED_NewString(psz);
#else
				g_psv.event_precache[i].filename = psz;
#endif // REHLDS_FIXES
				g_psv.event_precache[i].filesize = scriptSize;
				g_psv.event_precache[i].pszScript = evScript;
				g_psv.event_precache[i].index = i;

				return i;
			}

			if(!Q_stricmp(ev->filename, psz))
				return i;
		}
		Host_Error("%s: '%s' overflow", __FUNCTION__, psz);
	}
	else
	{
		for(int i = 1; i < MAX_EVENTS; i++)
		{
			struct event_s *ev = &g_psv.event_precache[i];
			if(!Q_stricmp(ev->filename, psz))
				return i;
		}

		Host_Error("%s: '%s' Precache can only be done in spawn functions",
		           __FUNCTION__, psz);
	}
}

void EXT_FUNC EV_PlayReliableEvent_api(IGameClient *cl, int entindex, unsigned short eventindex, float delay, event_args_t *pargs)
{
	EV_PlayReliableEvent_internal(cl->GetClient(), entindex, eventindex, delay, pargs);
}

void EV_PlayReliableEvent(client_t *cl, int entindex, unsigned short eventindex, float delay, event_args_t *pargs)
{
	g_RehldsHookchains.m_EV_PlayReliableEvent.callChain(
	EV_PlayReliableEvent_api, GetRehldsApiClient(cl), entindex, eventindex, delay, pargs);
}

void EV_PlayReliableEvent_internal(client_t *cl, int entindex, unsigned short eventindex, float delay, event_args_t *pargs)
{
	unsigned char data[1024];
	event_args_t from;
	event_args_t to;
	sizebuf_t msg;

	if(cl->fakeclient)
		return;

	Q_memset(&msg, 0, sizeof(msg));
	msg.buffername = "Reliable Event";
	msg.data = data;
	msg.cursize = 0;
	msg.maxsize = sizeof(data);

	Q_memset(&from, 0, sizeof(from));
	to = *pargs;
	to.entindex = entindex;

	MSG_WriteByte(&msg, svc_event_reliable);
	MSG_StartBitWriting(&msg);
	MSG_WriteBits(eventindex, 10);
	DELTA_WriteDelta((unsigned char *)&from, (unsigned char *)&to, 1, g_peventdelta, 0);
	if(delay == 0.0)
	{
		MSG_WriteBits(0, 1);
	}
	else
	{
		MSG_WriteBits(1, 1);
		MSG_WriteBits((int)(delay * 100.0f), 16);
	}
	MSG_EndBitWriting(&msg);

	if(msg.cursize + cl->netchan.message.cursize <= cl->netchan.message.maxsize)
		SZ_Write(&cl->netchan.message, msg.data, msg.cursize);
	else
		Netchan_CreateFragments(1, &cl->netchan, &msg);
}

void EXT_FUNC EV_Playback(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	client_t *cl;
	signed int j;
	event_args_t eargs;
	vec3_t event_origin;
	int invoker;
	int slot;
	int leafnum;

	if(flags & FEV_CLIENT)
		return;

	invoker = -1;
	Q_memset(&eargs, 0, sizeof(eargs));
	if(!VectorCompare(origin, vec3_origin))
	{
		eargs.origin[2] = origin[2];
		eargs.origin[0] = origin[0];
		eargs.origin[1] = origin[1];
		eargs.flags |= FEVENT_ORIGIN;
	}
	if(!VectorCompare(angles, vec3_origin))
	{
		eargs.angles[2] = angles[2];
		eargs.angles[0] = angles[0];
		eargs.angles[1] = angles[1];
		eargs.flags |= FEVENT_ANGLES;
	}
	eargs.fparam1 = fparam1;
	eargs.fparam2 = fparam2;
	eargs.iparam1 = iparam1;
	eargs.iparam2 = iparam2;
	eargs.bparam1 = bparam1;
	eargs.bparam2 = bparam2;

	if(eventindex < 1u || eventindex >= MAX_EVENTS)
	{
		Con_DPrintf("%s:  index out of range %i\n", __FUNCTION__, eventindex);
		return;
	}

	if(!g_psv.event_precache[eventindex].pszScript)
	{
		Con_DPrintf("%s:  no event for index %i\n", __FUNCTION__, eventindex);
		return;
	}

	if(pInvoker)
	{
		event_origin[0] = pInvoker->v.origin[0];
		event_origin[1] = pInvoker->v.origin[1];
		event_origin[2] = pInvoker->v.origin[2];
		invoker = NUM_FOR_EDICT(pInvoker);
		if(invoker >= 1)
		{
			if(invoker <= g_psvs.maxclients)
			{
				if(pInvoker->v.flags & FL_DUCKING)
					eargs.ducking = 1;
			}
		}
		if(!(eargs.flags & FEVENT_ORIGIN))
		{
			eargs.origin[0] = pInvoker->v.origin[0];
			eargs.origin[1] = pInvoker->v.origin[1];
			eargs.origin[2] = pInvoker->v.origin[2];
		}
		if(!(eargs.flags & FEVENT_ANGLES))
		{
			eargs.angles[0] = pInvoker->v.angles[0];
			eargs.angles[1] = pInvoker->v.angles[1];
			eargs.angles[2] = pInvoker->v.angles[2];
		}
	}
	else
	{
		event_origin[0] = eargs.origin[0];
		event_origin[1] = eargs.origin[1];
		event_origin[2] = eargs.origin[2];
	}

	leafnum = SV_PointLeafnum(event_origin);

	for(slot = 0; slot < g_psvs.maxclients; slot++)
	{
		cl = &g_psvs.clients[slot];
		if(!cl->active || !cl->spawned || !cl->connected || !cl->fully_connected ||
		   cl->fakeclient)
			continue;

		if(pInvoker)
		{
			if(pInvoker->v.groupinfo)
			{
				if(cl->edict->v.groupinfo)
				{
					if(g_groupop)
					{
						if(g_groupop == GROUP_OP_NAND &&
						   (cl->edict->v.groupinfo & pInvoker->v.groupinfo))
							continue;
					}
					else
					{
						if(!(cl->edict->v.groupinfo & pInvoker->v.groupinfo))
							continue;
					}
				}
			}
		}

		if(pInvoker && !(flags & FEV_GLOBAL))
		{
			if(!SV_ValidClientMulticast(cl, leafnum, 4))
				continue;
		}

		if(cl == host_client && (flags & FEV_NOTHOST) && cl->lw ||
		   (flags & FEV_HOSTONLY) && cl->edict != pInvoker)
			continue;

		if(flags & FEV_RELIABLE)
		{
			EV_PlayReliableEvent(cl, pInvoker ? NUM_FOR_EDICT(pInvoker) : 0, eventindex, delay, &eargs);
			continue;
		}

		if(flags & FEV_UPDATE)
		{
			for(j = 0; j < MAX_EVENT_QUEUE; j++)
			{
				event_info_s *ei = &cl->events.ei[j];
				if(ei->index == eventindex && invoker != -1 &&
				   ei->entity_index == invoker)
					break;
			}

			if(j < MAX_EVENT_QUEUE)
			{
				event_info_s *ei = &cl->events.ei[j];
				ei->entity_index = -1;
				ei->index = eventindex;
				ei->packet_index = -1;
				if(pInvoker)
					ei->entity_index = invoker;
				Q_memcpy(&ei->args, &eargs, sizeof(ei->args));
				ei->fire_time = delay;
				continue;
			}
		}

		for(j = 0; j < MAX_EVENT_QUEUE; j++)
		{
			event_info_s *ei = &cl->events.ei[j];
			if(ei->index == 0)
				break;
		}

		if(j < MAX_EVENT_QUEUE)
		{
			event_info_s *ei = &cl->events.ei[j];
			ei->entity_index = -1;
			ei->index = eventindex;
			ei->packet_index = -1;
			if(pInvoker)
				ei->entity_index = invoker;
			Q_memcpy(&ei->args, &eargs, sizeof(ei->args));
			ei->fire_time = delay;
		}
	}
}

void EXT_FUNC EV_SV_Playback(int flags, int clientindex, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	if(flags & FEV_CLIENT)
		return;

	if(clientindex < 0 || clientindex >= g_psvs.maxclients)
		Host_Error("%s:  Client index %i out of range\n", __FUNCTION__, clientindex);

	edict_t *pEdict = g_psvs.clients[clientindex].edict;
	EV_Playback(flags, pEdict, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}

#ifdef REHLDS_FIXES
int SV_LookupModelIndex(const char *name)
{
	if(!name || !name[0])
		return 0;

#ifdef REHLDS_OPT_PEDANTIC
	auto node = g_rehlds_sv.modelsMap.get(name);
	if(node)
	{
		return node->val;
	}
#else  // REHLDS_OPT_PEDANTIC
	for(int i = 0; i < MAX_MODELS; i++)
	{
		if(!g_psv.model_precache[i])
			break;

		if(!Q_stricmp(g_psv.model_precache[i], name))
			return i;
	};
#endif // REHLDS_OPT_PEDANTIC

	return 0;
}
#endif // REHLDS_FIXES

sizebuf_t *EXT_FUNC WriteDest_Parm(int dest)
{
	int entnum;

	switch(dest)
	{
	case MSG_BROADCAST:
		return &g_psv.datagram;
	case MSG_ONE:
	case MSG_ONE_UNRELIABLE:
		entnum = NUM_FOR_EDICT(gMsgEntity);
		if(entnum <= 0 || entnum > g_psvs.maxclients)
		{
			Host_Error("%s: not a client", __FUNCTION__);
		}
		if(dest == MSG_ONE)
		{
			return &g_psvs.clients[entnum - 1].netchan.message;
		}
		else
		{
			return &g_psvs.clients[entnum - 1].datagram;
		}
	case MSG_INIT:
		return &g_psv.signon;
	case MSG_ALL:
		return &g_psv.reliable_datagram;
	case MSG_PVS:
	case MSG_PAS:
		return &g_psv.multicast;
	case MSG_SPEC:
		return &g_psv.spectator;
	default:
		Host_Error("%s: bad destination=%d", __FUNCTION__, dest);
	}
}

void SeedRandomNumberGenerator()
{
	idum = -(int)CRehldsPlatformHolder::get()->time(NULL);
	if(idum > 1000)
	{
		idum = -idum;
	}
	else if(idum > -1000)
	{
		idum -= 22261048;
	}
}

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)

int32 ran1()
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];

	if(idum <= 0 || !iy)
	{
		if(-(idum) < 1)
			idum = 1;
		else
			idum = -(idum);
		for(j = NTAB + 7; j >= 0; j--)
		{
			k = (idum) / IQ;
			idum = IA * (idum - k * IQ) - IR * k;
			if(idum < 0)
				idum += IM;
			if(j < NTAB)
				iv[j] = idum;
		}
		iy = iv[0];
	}
	k = (idum) / IQ;
	idum = IA * (idum - k * IQ) - IR * k;
	if(idum < 0)
		idum += IM;
	j = iy / NDIV;
	iy = iv[j];
	iv[j] = idum;

	return iy;
}

#define AM (1.0 / IM)
#define EPS 1.2e-7
#define RNMX (1.0 - EPS)

float fran1()
{
	float temp = (float)AM * ran1();
	if(temp > RNMX)
		return (float)RNMX;
	else
		return temp;
}

void EXT_FUNC PF_BuildSoundMsg_I_internal(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	PF_MessageBegin_I(msg_dest, msg_type, pOrigin, ed);
	SV_BuildSoundMsg(entity, channel, sample, (int)volume, attenuation, fFlags, pitch, pOrigin, &gMsgBuffer);
	PF_MessageEnd_I();
}

NOXREF void QueryClientCvarValueCmd()
{
	NOXREFCHECK;
	
	if(Cmd_Argc() <= 1)
	{
		Con_Printf("%s <player name> <cvar>\n", Cmd_Argv(0));
		return;
	}
	
	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *cl = &g_psvs.clients[i];

		if(cl->active || cl->connected)
		{
			if(!Q_stricmp(cl->name, Cmd_Argv(1)))
			{
				QueryClientCvarValue(cl->edict, Cmd_Argv(2));
				break;
			}
		}
	}
}

NOXREF void QueryClientCvarValueCmd2()
{
	NOXREFCHECK;
	
	int i;
	client_t *cl;
	int requestID;

	if(Cmd_Argc() < 3)
	{
		Con_Printf("%s <player name> <cvar> <requestID>", Cmd_Argv(0));
		return;
	}
	requestID = Q_atoi(Cmd_Argv(3));
	for(i = 0; i < g_psvs.maxclients; i++)
	{
		cl = &g_psvs.clients[i];

		if(cl->active || cl->connected)
		{
			if(!Q_stricmp(cl->name, Cmd_Argv(1)))
			{
				QueryClientCvarValue2(cl->edict, Cmd_Argv(2), requestID);
				break;
			}
		}
	}
}



int hudCheckParm(char *parm, char **ppnext)
{
#ifndef SWDS
	g_engdstAddrs.CheckParm(&parm, &ppnext);
#endif

	int i = COM_CheckParm(parm);
	if(ppnext)
	{
		if(i && i < com_argc - 1)
			*ppnext = com_argv[i + 1];
		else
			*ppnext = 0;
	}
	return i;
}


