#include <cassert>

#include "CFileSystem.h"

#include "CFileHandle.h"

//TODO: move - Solokiller
#ifdef WIN32
//64 bit fopen support. - Solokiller
#define fopen64 fopen
#define fseek64 _fseeki64
#define ftell64 _ftelli64
#else
#define fseek64 fseeko64
#define ftell64 ftello64
#endif

CFileHandle::CFileHandle( CFileSystem& fileSystem, const char* pszFileName, const char* pszMode, const bool bIsPackFile )
{
	assert( pszFileName );
	assert( pszMode );

	m_pFile = fopen64( pszFileName, pszMode );

	if( m_pFile )
	{
		m_szFileName = pszFileName;

		struct stat buffer{};

		if( stat( pszFileName, &buffer ) == -1 )
		{
			fileSystem.Warning( FILESYSTEM_WARNING_CRITICAL, "CFileHandle::CFileHandle: Couldn't stat file \"%s\" that was opened with mode \"%s\"!\n", pszFileName, pszMode );
		}

		m_uiLength = buffer.st_size;

		if( bIsPackFile )
			SetFlags( FileHandleFlag::IS_PACK_FILE );
	}
}

CFileHandle::CFileHandle( CFileSystem& fileSystem, std::string&& szFileName, FILE* pFile, uint64_t uiStartOffset, uint64_t uiLength )
{
	assert( pFile );

	m_pFile = pFile;

	if( m_pFile )
	{
		m_szFileName = std::move( szFileName );

		m_uiStartOffset = uiStartOffset;
		m_uiLength = uiLength;

		m_Flags |= FileHandleFlag::IS_PACK_ENTRY;

		//TODO: use 64 bit seek - Solokiller
		fseek( m_pFile, static_cast<long>( m_uiStartOffset ), SEEK_SET );
	}
	else
	{
		fileSystem.Warning( FILESYSTEM_WARNING_CRITICAL, "CFileHandle::CFileHandle: Null FILE* for pack file entry \"%s\"!\n", szFileName.c_str() );
	}
}

CFileHandle::CFileHandle( CFileHandle&& other )
	: CFileHandle()
{
	swap( other );
}

CFileHandle& CFileHandle::operator=( CFileHandle&& other )
{
	if( this != &other )
	{
		Close();

		swap( other );
	}

	return *this;
}

void CFileHandle::Close()
{
	if( IsOpen() )
	{
		if( !IsPackEntry() )
		{
			fclose( m_pFile );
		}

		m_pFile = nullptr;
		m_szFileName.clear();

		m_uiStartOffset = m_uiLength = 0;

		m_Flags = FileHandleFlag::NONE;
	}
}

void CFileHandle::swap( CFileHandle& other )
{
	if( this != &other )
	{
		std::swap( m_pFile, other.m_pFile );
		std::swap( m_szFileName, other.m_szFileName );
		std::swap( m_uiStartOffset, other.m_uiStartOffset );
		std::swap( m_uiLength, other.m_uiLength );
		std::swap( m_Flags, other.m_Flags );
	}
}
