#ifndef FILESYSTEM_CFILESYSTEM_H
#define FILESYSTEM_CFILESYSTEM_H

#include <cstdint>
#include <cstdio>
#include <experimental/filesystem>
#include <memory>
#include <string>
#include <vector>

#include "tier0/platform.h"

#include "CFileHandle.h"
#include "CSearchPath.h"

#include "FileSystem.h"

#undef SearchPath

typedef uint32_t FileSystemFindFlags_t;

namespace FileSystemFindFlag
{
enum FileSystemFindFlag : FileSystemFindFlags_t
{
	NONE = 0,

	/**
	*	Skip identical search paths.
	*	Different path IDs can have the same search paths, which can result in the same file being returned multiple times.
	*/
	SKIP_IDENTICAL_PATHS = 1 << 0,
};
}

class CFileSystem : public IFileSystem
{
private:
	typedef std::vector<std::unique_ptr<CSearchPath>> SearchPaths_t;

	typedef uint32_t FindFileFlags_t;

	struct FindFileFlag
	{
		enum : FindFileFlags_t
		{
			NONE					= 0,
			END_OF_DATA				= 1 << 0,
			VALID					= 1 << 1,
			IS_PACK_FILE			= 1 << 2,
			SKIP_IDENTICAL_PATHS	= 1 << 3,
		};
	};

	struct FindFileData
	{
		bool EndOfPath() const
		{
			if( flags & FindFileFlag::IS_PACK_FILE )
			{
				return pack_iterator == currentPath->get()->packEntries.end();
			}
			else
			{
				return iterator == std::experimental::filesystem::recursive_directory_iterator();
			}
		}

		std::experimental::filesystem::recursive_directory_iterator iterator;
		//For pack search paths: the index of the current pack file entry.
		CSearchPath::Entries_t::const_iterator pack_iterator;

		std::experimental::filesystem::directory_entry entry;
		std::string szFileName;
		std::string szFilter;

		char szPathID[ MAX_PATH ];

		SearchPaths_t::const_iterator currentPath;

		FindFileFlags_t flags = FindFileFlag::VALID;

		std::vector<const char*> searchedPaths;
	};

	typedef std::vector<std::unique_ptr<CFileHandle>> OpenedFiles_t;
	typedef std::vector<std::unique_ptr<FindFileData>> FindFiles_t;

public:
	CFileSystem() = default;

	void			Mount() override;

	void			Unmount() override;

	void			RemoveAllSearchPaths() override;

	void			AddSearchPath( const char *pPath, const char *pathID ) override;

	bool			RemoveSearchPath( const char *pPath ) override;

	void			RemoveFile( const char *pRelativePath, const char *pathID ) override;

	void			CreateDirHierarchy( const char *path, const char *pathID ) override;

	// File I/O and info
	bool			FileExists( const char *pFileName ) override;

	bool			IsDirectory( const char *pFileName ) override;

	FileHandle_t	Open( const char *pFileName, const char *pOptions, const char *pathID = nullptr ) override;

	void			Close( FileHandle_t file ) override;

	void			Seek( FileHandle_t file, int pos, FileSystemSeek_t seekType ) override;

	unsigned int	Tell( FileHandle_t file ) override;

	unsigned int	Size( FileHandle_t file ) override;

	unsigned int	Size( const char *pFileName ) override;

	long			GetFileTime( const char *pFileName ) override;

	void			FileTimeToString( char* pStrip, int maxCharsIncludingTerminator, long fileTime ) override;

	bool			IsOk( FileHandle_t file ) override;

	void			Flush( FileHandle_t file ) override;

	bool			EndOfFile( FileHandle_t file ) override;

	int				Read( void* pOutput, int size, FileHandle_t file ) override;

	int				Write( void const* pInput, int size, FileHandle_t file ) override;

	char			*ReadLine( char *pOutput, int maxChars, FileHandle_t file ) override;

	int				FPrintf( FileHandle_t file, const char *pFormat, ... ) override;

	// direct filesystem buffer access
	void			*GetReadBuffer( FileHandle_t file, int *outBufferSize, bool failIfNotInCache ) override;

	void            ReleaseReadBuffer( FileHandle_t file, void *readBuffer ) override;

	// FindFirst/FindNext
	const char		*FindFirst( const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID = nullptr ) override;

	const char		*FindNext( FileFindHandle_t handle ) override;

	bool			FindIsDirectory( FileFindHandle_t handle ) override;

	void			FindClose( FileFindHandle_t handle ) override;

	void			GetLocalCopy( const char *pFileName ) override;

	const char		*GetLocalPath( const char *pFileName, char *pLocalPath, int localPathBufferSize ) override;

	char			*ParseFile( char* pFileBytes, char* pToken, bool* pWasQuoted ) override;

	bool			FullPathToRelativePath( const char *pFullpath, char *pRelative ) override;

	bool			GetCurrentDirectory( char* pDirectory, int maxlen ) override;

	void			PrintOpenedFiles( void ) override;

	void			SetWarningFunc( FileSystemWarningFunc pfnWarning ) override;

	void			SetWarningLevel( FileWarningLevel_t level ) override;

	void			LogLevelLoadStarted( const char *name ) override;

	void			LogLevelLoadFinished( const char *name ) override;

	int				HintResourceNeed( const char *hintlist, int forgetEverything ) override;

	int				PauseResourcePreloading( void ) override;

	int				ResumeResourcePreloading( void ) override;

	int				SetVBuf( FileHandle_t stream, char *buffer, int mode, long size ) override;

	void			GetInterfaceVersion( char *p, int maxlen ) override;

	bool			IsFileImmediatelyAvailable( const char *pFileName ) override;

	WaitForResourcesHandle_t WaitForResources( const char *resourcelist ) override;

	bool			GetWaitForResourcesProgress( WaitForResourcesHandle_t handle, float *progress /* out */, bool *complete /* out */ ) override;

	void			CancelWaitForResources( WaitForResourcesHandle_t handle ) override;

	bool			IsAppReadyForOfflinePlay( int appID ) override;

	bool			AddPackFile( const char *fullpath, const char *pathID ) override;

	FileHandle_t	OpenFromCacheForRead( const char *pFileName, const char *pOptions, const char *pathID = nullptr ) override;

	void			AddSearchPathNoWrite( const char *pPath, const char *pathID ) override;

	//IFileSystem2

	void			Seek64( FileHandle_t file, int64_t pos, FileSystemSeek_t seekType );

	uint64_t		Tell64( FileHandle_t file );

	uint64_t		Size64( FileHandle_t file );

	uint64_t		Size64( const char *pFileName );

	int64_t			GetFileTimeEx( const char *pFileName );

	void			FileTimeToStringEx( char* pStrip, int maxCharsIncludingTerminator, int64_t fileTime );

	int				VFPrintf( FileHandle_t file, const char *pFormat, va_list list );

	const char		*FindFirstEx( const char *pWildCard, FileFindHandle_t *pHandle, FileSystemFindFlags_t flags, const char *pathID = nullptr );

	bool			FullPathToRelativePathEx( const char *pFullpath, char *pRelative, size_t uiSizeInChars );

	//CFileSystem

	void Warning( FileWarningLevel_t level, const char* pszFormat, ... );

private:

	SearchPaths_t::const_iterator FindSearchPath( const char* pszPath, const bool bCheckPathID = false, const char* pszPathID = nullptr ) const;

	bool AddSearchPath( const char *pPath, const char *pathID, const bool bReadOnly );

	bool PreparePackFile( const char* pszFullPath, const char* pszPathID, CFileHandle& file, int64_t offset );

	bool AddPackFile( const char* pszFullPath, const char* pszPathID, const bool bCheckForAppendPack );

	void AddPackFiles( const char* pszPath );

	CFileHandle* FindFile( CSearchPath& searchPath, const char* pszFileName, const char* pszOptions );

private:
	SearchPaths_t m_SearchPaths;
	OpenedFiles_t m_OpenedFiles;
	FindFiles_t m_FindFiles;

	FileSystemWarningFunc m_WarningFunc = nullptr;

	FileWarningLevel_t m_WarningLevel = FileWarningLevel_t::FILESYSTEM_WARNING_REPORTUNCLOSED;

private:
	CFileSystem( const CFileSystem& ) = delete;
	CFileSystem& operator=( const CFileSystem& ) = delete;
};

#endif //FILESYSTEM_CFILESYSTEM_H
