#include <KeyValues.h>

#include <vgui/ILocalize.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Controls.h>

#include "CvarSlider.h"
#include "CvarToggleCheckButton.h"
#include "EngineInterface.h"
#include "GameUI_Interface.h"
#include "igameuifuncs.h"
#include "ModInfo.h"
#include "OptionsSubVideo.h"

//TODO: add software mode (windows only) - Solokiller

COptionsSubVideo::COptionsSubVideo( vgui2::Panel* parent )
	: BaseClass( parent, nullptr )
{
	memset( &m_OrigSettings, 0, sizeof( m_OrigSettings ) );
	memset( &m_CurrentSettings, 0, sizeof( m_CurrentSettings ) );

	m_pBrightnessSlider = new CCvarSlider( this, "Brightness", "#GameUI_Brightness", 0.0, 2.0, "brightness", false );
	m_pGammaSlider = new CCvarSlider( this, "Gamma", "#GameUI_Gamma", 1.0, 3.0, "gamma", false );
	
	GetVidSettings();

	m_pMode = new vgui2::ComboBox( this, "Resolution", 6, false );
	m_pAspectRatio = new vgui2::ComboBox( this, "AspectRatio", 2, false );
	m_pDetailTextures = new CCvarToggleCheckButton( this, "DetailTextures", "#GameUI_DetailTextures", "r_detailtextures" );
	m_pVsync = new CCvarToggleCheckButton( this, "Vsync", "#GameUI_VSync", "gl_vsync" );

	vgui2::localize()->ConvertUnicodeToANSI(
		vgui2::localize()->Find( "#GameUI_AspectNormal" ),
		m_pszAspectName[ 0 ],
		ARRAYSIZE( m_pszAspectName[ 0 ] ) );

	vgui2::localize()->ConvertUnicodeToANSI(
		vgui2::localize()->Find( "#GameUI_AspectWide" ),
		m_pszAspectName[ 1 ],
		ARRAYSIZE( m_pszAspectName[ 1 ] ) );

	const auto iAspectNormal = m_pAspectRatio->AddItem( m_pszAspectName[ 0 ], nullptr );
	const auto iAspectWide = m_pAspectRatio->AddItem( m_pszAspectName[ 1 ], nullptr );

	//Determine which aspect is in use right now
	if( 3 * m_CurrentSettings.w + -4 * m_CurrentSettings.h )
	{
		m_bStartWidescreen = true;
		m_pAspectRatio->ActivateItem( iAspectWide );
	}
	else
	{
		m_bStartWidescreen = false;
		m_pAspectRatio->ActivateItem( iAspectNormal );
	}

	vgui2::localize()->ConvertUnicodeToANSI(
		vgui2::localize()->Find( "#GameUI_OpenGL" ),
		m_pszRenderNames[ 0 ],
		ARRAYSIZE( m_pszRenderNames[ 0 ] ) );

	m_pRenderer = new vgui2::ComboBox( this, "Renderer", 1, false );
	m_pRenderer->AddItem( m_pszRenderNames[ 0 ], nullptr );

	SetCurrentRendererComboItem();

	m_pWindowed = new vgui2::CheckButton( this, "Windowed", "#GameUI_Windowed" );
	m_pWindowed->SetSelected( m_CurrentSettings.windowed != 0 );

	m_pHDModels = new vgui2::CheckButton( this, "HDModels", "#GameUI_HDModels" );
	m_pHDModels->SetSelected( m_CurrentSettings.hdmodels != 0 );

	m_pAddonsFolder = new vgui2::CheckButton( this, "AddonsFolder", "#GameUI_AddonsFolder" );
	m_pAddonsFolder->SetSelected( m_CurrentSettings.addons_folder != 0 );

	m_pLowVideoDetail = new vgui2::CheckButton( this, "LowVideoDetail", "#GameUI_LowVideoDetail" );
	//TODO: not consistent with OnResetData - Solokiller
	m_pLowVideoDetail->SetSelected( m_CurrentSettings.vid_level == 0 );

	LoadControlSettings( "Resource\\OptionsSubVideo.res" );

	PrepareResolutionList();

	const auto flDetailTextures = engine->pfnGetCvarFloat( "r_detailtexturessupported" );

	if( ModInfo().GetDetailedTexture() )
	{
		if( flDetailTextures <= 0.0 )
			m_pDetailTextures->SetEnabled( false );
	}
	else
	{
		m_pDetailTextures->SetVisible( false );
	}
}

void COptionsSubVideo::OnButtonChecked( KeyValues* data )
{
	const auto iState = data->GetInt( "state", 0 );
	auto pButton = static_cast<vgui2::CheckButton*>( data->GetPtr( "panel", nullptr ) );

	if( m_pWindowed == pButton )
	{
		if( m_CurrentSettings.windowed != iState )
			OnDataChanged();
	}
	else if( m_pHDModels == pButton )
	{
		if( m_CurrentSettings.hdmodels != iState )
			OnDataChanged();
	}
	else if( m_pAddonsFolder == pButton )
	{
		if( m_CurrentSettings.addons_folder != iState )
			OnDataChanged();
	}
	else if( m_pLowVideoDetail == pButton )
	{
		if( m_CurrentSettings.vid_level != iState )
			OnDataChanged();
	}
	else if( m_pDetailTextures == pButton || m_pVsync == pButton )
		OnDataChanged();
}

void COptionsSubVideo::OnDataChanged()
{
	PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
}

void COptionsSubVideo::PrepareResolutionList()
{
	vmode_t* plist = nullptr;
	int count = 0;
	gameuifuncs->GetVideoModes( &plist, &count );

	m_pMode->DeleteAllItems();

	int nItemsAdded = 0;
	int selectedItemID = -1;
	bool bFoundWidescreen = false;
	char sz[ 256 ];

	for( int i = 0; i < count; ++i, ++plist )
	{
		snprintf( sz, ARRAYSIZE( sz ), "%i x %i", plist->width, plist->height );

		int itemID = -1;

		const bool bIsWideScreen = ( 3 * plist->width + -4 * plist->height ) != 0;

		if( bIsWideScreen )
		{
			bFoundWidescreen = true;
		}

		// filter the list for those matching the current aspect
		if( m_bStartWidescreen == bIsWideScreen )
		{
			itemID = m_pMode->AddItem( sz, nullptr );
			++nItemsAdded;
		}

		// try and find the best match for the resolution to be selected
		if( plist->width == m_CurrentSettings.w && plist->height == m_CurrentSettings.h )
		{
			selectedItemID = itemID;
		}
		else if( selectedItemID == -1 && plist->width == m_CurrentSettings.w && plist->height == m_CurrentSettings.h )
		{
			selectedItemID = itemID;
		}
	}

	m_pAspectRatio->SetEnabled( bFoundWidescreen );

	if( selectedItemID != -1 )
	{
		m_pMode->ActivateItem( selectedItemID );
	}

	//If no items could be added, the current system probably doesn't support the resolutions at this aspect.
	//Switch to the next aspect ratio and try again.
	//TODO: this could end up in an infinite loop if no resolutions work - Solokiller
	if( !nItemsAdded )
	{
		if( count )
		{
			m_bStartWidescreen = !m_bStartWidescreen;
			m_pAspectRatio->ActivateItem( ( m_pAspectRatio->GetActiveItem() + 1 ) % m_pAspectRatio->GetItemCount() );
			PrepareResolutionList();
		}
	}
}

void COptionsSubVideo::OnTextChanged( vgui2::Panel* pPanel, const char* pszText )
{
	if( m_pMode == pPanel )
	{
		char s[ 256 ];
		snprintf( s, ARRAYSIZE( s ), "%i x %i", m_CurrentSettings.w, m_CurrentSettings.h );

		if( strcmp( pszText, s ) )
			OnDataChanged();
	}
	else if( m_pRenderer == pPanel )
	{
		if( strcmp( pszText, m_pszRenderNames[ m_iStartRenderer ] ) )
			OnDataChanged();
	}
	else if( m_pAspectRatio == pPanel )
	{
		if( strcmp( pszText, m_pszAspectName[ m_bStartWidescreen ? 1 : 0 ] ) )
		{
			m_bStartWidescreen = !m_bStartWidescreen;
			PrepareResolutionList();
		}
	}
}

void COptionsSubVideo::SetCurrentRendererComboItem()
{
	m_iStartRenderer = 0;
	m_pRenderer->ActivateItemByRow( 0 );
}

void COptionsSubVideo::SetCurrentResolutionComboItem()
{
	vmode_t* plist = nullptr;
	int count = 0;
	gameuifuncs->GetVideoModes( &plist, &count );

	for( int i = 0; i < count; ++i, ++plist )
	{
		if( plist->width == m_CurrentSettings.w && plist->height == m_CurrentSettings.h )
		{
			char sz[ 256 ];
			snprintf( sz, ARRAYSIZE( sz ), "%i x %i", m_CurrentSettings.w, m_CurrentSettings.h );
			m_pMode->SetText( sz );
			break;
		}
	}
}

void COptionsSubVideo::OnResetData()
{
	m_CurrentSettings = m_OrigSettings;

	m_pBrightnessSlider->Reset();
	m_pGammaSlider->Reset();

	m_pWindowed->SetSelected( m_CurrentSettings.windowed != 0 );
	m_pHDModels->SetSelected( m_CurrentSettings.hdmodels != 0 );
	m_pAddonsFolder->SetSelected( m_CurrentSettings.addons_folder != 0 );
	m_pLowVideoDetail->SetSelected( m_CurrentSettings.vid_level != 0 );

	m_pDetailTextures->Reset();
	m_pVsync->Reset();

	SetCurrentRendererComboItem();
	SetCurrentResolutionComboItem();
}

void COptionsSubVideo::GetVidSettings()
{
	gameuifuncs->GetCurrentVideoMode(
		&m_OrigSettings.w,
		&m_OrigSettings.h,
		&m_OrigSettings.bpp
	);

	gameuifuncs->GetCurrentRenderer(
		m_OrigSettings.renderer,
		ARRAYSIZE( m_OrigSettings.renderer ),
		&m_OrigSettings.windowed,
		&m_OrigSettings.hdmodels,
		&m_OrigSettings.addons_folder,
		&m_OrigSettings.vid_level
	);

	RevertVidSettings();
}

void COptionsSubVideo::RevertVidSettings()
{
	m_CurrentSettings = m_OrigSettings;
}

void COptionsSubVideo::ApplyVidSettings( bool bForceRefresh )
{
	if( m_pMode )
	{
		char sz[ 256 ];
		m_pMode->GetText( sz, ARRAYSIZE( sz ) );

		int w, h;
		sscanf( sz, "%i x %i", &w, &h );

		m_CurrentSettings.w = w;
		m_CurrentSettings.h = h;
		m_CurrentSettings.bpp = 32;
	}

	if( m_pRenderer )
	{
		char sz[ 256 ];
		m_pRenderer->GetText( sz, ARRAYSIZE( sz ) );

		strcpy( m_CurrentSettings.renderer, "gl" );
	}

	if( m_pWindowed )
		m_CurrentSettings.windowed = m_pWindowed->IsSelected();

	if( m_pHDModels )
		m_CurrentSettings.hdmodels = m_pHDModels->IsSelected();

	if( m_pAddonsFolder )
		m_CurrentSettings.addons_folder = m_pAddonsFolder->IsSelected();

	if( m_pLowVideoDetail )
		m_CurrentSettings.vid_level = m_pLowVideoDetail->IsSelected();

	if( m_pDetailTextures )
		m_pDetailTextures->ApplyChanges();

	if( m_pVsync )
		m_pVsync->ApplyChanges();
	if( memcmp( &m_OrigSettings, &m_CurrentSettings, sizeof( m_OrigSettings ) ) || bForceRefresh )
	{
		char szCmd[ 256 ];

		sprintf(
			szCmd,
			"_setvideomode %i %i %i\n",
			m_CurrentSettings.w,
			m_CurrentSettings.h,
			m_CurrentSettings.bpp
		);

		engine->pfnClientCmd( szCmd );

		auto pszWindowMode = m_CurrentSettings.windowed ? "windowed" : "fullscreen";

		sprintf( szCmd, "_setrenderer %s %s\n", m_CurrentSettings.renderer, pszWindowMode );
		engine->pfnClientCmd( szCmd );

		sprintf( szCmd, "_sethdmodels %d\n", m_CurrentSettings.hdmodels );
		engine->pfnClientCmd( szCmd );

		sprintf( szCmd, "_setaddons_folder %d\n", m_CurrentSettings.addons_folder );
		engine->pfnClientCmd( szCmd );

		sprintf( szCmd, "_set_vid_level %d\n", m_CurrentSettings.vid_level );
		engine->pfnClientCmd( szCmd );

		engine->pfnClientCmd( "_restart\n" );
	}
}

void COptionsSubVideo::OnApplyChanges()
{
	const bool bSlidersChanged =
		m_pBrightnessSlider->HasBeenModified() ||
		m_pGammaSlider->HasBeenModified();

	m_pBrightnessSlider->ApplyChanges();
	m_pGammaSlider->ApplyChanges();

	ApplyVidSettings( bSlidersChanged );
}
