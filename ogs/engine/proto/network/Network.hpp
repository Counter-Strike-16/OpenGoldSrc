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

/// @file

#pragma once

struct IConsole;
class CNetServer;
class CNetClient;

class CNetwork
{
public:
	CNetwork(IConsole *apConsole) : mpConsole(apConsole){}

	void Init();
	void Shutdown();
	
	void Update();
	
	void Config(bool multiplayer);
	int IsConfigured();
	
	CNetServer *StartServer(int anPort);
	CNetClient *StartClient();
private:
	void AllocateQueues();
	void FlushQueues();
	
	void StartThread();
	void StopThread();
	
	void ThreadLock();
	void ThreadUnlock();
	
	void OpenIP();
	
#ifdef _WIN32
	void OpenIPX();
#endif
	
	void GetLocalAddress();
	
#ifdef _WIN32
	CRITICAL_SECTION net_cs;
#endif // _WIN32
	
	IConsole *mpConsole{nullptr};
	
	bool use_thread{false};
	bool net_thread_initialized{false};
	
	int net_configured{0};
};