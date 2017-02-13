/*
FontCache.hpp - font cache class
Copyright (C) 2015 XashXT Team

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#ifndef XASH_FONTCACHE_HPP
#define XASH_FONTCACHE_HPP

#include<VGUI_Font.h>

using namespace vgui;

class FontCache
{
public:
	FontCache();
	~FontCache() { }

	// returns a texture ID and a pointer to an array of 4 texture coords for the given character & font
	// uploads more texture if necessary
	bool GetTextureForChar( Font *font, char ch, int *textureID, float **texCoords );
private:
	// NOTE: If you change this, change s_pFontPageSize
	enum
	{
		FONT_PAGE_SIZE_16,
		FONT_PAGE_SIZE_32,
		FONT_PAGE_SIZE_64,
		FONT_PAGE_SIZE_128,
		FONT_PAGE_SIZE_COUNT,
	};

	// a single character in the cache
	typedef unsigned short HCacheEntry;
	struct CacheEntry_t
	{
		Font		*font;
		char		ch;
		byte		page;
		float		texCoords[4];

		HCacheEntry	nextEntry;	// doubly-linked list for use in the LRU
		HCacheEntry	prevEntry;
	};
	
	// a single texture page
	struct Page_t
	{
		short		textureID;
		short		fontHeight;
		short		wide, tall;	// total size of the page
		short		nextX, nextY;	// position to draw any new character positions
	};

	// allocates a new page for a given character
	bool AllocatePageForChar( int charWide, int charTall, int &pageIndex, int &drawX, int &drawY, int &twide, int &ttall );

	// Computes the page size given a character height
	int ComputePageType( int charTall ) const;

	static bool CacheEntryLessFunc( const CacheEntry_t &lhs, const CacheEntry_t &rhs );

	// cache
	typedef CUtlVector<Page_t> FontPageList_t;

	CUtlRBTree<CacheEntry_t, HCacheEntry> m_CharCache;
	FontPageList_t m_PageList;
	int m_pCurrPage[FONT_PAGE_SIZE_COUNT];
	HCacheEntry m_LRUListHeadIndex;

	static int s_pFontPageSize[FONT_PAGE_SIZE_COUNT];
};

#endif // XASH_FONTCACHE_HPP