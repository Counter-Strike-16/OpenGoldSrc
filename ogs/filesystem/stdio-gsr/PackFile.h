#ifndef FILESYSTEM_PACKFILE_H
#define FILESYSTEM_PACKFILE_H

#include <cstdint>

/**
*	@file
*	Pack file header and data entry structures and constants
*/

namespace pack
{
/**
*	Dummy header used to provide a common interface for 32 and 64 bit pack files.
*/
struct Header_t
{
	char identifier[ 4 ];
};

enum class PackType
{
	NOT_A_PACK = 0,
	PACK_32BIT,
	PACK_64BIT
};

/**
*	Identifies the type of pack file.
*/
inline PackType IdentifyPackType( const Header_t& header )
{
	if( header.identifier[ 0 ] == 'P' &&
		header.identifier[ 1 ] == 'A' &&
		header.identifier[ 2 ] == 'C' &&
		header.identifier[ 3 ] == 'K' )
		return PackType::PACK_32BIT;
	else if( 
		header.identifier[ 0 ] == 'P' &&
		header.identifier[ 1 ] == 'K' &&
		header.identifier[ 2 ] == '6' &&
		header.identifier[ 3 ] == '4' )
		return PackType::PACK_64BIT;

	return PackType::NOT_A_PACK;
}

template<typename SIZE>
struct PackInfoForSize
{
};

template<>
struct PackInfoForSize<int32_t>
{
	static const PackType TYPE = PackType::PACK_32BIT;

	static const char* const NAME;
};

const char* const PackInfoForSize<int32_t>::NAME = "32 bit Pack File";

template<>
struct PackInfoForSize<int64_t>
{
	static const PackType TYPE = PackType::PACK_64BIT;

	static const char* const NAME;
};

const char* const PackInfoForSize<int64_t>::NAME = "64 bit Pack File";

/**
*	Template class that defines constants and data structures for pack files.
*	Specialized for 32 and 64 bit types.
*	@tparam SIZE Type that indicates which size to provide data for. Valid types are int32_t and int64_t.
*	@see Pack32_t
*	@see Pack64_t
*/
template<typename SIZE>
class Pack final
{
public:
	static_assert( sizeof( SIZE ) == 4 || sizeof( SIZE ) == 8, "Invalid pack file size" );

	typedef PackInfoForSize<SIZE> Info_t;

	static const PackType TYPE = PackInfoForSize<SIZE>::TYPE;

	typedef SIZE size_type;

	struct Header_t
	{
		char identifier[ 4 ];

		size_type dirofs;
		size_type dirlen;
	};

	static const size_t ENTRY_NAME_MAX_LENGTH = ( sizeof( size_type ) * 16 ) - ( sizeof( size_type ) * 2 );

	struct Entry_t
	{
		char szFileName[ ENTRY_NAME_MAX_LENGTH ];
		size_type filepos;
		size_type filelen;
	};

	/**
	*	Maximum number of files in a single pack file.
	*/
	static const size_t MAX_FILES = 32768;

private:
	Pack() = delete;
	Pack( const Pack& ) = delete;
	Pack& operator=( const Pack& ) = delete;
};

typedef Pack<int32_t> Pack32_t;
typedef Pack<int64_t> Pack64_t;

struct PackAppend_t
{
	char identifier[ 8 ];

	int64_t packheaderpos;
	int64_t originalfilesize;
};

inline bool IsAppendPack( const PackAppend_t& header )
{
	return
		header.identifier[ 0 ] == 'P' &&
		header.identifier[ 1 ] == 'A' &&
		header.identifier[ 2 ] == 'C' &&
		header.identifier[ 3 ] == 'K' &&
		header.identifier[ 0 ] == 'A' &&
		header.identifier[ 1 ] == 'P' &&
		header.identifier[ 2 ] == 'P' &&
		header.identifier[ 2 ] == 'E';
}
}

#endif //FILESYSTEM_PACKFILE_H
