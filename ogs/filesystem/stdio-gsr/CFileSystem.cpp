#include <algorithm>
#include <cassert>
#include <cstdarg>

#include "interface.h"

#include "characterset.h"
#include "PackFile.h"
#include "StringUtils.h"

#include "CFileSystem.h"

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

namespace fs = std::experimental::filesystem;

namespace
{
static characterset_t g_BreakSet;
static characterset_t g_BreakSetIncludingColons;

class CInitCharSets final
{
public:
	CInitCharSets()
	{
		CharacterSetBuild( &g_BreakSet, "{}()'" );
		CharacterSetBuild( &g_BreakSetIncludingColons, "{}()':" );
	}

} g_Init;

static CFileSystem g_FileSystem;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION, g_FileSystem );

void CFileSystem::RemoveAllSearchPaths()
{
	m_SearchPaths.clear();
}

void CFileSystem::AddSearchPath( const char *pPath, const char *pathID )
{
	AddSearchPath( pPath, pathID, false );
}

bool CFileSystem::RemoveSearchPath( const char *pPath )
{
	if( !pPath )
		return false;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pPath, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	auto it = FindSearchPath( szPath );

	if( it == m_SearchPaths.end() )
		return false;

	do
	{
		m_SearchPaths.erase( it );

		it = FindSearchPath( szPath );
	}
	while( it != m_SearchPaths.end() );

	return true;
}

void CFileSystem::RemoveFile( const char *pRelativePath, const char *pathID )
{
	if( !pRelativePath )
		return;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pRelativePath, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	fs::path path;

	std::error_code error;

	for( const auto& searchPath : m_SearchPaths )
	{
		if( searchPath->flags & SearchPathFlag::READ_ONLY )
			continue;

		if( pathID && ( !searchPath->HasPathID() || strcmp( pathID, searchPath->GetPathID() ) != 0 ) )
			continue;

		path = fs::path( searchPath->szPath ) / szPath;

		if( fs::remove( path, error ) )
			break;
	}
}

void CFileSystem::CreateDirHierarchy( const char *path, const char *pathID )
{
	if( !path )
		return;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, path, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	for( const auto& searchPath : m_SearchPaths )
	{
		if( searchPath->flags & SearchPathFlag::READ_ONLY )
			continue;

		if( pathID && ( !searchPath->HasPathID() || strcmp( pathID, searchPath->GetPathID() ) != 0 ) )
			continue;

		std::error_code error;

		fs::path directories = fs::path( searchPath->szPath ) / szPath;

		fs::create_directories( directories, error );

		return;
	}

	//No search paths found, so see if we need to fall back to the write path.
	if( pathID )
	{
		for( const auto& searchPath : m_SearchPaths )
		{
			if( searchPath->flags & SearchPathFlag::READ_ONLY )
				continue;

			std::error_code error;

			fs::path directories = fs::path( searchPath->szPath ) / szPath;

			fs::create_directories( directories, error );

			return;
		}
	}
}

bool CFileSystem::FileExists( const char *pFileName )
{
	if( !pFileName )
		return false;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFileName, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	fs::path path;

	std::error_code error;

	for( const auto& searchPath : m_SearchPaths )
	{
		path = fs::path( searchPath->szPath ) / szPath;

		if( fs::exists( path, error ) )
		{
			return true;
		}
	}

	return false;
}

bool CFileSystem::IsDirectory( const char *pFileName )
{
	if( !pFileName )
		return false;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFileName, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	fs::path path;

	std::error_code error;

	for( const auto& searchPath : m_SearchPaths )
	{
		path = fs::path( searchPath->szPath ) / szPath;

		if( fs::is_directory( path, error ) )
			return true;
	}

	return false;
}

FileHandle_t CFileSystem::Open( const char *pFileName, const char *pOptions, const char *pathID )
{
	if( !pFileName || !pOptions )
		return FILESYSTEM_INVALID_HANDLE;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFileName, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	if( !strchr( pOptions, 'r' ) || strchr( pOptions, '+' ) )
	{
		//Find the first write path that matches the path ID.
		//Don't need to worry about pack files here since they're always read only.

		auto it = m_SearchPaths.begin();

		for( auto end = m_SearchPaths.end(); it != end; ++it )
		{
			auto& searchPath = *it;

			if( searchPath->flags & SearchPathFlag::READ_ONLY )
				continue;

			if( pathID && ( !searchPath->HasPathID() || strcmp( pathID, searchPath->GetPathID() ) != 0 ) )
				continue;

			auto path = fs::path( searchPath->szPath ) / szPath;

			path.make_preferred();

			CFileHandle file( *this, path.u8string().c_str(), pOptions );

			if( file.IsOpen() )
			{
				m_OpenedFiles.emplace_back( std::make_unique<CFileHandle>( std::move( file ) ) );

				return reinterpret_cast<FileHandle_t>( m_OpenedFiles.back().get() );
			}

			break;
		}

		return FILESYSTEM_INVALID_HANDLE;
	}

	//Reading from a file, consider all paths.
	for( const auto& searchPath : m_SearchPaths )
	{
		if( pathID && ( !searchPath->HasPathID() || strcmp( pathID, searchPath->GetPathID() ) != 0 ) )
			continue;

		if( auto pFileHandle = FindFile( *searchPath, szPath, pOptions ) )
			return reinterpret_cast<FileHandle_t>( pFileHandle );
	}

	return FILESYSTEM_INVALID_HANDLE;
}

void CFileSystem::Close( FileHandle_t file )
{
	if( file == FILESYSTEM_INVALID_HANDLE )
		return;

	auto pFile = reinterpret_cast<CFileHandle*>( file );

	auto position = m_OpenedFiles.begin();

	for( auto end = m_OpenedFiles.end(); position != end; ++position )
	{
		if( position->get() == pFile )
			break;
	}

	if( position == m_OpenedFiles.end() )
		return;

	if( pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_REPORTALLACCESSES, "CFileSystem::Close: Closing file \"%s\"\n", pFile->GetFileName().c_str() );
		pFile->Close();
	}
	else
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Close: Closing file that was already closed, or not opened!\n" );
	}

	m_OpenedFiles.erase( position );
}

void CFileSystem::Seek( FileHandle_t file, int pos, FileSystemSeek_t seekType )
{
	Seek64( file, pos, seekType );
}

unsigned int CFileSystem::Tell( FileHandle_t file )
{
	return static_cast<unsigned int>( Tell64( file ) );
}
			 
unsigned int CFileSystem::Size( FileHandle_t file )
{
	return static_cast<unsigned int>( Size64( file ) );
}
			 
unsigned int CFileSystem::Size( const char *pFileName )
{
	return static_cast<unsigned int>( Size64( pFileName ) );
}

long CFileSystem::GetFileTime( const char *pFileName )
{
	return static_cast<long>( GetFileTimeEx( pFileName ) );
}

void CFileSystem::FileTimeToString( char* pStrip, int maxCharsIncludingTerminator, long fileTime )
{
	FileTimeToStringEx( pStrip, maxCharsIncludingTerminator, fileTime );
}

bool CFileSystem::IsOk( FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::IsOk: Attempted to check for ok null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::IsOk: Attempted to check for ok handle with null file pointer!\n" );
		return 0;
	}

	return ferror( pFile->GetFile() ) == 0;
}

void CFileSystem::Flush( FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Flush: Attempted to flush null file handle!\n" );
		return;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Flush: Attempted to flush handle with null file pointer!\n" );
		return;
	}

	fflush( pFile->GetFile() );
}

bool CFileSystem::EndOfFile( FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::EndOfFile: Attempted to check for EOF on null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::EndOfFile: Attempted to check for EOF handle with null file pointer!\n" );
		return 0;
	}

	if( pFile->IsPackEntry() )
	{
		const auto position = ftell64( pFile->GetFile() );

		return static_cast<uint64_t>( position ) >= pFile->GetStartOffset() + pFile->GetLength();
	}

	return !!feof( pFile->GetFile() );
}

int CFileSystem::Read( void* pOutput, int size, FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Read: Attempted to read from null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Read: Attempted to read from handle with null file pointer!\n" );
		return 0;
	}

	if( pFile->IsPackEntry() )
	{
		if( pFile->GetLength() == 0 )
			return 0;

		const auto position = ftell64( pFile->GetFile() );

		const auto relativePos = position - pFile->GetStartOffset();

		if( relativePos >= pFile->GetLength() )
			return 0;

		auto result = fread( pOutput, 1, size, pFile->GetFile() );

		if( relativePos + size >= pFile->GetLength() + 1 )
		{
			//Adjust the amount that was read to match the file's contents.
			const auto maxRead = static_cast<size_t>( ( pFile->GetLength() + 1 ) - relativePos );

			result = min( result, maxRead );
		}

		return result;
	}

	return fread( pOutput, 1, size, pFile->GetFile() );
}

int CFileSystem::Write( void const* pInput, int size, FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Write: Attempted to write to null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Write: Attempted to write to handle with null file pointer!\n" );
		return 0;
	}

	return fwrite( pInput, 1, size, pFile->GetFile() );
}

char *CFileSystem::ReadLine( char *pOutput, int maxChars, FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::ReadLine: Attempted to read line from null file handle!\n" );
		return nullptr;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::ReadLine: Attempted to read line from handle with null file pointer!\n" );
		return nullptr;
	}

	if( pFile->IsPackEntry() )
	{
		if( pFile->GetLength() == 0 )
			return nullptr;

		const auto position = ftell64( pFile->GetFile() );

		const auto relativePos = position - pFile->GetStartOffset();

		if( relativePos >= pFile->GetLength() )
			return nullptr;

		//The length is the number of bytes in the file, so allow that much to be read.
		if( relativePos + maxChars >= pFile->GetLength() + 1 )
		{
			maxChars = static_cast<int>( ( pFile->GetLength() + 1 ) - relativePos );
		}
	}

	return fgets( pOutput, maxChars, pFile->GetFile() );
}

int CFileSystem::FPrintf( FileHandle_t file, const char *pFormat, ... )
{
	va_list list;

	va_start( list, pFormat );

	const auto result = VFPrintf( file, pFormat, list );

	va_end( list );

	return result;
}

const char *CFileSystem::FindFirst( const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID )
{
	return FindFirstEx( pWildCard, pHandle, FileSystemFindFlag::NONE, pathID );
}

const char *CFileSystem::FindNext( FileFindHandle_t handle )
{
	if( static_cast<size_t>( handle ) >= m_FindFiles.size() )
		return nullptr;

	auto& data = *m_FindFiles[ handle ];

	if( data.flags & FindFileFlag::END_OF_DATA )
	{
		return nullptr;
	}

	do
	{
		//Reached the end of the current path ID.
		if( data.EndOfPath() )
		{
			auto path = data.currentPath != m_SearchPaths.end() ? data.currentPath + 1 : m_SearchPaths.begin();

			bool bSetNext = false;

			for( auto end = m_SearchPaths.end(); path != end; ++path )
			{
				auto& searchPath = *path;

				if( *data.szPathID && ( !searchPath->HasPathID() || strcmp( data.szPathID, searchPath->GetPathID() ) != 0 ) )
					continue;

				if( data.flags & FindFileFlag::SKIP_IDENTICAL_PATHS )
				{
					if( std::find_if( data.searchedPaths.begin(), data.searchedPaths.end(), 
							[ & ]( const char* pszPath )
							{
								return strcmp( pszPath, searchPath->szPath ) == 0;
							}
					) != data.searchedPaths.end() )
						continue;

					data.searchedPaths.emplace_back( searchPath->szPath );
				}

				data.currentPath = path;

				if( searchPath->IsPackFile() )
				{
					data.pack_iterator = searchPath->packEntries.begin();
					data.flags |= FindFileFlag::IS_PACK_FILE;
				}
				else
				{
					data.iterator = fs::recursive_directory_iterator( searchPath->szPath );
					data.flags &= ~FindFileFlag::IS_PACK_FILE;
				}

				bSetNext = true;

				break;
			}

			if( !bSetNext )
			{
				data.flags |= FindFileFlag::END_OF_DATA;
			}
		}

		if( data.flags & FindFileFlag::END_OF_DATA )
		{
			return nullptr;
		}

		auto searchPath = ( *data.currentPath ).get();

		if( searchPath->IsPackFile() )
		{
			for( auto end = searchPath->packEntries.end(); data.pack_iterator != end; )
			{
				data.szFileName = data.pack_iterator->second->GetFileName();

				++data.pack_iterator;

				//Matches the wildcard.
				if( UTIL_TokenMatches( data.szFileName.c_str(), data.szFilter.c_str() ) )
					return data.szFileName.c_str();
			}
		}
		else
		{
			for( auto end = fs::recursive_directory_iterator(); data.iterator != end; )
			{
				data.entry = *data.iterator;
				data.szFileName = data.entry.path().u8string();

				//Trim the search path so it returns uniform paths for use in wildcard searches.
				if( *searchPath->szPath )
				{
					data.szFileName = data.szFileName.substr( strlen( searchPath->szPath ) + 1 );
				}

				++data.iterator;

				//Matches the wildcard.
				if( UTIL_TokenMatches( data.szFileName.c_str(), data.szFilter.c_str() ) )
				{
					//Trim to just the filename.
					data.szFileName = data.entry.path().filename().u8string();

					return data.szFileName.c_str();
				}
			}
		}

		//The directory was completely empty or didn't have matching contents, so go to the next one.
	}
	while( true );

	//Never reached.
	return nullptr;
}

bool CFileSystem::FindIsDirectory( FileFindHandle_t handle )
{
	if( static_cast<size_t>( handle ) >= m_FindFiles.size() )
		return false;

	auto& data = *m_FindFiles[ handle ];

	if( data.flags & FindFileFlag::END_OF_DATA )
	{
		return false;
	}

	return fs::is_directory( data.entry.status() );
}

void CFileSystem::FindClose( FileFindHandle_t handle )
{
	if( static_cast<size_t>( handle ) >= m_FindFiles.size() )
		return;

	auto& data = *m_FindFiles[ handle ];

	//Already flagged, another close operation will take care of this.
	if( !( data.flags & FindFileFlag::VALID ) )
		return;

	//It's the last one, so clear it now.
	if( handle == m_FindFiles.size() - 1 )
	{
		data.flags &= ~FindFileFlag::VALID;

		//Close all invalid find handles between the end and last valid handle.
		for( auto it = m_FindFiles.rbegin(); it != m_FindFiles.rend(); )
		{
			if( data.flags & FindFileFlag::VALID )
				break;

			it = decltype( it )( m_FindFiles.erase( std::next( it ).base() ) );
		}
	}
	else
	{
		//Flag for removal.
		data.flags &= ~FindFileFlag::VALID;
	}
}

const char *CFileSystem::GetLocalPath( const char *pFileName, char *pLocalPath, int localPathBufferSize )
{
	if( !pLocalPath || localPathBufferSize <= 0 )
		return nullptr;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFileName, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	fs::path path;

	std::error_code error;

	for( const auto& searchPath : m_SearchPaths )
	{
		path = fs::path( searchPath->szPath ) / szPath;

		if( fs::exists( path, error ) )
		{
			path.make_preferred();

			strncpy( pLocalPath, path.u8string().c_str(), localPathBufferSize );
			pLocalPath[ localPathBufferSize - 1 ] = '\0';

			return pLocalPath;
		}
	}

	return nullptr;
}

//TODO: Ripped from HLE, should refactor the tokenization code and put it there. - Solokiller
char* COM_SkipWhitespace( char* pszData )
{
	if( !pszData )
		return nullptr;

	char c;

	while( ( c = *pszData ) <= ' ' )
	{
		if( c == '\0' )
			return nullptr;                    // end of file;
		++pszData;
	}

	return pszData;
}

char* COM_SkipComments( char* pszData, bool& bWasComment )
{
	if( !pszData )
	{
		bWasComment = false;
		return nullptr;
	}

	char c = *pszData;

	// skip // comments
	if( c == '/' && pszData[ 1 ] == '/' )
	{
		while( *pszData && *pszData != '\n' )
			++pszData;
		bWasComment = true;
	}
	else
		bWasComment = false;

	if( !( *pszData ) )
		return nullptr;

	return pszData;
}

char *CFileSystem::ParseFile( char* pFileBytes, char* pToken, bool* pWasQuoted )
{
	char* result = pFileBytes;

	if( pWasQuoted )
		*pWasQuoted = false;

	if( !pFileBytes )
		return nullptr;

	*pToken = '\0';

	{
		bool bWasComment;

		do
		{
			// skip whitespace
			result = COM_SkipWhitespace( result );

			if( !result )
				return nullptr;

			// skip // comments
			result = COM_SkipComments( result, bWasComment );

			if( !result )
				return nullptr;
		}
		while( bWasComment );
	}

	if( *result != '"' )
	{
		if( IN_CHARACTERSET( g_BreakSetIncludingColons, *result ) )
		{
			pToken[ 0 ] = *result;
			pToken[ 1 ] = '\0';
			++result;
		}
		else
		{
			size_t uiIndex;

			for( uiIndex = 0; *result && *result > ' ' && !IN_CHARACTERSET( g_BreakSetIncludingColons, *result ); ++uiIndex, ++result )
			{
				pToken[ uiIndex ] = *result;
			}

			pToken[ uiIndex ] = '\0';
		}

		return result;
	}

	if( pWasQuoted )
		*pWasQuoted = true;

	++result;

	if( *result != '"' )
	{
		size_t uiIndex;

		for( uiIndex = 0; *result && *result != '"'; ++uiIndex, ++result )
		{
			pToken[ uiIndex ] = *result;
		}

		//Skip closing quote.
		++result;

		pToken[ uiIndex ] = '\0';
	}
	else
	{
		pToken[ 0 ] = '\0';
	}

	return result;
}
	 
bool CFileSystem::FullPathToRelativePath( const char *pFullpath, char *pRelative )
{
	//Assume relative path is MAX_PATH large. - Solokiller
	return FullPathToRelativePathEx( pFullpath, pRelative, MAX_PATH );
}
	 
bool CFileSystem::GetCurrentDirectory( char* pDirectory, int maxlen )
{
	if( !pDirectory || maxlen <= 0 )
		return false;

	std::error_code error;

	auto path = fs::current_path( error );

	if( error )
		return false;

	const auto szCurPath = path.u8string();

	if( szCurPath.length() >= static_cast<decltype( szCurPath.length() )>( maxlen ) )
	{
		pDirectory[ 0 ] = '\0';
		return false;
	}

	strncpy( pDirectory, szCurPath.c_str(), maxlen );
	pDirectory[ maxlen - 1 ] = '\0';

	return true;
}
	 
void CFileSystem::PrintOpenedFiles()
{
	for( const auto& file : m_OpenedFiles )
	{
		const char* const pszName = !file->GetFileName().empty() ? file->GetFileName().c_str() : "???";

		Warning( FILESYSTEM_WARNING_REPORTUNCLOSED, "File %s was never closed\n", pszName );
	}
}
	 
void CFileSystem::SetWarningFunc( FileSystemWarningFunc pfnWarning )
{
	m_WarningFunc = pfnWarning;
}
	 
void CFileSystem::SetWarningLevel( FileWarningLevel_t level )
{
	m_WarningLevel = level;
}

int CFileSystem::SetVBuf( FileHandle_t stream, char *buffer, int mode, long size )
{
	auto pFile = reinterpret_cast<CFileHandle*>( stream );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::SetVBuf: Attempted to set VBuf for null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::SetVBuf: Attempted to set VBuf for handle with null file pointer!\n" );
		return 0;
	}

	return setvbuf( pFile->GetFile(), buffer, mode, size );
}

void CFileSystem::GetInterfaceVersion( char *p, int maxlen )
{
	if( maxlen <= 0 )
		return;

	strncpy( p, "Stdio", maxlen );
	p[ maxlen - 1 ] = '\0';
}

template<typename PackType>
bool ProcessPackFile( CFileSystem& fileSystem, const char* pszFileName, FILE* pFile, CSearchPath::Entries_t& entries )
{
	assert( pFile );

	typename PackType::Header_t header;

	if( fread( &header, sizeof( header ), 1, pFile ) != 1 )
	{
		fileSystem.Warning( FILESYSTEM_WARNING_CRITICAL, "ProcessPackFile(%s): Couldn't read pack file \"%s\" header!n", PackType::Info_t::NAME, pszFileName );
		return false;
	}

	header.dirofs = header.dirofs;
	header.dirlen = header.dirlen;

	if( ( header.dirlen % sizeof( typename PackType::Entry_t ) ) != 0 )
	{
		fileSystem.Warning( FILESYSTEM_WARNING_CRITICAL, "ProcessPackFile(%s): Invalid directory length for \"%s\"\n", PackType::Info_t::NAME, pszFileName );
		return false;
	}

	const size_t numFiles = static_cast<size_t>( header.dirlen / sizeof( typename PackType::Entry_t ) );

	if( numFiles > PackType::MAX_FILES )
	{
		fileSystem.Warning( FILESYSTEM_WARNING_CRITICAL, "ProcessPackFile(%s): Too many files in pack file \"%s\" (Max %u, got %u)\n", 
							PackType::Info_t::NAME, pszFileName, PackType::MAX_FILES, numFiles );
		return false;
	}

	fseek64( pFile, header.dirofs, SEEK_SET );

	auto packEntries = std::make_unique<typename PackType::Entry_t[]>( numFiles );

	if( fread( packEntries.get(), sizeof( typename PackType::Entry_t ), numFiles, pFile ) != numFiles )
	{
		fileSystem.Warning( FILESYSTEM_WARNING_CRITICAL, "ProcessPackFile(%s): Couldn't read directory entries from \"%s\"\n", PackType::Info_t::NAME, pszFileName );
		return false;
	}

	for( size_t uiIndex = 0; uiIndex < numFiles; ++uiIndex )
	{
		auto& packEntry = packEntries[ uiIndex ];

		auto entry = std::make_unique<CPackFileEntry>( std::move( fs::path( packEntry.szFileName ).make_preferred().u8string() ), packEntry.filepos, packEntry.filelen );

		entries.emplace( std::make_pair( entry->GetFileName().c_str(), std::move( entry ) ) );
	}

	return true;
}

bool CFileSystem::AddPackFile( const char *fullpath, const char *pathID )
{
	return AddPackFile( fullpath, pathID, true );
}

FileHandle_t CFileSystem::OpenFromCacheForRead( const char *pFileName, const char *pOptions, const char *pathID )
{
	if( !pFileName || !pOptions )
		return FILESYSTEM_INVALID_HANDLE;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFileName, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	if( strchr( pOptions, 'w' ) || strchr( pOptions, '+' ) )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::OpenFromCacheForRead: Tried to open file \"%s\" with write option!\n", szPath );
		return FILESYSTEM_INVALID_HANDLE;
	}

	for( const auto& searchPath : m_SearchPaths )
	{
		if( !( searchPath->flags & SearchPathFlag::IS_PACK_FILE ) )
			continue;

		if( pathID && ( !searchPath->HasPathID() || strcmp( pathID, searchPath->GetPathID() ) != 0 ) )
			continue;

		if( auto pFileHandle = FindFile( *searchPath, szPath, pOptions ) )
			return reinterpret_cast<FileHandle_t>( pFileHandle );
	}

	return FILESYSTEM_INVALID_HANDLE;
}

void CFileSystem::AddSearchPathNoWrite( const char *pPath, const char *pathID )
{
	AddSearchPath( pPath, pathID, true );
}

void CFileSystem::Seek64( FileHandle_t file, int64_t pos, FileSystemSeek_t seekType )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Seek: Attempted to seek null file handle!\n" );
		return;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Seek: Attempted to seek handle with null file pointer!\n" );
		return;
	}

	int64_t position = pos;

	if( pFile->IsPackEntry() )
	{
		switch( seekType )
		{
		case FILESYSTEM_SEEK_HEAD:
			{
				position += pFile->GetStartOffset();
				break;
			}

		case FILESYSTEM_SEEK_TAIL:
			{
				//Adjust end position to the file's end
				position += pFile->GetStartOffset() + pFile->GetLength();
				seekType = FILESYSTEM_SEEK_HEAD;
				break;
			}
		}
	}

	int origin;

	switch( seekType )
	{
	case FILESYSTEM_SEEK_HEAD:		origin = SEEK_SET; break;
	case FILESYSTEM_SEEK_CURRENT:	origin = SEEK_CUR; break;
	case FILESYSTEM_SEEK_TAIL:		origin = SEEK_END; break;
	default:
		{
			Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Seek: invalid seek type '%d'\n", seekType );
			return;
		}
	}

	fseek64( pFile->GetFile(), position, origin );
}

uint64_t CFileSystem::Tell64( FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Tell: Attempted to tell null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Tell: Attempted to tell handle with null file pointer!\n" );
		return 0;
	}

	if( pFile->IsPackEntry() )
	{
		const auto position = ftell64( pFile->GetFile() );

		return position - pFile->GetStartOffset();
	}

	return ftell64( pFile->GetFile() );
}

uint64_t CFileSystem::Size64( FileHandle_t file )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Size: Attempted to size null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::Size: Attempted to size handle with null file pointer!\n" );
		return 0;
	}

	return pFile->GetLength();
}

uint64_t CFileSystem::Size64( const char *pFileName )
{
	if( !pFileName )
		return -1;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFileName, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	std::error_code error;

	return fs::file_size( szPath, error );
}

int64_t CFileSystem::GetFileTimeEx( const char *pFileName )
{
	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFileName, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	fs::path path;

	std::error_code error;

	for( const auto& searchPath : m_SearchPaths )
	{
		if( searchPath->IsPackFile() )
		{
			auto it = searchPath->packEntries.find( szPath );

			if( it != searchPath->packEntries.end() )
			{
				//Use the pack file's file time. - Solokiller
				return std::chrono::duration_cast<std::chrono::seconds>( fs::last_write_time( searchPath->szPath, error ).time_since_epoch() ).count();
			}
		}
		else
		{
			path = fs::path( searchPath->szPath ) / szPath;

			if( fs::exists( path, error ) )
			{
				//Don't cast to int64_t here so we get a warning if it's incompatible. - Solokiller
				//Cast to seconds since the write time is returned in different format. - Solokiller
				return std::chrono::duration_cast<std::chrono::seconds>( fs::last_write_time( path, error ).time_since_epoch() ).count();
			}
		}
	}

	return 0;
}

void CFileSystem::FileTimeToStringEx( char* pStrip, int maxCharsIncludingTerminator, int64_t fileTime )
{
	time_t time = fileTime;
	auto pszResult = ctime( &time );

	strncpy( pStrip, pszResult, maxCharsIncludingTerminator );
	pStrip[ maxCharsIncludingTerminator - 1 ] = '\0';
}

int CFileSystem::VFPrintf( FileHandle_t file, const char *pFormat, va_list list )
{
	auto pFile = reinterpret_cast<CFileHandle*>( file );

	if( !pFile )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::FPrint: Attempted to format print to null file handle!\n" );
		return 0;
	}

	if( !pFile->IsOpen() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::FPrintf: Attempted to format print to handle with null file pointer!\n" );
		return 0;
	}

	const auto result = vfprintf( pFile->GetFile(), pFormat, list );

	return result;
}

const char *CFileSystem::FindFirstEx( const char *pWildCard, FileFindHandle_t *pHandle, FileSystemFindFlags_t flags, const char *pathID )
{
	if( !pWildCard || !pHandle )
		return nullptr;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pWildCard, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	FindFileData data;

	//Note: Not the same constant! - Solokiller
	if( flags & FileSystemFindFlag::SKIP_IDENTICAL_PATHS )
		data.flags |= FindFileFlag::SKIP_IDENTICAL_PATHS;

	data.szFilter = std::move( fs::path( szPath ).make_preferred().u8string() );

	if( pathID )
	{
		strncpy( data.szPathID, pathID, sizeof( data.szPathID ) );
		data.szPathID[ sizeof( data.szPathID ) - 1 ] = '\0';
	}
	else
	{
		data.szPathID[ 0 ] = '\0';
	}

	data.currentPath = m_SearchPaths.end();

	m_FindFiles.emplace_back( std::make_unique<FindFileData>( std::move( data ) ) );

	*pHandle = m_FindFiles.size() - 1;

	if( auto pszFileName = FindNext( *pHandle ) )
		return pszFileName;

	//Nothing found.
	FindClose( *pHandle );

	*pHandle = FILESYSTEM_INVALID_FIND_HANDLE;

	return nullptr;
}

bool CFileSystem::FullPathToRelativePathEx( const char *pFullpath, char *pRelative, size_t uiSizeInChars )
{
	if( !pRelative || uiSizeInChars <= 0 )
		return false;

	if( !pFullpath || !( *pFullpath ) )
	{
		*pRelative = '\0';
		return false;
	}

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pFullpath, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	std::string szFullpath = fs::path( szPath ).make_preferred().u8string();

	for( auto it = m_SearchPaths.begin(), end = m_SearchPaths.end(); it != end; ++it )
	{
		const char* const pszPath = it->get()->szPath;

		const auto uiLength = strlen( pszPath );

		if( strnicmp( szFullpath.c_str(), pszPath, uiLength ) == 0 )
		{
			//If the input path is identical to the search path, this can happen.
			//This should only be used to get relative paths to files and perhaps directories, so it's rather useless to return empty paths.
			if( szFullpath.length() > uiLength )
			{
				//Skip the slash that separates the search path from the relative path.
				strncpy( pRelative, &szFullpath[ uiLength + 1 ], uiSizeInChars );
				pRelative[ uiSizeInChars - 1 ] = '\0';
				return true;
			}
		}
	}

	return false;
}

void CFileSystem::Warning( FileWarningLevel_t level, const char* pszFormat, ... )
{
	char szBuffer[ 4096 ];

	if( level <= m_WarningLevel )
	{
		va_list list;

		va_start( list, pszFormat );

		vsnprintf( szBuffer, sizeof( szBuffer ), pszFormat, list );

		if( m_WarningFunc )
			m_WarningFunc( "%s", szBuffer );
		else
			fprintf( stderr, "%s", szBuffer );

		va_end( list );
	}
}

CFileSystem::SearchPaths_t::const_iterator CFileSystem::FindSearchPath( const char* pszPath, const bool bCheckPathID, const char* pszPathID ) const
{
	fs::path path( pszPath );

	std::error_code error;

	path = fs::canonical( path, error );

	if( error )
		return m_SearchPaths.end();

	const auto szPath = path.u8string();

	for( auto it = m_SearchPaths.begin(), end = m_SearchPaths.end(); it != end; ++it )
	{
		if( stricmp( szPath.c_str(), ( *it )->szPath ) == 0 )
		{
			if( !bCheckPathID ||
				( ( pszPathID == nullptr && !( *it )->HasPathID() ) ||
				( ( pszPathID != nullptr && ( *it )->HasPathID() ) && strcmp( pszPathID, ( *it )->GetPathID() ) == 0 ) ) )
				return it;
		}
	}

	return m_SearchPaths.end();
}

bool CFileSystem::AddSearchPath( const char *pPath, const char *pathID, const bool bReadOnly )
{
	if( !pPath )
	{
		return false;
	}

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pPath, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	//Despite the documentation stating that bsp files are supported, the filesystem doesn't actually support it. Sorry. - Solokiller
	if( strstr( szPath, ".bsp" ) )
		return false;

	if( FindSearchPath( szPath, true, pathID ) != m_SearchPaths.end() )
		return false;

	fs::path osPath( szPath );

	//Convert all search paths to the absolute representation, fully resolved.
	std::error_code error;

	osPath = fs::canonical( osPath, error );

	//Don't want to add empty paths when non-existent search paths are added.
	if( error || osPath.empty() )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::AddSearchPath: Couldn't convert path \"%s\" to its canonial representation\n", szPath );
		return false;
	}

	osPath.make_preferred();

	auto path = std::make_unique<CSearchPath>();

	strncpy( path->szPath, osPath.u8string().c_str(), sizeof( path->szPath ) );
	path->szPath[ sizeof( path->szPath ) - 1 ] = '\0';

	strncpy( path->szPathID, pathID, ARRAYSIZE( path->szPathID ) );
	path->szPathID[ ARRAYSIZE( path->szPathID ) - 1 ] = '\0';

	path->flags = SearchPathFlag::NONE;

	if( bReadOnly )
		path->flags |= SearchPathFlag::READ_ONLY;

	m_SearchPaths.emplace_back( std::move( path ) );

	//Add any game pack files present in the path.
	AddPackFiles( m_SearchPaths.back()->szPath );

	return true;
}

bool CFileSystem::PreparePackFile( const char* pszFullPath, const char* pszPathID, CFileHandle& file, int64_t offset )
{
	fseek64( file.GetFile(), file.GetStartOffset() + offset, SEEK_SET );

	pack::PackType type;

	{
		pack::Header_t header;

		if( fread( &header, sizeof( header ), 1, file.GetFile() ) != 1 )
		{
			Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::AddPackFile: Couldn't read pack file \"%s\" identifier\n", pszFullPath );
			return false;
		}

		type = pack::IdentifyPackType( header );
	}

	if( type == pack::PackType::NOT_A_PACK )
	{
		Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::AddPackFile: \"%s\" is not a pack file\n", pszFullPath );
		return false;
	}

	fseek64( file.GetFile(), file.GetStartOffset() + offset, SEEK_SET );

	CSearchPath::Entries_t entries;

	bool bSuccess = false;

	switch( type )
	{
	case pack::PackType::PACK_32BIT:	bSuccess = ProcessPackFile<pack::Pack32_t>( *this, pszFullPath, file.GetFile(), entries ); break;
	case pack::PackType::PACK_64BIT:	bSuccess = ProcessPackFile<pack::Pack64_t>( *this, pszFullPath, file.GetFile(), entries ); break;
	}

	if( !bSuccess )
	{
		return false;
	}

	auto path = std::make_unique<CSearchPath>();

	fs::path osPath( pszFullPath );

	osPath.make_preferred();

	strncpy( path->szPath, osPath.u8string().c_str(), sizeof( path->szPath ) );
	path->szPath[ sizeof( path->szPath ) - 1 ] = '\0';

	strncpy( path->szPathID, pszPathID, ARRAYSIZE( path->szPathID ) );
	path->szPathID[ ARRAYSIZE( path->szPathID ) - 1 ] = '\0';

	path->flags = SearchPathFlag::READ_ONLY | SearchPathFlag::IS_PACK_FILE;

	path->packFile = std::make_unique<CFileHandle>( std::move( file ) );

	path->packEntries = std::move( entries );

	m_SearchPaths.emplace_back( std::move( path ) );

	return true;
}

bool CFileSystem::AddPackFile( const char* pszFullPath, const char* pszPathID, const bool bCheckForAppendPack )
{
	if( !pszFullPath )
		return false;

	char szPath[ MAX_PATH ];

	UTIL_SafeStrncpy( szPath, pszFullPath, sizeof( szPath ) );

	UTIL_FixSlashes( szPath );

	CFileHandle file( *this, szPath, "rb", true );

	if( !file.IsOpen() )
		return false;

	int64_t iOffset = 0;

	if( bCheckForAppendPack )
	{
		pack::PackAppend_t header;

		fseek64( file.GetFile(), -static_cast<int64_t>( sizeof( header ) ), SEEK_END );

		if( fread( &header, sizeof( header ), 1, file.GetFile() ) != 1 )
		{
			Warning( FILESYSTEM_WARNING_CRITICAL, "CFileSystem::AddPackFile: Failed to read pack append header for pack file \"%s\"!\n", szPath );
			return false;
		}

		if( pack::IsAppendPack( header ) )
		{
			iOffset = header.packheaderpos;
		}
	}

	return PreparePackFile( szPath, pszPathID, file, iOffset );
}

void CFileSystem::AddPackFiles( const char* pszPath )
{
	char szPath[ MAX_PATH ];

	std::error_code error;

	for( size_t uiPakIndex = 0; ; ++uiPakIndex )
	{
		snprintf( szPath, sizeof( szPath ), "%s/pak%u.pak", pszPath, uiPakIndex );

		if( !fs::exists( szPath, error ) )
			break;

		AddPackFile( szPath, "", false );
	}
}

CFileHandle* CFileSystem::FindFile( CSearchPath& searchPath, const char* pszFileName, const char* pszOptions )
{
	std::unique_ptr<CFileHandle> file;
	
	if( searchPath.IsPackFile() )
	{
		auto path = fs::path( pszFileName );

		path.make_preferred();

		auto szFileName = path.u8string();

		auto it = searchPath.packEntries.find( szFileName.c_str() );

		if( it == searchPath.packEntries.end() )
			return nullptr;

		auto& entry = *( it->second );

		file = std::make_unique<CFileHandle>( *this, std::move( path.u8string() ), searchPath.packFile->GetFile(), entry.GetStartOffset(), entry.GetLength() );
	}
	else
	{
		auto path = fs::path( searchPath.szPath ) / pszFileName;

		path.make_preferred();

		file = std::make_unique<CFileHandle>( *this, path.u8string().c_str(), pszOptions );
	}

	if( file->IsOpen() )
	{
		m_OpenedFiles.emplace_back( std::move( file ) );

		return m_OpenedFiles.back().get();
	}

	return nullptr;
}
