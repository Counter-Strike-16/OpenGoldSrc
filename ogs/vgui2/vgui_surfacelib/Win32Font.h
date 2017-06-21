#ifndef VGUI2_VGUI_SURFACELIB_WIN32FONT_H
#define VGUI2_VGUI_SURFACELIB_WIN32FONT_H

#include <UtlRBTree.h>

#include <winheaders.h>

#undef GetCharABCWidths

extern bool s_bSupportsUnicode;

class CWin32Font
{
private:
	struct abc_t
	{
		short b;
		char a;
		char c;
	};

	struct abc_cache_t
	{
		wchar_t wch;
		abc_t abc;
	};

	static const size_t MAX_CHARS = 128;

public:
	CWin32Font();
	~CWin32Font();

	bool Create( const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags );

	void ApplyRotaryEffectToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba );

	void ApplyScanlineEffectToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba );

	void ApplyDropShadowToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba );

	void ApplyGaussianBlurToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba );

	void GetCharABCWidths( int ch, int& a, int& b, int& c );

	void GetCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* prgba );

	bool IsEqualTo( const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags );

	bool IsValid()
	{
		return m_szName[ 0 ] != '\0';
	}

	const char* GetName() { return m_szName; }

	int GetHeight()
	{
		return m_iHeight;
	}

	int GetAscent()
	{
		return m_iAscent;
	}

	int GetMaxCharWidth()
	{
		return m_iMaxCharWidth;
	}

	int GetFlags()
	{
		return m_iFlags;
	}

	bool GetUnderlined()
	{
		return m_bUnderlined;
	}

private:
	static bool ExtendedABCWidthsCacheLessFunc( const abc_cache_t& lhs, const abc_cache_t& rhs );

private:
	char m_szName[ 32 ] = { '\0' };

	int m_iTall = 0;
	int m_iWeight = 0;
	int m_iFlags = 0;

	bool m_bAntiAliased = false;
	bool m_bRotary = false;
	bool m_bAdditive = false;
	int m_iDropShadowOffset = 0;
	bool m_bUnderlined = false;
	int m_iHeight = 0;
	int m_iMaxCharWidth = 0;
	int m_iAscent = 0;
	
	abc_t m_ABCWidthsCache[ MAX_CHARS ];

	CUtlRBTree<abc_cache_t> m_ExtendedABCWidthsCache;

	HFONT m_hFont = NULL;
	HDC m_hDC = NULL;
	HBITMAP m_hBitmap = NULL;

	int m_rgiBitmapSize[ 2 ];

	//DIB buffer
	byte* m_pBuf;

	int m_iScanLines = 0;
	int m_iBlur = 0;
	float* m_pGaussianDistribution = nullptr;

private:
	CWin32Font( const CWin32Font& ) = delete;
	CWin32Font& operator=( const CWin32Font& ) = delete;
};

#endif //VGUI2_VGUI_SURFACELIB_WIN32FONT_H