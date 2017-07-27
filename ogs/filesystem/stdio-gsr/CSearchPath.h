#pragma once

#include <cstdint>
#include <map>
#include <memory>

#include "tier0/platform.h"

#include "CPackFileEntry.h"

#include "StringUtils.h"

class CFileHandle;

typedef uint32_t SearchPathFlags_t;

namespace SearchPathFlag
{
	enum : SearchPathFlags_t
	{
		NONE			= 0,
		READ_ONLY		= 1 << 0,
		IS_PACK_FILE	= 1 << 1,
	};
};

class CSearchPath
{
public:
	typedef std::map<const char*, std::unique_ptr<CPackFileEntry>, Less_C_String<const char*>> Entries_t;

	CSearchPath() = default;
	CSearchPath( CSearchPath&& other ) = default;
	CSearchPath& operator=( CSearchPath&& other ) = default;

	bool IsPackFile() const { return ( flags & SearchPathFlag::IS_PACK_FILE ) != 0; }

	char szPath[ MAX_PATH ];

	//TODO: needs to use CUtlSymbol - Solokiller
	char szPathID[ MAX_PATH ];

	SearchPathFlags_t flags;

	std::unique_ptr<CFileHandle> packFile;

	Entries_t packEntries;

	bool HasPathID() const { return *szPathID != '\0'; }

	const char* GetPathID() const { return szPathID; }

private:
	CSearchPath( const CSearchPath& ) = delete;
	CSearchPath& operator=( const CSearchPath& ) = delete;
};