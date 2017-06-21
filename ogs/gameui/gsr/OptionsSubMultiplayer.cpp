#include "winheaders.h"

#include <FileSystem.h>
#include <UtlBuffer.h>
#include <UtlRBTree.h>
#include <UtlSymbol.h>

#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>

#include "BitmapImagePanel.h"
#include "CreateMultiplayerGameServerPage.h"
#include "CvarSlider.h"
#include "CvarTextEntry.h"
#include "CvarToggleCheckButton.h"
#include "EngineInterface.h"
#include "LabeledCommandComboBox.h"
#include "LogoFile.h"
#include "ModInfo.h"
#include "OptionsSubMultiplayer.h"
#include "shared_util.h"

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')
#define SUIT_HUE_START 192
#define SUIT_HUE_END 223
#define PLATE_HUE_START 160
#define PLATE_HUE_END 191

struct ColorItem_t
{
	const char* name;
	int r;
	int g;
	int b;
};

static const ColorItem_t itemlist[] = 
{
	{ "#Valve_Orange", 255, 120, 24 },
	{ "#Valve_Yellow", 225, 180, 24 },
	{ "#Valve_Blue", 0, 60, 255 },
	{ "#Valve_Ltblue", 0, 167, 255 },
	{ "#Valve_Green", 0, 167, 0 },
	{ "#Valve_Red", 255, 43, 0 },
	{ "#Valve_Brown", 123, 73, 0 },
	{ "#Valve_Ltgray", 100, 100, 100 },
	{ "#Valve_Dkgray", 36, 36, 36 }
};

static const ColorItem_t s_crosshairColors[] =
{
	{ "#Valve_Green",	50,		250,	50 },
	{ "#Valve_Red",		250,	50,		50 },
	{ "#Valve_Blue",	50,		50,		250 },
	{ "#Valve_Yellow",	250,	250,	50 },
	{ "#Valve_Ltblue",	50,		250,	250 }
};

static void PaletteHueReplace( RGBQUAD *palSrc, int newHue, int Start, int end )
{
	int i;
	float r, b, g;
	float maxcol, mincol;
	float hue, val, sat;

	hue = ( float ) ( newHue * ( 360.0 / 255 ) );

	for( i = Start; i <= end; i++ )
	{
		b = palSrc[ i ].rgbBlue;
		g = palSrc[ i ].rgbGreen;
		r = palSrc[ i ].rgbRed;

		maxcol = max( max( r, g ), b ) / 255.0f;
		mincol = min( min( r, g ), b ) / 255.0f;

		val = maxcol;
		sat = ( maxcol - mincol ) / maxcol;

		mincol = val * ( 1.0f - sat );

		if( hue <= 120 )
		{
			b = mincol;
			if( hue < 60 )
			{
				r = val;
				g = mincol + hue * ( val - mincol ) / ( 120 - hue );
			}
			else
			{
				g = val;
				r = mincol + ( 120 - hue )*( val - mincol ) / hue;
			}
		}
		else if( hue <= 240 )
		{
			r = mincol;
			if( hue < 180 )
			{
				g = val;
				b = mincol + ( hue - 120 )*( val - mincol ) / ( 240 - hue );
			}
			else
			{
				b = val;
				g = mincol + ( 240 - hue )*( val - mincol ) / ( hue - 120 );
			}
		}
		else
		{
			g = mincol;
			if( hue < 300 )
			{
				b = val;
				r = mincol + ( hue - 240 )*( val - mincol ) / ( 360 - hue );
			}
			else
			{
				r = val;
				b = mincol + ( 360 - hue )*( val - mincol ) / ( hue - 240 );
			}
		}

		palSrc[ i ].rgbBlue = ( unsigned char ) ( b * 255 );
		palSrc[ i ].rgbGreen = ( unsigned char ) ( g * 255 );
		palSrc[ i ].rgbRed = ( unsigned char ) ( r * 255 );
	}
}

CrosshairImagePanel::CrosshairImagePanel( vgui2::Panel* parent, const char* name, CCvarToggleCheckButton* pAdditive )
	: BaseClass( parent, name )
{
	m_pAdditive = pAdditive;
	UpdateCrosshair( 50, 250, 50, 0 );
}

void CrosshairImagePanel::UpdateCrosshair( int r, int g, int b, int size )
{
	m_R = r;
	m_G = g;
	m_B = b;

	int screenWide, screenTall;
	vgui2::surface()->GetScreenSize( screenWide, screenTall );
	if( size == 0 )
	{
		if( screenWide <= 640 )
		{
			// if the screen width is 640 or less, set the crosshair num to 3 (large)
			size = 1;
		}
		else if( screenWide < 1024 )
		{
			// if the screen width is between 640 and 1024, set the crosshair num to 2 (medium)
			size = 2;
		}
		else
		{
			// if the screen width is 1024 or greater, set the crosshair num to 1 (small)
			size = 3;
		}
	}

	int scaleBase = 1024;
	switch( size )
	{
	case 3:
		scaleBase = 640;
		break;
	case 2:
		scaleBase = 800;
		break;
	default:
		scaleBase = 1024;
		break;
	}

	m_barSize = ( int ) 9 * screenWide / scaleBase;
	m_barGap = ( int ) 5 * screenWide / scaleBase;
}

void CrosshairImagePanel::Paint()
{
	BaseClass::Paint();

	if( !m_pAdditive )
		return;

	int wide, tall;
	GetSize( wide, tall );

	bool additive = m_pAdditive->IsSelected();

	auto pfnFill = additive ? engine->pfnFillRGBA : engine->pfnFillRGBABlend;

	pfnFill( ( wide / 2 - m_barGap - m_barSize ) + 1, tall / 2, m_barSize, 1, m_R, m_G, m_B, 255 );
	pfnFill( wide / 2 + m_barGap, tall / 2, m_barSize, 1, m_R, m_G, m_B, 255 );
	pfnFill( wide / 2, tall / 2 - m_barGap - m_barSize, 1, m_barSize, m_R, m_G, m_B, 255 );
	pfnFill( wide / 2, tall / 2 + m_barGap, 1, m_barSize, m_R, m_G, m_B, 255 );
}

COptionsSubMultiplayer::COptionsSubMultiplayer( vgui2::Panel* parent )
	: BaseClass( parent, "OptionsSubMultiplayer" )
{
	auto pCancel = new vgui2::Button( this, "Cancel", "#GameUI_Cancel" );
	pCancel->SetCommand( "Close" );

	auto pOk = new vgui2::Button( this, "OK", "#GameUI_OK" );
	pOk->SetCommand( "Ok" );

	auto pApply = new vgui2::Button( this, "Apply", "#GameUI_Apply" );
	pApply->SetCommand( "Apply" );

	auto pAdvanced = new vgui2::Button( this, "Advanced", "#GameUI_AdvancedEllipsis" );
	pAdvanced->SetCommand( "Advanced" );

	auto pPlayerName = new vgui2::Label( this, "NameLabel", "#GameUI_PlayerName" );

	m_pNameTextEntry = new CCvarTextEntry( this, "NameEntry", "name" );

	m_pPrimaryColorSlider = new CCvarSlider( this, "Primary Color Slider", "#GameUI_PrimaryColor", 0.0, 255.0, "topcolor", false );
	m_pSecondaryColorSlider = new CCvarSlider( this, "Secondary Color Slider", "#GameUI_SecondaryColor", 0.0, 255.0, "bottomcolor", false );
	
	m_pHighQualityModelCheckBox = new CCvarToggleCheckButton( this, "High Quality Models", "#GameUI_HighModels", "cl_himodels" );
	
	m_pModelList = new CLabeledCommandComboBox( this, "Player model" );
	InitModelList( m_pModelList );

	m_pLogoList = new CLabeledCommandComboBox( this, "SpraypaintList" );
	m_LogoName[ 0 ] = '\0';
	InitLogoList( m_pLogoList );

	m_pColorList = new CLabeledCommandComboBox( this, "SpraypaintColor" );

	auto pszLogoColor = engine->pfnGetCvarString( "cl_logocolor" );

	char sz[ 128 ];

	int selected = 0;

	for( int i = 0; i < ARRAYSIZE( itemlist ); ++i )
	{
		const auto& color = itemlist[ i ];

		sprintf( sz, "cl_logocolor %s\n", color.name );
		m_pColorList->AddItem( color.name, sz );

		if( pszLogoColor && !stricmp( pszLogoColor, color.name ) )
			selected = i;
	}

	m_pColorList->SetInitialItem( selected );
	m_pColorList->AddActionSignalTarget( this );

	m_pModelImage = new CBitmapImagePanel( this, "ModelImage", nullptr );
	m_pModelImage->AddActionSignalTarget( this );

	m_pLogoImage = new CBitmapImagePanel( this, "LogoImage", nullptr );
	m_pLogoImage->AddActionSignalTarget( this );

	m_nTopColor = 30;
	m_nBottomColor = 6;
	m_pCrosshairImage = 0;
	m_pCrosshairSize = 0;
	m_pCrosshairTranslucencyCheckbox = 0;
	m_pCrosshairColorComboBox = 0;
	m_displayedCrosshairColorRed = -1;
	m_displayedCrosshairColorGreen = -1;
	m_displayedCrosshairColorBlue = -1;

	if( !stricmp( gEngfuncs.pfnGetGameDirectory(), "czero" ) ||
		!stricmp( gEngfuncs.pfnGetGameDirectory(), "terror" ) ||
		!stricmp( gEngfuncs.pfnGetGameDirectory(), "cstrike" ) ||
		!stricmp( gEngfuncs.pfnGetGameDirectory(), "cstrike_beta" ) )
	{
		m_displayedCrosshairColorGreen = 250;
		m_displayedCrosshairColorBlue = 50;
		m_displayedCrosshairColorRed = 50;

		m_pCrosshairColorComboBox = new vgui2::ComboBox( this, "CrosshairColorComboBox", 6, false );

		InitCrosshairColorEntries();

		m_pCrosshairSize = new CLabeledCommandComboBox( this, "CrosshairSizeComboBox" );

		m_pCrosshairTranslucencyCheckbox = new CCvarToggleCheckButton( this, "CrosshairTranslucencyCheckbox", "#GameUI_Translucent", "cl_crosshair_translucent" );
		
		InitCrosshairSizeList( m_pCrosshairSize );

		m_pCrosshairImage = new CrosshairImagePanel( this, "CrosshairImage", m_pCrosshairTranslucencyCheckbox );
		
		RedrawCrosshairImage();
	}

	m_nLogoR = 255;
	m_nLogoG = 255;
	m_nLogoB = 255;

	LoadControlSettings( "Resource/OptionsSubMultiplayer.res" );

	if( ModInfo().NoModels() )
	{
		if( m_pModelImage )
			m_pModelImage->SetVisible( false );

		if( m_pModelList )
			m_pModelList->SetVisible( false );

		if( m_pPrimaryColorSlider )
			m_pPrimaryColorSlider->SetVisible( false );

		if( m_pSecondaryColorSlider )
			m_pSecondaryColorSlider->SetVisible( false );

		auto pLabel1 = FindChildByName( "Label1", false );
		if( pLabel1 )
			pLabel1->SetVisible( false );

		auto pColors = FindChildByName( "Colors", false );
		if( pColors )
			pColors->SetVisible( false );
	}

	if( ModInfo().NoHiModel() )
	{
		if( m_pHighQualityModelCheckBox )
			m_pHighQualityModelCheckBox->SetVisible( false );
	}
}

void COptionsSubMultiplayer::OnResetData()
{
	//Nothing
}

void COptionsSubMultiplayer::OnApplyButtonEnable()
{
	PostMessage( GetParent(), new KeyValues( "ApplyButtonEnable" ) );

	InvalidateLayout();
}

void COptionsSubMultiplayer::OnCommand( const char* command )
{
	if( !stricmp( command, "Advanced" ) )
	{
		if( !m_hMultiplayerAdvancedDialog )
		{
			m_hMultiplayerAdvancedDialog = new CMultiplayerAdvancedDialog( this );
		}
		
		m_hMultiplayerAdvancedDialog->Activate();
	}

	BaseClass::OnCommand( command );
}

void COptionsSubMultiplayer::OnApplyChanges()
{
	m_pPrimaryColorSlider->ApplyChanges();
	m_pSecondaryColorSlider->ApplyChanges();
	m_pModelList->ApplyChanges();
	m_pLogoList->ApplyChanges();

	m_pLogoList->GetText( m_LogoName, ARRAYSIZE( m_LogoName ) );

	m_pColorList->ApplyChanges();
	m_pHighQualityModelCheckBox->ApplyChanges();

	m_pNameTextEntry->ApplyChanges( false );

	if( m_pCrosshairSize )
		m_pCrosshairSize->ApplyChanges();

	if( m_pCrosshairTranslucencyCheckbox )
		m_pCrosshairTranslucencyCheckbox->ApplyChanges();

	ApplyCrosshairColorChanges();

	auto pszItem = m_pColorList->GetActiveItemCommand();

	if( pszItem && *pszItem )
	{
		auto pszFileName = pszItem + strlen( "cl_logocolor " );
		char cmd[ 512 ];
		snprintf( cmd, ARRAYSIZE( cmd ) - 1, "cl_logofile %s\n", m_LogoName );
		engine->pfnClientCmd( cmd );

		char infile[ 256 ];
		sprintf( infile, "logos/%s.tga", m_LogoName );

		if( vgui2::filesystem()->FileExists( infile ) )
		{
			UpdateLogoWADFromTGA( infile );
			return;
		}

		int r = 0, g = 0, b = 0;

		for( int i = 0; i < ARRAYSIZE( itemlist ); ++i )
		{
			if( !strnicmp( pszFileName, itemlist[ i ].name, strlen( itemlist[ i ].name ) ) )
			{
				r = itemlist[ i ].r;
				g = itemlist[ i ].g;
				b = itemlist[ i ].b;
				break;
			}
		}

		strcpy( infile, "logos/remapped.bmp" );

		auto hFile = vgui2::filesystem()->Open( infile, "rb" );

		if( hFile )
		{
			auto uiSize = vgui2::filesystem()->Size( hFile );

			BITMAPFILEHEADER bmfHeader;

			vgui2::filesystem()->Read( &bmfHeader, sizeof( bmfHeader ), hFile );

			if( bmfHeader.bfType == DIB_HEADER_MARKER )
			{
				auto uiBufSize = uiSize - sizeof( bmfHeader );
				auto pBuffer = malloc( uiBufSize );
				vgui2::filesystem()->Read( pBuffer, uiBufSize, hFile );
				UpdateLogoWAD( pBuffer, r, g, b );
				free( pBuffer );
			}

			vgui2::filesystem()->Close( hFile );
		}
	}
}

void COptionsSubMultiplayer::InitModelList( CLabeledCommandComboBox* cb )
{
	vgui2::filesystem()->RemoveFile( "models/player/remapped.bmp", "GAMECONFIG" );
	vgui2::filesystem()->RemoveFile( "models/player/remapped.bmp", "GAME" );
	vgui2::filesystem()->RemoveFile( "models/player/remapped.bmp", nullptr );

	auto pszModel = engine->pfnGetCvarString( "model" );

	char currentmodel[ 64 ];
	currentmodel[ 0 ] = '\0';

	if( pszModel )
		strcpy( currentmodel, pszModel );

	char cmdstring[ 256 ];

	FileFindHandle_t findHandle;
	auto pszFileName = vgui2::filesystem()->FindFirst( "models/player/*.*", &findHandle );

	int iInitialItem = 0;

	if( pszFileName )
	{
		int c = 0;

		do
		{
			if( vgui2::filesystem()->FindIsDirectory( findHandle ) && *pszFileName != '.' )
			{
				snprintf( cmdstring, ARRAYSIZE( cmdstring ), "model %s\n", pszFileName );

				if( !stricmp( currentmodel, pszFileName ) )
					iInitialItem = c;

				cb->AddItem( pszFileName, cmdstring );
				++c;
			}
		}
		while( ( pszFileName = vgui2::filesystem()->FindNext( findHandle ) ) != nullptr );
	}

	//TODO: this can crash if FindFirst returned null - Solokiller
	vgui2::filesystem()->FindClose( findHandle );

	cb->SetInitialItem( iInitialItem );
}

void COptionsSubMultiplayer::RedrawCrosshairImage()
{
	if( m_pCrosshairColorComboBox )
	{
		auto pItemData = m_pCrosshairColorComboBox->GetActiveItemUserData();
		const int r = pItemData->GetInt( "red", 0 );
		const int g = pItemData->GetInt( "green", 0 );
		const int b = pItemData->GetInt( "blue", 0 );

		if( m_displayedCrosshairColorRed != r ||
			m_displayedCrosshairColorGreen != g ||
			m_displayedCrosshairColorBlue != b )
		{
			m_displayedCrosshairColorRed = r;
			m_displayedCrosshairColorGreen = g;
			m_displayedCrosshairColorBlue = b;

			OnApplyButtonEnable();
		}

		if( m_pCrosshairImage )
		{
			if( m_pCrosshairSize )
			{	
				m_pCrosshairImage->UpdateCrosshair(
					m_displayedCrosshairColorRed,
					m_displayedCrosshairColorGreen,
					m_displayedCrosshairColorBlue,
					m_pCrosshairSize->GetActiveItem()
				);
			}
		}
	}
}

void COptionsSubMultiplayer::InitCrosshairColorEntries()
{
	int r = 50;
	int g = 250;
	int b = 50;

	const char* pszCrosshairColor = engine->pfnGetCvarString( "cl_crosshair_color" );

	if( pszCrosshairColor )
	{
		int parsedR, parsedG, parsedB;

		pszCrosshairColor = SharedParse( pszCrosshairColor );

		auto pszToken = SharedGetToken();

		if( pszToken )
		{
			parsedR = strtol( pszToken, nullptr, 10 );

			pszCrosshairColor = SharedParse( pszCrosshairColor );
			pszToken = SharedGetToken();

			if( pszToken )
			{
				parsedG = strtol( pszToken, nullptr, 10 );

				pszCrosshairColor = SharedParse( pszCrosshairColor );
				pszToken = SharedGetToken();

				if( pszToken )
				{
					parsedB = strtol( pszToken, nullptr, 10 );

					r = parsedR;
					g = parsedG;
					b = parsedB;
				}
			}
		}
	}

	if( m_pCrosshairColorComboBox )
	{
		auto pData = new KeyValues( "data" );

		pData->Clear();

		int iCurrentCrosshairIndex = 0;

		for( int i = 0; i < ARRAYSIZE( s_crosshairColors ); ++i )
		{
			const auto& color = s_crosshairColors[ i ];

			pData->SetInt( "red", color.r );
			pData->SetInt( "green", color.g );
			pData->SetInt( "blue", color.b );

			m_pCrosshairColorComboBox->AddItem( color.name, pData );

			if( color.r == r &&
				color.g == g &&
				color.b == b )
				iCurrentCrosshairIndex = i;
		}

		m_pCrosshairColorComboBox->ActivateItemByRow( iCurrentCrosshairIndex );

		pData->deleteThis();
	}

	RedrawCrosshairImage();
}

void COptionsSubMultiplayer::InitCrosshairSizeList( CLabeledCommandComboBox* cb )
{
	if( cb )
	{
		cb->Reset();

		cb->AddItem( "#GameUI_Auto", "cl_crosshair_size auto" );
		cb->AddItem( "#GameUI_Small", "cl_crosshair_size small" );
		cb->AddItem( "#GameUI_Medium", "cl_crosshair_size medium" );
		cb->AddItem( "#GameUI_Large", "cl_crosshair_size large" );

		auto pszSize = engine->pfnGetCvarString( "cl_crosshair_size" );

		int iInitialItem;

		if( pszSize || !stricmp( "auto", pszSize ) )
		{
			iInitialItem = 0;
		}
		if( !stricmp( "small", pszSize ) )
		{
			iInitialItem = 1;
		}
		if( !stricmp( "medium", pszSize ) )
		{
			iInitialItem = 2;
		}
		if( !stricmp( "large", pszSize ) )
		{
			iInitialItem = 3;
		}
		else
		{
			iInitialItem = 0;
		}

		cb->SetInitialItem( iInitialItem );
	}
}

void COptionsSubMultiplayer::RemapPalette( char* filename, int topcolor, int bottomcolor )
{
	char infile[ 256 ];
	char outfile[ 256 ];

	FileHandle_t file;
	CUtlBuffer outbuffer( 20480, 20480 );

	Q_snprintf( infile, sizeof( infile ), "models/player/%s/%s.bmp", filename, filename );
	Q_strncpy( outfile, "models/player/remapped.bmp", sizeof( outfile ) );

	file = vgui2::filesystem()->Open( infile, "rb" );
	if( file == FILESYSTEM_INVALID_HANDLE )
		return;

	// Parse bitmap
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize, dwFileSize;
	LPBITMAPINFO lpbmi;

	dwFileSize = vgui2::filesystem()->Size( file );

	vgui2::filesystem()->Read( &bmfHeader, sizeof( bmfHeader ), file );

	outbuffer.Put( &bmfHeader, sizeof( bmfHeader ) );

	if( bmfHeader.bfType == DIB_HEADER_MARKER )
	{
		dwBitsSize = dwFileSize - sizeof( bmfHeader );

		HGLOBAL hDIB = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize );
		char *pDIB = ( LPSTR ) GlobalLock( ( HGLOBAL ) hDIB );
		{
			vgui2::filesystem()->Read( pDIB, dwBitsSize, file );

			lpbmi = ( LPBITMAPINFO ) pDIB;

	
			//TODO: add a check for biClrUsed == 0 - Solokiller
			//See https://msdn.microsoft.com/en-us/library/windows/desktop/dd183376(v=vs.85).aspx for why this is needed
			// Remap palette
			if( lpbmi->bmiHeader.biClrUsed > SUIT_HUE_END )
				PaletteHueReplace( lpbmi->bmiColors, topcolor, SUIT_HUE_START, SUIT_HUE_END );
			
			if( lpbmi->bmiHeader.biClrUsed > PLATE_HUE_END )
				PaletteHueReplace( lpbmi->bmiColors, bottomcolor, PLATE_HUE_START, PLATE_HUE_END );

			outbuffer.Put( pDIB, dwBitsSize );
		}

		GlobalUnlock( hDIB );
		GlobalFree( ( HGLOBAL ) hDIB );
	}

	vgui2::filesystem()->Close( file );

	vgui2::filesystem()->RemoveFile( outfile, "GAMECONFIG" );

	vgui2::filesystem()->CreateDirHierarchy( "models/player", "GAMECONFIG" );
	file = vgui2::filesystem()->Open( outfile, "wb", "GAMECONFIG" );
	if( file != FILESYSTEM_INVALID_HANDLE )
	{
		vgui2::filesystem()->Write( outbuffer.Base(), outbuffer.TellPut(), file );
		vgui2::filesystem()->Close( file );
	}
}

void COptionsSubMultiplayer::RemapModel()
{
	char modelname[ 256 ];
	m_pModelList->GetText( modelname, ARRAYSIZE( modelname ) );

	if( modelname[ 0 ] )
	{
		char texture[ 256 ] = "models/player/remapped";

		RemapPalette( modelname, m_nTopColor, m_nBottomColor );
		m_pModelImage->setTexture( texture );
	}
}

void COptionsSubMultiplayer::OnSliderMoved( KeyValues* data )
{
	m_nTopColor = static_cast<int>( m_pPrimaryColorSlider->GetSliderValue() );
	m_nBottomColor = static_cast<int>( m_pSecondaryColorSlider->GetSliderValue() );

	RemapModel();
	RedrawCrosshairImage();
}

void COptionsSubMultiplayer::ColorForName( const char* pszColorName, int& r, int& g, int& b )
{
	r = 0;
	g = 0;
	b = 0;

	for( const auto& color : itemlist )
	{
		if( !strnicmp( pszColorName, color.name, strlen( color.name ) ) )
		{
			r = color.r;
			g = color.g;
			b = color.b;
			return;
		}
	}
}

void COptionsSubMultiplayer::RemapLogoPalette( char* filename, int r, int g, int b )
{
	CUtlBuffer outbuffer( 20480, 20480 );

	char infile[ 256 ];
	sprintf( infile, "logos/%s.bmp", filename );

	auto hInFile = vgui2::filesystem()->Open( infile, "rb" );

	if( hInFile )
	{
		const auto uiSize = vgui2::filesystem()->Size( hInFile );

		BITMAPFILEHEADER bmfHeader;
		vgui2::filesystem()->Read( &bmfHeader, sizeof( bmfHeader ), hInFile );
		outbuffer.Put( &bmfHeader, sizeof( bmfHeader ) );

		if( bmfHeader.bfType == DIB_HEADER_MARKER )
		{
			const auto uiBufferSize = uiSize - sizeof( bmfHeader );
			auto pBuffer = malloc( uiBufferSize );
			vgui2::filesystem()->Read( pBuffer, uiBufferSize, hInFile );

			auto pInfo = reinterpret_cast<BITMAPINFO*>( pBuffer );

			pInfo->bmiHeader.biClrUsed = 256;

			auto pColor = pInfo->bmiColors;

			for( int i = 0; i < 256; ++i, ++pColor )
			{
				pColor->rgbRed = static_cast<int>( i * ( r / 255.0 ) );
				pColor->rgbGreen = static_cast<int>( i * ( g / 255.0 ) );
				pColor->rgbBlue = static_cast<int>( i * ( b / 255.0 ) );
			}

			outbuffer.Put( pBuffer, uiBufferSize );
			free( pBuffer );
		}
		vgui2::filesystem()->Close( hInFile );

		char outfile[ 256 ] = "logos/remapped.bmp";

		vgui2::filesystem()->RemoveFile( outfile, "GAMECONFIG" );
		vgui2::filesystem()->CreateDirHierarchy( "logos", "GAMECONFIG" );
		auto hOutFile = vgui2::filesystem()->Open( outfile, "wb", "GAMECONFIG" );

		if( hOutFile )
		{
			vgui2::filesystem()->Write( outbuffer.Base(), outbuffer.TellPut(), hOutFile );
			vgui2::filesystem()->Close( hOutFile );
		}
	}
}

void COptionsSubMultiplayer::RemapLogo()
{
	char logoname[ 256 ];
	m_pLogoList->GetText( logoname, ARRAYSIZE( logoname ) );

	if( !logoname[ 0 ] )
		return;

	char texture[ 256 ];
	sprintf( texture, "logos/%s.tga", logoname );

	if( vgui2::filesystem()->FileExists( texture ) )
	{
		sprintf( texture, "logos/%s", logoname );
		m_pColorList->SetEnabled( false );
		m_pLogoImage->setTexture( texture );
		return;
	}

	strcpy( texture, "logos/remapped" );

	auto pszItem = m_pColorList->GetActiveItemCommand();

	if( !pszItem || !( *pszItem ) )
		return;

	auto pszFileName = pszItem + strlen( "cl_logocolor " );

	int r = 0, g = 0, b = 0;

	for( int i = 0; i < ARRAYSIZE( itemlist ); ++i )
	{
		if( !strnicmp( pszFileName, itemlist[ i ].name, strlen( itemlist[ i ].name ) ) )
		{
			r = itemlist[ i ].r;
			g = itemlist[ i ].g;
			b = itemlist[ i ].b;
			break;
		}
	}

	RemapLogoPalette( logoname, r, g, b );
	m_pColorList->SetEnabled( true );
	m_pLogoImage->setTexture( texture );
}

void COptionsSubMultiplayer::OnTextChanged( vgui2::Panel* panel )
{
	if( m_pNameTextEntry != panel )
	{
		RemapModel();
		RemapLogo();
		RedrawCrosshairImage();
	}
}

void COptionsSubMultiplayer::ApplyCrosshairColorChanges()
{
	if( m_displayedCrosshairColorRed != -1 &&
		m_displayedCrosshairColorGreen != -1 &&
		m_displayedCrosshairColorBlue != -1 )
	{
		char cmd[ 256 ];
		cmd[ 0 ] = '\0';

		sprintf(
			cmd, "%d %d %d",
			m_displayedCrosshairColorRed,
			m_displayedCrosshairColorGreen,
			m_displayedCrosshairColorBlue
		);

		engine->Cvar_Set( "cl_crosshair_color", cmd );
	}
}

void COptionsSubMultiplayer::InitLogoList( CLabeledCommandComboBox* cb )
{
	vgui2::filesystem()->RemoveFile( "logos/remapped.bmp", "GAMECONFIG" );
	vgui2::filesystem()->RemoveFile( "logos/remapped.bmp", "GAME" );
	vgui2::filesystem()->RemoveFile( "logos/remapped.bmp", nullptr );

	CUtlRBTree<CUtlSymbol, short> treeMapNames( &CaselessStringLessThan );

	auto logofile = engine->pfnGetCvarString( "cl_logofile" );

	char directory[ 512 ] = "logos/*.bmp";

	FileFindHandle_t fh;
	auto pszFileName = vgui2::filesystem()->FindFirst( directory, &fh );

	if( pszFileName )
	{
		char filename[ 512 ];

		do
		{
			if( stricmp( pszFileName, "remapped.bmp" ) && *pszFileName != '.' && *pszFileName )
			{
				strcpy( filename, pszFileName );

				const auto uiFilenameLength = strlen( filename );

				if( uiFilenameLength > 3 )
					filename[ uiFilenameLength - 4 ] = '\0';

				treeMapNames.InsertIfNotFound( CUtlSymbol( filename ) );
			}
		}
		while( ( pszFileName = vgui2::filesystem()->FindNext( fh ) ) != nullptr );
	}

	//TODO: this can crash if FindFirst returned null - Solokiller
	vgui2::filesystem()->FindClose( fh );

	int iItem = 0;

	for( auto i = treeMapNames.FirstInorder();
		 i != treeMapNames.InvalidIndex();
		 i = treeMapNames.NextInorder( i ) )
	{
		const auto& file = treeMapNames[ i ];

		cb->AddItem( file.String(), "" );

		if( !stricmp( file.String(), logofile ) )
			cb->SetInitialItem( iItem );

		++iItem;
	}
}
