#ifndef IGAMEUI_H
#define IGAMEUI_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include "tier1/interface.h"

class IGameUI : public IBaseInterface
{
public:
	virtual void Initialize(CreateInterfaceFn *factories, int count) = 0;
	virtual void Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion, void *system) = 0;
	virtual void Shutdown() = 0;
	
	virtual int ActivateGameUI() = 0;
	virtual int ActivateDemoUI() = 0;
	
	virtual int HasExclusiveInput() = 0;
	
	virtual void RunFrame() = 0;
	
	virtual void ConnectToServer(const char *game, int IP, int port) = 0;
	virtual void DisconnectFromServer() = 0;
	
	virtual void HideGameUI() = 0;
	virtual bool IsGameUIActive() = 0;
	
	virtual void LoadingStarted(const char *resourceType, const char *resourceName) = 0;
	virtual void LoadingFinished(const char *resourceType, const char *resourceName) = 0;
	
	virtual void StartProgressBar(const char *progressType, int progressSteps) = 0;
	virtual int ContinueProgressBar(int progressPoint, float progressFraction) = 0;
	virtual void StopProgressBar(bool bError, const char *failureReason, const char *extendedReason = NULL) = 0;
	
	virtual int SetProgressBarStatusText(const char *statusText) = 0;
	
	virtual void SetSecondaryProgressBar(float progress) = 0;
	virtual void SetSecondaryProgressBarText(const char *statusText) = 0;
};

extern IGameUI* g_pGameUI;

#define GAMEUI_INTERFACE_VERSION "GameUI007"

/*
class IGameUI007 : public IBaseInterface
{
public:
	// virtual destructor from IBaseInterface

	// 0x04
	// Phil: Implementation detail: actual factorys are from unknown,
	//       vgui2.dll, filesystem.dll, gameui.dll, client.dll
	virtual bool Init( CreateInterfaceFn *factoryList, int numFactories ) = 0;

	// 0x08
	virtual void Start( struct cl_enginefuncs_s *enginefuncs, int interfaceVersion, IAppSystem *system ) = 0;

	// 0x0c
	virtual void Shutdown() = 0;

	// 0x10
	// Assuming this is the same as HL2 version...
	// Activates the menus, returns 0 if it doesn't want to handle it
	virtual int ActivateGameUI() = 0;

	// 0x14
	virtual void ActivateDemoUI() = 0;

	// 0x18
	virtual bool HasExclusiveInput() = 0;

	// 0x1c
	// Assuming this is the same as HL2 version...
	virtual void RunFrame() = 0;

	// 0x20
	virtual void ConnectToServer( const char *game, int IP, int port ) = 0;

	// 0x24
	virtual void DisconnectFromServer() = 0;

	// 0x28
	// Assuming this is the same as HL2 version...
	virtual void HideGameUI() = 0;

	// 0x2c
	// Assuming this is the same as HL2 version...
	virtual bool IsGameUIActive() = 0;

	// 0x30
	virtual void LoadingStarted( const char *resourceType, const char *resourceName ) = 0;

	// 0x34
	virtual void LoadingFinished( const char *resourceType, const char *resourceName ) = 0;

	// 0x38
	virtual void StartProgressBar( const char *progressType, int numProgressPoints ) = 0;

	// 0x3c
	virtual int ContinueProgressBar( int progressPoint, float progressFraction ) = 0;

	// 0x40
	virtual void StopProgressBar( bool bError, const char *failureReasonIfAny, const char *extendedReason ) = 0;

	// 0x44
	virtual int SetProgressBarStatusText( const char *statusText ) = 0;

	// 0x48
	virtual void Unknown1( void *u1 ) = 0;

	// 0x4c
	// Maybe progress bar image/time left?
	virtual void Unknown2( void *u1 ) = 0;

	// 0x50
	virtual void Unknown3( void *u1, void *u2 ) = 0;

	// 0x54
	virtual void Unknown4( void *u1, void *u2 ) = 0;
};
*/

#endif // IGAMEUI_H