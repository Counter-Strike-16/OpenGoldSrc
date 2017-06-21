#ifndef FILESYSTEM_CFILEHANDLE_H
#define FILESYSTEM_CFILEHANDLE_H

#include <cstdio>
#include <cstdint>
#include <string>

class CFileSystem;

typedef uint32_t FileHandleFlags_t;

namespace FileHandleFlag
{
enum : FileHandleFlags_t
{
	NONE			= 0,
	/**
	*	This is a pack file.
	*/
	IS_PACK_FILE	= 1 << 0,

	/**
	*	This is a file in a pack file.
	*/
	IS_PACK_ENTRY	= 1 << 1,
};
}

/**
*	A handle to a file. Contains information about the file, such as the name.
*/
class CFileHandle
{
public:
	/**
	*	Constructs a handle that points to no file.
	*/
	CFileHandle() = default;

	/**
	*	Constructs a handle that points to a file with the given name. The file is opened with the given mode.
	*/
	CFileHandle( CFileSystem& fileSystem, const char* pszFileName, const char* pszMode, const bool bIsPackFile = false );

	/**
	*	Constructs a handle that points to a file with the given name within the given file. The given offset and length are used.
	*/
	CFileHandle( CFileSystem& fileSystem, std::string&& szFileName, FILE* pFile, uint64_t uiStartOffset, uint64_t uiLength );

	CFileHandle( CFileHandle&& other );
	CFileHandle& operator=( CFileHandle&& other );

	~CFileHandle()
	{
		Close();
	}

	inline FILE* GetFile() { return m_pFile; }

	inline const std::string& GetFileName() const { return m_szFileName; }

	inline uint64_t GetStartOffset() const { return m_uiStartOffset; }

	inline uint64_t GetLength() const { return m_uiLength; }

	inline FileHandleFlags_t GetFlags() const { return m_Flags; }

	void SetFlags( const FileHandleFlags_t flags );

	void ClearFlags( const FileHandleFlags_t flags );

	inline bool IsPackFile() const { return ( m_Flags & FileHandleFlag::IS_PACK_FILE ) != 0; }

	inline bool IsPackEntry() const { return ( m_Flags & FileHandleFlag::IS_PACK_ENTRY ) != 0; }

	bool IsOpen() const;

	void Close();

	bool operator==( const CFileHandle& other ) const;

	bool operator!=( const CFileHandle& other ) const { return !( *this == other ); }

	void swap( CFileHandle& other );

private:
	FILE* m_pFile = nullptr;

	std::string m_szFileName;

	uint64_t m_uiStartOffset = 0;
	uint64_t m_uiLength = 0;

	FileHandleFlags_t m_Flags = FileHandleFlag::NONE;

private:
	CFileHandle( const CFileHandle& ) = delete;
	CFileHandle& operator=( const CFileHandle& ) = delete;
};

inline void CFileHandle::SetFlags( const FileHandleFlags_t flags )
{
	m_Flags |= flags;
}

inline void CFileHandle::ClearFlags( const FileHandleFlags_t flags )
{
	m_Flags &= ~flags;
}

inline bool CFileHandle::IsOpen() const
{
	return m_pFile != nullptr;
}

inline bool CFileHandle::operator==( const CFileHandle& other ) const
{
	return m_pFile == other.m_pFile;
}

#endif //FILESYSTEM_CFILEHANDLE_H
