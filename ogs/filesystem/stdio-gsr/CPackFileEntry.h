#ifndef FILESYSTEM_CPACKFILEENTRY_H
#define FILESYSTEM_CPACKFILEENTRY_H

#include <cstdint>
#include <string>

/**
*	Contains information about a file inside a pack file.
*/
class CPackFileEntry
{
public:
	CPackFileEntry( const char* pszFileName, uint64_t uiStartOffset, uint64_t uiLength );

	CPackFileEntry( std::string&& szFileName, uint64_t uiStartOffset, uint64_t uiLength );

	CPackFileEntry( CPackFileEntry&& other ) = default;
	CPackFileEntry& operator=( CPackFileEntry&& other ) = default;

	inline const std::string& GetFileName() const { return m_szFileName; }

	inline uint64_t GetStartOffset() const { return m_uiStartOffset; }

	inline uint64_t GetLength() const { return m_uiLength; }

private:
	std::string m_szFileName;
	uint64_t m_uiStartOffset;
	uint64_t m_uiLength;

private:
	CPackFileEntry( const CPackFileEntry& ) = delete;
	CPackFileEntry& operator=( const CPackFileEntry& ) = delete;
};

inline CPackFileEntry::CPackFileEntry( const char* pszFileName, uint64_t uiStartOffset, uint64_t uiLength )
	: m_szFileName( pszFileName )
	, m_uiStartOffset( uiStartOffset )
	, m_uiLength( uiLength )
{
}

inline CPackFileEntry::CPackFileEntry( std::string&& szFileName, uint64_t uiStartOffset, uint64_t uiLength )
	: m_szFileName( std::move( szFileName ) )
	, m_uiStartOffset( uiStartOffset )
	, m_uiLength( uiLength )
{
}

inline static bool PackFileEntryLessFunc( const CPackFileEntry& lhs, const CPackFileEntry& rhs )
{
	return lhs.GetFileName() < rhs.GetFileName();
}

inline static bool operator<( const CPackFileEntry& lhs, const CPackFileEntry& rhs )
{
	return PackFileEntryLessFunc( lhs, rhs );
}

struct PackLess
{
	bool operator()( const CPackFileEntry& lhs, const CPackFileEntry& rhs )
	{
		return PackFileEntryLessFunc( lhs, rhs );
	}
};

#endif //FILESYSTEM_CPACKFILEENTRY_H
