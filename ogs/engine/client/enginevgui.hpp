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
/// @brief Internal implementation of engine vgui interface - works with gameui and gameconsole interfaces

#pragma once

#include <cstddef>
#include "common/commontypes.h"
#include "public/archtypes.h"
#include "IEngineVGui.h"

/// Enumeration of level loading progress bar spots
enum eLevelLoadingProgress
{
	PROGRESS_NONE = 0,
	PROGRESS_CHANGELEVEL,               /// Server is changing level
	PROGRESS_SPAWNSERVER,               ///
	PROGRESS_LOADWORLDMODEL,            ///
	PROGRESS_CRCMAP,                    ///
	PROGRESS_CRCCLIENTDLL,              ///
	PROGRESS_CREATENETWORKSTRINGTABLES, ///
	PROGRESS_PRECACHEWORLD,             ///
	PROGRESS_CLEARWORLD,                ///
	PROGRESS_LEVELINIT,                 ///
	PROGRESS_PRECACHE,                  ///< Precaching resources...
	PROGRESS_ACTIVATESERVER,            ///
	PROGRESS_BEGINCONNECT,              ///< Connecting to server?
	PROGRESS_SIGNONCHALLENGE,           ///
	PROGRESS_SIGNONCONNECT,             ///
	PROGRESS_SIGNONCONNECTED,           ///
	PROGRESS_PROCESSSERVERINFO,         ///
	PROGRESS_PROCESSSTRINGTABLE,        ///
	PROGRESS_SIGNONNEW,                 ///
	PROGRESS_SENDCLIENTINFO,            ///< Sending client data...
	PROGRESS_SENDSIGNONDATA,            ///
	PROGRESS_SIGNONSPAWN,               ///
	PROGRESS_FULLYCONNECTED,            ///
	PROGRESS_READYTOPLAY,               ///

	PROGRESS_HIGHESTITEM, ///< Must be last item in list
};

/// Centerpoint for handling all user interface in the engine
class IEngineVGuiInternal : public IEngineVGui
{
public:
	virtual void Init()     = 0;
	virtual void Connect()  = 0;
	virtual void Shutdown() = 0;

	virtual bool SetVGUIDirectories() = 0;

	virtual bool IsInitialized() const = 0;

	virtual CreateInterfaceFn GetGameUIFactory() = 0;

	virtual bool Key_Event(const InputEvent_t &event) = 0;

	virtual void BackwardCompatibility_Paint() = 0;

	virtual void UpdateButtonState(const InputEvent_t &event) = 0;

	virtual void PostInit() = 0;

	virtual void Paint(PaintMode_t mode) = 0;

	// handlers for game UI (main menu)
	virtual void ActivateGameUI()  = 0;
	virtual bool HideGameUI()      = 0;
	virtual bool IsGameUIVisible() = 0;

	// console
	virtual void SetConsoleVisible(bool bVisible) = 0;
	virtual bool IsConsoleVisible()               = 0;
	virtual void ClearConsole()                   = 0;

	virtual void HideDebugSystem() = 0;

	// level loading
	virtual void OnLevelLoadingStarted()  = 0;
	virtual void OnLevelLoadingFinished() = 0;

	virtual void NotifyOfServerConnect(const char *game, int IP, int connectionPort, int queryPort) = 0;
	virtual void NotifyOfServerDisconnect() = 0;

	virtual void EnabledProgressBarForNextLoad() = 0;

	virtual void UpdateProgressBar(eLevelLoadingProgress progress) = 0;
	virtual void UpdateCustomProgressBar(float progress, const wchar_t *desc) = 0;

	virtual void StartCustomProgress()  = 0;
	virtual void FinishCustomProgress() = 0;

	virtual void ShowErrorMessage() = 0;

	// Should pause?
	virtual bool ShouldPause()                      = 0;
	virtual void SetGameDLLPanelsVisible(bool show) = 0;
	virtual void ShowNewGameDialog(int chapter)     = 0;

	virtual void Simulate() = 0;

	virtual void SetNotAllowedToHideGameUI(bool bNotAllowedToHide) = 0;
	virtual void SetNotAllowedToShowGameUI(bool bNotAllowedToShow) = 0;

	// Xbox 360
	virtual void SessionNotification(const int notification, const int param = 0) = 0;
	virtual void SystemNotification(const int notification) = 0;
	virtual void ShowMessageDialog(const uint nType, vgui::Panel *pOwner = NULL) = 0;
	virtual void UpdatePlayerInfo(uint64 nPlayerId, const char *pName, int nTeam, byte cVoiceState, int nPlayersNeeded, bool bHost) = 0;
	virtual void SessionSearchResult(int searchIdx, void *pHostData, XSESSION_SEARCHRESULT *pResult, int ping) = 0;
	virtual void OnCreditsFinished(void) = 0;

	// Storage device validation:
	//		returns true right away if storage device has been previously selected.
	//		otherwise returns false and will set the variable pointed by pStorageDeviceValidated to 1
	//				  once the storage device is selected by user.
	virtual bool ValidateStorageDevice(int *pStorageDeviceValidated) = 0;
};

/// Singleton accessor
extern IEngineVGuiInternal *EngineVGui();

/// Centerpoint for handling all user interface in the engine
class CEngineVGui : public IEngineVGuiInternal
{
public:
	CEngineVGui();
	~CEngineVGui() = default;

	VPANEL GetPanel(VGUIPANEL type);

	void Init();
	void Connect();
	void Shutdown();

	bool SetVGUIDirectories();

	bool IsInitialized() const;

	CreateInterfaceFn GetGameUIFactory();

	bool Key_Event(const InputEvent_t &event);

	void BackwardCompatibility_Paint();

	void UpdateButtonState(const InputEvent_t &event);

	void PostInit();

	void Paint(PaintMode_t mode);

	// handlers for game UI (main menu)
	//void ActivateGameUI();
	//bool HideGameUI();
	void SetGameUIActive(bool bActive);
	bool IsGameUIVisible();

	// console
	void SetConsoleVisible(bool bVisible);
	bool IsConsoleVisible();
	void ClearConsole();

	void HideDebugSystem();

	// level loading
	void OnLevelLoadingStarted();
	void OnLevelLoadingFinished();

	void NotifyOfServerConnect(const char *game, int IP, int connectionPort, int queryPort);
	void NotifyOfServerDisconnect();

	void EnabledProgressBarForNextLoad();

	void UpdateProgressBar(eLevelLoadingProgress progress);
	void UpdateCustomProgressBar(float progress, const wchar_t *desc);

	void StartCustomProgress();
	void FinishCustomProgress();

	void ShowErrorMessage();

	// Should pause?
	bool ShouldPause();

	void SetGameDLLPanelsVisible(bool show);
	void ShowNewGameDialog(int chapter);

	void Simulate();

	void SetNotAllowedToHideGameUI(bool bNotAllowedToHide);
	void SetNotAllowedToShowGameUI(bool bNotAllowedToShow);

	// Xbox 360
	void SessionNotification(const int notification, const int param = 0);
	void SystemNotification(const int notification);
	void ShowMessageDialog(const uint nType, vgui::Panel *pOwner = NULL);
	void UpdatePlayerInfo(uint64 nPlayerId, const char *pName, int nTeam, byte cVoiceState, int nPlayersNeeded, bool bHost);
	void SessionSearchResult(int searchIdx, void *pHostData, XSESSION_SEARCHRESULT *pResult, int ping);
	void OnCreditsFinished(void);

	// Storage device validation:
	//		returns true right away if storage device has been previously selected.
	//		otherwise returns false and will set the variable pointed by pStorageDeviceValidated to 1
	//				  once the storage device is selected by user.
	bool ValidateStorageDevice(int *pStorageDeviceValidated);
};