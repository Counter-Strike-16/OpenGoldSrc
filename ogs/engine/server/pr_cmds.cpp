/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#include "precompiled.h"

vec_t gHullMins[4][3] = {
		{ 0.0f,   0.0f,   0.0f },
		{ -16.0f, -16.0f, -36.0f },
		{ -32.0f, -32.0f, -32.0f },
		{ -16.0f, -16.0f, -18.0f },
};

vec_t gHullMaxs[4][3] = {
		{ 0.0f,   0.0f,   0.0f },
		{ 16.0f,  16.0f,  36.0f },
		{ 32.0f,  32.0f,  32.0f },
		{ 16.0f,  16.0f,  18.0f },
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

#else //HOOK_ENGINE

sizebuf_t gMsgBuffer;

#endif //HOOK_ENGINE

/* <782a3> ../engine/pr_cmds.c:53 */
void EXT_FUNC PF_makevectors_I(const float *rgflVector)
{
	AngleVectors(rgflVector, gGlobalVariables.v_forward, gGlobalVariables.v_right, gGlobalVariables.v_up);
}

/* <782cb> ../engine/pr_cmds.c:58 */
float EXT_FUNC PF_Time(void)
{
	return Sys_FloatTime();
}

/* <782e4> ../engine/pr_cmds.c:74 */
void EXT_FUNC PF_setorigin_I(edict_t *e, const float *org)
{
	if (!e)
		return;
	
	e->v.origin[0] = org[0];
	e->v.origin[1] = org[1];
	e->v.origin[2] = org[2];
	SV_LinkEdict(e, FALSE);
}

/* <78317> ../engine/pr_cmds.c:84 */
void EXT_FUNC SetMinMaxSize(edict_t *e, const float *min, const float *max, qboolean rotate)
{
	for (int i = 0; i < 3; i++)
	{
		if (min[i] > max[i])
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

/* <7840f> ../engine/pr_cmds.c:169 */
void EXT_FUNC PF_setsize_I(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	SetMinMaxSize(e, rgflMin, rgflMax, 0);
}

/* <78451> ../engine/pr_cmds.c:184 */
void EXT_FUNC PF_setmodel_I(edict_t *e, const char *m)
{
	const char** check = &g_psv.model_precache[0];
	int i = 0;

#ifdef REHLDS_CHECKS
	for (; *check && i < HL_MODEL_MAX; i++, check++)
#else
	for (; *check; i++, check++)
#endif
	{

		//use case-sensitive names to increase performance
#ifdef REHLDS_FIXES
		if (!Q_strcmp(*check, m))
#else
		if (!Q_stricmp(*check, m))
#endif
		
		{
			e->v.modelindex = i;
			model_t *mod = g_psv.models[i];
#ifdef REHLDS_FIXES
			e->v.model = *check - pr_strings;
#else // REHLDS_FIXES
			e->v.model = m - pr_strings;
#endif // REHLDS_FIXES
			if (mod)
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
}

/* <784b4> ../engine/pr_cmds.c:210 */
int EXT_FUNC PF_modelindex(const char *pstr)
{
	return SV_ModelIndex(pstr);
}

/* <784df> ../engine/pr_cmds.c:217 */
int EXT_FUNC ModelFrames(int modelIndex)
{
	if (modelIndex <= 0 || modelIndex >= 512)
	{
		Con_DPrintf("Bad sprite index!\n");
		return 1;
	}

	return ModelFrameCount(g_psv.models[modelIndex]);
}

/* <7851a> ../engine/pr_cmds.c:239 */
void EXT_FUNC PF_bprint(char *s)
{
	SV_BroadcastPrintf("%s", s);
}

/* <78540> ../engine/pr_cmds.c:253 */
void EXT_FUNC PF_sprint(char *s, int entnum)
{
	if (entnum <= 0 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to sprint to a non-client\n");
		return;
	}

	client_t* client = &g_psvs.clients[entnum - 1];
	if (!client->fakeclient)
	{
#ifdef REHLDS_FIXES
		MSG_WriteByte(&client->netchan.message, svc_print);
#else // REHLDS_FIXES
		MSG_WriteChar(&client->netchan.message, svc_print);
#endif // REHLDS_FIXES
		MSG_WriteString(&client->netchan.message, s);
	}
}

/* <78589> ../engine/pr_cmds.c:280 */
void EXT_FUNC ServerPrint(const char *szMsg)
{
	Con_Printf("%s", szMsg);
}

/* <785b3> ../engine/pr_cmds.c:293 */
void EXT_FUNC ClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg)
{
	client_t *client;
	int entnum;

	entnum = NUM_FOR_EDICT(pEdict);
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to sprint to a non-client\n");
		return;
	}

	client = &g_psvs.clients[entnum - 1];
	if (client->fakeclient)
		return;

	switch (ptype)
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

}

/* <7861b> ../engine/pr_cmds.c:339 */
float EXT_FUNC PF_vectoyaw_I(const float *rgflVector)
{
	float yaw = 0.0f;
	if (rgflVector[1] == 0.0f && rgflVector[0] == 0.0f)
		return 0.0f;

	yaw = (float)(int)floor(atan2((double)rgflVector[1], (double)rgflVector[0]) * 180.0 / M_PI);
	if (yaw < 0.0)
		yaw = yaw + 360.0;

	return yaw;
}

/* <78659> ../engine/pr_cmds.c:363 */
void EXT_FUNC PF_vectoangles_I(const float *rgflVectorIn, float *rgflVectorOut)
{
	VectorAngles(rgflVectorIn, rgflVectorOut);
}

/* <78691> ../engine/pr_cmds.c:377 */
void EXT_FUNC PF_particle_I(const float *org, const float *dir, float color, float count)
{
	SV_StartParticle(org, dir, color, count);
}

/* <786e7> ../engine/pr_cmds.c:390 */
void EXT_FUNC PF_ambientsound_I(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	int i;
	int soundnum;
	int ent;
	sizebuf_t *pout;

	if (samp[0] == '!')
	{
		fFlags |= SND_FL_SENTENCE;
		soundnum = Q_atoi(samp + 1);
		if (soundnum >= CVOXFILESENTENCEMAX)
		{
			Con_Printf("invalid sentence number: %s", &samp[1]);
			return;
		}
	}
	else
	{
		for (i = 0; i < HL_SOUND_MAX; i++)
		{
			if (g_psv.sound_precache[i] && !Q_stricmp(g_psv.sound_precache[i], samp))
			{
				soundnum = i;
				break;
			}
		}

		if (i == HL_SOUND_MAX)
		{
			Con_Printf("no precache: %s\n", samp);
			return;
		}
	}

	ent = NUM_FOR_EDICT(entity);
	pout = &g_psv.signon;
	if (!(fFlags & SND_FL_SPAWNING))
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
}

/* <787c0> ../engine/pr_cmds.c:459 */
void EXT_FUNC PF_sound_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch)
{
	if (volume < 0.0 || volume > 255.0)
		Sys_Error("EMIT_SOUND: volume = %i", volume);
	if (attenuation < 0.0 || attenuation > 4.0)
		Sys_Error("EMIT_SOUND: attenuation = %f", attenuation);
	if (channel < 0 || channel > 7)
		Sys_Error("EMIT_SOUND: channel = %i", channel);
	if (pitch < 0 || pitch > 255)
		Sys_Error("EMIT_SOUND: pitch = %i", pitch);
	SV_StartSound(0, entity, channel, sample, (int)(volume * 255), attenuation, fFlags, pitch);
}

/* <78cdd> ../engine/pr_cmds.c:491 */
void EXT_FUNC PF_traceline_Shared(const float *v1, const float *v2, int nomonsters, edict_t *ent)
{
#ifdef REHLDS_OPT_PEDANTIC
	trace_t trace = SV_Move_Point(v1, v2, nomonsters, ent);
#else // REHLDS_OPT_PEDANTIC
	trace_t trace = SV_Move(v1, vec3_origin, vec3_origin, v2, nomonsters, ent, 0);
#endif // REHLDS_OPT_PEDANTIC

	gGlobalVariables.trace_flags = 0;
	SV_SetGlobalTrace(&trace);
} 

/* <78d1c> ../engine/pr_cmds.c:502 */
void EXT_FUNC PF_traceline_DLL(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	PF_traceline_Shared(v1, v2, fNoMonsters, pentToSkip ? pentToSkip : &g_psv.edicts[0]);
	ptr->fAllSolid = (int)gGlobalVariables.trace_allsolid;
	ptr->fStartSolid = (int) gGlobalVariables.trace_startsolid;
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
}

/* <78844> ../engine/pr_cmds.c:531 */
void EXT_FUNC TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
	hullNumber = hullNumber;
	if (hullNumber < 0 || hullNumber > 3)
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
}

/* <788c8> ../engine/pr_cmds.c:556 */
void EXT_FUNC TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
	Sys_Error("TraceSphere not yet implemented!\n");
}

/* <7893a> ../engine/pr_cmds.c:578 */
void EXT_FUNC TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
	int oldMovetype, oldSolid;

	if (hullNumber < 0 || hullNumber > 3)
		hullNumber = 0;
	
	model_t* pmodel = g_psv.models[pent->v.modelindex];
	if (pmodel && pmodel->type == mod_brush)
	{
		oldMovetype = pent->v.movetype;
		oldSolid = pent->v.solid;
		pent->v.solid = SOLID_BSP;
		pent->v.movetype = MOVETYPE_PUSH;
	}
	trace_t trace = SV_ClipMoveToEntity(pent, v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2);
	if (pmodel && pmodel->type == mod_brush)
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
}

/* <789df> ../engine/pr_cmds.c:619 */
msurface_t* EXT_FUNC SurfaceAtPoint(model_t *pModel, mnode_t *node, vec_t *start, vec_t *end)
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

	if (node->contents < 0)
		return 0;

	plane = node->plane;
	front = _DotProduct(start, plane->normal) - plane->dist;
	back = _DotProduct(end, plane->normal) - plane->dist;
	s = (front < 0.0f) ? 1 : 0;
	t = (back < 0.0f) ? 1 : 0;
	if (t == s)
		return SurfaceAtPoint(pModel, node->children[s], start, end);

	frac = front / (front - back);
	mid[0] = (end[0] - start[0]) * frac + start[0];
	mid[1] = (end[1] - start[1]) * frac + start[1];
	mid[2] = (end[2] - start[2]) * frac + start[2];
	surf = SurfaceAtPoint(pModel, node->children[s], start, mid);
	if (surf)
		return surf;

	/* Unreachable code
	if (t == s)
		return NULL;
	*/

	for (int i = 0; i < node->numsurfaces; i++)
	{
		surf = &pModel->surfaces[node->firstsurface + i];
		tex = surf->texinfo;
		ds = (int)(_DotProduct(mid, tex->vecs[0]) + tex->vecs[0][3]);
		dt = (int)(_DotProduct(mid, tex->vecs[1]) + tex->vecs[1][3]);
		if (ds >= surf->texturemins[0])
		{
			if (dt >= surf->texturemins[1])
			{
				if (ds - surf->texturemins[0] <= surf->extents[0] && dt - surf->texturemins[1] <= surf->extents[1])
					return surf;
			}
		}
	}

	return SurfaceAtPoint(pModel, node->children[s ^ 1], mid, end);
}

/* <78af9> ../engine/pr_cmds.c:688 */
const char* EXT_FUNC TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
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
	if (pTextureEntity)
	{
		pmodel = g_psv.models[pTextureEntity->v.modelindex];
		if (!pmodel || pmodel->type)
			return NULL;

		phull = SV_HullForBsp(pTextureEntity, vec3_origin, vec3_origin, offset);
		start[0] = v1[0] - offset[0];
		start[1] = v1[1] - offset[1];
		start[2] = v1[2] - offset[2];
		end[0] = v2[0] - offset[0];
		end[1] = v2[1] - offset[1];
		end[2] = v2[2] - offset[2];

		firstnode = phull->firstclipnode;
		if (pTextureEntity->v.angles[0] != 0.0 || pTextureEntity->v.angles[1] != 0.0 || pTextureEntity->v.angles[2] != 0.0)
		{
			AngleVectors(pTextureEntity->v.angles, forward, right, up);

			temp[0] = start[0];	temp[1] = start[1]; temp[2] = start[2];
			start[0] = _DotProduct(forward, temp);
			start[1] = -_DotProduct(right, temp);
			start[2] = _DotProduct(up, temp);
			
			temp[0] = end[0]; temp[1] = end[1]; temp[2] = end[2];
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

	if (!pmodel || pmodel->type != mod_brush || !pmodel->nodes)
		return NULL;

	psurf = SurfaceAtPoint(pmodel, &pmodel->nodes[firstnode], start, end);
	if (psurf)
		return psurf->texinfo->texture->name;
	
	return NULL;
}

/* <78c30> ../engine/pr_cmds.c:749 */
void EXT_FUNC PF_TraceToss_Shared(edict_t *ent, edict_t *ignore)
{
	trace_t trace = SV_Trace_Toss(ent, ignore);
	SV_SetGlobalTrace(&trace);
}

/* <78c06> ../engine/pr_cmds.c:758 */
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
	if (ptrace->ent)
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

/* <78dc1> ../engine/pr_cmds.c:775 */
void EXT_FUNC PF_TraceToss_DLL(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
	PF_TraceToss_Shared(pent, pentToIgnore ? pentToIgnore : &g_psv.edicts[0]);

	ptr->fAllSolid = (int) gGlobalVariables.trace_allsolid;
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
}

/* <78e3a> ../engine/pr_cmds.c:791 */
int EXT_FUNC TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	qboolean monsterClip = (pEdict->v.flags & FL_MONSTERCLIP) ? 1 : 0;
	trace_t trace = SV_Move(v1, pEdict->v.mins, pEdict->v.maxs, v2, fNoMonsters, pentToSkip, monsterClip);
	if (ptr)
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
}

/* <78ed1> ../engine/pr_cmds.c:830 */
int EXT_FUNC PF_newcheckclient(int check)
{
	int i;
	unsigned char *pvs;
	edict_t *ent;
	mleaf_t *leaf;
	vec3_t org;

	if (check < 1)
		check = 1;
	if (check > g_psvs.maxclients)
		check = g_psvs.maxclients;
	i = 1;
	if (check != g_psvs.maxclients)
		i = check + 1;
	while (1)
	{
		if (i == g_psvs.maxclients + 1)
			i = 1;

		ent = &g_psv.edicts[i];
		if (i == check)
			break;
		if (!ent->free && ent->pvPrivateData && !(ent->v.flags & FL_NOTARGET))
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

/* <78f53> ../engine/pr_cmds.c:898 */
edict_t* EXT_FUNC PF_checkclient_I(edict_t *pEdict)
{
	edict_t *ent; 
	mleaf_t *leaf; 
	int l;
	vec3_t view;

	if (g_psv.time - g_psv.lastchecktime >= 0.1)
	{
		g_psv.lastcheck = PF_newcheckclient(g_psv.lastcheck);
		g_psv.lastchecktime = g_psv.time;
	}

	ent = &g_psv.edicts[g_psv.lastcheck];
	if (!ent->free && ent->pvPrivateData)
	{
		view[0] = pEdict->v.view_ofs[0] + pEdict->v.origin[0];
		view[1] = pEdict->v.view_ofs[1] + pEdict->v.origin[1];
		view[2] = pEdict->v.view_ofs[2] + pEdict->v.origin[2];
		leaf = Mod_PointInLeaf(view, g_psv.worldmodel);
		l = (leaf - g_psv.worldmodel->leafs) - 1;
		if (l >= 0 && ((1 << (l & 7)) & checkpvs[l >> 3]))
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
}

/* <78fbe> ../engine/pr_cmds.c:942 */
mnode_t* EXT_FUNC PVSNode(mnode_t *node, vec_t *emins, vec_t *emaxs)
{
	mplane_t *splitplane;
	int sides;
	mnode_t *splitNode;

	if (node->visframe != r_visframecount)
		return NULL;

	if (node->contents < 0)
		return node->contents != CONTENT_SOLID ? node : NULL;
	

	splitplane = node->plane;
	if (splitplane->type >= 3u)
	{
		sides = BoxOnPlaneSide(emins, emaxs, splitplane);
	}
	else
	{
		if (splitplane->dist > emins[splitplane->type])
		{
			if (splitplane->dist < emaxs[splitplane->type])
				sides = 3;
			else
				sides = 2;
		}
		else
		{
			sides = 1;
		}
	}

	if (sides & 1)
	{
		splitNode = PVSNode(node->children[0], emins, emaxs);
		if (splitNode)
			return splitNode;
	}

	if (sides & 2)
		return PVSNode(node->children[1], emins, emaxs);

	return NULL;
}

/* <7903a> ../engine/pr_cmds.c:976 */
void EXT_FUNC PVSMark(model_t *pmodel, unsigned char *ppvs)
{
	++r_visframecount;
	for (int i = 0; i < pmodel->numleafs; i++)
	{
		if ((1 << (i & 7)) & ppvs[i >> 3])
		{
			mnode_t *node = (mnode_t *) &pmodel->leafs[i + 1];
			do
			{
				if (node->visframe == r_visframecount)
					break;
				node->visframe = r_visframecount;
				node = node->parent;
			} while (node);
		}
	}
}

/* <790b0> ../engine/pr_cmds.c:1009 */
edict_t* EXT_FUNC PVSFindEntities(edict_t *pplayer)
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

	for (int i = 1; i < g_psv.num_edicts; i++)
	{
		pent = &g_psv.edicts[i];
		if (pent->free)
			continue;

		pentPVS = pent;
		if (pent->v.movetype == MOVETYPE_FOLLOW && pent->v.aiment)
			pentPVS = pent->v.aiment;

		if (PVSNode(g_psv.worldmodel->nodes, pentPVS->v.absmin, pentPVS->v.absmax))
		{
			pent->v.chain = pchain;
			pchain = pent;
		}
		
	}
	
	if (g_pcl.worldmodel)
	{
		//r_oldviewleaf = NULL; //clientside only
		R_MarkLeaves();
	}
	return pchain;
}

/* <79182> ../engine/pr_cmds.c:1055 */
qboolean EXT_FUNC ValidCmd(const char *pCmd)
{
	int len = Q_strlen(pCmd);
	return len && (pCmd[len - 1] == '\n' || pCmd[len - 1] == ';');
} 

/* <791d5> ../engine/pr_cmds.c:1079 */
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
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf(
			"\n!!!\n\nStuffCmd:  Some entity tried to stuff '%s' to console buffer of entity %i when maxclients was set to %i, ignoring\n\n",
			szOut,
			entnum,
			g_psvs.maxclients);
	}
	else
	{
		if (ValidCmd(szOut))
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
}

/* <79292> ../engine/pr_cmds.c:1119 */
void EXT_FUNC PF_localcmd_I(char *str)
{
	if (ValidCmd(str))
		Cbuf_AddText(str);
	else
		Con_Printf("Error, bad server command %s\n", str);
}

/* <792e8> ../engine/pr_cmds.c:1137 */
void EXT_FUNC PF_localexec_I(void)
{
	Cbuf_Execute();
}

/* <792fd> ../engine/pr_cmds.c:1154 */
edict_t* EXT_FUNC FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
	int e = pEdictStartSearchAfter ? NUM_FOR_EDICT(pEdictStartSearchAfter) : 0;

	for (int i = e + 1; i < g_psv.num_edicts; i++)
	{
		edict_t* ent = &g_psv.edicts[i];
		if (ent->free || !ent->v.classname)
			continue;

		if (i <= g_psvs.maxclients && !g_psvs.clients[i - 1].active)
			continue;


		float distSquared = 0.0;
		for (int j = 0; j < 3 && distSquared <= (rad * rad); j++)
		{
			float eorg;
			if (org[j] >= ent->v.absmin[j])
				eorg = (org[j] <= ent->v.absmax[j]) ? 0.0f : org[j] - ent->v.absmax[j];
			else
				eorg = org[j] - ent->v.absmin[j];
			distSquared = eorg * eorg + distSquared;
		}

		if (distSquared <= ((rad * rad)))
			return ent;
	}

	return &g_psv.edicts[0];
}

/* <793a2> ../engine/pr_cmds.c:1219 */
edict_t* EXT_FUNC PF_Spawn_I(void)
{
	return ED_Alloc();
}

/* <793cc> ../engine/pr_cmds.c:1226 */
edict_t* EXT_FUNC CreateNamedEntity(int className)
{
	edict_t *pedict;
	ENTITYINIT pEntityInit;

	if (!className)
		Sys_Error("Spawned a NULL entity!");

	pedict = ED_Alloc();
	pedict->v.classname = className;
	pEntityInit = GetEntityInit(&pr_strings[className]);
	if (pEntityInit)
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
}

void EXT_FUNC PF_Remove_I(edict_t *ed)
{
	g_RehldsHookchains.m_PF_Remove_I.callChain(PF_Remove_I_internal, ed);
}

/* <7941a> ../engine/pr_cmds.c:1253 */
void EXT_FUNC PF_Remove_I_internal(edict_t *ed)
{
	ED_Free(ed);
}

/* <7820f> ../engine/pr_cmds.c:1263 */
edict_t* EXT_FUNC PF_find_Shared(int eStartSearchAfter, int iFieldToMatch, const char *szValueToFind)
{
	for (int e = eStartSearchAfter + 1; e < g_psv.num_edicts; e++)
	{
		edict_t* ed = &g_psv.edicts[e];
		if (ed->free)
			continue;

		char* t = &pr_strings[*(string_t*)((size_t)&ed->v + iFieldToMatch)];
		if (t == 0 || t == &pr_strings[0])
			continue;

		if (!Q_strcmp(t, szValueToFind))
			return ed;

	}
	return &g_psv.edicts[0];

}

/* <79442> ../engine/pr_cmds.c:1290 */

int EXT_FUNC iGetIndex(const char *pszField)
{
	char sz[512];

	Q_strncpy(sz, pszField, sizeof(sz) - 1);
	sz[sizeof(sz) - 1] = 0;
	Q_strlwr(sz);

	#define IGETINDEX_CHECK_FIELD(f) 	if (!Q_strcmp(sz, #f)) return offsetof(entvars_t, f);

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

/* <7949b> ../engine/pr_cmds.c:1332 */
edict_t* EXT_FUNC FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
	if (!pszValue)
		return NULL;

	int iField = iGetIndex(pszField);
	if (iField == -1)
		return NULL;

	return PF_find_Shared(pEdictStartSearchAfter ? NUM_FOR_EDICT(pEdictStartSearchAfter) : 0, iField, pszValue);
}

/* <79540> ../engine/pr_cmds.c:1348 */
int EXT_FUNC GetEntityIllum(edict_t *pEnt)
{
	if (!pEnt)
		return -1;

	if (NUM_FOR_EDICT(pEnt) <= g_psvs.maxclients)
	{
		return pEnt->v.light_level;
	}
	else
	{
		if (g_pcls.state == ca_connected || g_pcls.state == ca_uninitialized || g_pcls.state == ca_active)
			return 0x80;
		else
			return 0;
	}
}

/* <78265> ../engine/pr_cmds.c:1383 */
qboolean EXT_FUNC PR_IsEmptyString(const char *s)
{
	return s[0] < ' ';
}

/* <795b5> ../engine/pr_cmds.c:1397 */
int EXT_FUNC PF_precache_sound_I(const char *s)
{
	int i;

	if (!s)
		Host_Error("PF_precache_sound_I: NULL pointer");

	if (PR_IsEmptyString(s))
		Host_Error("PF_precache_sound_I: Bad string '%s'", s);

	if (s[0] == '!')
		Host_Error("PF_precache_sound_I: '%s' do not precache sentence names!", s);

	if (g_psv.state == ss_loading)
	{
		g_psv.sound_precache_hashedlookup_built = 0;

		for (i = 0; i < HL_SOUND_MAX; i++)
		{
			if (!g_psv.sound_precache[i])
			{
#ifdef REHLDS_FIXES
				// For more information, see EV_Precache
				g_psv.sound_precache[i] = ED_NewString(s);
#else
				g_psv.sound_precache[i] = s;
#endif // REHLDS_FIXES
				return i;
			}

			if (!Q_stricmp(g_psv.sound_precache[i], s))
				return i;
		}

		Host_Error(
			"PF_precache_sound_I: Sound '%s' failed to precache because the item count is over the %d limit.\nReduce the number of brush models and/or regular models in the map to correct this.",
			s,
			HL_SOUND_MAX);
	}
	else
	{
		// precaching not enabled. check if already exists.
		for (i = 0; i < HL_SOUND_MAX; i++)
		{
			if (g_psv.sound_precache[i] && !Q_stricmp(g_psv.sound_precache[i], s))
					return i;
		}

		Host_Error("PF_precache_sound_I: '%s' Precache can only be done in spawn functions", s);
	}

	return -1; // unreach
}

/* <79609> ../engine/pr_cmds.c:1455 */
short unsigned int EXT_FUNC EV_Precache(int type, const char *psz)
{
	if (!psz)
		Host_Error("EV_Precache: NULL pointer");

	if (PR_IsEmptyString(psz))
		Host_Error("EV_Precache: Bad string '%s'", psz);

	if (g_psv.state == ss_loading)
	{
		for (int i = 1; i < HL_EVENT_MAX; i++)
		{
			struct event_s* ev = &g_psv.event_precache[i];
			if (!ev->filename)
			{
				if (type != 1)
					Host_Error("EV_Precache:  only file type 1 supported currently\n");

				char szpath[MAX_PATH];
				Q_snprintf(szpath, sizeof(szpath), "%s", psz);
				COM_FixSlashes(szpath);

				int scriptSize = 0;
				char* evScript = (char*) COM_LoadFile(szpath, 5, &scriptSize);
				if (!evScript)
					Host_Error("EV_Precache:  file %s missing from server\n", psz);
#ifdef REHLDS_FIXES
				// Many modders don't know that the resource names passed to precache functions must be a static strings.
				// Also some metamod modules can be unloaded during the server running.
				// Thereafter the pointers to resource names, precached by this modules becomes invalid and access to them will cause segfault error.
				// Reallocating strings in the rehlds temporary hunk memory helps to avoid these problems.
				g_psv.event_precache[i].filename = ED_NewString(psz);
#else
				g_psv.event_precache[i].filename = psz;
#endif // REHLDS_FIXES
				g_psv.event_precache[i].filesize = scriptSize;
				g_psv.event_precache[i].pszScript = evScript;
				g_psv.event_precache[i].index = i;

				return i;
			}

			if (!Q_stricmp(ev->filename, psz))
				return i;
		}
		Host_Error("EV_Precache: '%s' overflow", psz);
	}
	else
	{
		for (int i = 1; i < HL_EVENT_MAX; i++)
		{
			struct event_s* ev = &g_psv.event_precache[i];
			if (!Q_stricmp(ev->filename, psz))
				return i;
		}

		Host_Error("EV_Precache: '%s' Precache can only be done in spawn functions", psz);
	}
}

void EXT_FUNC EV_PlayReliableEvent_api(IGameClient *cl, int entindex, short unsigned int eventindex, float delay, event_args_t *pargs)
{
	EV_PlayReliableEvent_internal(cl->GetClient(), entindex, eventindex, delay, pargs);
}

void EV_PlayReliableEvent(client_t *cl, int entindex, short unsigned int eventindex, float delay, event_args_t *pargs)
{
	g_RehldsHookchains.m_EV_PlayReliableEvent.callChain(EV_PlayReliableEvent_api, GetRehldsApiClient(cl), entindex, eventindex, delay, pargs);
}

/* <796ae> ../engine/pr_cmds.c:1531 */
void EV_PlayReliableEvent_internal(client_t *cl, int entindex, short unsigned int eventindex, float delay, event_args_t *pargs)
{
//	unsigned char data;                                           //  1533
//	sizebuf_t msg;                                                //  1534
//	event_args_t eargs;                                           //  1536
//	event_args_t nullargs;                                        //  1537

	unsigned char data[1024]; // [sp+4h] [bp-4A4h]@2
	event_args_t from; // [sp+404h] [bp-A4h]@2
	event_args_t to; // [sp+44Ch] [bp-5Ch]@2
	sizebuf_t msg; // [sp+494h] [bp-14h]@2

	if (cl->fakeclient)
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
	if (delay == 0.0)
	{
		MSG_WriteBits(0, 1);
	}
	else
	{
		MSG_WriteBits(1, 1);
		MSG_WriteBits((int)(delay * 100.0f), 16);
	}
	MSG_EndBitWriting(&msg);

	if (msg.cursize + cl->netchan.message.cursize <= cl->netchan.message.maxsize)
		SZ_Write(&cl->netchan.message, msg.data, msg.cursize);
	else
		Netchan_CreateFragments(1, &cl->netchan, &msg);
	
}

/* <79769> ../engine/pr_cmds.c:1595 */
void EXT_FUNC EV_Playback(int flags, const edict_t *pInvoker, short unsigned int eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	client_t *cl;
	signed int j;
	event_args_t eargs;
	vec3_t event_origin;
	int invoker;
	int slot;
	int leafnum;
	
	if (flags & FEV_CLIENT)
		return;

	invoker = -1;
	Q_memset(&eargs, 0, sizeof(eargs));
	if (!VectorCompare(origin, vec3_origin))
	{
		eargs.origin[2] = origin[2];
		eargs.origin[0] = origin[0];
		eargs.origin[1] = origin[1];
		eargs.flags |= FEVENT_ORIGIN;
	}
	if (!VectorCompare(angles, vec3_origin))
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

	if (eventindex < 1u || eventindex >= HL_EVENT_MAX)
	{
		Con_DPrintf("EV_Playback:  index out of range %i\n", eventindex);
		return;
	}

	if (!g_psv.event_precache[eventindex].pszScript)
	{
		Con_DPrintf("EV_Playback:  no event for index %i\n", eventindex);
		return;
	}

	if (pInvoker)
	{
		event_origin[0] = pInvoker->v.origin[0];
		event_origin[1] = pInvoker->v.origin[1];
		event_origin[2] = pInvoker->v.origin[2];
		invoker = NUM_FOR_EDICT(pInvoker);
		if (invoker >= 1)
		{
			if (invoker <= g_psvs.maxclients)
			{
				if (pInvoker->v.flags & FL_DUCKING)
					eargs.ducking = 1;
			}
		}
		if (!(eargs.flags & FEVENT_ORIGIN))
		{
			eargs.origin[0] = pInvoker->v.origin[0];
			eargs.origin[1] = pInvoker->v.origin[1];
			eargs.origin[2] = pInvoker->v.origin[2];
		}
		if (!(eargs.flags & FEVENT_ANGLES))
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

	for (slot = 0; slot < g_psvs.maxclients; slot++)
	{
		cl = &g_psvs.clients[slot];
		if (!cl->active || !cl->spawned || !cl->connected || !cl->fully_connected || cl->fakeclient)
			continue;

		if (pInvoker)
		{
			if (pInvoker->v.groupinfo)
			{
				if (cl->edict->v.groupinfo)
				{
					if (g_groupop)
					{
						if (g_groupop == GROUP_OP_NAND && (cl->edict->v.groupinfo & pInvoker->v.groupinfo))
							continue;
					}
					else
					{
						if (!(cl->edict->v.groupinfo & pInvoker->v.groupinfo))
							continue;
					}
				}
			}
		}

		if (pInvoker && !(flags & FEV_GLOBAL))
		{
			if (!SV_ValidClientMulticast(cl, leafnum, 4))
				continue;
		}

		if (cl == host_client && (flags & FEV_NOTHOST) && cl->lw || (flags & FEV_HOSTONLY) && cl->edict != pInvoker)
			continue;

		if (flags & FEV_RELIABLE)
		{
			EV_PlayReliableEvent(cl, pInvoker ? NUM_FOR_EDICT(pInvoker) : 0, eventindex, delay, &eargs);
			continue;
		}

		if (flags & FEV_UPDATE)
		{
			for (j = 0; j < MAX_EVENT_QUEUE; j++)
			{
				event_info_s *ei = &cl->events.ei[j];
				if (ei->index == eventindex && invoker != -1 && ei->entity_index == invoker)
					break;
			}

			if (j < MAX_EVENT_QUEUE)
			{
				event_info_s *ei = &cl->events.ei[j];
				ei->entity_index = -1;
				ei->index = eventindex;
				ei->packet_index = -1;
				if (pInvoker)
					ei->entity_index = invoker;
				Q_memcpy(&ei->args, &eargs, sizeof(ei->args));
				ei->fire_time = delay;
				continue;
			}
		}

		for (j = 0; j < MAX_EVENT_QUEUE; j++)
		{
			event_info_s *ei = &cl->events.ei[j];
			if (ei->index == 0)
				break;
		}

		if (j < MAX_EVENT_QUEUE)
		{
			event_info_s *ei = &cl->events.ei[j];
			ei->entity_index = -1;
			ei->index = eventindex;
			ei->packet_index = -1;
			if (pInvoker)
				ei->entity_index = invoker;
			Q_memcpy(&ei->args, &eargs, sizeof(ei->args));
			ei->fire_time = delay;
		}
		
	}
}

/* <798fb> ../engine/pr_cmds.c:1826 */
void EXT_FUNC EV_SV_Playback(int flags, int clientindex, short unsigned int eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	if (flags & FEV_CLIENT)
		return;

	if (clientindex < 0 || clientindex >= g_psvs.maxclients)
		Host_Error("EV_SV_Playback:  Client index %i out of range\n", clientindex);

	edict_t *pEdict = g_psvs.clients[clientindex].edict;
	EV_Playback(flags,pEdict, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}

#ifdef REHLDS_FIXES
int SV_LookupModelIndex(const char *name)
{
	if (!name || !name[0])
		return 0;

#ifdef REHLDS_OPT_PEDANTIC
	auto node = g_rehlds_sv.modelsMap.get(name);
	if (node) {
		return node->val;
	}
#else // REHLDS_OPT_PEDANTIC
	for (int i = 0; i < HL_MODEL_MAX; i++)
	{
		if (!g_psv.model_precache[i])
			break;

		if (!Q_stricmp(g_psv.model_precache[i], name))
			return i;
	};
#endif // REHLDS_OPT_PEDANTIC

	return 0;
}
#endif // REHLDS_FIXES

/* <799da> ../engine/pr_cmds.c:1849 */
int EXT_FUNC PF_precache_model_I(const char *s)
{
	int iOptional = 0;
	if (!s)
		Host_Error("PF_precache_model_I: NULL pointer");

	if (PR_IsEmptyString(s))
		Host_Error("PF_precache_model_I: Bad string '%s'", s);

	if (*s == '!')
	{
		s++;
		iOptional = 1;
	}

	if (g_psv.state == ss_loading)
	{
		for (int i = 0; i < HL_MODEL_MAX; i++)
		{
			if (!g_psv.model_precache[i])
			{
#ifdef REHLDS_FIXES
				// For more information, see EV_Precache
				g_psv.model_precache[i] = ED_NewString(s);
#else
				g_psv.model_precache[i] = s;
#endif // REHLDS_FIXES

#ifdef REHLDS_OPT_PEDANTIC
				g_rehlds_sv.modelsMap.put(g_psv.model_precache[i], i);
#endif //REHLDS_OPT_PEDANTIC

				g_psv.models[i] = Mod_ForName(s, 1, 1);
				if (!iOptional)
					g_psv.model_precache_flags[i] |= 1u;

				return i;
			}

			//use case-sensitive names to increase performance
#ifdef REHLDS_FIXES
			if (!Q_strcmp(g_psv.model_precache[i], s))
				return i;
#else
			if (!Q_stricmp(g_psv.model_precache[i], s))
				return i;
#endif
		}
		Host_Error(
			"PF_precache_model_I: Model '%s' failed to precache because the item count is over the %d limit.\nReduce the number of brush models and/or regular models in the map to correct this.",
			s,
			512);
	}
	else
	{
		for (int i = 0; i < HL_MODEL_MAX; i++)
		{
			//use case-sensitive names to increase performance
#ifdef REHLDS_FIXES
			if (!Q_strcmp(g_psv.model_precache[i], s))
				return i;
#else
			if (!Q_stricmp(g_psv.model_precache[i], s))
				return i;
#endif
		}
		Host_Error("PF_precache_model_I: '%s' Precache can only be done in spawn functions", s);
	}
}

#ifdef REHLDS_FIXES
/* <79a3f> ../engine/pr_cmds.c:1903 */
int EXT_FUNC PF_precache_generic_I(char *s)
// TODO: Call to Con_Printf is replaced with Host_Error in 6153
{
	if (!s)
		Host_Error("PF_precache_generic_I: NULL pointer");

	if (PR_IsEmptyString(s))
		Host_Error("PF_precache_generic_I: Bad string '%s'", s);

	char resName[MAX_QPATH];
	Q_strncpy(resName, s, sizeof(resName));
	resName[sizeof(resName) - 1] = '\0';
	ForwardSlashes(resName);

	size_t soundPrefixLength = sizeof("sound/") - 1;
	bool isSoundPrefixed = !Q_strnicmp(resName, "sound/", soundPrefixLength);

	// TODO: check sound with index 0?
	// UPD: no, not need, because engine do this: g_psv.sound_precache[0] = pr_strings;
	if ((isSoundPrefixed && SV_LookupSoundIndex(&resName[soundPrefixLength])) ||
		SV_LookupModelIndex(resName))
		return 0;

	size_t resCount = g_rehlds_sv.precachedGenericResourceCount;
	for (size_t i = 0; i < resCount; i++)
	{
		if (!Q_stricmp(g_rehlds_sv.precachedGenericResourceNames[i], resName))
			return i;
	}

	if (g_psv.state != ss_loading)
		Host_Error("PF_precache_generic_I: '%s' Precache can only be done in spawn functions", resName);
	
	if (resCount >= ARRAYSIZE(g_rehlds_sv.precachedGenericResourceNames))
		Host_Error(
			"PF_precache_generic_I: Generic item '%s' failed to precache because the item count is over the %d limit.\n\
Reduce the number of brush models and/or regular models in the map to correct this.",
			resName,
			ARRAYSIZE(g_rehlds_sv.precachedGenericResourceNames));

	Q_strcpy(g_rehlds_sv.precachedGenericResourceNames[resCount], resName);

	return g_rehlds_sv.precachedGenericResourceCount++;
}
#else // REHLDS_FIXES
/* <79a3f> ../engine/pr_cmds.c:1903 */
int EXT_FUNC PF_precache_generic_I(char *s)
// TODO: Call to Con_Printf is replaced with Host_Error in 6153
{
	if (!s)
		Host_Error("PF_precache_generic_I: NULL pointer");

	if (PR_IsEmptyString(s))
		Host_Error("PF_precache_generic_I: Bad string '%s'", s);

	if (g_psv.state == ss_loading)
	{
		for (int i = 0; i < HL_GENERIC_MAX; i++)
		{
			if (!g_psv.generic_precache[i])
			{
				g_psv.generic_precache[i] = s;
				return i;
			}

			if (!Q_stricmp(g_psv.generic_precache[i], s))
				return i;
		}
		Host_Error(
			"PF_precache_generic_I: Generic item '%s' failed to precache because the item count is over the %d limit.\nReduce the number of brush models and/or regular models in the map to correct this.",
			s,
			HL_GENERIC_MAX);
	}
	else
	{
		for (int i = 0; i < HL_GENERIC_MAX; i++)
		{
			if (!Q_stricmp(g_psv.generic_precache[i], s))
				return i;
		}
		Host_Error("PF_precache_generic_I: '%s' Precache can only be done in spawn functions", s);
	}
}
#endif // REHLDS_FIXES

/* <79a93> ../engine/pr_cmds.c:1944 */
int EXT_FUNC PF_IsMapValid_I(char *mapname)
{
	char cBuf[260];
	if (!mapname || Q_strlen(mapname) == 0)
		return 0;

	
	Q_snprintf(cBuf, sizeof(cBuf), "maps/%.32s.bsp", mapname);
	return FS_FileExists(cBuf);
}

/* <79ad1> ../engine/pr_cmds.c:1955 */
int EXT_FUNC PF_NumberOfEntities_I(void)
{
	int ent_count = 0;
	for (int i = 1; i < g_psv.num_edicts; i++)
	{
		if (!g_psv.edicts[i].free)
			++ent_count;
	}

	return ent_count;
}

/* <79b0e> ../engine/pr_cmds.c:1977 */
char* EXT_FUNC PF_GetInfoKeyBuffer_I(edict_t *e)
{
	int e1;
	char *value;

	if (e)
	{
		e1 = NUM_FOR_EDICT(e);
		if (e1)
		{
#ifdef REHLDS_FIXES
			if (e1 <= 0 || e1 > g_psvs.maxclients)	// FIXED: Check against correct amount of clients
#else // REHLDS_FIXES
			if (e1 > 32)
#endif // REHLDS_FIXES
				value = (char *)"";
			else
				value = (char *)&g_psvs.clients[e1 - 1].userinfo;
		}
		else
		{
			value = Info_Serverinfo();
		}
	}
	else
	{
		value = localinfo;
	}

	return value;
}

/* <79b55> ../engine/pr_cmds.c:2012 */
char* EXT_FUNC PF_InfoKeyValue_I(char *infobuffer, const char *key)
{
	return (char *)Info_ValueForKey(infobuffer, key);
}

/* <79b91> ../engine/pr_cmds.c:2022 */
void EXT_FUNC PF_SetKeyValue_I(char *infobuffer, const char *key, const char *value)
{
	if (infobuffer == localinfo)
	{
		Info_SetValueForKey(infobuffer, key, value, MAX_INFO_STRING * 128);
	}
	else
	{
		if (infobuffer != Info_Serverinfo())
		{
			Sys_Error("Can't set client keys with SetKeyValue");
		}
#ifdef REHLDS_FIXES
		Info_SetValueForKey(infobuffer, key, value, MAX_INFO_STRING);	// Use correct length
#else // REHLDS_FIXES
		Info_SetValueForKey(infobuffer, key, value, 512);
#endif // REHLDS_FIXES
	}
}

/* <79bd9> ../engine/pr_cmds.c:2037 */
void EXT_FUNC PF_RemoveKey_I(char *s, const char *key)
{
	Info_RemoveKey(s, key);
}

/* <79c0f> ../engine/pr_cmds.c:2047 */
void EXT_FUNC PF_SetClientKeyValue_I(int clientIndex, char *infobuffer, const char *key, const char *value)
{
	client_t *pClient;

	if (infobuffer == localinfo ||
		infobuffer == Info_Serverinfo() ||
		clientIndex <= 0 ||
		clientIndex > g_psvs.maxclients)
	{
		return;
	}

	if (Q_strcmp(Info_ValueForKey(infobuffer, key), value))
	{
		Info_SetValueForStarKey(infobuffer, key, value, MAX_INFO_STRING);
		pClient = &g_psvs.clients[clientIndex - 1];
		pClient->sendinfo = TRUE;
		pClient->sendinfo_time = 0.0f;
	}
}

/* <79c74> ../engine/pr_cmds.c:2078 */
int EXT_FUNC PF_walkmove_I(edict_t *ent, float yaw, float dist, int iMode)
{
	vec3_t move;

	if (ent->v.flags & (FL_SWIM | FL_FLY | FL_ONGROUND))
	{
		move[0] = cos(yaw * 2.0 * M_PI / 360.0) * dist;
		move[1] = sin(yaw * 2.0 * M_PI / 360.0) * dist;
		move[2] = 0;

		switch (iMode) {
		case 1:
			return SV_movetest(ent, move, 1);

		case 2:
			return SV_movestep(ent, move, 0);

		default:
			return SV_movestep(ent, move, 1);
		}
	}
	return 0;
}

/* <79cef> ../engine/pr_cmds.c:2120 */
int EXT_FUNC PF_droptofloor_I(edict_t *ent)
{
	vec3_t end;
	trace_t trace;
	qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) ? 1 : 0;

	end[0] = ent->v.origin[0];
	end[1] = ent->v.origin[1];
	end[2] = ent->v.origin[2] - 256.0;
	trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, 0, ent, monsterClip);
	if (trace.allsolid)
		return -1;

	if (trace.fraction == 1.0f)
		return 0;

	ent->v.origin[0] = trace.endpos[0];
	ent->v.origin[1] = trace.endpos[1];
	ent->v.origin[2] = trace.endpos[2];
	SV_LinkEdict(ent, 0);
	ent->v.flags |= FL_ONGROUND;
	ent->v.groundentity = trace.ent;

	return 1;
}

/* <79d4c> ../engine/pr_cmds.c:2158 */
int EXT_FUNC PF_DecalIndex(const char *name)
{
	for (int i = 0; i < sv_decalnamecount; i++)
	{
		if (!Q_stricmp(sv_decalnames[i].name, name))
			return i;
	}

	return -1;
}

/* <79d88> ../engine/pr_cmds.c:2180 */
void EXT_FUNC PF_lightstyle_I(int style, char *val)
{
	g_psv.lightstyles[style] = val;
	if (g_psv.state != ss_active)
		return;

	for (int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t* cl = &g_psvs.clients[i];
		if ((cl->active || cl->spawned) && !cl->fakeclient)
		{
			MSG_WriteChar(&cl->netchan.message, svc_lightstyle);
			MSG_WriteChar(&cl->netchan.message, style);
			MSG_WriteString(&cl->netchan.message, val);
		}
	}
}

/* <79ddf> ../engine/pr_cmds.c:2209 */
int EXT_FUNC PF_checkbottom_I(edict_t *pEdict)
{
	return SV_CheckBottom(pEdict);
}

/* <79e0c> ../engine/pr_cmds.c:2221 */
int EXT_FUNC PF_pointcontents_I(const float *rgflVector)
{
	return SV_PointContents(rgflVector);
}

/* <79e39> ../engine/pr_cmds.c:2237 */
void EXT_FUNC PF_aim_I(edict_t *ent, float speed, float *rgflReturn)
{
	vec3_t start;                                                 //  2240
	vec3_t dir;                                                   //  2240
	vec3_t end;                                                   //  2240
	vec3_t bestdir;                                               //  2240
	int j;                                                        //  2241
	trace_t tr;                                                   //  2242
	float dist;                                                   //  2243
	float bestdist;                                               //  2243

	if (!ent || (ent->v.flags & FL_FAKECLIENT))
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

	if (tr.ent && tr.ent->v.takedamage == 2.0f && (ent->v.team <= 0 || ent->v.team != tr.ent->v.team))
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

	for (int i = 1; i < g_psv.num_edicts; i++)
	{
		edict_t* check = &g_psv.edicts[i];
		if (check->v.takedamage != 2.0f || (check->v.flags & FL_FAKECLIENT) || check == ent)
			continue;

		if (ent->v.team > 0 && ent->v.team == check->v.team)
			continue;

		for (j = 0; j < 3; j++)
		{
			end[j] = (check->v.maxs[j] + check->v.mins[j]) * 0.75 + check->v.origin[j] + ent->v.view_ofs[j] * 0.0;
		}

		dir[0] = end[0] - start[0];
		dir[1] = end[1] - start[1];
		dir[2] = end[2] - start[2];
		VectorNormalize(dir);
		dist = gGlobalVariables.v_forward[2] * dir[2]
			+ gGlobalVariables.v_forward[1] * dir[1]
			+ gGlobalVariables.v_forward[0] * dir[0];

		if (dist >= bestdist)
		{
			tr = SV_Move(start, vec3_origin, vec3_origin, end, 0, ent, 0);
			if (tr.ent == check)
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
}

/* <79f2a> ../engine/pr_cmds.c:2313 */
void EXT_FUNC PF_changeyaw_I(edict_t *ent)
{
	float ideal;
	float current;
	float move;
	float speed;

	current = anglemod(ent->v.angles[1]);
	ideal = ent->v.ideal_yaw;
	speed = ent->v.yaw_speed;
	if (current == ideal)
		return;

	move = ideal - current;
	if (ideal <= current)
	{
		if (move <= -180.0)
			move += 360.0;
	}
	else
	{
		if (move >= 180.0)
			move -= 360.0;
	}

	if (move <= 0.0f)
	{
		if (move < -speed)
			move = -speed;
	}
	else
	{
		if (move > speed)
			move = speed;
	}

	ent->v.angles[1] = anglemod(move + current);
}

/* <79f94> ../engine/pr_cmds.c:2355 */
void EXT_FUNC PF_changepitch_I(edict_t *ent)
{
	float ideal;
	float current;
	float move;
	float speed;

	current = anglemod(ent->v.angles[0]);
	ideal = ent->v.idealpitch;
	speed = ent->v.pitch_speed;
	if (current == ideal)
		return;

	move = ideal - current;
	if (ideal <= (double)current)
	{
		if (move <= -180.0)
			move += 360.0;
	}
	else
	{
		if (move >= 180.0)
			move -= 360;
	}

	if (move <= 0.0)
	{
		if (move < -speed)
			move = -speed;
	}
	else
	{
		if (move > speed)
			move = speed;
	}

	ent->v.angles[0] = anglemod(move + current);
}

/* <79ffe> ../engine/pr_cmds.c:2398 */
void EXT_FUNC PF_setview_I(const edict_t *clientent, const edict_t *viewent)
{
	int clientnum = NUM_FOR_EDICT(clientent);
	if (clientnum < 1 || clientnum > g_psvs.maxclients)
		Host_Error("PF_setview_I: not a client");

	client_t * client = &g_psvs.clients[clientnum - 1];
	if (!client->fakeclient)
	{
		client->pViewEntity = viewent;
		MSG_WriteByte(&client->netchan.message, svc_setview);
		MSG_WriteShort(&client->netchan.message, NUM_FOR_EDICT(viewent));
	}
}

/* <7a057> ../engine/pr_cmds.c:2426 */
void EXT_FUNC PF_crosshairangle_I(const edict_t *clientent, float pitch, float yaw)
{
	int clientnum = NUM_FOR_EDICT(clientent);
	if (clientnum < 1 || clientnum > g_psvs.maxclients)
		return;


	client_t* client = &g_psvs.clients[clientnum - 1];
	if (!client->fakeclient)
	{
		if (pitch > 180.0)
			pitch -= 360.0;

		if (pitch < -180.0)
			pitch += 360.0;

		if (yaw > 180.0)
			yaw -= 360.0;

		if (yaw < -180.0)
			yaw += 360.0;

		MSG_WriteByte(&client->netchan.message, svc_crosshairangle);
		MSG_WriteChar(&client->netchan.message, (int)(pitch * 5.0));
		MSG_WriteChar(&client->netchan.message, (int)(yaw * 5.0));
	}
}

/* <7a0c1> ../engine/pr_cmds.c:2457 */
edict_t* EXT_FUNC PF_CreateFakeClient_I(const char *netname)
{
	client_t *fakeclient;
	edict_t *ent;

	int i = 0;
	fakeclient = g_psvs.clients;
	for (i = 0; i < g_psvs.maxclients; i++, fakeclient++)
	{
		if (!fakeclient->active && !fakeclient->spawned && !fakeclient->connected)
			break;
	}

	if (i >= g_psvs.maxclients)
		return NULL;

	ent = EDICT_NUM(i + 1);
	if (fakeclient->frames)
		SV_ClearFrames(&fakeclient->frames);

	Q_memset(fakeclient, 0, sizeof(client_t));
	fakeclient->resourcesneeded.pPrev = &fakeclient->resourcesneeded;
	fakeclient->resourcesneeded.pNext = &fakeclient->resourcesneeded;
	fakeclient->resourcesonhand.pPrev = &fakeclient->resourcesonhand;
	fakeclient->resourcesonhand.pNext = &fakeclient->resourcesonhand;

	Q_strncpy(fakeclient->name, netname, sizeof(fakeclient->name) - 1);
	fakeclient->name[sizeof(fakeclient->name) - 1] = 0;

	fakeclient->active = 1;
	fakeclient->spawned = 1;
	fakeclient->fully_connected = 1;
	fakeclient->connected = 1;
	fakeclient->fakeclient = 1;
	fakeclient->userid = g_userid++;
	fakeclient->uploading = 0;
	fakeclient->edict = ent;
	ent->v.netname = (size_t)fakeclient->name - (size_t)pr_strings;
	ent->v.pContainingEntity = ent;
	ent->v.flags = FL_FAKECLIENT | FL_CLIENT;

	Info_SetValueForKey(fakeclient->userinfo, "name", netname, MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "model", "gordon", MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "topcolor", "1", MAX_INFO_STRING);
	Info_SetValueForKey(fakeclient->userinfo, "bottomcolor", "1", MAX_INFO_STRING);
	fakeclient->sendinfo = 1;
	SV_ExtractFromUserinfo(fakeclient);

	fakeclient->network_userid.m_SteamID = ISteamGameServer_CreateUnauthenticatedUserConnection();
	fakeclient->network_userid.idtype = AUTH_IDTYPE_STEAM;
	ISteamGameServer_BUpdateUserData(fakeclient->network_userid.m_SteamID, netname, 0);
	
	return ent;
}

/* <7a12b> ../engine/pr_cmds.c:2519 */
void EXT_FUNC PF_RunPlayerMove_I(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, short unsigned int buttons, unsigned char impulse, unsigned char msec)
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
}

/* <7a1fe> ../engine/pr_cmds.c:2578 */
sizebuf_t* EXT_FUNC WriteDest_Parm(int dest)
{
	int entnum;

	switch (dest)
	{
	case MSG_BROADCAST:
		return &g_psv.datagram;
	case MSG_ONE:
	case MSG_ONE_UNRELIABLE:
		entnum = NUM_FOR_EDICT(gMsgEntity);
		if (entnum <= 0 || entnum > g_psvs.maxclients)
		{
			Host_Error("WriteDest_Parm: not a client");
		}
		if (dest == MSG_ONE)
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
		Host_Error("WriteDest_Parm: bad destination=%d", dest);
	}
}

/* <7a23d> ../engine/pr_cmds.c:2624 */
void EXT_FUNC PF_MessageBegin_I(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	if (msg_dest == MSG_ONE || msg_dest == MSG_ONE_UNRELIABLE)
	{
		if (!ed)
			Sys_Error("MSG_ONE or MSG_ONE_UNRELIABLE with no target entity\n");
	}
	else
	{
		if (ed)
			Sys_Error("Invalid message;  cannot use broadcast message with a target entity");
	}

	if (gMsgStarted)
		Sys_Error("New message started when msg '%d' has not been sent yet", gMsgType);

	if (msg_type == 0)
		Sys_Error("Tried to create a message with a bogus message type ( 0 )");

	gMsgStarted = 1;
	gMsgType = msg_type;
	gMsgEntity = ed;
	gMsgDest = msg_dest;
	if (msg_dest == MSG_PVS || msg_dest == MSG_PAS)
	{
		if (pOrigin)
		{
			gMsgOrigin[0] = pOrigin[0];
			gMsgOrigin[1] = pOrigin[1];
			gMsgOrigin[2] = pOrigin[2];
		}

		//No idea why is it called here
		//Host_IsSinglePlayerGame();
	}

	gMsgBuffer.flags = SIZEBUF_ALLOW_OVERFLOW;
	gMsgBuffer.cursize = 0;
}

/* <7a293> ../engine/pr_cmds.c:2669 */
void EXT_FUNC PF_MessageEnd_I(void)
{
	qboolean MsgIsVarLength = 0;
	if (!gMsgStarted)
		Sys_Error("MESSAGE_END called with no active message\n");
	gMsgStarted = 0;

	if (gMsgEntity && (gMsgEntity->v.flags & FL_FAKECLIENT))
		return;

	if (gMsgBuffer.flags & SIZEBUF_OVERFLOWED)
		Sys_Error("MESSAGE_END called, but message buffer from .dll had overflowed\n");


	if (gMsgType > svc_startofusermessages)
	{
		UserMsg* pUserMsg = sv_gpUserMsgs;
		while (pUserMsg && pUserMsg->iMsg != gMsgType)
			pUserMsg = pUserMsg->next;

		if (!pUserMsg && gMsgDest == MSG_INIT)
		{
			pUserMsg = sv_gpNewUserMsgs;
			while (pUserMsg && pUserMsg->iMsg != gMsgType)
				pUserMsg = pUserMsg->next;
		}

		if (!pUserMsg)
		{
			Con_DPrintf("PF_MessageEnd_I:  Unknown User Msg %d\n", gMsgType);
			return;
		}

		if (pUserMsg->iSize == -1)
		{
			MsgIsVarLength = 1;

			// Limit packet sizes
			if (gMsgBuffer.cursize > MAX_USER_MSG_DATA)
				Host_Error(
					"PF_MessageEnd_I:  Refusing to send user message %s of %i bytes to client, user message size limit is %i bytes\n",
					pUserMsg->szName,
					gMsgBuffer.cursize,
					MAX_USER_MSG_DATA
				);
		}
		else
		{
			if (pUserMsg->iSize != gMsgBuffer.cursize)
				Sys_Error(
					"User Msg '%s': %d bytes written, expected %d\n",
					pUserMsg->szName,
					gMsgBuffer.cursize,
					pUserMsg->iSize
				);
		}
	}

	sizebuf_t * pBuffer = WriteDest_Parm(gMsgDest);
	if ((!gMsgDest && gMsgBuffer.cursize + pBuffer->cursize > pBuffer->maxsize) || !pBuffer->data)
		return;

	if (gMsgType > svc_startofusermessages && (gMsgDest == MSG_ONE || gMsgDest == MSG_ONE_UNRELIABLE))
	{
		int entnum = NUM_FOR_EDICT((const edict_t *)gMsgEntity);
		if (entnum < 1 || entnum > g_psvs.maxclients)
			Host_Error("WriteDest_Parm: not a client");

		client_t* client = &g_psvs.clients[entnum - 1];
		if (client->fakeclient || !client->hasusrmsgs || (!client->active && !client->spawned))
			return;
	}

	MSG_WriteByte(pBuffer, gMsgType);
	if (MsgIsVarLength)
		MSG_WriteByte(pBuffer, gMsgBuffer.cursize);
	MSG_WriteBuf(pBuffer, gMsgBuffer.cursize, gMsgBuffer.data);

	switch (gMsgDest)
	{
	case MSG_PVS:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PVS, 0);
		break;

	case MSG_PAS:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PAS, 0);
		break;

	case MSG_PVS_R:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PAS, 1); //TODO: Should be MSG_FL_PVS, investigation needed
		break;

	case MSG_PAS_R:
		SV_Multicast((edict_t *)gMsgEntity, gMsgOrigin, MSG_FL_PAS, 1);
		break;

	default:
		return;
	}
}

/* <7a317> ../engine/pr_cmds.c:2802 */
void EXT_FUNC PF_WriteByte_I(int iValue)
{
	if (!gMsgStarted)
		Sys_Error("WRITE_BYTE called with no active message\n");
	MSG_WriteByte(&gMsgBuffer, iValue);
}

/* <7a341> ../engine/pr_cmds.c:2810 */
void EXT_FUNC PF_WriteChar_I(int iValue)
{
	if (!gMsgStarted)
		Sys_Error("WRITE_CHAR called with no active message\n");
	MSG_WriteChar(&gMsgBuffer, iValue);
}

/* <7a36b> ../engine/pr_cmds.c:2818 */
void EXT_FUNC PF_WriteShort_I(int iValue)
{
	if (!gMsgStarted)
		Sys_Error("WRITE_SHORT called with no active message\n");
	MSG_WriteShort(&gMsgBuffer, iValue);
}

/* <7a395> ../engine/pr_cmds.c:2826 */
void EXT_FUNC PF_WriteLong_I(int iValue)
{
	if (!gMsgStarted)
		Sys_Error("PF_WriteLong_I called with no active message\n");
	MSG_WriteLong(&gMsgBuffer, iValue);
}

/* <7a3bf> ../engine/pr_cmds.c:2834 */
void EXT_FUNC PF_WriteAngle_I(float flValue)
{
	if (!gMsgStarted)
		Sys_Error("PF_WriteAngle_I called with no active message\n");
	MSG_WriteAngle(&gMsgBuffer, flValue);
}

/* <7a3e9> ../engine/pr_cmds.c:2842 */
void EXT_FUNC PF_WriteCoord_I(float flValue)
{
	if (!gMsgStarted)
		Sys_Error("PF_WriteCoord_I called with no active message\n");
	MSG_WriteShort(&gMsgBuffer, (int)(flValue * 8.0));
}

/* <7a413> ../engine/pr_cmds.c:2851 */
void EXT_FUNC PF_WriteString_I(const char *sz)
{
	if (!gMsgStarted)
		Sys_Error("PF_WriteString_I called with no active message\n");
	MSG_WriteString(&gMsgBuffer, sz);
}

/* <7a43c> ../engine/pr_cmds.c:2859 */
void EXT_FUNC PF_WriteEntity_I(int iValue)
{
	if (!gMsgStarted)
		Sys_Error("PF_WriteEntity_I called with no active message\n");
	MSG_WriteShort(&gMsgBuffer, iValue);
}

/* <7a466> ../engine/pr_cmds.c:2874 */
void EXT_FUNC PF_makestatic_I(edict_t *ent)
{
	MSG_WriteByte(&g_psv.signon, svc_spawnstatic);
	MSG_WriteShort(&g_psv.signon, SV_ModelIndex(&pr_strings[ent->v.model]));
	MSG_WriteByte(&g_psv.signon, ent->v.sequence);
	MSG_WriteByte(&g_psv.signon, (int)ent->v.frame);
	MSG_WriteWord(&g_psv.signon, ent->v.colormap);
	MSG_WriteByte(&g_psv.signon, ent->v.skin);

	for (int i = 0; i < 3; i++)
	{
		MSG_WriteCoord(&g_psv.signon, ent->v.origin[i]);
		MSG_WriteAngle(&g_psv.signon, ent->v.angles[i]);
	}

	MSG_WriteByte(&g_psv.signon, ent->v.rendermode);
	if (ent->v.rendermode)
	{
		MSG_WriteByte(&g_psv.signon, (int)ent->v.renderamt);
		MSG_WriteByte(&g_psv.signon, (int)ent->v.rendercolor[0]);
		MSG_WriteByte(&g_psv.signon, (int)ent->v.rendercolor[1]);
		MSG_WriteByte(&g_psv.signon, (int)ent->v.rendercolor[2]);
		MSG_WriteByte(&g_psv.signon, ent->v.renderfx);
	}

	ED_Free(ent);
}

/* <7a49e> ../engine/pr_cmds.c:2910 */
void EXT_FUNC PF_StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
	MSG_WriteByte(&g_psv.signon, svc_temp_entity);
	MSG_WriteByte(&g_psv.signon, TE_BSPDECAL);
	MSG_WriteCoord(&g_psv.signon, *origin);
	MSG_WriteCoord(&g_psv.signon, origin[1]);
	MSG_WriteCoord(&g_psv.signon, origin[2]);
	MSG_WriteShort(&g_psv.signon, decalIndex);
	MSG_WriteShort(&g_psv.signon, entityIndex);
	
	if (entityIndex)
		MSG_WriteShort(&g_psv.signon, modelIndex);
}

/* <7a4f5> ../engine/pr_cmds.c:2935 */
void EXT_FUNC PF_setspawnparms_I(edict_t *ent)
{
	int i = NUM_FOR_EDICT(ent);
	if (i < 1 || i > g_psvs.maxclients)
		Host_Error("Entity is not a client");
}

/* <7a539> ../engine/pr_cmds.c:2956 */
void EXT_FUNC PF_changelevel_I(const char *s1, const char *s2)
{
	static int last_spawncount;

	if (g_psvs.spawncount != last_spawncount)
	{
		last_spawncount = g_psvs.spawncount;
		SV_SkipUpdates();
		if (s2)
			Cbuf_AddText(va("changelevel2 %s %s\n", s1, s2));
		else
			Cbuf_AddText(va("changelevel %s\n", s1));
	}
}

/* <7a582> ../engine/pr_cmds.c:2988 */
void SeedRandomNumberGenerator(void)
{
	idum = -(int)CRehldsPlatformHolder::get()->time(NULL);
	if (idum > 1000)
	{
		idum = -idum;
	}
	else if (idum > -1000)
	{
		idum -= 22261048;
	}
}

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)

/* <7a598> ../engine/pr_cmds.c:3003 */
int32 ran1(void)
{
	int j;
	long k;
	static long iy = 0;
	static long iv[NTAB];

	if (idum <= 0 || !iy)
	{
		if (-(idum) < 1) idum = 1;
		else idum = -(idum);
		for (j = NTAB + 7; j >= 0; j--)
		{
			k = (idum) / IQ;
			idum = IA*(idum - k*IQ) - IR*k;
			if (idum < 0) idum += IM;
			if (j < NTAB) iv[j] = idum;
		}
		iy = iv[0];
	}
	k = (idum) / IQ;
	idum = IA*(idum - k*IQ) - IR*k;
	if (idum < 0) idum += IM;
	j = iy / NDIV;
	iy = iv[j];
	iv[j] = idum;

	return iy;
}

#define AM (1.0/IM)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

/* <7a605> ../engine/pr_cmds.c:3038 */
float fran1(void)
{
	float temp = (float)AM*ran1();
	if (temp > RNMX) return (float)RNMX;
	else return temp;
}

/* <7a644> ../engine/pr_cmds.c:3045 */
float EXT_FUNC RandomFloat(float flLow, float flHigh)
{
#ifndef SWDS
	g_engdstAddrs.pfnRandomFloat(&flLow, &flHigh);
#endif

	float fl = fran1(); // float in [0,1)
	return (fl * (flHigh - flLow)) + flLow; // float in [low,high)
}

/* <7a6b2> ../engine/pr_cmds.c:3056 */
int32 EXT_FUNC RandomLong(int32 lLow, int32 lHigh)
{
#ifndef SWDS
	g_engdstAddrs.pfnRandomLong(&lLow, &lHigh);
#endif

	unsigned long maxAcceptable;
	unsigned long x = lHigh - lLow + 1;
	unsigned long n;
	if (x <= 0 || MAX_RANDOM_RANGE < x - 1)
	{
		return lLow;
	}

	// The following maps a uniform distribution on the interval [0,MAX_RANDOM_RANGE]
	// to a smaller, client-specified range of [0,x-1] in a way that doesn't bias
	// the uniform distribution unfavorably. Even for a worst case x, the loop is
	// guaranteed to be taken no more than half the time, so for that worst case x,
	// the average number of times through the loop is 2. For cases where x is
	// much smaller than MAX_RANDOM_RANGE, the average number of times through the
	// loop is very close to 1.
	//
	maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE + 1) % x);
	do
	{
		n = ran1();
	} while (n > maxAcceptable);

	return lLow + (n % x);
}

/* <7a71b> ../engine/pr_cmds.c:3089 */
void EXT_FUNC PF_FadeVolume(const edict_t *clientent, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
	int entnum = NUM_FOR_EDICT(clientent);
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_FadeVolume a non-client\n");
		return;
	}

	client_t* client = &g_psvs.clients[entnum - 1];
	if (client->fakeclient)
		return;

	MSG_WriteChar(&client->netchan.message, svc_soundfade);
	MSG_WriteByte(&client->netchan.message, (uint8)fadePercent);
	MSG_WriteByte(&client->netchan.message, (uint8)holdTime);
	MSG_WriteByte(&client->netchan.message, (uint8)fadeOutSeconds);
	MSG_WriteByte(&client->netchan.message, (uint8)fadeInSeconds);
}

/* <7a7a1> ../engine/pr_cmds.c:3124 */
void EXT_FUNC PF_SetClientMaxspeed(edict_t *clientent, float fNewMaxspeed)
{
	int entnum = NUM_FOR_EDICT(clientent);
	if (entnum < 1 || entnum > g_psvs.maxclients)
		Con_Printf("tried to PF_SetClientMaxspeed a non-client\n");
	
	clientent->v.maxspeed = fNewMaxspeed;
}

/* <7a80b> ../engine/pr_cmds.c:3149 */
int EXT_FUNC PF_GetPlayerUserId(edict_t *e)
{
	if (!g_psv.active || !e)
		return -1;

	for (int i = 0; i < g_psvs.maxclients; i++) {
		if (g_psvs.clients[i].edict == e) {
			return g_psvs.clients[i].userid;
		}
	}

	return -1;
}

/* <7a854> ../engine/pr_cmds.c:3172 */
unsigned int EXT_FUNC PF_GetPlayerWONId(edict_t *e)
{
	return 0xFFFFFFFF;
}

/* <7a87f> ../engine/pr_cmds.c:3180 */
const char* EXT_FUNC PF_GetPlayerAuthId(edict_t *e)
{
	static char szAuthID[5][64];
	static int count = 0;
	client_t *cl;
	int i;

	count = (count + 1) % 5;
	szAuthID[count][0] = 0;

	if (!g_psv.active || e == NULL)
	{
		return szAuthID[count];
	}

	for (i = 0; i < g_psvs.maxclients; i++)
	{
		cl = &g_psvs.clients[i];
		if (cl->edict != e)
		{
			continue;
		}

		if (cl->fakeclient)
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
}

void EXT_FUNC PF_BuildSoundMsg_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	g_RehldsHookchains.m_PF_BuildSoundMsg_I.callChain(PF_BuildSoundMsg_I_internal, entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
}

/* <7a902> ../engine/pr_cmds.c:3229 */
void EXT_FUNC PF_BuildSoundMsg_I_internal(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	PF_MessageBegin_I(msg_dest, msg_type, pOrigin, ed);
	SV_BuildSoundMsg(entity, channel, sample, (int)volume, attenuation, fFlags, pitch, pOrigin, &gMsgBuffer);
	PF_MessageEnd_I();
}

/* <7a9c1> ../engine/pr_cmds.c:3240 */
int EXT_FUNC PF_IsDedicatedServer(void)
{
	return g_bIsDedicatedServer;
}

/* <7a9da> ../engine/pr_cmds.c:3245 */
const char* EXT_FUNC PF_GetPhysicsInfoString(const edict_t *pClient)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_GetPhysicsInfoString a non-client\n");
		return "";
	}

	client_t* client = &g_psvs.clients[entnum - 1];
	return client->physinfo;
}

/* <7aa28> ../engine/pr_cmds.c:3259 */
const char* EXT_FUNC PF_GetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_GetPhysicsKeyValue a non-client\n");
		return "";
	}

	client_t* client = &g_psvs.clients[entnum - 1];
	return Info_ValueForKey(client->physinfo, key);
}

/* <7aa85> ../engine/pr_cmds.c:3273 */
void EXT_FUNC PF_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if (entnum < 1 || entnum > g_psvs.maxclients)
		Con_Printf("tried to PF_SetPhysicsKeyValue a non-client\n");

	client_t* client = &g_psvs.clients[entnum - 1];
	Info_SetValueForKey(client->physinfo, key, value, MAX_INFO_STRING);
}

/* <7aaed> ../engine/pr_cmds.c:3287 */
int EXT_FUNC PF_GetCurrentPlayer(void)
{
	int idx = host_client - g_psvs.clients;
	if (idx < 0 || idx >= g_psvs.maxclients)
		return -1;

	return idx;
}

/* <7ab19> ../engine/pr_cmds.c:3296 */
int EXT_FUNC PF_CanSkipPlayer(const edict_t *pClient)
{
	int entnum = NUM_FOR_EDICT(pClient);
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_CanSkipPlayer a non-client\n");
		return 0;
	}

	client_t* client = &g_psvs.clients[entnum - 1];
	return client->lw != 0;
}

/* <7ab67> ../engine/pr_cmds.c:3313 */
void EXT_FUNC PF_SetGroupMask(int mask, int op)
{
	g_groupmask = mask;
	g_groupop = op;
}

/* <7ab9e> ../engine/pr_cmds.c:3319 */
int EXT_FUNC PF_CreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
	extra_baselines_t *bls = g_psv.instance_baselines;
	if (bls->number >= NUM_BASELINES)
		return 0;

	bls->classname[bls->number] = classname;
	Q_memcpy(&bls->baseline[bls->number], baseline, sizeof(struct entity_state_s));
	bls->number += 1;
	return bls->number;
}

/* <7abdb> ../engine/pr_cmds.c:3332 */
void EXT_FUNC PF_Cvar_DirectSet(struct cvar_s *var, const char *value)
{
	Cvar_DirectSet(var, value);
}

/* <7ac13> ../engine/pr_cmds.c:3337 */
void EXT_FUNC PF_ForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
	int i;

	if (!filename)
		Host_Error("PF_ForceUnmodified: NULL pointer");

	if (PR_IsEmptyString(filename))
		Host_Error("PF_ForceUnmodified: Bad string '%s'", filename);

	if (g_psv.state == ss_loading)
	{
		i = 0;
		consistency_t* cnode = g_psv.consistency_list;
		while (cnode->filename)
		{
			if (!Q_stricmp(cnode->filename, (char *)filename))
				return;

			++cnode;
			++i;

			if (i >= 512)
				Host_Error("ForceUnmodified: '%s' overflow", filename);
		}

		cnode->check_type = type;
		cnode->filename = (char *)filename;
		if (mins)
		{
			cnode->mins[0] = mins[0];
			cnode->mins[1] = mins[1];
			cnode->mins[2] = mins[2];
		}
		if (maxs)
		{
			cnode->maxs[0] = maxs[0];
			cnode->maxs[1] = maxs[1];
			cnode->maxs[2] = maxs[2];
		}
	}
	else
	{
		i = 0;
		consistency_t* cnode = g_psv.consistency_list;
		while (!cnode->filename || Q_stricmp(cnode->filename, filename))
		{
			++cnode;
			++i;
			if (i >= 512)
				Host_Error("ForceUnmodified: '%s' Precache can only be done in spawn functions", filename);
		}
	}
}

/* <7ac92> ../engine/pr_cmds.c:3386 */
void EXT_FUNC PF_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
	*packet_loss = 0;
	*ping = 0;
	int c = NUM_FOR_EDICT(pClient);
	if (c < 1 || c > g_psvs.maxclients)
	{
		Con_Printf("tried to PF_GetPlayerStats a non-client\n");
		return;
	}

	client_t* client = &g_psvs.clients[c - 1];
	*packet_loss = client->packet_loss;
	*ping = client->latency * 1000.0;
}

/* <7ad3a> ../engine/pr_cmds.c:3408 */
NOXREF void QueryClientCvarValueCmd(void)
{
	if (Cmd_Argc() <= 1)
	{
		Con_Printf("%s <player name> <cvar>\n", Cmd_Argv(0));
		return;
	}
	for (int i = 0; i < g_psvs.maxclients; i++)
	{
		client_t *cl = &g_psvs.clients[i];

		if (cl->active || cl->connected)
		{
			if (!Q_stricmp(cl->name, Cmd_Argv(1)))
			{
				QueryClientCvarValue(cl->edict, Cmd_Argv(2));
				break;
			}
		}
	}
}

/* <7adff> ../engine/pr_cmds.c:3433 */
NOXREF void QueryClientCvarValueCmd2(void)
{
	int i;
	client_t *cl;
	int requestID;

	if (Cmd_Argc() < 3)
	{
		Con_Printf("%s <player name> <cvar> <requestID>", Cmd_Argv(0));
		return;
	}
	requestID = Q_atoi(Cmd_Argv(3));
	for (i = 0; i < g_psvs.maxclients; i++)
	{
		cl = &g_psvs.clients[i];

		if (cl->active || cl->connected)
		{
			if (!Q_stricmp(cl->name, Cmd_Argv(1)))
			{
				QueryClientCvarValue2(cl->edict, Cmd_Argv(2), requestID);
				break;
			}
		}
	}
}

/* <7acfa> ../engine/pr_cmds.c:3461 */
void EXT_FUNC QueryClientCvarValue(const edict_t *player, const char *cvarName)
{

	int entnum = NUM_FOR_EDICT(player);
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		if (gNewDLLFunctions.pfnCvarValue)
			gNewDLLFunctions.pfnCvarValue(player, "Bad Player");

		Con_Printf("tried to QueryClientCvarValue a non-client\n");
		return;
	}
	client_t *client = &g_psvs.clients[entnum - 1];
	MSG_WriteChar(&client->netchan.message, svc_sendcvarvalue);
	MSG_WriteString(&client->netchan.message, cvarName);
}

/* <7adb3> ../engine/pr_cmds.c:3485 */
void EXT_FUNC QueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
	int entnum = NUM_FOR_EDICT(player);
	if (entnum < 1 || entnum > g_psvs.maxclients)
	{
		if (gNewDLLFunctions.pfnCvarValue2)
			gNewDLLFunctions.pfnCvarValue2(player, requestID, cvarName, "Bad Player");

		Con_Printf("tried to QueryClientCvarValue a non-client\n");
		return;
	}
	client_t *client = &g_psvs.clients[entnum - 1];
	MSG_WriteChar(&client->netchan.message, svc_sendcvarvalue2);
	MSG_WriteLong(&client->netchan.message, requestID);
	MSG_WriteString(&client->netchan.message, cvarName);
}

/* <7af41> ../engine/pr_cmds.c:3509 */
int hudCheckParm(char *parm, char **ppnext)
{
#ifndef SWDS
	g_engdstAddrs.CheckParm(&parm, &ppnext);
#endif

	int i = COM_CheckParm(parm);
	if (ppnext)
	{
		if (i && i < com_argc - 1)
			*ppnext = com_argv[i + 1];
		else
			*ppnext = 0;
	}
	return i;
}

/* <7afa4> ../engine/pr_cmds.c:3532 */
int EXT_FUNC EngCheckParm(const char *pchCmdLineToken, char **pchNextVal)
{
	return hudCheckParm((char*)pchCmdLineToken, pchNextVal);
}
