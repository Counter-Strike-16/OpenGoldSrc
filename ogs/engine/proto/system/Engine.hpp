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

//#include <memory>
#include "system/IEngine.hpp"

extern IEngine *eng;

struct THostInfo
{
	float fFPS{0.0f};
	
	int nActivePlayers{0};
	int nMaxPlayers{0};
	
	char *sMap{nullptr};
};

class CCmdLine;
class CHost;
class CFileSystem;
class CFileSystemProvider;
class CGameLoaderHandler;
class CScreen;
class CInput;
class CSound;
class CNetwork;
class CConsole;
class CGameServer;

class CEngine : public IEngine // CHost/CCommon
{
public:
	CEngine() = default;
	virtual ~CEngine() = default;
	
	virtual bool Init(const TEngineLoadParams &aLoadParams); // Load
	virtual void Shutdown(); // Unload
	
	virtual int Frame();
	
	virtual void AddCommandText(const char *asText);
	
	virtual void GetHostInfo(THostInfo &aInfo);
private:
	// non-virtual function's of wrap for hooks a virtual
	// Only needed for HOOK_ENGINE
	
	bool Init_noVirt(const TEngineLoadParams &aLoadParams);
	void Shutdown_noVirt();
	
	int Frame_noVirt();
	
	void AddCommandText_noVirt(const char *asText);
	
	void GetHostInfo_noVirt(THostInfo &aInfo);
	
	void InitLocal();
	void InitCommands();
	
	bool FilterTime(float time);
	
	std::unique_ptr<CCmdLine> mpCmdLine;
	std::unique_ptr<CSystemNew> mpSystem;
	std::unique_ptr<CFileSystem> mpFileSystem;
	std::unique_ptr<CNetwork> mpNetwork;
	std::unique_ptr<CSound> mpSound;
	std::unique_ptr<CInput> mpInput;
	
	std::unique_ptr<CFileSystemProvider> mpFileSystemProvider;
	//std::unique_ptr<CHost> mpHost;
	std::unique_ptr<CConsole> mpConsole; // IConsole
	//std::unique_ptr<CGameServer> mpServer;
	std::unique_ptr<CScreen> mpScreen;
	std::unique_ptr<CGameLoaderHandler> mpGameLoaderHandler;
	
	//IGame *mpGame{nullptr};
	
	double rolling_fps{0.0f};
	
	double realtime{0.0f};  // without any filtering or bounding;
							// not bounded in any way, changed at
							// start of every frame, never reset
	double oldrealtime{0.0f}; // last frame run
	
	double host_frametime{0.0f};
	
	bool host_initialized{false}; // true if into command execution
	bool mbDedicated{false};
};