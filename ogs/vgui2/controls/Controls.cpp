//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//


#include <FileSystem.h>

#include <stdio.h>
#include <string.h>
#include <locale.h>

#include <vgui/IInputInternal.h>
#include <vgui/IKeyValues.h>
#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>

#include <vgui_controls/Controls.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IKeyValues* g_pKeyValuesInterface = nullptr;

IKeyValues* keyvalues()
{
	return g_pKeyValuesInterface;
}

namespace vgui2
{

vgui2::IInputInternal *g_pInputInterface = NULL;
vgui2::IInputInternal *input()
{
	return g_pInputInterface;
}

vgui2::ISchemeManager009 *g_pSchemeInterface = NULL;

class CSchemeManagerWrapper final : public ISchemeManager
{
public:
	HScheme LoadSchemeFromFile( const char *fileName, const char *tag ) override
	{
		return g_pSchemeInterface->LoadSchemeFromFile( fileName, tag );
	}

	void ReloadSchemes() override
	{
		g_pSchemeInterface->ReloadSchemes();
	}

	HScheme GetDefaultScheme() override
	{
		return g_pSchemeInterface->GetDefaultScheme();
	}

	HScheme GetScheme( const char *tag ) override
	{
		return g_pSchemeInterface->GetScheme( tag );
	}

	IImage *GetImage( const char *imageName, bool hardwareFiltered ) override
	{
		return g_pSchemeInterface->GetImage( imageName, hardwareFiltered );
	}

	HTexture GetImageID( const char *imageName, bool hardwareFiltered ) override
	{
		return g_pSchemeInterface->GetImageID( imageName, hardwareFiltered );
	}

#ifdef _XBOX
	void DeleteImage( const char *pImageName ) override
	{
		g_pSchemeInterface->DeleteImage( pImageName );
	}
#endif

	IScheme *GetIScheme( HScheme scheme ) override
	{
		return g_pSchemeInterface->GetIScheme( scheme );
	}

	void Shutdown( bool full = true ) override
	{
		g_pSchemeInterface->Shutdown( full );
	}

	int GetProportionalScaledValue( int normalizedValue ) override
	{
		return g_pSchemeInterface->GetProportionalScaledValue( normalizedValue );
	}

	int GetProportionalNormalizedValue( int scaledValue ) override
	{
		return g_pSchemeInterface->GetProportionalNormalizedValue( scaledValue );
	}

	//Just call through to the normal versions.
	HScheme LoadSchemeFromFileEx( VPANEL sizingPanel, const char *fileName, const char *tag ) override
	{
		return g_pSchemeInterface->LoadSchemeFromFile( fileName, tag );
	}

	int GetProportionalScaledValueEx( HScheme scheme, int normalizedValue ) override
	{
		return g_pSchemeInterface->GetProportionalScaledValue( normalizedValue );
	}

	int GetProportionalNormalizedValueEx( HScheme scheme, int scaledValue ) override
	{
		return g_pSchemeInterface->GetProportionalNormalizedValue( scaledValue );
	}
};

static CSchemeManagerWrapper g_SchemeWrapper;

vgui2::ISchemeManager *scheme()
{
	return &g_SchemeWrapper;
}

vgui2::ISurface *g_pSurfaceInterface = NULL;
vgui2::ISurface *surface()
{
	return g_pSurfaceInterface;
}

vgui2::ISystem *g_pSystemInterface = NULL;
vgui2::ISystem *system()
{
	return g_pSystemInterface;
}

vgui2::IVGui *g_pVGuiInterface = NULL;
vgui2::IVGui *ivgui()
{
	return g_pVGuiInterface;
}

vgui2::IPanel *g_pPanelInterface = NULL;
vgui2::IPanel *ipanel()
{
	return g_pPanelInterface;
}

IFileSystem *g_pFileSystemInterface = NULL;
IFileSystem *filesystem()
{
	Assert( g_pFileSystemInterface );
	return g_pFileSystemInterface;
}

vgui2::ILocalize *g_pLocalizeInterface = NULL;
vgui2::ILocalize *localize()
{
	return g_pLocalizeInterface;
}

static char g_szControlsModuleName[256];

//-----------------------------------------------------------------------------
// Purpose: Initializes the controls
//-----------------------------------------------------------------------------
extern "C" { extern int _heapmin(); }

//-----------------------------------------------------------------------------
// Purpose: finds a particular interface in the factory set
//-----------------------------------------------------------------------------
IBaseInterface* InitializeInterface( const char* interfaceName, CreateInterfaceFn* factoryList, int numFactories )
{
	IBaseInterface* retval;

	for( int i = 0; i < numFactories; i++ )
	{
		CreateInterfaceFn factory = factoryList[ i ];
		if( !factory )
			continue;

		retval = factory( interfaceName, nullptr );
		if( retval )
			return retval;
	}

	// No provider for requested interface!!!
	// Assert( !"No provider for requested interface!!!" );

	return nullptr;
}

bool VGui_InitInterfacesList( const char *moduleName, CreateInterfaceFn *factoryList, int numFactories )
{
	// If you hit this error, then you need to include memoverride.cpp in the project somewhere or else
	// you'll get crashes later when vgui_controls allocates KeyValues and vgui tries to delete them.
#if !defined(NO_MALLOC_OVERRIDE)
#ifndef _XBOX
	if ( _heapmin() != 1 )
	{
		Assert( false );
		Error( "Must include memoverride.cpp in your project." );
	}
#endif
#endif	
	// keep a record of this module name
	strncpy(g_szControlsModuleName, moduleName, sizeof(g_szControlsModuleName));
	g_szControlsModuleName[sizeof(g_szControlsModuleName) - 1] = 0;

	// initialize our locale (must be done for every vgui dll/exe)
	// "" makes it use the default locale, required to make iswprint() work correctly in different languages
	setlocale(LC_CTYPE, "");
	setlocale(LC_TIME, "");
	setlocale(LC_COLLATE, "");
	setlocale(LC_MONETARY, "");

	g_pVGuiInterface = (IVGui *)InitializeInterface( VGUI_IVGUI_INTERFACE_VERSION_GS, factoryList, numFactories );
	g_pPanelInterface = (IPanel *)InitializeInterface( VGUI_PANEL_INTERFACE_VERSION_GS, factoryList, numFactories );
	g_pSurfaceInterface = (ISurface *)InitializeInterface( VGUI_SURFACE_INTERFACE_VERSION_GS, factoryList, numFactories );
	g_pSchemeInterface = (ISchemeManager *)InitializeInterface( VGUI_SCHEME_INTERFACE_VERSION_GS, factoryList, numFactories );
	g_pSystemInterface = (ISystem *)InitializeInterface( VGUI_SYSTEM_INTERFACE_VERSION_GS, factoryList, numFactories );
	g_pInputInterface = ( IInputInternal *)InitializeInterface( VGUI_INPUT_INTERFACE_VERSION_GS, factoryList, numFactories );
	g_pLocalizeInterface = (ILocalize *)InitializeInterface( VGUI_LOCALIZE_INTERFACE_VERSION, factoryList, numFactories );
	g_pFileSystemInterface = (IFileSystem *)InitializeInterface( FILESYSTEM_INTERFACE_VERSION, factoryList, numFactories );

	g_pKeyValuesInterface = static_cast<IKeyValues*>( InitializeInterface( KEYVALUES_INTERFACE_VERSION, factoryList, numFactories ) );

	if (!g_pVGuiInterface)
		return false;

	g_pVGuiInterface->Init( factoryList, numFactories );

	if( g_pKeyValuesInterface )
		g_pKeyValuesInterface->RegisterSizeofKeyValues( sizeof( KeyValues ) );

	if ( g_pSchemeInterface && 
		 g_pSurfaceInterface && 
		 g_pSystemInterface && 
		 g_pInputInterface && 
		 g_pVGuiInterface && 
		 g_pFileSystemInterface && 
		 g_pLocalizeInterface && 
		 g_pKeyValuesInterface && 
		 g_pPanelInterface)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: returns the name of the module this has been compiled into
//-----------------------------------------------------------------------------
const char *GetControlsModuleName()
{
	return g_szControlsModuleName;
}



} // namespace vgui2



