#include "system/precompiled.h"
#include "client/enginevgui.h"

IGameUI *gpGameUI = nullptr;
IGameConsole *gpGameConsole = nullptr;

static CEngineVGui gEngineVGui;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CEngineVGui, IEngineVGui, VENGINE_VGUI_VERSION, gEngineVGui);

IEngineVGuiInternal *EngineVGui()
{
	return &gEngineVGui;
}

CEngineVGui::CEngineVGui()
{
};

// Setup the base vgui panels
void CEngineVGui::Init()
{
	COM_TimestampedLog("Loading gameui dll");
	
	gpGameUI = (IGameUI*)fnLauncherFactory(GAMEUI_INTERFACE_VERSION, nullptr);
	
	if(!gpGameUI)
		return;
	
	gpGameConsole = (IGameConsole*)fnLauncherFactory(GAMECONSOLE_INTERFACE_VERSION, nullptr);
	
	if(!gpGameConsole)
		return;
	
	gpGameUI->Initialize();
	gpGameConsole->Initialize();
};

void CEngineVGui::Connect()
{
};

// Called to Shutdown the game UI system
void CEngineVGui::Shutdown()
{
	gpGameUI->Shutdown();
};

//-----------------------------------------------------------------------------
// add all the base search paths used by VGUI (platform, skins directory, language dirs)
//-----------------------------------------------------------------------------
bool CEngineVGui::SetVGUIDirectories()
{
	// add vgui skins directory last
#if defined(_WIN32)
	char temp[ 512 ];
	char skin[128];
	skin[0] = 0;
	Sys_GetRegKeyValue("Software\\Valve\\Steam", "Skin", skin, sizeof(skin), "");
	if (strlen(skin) > 0)
	{
		sprintf( temp, "%s/platform/skins/%s", GetBaseDirectory(), skin );
		g_pFileSystem->AddSearchPath( temp, "SKIN" );
	}
#endif

	return true;
}

//-----------------------------------------------------------------------------
// Are we initialized?
//-----------------------------------------------------------------------------
bool CEngineVGui::IsInitialized() const
{
	return staticPanel != NULL;
}

CreateInterfaceFn CEngineVGui::GetGameUIFactory()
{
	return m_GameUIFactory;
}

//-----------------------------------------------------------------------------
// Purpose: Returns 1 if the key event is handled, 0 if the engine should handle it
//-----------------------------------------------------------------------------
bool CEngineVGui::Key_Event( const InputEvent_t &event )
{
	bool bDown = event.m_nType != IE_ButtonReleased;
	ButtonCode_t code = (ButtonCode_t)event.m_nData;

	if ( IsPC() && IsShiftKeyDown() )
	{
		switch( code )
		{
		case KEY_F1:
			if ( bDown )
			{
				Cbuf_AddText( "debugsystemui" );
			}
			return true;

		case KEY_F2:
			if ( bDown )
			{
				Cbuf_AddText( "demoui" );
			}
			return true;
		}
	}

#if defined( _WIN32 )
	// Ignore alt tilde, since the Japanese IME uses this to toggle itself on/off
	if ( IsPC() && code == KEY_BACKQUOTE && ( IsAltKeyDown() || IsCtrlKeyDown() ) )
		return true;
#endif
			   
	// ESCAPE toggles game ui
	ButtonCode_t uiToggleKey = IsPC() ? KEY_ESCAPE : KEY_XBUTTON_START;
	if ( bDown && code == uiToggleKey )
	{
		if ( IsPC() )
		{
			if ( IsGameUIVisible()  )
			{
				// Don't allow hiding of the game ui if there's no level
				const char *pLevelName = engineClient->GetLevelName();
				if ( pLevelName && pLevelName[0] )
				{
					Cbuf_AddText( "gameui_hide" );
					if ( IsDebugSystemVisible() )
					{
						Cbuf_AddText( "debugsystemui 0" );
					}
				}
			}
			else
			{
				Cbuf_AddText( "gameui_activate" );
			}
			return true;
		}
		if ( IsX360() && !IsGameUIVisible() )
		{
			// 360 UI does not toggle, engine does "show", but UI needs to handle "hide"
			Cbuf_AddText( "gameui_activate" );
			return true;
		}
	}

	if ( g_pMatSystemSurface && g_pMatSystemSurface->HandleInputEvent( event ) )
	{
		// always let the engine handle the console keys
		// FIXME: Do a lookup of the key bound to toggleconsole
		// want to cache it off so the lookup happens only when keys are bound?
		if ( IsPC() && ( code == KEY_BACKQUOTE ) )
			return false;
		return true;
	}
	return false;
}

void CEngineVGui::BackwardCompatibility_Paint()
{
	Paint( (PaintMode_t)(PAINT_UIPANELS | PAINT_INGAMEPANELS) );
}

//-----------------------------------------------------------------------------
// Purpose: Returns 1 if the key event is handled, 0 if the engine should handle it
//-----------------------------------------------------------------------------
void CEngineVGui::UpdateButtonState( const InputEvent_t &event )
{
	m_pInputInternal->UpdateButtonState( event );
}

void CEngineVGui::PostInit()
{
	staticGameUIFuncs->PostInit();
#if defined( _X360 )
	g_pMatSystemSurface->ClearTemporaryFontCache();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: paints all the vgui elements
//-----------------------------------------------------------------------------
void CEngineVGui::Paint( PaintMode_t mode )
{
	VPROF_BUDGET( "CEngineVGui::Paint", VPROF_BUDGETGROUP_OTHER_VGUI );

	if ( !staticPanel )
		return;

	// setup the base panel to cover the screen
	vgui::VPANEL pVPanel = vgui::surface()->GetEmbeddedPanel();
	if ( !pVPanel )
		return;

	bool drawVgui = r_drawvgui.GetBool();

	// Don't draw the console at all if vgui is off during a time demo
	if ( demoplayer->IsPlayingTimeDemo() && !drawVgui )
	{
		return;
	}

	if ( !drawVgui || m_bNoShaderAPI )
	{
		return;
	}

	// Force engine's root panel (staticPanel) to be full screen size
	RECT rect;
	::GetClientRect(*pmainwindow, &rect);

	int w, h;
	w = rect.right;
	h = rect.bottom;

	// draw from the main panel down
	vgui::Panel *panel = staticPanel;
	panel->SetBounds(0, 0, w, h);
	panel->Repaint();

	toolframework->VGui_PreRenderAllTools( mode );

	// Paint both ( backward compatibility support )

	// It's either the full screen, or just the client .dll stuff
	if ( mode & PAINT_UIPANELS )
	{
		// Hide the client .dll, and paint everything else
		bool saveVisible = staticClientDLLPanel->IsVisible();
		bool saveToolsVisible = staticClientDLLToolsPanel->IsVisible();
		staticClientDLLPanel->SetVisible( false );
		staticClientDLLToolsPanel->SetVisible( false );

		vgui::surface()->PaintTraverseEx(pVPanel, true );

		staticClientDLLPanel->SetVisible( saveVisible );
		staticClientDLLToolsPanel->SetVisible( saveToolsVisible );
	}
	
	if ( mode & PAINT_INGAMEPANELS )
	{
		bool bSaveVisible = vgui::ipanel()->IsVisible( pVPanel );
		vgui::ipanel()->SetVisible( pVPanel, false );

		// Remove the client .dll from the main hierarchy so that popups will only paint for the client .dll here
		// NOTE: Disconnect each surface one at a time so that we don't draw popups twice

		// Paint the client .dll only
		vgui::VPANEL ingameRoot = staticClientDLLPanel->GetVPanel();
		vgui::VPANEL saveParent = vgui::ipanel()->GetParent( ingameRoot );
		vgui::ipanel()->SetParent( ingameRoot, 0 );
		vgui::surface()->PaintTraverseEx( ingameRoot, true );
		vgui::ipanel()->SetParent( ingameRoot, saveParent );

		// Overlay the client .dll tools next
		vgui::VPANEL ingameToolsRoot = staticClientDLLToolsPanel->GetVPanel();
		vgui::VPANEL saveToolParent = vgui::ipanel()->GetParent( ingameToolsRoot );
		vgui::ipanel()->SetParent( ingameToolsRoot, 0 );
		vgui::surface()->PaintTraverseEx( ingameToolsRoot, true );
		vgui::ipanel()->SetParent( ingameToolsRoot, saveToolParent );

		vgui::ipanel()->SetVisible( pVPanel, bSaveVisible );
	}

	toolframework->VGui_PostRenderAllTools( mode );
}

void CEngineVGui::SetGameUIActive(bool bActive)
{
	if(bActive)
		gpGameUI->ActivateGameUI();
	else
		gpGameUI->HideGameUI();
};

bool CEngineVGui::IsGameUIActive()
{
	//return gpGameUI && gpGameUI->IsGameUIActive();
	return false;
};

// Show/hide the console
void CEngineVGui::SetConsoleVisible(bool bVisible)
{
	if(gpGameConsole)
	{
		if(bVisible)
		{
			SetGameUIActive(true);
			
			gpGameConsole->Activate();
		}
		else
			gpGameConsole->Hide();
	};
};

// Returns true if the console is currently open
bool CEngineVGui::IsConsoleVisible()
{
	return gpGameConsole->IsConsoleVisible();
};

// Clear all text from the console
void CEngineVGui::ClearConsole()
{
	if(gpGameConsole)
		gpGameConsole->Clear();
};

void CEngineVGui::HideDebugSystem( void )
{
	if ( staticDebugSystemPanel )
	{
		staticDebugSystemPanel->SetVisible( false );
		SetEngineVisible( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: transition handler
//-----------------------------------------------------------------------------
void CEngineVGui::OnLevelLoadingStarted()
{
	if (!staticGameUIFuncs)
		return;

	ConVar *pSyncReportConVar = g_pCVar->FindVar( "fs_report_sync_opens" );
	if ( pSyncReportConVar )
	{
		// If convar is set to 2, suppress warnings during level load
		g_syncReportLevel = pSyncReportConVar->GetInt();
		if ( g_syncReportLevel > 1 )
		{
			pSyncReportConVar->SetValue( 0 );
		}
	}
	
	if ( IsX360() )
	{
		// TCR requirement, always!!!
		m_bShowProgressDialog = true;
	}

	// we've starting loading a level/connecting to a server
	staticGameUIFuncs->OnLevelLoadingStarted( m_bShowProgressDialog );

	// reset progress bar timers
	m_flLoadingStartTime = Plat_FloatTime();
	m_LoadingProgress.RemoveAll();
	m_eLastProgressPoint = PROGRESS_NONE;
	m_nLastProgressPointRepeatCount = 0;
	m_ProgressBias = 0;

	// choose which progress bar to use
	if (NET_IsMultiplayer())
	{
		// we're connecting
		g_pLoadingProgressDescriptions = g_RemoteConnectLoadingProgressDescriptions;
	}
	else
	{
		g_pLoadingProgressDescriptions = g_ListenServerLoadingProgressDescriptions;
	}

	if ( m_bShowProgressDialog )
	{
		ActivateGameUI();
	}

	m_bShowProgressDialog = false;
}

//-----------------------------------------------------------------------------
// Purpose: transition handler
//-----------------------------------------------------------------------------
void CEngineVGui::OnLevelLoadingFinished()
{
	if (!staticGameUIFuncs)
		return;

	staticGameUIFuncs->OnLevelLoadingFinished( gfExtendedError, gszDisconnectReason, gszExtendedDisconnectReason );
	m_eLastProgressPoint = PROGRESS_NONE;

	// clear any error message
	gfExtendedError = false;
	gszDisconnectReason[0] = 0;
	gszExtendedDisconnectReason[0] = 0;

#if defined(ENABLE_LOADING_PROGRESS_PROFILING)
	// display progress bar stats (for debugging/tuning progress bar)
	float flEndTime = (float)Plat_FloatTime();
	// add a finished entry
	LoadingProgressEntry_t &entry = m_LoadingProgress[m_LoadingProgress.AddToTail()];
	entry.flTime = flEndTime - m_flLoadingStartTime;
	entry.eProgress = PROGRESS_HIGHESTITEM;
	// dump the info
	Msg("Level load timings:\n");
	float flTotalTime = flEndTime - m_flLoadingStartTime;
	int nRepeatCount = 0;
	float flTimeTaken = 0.0f;
	float flFirstLoadProgressTime = 0.0f;
	for (int i = 0; i < m_LoadingProgress.Count() - 1; i++)
	{
		// keep track of time
		flTimeTaken += (float)m_LoadingProgress[i+1].flTime - m_LoadingProgress[i].flTime;

		// keep track of how often something is repeated
		if (m_LoadingProgress[i+1].eProgress == m_LoadingProgress[i].eProgress)
		{
			if (nRepeatCount == 0)
			{
				flFirstLoadProgressTime = m_LoadingProgress[i].flTime;
			}
			++nRepeatCount;
			continue;
		}

		// work out the time it took to do this
		if (nRepeatCount == 0)
		{
			flFirstLoadProgressTime = m_LoadingProgress[i].flTime;
		}

		int nPerc = (int)(100 * (flFirstLoadProgressTime / flTotalTime));
		int nTickPerc = (int)(100 * ((float)m_LoadingProgress[i].eProgress / (float)PROGRESS_HIGHESTITEM));
		
		// interpolated percentage is in between the real times and the most ticks
		int nInterpPerc = (nPerc + nTickPerc) / 2;
		Msg("\t%d\t%.3f\t\ttime: %d%%\t\tinterp: %d%%\t\trepeat: %d\n", m_LoadingProgress[i].eProgress, flTimeTaken, nPerc, nInterpPerc, nRepeatCount);

		// reset accumlated vars
		nRepeatCount = 0;
		flTimeTaken = 0.0f;
	}
#endif // ENABLE_LOADING_PROGRESS_PROFILING

	HideGameUI();

	// Restore convar setting after level load
	if ( g_syncReportLevel > 1 )
	{
		ConVar *pSyncReportConVar = g_pCVar->FindVar( "fs_report_sync_opens" );
		if ( pSyncReportConVar )
		{
			pSyncReportConVar->SetValue( g_syncReportLevel );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: notification
//-----------------------------------------------------------------------------
void CEngineVGui::NotifyOfServerConnect(const char *game, int IP, int connectionPort, int queryPort)
{
	if (!staticGameUIFuncs)
		return;

	staticGameUIFuncs->OnConnectToServer2(game, IP, connectionPort, queryPort);
}

//-----------------------------------------------------------------------------
// Purpose: notification
//-----------------------------------------------------------------------------
void CEngineVGui::NotifyOfServerDisconnect()
{
	if (!staticGameUIFuncs)
		return;

	staticGameUIFuncs->OnDisconnectFromServer( g_eSteamLoginFailure );
	g_eSteamLoginFailure = 0;
}

void CEngineVGui::EnabledProgressBarForNextLoad()
{
	m_bShowProgressDialog = true;
}

//-----------------------------------------------------------------------------
// Purpose: Updates progress
//-----------------------------------------------------------------------------
void CEngineVGui::UpdateProgressBar(LevelLoadingProgress_e progress)
{
	if (!staticGameUIFuncs)
		return;

	if ( !ThreadInMainThread() )
		return;

#if defined(ENABLE_LOADING_PROGRESS_PROFILING)
	// track the progress times, for debugging & tuning
	LoadingProgressEntry_t &entry = m_LoadingProgress[m_LoadingProgress.AddToTail()];
	entry.flTime = Plat_FloatTime() - m_flLoadingStartTime;
	entry.eProgress = progress;
#endif

	if (!g_pLoadingProgressDescriptions)
		return;

	// don't go backwards
	if (progress < m_eLastProgressPoint)
		return;

	// count progress repeats
	if (progress == m_eLastProgressPoint)
	{
		++m_nLastProgressPointRepeatCount;
	}
	else
	{
		m_nLastProgressPointRepeatCount = 0;
	}

	// construct a string describing it
	LoadingProgressDescription_t &desc = GetProgressDescription(progress);

	// calculate partial progress
	float flPerc = desc.nPercent / 100.0f;
	if ( desc.nRepeat > 1 && m_nLastProgressPointRepeatCount )
	{
		// cap the repeat count
		m_nLastProgressPointRepeatCount = min(m_nLastProgressPointRepeatCount, desc.nRepeat);

		// next progress point
		float flNextPerc = GetProgressDescription(progress + 1).nPercent / 100.0f;

		// move along partially towards the next tick
		flPerc += (flNextPerc - flPerc) * ((float)m_nLastProgressPointRepeatCount / desc.nRepeat);
	}

	// the bias allows the loading bar to have an optional reserved initial band
	// isolated from the normal progress descriptions
	flPerc = flPerc * ( 1.0f - m_ProgressBias ) + m_ProgressBias;

	if ( staticGameUIFuncs->UpdateProgressBar( flPerc, desc.pszDesc ) )
	{
		// re-render vgui on screen
		extern void V_RenderVGuiOnly();
		V_RenderVGuiOnly();
	}

	m_eLastProgressPoint = progress;
}

//-----------------------------------------------------------------------------
// Purpose: Updates progress
//-----------------------------------------------------------------------------
void CEngineVGui::UpdateCustomProgressBar( float progress, const wchar_t *desc )
{
	if (!staticGameUIFuncs)
		return;

	char ansi[1024];
	g_pVGuiLocalize->ConvertUnicodeToANSI( desc, ansi, sizeof( ansi ) );

	if ( staticGameUIFuncs->UpdateProgressBar( progress, ansi ) )
	{
		// re-render vgui on screen
		extern void V_RenderVGuiOnly();
		V_RenderVGuiOnly();
	}
}

void CEngineVGui::StartCustomProgress()
{
	if (!staticGameUIFuncs)
		return;

	// we've starting loading a level/connecting to a server
	staticGameUIFuncs->OnLevelLoadingStarted(true);
	m_bSaveProgress = staticGameUIFuncs->SetShowProgressText( true );
}

void CEngineVGui::FinishCustomProgress()
{
	if (!staticGameUIFuncs)
		return;

	staticGameUIFuncs->SetShowProgressText( m_bSaveProgress );
	staticGameUIFuncs->OnLevelLoadingFinished( false, "", "" );
}

//-----------------------------------------------------------------------------
// Purpose: transition handler
//-----------------------------------------------------------------------------
void CEngineVGui::ShowErrorMessage()
{
	if (!staticGameUIFuncs || !gfExtendedError)
		return;

	staticGameUIFuncs->OnLevelLoadingFinished( gfExtendedError, gszDisconnectReason, gszExtendedDisconnectReason );
	m_eLastProgressPoint = PROGRESS_NONE;

	// clear any error message
	gfExtendedError = false;
	gszDisconnectReason[0] = 0;
	gszExtendedDisconnectReason[0] = 0;

	HideGameUI();
}

//-----------------------------------------------------------------------------
// Should pause?
//-----------------------------------------------------------------------------
bool CEngineVGui::ShouldPause()
{
	if ( IsPC() )
		return bugreporter->ShouldPause() || perftools->ShouldPause();
	
	return false;
}

void CEngineVGui::SetGameDLLPanelsVisible( bool show )
{
	if ( !staticGameDLLPanel )
	{
		return;
	}

	staticGameDLLPanel->SetVisible( show );
}

void CEngineVGui::ShowNewGameDialog( int chapter )
{
	staticGameUIFuncs->ShowNewGameDialog( chapter );
}

void CEngineVGui::Simulate()
{
	toolframework->VGui_PreSimulateAllTools();

	if ( staticPanel )
	{
		VPROF_BUDGET( "CEngineVGui::Simulate", VPROF_BUDGETGROUP_OTHER_VGUI );

		// update vgui animations
		//!! currently this has to be done once per dll, because the anim controller object is in a lib;
		//!! need to make it globally pumped (gameUI.dll has it's own version of this)
		vgui::GetAnimationController()->UpdateAnimations( Sys_FloatTime() );

		RECT rect;
		::GetClientRect(*pmainwindow, &rect);

		int w, h;
		w = rect.right;
		h = rect.bottom;

		CMatRenderContextPtr pRenderContext( materials );

		pRenderContext->Viewport( 0, 0, w, h );
		
		gpGameUI->RunFrame();
		vgui::ivgui()->RunFrame();

		// Some debugging helpers
		DrawMouseFocus();
		VGui_UpdateDrawTreePanel();
		VGui_UpdateTextureListPanel();

		vgui::surface()->CalculateMouseVisible();
		VGui_ActivateMouse();
	}

	toolframework->VGui_PostSimulateAllTools();
};