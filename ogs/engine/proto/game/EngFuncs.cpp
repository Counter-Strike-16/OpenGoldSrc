/*
 *	This file is part of OGS Engine
 *	Copyright (C) 2017 OGS Dev Team
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

#include "precompiled.hpp"
#include "system/common.hpp"
#include "system/system.hpp"
#include "system/host_cmd.hpp"
#include "math/mathlib_e.hpp"
#include "world/pr_cmds.hpp"
#include "world/pr_edict.hpp"
#include "physics/sv_move.hpp"
#include "network/delta.hpp"
#include "resources/studio_rehlds.hpp"
#include "voice/voiceserver.hpp"
#include "console/cmd.hpp"
#include "system/server.hpp"

/*
* Globals initialization
*/
#ifndef HOOK_ENGINE

int EXT_FUNC PF_precache_model_I(const char *s)
{
	int iOptional = 0;
	if(!s)
		Host_Error("%s: NULL pointer", __FUNCTION__);

	if(PR_IsEmptyString(s))
		Host_Error("%s: Bad string '%s'", __FUNCTION__, s);

	if(*s == '!')
	{
		s++;
		iOptional = 1;
	}

	if(g_psv.state == ss_loading)
	{
		for(int i = 0; i < MAX_MODELS; i++)
		{
			if(!g_psv.model_precache[i])
			{
#ifdef REHLDS_FIXES
				// For more information, see EV_Precache
				g_psv.model_precache[i] = ED_NewString(s);
#else
				g_psv.model_precache[i] = s;
#endif // REHLDS_FIXES

#ifdef REHLDS_OPT_PEDANTIC
				g_rehlds_sv.modelsMap.put(g_psv.model_precache[i], i);
#endif // REHLDS_OPT_PEDANTIC

				g_psv.models[i] = Mod_ForName(s, TRUE, TRUE);
				if(!iOptional)
					g_psv.model_precache_flags[i] |= RES_FATALIFMISSING;

				return i;
			}

// use case-sensitive names to increase performance
#ifdef REHLDS_FIXES
			if(!Q_strcmp(g_psv.model_precache[i], s))
				return i;
#else
			if(!Q_stricmp(g_psv.model_precache[i], s))
				return i;
#endif
		}
		Host_Error("%s: Model '%s' failed to precache because the "
		           "item count is over the %d limit.\nReduce the number of brush "
		           "models and/or regular models in the map to correct this.",
				   __FUNCTION__,
		           s,
		           MAX_MODELS);
	}
	else
	{
		for(int i = 0; i < MAX_MODELS; i++)
		{
// use case-sensitive names to increase performance
#ifdef REHLDS_FIXES
			if(!Q_strcmp(g_psv.model_precache[i], s))
				return i;
#else
			if(!Q_stricmp(g_psv.model_precache[i], s))
				return i;
#endif
		}
		Host_Error("%s: '%s' Precache can only be done in spawn functions", __FUNCTION__, s);
	}
};

int EXT_FUNC PF_precache_sound_I(const char *s)
{
	int i;

	if(!s)
		Host_Error("%s: NULL pointer", __FUNCTION__);

	if(PR_IsEmptyString(s))
		Host_Error("%s: Bad string '%s'", __FUNCTION__, s);

	if(s[0] == '!')
		Host_Error("%s: '%s' do not precache sentence names!", __FUNCTION__, s);

	if(g_psv.state == ss_loading)
	{
		g_psv.sound_precache_hashedlookup_built = 0;

		for(i = 0; i < MAX_SOUNDS; i++)
		{
			if(!g_psv.sound_precache[i])
			{
#ifdef REHLDS_FIXES
				// For more information, see EV_Precache
				g_psv.sound_precache[i] = ED_NewString(s);
#else
				g_psv.sound_precache[i] = s;
#endif // REHLDS_FIXES
				return i;
			}

			if(!Q_stricmp(g_psv.sound_precache[i], s))
				return i;
		}

		Host_Error("%s: Sound '%s' failed to precache because the "
		           "item count is over the %d limit.\nReduce the number of brush "
		           "models and/or regular models in the map to correct this.",
				   __FUNCTION__,
		           s,
		           MAX_SOUNDS);
	}
	else
	{
		// precaching not enabled. check if already exists.
		for(i = 0; i < MAX_SOUNDS; i++)
		{
			if(g_psv.sound_precache[i] && !Q_stricmp(g_psv.sound_precache[i], s))
				return i;
		}

		Host_Error("%s: '%s' Precache can only be done in spawn functions", __FUNCTION__, s);
	}

	return -1; // unreachable
};

void EXT_FUNC PF_setmodel_I(edict_t *e, const char *m)
{
	const char **check = &g_psv.model_precache[0];
	int i = 0;

#ifdef REHLDS_CHECKS
	for(; *check && i < MAX_MODELS; i++, check++)
#else
	for(; *check; i++, check++)
#endif
	{
// use case-sensitive names to increase performance
#ifdef REHLDS_FIXES
		if(!Q_strcmp(*check, m))
#else
		if(!Q_stricmp(*check, m))
#endif

		{
			e->v.modelindex = i;
			model_t *mod = g_psv.models[i];
#ifdef REHLDS_FIXES
			e->v.model = *check - pr_strings;
#else  // REHLDS_FIXES
			e->v.model = m - pr_strings;
#endif // REHLDS_FIXES
			if(mod)
			{
				SetMinMaxSize(e, mod->mins, mod->maxs, 1);
			}
			else
			{
				SetMinMaxSize(e, vec3_origin, vec3_origin, 1);
			}

			return;
		}
	}

	Host_Error("no precache: %s\n", m);
};

int EXT_FUNC PF_modelindex(const char *pstr)
{
	return SV_ModelIndex(pstr);
};

int EXT_FUNC ModelFrames(int modelIndex)
{
	if(modelIndex <= 0 || modelIndex >= MAX_MODELS)
	{
		Con_DPrintf("Bad sprite index!\n");
		return 1;
	};

	return ModelFrameCount(g_psv.models[modelIndex]);
};

void EXT_FUNC PF_setsize_I(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	SetMinMaxSize(e, rgflMin, rgflMax, 0);
};

void EXT_FUNC PF_changelevel_I(const char *s1, const char *s2)
{
	static int last_spawncount;

	if(g_psvs.spawncount != last_spawncount)
	{
		last_spawncount = g_psvs.spawncount;
		SV_SkipUpdates();
		if(s2)
			Cbuf_AddText(va("changelevel2 %s %s\n", s1, s2));
		else
			Cbuf_AddText(va("changelevel %s\n", s1));
	}
};

void EXT_FUNC PF_setspawnparms_I(edict_t *ent)
{
	int i = NUM_FOR_EDICT(ent);
	if(i < 1 || i > g_psvs.maxclients)
		Host_Error("Entity is not a client");
};

float EXT_FUNC PF_vectoyaw_I(const float *rgflVector)
{
	float yaw = 0.0f;
	if(rgflVector[1] == 0.0f && rgflVector[0] == 0.0f)
		return 0.0f;

	yaw = (float)(int)floor(atan2((double)rgflVector[1], (double)rgflVector[0]) *
	                        180.0 / M_PI);
	if(yaw < 0.0)
		yaw = yaw + 360.0;

	return yaw;
};

void EXT_FUNC PF_vectoangles_I(const float *rgflVectorIn, float *rgflVectorOut)
{
	VectorAngles(rgflVectorIn, rgflVectorOut);
};

void EXT_FUNC PF_changeyaw_I(edict_t *ent)
{
	float ideal;
	float current;
	float move;
	float speed;

	current = anglemod(ent->v.angles[1]);
	ideal = ent->v.ideal_yaw;
	speed = ent->v.yaw_speed;
	if(current == ideal)
		return;

	move = ideal - current;
	if(ideal <= current)
	{
		if(move <= -180.0)
			move += 360.0;
	}
	else
	{
		if(move >= 180.0)
			move -= 360.0;
	}

	if(move <= 0.0f)
	{
		if(move < -speed)
			move = -speed;
	}
	else
	{
		if(move > speed)
			move = speed;
	}

	ent->v.angles[1] = anglemod(move + current);
};

void EXT_FUNC PF_changepitch_I(edict_t *ent)
{
	float ideal;
	float current;
	float move;
	float speed;

	current = anglemod(ent->v.angles[0]);
	ideal = ent->v.idealpitch;
	speed = ent->v.pitch_speed;
	if(current == ideal)
		return;

	move = ideal - current;
	if(ideal <= (double)current)
	{
		if(move <= -180.0)
			move += 360.0;
	}
	else
	{
		if(move >= 180.0)
			move -= 360;
	}

	if(move <= 0.0)
	{
		if(move < -speed)
			move = -speed;
	}
	else
	{
		if(move > speed)
			move = speed;
	}

	ent->v.angles[0] = anglemod(move + current);
};

edict_t *EXT_FUNC FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
	if(!pszValue)
		return NULL;

	int iField = iGetIndex(pszField);
	if(iField == -1)
		return NULL;

	return PF_find_Shared(
	pEdictStartSearchAfter ? NUM_FOR_EDICT(pEdictStartSearchAfter) : 0,
	iField,
	pszValue);
};

int EXT_FUNC GetEntityIllum(edict_t *pEnt)
{
	if(!pEnt)
		return -1;

	if(NUM_FOR_EDICT(pEnt) <= g_psvs.maxclients)
	{
		return pEnt->v.light_level;
	}
	else
	{
		if(cls.state == ca_connected || cls.state == ca_uninitialized ||
		   cls.state == ca_active)
			return 0x80;
		else
			return 0;
	}
};

edict_t *EXT_FUNC FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
	int e = pEdictStartSearchAfter ? NUM_FOR_EDICT(pEdictStartSearchAfter) : 0;

	for(int i = e + 1; i < g_psv.num_edicts; i++)
	{
		edict_t *ent = &g_psv.edicts[i];
		if(ent->free || !ent->v.classname)
			continue;

		if(i <= g_psvs.maxclients && !g_psvs.clients[i - 1].active)
			continue;

		float distSquared = 0.0;
		for(int j = 0; j < 3 && distSquared <= (rad * rad); j++)
		{
			float eorg;
			if(org[j] >= ent->v.absmin[j])
				eorg = (org[j] <= ent->v.absmax[j]) ? 0.0f : org[j] - ent->v.absmax[j];
			else
				eorg = org[j] - ent->v.absmin[j];
			distSquared = eorg * eorg + distSquared;
		}

		if(distSquared <= ((rad * rad)))
			return ent;
	}

	return &g_psv.edicts[0];
};

edict_t *EXT_FUNC PF_checkclient_I(edict_t *pEdict)
{
	edict_t *ent;
	mleaf_t *leaf;
	int l;
	vec3_t view;

	if(g_psv.time - g_psv.lastchecktime >= 0.1)
	{
		g_psv.lastcheck = PF_newcheckclient(g_psv.lastcheck);
		g_psv.lastchecktime = g_psv.time;
	}

	ent = &g_psv.edicts[g_psv.lastcheck];
	if(!ent->free && ent->pvPrivateData)
	{
		view[0] = pEdict->v.view_ofs[0] + pEdict->v.origin[0];
		view[1] = pEdict->v.view_ofs[1] + pEdict->v.origin[1];
		view[2] = pEdict->v.view_ofs[2] + pEdict->v.origin[2];
		leaf = Mod_PointInLeaf(view, g_psv.worldmodel);
		l = (leaf - g_psv.worldmodel->leafs) - 1;
		if(l >= 0 && ((1 << (l & 7)) & checkpvs[l >> 3]))
		{
			++c_invis;
			return ent;
		}
		else
		{
			++c_notvis;
			return &g_psv.edicts[0];
		}
	}
	return &g_psv.edicts[0];
};

edict_t *EXT_FUNC PVSFindEntities(edict_t *pplayer)
{
	edict_t *pent;
	edict_t *pchain;
	edict_t *pentPVS;
	vec3_t org;
	unsigned char *ppvs;
	mleaf_t *pleaf;

	org[0] = pplayer->v.view_ofs[0] + pplayer->v.origin[0];
	org[1] = pplayer->v.view_ofs[1] + pplayer->v.origin[1];
	org[2] = pplayer->v.view_ofs[2] + pplayer->v.origin[2];
	pleaf = Mod_PointInLeaf(org, g_psv.worldmodel);
	ppvs = Mod_LeafPVS(pleaf, g_psv.worldmodel);
	PVSMark(g_psv.worldmodel, ppvs);
	pchain = g_psv.edicts;

	for(int i = 1; i < g_psv.num_edicts; i++)
	{
		pent = &g_psv.edicts[i];
		if(pent->free)
			continue;

		pentPVS = pent;
		if(pent->v.movetype == MOVETYPE_FOLLOW && pent->v.aiment)
			pentPVS = pent->v.aiment;

		if(PVSNode(g_psv.worldmodel->nodes, pentPVS->v.absmin, pentPVS->v.absmax))
		{
			pent->v.chain = pchain;
			pchain = pent;
		}
	}

	if(cl.worldmodel)
	{
		// r_oldviewleaf = NULL; //clientside only
		//R_MarkLeaves();
	}
	return pchain;
};

void EXT_FUNC PF_makevectors_I(const float *rgflVector)
{
	AngleVectors(rgflVector, gGlobalVariables.v_forward, gGlobalVariables.v_right, gGlobalVariables.v_up);
};

edict_t *EXT_FUNC PF_Spawn_I()
{
	return ED_Alloc();
};

void EXT_FUNC PF_Remove_I(edict_t *ed)
{
	g_RehldsHookchains.m_PF_Remove_I.callChain(PF_Remove_I_internal, ed);
};

edict_t *EXT_FUNC CreateNamedEntity(int className)
{
	edict_t *pedict;
	ENTITYINIT pEntityInit;

	if(!className)
		Sys_Error("Spawned a NULL entity!");

	pedict = ED_Alloc();
	pedict->v.classname = className;
	pEntityInit = GetEntityInit(&pr_strings[className]);
	if(pEntityInit)
	{
		pEntityInit(&pedict->v);
		return pedict;
	}
	else
	{
		ED_Free(pedict);
		Con_DPrintf("Can't create entity: %s\n", &pr_strings[className]);
		return NULL;
	}
};

void EXT_FUNC PF_makestatic_I(edict_t *ent)
{
	MSG_WriteByte(&g_psv.signon, svc_spawnstatic);
	MSG_WriteShort(&g_psv.signon, SV_ModelIndex(&pr_strings[ent->v.model]));
	MSG_WriteByte(&g_psv.signon, ent->v.sequence);
	MSG_WriteByte(&g_psv.signon, (int)ent->v.frame);
	MSG_WriteWord(&g_psv.signon, ent->v.colormap);
	MSG_WriteByte(&g_psv.signon, ent->v.skin);

	for(int i = 0; i < 3; i++)
	{
		MSG_WriteCoord(&g_psv.signon, ent->v.origin[i]);
		MSG_WriteAngle(&g_psv.signon, ent->v.angles[i]);
	}

	MSG_WriteByte(&g_psv.signon, ent->v.rendermode);
	if(ent->v.rendermode)
	{
		MSG_WriteByte(&g_psv.signon, (int)ent->v.renderamt);
		MSG_WriteByte(&g_psv.signon, (int)ent->v.rendercolor[0]);
		MSG_WriteByte(&g_psv.signon, (int)ent->v.rendercolor[1]);
		MSG_WriteByte(&g_psv.signon, (int)ent->v.rendercolor[2]);
		MSG_WriteByte(&g_psv.signon, ent->v.renderfx);
	}

	ED_Free(ent);
};

int EXT_FUNC PF_checkbottom_I(edict_t *pEdict)
{
	return SV_CheckBottom(pEdict);
};

int EXT_FUNC PF_droptofloor_I(edict_t *ent)
{
	vec3_t end;
	trace_t trace;
	qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? 1 : 0;

	end[0] = ent->v.origin[0];
	end[1] = ent->v.origin[1];
	end[2] = ent->v.origin[2] - 256.0;
	trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, 0, ent, monsterClip);
	if(trace.allsolid)
		return -1;

	if(trace.fraction == 1.0f)
		return 0;

	ent->v.origin[0] = trace.endpos[0];
	ent->v.origin[1] = trace.endpos[1];
	ent->v.origin[2] = trace.endpos[2];
	SV_LinkEdict(ent, 0);
	ent->v.flags |= FL_ONGROUND;
	ent->v.groundentity = trace.ent;

	return 1;
};

int EXT_FUNC PF_walkmove_I(edict_t *ent, float yaw, float dist, int iMode)
{
	vec3_t move;

	if(ent->v.flags & (FL_SWIM | FL_FLY | FL_ONGROUND))
	{
		move[0] = cos(yaw * 2.0 * M_PI / 360.0) * dist;
		move[1] = sin(yaw * 2.0 * M_PI / 360.0) * dist;
		move[2] = 0;

		switch(iMode)
		{
		case 1:
			return SV_movetest(ent, move, 1);
		case 2:
			return SV_movestep(ent, move, 0);
		default:
			return SV_movestep(ent, move, 1);
		}
	}
	return 0;
};

void EXT_FUNC PF_setorigin_I(edict_t *e, const float *org)
{
	if(!e)
		return;

	e->v.origin[0] = org[0];
	e->v.origin[1] = org[1];
	e->v.origin[2] = org[2];
	SV_LinkEdict(e, FALSE);
};

void EXT_FUNC PF_sound_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch)
{
	if(volume < 0.0 || volume > 255.0)
		Sys_Error("EMIT_SOUND: volume = %i", volume);
	if(attenuation < 0.0 || attenuation > 4.0)
		Sys_Error("EMIT_SOUND: attenuation = %f", attenuation);
	if(channel < 0 || channel > 7)
		Sys_Error("EMIT_SOUND: channel = %i", channel);
	if(pitch < 0 || pitch > 255)
		Sys_Error("EMIT_SOUND: pitch = %i", pitch);
	SV_StartSound(0, entity, channel, sample, (int)(volume * 255), attenuation, fFlags, pitch);
};

void EXT_FUNC PF_ambientsound_I(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	int i;
	int soundnum;
	int ent;
	sizebuf_t *pout;

	if(samp[0] == '!')
	{
		fFlags |= SND_FL_SENTENCE;
		soundnum = Q_atoi(samp + 1);
		if(soundnum >= CVOXFILESENTENCEMAX)
		{
			Con_Printf("invalid sentence number: %s", &samp[1]);
			return;
		}
	}
	else
	{
		for(i = 0; i < MAX_SOUNDS; i++)
		{
			if(g_psv.sound_precache[i] &&
			   !Q_stricmp(g_psv.sound_precache[i], samp))
			{
				soundnum = i;
				break;
			}
		}

		if(i == MAX_SOUNDS)
		{
			Con_Printf("no precache: %s\n", samp);
			return;
		}
	}

	ent = NUM_FOR_EDICT(entity);
	pout = &g_psv.signon;
	if(!(fFlags & SND_FL_SPAWNING))
		pout = &g_psv.datagram;

	MSG_WriteByte(pout, svc_spawnstaticsound);
	MSG_WriteCoord(pout, pos[0]);
	MSG_WriteCoord(pout, pos[1]);
	MSG_WriteCoord(pout, pos[2]);

	MSG_WriteShort(pout, soundnum);
	MSG_WriteByte(pout, (vol * 255.0));
	MSG_WriteByte(pout, (attenuation * 64.0));
	MSG_WriteShort(pout, ent);
	MSG_WriteByte(pout, pitch);
	MSG_WriteByte(pout, fFlags);
};

void EXT_FUNC PF_traceline_DLL(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	PF_traceline_Shared(v1, v2, fNoMonsters, pentToSkip ? pentToSkip : &g_psv.edicts[0]);
	ptr->fAllSolid = (int)gGlobalVariables.trace_allsolid;
	ptr->fStartSolid = (int)gGlobalVariables.trace_startsolid;
	ptr->fInOpen = (int)gGlobalVariables.trace_inopen;
	ptr->fInWater = (int)gGlobalVariables.trace_inwater;
	ptr->flFraction = gGlobalVariables.trace_fraction;
	ptr->flPlaneDist = gGlobalVariables.trace_plane_dist;
	ptr->pHit = gGlobalVariables.trace_ent;
	ptr->vecEndPos[0] = gGlobalVariables.trace_endpos[0];
	ptr->vecEndPos[1] = gGlobalVariables.trace_endpos[1];
	ptr->vecEndPos[2] = gGlobalVariables.trace_endpos[2];
	ptr->vecPlaneNormal[0] = gGlobalVariables.trace_plane_normal[0];
	ptr->vecPlaneNormal[1] = gGlobalVariables.trace_plane_normal[1];
	ptr->vecPlaneNormal[2] = gGlobalVariables.trace_plane_normal[2];
	ptr->iHitgroup = gGlobalVariables.trace_hitgroup;
};

void EXT_FUNC PF_TraceToss_DLL(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
	PF_TraceToss_Shared(pent, pentToIgnore ? pentToIgnore : &g_psv.edicts[0]);

	ptr->fAllSolid = (int)gGlobalVariables.trace_allsolid;
	ptr->fStartSolid = (int)gGlobalVariables.trace_startsolid;
	ptr->fInOpen = (int)gGlobalVariables.trace_inopen;
	ptr->fInWater = (int)gGlobalVariables.trace_inwater;
	ptr->flFraction = gGlobalVariables.trace_fraction;
	ptr->flPlaneDist = gGlobalVariables.trace_plane_dist;
	ptr->pHit = gGlobalVariables.trace_ent;
	ptr->vecEndPos[0] = gGlobalVariables.trace_endpos[0];
	ptr->vecEndPos[1] = gGlobalVariables.trace_endpos[1];
	ptr->vecEndPos[2] = gGlobalVariables.trace_endpos[2];
	ptr->vecPlaneNormal[0] = gGlobalVariables.trace_plane_normal[0];
	ptr->vecPlaneNormal[1] = gGlobalVariables.trace_plane_normal[1];
	ptr->vecPlaneNormal[2] = gGlobalVariables.trace_plane_normal[2];
	ptr->iHitgroup = gGlobalVariables.trace_hitgroup;
};

int EXT_FUNC TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	qboolean monsterClip = (pEdict->v.flags & FL_MONSTERCLIP) ? 1 : 0;
	trace_t trace = SV_Move(v1, pEdict->v.mins, pEdict->v.maxs, v2, fNoMonsters, pentToSkip, monsterClip);
	if(ptr)
	{
		ptr->fAllSolid = trace.allsolid;
		ptr->fStartSolid = trace.startsolid;
		ptr->fInOpen = trace.inopen;
		ptr->fInWater = trace.inwater;
		ptr->flPlaneDist = trace.plane.dist;
		ptr->pHit = trace.ent;
		ptr->iHitgroup = trace.hitgroup;
		ptr->vecEndPos[0] = trace.endpos[0];
		ptr->vecEndPos[1] = trace.endpos[1];
		ptr->vecEndPos[2] = trace.endpos[2];
		ptr->vecPlaneNormal[0] = trace.plane.normal[0];
		ptr->vecPlaneNormal[1] = trace.plane.normal[1];
		ptr->flFraction = trace.fraction;
		ptr->vecPlaneNormal[2] = trace.plane.normal[2];
	}

	return trace.allsolid || trace.fraction != 1.0f;
};

void EXT_FUNC TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
	hullNumber = hullNumber;
	if(hullNumber < 0 || hullNumber > 3)
		hullNumber = 0;
	trace_t trace = SV_Move(v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2, fNoMonsters, pentToSkip, 0);

	ptr->fAllSolid = trace.allsolid;
	ptr->fStartSolid = trace.startsolid;
	ptr->fInOpen = trace.inopen;
	ptr->fInWater = trace.inwater;
	ptr->flFraction = trace.fraction;
	ptr->flPlaneDist = trace.plane.dist;
	ptr->pHit = trace.ent;
	ptr->iHitgroup = trace.hitgroup;
	ptr->vecEndPos[0] = trace.endpos[0];
	ptr->vecEndPos[1] = trace.endpos[1];
	ptr->vecEndPos[2] = trace.endpos[2];
	ptr->vecPlaneNormal[0] = trace.plane.normal[0];
	ptr->vecPlaneNormal[1] = trace.plane.normal[1];
	ptr->vecPlaneNormal[2] = trace.plane.normal[2];
};

void EXT_FUNC TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
	int oldMovetype, oldSolid;

	if(hullNumber < 0 || hullNumber > 3)
		hullNumber = 0;

	model_t *pmodel = g_psv.models[pent->v.modelindex];
	if(pmodel && pmodel->type == mod_brush)
	{
		oldMovetype = pent->v.movetype;
		oldSolid = pent->v.solid;
		pent->v.solid = SOLID_BSP;
		pent->v.movetype = MOVETYPE_PUSH;
	}
	trace_t trace = SV_ClipMoveToEntity(pent, v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2);
	if(pmodel && pmodel->type == mod_brush)
	{
		pent->v.solid = oldSolid;
		pent->v.movetype = oldMovetype;
	}

	ptr->fAllSolid = trace.allsolid;
	ptr->fStartSolid = trace.startsolid;
	ptr->fInOpen = trace.inopen;
	ptr->fInWater = trace.inwater;
	ptr->flFraction = trace.fraction;
	ptr->flPlaneDist = trace.plane.dist;
	ptr->pHit = trace.ent;
	ptr->iHitgroup = trace.hitgroup;
	ptr->vecEndPos[0] = trace.endpos[0];
	ptr->vecEndPos[1] = trace.endpos[1];
	ptr->vecEndPos[2] = trace.endpos[2];
	ptr->vecPlaneNormal[0] = trace.plane.normal[0];
	ptr->vecPlaneNormal[1] = trace.plane.normal[1];
	ptr->vecPlaneNormal[2] = trace.plane.normal[2];
};

const char *EXT_FUNC TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
	int firstnode;
	model_t *pmodel;
	hull_t *phull;
	msurface_t *psurf;
	vec3_t up;
	vec3_t right;
	vec3_t forward;
	vec3_t offset;
	vec3_t temp;
	vec3_t start;
	vec3_t end;

	firstnode = 0;
	if(pTextureEntity)
	{
		pmodel = g_psv.models[pTextureEntity->v.modelindex];
		if(!pmodel || pmodel->type)
			return NULL;

		phull = SV_HullForBsp(pTextureEntity, vec3_origin, vec3_origin, offset);
		start[0] = v1[0] - offset[0];
		start[1] = v1[1] - offset[1];
		start[2] = v1[2] - offset[2];
		end[0] = v2[0] - offset[0];
		end[1] = v2[1] - offset[1];
		end[2] = v2[2] - offset[2];

		firstnode = phull->firstclipnode;
		if(pTextureEntity->v.angles[0] != 0.0 ||
		   pTextureEntity->v.angles[1] != 0.0 ||
		   pTextureEntity->v.angles[2] != 0.0)
		{
			AngleVectors(pTextureEntity->v.angles, forward, right, up);

			temp[0] = start[0];
			temp[1] = start[1];
			temp[2] = start[2];
			start[0] = _DotProduct(forward, temp);
			start[1] = -_DotProduct(right, temp);
			start[2] = _DotProduct(up, temp);

			temp[0] = end[0];
			temp[1] = end[1];
			temp[2] = end[2];
			end[0] = _DotProduct(forward, temp);
			end[1] = -_DotProduct(right, temp);
			end[2] = _DotProduct(up, temp);
		}
	}
	else
	{
		pmodel = g_psv.worldmodel;
		start[0] = v1[0];
		start[1] = v1[1];
		start[2] = v1[2];
		end[0] = v2[0];
		end[1] = v2[1];
		end[2] = v2[2];
	}

	if(!pmodel || pmodel->type != mod_brush || !pmodel->nodes)
		return NULL;

	psurf = SurfaceAtPoint(pmodel, &pmodel->nodes[firstnode], start, end);
	if(psurf)
		return psurf->texinfo->texture->name;

	return NULL;
};

void EXT_FUNC TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
	Sys_Error("TraceSphere not yet implemented!\n");
};

void EXT_FUNC PF_aim_I(edict_t *ent, float speed, float *rgflReturn)
{
	vec3_t start;
	vec3_t dir;
	vec3_t end;
	vec3_t bestdir;
	int j;
	trace_t tr;
	float dist;
	float bestdist;

	if(!ent || (ent->v.flags & FL_FAKECLIENT))
	{
		rgflReturn[0] = gGlobalVariables.v_forward[0];
		rgflReturn[1] = gGlobalVariables.v_forward[1];
		rgflReturn[2] = gGlobalVariables.v_forward[2];
		return;
	}

	start[0] = ent->v.origin[0];
	start[1] = ent->v.origin[1];
	start[2] = ent->v.origin[2];

	dir[0] = gGlobalVariables.v_forward[0];
	dir[1] = gGlobalVariables.v_forward[1];
	dir[2] = gGlobalVariables.v_forward[2];

	start[0] += ent->v.view_ofs[0];
	start[1] += ent->v.view_ofs[1];
	start[2] += ent->v.view_ofs[2];
	VectorMA(start, 2048.0, dir, end);
	tr = SV_Move(start, vec3_origin, vec3_origin, end, 0, ent, 0);

	if(tr.ent && tr.ent->v.takedamage == 2.0f &&
	   (ent->v.team <= 0 || ent->v.team != tr.ent->v.team))
	{
		rgflReturn[0] = gGlobalVariables.v_forward[0];
		rgflReturn[1] = gGlobalVariables.v_forward[1];
		rgflReturn[2] = gGlobalVariables.v_forward[2];
		return;
	}

	bestdir[1] = dir[1];
	bestdir[2] = dir[2];
	bestdir[0] = dir[0];
	bestdist = sv_aim.value;

	for(int i = 1; i < g_psv.num_edicts; i++)
	{
		edict_t *check = &g_psv.edicts[i];
		if(check->v.takedamage != 2.0f || (check->v.flags & FL_FAKECLIENT) ||
		   check == ent)
			continue;

		if(ent->v.team > 0 && ent->v.team == check->v.team)
			continue;

		for(j = 0; j < 3; j++)
		{
			end[j] = (check->v.maxs[j] + check->v.mins[j]) * 0.75 +
			check->v.origin[j] + ent->v.view_ofs[j] * 0.0;
		}

		dir[0] = end[0] - start[0];
		dir[1] = end[1] - start[1];
		dir[2] = end[2] - start[2];
		VectorNormalize(dir);
		dist = gGlobalVariables.v_forward[2] * dir[2] +
		gGlobalVariables.v_forward[1] * dir[1] +
		gGlobalVariables.v_forward[0] * dir[0];

		if(dist >= bestdist)
		{
			tr = SV_Move(start, vec3_origin, vec3_origin, end, 0, ent, 0);
			if(tr.ent == check)
			{
				bestdist = dist;
				bestdir[0] = dir[0];
				bestdir[1] = dir[1];
				bestdir[2] = dir[2];
			}
		}
	}

	rgflReturn[0] = bestdir[0];
	rgflReturn[1] = bestdir[1];
	rgflReturn[2] = bestdir[2];
};

void EXT_FUNC PF_localcmd_I(char *str)
{
	if(ValidCmd(str))
		Cbuf_AddText(str);
	else
		Con_Printf("Error, bad server command %s\n", str);
};

void EXT_FUNC PF_localexec_I()
{
	Cbuf_Execute();
};

void EXT_FUNC PF_stuffcmd_I(edict_t *pEdict, char *szFmt, ...)
{
	int entnum;
	client_t *old;
	va_list argptr;
	static char szOut[1024];

	va_start(argptr, szFmt);
	entnum = NUM_FOR_EDICT(pEdict);
	Q_vsnprintf(szOut, sizeof(szOut), szFmt, argptr);
	va_end(argptr);

	szOut[1023] = 0;
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("\n!!!\n\nStuffCmd:  Some entity tried to stuff '%s' to console "
		           "buffer of entity %i when maxclients was set to %i, "
		           "ignoring\n\n",
		           szOut,
		           entnum,
		           g_psvs.maxclients);
	}
	else
	{
		if(ValidCmd(szOut))
		{
			old = host_client;
			host_client = &g_psvs.clients[entnum - 1];
			Host_ClientCommands("%s", szOut);
			host_client = old;
		}
		else
		{
			Con_Printf("Tried to stuff bad command %s\n", szOut);
		}
	}
};

void EXT_FUNC PF_particle_I(const float *org, const float *dir, float color, float count)
{
	SV_StartParticle(org, dir, color, count);
};

void EXT_FUNC PF_lightstyle_I(int style, char *val)
{
	g_psv.lightstyles[style] = val;
	if(g_psv.state != ss_active)
		return;

	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *cl = &g_psvs.clients[i];
		if((cl->active || cl->spawned) && !cl->fakeclient)
		{
			MSG_WriteChar(&cl->netchan.message, svc_lightstyle);
			MSG_WriteChar(&cl->netchan.message, style);
			MSG_WriteString(&cl->netchan.message, val);
		}
	}
};

int EXT_FUNC PF_DecalIndex(const char *name)
{
	for(int i = 0; i < sv_decalnamecount; i++)
	{
		if(!Q_stricmp(sv_decalnames[i].name, name))
			return i;
	}

	return -1;
};

int EXT_FUNC PF_pointcontents_I(const float *rgflVector)
{
	return SV_PointContents(rgflVector);
};

void EXT_FUNC PF_MessageBegin_I(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	if(msg_dest == MSG_ONE || msg_dest == MSG_ONE_UNRELIABLE)
	{
		if(!ed)
			Sys_Error("MSG_ONE or MSG_ONE_UNRELIABLE with no target entity\n");
	}
	else
	{
		if(ed)
			Sys_Error("Invalid message;  cannot use broadcast message with a target "
			          "entity");
	}

	if(gMsgStarted)
		Sys_Error("New message started when msg '%d' has not been sent yet",
		          gMsgType);

	if(msg_type == 0)
		Sys_Error("Tried to create a message with a bogus message type ( 0 )");

	gMsgStarted = 1;
	gMsgType = msg_type;
	gMsgEntity = ed;
	gMsgDest = msg_dest;
	if(msg_dest == MSG_PVS || msg_dest == MSG_PAS)
	{
		if(pOrigin)
		{
			gMsgOrigin[0] = pOrigin[0];
			gMsgOrigin[1] = pOrigin[1];
			gMsgOrigin[2] = pOrigin[2];
		}

		// No idea why is it called here
		// Host_IsSinglePlayerGame();
	}

	gMsgBuffer.flags = SIZEBUF_ALLOW_OVERFLOW;
	gMsgBuffer.cursize = 0;
};

void EXT_FUNC PF_MessageEnd_I()
{
	qboolean MsgIsVarLength = 0;
	if(!gMsgStarted)
		Sys_Error("MESSAGE_END called with no active message\n");
	gMsgStarted = 0;

	if(gMsgEntity && (gMsgEntity->v.flags & FL_FAKECLIENT))
		return;

	if(gMsgBuffer.flags & SIZEBUF_OVERFLOWED)
		Sys_Error("MESSAGE_END called, but message buffer from .dll had overflowed\n");

	if(gMsgType > svc_startofusermessages)
	{
		UserMsg *pUserMsg = sv_gpUserMsgs;
		while(pUserMsg && pUserMsg->iMsg != gMsgType)
			pUserMsg = pUserMsg->next;

		if(!pUserMsg && gMsgDest == MSG_INIT)
		{
			pUserMsg = sv_gpNewUserMsgs;
			while(pUserMsg && pUserMsg->iMsg != gMsgType)
				pUserMsg = pUserMsg->next;
		}

		if(!pUserMsg)
		{
			Con_DPrintf("%s:  Unknown User Msg %d\n", __FUNCTION__, gMsgType);
			return;
		}

		if(pUserMsg->iSize == -1)
		{
			MsgIsVarLength = 1;

			// Limit packet sizes
			if(gMsgBuffer.cursize > MAX_USER_MSG_DATA)
				Host_Error("%s:  Refusing to send user message %s of %i "
				           "bytes to client, user message size limit is %i bytes\n",
						   __FUNCTION__,
				           pUserMsg->szName,
				           gMsgBuffer.cursize,
				           MAX_USER_MSG_DATA);
		}
		else
		{
			if(pUserMsg->iSize != gMsgBuffer.cursize)
				Sys_Error("User Msg '%s': %d bytes written, expected %d\n",
				          pUserMsg->szName,
				          gMsgBuffer.cursize,
				          pUserMsg->iSize);
		}
	}
#ifdef REHLDS_FIXES
	auto writer =
	[MsgIsVarLength]
#endif
	{
		sizebuf_t *pBuffer = WriteDest_Parm(gMsgDest);
		if((gMsgDest == MSG_BROADCAST &&
		    gMsgBuffer.cursize + pBuffer->cursize > pBuffer->maxsize) ||
		   !pBuffer->data)
			return;

		if(gMsgType > svc_startofusermessages &&
		   (gMsgDest == MSG_ONE || gMsgDest == MSG_ONE_UNRELIABLE))
		{
			int entnum = NUM_FOR_EDICT((const edict_t *)gMsgEntity);
			if(entnum < 1 || entnum > g_psvs.maxclients)
				Host_Error("WriteDest_Parm: not a client");

			client_t *client = &g_psvs.clients[entnum - 1];
			if(client->fakeclient || !client->hasusrmsgs ||
			   (!client->active && !client->spawned))
				return;
		}

		MSG_WriteByte(pBuffer, gMsgType);
		if(MsgIsVarLength)
			MSG_WriteByte(pBuffer, gMsgBuffer.cursize);
		MSG_WriteBuf(pBuffer, gMsgBuffer.cursize, gMsgBuffer.data);
	}
#ifdef REHLDS_FIXES
	;

	if(gMsgDest == MSG_ALL)
	{
		gMsgDest = MSG_ONE;
		for(int i = 0; i < g_psvs.maxclients; i++)
		{
			gMsgEntity = g_psvs.clients[i].edict;
			if(gMsgEntity == nullptr)
				continue;
			if(FBitSet(gMsgEntity->v.flags, FL_FAKECLIENT))
				continue;
			writer();
		}
	}
	else
	{
		writer();
	}
#endif

	switch(gMsgDest)
	{
	case MSG_PVS:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PVS, 0);
		break;

	case MSG_PAS:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PAS, 0);
		break;

	case MSG_PVS_R:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PAS, 1); // TODO: Should be MSG_FL_PVS, investigation needed
		break;

	case MSG_PAS_R:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PAS, 1);
		break;

	default:
		return;
	}
};

void EXT_FUNC PF_WriteByte_I(int iValue)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteByte(&gMsgBuffer, iValue);
};

void EXT_FUNC PF_WriteChar_I(int iValue)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteChar(&gMsgBuffer, iValue);
};

void EXT_FUNC PF_WriteShort_I(int iValue)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteShort(&gMsgBuffer, iValue);
};

void EXT_FUNC PF_WriteLong_I(int iValue)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteLong(&gMsgBuffer, iValue);
};

void EXT_FUNC PF_WriteAngle_I(float flValue)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteAngle(&gMsgBuffer, flValue);
};

void EXT_FUNC PF_WriteCoord_I(float flValue)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteShort(&gMsgBuffer, (int)(flValue * 8.0));
};

void EXT_FUNC PF_WriteString_I(const char *sz)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteString(&gMsgBuffer, sz);
};

void EXT_FUNC PF_WriteEntity_I(int iValue)
{
	if(!gMsgStarted)
		Sys_Error("%s called with no active message\n", __FUNCTION__);
	MSG_WriteShort(&gMsgBuffer, iValue);
};

void EXT_FUNC ClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg)
{
	client_t *client;
	int entnum;

	entnum = NUM_FOR_EDICT(pEdict);
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to sprint to a non-client\n");
		return;
	}

	client = &g_psvs.clients[entnum - 1];
	if(client->fakeclient)
		return;

	switch(ptype)
	{
	case print_center:
		MSG_WriteChar(&client->netchan.message, svc_centerprint);
		MSG_WriteString(&client->netchan.message, szMsg);
		break;

	case print_chat:
	case print_console:
		MSG_WriteByte(&client->netchan.message, svc_print);
		MSG_WriteString(&client->netchan.message, szMsg);
		break;

	default:
		Con_Printf("invalid PRINT_TYPE %i\n", ptype);
		break;
	}
};

void EXT_FUNC ServerPrint(const char *szMsg)
{
	Con_Printf("%s", szMsg);
};

int32 EXT_FUNC RandomLong(int32 lLow, int32 lHigh)
{
#ifndef SWDS
	g_engdstAddrs.pfnRandomLong(&lLow, &lHigh);
#endif

	unsigned long maxAcceptable;
	unsigned long x = lHigh - lLow + 1;
	unsigned long n;
	if(x <= 0 || MAX_RANDOM_RANGE < x - 1)
	{
		return lLow;
	}

	// The following maps a uniform distribution on the interval
	// [0,MAX_RANDOM_RANGE]
	// to a smaller, client-specified range of [0,x-1] in a way that doesn't bias
	// the uniform distribution unfavorably. Even for a worst case x, the loop is
	// guaranteed to be taken no more than half the time, so for that worst case
	// x,
	// the average number of times through the loop is 2. For cases where x is
	// much smaller than MAX_RANDOM_RANGE, the average number of times through the
	// loop is very close to 1.
	//
	maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE + 1) % x);
	do
	{
		n = ran1();
	} while(n > maxAcceptable);

	return lLow + (n % x);
};

float EXT_FUNC RandomFloat(float flLow, float flHigh)
{
#ifndef SWDS
	g_engdstAddrs.pfnRandomFloat(&flLow, &flHigh);
#endif

	float fl = fran1();                     // float in [0,1)
	return (fl * (flHigh - flLow)) + flLow; // float in [low,high)
};

void EXT_FUNC PF_setview_I(const edict_t *clientent, const edict_t *viewent)
{
	int clientnum = NUM_FOR_EDICT(clientent);
	if(clientnum < 1 || clientnum > g_psvs.maxclients)
		Host_Error("%s: not a client", __FUNCTION__);

	client_t *client = &g_psvs.clients[clientnum - 1];
	if(!client->fakeclient)
	{
		client->pViewEntity = viewent;
		MSG_WriteByte(&client->netchan.message, svc_setview);
		MSG_WriteShort(&client->netchan.message, NUM_FOR_EDICT(viewent));
	}
};

float EXT_FUNC PF_Time()
{
	return Sys_FloatTime();
};

void EXT_FUNC PF_crosshairangle_I(const edict_t *clientent, float pitch, float yaw)
{
	int clientnum = NUM_FOR_EDICT(clientent);
	if(clientnum < 1 || clientnum > g_psvs.maxclients)
		return;

	client_t *client = &g_psvs.clients[clientnum - 1];
	if(!client->fakeclient)
	{
		if(pitch > 180.0)
			pitch -= 360.0;

		if(pitch < -180.0)
			pitch += 360.0;

		if(yaw > 180.0)
			yaw -= 360.0;

		if(yaw < -180.0)
			yaw += 360.0;

		MSG_WriteByte(&client->netchan.message, svc_crosshairangle);
		MSG_WriteChar(&client->netchan.message, (int)(pitch * 5.0));
		MSG_WriteChar(&client->netchan.message, (int)(yaw * 5.0));
	}
};

void EXT_FUNC PF_FadeVolume(const edict_t *clientent, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
	int entnum = NUM_FOR_EDICT(clientent);
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_FadeVolume a non-client\n");
		return;
	}

	client_t *client = &g_psvs.clients[entnum - 1];
	if(client->fakeclient)
		return;

	MSG_WriteChar(&client->netchan.message, svc_soundfade);
	MSG_WriteByte(&client->netchan.message, (uint8)fadePercent);
	MSG_WriteByte(&client->netchan.message, (uint8)holdTime);
	MSG_WriteByte(&client->netchan.message, (uint8)fadeOutSeconds);
	MSG_WriteByte(&client->netchan.message, (uint8)fadeInSeconds);
};

void EXT_FUNC PF_SetClientMaxspeed(edict_t *clientent, float fNewMaxspeed)
{
	int entnum = NUM_FOR_EDICT(clientent);
	if(entnum < 1 || entnum > g_psvs.maxclients)
		Con_Printf("tried to PF_SetClientMaxspeed a non-client\n");

	clientent->v.maxspeed = fNewMaxspeed;
};

edict_t *EXT_FUNC PF_CreateFakeClient_I(const char *netname)
{
	client_t *fakeclient;
	edict_t *ent;

	int i = 0;
	fakeclient = g_psvs.clients;
	for(i = 0; i < g_psvs.maxclients; i++, fakeclient++)
	{
		if(!fakeclient->active && !fakeclient->spawned && !fakeclient->connected)
			break;
	}

	if(i >= g_psvs.maxclients)
		return NULL;

	ent = EDICT_NUM(i + 1);
	if(fakeclient->frames)
		SV_ClearFrames(&fakeclient->frames);

	Q_memset(fakeclient, 0, sizeof(client_t));
	fakeclient->resourcesneeded.pPrev = &fakeclient->resourcesneeded;
	fakeclient->resourcesneeded.pNext = &fakeclient->resourcesneeded;
	fakeclient->resourcesonhand.pPrev = &fakeclient->resourcesonhand;
	fakeclient->resourcesonhand.pNext = &fakeclient->resourcesonhand;

	Q_strncpy(fakeclient->name, netname, sizeof(fakeclient->name) - 1);
	fakeclient->name[sizeof(fakeclient->name) - 1] = 0;

	fakeclient->active = TRUE;
	fakeclient->spawned = TRUE;
	fakeclient->fully_connected = TRUE;
	fakeclient->connected = TRUE;
	fakeclient->fakeclient = TRUE;
	fakeclient->userid = g_userid++;
	fakeclient->uploading = FALSE;
	fakeclient->edict = ent;
	ent->v.netname = (size_t)fakeclient->name - (size_t)pr_strings;
	ent->v.pContainingEntity = ent;
	ent->v.flags = FL_FAKECLIENT | FL_CLIENT;

	Info_SetValueForKey(fakeclient->userinfo, "name", netname, MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "model", "gordon", MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "topcolor", "1", MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "bottomcolor", "1", MAX_INFO_STRING);
	
	fakeclient->sendinfo = TRUE;
	
	SV_ExtractFromUserinfo(fakeclient);

	fakeclient->network_userid.m_SteamID =
	//ISteamGameServer_CreateUnauthenticatedUserConnection();
	fakeclient->network_userid.idtype = AUTH_IDTYPE_STEAM;
	//ISteamGameServer_BUpdateUserData(fakeclient->network_userid.m_SteamID, netname, 0);

	return ent;
};

void EXT_FUNC PF_RunPlayerMove_I(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, unsigned char impulse, unsigned char msec)
{
	usercmd_t cmd;
	edict_t *oldclient;
	client_t *old;
	int entnum;

	oldclient = sv_player;
	old = host_client;
	entnum = NUM_FOR_EDICT(fakeclient);
	sv_player = fakeclient;
	host_client = &g_psvs.clients[entnum - 1];

	host_client->svtimebase = host_frametime + g_psv.time - msec / 1000.0;
	Q_memset(&cmd, 0, sizeof(cmd));
	cmd.lightlevel = 0;
	pmove = &g_svmove;

	cmd.viewangles[0] = viewangles[0];
	cmd.viewangles[1] = viewangles[1];
	cmd.viewangles[2] = viewangles[2];
	cmd.forwardmove = forwardmove;
	cmd.sidemove = sidemove;
	cmd.upmove = upmove;
	cmd.buttons = buttons;
	cmd.impulse = impulse;
	cmd.msec = msec;

	SV_PreRunCmd();
	SV_RunCmd(&cmd, 0);
	Q_memcpy(&host_client->lastcmd, &cmd, sizeof(host_client->lastcmd));

	sv_player = oldclient;
	host_client = old;
};

int EXT_FUNC PF_NumberOfEntities_I()
{
	int ent_count = 0;
	for(int i = 1; i < g_psv.num_edicts; i++)
	{
		if(!g_psv.edicts[i].free)
			++ent_count;
	}

	return ent_count;
};

char *EXT_FUNC PF_GetInfoKeyBuffer_I(edict_t *e)
{
	int e1;
	char *value;

	if(e)
	{
		e1 = NUM_FOR_EDICT(e);
		if(e1)
		{
#ifdef REHLDS_FIXES
			if(e1 <= 0 || e1 > g_psvs.maxclients) // FIXED: Check against correct
                                                  // amount of clients
#else                                             // REHLDS_FIXES
			if(e1 > 32)
#endif                                            // REHLDS_FIXES
				value = (char *)"";
			else
				value = (char *)&g_psvs.clients[e1 - 1].userinfo;
		}
		else
			value = Cvar_Serverinfo();
	}
	else
		value = localinfo;

	return value;
};

char *EXT_FUNC PF_InfoKeyValue_I(char *infobuffer, const char *key)
{
	return (char *)Info_ValueForKey(infobuffer, key);
};

void EXT_FUNC PF_SetKeyValue_I(char *infobuffer, const char *key, const char *value)
{
	if(infobuffer == localinfo)
	{
		Info_SetValueForKey(infobuffer, key, value, MAX_INFO_STRING * 128);
	}
	else
	{
		if(infobuffer != Cvar_Serverinfo())
		{
			Sys_Error("Can't set client keys with SetKeyValue");
		}
#ifdef REHLDS_FIXES
		Info_SetValueForKey(infobuffer, key, value,
		                    MAX_INFO_STRING); // Use correct length
#else                                         // REHLDS_FIXES
		Info_SetValueForKey(infobuffer, key, value, 512);
#endif                                        // REHLDS_FIXES
	}
};

void EXT_FUNC PF_SetClientKeyValue_I(int clientIndex, char *infobuffer, const char *key, const char *value)
{
	client_t *pClient;

	if(infobuffer == localinfo || infobuffer == Cvar_Serverinfo() ||
	   clientIndex <= 0 || clientIndex > g_psvs.maxclients)
	{
		return;
	}

	if(Q_strcmp(Info_ValueForKey(infobuffer, key), value))
	{
		Info_SetValueForStarKey(infobuffer, key, value, MAX_INFO_STRING);
		pClient = &g_psvs.clients[clientIndex - 1];
		pClient->sendinfo = TRUE;
		pClient->sendinfo_time = 0.0f;
	}
};

int EXT_FUNC PF_IsMapValid_I(char *mapname)
{
	char cBuf[260];
	if(!mapname || Q_strlen(mapname) == 0)
		return 0;

	Q_snprintf(cBuf, sizeof(cBuf), "maps/%.32s.bsp", mapname);
	return FS_FileExists(cBuf);
};

void EXT_FUNC PF_StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
	MSG_WriteByte(&g_psv.signon, svc_temp_entity);
	MSG_WriteByte(&g_psv.signon, TE_BSPDECAL);
	MSG_WriteCoord(&g_psv.signon, *origin);
	MSG_WriteCoord(&g_psv.signon, origin[1]);
	MSG_WriteCoord(&g_psv.signon, origin[2]);
	MSG_WriteShort(&g_psv.signon, decalIndex);
	MSG_WriteShort(&g_psv.signon, entityIndex);

	if(entityIndex)
		MSG_WriteShort(&g_psv.signon, modelIndex);
};

#ifdef REHLDS_FIXES

int EXT_FUNC PF_precache_generic_I(char *s)
{
	if(!s)
		Host_Error("%s: NULL pointer", __FUNCTION__);

	if(PR_IsEmptyString(s))
	{
		// TODO: Call to Con_Printf is replaced with Host_Error in 6153
		Host_Error("%s: Bad string '%s'", __FUNCTION__, s);
	};

	char resName[MAX_QPATH];
	Q_strncpy(resName, s, sizeof(resName));
	resName[sizeof(resName) - 1] = '\0';
	ForwardSlashes(resName);

	size_t soundPrefixLength = sizeof("sound/") - 1;
	bool isSoundPrefixed = !Q_strnicmp(resName, "sound/", soundPrefixLength);

	// TODO: check sound with index 0?
	// UPD: no, not need, because engine do this: g_psv.sound_precache[0] =
	// pr_strings;
	if((isSoundPrefixed && SV_LookupSoundIndex(&resName[soundPrefixLength])) ||
	   SV_LookupModelIndex(resName))
		return 0;

	size_t resCount = g_rehlds_sv.precachedGenericResourceCount;
	for(size_t i = 0; i < resCount; i++)
	{
		if(!Q_stricmp(g_rehlds_sv.precachedGenericResourceNames[i], resName))
			return i;
	}

	if(g_psv.state != ss_loading)
		Host_Error("%s: '%s' Precache can only be done in spawn functions", __FUNCTION__, resName);

	if(resCount >= ARRAYSIZE(g_rehlds_sv.precachedGenericResourceNames))
	{
		Host_Error(
		"%s: Generic item '%s' failed to precache because the item count is over the %d limit.\n\
		Reduce the number of brush models and/or regular models in the map to correct this.",
		__FUNCTION__, resName,
		ARRAYSIZE(g_rehlds_sv.precachedGenericResourceNames));
	};

	Q_strcpy(g_rehlds_sv.precachedGenericResourceNames[resCount], resName);

	return g_rehlds_sv.precachedGenericResourceCount++;
};

#else  // REHLDS_FIXES

int EXT_FUNC PF_precache_generic_I(char *s)
{
	if(!s)
		Host_Error("%s: NULL pointer", __FUNCTION__);

	if(PR_IsEmptyString(s))
	{
		// TODO: Call to Con_Printf is replaced with Host_Error in 6153
		Host_Error("%s: Bad string '%s'", __FUNCTION__, s);
	};

	if(g_psv.state == ss_loading)
	{
		for(int i = 0; i < MAX_GENERIC; i++)
		{
			if(!g_psv.generic_precache[i])
			{
				g_psv.generic_precache[i] = s;
				return i;
			}

			if(!Q_stricmp(g_psv.generic_precache[i], s))
				return i;
		}
		Host_Error("%s: Generic item '%s' failed to precache "
		           "because the item count is over the %d limit.\nReduce the "
		           "number of brush models and/or regular models in the map to "
		           "correct this.",
				   __FUNCTION__,
		           s,
		           MAX_GENERIC);
	}
	else
	{
		for(int i = 0; i < MAX_GENERIC; i++)
		{
			if(!Q_stricmp(g_psv.generic_precache[i], s))
				return i;
		}
		Host_Error("%s: '%s' Precache can only be done in spawn functions",
				   __FUNCTION__,
		           s);
	};
};
#endif // REHLDS_FIXES

int EXT_FUNC PF_GetPlayerUserId(edict_t *e)
{
	if(!g_psv.active || !e)
		return -1;

	for(int i = 0; i < g_psvs.maxclients; i++)
	{
		if(g_psvs.clients[i].edict == e)
		{
			return g_psvs.clients[i].userid;
		}
	}

	return -1;
};

void EXT_FUNC PF_BuildSoundMsg_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	g_RehldsHookchains.m_PF_BuildSoundMsg_I.callChain(
	PF_BuildSoundMsg_I_internal, entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
};

int EXT_FUNC PF_IsDedicatedServer()
{
	return g_bIsDedicatedServer;
};

unsigned int EXT_FUNC PF_GetPlayerWONId(edict_t *e)
{
	return 0xFFFFFFFF;
};

void EXT_FUNC PF_RemoveKey_I(char *s, const char *key)
{
	Info_RemoveKey(s, key);
};

const char *EXT_FUNC PF_GetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_GetPhysicsKeyValue a non-client\n");
		return "";
	}

	client_t *client = &g_psvs.clients[entnum - 1];
	return Info_ValueForKey(client->physinfo, key);
};

void EXT_FUNC PF_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if(entnum < 1 || entnum > g_psvs.maxclients)
		Con_Printf("tried to PF_SetPhysicsKeyValue a non-client\n");

	client_t *client = &g_psvs.clients[entnum - 1];
	Info_SetValueForKey(client->physinfo, key, value, MAX_INFO_STRING);
};

const char *EXT_FUNC PF_GetPhysicsInfoString(const edict_t *pClient)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_GetPhysicsInfoString a non-client\n");
		return "";
	}

	client_t *client = &g_psvs.clients[entnum - 1];
	return client->physinfo;
};

int EXT_FUNC PF_GetCurrentPlayer()
{
	int idx = host_client - g_psvs.clients;
	if(idx < 0 || idx >= g_psvs.maxclients)
		return -1;

	return idx;
};

int EXT_FUNC PF_CanSkipPlayer(const edict_t *pClient)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_CanSkipPlayer a non-client\n");
		return 0;
	}

	client_t *client = &g_psvs.clients[entnum - 1];
	return client->lw != 0;
};

void EXT_FUNC PF_SetGroupMask(int mask, int op)
{
	g_groupmask = mask;
	g_groupop = op;
};

int EXT_FUNC PF_CreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
	extra_baselines_t *bls = g_psv.instance_baselines;
	if(bls->number >= NUM_BASELINES)
		return 0;

	bls->classname[bls->number] = classname;
	Q_memcpy(&bls->baseline[bls->number], baseline, sizeof(struct entity_state_s));
	bls->number += 1;
	return bls->number;
};

void EXT_FUNC PF_Cvar_DirectSet(struct cvar_s *var, const char *value)
{
	Cvar_DirectSet(var, value);
};

void EXT_FUNC PF_ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
	int i;

	if(!filename)
		Host_Error("%s: NULL pointer", __FUNCTION__);

	if(PR_IsEmptyString(filename))
		Host_Error("%s: Bad string '%s'", __FUNCTION__, filename);

	if(g_psv.state == ss_loading)
	{
		i = 0;
		consistency_t *cnode = g_psv.consistency_list;
		while(cnode->filename)
		{
			if(!Q_stricmp(cnode->filename, (char *)filename))
				return;

			++cnode;
			++i;

			if(i >= 512)
				Host_Error("%s: '%s' overflow", __FUNCTION__, filename);
		}

		cnode->check_type = type;
		cnode->filename = (char *)filename;
		if(mins)
		{
			cnode->mins[0] = mins[0];
			cnode->mins[1] = mins[1];
			cnode->mins[2] = mins[2];
		}
		if(maxs)
		{
			cnode->maxs[0] = maxs[0];
			cnode->maxs[1] = maxs[1];
			cnode->maxs[2] = maxs[2];
		}
	}
	else
	{
		i = 0;
		consistency_t *cnode = g_psv.consistency_list;
		while(!cnode->filename || Q_stricmp(cnode->filename, filename))
		{
			++cnode;
			++i;
			if(i >= 512)
				Host_Error("%s: '%s' Precache can only be done in spawn functions", __FUNCTION__, filename);
		}
	}
};

void EXT_FUNC PF_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
	*packet_loss = 0;
	*ping = 0;
	int c = NUM_FOR_EDICT(pClient);
	if(c < 1 || c > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_GetPlayerStats a non-client\n");
		return;
	}

	client_t *client = &g_psvs.clients[c - 1];
	*packet_loss = client->packet_loss;
	*ping = client->latency * 1000.0;
};

const char *EXT_FUNC PF_GetPlayerAuthId(edict_t *e)
{
	static char szAuthID[5][64];
	static int count = 0;
	client_t *cl;
	int i;

	count = (count + 1) % 5;
	szAuthID[count][0] = 0;

	if(!g_psv.active || e == NULL)
	{
		return szAuthID[count];
	}

	for(i = 0; i < g_psvs.maxclients; i++)
	{
		cl = &g_psvs.clients[i];
		if(cl->edict != e)
		{
			continue;
		}

		if(cl->fakeclient)
		{
			Q_strcpy(szAuthID[count], "BOT");
		}
		//		AUTH_IDTYPE_LOCAL is handled inside SV_GetIDString(), no need to do it here
		//		else if (cl->network_userid.idtype == AUTH_IDTYPE_LOCAL)
		//		{
		//			Q_strcpy(szAuthID[count], "HLTV");
		//		}
		else
		{
			Q_snprintf(szAuthID[count], sizeof(szAuthID[count]) - 1, "%s", SV_GetClientIDString(cl));
		}

		break;
	}

	return szAuthID[count];
};

void EXT_FUNC QueryClientCvarValue(const edict_t *player, const char *cvarName)
{
	int entnum = NUM_FOR_EDICT(player);
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		if(gNewDLLFunctions.pfnCvarValue)
			gNewDLLFunctions.pfnCvarValue(player, "Bad Player");

		Con_Printf("tried to QueryClientCvarValue a non-client\n");
		return;
	}
	client_t *client = &g_psvs.clients[entnum - 1];
	MSG_WriteChar(&client->netchan.message, svc_sendcvarvalue);
	MSG_WriteString(&client->netchan.message, cvarName);
};

void EXT_FUNC QueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
	int entnum = NUM_FOR_EDICT(player);
	if(entnum < 1 || entnum > g_psvs.maxclients)
	{
		if(gNewDLLFunctions.pfnCvarValue2)
			gNewDLLFunctions.pfnCvarValue2(player, requestID, cvarName, "Bad Player");

		Con_Printf("tried to QueryClientCvarValue a non-client\n");
		return;
	}
	client_t *client = &g_psvs.clients[entnum - 1];
	MSG_WriteChar(&client->netchan.message, svc_sendcvarvalue2);
	MSG_WriteLong(&client->netchan.message, requestID);
	MSG_WriteString(&client->netchan.message, cvarName);
};

int EXT_FUNC EngCheckParm(const char *pchCmdLineToken, char **pchNextVal)
{
	return hudCheckParm((char *)pchCmdLineToken, pchNextVal);
};

enginefuncs_t g_engfuncsExportedToDlls = { PF_precache_model_I,
	                                       PF_precache_sound_I,
	                                       PF_setmodel_I,
	                                       PF_modelindex,
	                                       ModelFrames,
	                                       PF_setsize_I,
	                                       PF_changelevel_I,
	                                       PF_setspawnparms_I,
	                                       //SaveSpawnParms,
	                                       PF_vectoyaw_I,
	                                       PF_vectoangles_I,
	                                       //SV_MoveToOrigin_I,
	                                       PF_changeyaw_I,
	                                       PF_changepitch_I,
	                                       FindEntityByString,
	                                       GetEntityIllum,
	                                       FindEntityInSphere,
	                                       PF_checkclient_I,
	                                       PVSFindEntities,
	                                       PF_makevectors_I,
	                                       //AngleVectors_ext,
	                                       PF_Spawn_I,
	                                       PF_Remove_I,
	                                       CreateNamedEntity,
	                                       PF_makestatic_I,
	                                       PF_checkbottom_I,
	                                       PF_droptofloor_I,
	                                       PF_walkmove_I,
	                                       PF_setorigin_I,
	                                       PF_sound_I,
	                                       PF_ambientsound_I,
	                                       PF_traceline_DLL,
	                                       PF_TraceToss_DLL,
	                                       TraceMonsterHull,
	                                       TraceHull,
	                                       TraceModel,
	                                       TraceTexture,
	                                       TraceSphere,
	                                       PF_aim_I,
	                                       PF_localcmd_I,
	                                       PF_localexec_I,
	                                       PF_stuffcmd_I,
	                                       PF_particle_I,
	                                       PF_lightstyle_I,
	                                       PF_DecalIndex,
	                                       PF_pointcontents_I,
	                                       PF_MessageBegin_I,
	                                       PF_MessageEnd_I,
	                                       PF_WriteByte_I,
	                                       PF_WriteChar_I,
	                                       PF_WriteShort_I,
	                                       PF_WriteLong_I,
	                                       PF_WriteAngle_I,
	                                       PF_WriteCoord_I,
	                                       PF_WriteString_I,
	                                       PF_WriteEntity_I,
	                                       //CVarRegister,
	                                       //CVarGetFloat,
	                                       //CVarGetString,
	                                       //CVarSetFloat,
	                                       //CVarSetString,
	                                       //AlertMessage,
	                                       //EngineFprintf,
	                                       //PvAllocEntPrivateData,
	                                       //PvEntPrivateData,
	                                       //FreeEntPrivateData,
	                                       //SzFromIndex,
	                                       //AllocEngineString,
	                                       //GetVarsOfEnt,
	                                       //PEntityOfEntOffset,
	                                       //EntOffsetOfPEntity,
	                                       //IndexOfEdict,
	                                       //PEntityOfEntIndex,
	                                       //FindEntityByVars,
	                                       //GetModelPtr,
	                                       //RegUserMsg,
	                                       //AnimationAutomove,
	                                       //GetBonePosition,
	                                       //FunctionFromName,
	                                       //NameForFunction,
	                                       ClientPrintf,
	                                       ServerPrint,
	                                       //Cmd_Args,
	                                       //Cmd_Argv,
	                                       //Cmd_Argc,
	                                       //GetAttachment,
	                                       //CRC32_Init,
	                                       //CRC32_ProcessBuffer,
	                                       //CRC32_ProcessByte,
	                                       //CRC32_Final,
	                                       RandomLong,
	                                       RandomFloat,
	                                       PF_setview_I,
	                                       PF_Time,
	                                       PF_crosshairangle_I,
	                                       //COM_LoadFileForMe,
	                                       //COM_FreeFile,
	                                       //Host_EndSection,
	                                       //COM_CompareFileTime,
	                                       //COM_GetGameDir,
	                                       //Cvar_RegisterVariable,
	                                       PF_FadeVolume,
	                                       PF_SetClientMaxspeed,
	                                       PF_CreateFakeClient_I,
	                                       PF_RunPlayerMove_I,
	                                       PF_NumberOfEntities_I,
	                                       PF_GetInfoKeyBuffer_I,
	                                       PF_InfoKeyValue_I,
	                                       PF_SetKeyValue_I,
	                                       PF_SetClientKeyValue_I,
	                                       PF_IsMapValid_I,
	                                       PF_StaticDecal,
	                                       PF_precache_generic_I,
	                                       PF_GetPlayerUserId,
	                                       PF_BuildSoundMsg_I,
	                                       PF_IsDedicatedServer,
	                                       //CVarGetPointer,
	                                       PF_GetPlayerWONId,
	                                       PF_RemoveKey_I,
	                                       PF_GetPhysicsKeyValue,
	                                       PF_SetPhysicsKeyValue,
	                                       PF_GetPhysicsInfoString,
	                                       /EV_Precache,
	                                       /EV_Playback,
	                                       //SV_FatPVS,
	                                       //SV_FatPAS,
	                                       //SV_CheckVisibility,
	                                       //DELTA_SetField,
	                                       //DELTA_UnsetField,
	                                       //DELTA_AddEncoder,
	                                       PF_GetCurrentPlayer,
	                                       PF_CanSkipPlayer,
	                                       //DELTA_FindFieldIndex,
	                                       //DELTA_SetFieldByIndex,
	                                       //DELTA_UnsetFieldByIndex,
	                                       PF_SetGroupMask,
	                                       PF_CreateInstancedBaseline,
	                                       PF_Cvar_DirectSet,
	                                       PF_ForceUnmodified,
	                                       PF_GetPlayerStats,
	                                       //Cmd_AddGameCommand,
	                                       //Voice_GetClientListening,
	                                       //Voice_SetClientListening,
	                                       PF_GetPlayerAuthId,
	                                       NULL, // sequenceEntry_s *(*pfnSequenceGet)(const char *fileName, const char *entryName);
	                                       NULL, // sentenceEntry_s *(*pfnSequencePickSentence)(const char *groupName, int pickMethod, int *picked);
	                                       //COM_FileSize,
	                                       //COM_GetApproxWavePlayLength,
	                                       nullptr, //VGuiWrap2_IsInCareerMatch,
	                                       nullptr, //VGuiWrap2_GetLocalizedStringLength,
	                                       nullptr, //RegisterTutorMessageShown,
	                                       nullptr, //GetTimesTutorMessageShown,
	                                       nullptr, //ProcessTutorMessageDecayBuffer,
	                                       nullptr, //ConstructTutorMessageDecayBuffer,
	                                       nullptr, //ResetTutorMessageDecayData,
	                                       QueryClientCvarValue,
	                                       QueryClientCvarValue2,
	                                       EngCheckParm };

#else // HOOK_ENGINE

enginefuncs_t g_engfuncsExportedToDlls;

#endif // HOOK_ENGINE