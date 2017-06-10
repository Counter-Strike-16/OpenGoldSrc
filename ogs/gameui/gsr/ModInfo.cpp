#include <tier0/platform.h>
#include <KeyValues.h>
#include <strtools.h>

#include <FileSystem.h>
#include <vgui_controls/Controls.h>

#include "EngineInterface.h"
#include "ModInfo.h"

CModInfo& ModInfo()
{
	static CModInfo s_ModInfo;
	return s_ModInfo;
}

CModInfo::CModInfo()
	: m_pModData( new KeyValues( "ModData" ) )
{
}

CModInfo::~CModInfo()
{
	//Nothing
}

void CModInfo::FreeModInfo()
{
	if( m_pModData )
	{
		m_pModData->deleteThis();
		m_pModData = nullptr;
	}
}

const char* CModInfo::GetStartMap()
{
	return m_pModData->GetString( "startmap", "c0a0" );
}

const char* CModInfo::GetTrainMap()
{
	return m_pModData->GetString( "trainmap", "t0a0" );
}

bool CModInfo::IsMultiplayerOnly()
{
	return !stricmp( m_pModData->GetString( "type" ), "multiplayer_only" );
}

bool CModInfo::IsSinglePlayerOnly()
{
	return !stricmp( m_pModData->GetString( "type" ), "singleplayer_only" );
}

bool CModInfo::BShowSimpleLoadingDialog()
{
	if( !stricmp( m_pModData->GetString( "type" ), "singleplayer_only" ) )
		return true;

	return !stricmp( m_pModData->GetString( "game", "0" ), "Opposing Force" );
}

bool CModInfo::NoModels()
{
	return !stricmp( m_pModData->GetString( "nomodels", "0" ), "1" );
}

bool CModInfo::NoHiModel()
{
	return !stricmp( m_pModData->GetString( "nohimodel", "0" ), "1" );
}

const char* CModInfo::GetFallbackDir()
{
	return m_pModData->GetString( "fallback_dir" );
}

bool CModInfo::UseFallbackDirMaps()
{
	return !stricmp( m_pModData->GetString( "fallback_maps", "1" ), "1" );
}

bool CModInfo::GetDetailedTexture()
{
	return !stricmp( m_pModData->GetString( "detailed_textures", "0" ), "1" );
}

const char* CModInfo::GetGameDescription()
{
	return m_pModData->GetString( "game", "Half-Life" );
}

const char* CModInfo::GetMPFilter()
{
	return m_pModData->GetString( "mpfilter" );
}

void CModInfo::LoadCurrentGameInfo()
{
	auto hFile = vgui2::filesystem()->Open( "liblist.gam", "rb" );

	if( FILESYSTEM_INVALID_HANDLE != hFile )
	{
		auto size = vgui2::filesystem()->Size( hFile );

		if( size > 0 )
		{
			//Align to 16 byte boundary, include null terminator.
			auto pszBuffer = reinterpret_cast<char*>( stackalloc( size + 15 + 1 ) );

			auto pszData = AlignValue( pszBuffer, 16 );

			vgui2::filesystem()->Read( pszData, size, hFile );

			pszData[ size ] = '\0';

			LoadGameInfoFromBuffer( pszData );

			stackfree( pszBuffer );
		}

		vgui2::filesystem()->Close( hFile );
	}
}

void CModInfo::LoadGameInfoFromBuffer( const char* buffer )
{
	char token[ 1024 ];
	char key[ 256 ];
	char value[ 256 ];

	auto pszData = const_cast<char*>( buffer );

	while( pszData )
	{
		pszData = engine->COM_ParseFile( pszData, token );

		if( !token[ 0 ] )
			break;

		strcpy( key, token );

		pszData = engine->COM_ParseFile( pszData, token );
		strcpy( value, token );

		m_pModData->SetString( key, value );
	}
}
