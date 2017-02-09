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
/// @brief builds an intended movement command to send to the server

//#include "precompiled.hpp"
#include "client/client.hpp"
#include "client/frame.hpp"
#include "client/clientdll.hpp"
#include "network/net_msg.hpp"
#include "console/cvar.hpp"

cvar_t cl_nodelta = { "cl_nodelta", "0" };

/*
================
CL_BaseMove initially
TODO: remove

Send the intended movement message to the server
================
*/
void CL_CreateMove(usercmd_t *cmd)
{
	// QW

	// VectorCopy (cl.viewangles, cmd->angles);

	// GS

	//gEngfuncs.GetViewAngles((float *)viewangles);

	//CL_AdjustAngles(frametime, viewangles);

	//gEngfuncs.SetViewAngles((float *)viewangles);
}

int MakeChar(int i)
{
	i &= ~3;
	if(i < -127 * 4)
		i = -127 * 4;
	if(i > 127 * 4)
		i = 127 * 4;
	return i;
}

/*
==============
CL_FinishMove
==============
*/
void CL_FinishMove(usercmd_t *cmd)
{
	int i;
	int ms;

	//
	// allways dump the first two message, because it may contain leftover inputs
	// from the last level
	//
	//if(++cl.movemessages <= 2)
		//return;
	//
	// figure button bits
	//
	//if(in_attack.state & 3)
		//cmd->buttons |= 1;
	//in_attack.state &= ~2;

	//if(in_jump.state & 3)
		//cmd->buttons |= 2;
	//in_jump.state &= ~2;

	// send milliseconds of time to apply the move
	//ms = host_frametime * 1000;
	if(ms > 250)
		ms = 100; // time was unreasonable
	cmd->msec = ms;

	//VectorCopy(cl.viewangles, cmd->angles);

	//cmd->impulse = in_impulse;
	//in_impulse = 0;

	//
	// chop down so no extra bits are kept that the server wouldn't get
	//
	cmd->forwardmove = MakeChar(cmd->forwardmove);
	cmd->sidemove = MakeChar(cmd->sidemove);
	cmd->upmove = MakeChar(cmd->upmove);

	//for(i = 0; i < 3; i++)
		//cmd->angles[i] =((int)(cmd->angles[i] * 65536.0 / 360) & 65535) * (360.0 / 65536.0);
}

/*
=================
CL_SendCmd
=================
*/
void CL_SendCmd()
{
	sizebuf_t buf;
	byte data[128];
	int i;
	usercmd_t *cmd, *oldcmd;
	int checksumIndex;
	int lost;
	int seq_hash;

	if(cls.demoplayback)
		return; // sendcmds come from the demo

	// save this command off for prediction
	//i = cls.netchan.outgoing_sequence & UPDATE_MASK;

	//cmd = &cl.frames[i].cmd;

	//cl.frames[i].senttime = realtime;
	cl.frames[i].receivedtime = -1; // we haven't gotten a reply yet

	//	seq_hash = (cls.netchan.outgoing_sequence & 0xffff) ; // ^
	//QW_CHECK_HASH;
	seq_hash = cls.netchan.outgoing_sequence;

	// get basic movement from keyboard
	//ClientDLL_CreateMove(host_frametime, cmd, IDUNNO); // cl.active?

	// allow mice or other external controllers to add to the move
	//IN_Move(cmd);

	// if we are spectator, try autocam
	//if(cl.spectator)
		//Cam_Track(cmd);

	CL_FinishMove(cmd);

	//Cam_FinishMove(cmd);

	// send this and the previous cmds in the message, so
	// if the last packet was dropped, it can be recovered
	buf.maxsize = 128;
	buf.cursize = 0;
	buf.data = data;

	MSG_WriteByte(&buf, clc_move);

	// save the position for a checksum byte
	checksumIndex = buf.cursize;
	MSG_WriteByte(&buf, 0);

	// write our lossage percentage
	//lost = CL_CalcNet();
	MSG_WriteByte(&buf, (byte)lost);

	//i = (cls.netchan.outgoing_sequence - 2) & UPDATE_MASK;
	//cmd = &cl.frames[i].cmd;
	//MSG_WriteDeltaUsercmd(&buf, &nullcmd, cmd);
	oldcmd = cmd;

	//i = (cls.netchan.outgoing_sequence - 1) & UPDATE_MASK;
	//cmd = &cl.frames[i].cmd;
	//MSG_WriteDeltaUsercmd(&buf, oldcmd, cmd);
	oldcmd = cmd;

	//i = (cls.netchan.outgoing_sequence) & UPDATE_MASK;
	//cmd = &cl.frames[i].cmd;
	//MSG_WriteDeltaUsercmd(&buf, oldcmd, cmd);

	// calculate a checksum over the move commands
	buf.data[checksumIndex] = COM_BlockSequenceCRCByte(
	buf.data + checksumIndex + 1, buf.cursize - checksumIndex - 1, seq_hash);

	// request delta compression of entities
	//if(cls.netchan.outgoing_sequence - cl.validsequence >= UPDATE_BACKUP - 1)
		//cl.validsequence = 0;

	if(cl.validsequence && !cl_nodelta.value && cls.state == ca_active &&
	   !cls.demorecording)
	{
		//cl.frames[cls.netchan.outgoing_sequence & UPDATE_MASK].delta_sequence =
		cl.validsequence;
		MSG_WriteByte(&buf, clc_delta);
		MSG_WriteByte(&buf, cl.validsequence & 255);
	}
	//else
		//cl.frames[cls.netchan.outgoing_sequence & UPDATE_MASK].delta_sequence = -1;

	//if(cls.demorecording)
		//CL_WriteDemoCmd(cmd);

	//
	// deliver the message
	//
	Netchan_Transmit(&cls.netchan, buf.cursize, buf.data);
}

/*
============
CL_InitInput
============
*/
void CL_InitInput()
{
	//ClientDLL_InitInput();

	Cvar_RegisterVariable(&cl_nodelta);
}

/*
============
CL_ClearStates
============
*/
void CL_ClearStates()
{
}