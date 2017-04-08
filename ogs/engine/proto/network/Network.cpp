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

#include "precompiled.hpp"
#include "network/Network.hpp"
#include "system/System.hpp"
#include "system/common.hpp"
#include "console/IConsole.hpp"

void CNetwork::Init()
{
	/*
#ifdef HOOK_ENGINE
	Cmd_AddCommand("maxplayers", (xcommand_t)GetOriginalFuncAddrOrDefault("MaxPlayers_f", (void *)MaxPlayers_f));
#else
	Cmd_AddCommand("maxplayers", MaxPlayers_f);
#endif // HOOK_ENGINE

	Cvar_RegisterVariable(&net_address);
	Cvar_RegisterVariable(&ipname);
	Cvar_RegisterVariable(&iphostport);
	Cvar_RegisterVariable(&hostport);
	Cvar_RegisterVariable(&defport);
	Cvar_RegisterVariable(&ip_clientport);
	Cvar_RegisterVariable(&clientport);
	Cvar_RegisterVariable(&clockwindow);
	Cvar_RegisterVariable(&multicastport);
	
#ifdef _WIN32
	Cvar_RegisterVariable(&ipx_hostport);
	Cvar_RegisterVariable(&ipx_clientport);
#endif // _WIN32
	
	Cvar_RegisterVariable(&fakelag);
	Cvar_RegisterVariable(&fakeloss);
	Cvar_RegisterVariable(&net_graph);
	Cvar_RegisterVariable(&net_graphwidth);
	Cvar_RegisterVariable(&net_scale);
	Cvar_RegisterVariable(&net_graphpos);

	if(COM_CheckParm("-netthread"))
		use_thread = true;

	if(COM_CheckParm("-netsleep"))
		net_sleepforever = 0;

#ifdef _WIN32
	if(COM_CheckParm("-noipx"))
		noipx = TRUE;
#endif // _WIN32

	if(COM_CheckParm("-noip"))
		noip = TRUE;

	int port = COM_CheckParm("-port");
	
	if(port)
		Cvar_SetValue("hostport", Q_atof(com_argv[port + 1]));

	int clockwindow_ = COM_CheckParm("-clockwindow");
	
	if(clockwindow_)
		Cvar_SetValue("clockwindow", Q_atof(com_argv[clockwindow_ + 1]));

	net_message.data = (byte *)&net_message_buffer;
	net_message.maxsize = sizeof(net_message_buffer);
	net_message.flags = 0;
	net_message.buffername = "net_message";

	in_message.data = (byte *)&in_message_buf;
	in_message.maxsize = sizeof(in_message_buf);
	in_message.flags = 0;
	in_message.buffername = "in_message";
	*/
	for(int i = 0; i < 3; ++i)
	{
		//g_pLagData[i].pPrev = &g_pLagData[i];
		//g_pLagData[i].pNext = &g_pLagData[i];
	};

	AllocateQueues();
	mpConsole->DevPrintf("Base networking initialized.\n");
};

void CNetwork::Shutdown()
{
	ThreadLock();

	//NET_ClearLaggedList(g_pLagData);
	//NET_ClearLaggedList(&g_pLagData[1]);

	ThreadUnlock();

	Config(false);
	
	FlushQueues();
};

void CNetwork::Update()
{
};

void CNetwork::Config(bool multiplayer)
{
	int i;
	static bool old_config;
	static bool bFirst = true;

	if(old_config == multiplayer)
		return;

	old_config = multiplayer;

	if(multiplayer)
	{
		//if(!noip)
			OpenIP();
		
#ifdef _WIN32
		//if(!noipx)
			OpenIPX();
#endif //_WIN32
		
		if(bFirst)
		{
			bFirst = false;
			GetLocalAddress();
		};
	}
	else
	{
#ifdef REHLDS_FIXES
		const int invalid_socket = INVALID_SOCKET;
#else
		const int invalid_socket = 0;
#endif // REHLDS_FIXES
		
		ThreadLock();
		
		for(i = 0; i < 3; ++i)
		{
			//if(ip_sockets[i] != invalid_socket)
			{
#ifdef _WIN32
				//CRehldsPlatformHolder::get()->closesocket(ip_sockets[i]);
#else
				SOCKET_CLOSE(ip_sockets[i]);
#endif //_WIN32
				
				//ip_sockets[i] = invalid_socket;
			};
			
#ifdef _WIN32

			//if(ipx_sockets[i] != invalid_socket)
			{
				//CRehldsPlatformHolder::get()->closesocket(ipx_sockets[i]);
				//ipx_sockets[i] = invalid_socket;
			};
#endif //_WIN32
		
		};
		
		ThreadUnlock();
	};

	net_configured = multiplayer ? 1 : 0;
};

int CNetwork::IsConfigured()
{
	return net_configured;
};

CNetServer *CNetwork::StartServer(int anPort)
{
	return nullptr;
};

CNetClient *CNetwork::StartClient()
{
	return nullptr;
};

void CNetwork::AllocateQueues()
{
	//net_messages_t *p = nullptr;
	
	for(int i = 0; i < NUM_MSG_QUEUES; ++i)
	{
		//p = (net_messages_t *)Mem_ZeroMalloc(sizeof(net_messages_t));
		//p->buffer = (unsigned char *)Mem_ZeroMalloc(MSG_QUEUE_SIZE);
		//p->preallocated = 1;
		//p->next = normalqueue;
		//normalqueue = p;
	};

	StartThread();
};

void CNetwork::FlushQueues()
{
	for(int i = 0; i < 3; ++i)
	{
		//net_messages_t *p = messages[i];
		
		//while(p)
		{
			//net_messages_t *n = p->next;
			//Mem_Free(p->buffer);
			//Mem_Free(p);
			//p = n;
		};

		//messages[i] = nullptr;
	};

	//net_messages_t *p = normalqueue;
	
	//while(p)
	{
		//net_messages_t *n = p->next;
		//Mem_Free(p->buffer);
		//Mem_Free(p);
		//p = n;
	};
	
	//normalqueue = nullptr;
};

void CNetwork::StartThread()
{
	if(use_thread)
	{
		if(!net_thread_initialized)
		{
			net_thread_initialized = true;
			CSystem::Error("-net_thread is not reversed yet");
			
#ifdef _WIN32
			InitializeCriticalSection(&net_cs);
			//hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)NET_ThreadMain, 0, 0, &ThreadId);
			
			//if(!hThread)
			{
				DeleteCriticalSection(&net_cs);
				net_thread_initialized = false;
				use_thread = false;
				CSystem::Error("Couldn't initialize network thread, run without -net_thread\n");
			};
#endif // _WIN32
		
		};
	};
};

void CNetwork::StopThread()
{
	if(use_thread)
	{
		if(net_thread_initialized)
		{
		
#ifdef _WIN32
			//TerminateThread(hThread, 0);
			DeleteCriticalSection(&net_cs);
#endif // _WIN32
			
			net_thread_initialized = false;
			CSystem::Error("-net_thread is not reversed yet");
		};
	};
};

void CNetwork::ThreadLock()
{
#ifdef _WIN32
	if(use_thread && net_thread_initialized)
		EnterCriticalSection(&net_cs);
#endif // _WIN32
};

void CNetwork::ThreadUnlock()
{
#ifdef _WIN32
	if(use_thread && net_thread_initialized)
		LeaveCriticalSection(&net_cs);
#endif // _WIN32
};

void CNetwork::OpenIP()
{
	/*
	// cvar_t *ip; //unused?
	int port;
	int dedicated;
	int sv_port = 0;
	int cl_port = 0;
	// int mc_port; //unused?
	static qboolean bFirst = TRUE;

	port = 0;
	//dedicated = cls.state == ca_dedicated;

	ThreadLock();

#ifdef REHLDS_FIXES
	if(ip_sockets[NS_SERVER] == INVALID_SOCKET)
#else
	if(!ip_sockets[NS_SERVER])
#endif
	{
		port = (int)iphostport.value;

		if(!port)
		{
			port = (int)hostport.value;
			
			if(!port)
			{
				port = (int)defport.value;
				hostport.value = defport.value;
			};
		};
		
		ip_sockets[NS_SERVER] = NET_IPSocket(ipname.string, port, FALSE);

#ifdef REHLDS_FIXES
		if(ip_sockets[NS_SERVER] == INVALID_SOCKET && dedicated)
#else
		if(!ip_sockets[NS_SERVER] && dedicated)
#endif
		{
			CSystem::Error("Couldn't allocate dedicated server IP port %d.", port);
		};
		
		sv_port = port;
	};
	
	ThreadUnlock();

	if(dedicated)
		return;

	ThreadLock();

#ifdef REHLDS_FIXES
	if(ip_sockets[NS_CLIENT] == INVALID_SOCKET)
#else
	if(!ip_sockets[NS_CLIENT])
#endif
	{
		port = (int)ip_clientport.value;

		if(!port)
		{
			port = (int)clientport.value;
			if(!port)
				port = -1;
		}
		ip_sockets[NS_CLIENT] = NET_IPSocket(ipname.string, port, FALSE);
#ifdef REHLDS_FIXES
		if(ip_sockets[NS_CLIENT] == INVALID_SOCKET)
#else
		if(!ip_sockets[NS_CLIENT])
#endif
			ip_sockets[NS_CLIENT] = NET_IPSocket(ipname.string, -1, FALSE);
		cl_port = port;
	}
#ifdef REHLDS_FIXES
	if(ip_sockets[NS_MULTICAST] == INVALID_SOCKET)
#else
	if(!ip_sockets[NS_MULTICAST])
#endif
	{
		ip_sockets[NS_MULTICAST] = NET_IPSocket(ipname.string, multicastport.value, TRUE);

#ifdef REHLDS_FIXES
		if(ip_sockets[NS_MULTICAST] == INVALID_SOCKET && !dedicated)
#else
		if(!ip_sockets[NS_MULTICAST] && !dedicated)
#endif
			mpConsole->Printf("Warning! Couldn't allocate multicast IP port.\n");
	};
	
	ThreadUnlock();

	if(bFirst)
	{
		bFirst = FALSE;
		mpConsole->Printf("NET Ports:  server %i, client %i\n", sv_port, cl_port);
	};
	*/
};

#ifdef _WIN32

void CNetwork::OpenIPX()
{
	/*
	int port;
	
	int dedicated = 0; //cls.state == ca_dedicated;
	
	ThreadLock();
	
	if(!ipx_sockets[NS_SERVER])
	{
		port = ipx_hostport.value;
		
		if(!port)
		{
			port = hostport.value;
			
			if(!port)
			{
				hostport.value = defport.value;
				port = defport.value;
			};
		};
		
		ipx_sockets[NS_SERVER] = NET_IPXSocket(port);
	};
	
	ThreadUnlock();
	
	if(!dedicated)
	{
		ThreadLock();
		
		if(!ipx_sockets[NS_CLIENT])
		{
			port = ipx_clientport.value;
			
			if(!port)
			{
				port = clientport.value;
				if(!port)
					port = -1;
			};
			
			ipx_sockets[NS_CLIENT] = NET_IPXSocket(port);

			if(!ipx_sockets[NS_CLIENT])
				ipx_sockets[NS_CLIENT] = NET_IPXSocket(-1);
		};
		
		ThreadUnlock();
	};
	*/
};

#endif // _WIN32

void CNetwork::GetLocalAddress()
{
	/*
	char buff[512];
	struct sockaddr_in address;
	int namelen;
	int net_error;

	Q_memset(&net_local_adr, 0, sizeof(netadr_t));

#ifdef _WIN32
	Q_memset(&net_local_ipx_adr, 0, sizeof(netadr_t));
#endif // _WIN32

	if(noip)
		mpConsole->Printf("TCP/IP Disabled.\n");
	else
	{
		if(Q_strcmp(ipname.string, "localhost"))
			Q_strncpy(buff, ipname.string, ARRAYSIZE(buff) - 1);
		else
		{
#ifdef _WIN32
			//CRehldsPlatformHolder::get()->gethostname(buff, ARRAYSIZE(buff));
#else
			gethostname(buff, ARRAYSIZE(buff));
#endif // _WIN32
		}

		buff[ARRAYSIZE(buff) - 1] = 0;

#ifdef REHLDS_FIXES
		// check if address is valid
		if(NET_StringToAdr(buff, &net_local_adr))
		{
#else
		NET_StringToAdr(buff, &net_local_adr);
#endif
			namelen = sizeof(address);
#ifdef _WIN32
			//if(CRehldsPlatformHolder::get()->getsockname((SOCKET)ip_sockets[NS_SERVER], (struct sockaddr *)&address, (socklen_t *)&namelen) == SOCKET_ERROR)
#else
		if(getsockname((SOCKET)ip_sockets[NS_SERVER], (struct sockaddr *)&address, (socklen_t *)&namelen) == SOCKET_ERROR)
#endif // _WIN32
			{
				noip = TRUE;
				//net_error = NET_GetLastError();

				//mpConsole->Printf("Could not get TCP/IP address, TCP/IP disabled\nReason:  %s\n", NET_ErrorString(net_error));
			}
			//else
			{
				//net_local_adr.port = address.sin_port;
				//mpConsole->Printf("Server IP address %s\n", NET_AdrToString(net_local_adr));
				//Cvar_Set("net_address", va(NET_AdrToString(net_local_adr)));
			};

#ifdef REHLDS_FIXES
		}
		else
			mpConsole->Printf("Could not get TCP/IP address, Invalid hostname '%s'\n", buff);
#endif
	};

#ifdef _WIN32
	if(noipx)
		mpConsole->Printf("No IPX Support.\n");
	else
	{
		namelen = 14;
		//if(CRehldsPlatformHolder::get()->getsockname(ipx_sockets[NS_SERVER], (struct sockaddr *)&address, (socklen_t *)&namelen) == SOCKET_ERROR)
		{
			noipx = TRUE;
			//net_error = NET_GetLastError();
		}
		//else
		{
			//SockadrToNetadr((struct sockaddr *)&address, &net_local_ipx_adr);
			//mpConsole->Printf("Server IPX address %s\n", NET_AdrToString(net_local_ipx_adr));
		};
	};
#endif //_WIN32
*/
};