#include <tier0/platform.h>

#include <vgui/ISurface.h>

#include "Win32Font.h"

static bool g_bCachedOSVersionInfo = false;

static OSVERSIONINFOA g_OSVersionInfo;

bool s_bSupportsUnicode = false;

//TODO: can pass this as the lParam - Solokiller
static bool g_bFontFound = false;

static int CALLBACK EnumFontFamExProc(
	const LOGFONT* lpelfe,
	const TEXTMETRIC* lpntme,
	DWORD FontType,
	LPARAM lParam
)
{
	g_bFontFound = true;
	return 0;
}

bool CWin32Font::ExtendedABCWidthsCacheLessFunc( const abc_cache_t& lhs, const abc_cache_t& rhs )
{
	return lhs.wch < rhs.wch;
}

CWin32Font::CWin32Font()
	: m_ExtendedABCWidthsCache( 0, 256, &CWin32Font::ExtendedABCWidthsCacheLessFunc )
{
	//TODO: verify class layout - Solokiller

	if( !g_bCachedOSVersionInfo )
	{
		memset( &g_OSVersionInfo, 0, sizeof( g_OSVersionInfo ) );
		g_bCachedOSVersionInfo = true;

		g_OSVersionInfo.dwOSVersionInfoSize = sizeof( g_OSVersionInfo );
		GetVersionExA( &g_OSVersionInfo );

		s_bSupportsUnicode = g_OSVersionInfo.dwMajorVersion >= 5;
	}
}

CWin32Font::~CWin32Font()
{
	if( m_hFont )
	{
		DeleteObject( m_hFont );
	}

	if( m_hDC )
	{
		DeleteObject( m_hDC );
	}

	if( m_hBitmap )
	{
		DeleteObject( m_hBitmap );
	}

	//TODO: free gaussian distribution array - Solokiller
}

bool CWin32Font::Create( const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
{
	strncpy( m_szName, windowsFontName, ARRAYSIZE( m_szName ) );
	m_szName[ ARRAYSIZE( m_szName ) - 1 ] = '\0';

	m_bAntiAliased = ( flags & vgui2::ISurface::FONTFLAG_ANTIALIAS ) != 0;
	m_bUnderlined = ( flags & vgui2::ISurface::FONTFLAG_UNDERLINE ) != 0;
	m_iTall = tall;
	m_iDropShadowOffset = ( flags & vgui2::ISurface::FONTFLAG_DROPSHADOW ) != 0;
	m_bRotary = ( flags & vgui2::ISurface::FONTFLAG_ROTARY ) != 0;
	m_iWeight = weight;
	m_iBlur = blur;
	m_iFlags = flags;
	m_iScanLines = scanlines;
	m_iHeight = tall;

	DWORD charSet = ( flags & vgui2::ISurface::FONTFLAG_SYMBOL ) ? SYMBOL_CHARSET : ANSI_CHARSET;

	if( !stricmp( windowsFontName, "win98japanese" ) )
	{
		charSet = SHIFTJIS_CHARSET;
		strncpy( m_szName, "Tahoma", ARRAYSIZE( m_szName ) );
	}

	m_hDC = CreateCompatibleDC( NULL );

	LOGFONTA logFont;

	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	strcpy( logFont.lfFaceName, m_szName );

	g_bFontFound = false;
	EnumFontFamiliesExA( m_hDC, &logFont, &EnumFontFamExProc, 0, 0 );

	if( !g_bFontFound )
	{
		m_szName[ 0 ] = '\0';
		return false;
	}

	//At least one font exists with our desired settings.

	m_hFont = CreateFontA(
		tall, 0, 0, 0, m_iWeight,
		( flags & vgui2::ISurface::FONTFLAG_ITALIC ) != 0,
		( flags & vgui2::ISurface::FONTFLAG_UNDERLINE ) != 0,
		( flags & vgui2::ISurface::FONTFLAG_STRIKEOUT ) != 0,
		charSet,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		m_bAntiAliased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH,
		windowsFontName
	);

	if( !m_hFont )
	{
		m_szName[ 0 ] = '\0';
		return false;
	}

	SetMapMode( m_hDC, MM_TEXT );
	SelectObject( m_hDC, m_hFont );
	SetTextAlign( m_hDC, TA_UPDATECP );

	TEXTMETRICA metric;
	GetTextMetricsA( m_hDC, &metric );

	int iHeight = m_iDropShadowOffset + metric.tmHeight;

	m_iAscent = metric.tmAscent;
	m_iMaxCharWidth = metric.tmMaxCharWidth;
	m_iHeight = metric.tmHeight;

	m_rgiBitmapSize[ 0 ] = metric.tmMaxCharWidth;
	m_rgiBitmapSize[ 1 ] = iHeight;

	BITMAPINFO bmi;

	memset( &bmi, 0, sizeof( bmi ) );

	bmi.bmiHeader.biSize = sizeof( bmi.bmiHeader );
	bmi.bmiHeader.biHeight = -iHeight;
	bmi.bmiHeader.biWidth = metric.tmMaxCharWidth;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	m_hBitmap = CreateDIBSection( m_hDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>( &m_pBuf ), NULL, 0 );

	SelectObject( m_hDC, m_hBitmap );

	memset( m_ABCWidthsCache, 0, sizeof( m_ABCWidthsCache ) );

	ABC abc[ MAX_CHARS ];

	if( GetCharABCWidthsW( m_hDC, 0, MAX_CHARS - 1, abc ) ||
		GetCharABCWidthsA( m_hDC, 0, MAX_CHARS - 1, abc ) )
	{
		for( size_t i = 0; i < MAX_CHARS; ++i )
		{
			auto& abcSrc = abc[ i ];
			auto& abcDest = m_ABCWidthsCache[ i ];

			abcDest.a = abcSrc.abcA - m_iBlur;
			abcDest.b = abcSrc.abcB - m_iDropShadowOffset - m_iBlur;
			abcDest.c = abcSrc.abcC + m_iDropShadowOffset + 2 * m_iBlur;
		}
	}
	else
	{
		for( size_t i = 0; i < MAX_CHARS; ++i )
		{
			auto& abcDest = m_ABCWidthsCache[ i ];

			abcDest.b = metric.tmMaxCharWidth;
		}
	}

	if( m_iBlur > 1 )
	{
		m_pGaussianDistribution = new float[ ( 2 * m_iBlur ) + 1 ];

		//Generate Gaussian blur.
		if( !( m_iBlur & 0x40000000 ) )
		{
			//Seems to be this: https://en.wikipedia.org/wiki/Gaussian_blur
			const double flBlur = static_cast<double>( m_iBlur ) * 0.683;
			const double flBlurSquared = flBlur * flBlur;
			const double flBlurDouble = 2 * flBlur;

			//2 * pi * flBlurSquared
			const double x = 1.0 / sqrt( 6.28 * flBlurSquared );

			for( int i = 0; i <= 2 * m_iBlur; ++i )
			{
				m_pGaussianDistribution[ i ] = x * pow(
					2.7, //Mathematical constant 'e'
					static_cast<double>( -( ( i - m_iBlur ) * ( i - m_iBlur ) ) ) / flBlurDouble );
			}
		}
	}

	return true;
}

void CWin32Font::ApplyRotaryEffectToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba )
{
	if( m_bRotary && rgbaWide > 0 )
	{
		byte* pDest = &rgba[ 4 * ( rgbaX + rgbaWide * ( rgbaY + static_cast<int>( floor( rgbaTall * 0.5 ) ) ) ) ];
		
		for( int i = 0; i < rgbaWide; ++i )
		{
			pDest[ 0 ] = 127;
			pDest[ 1 ] = 127;
			pDest[ 2 ] = 127;
			pDest[ 3 ] = 0xFF;

			pDest += 4;
		}
	}
}

void CWin32Font::ApplyScanlineEffectToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba )
{
	if( m_iScanLines > 1 && rgbaTall > 0 )
	{
		byte* pDest = &rgba[ 4 * ( rgbaX + rgbaWide * ( rgbaY + 1 ) ) ];

		for( int i = 1; i < rgbaTall; ++i )
		{
			if( i % m_iScanLines && rgbaWide > 0 )
			{
				for( int x = 0; x < rgbaWide; ++x, pDest += 4 )
				{
					pDest[ 0 ] = static_cast<int>( floor( pDest[ 0 ] * 0.7 ) );
					pDest[ 1 ] = static_cast<int>( floor( pDest[ 1 ] * 0.7 ) );
					pDest[ 2 ] = static_cast<int>( floor( pDest[ 2 ] * 0.7 ) );
				}
			}
		}
	}
}

void CWin32Font::ApplyDropShadowToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba )
{
	if( !m_iDropShadowOffset )
		return;

	byte* pDest = &rgba[ 4 * ( rgbaX + rgbaWide - 1 + rgbaWide * ( rgbaX + ( rgbaTall - 1 ) ) ) ];

	for( int y = rgbaTall - 1; y >= m_iDropShadowOffset; --y )
	{
		for( int x = rgbaWide - 1; x >= m_iDropShadowOffset; --x, pDest -= 4 )
		{
			if( !pDest[ 3 ] )
			{
				pDest[ 0 ] = 0;
				pDest[ 1 ] = 0;
				pDest[ 2 ] = 0;
				pDest[ 3 ] = rgba[ 4 * ( ( rgbaY + y - m_iDropShadowOffset ) * rgbaWide + x + rgbaX - m_iDropShadowOffset ) + 3 ];
			}
		}
	}
}

void CWin32Font::ApplyGaussianBlurToTexture( int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* rgba )
{
	if( !m_pGaussianDistribution )
		return;

	unsigned int uiBufferSize = 4 * rgbaWide * rgbaTall;

	auto pBuffer = stackalloc( uiBufferSize + 15 );

	byte* pSrc = reinterpret_cast<byte*>( pBuffer );

	pSrc = AlignValue( pSrc, 16 );

	if( uiBufferSize >= 4 )
		memcpy( pSrc, rgba, uiBufferSize );

	auto pDest = rgba;

	for( int y = 0; y < rgbaTall; ++y, pDest += 4 * rgbaWide )
	{
		for( int x = 0; y < rgbaWide; ++x )
		{
			const int iBlurXMax = min( x + m_iBlur, rgbaWide );
			int iBlurXMin = max( x - m_iBlur, 0 );

			int iValue;

			if( iBlurXMax < iBlurXMin )
			{
				iValue = 0;
			}
			else
			{
				const int iBlurYMax = min( y + m_iBlur, rgbaTall - 1 );
				int iBlurYMin = max( y - m_iBlur, 0 );

				byte* pSrcBuffer = &pSrc[ 4 * ( iBlurXMin + iBlurYMin * rgbaWide ) ];
				float* pGDistr = &m_pGaussianDistribution[ m_iBlur - x ];

				double flValue = 0;

				while( iBlurXMin <= iBlurXMax )
				{
					auto pSrcLocal = pSrcBuffer;

					while( iBlurYMin <= iBlurYMax )
					{
						flValue += static_cast<double>( *pSrcLocal ) *
							( *( &m_pGaussianDistribution[ 4 * ( m_iBlur - y ) ] + iBlurYMin )
							  * pGDistr[ iBlurXMin ]
							  );

						pSrcLocal += 4 * rgbaWide;

						++iBlurYMin;
					}

					++iBlurXMin;
					pSrcBuffer += 4;
				}

				iValue = static_cast<int>( floor( flValue ) );

				if( iValue > 0xFF )
				{
					iValue = 0xFF;
				}
			}

			pDest[ 0 ] = iValue;
			pDest[ 1 ] = iValue;
			pDest[ 2 ] = iValue;
			pDest[ 3 ] = 0xFF;
		}
	}

	stackfree( pBuffer );
}

void CWin32Font::GetCharABCWidths( int ch, int& a, int& b, int& c )
{
	//Use cached entries
	if( ch < MAX_CHARS )
	{
		a = m_ABCWidthsCache[ ch ].a;
		b = m_ABCWidthsCache[ ch ].b;
		c = m_ABCWidthsCache[ ch ].c;
		return;
	}

	abc_cache_t finder;

	finder.abc.a = 0;
	finder.abc.b = 0;
	finder.abc.c = 0;

	finder.wch = ch;

	auto index = m_ExtendedABCWidthsCache.Find( finder );

	if( index != m_ExtendedABCWidthsCache.InvalidIndex() )
	{
		a = m_ExtendedABCWidthsCache[ index ].abc.a;
		b = m_ExtendedABCWidthsCache[ index ].abc.b;
		c = m_ExtendedABCWidthsCache[ index ].abc.c;
	}
	else
	{
		ABC abc;

		if( GetCharABCWidthsW( m_hDC, ch, ch, &abc ) )
		{
			finder.abc.a = abc.abcA;
			finder.abc.b = abc.abcB;
			finder.abc.c = abc.abcC;
		}
		else
		{
			char str[ 6 ] = {};

			wchar_t wChar = ch;

			WideCharToMultiByte( CP_ACP, 0, &wChar, 1, str, sizeof( str ), nullptr, nullptr );
			SIZE size;
			const bool bSuccess = GetTextExtentPoint32A( m_hDC, str, strlen( str ), &size ) != FALSE;

			finder.abc.a = 0;
			finder.abc.c = 0;

			if( bSuccess )
			{
				finder.abc.b = size.cx;
			}
			else
			{
				finder.abc.b = m_iMaxCharWidth;
			}
		}

		m_ExtendedABCWidthsCache.Insert( finder );

		a = finder.abc.a;
		b = finder.abc.b;
		c = finder.abc.c;
	}
}

void CWin32Font::GetCharRGBA( int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, byte* prgba )
{
	int a, b, c;

	GetCharABCWidths( ch, a, b, c );

	SelectObject( m_hDC, m_hFont );

	int iWidth = b;

	if( m_bUnderlined )
		iWidth = a + b + c;

	MAT2 mat2;

	mat2.eM11.value = 1;
	mat2.eM22.value = 1;

	mat2.eM11.fract = 0;
	mat2.eM12.fract = 0;
	mat2.eM12.value = 0;
	mat2.eM21.fract = 0;
	mat2.eM21.value = 0;
	mat2.eM22.fract = 0;

	bool bSuccess = false;

	if( m_bAntiAliased && ch <= 0xFF )
	{
		SelectObject( m_hDC, m_hFont );

		GLYPHMETRICS gm;

		const DWORD uiBufferSize = GetGlyphOutlineA( m_hDC, ch, GGO_GRAY8_BITMAP, &gm, 0, nullptr, &mat2 );
		
		if( uiBufferSize > 0 )
		{
			//Align buffer size to 4 byte boundary
			auto pBuffer = reinterpret_cast<DWORD*>( stackalloc( AlignValue( uiBufferSize, 4 ) ) );

			GetGlyphOutlineA( m_hDC, ch, GGO_GRAY8_BITMAP, &gm, uiBufferSize, pBuffer, &mat2 );

			iWidth = gm.gmBlackBoxX;

			//Align to 4 byte boundary
			while( iWidth % 4 )
			{
				++iWidth;
			}

			unsigned int uiX = 0;

			if( static_cast<int>( gm.gmBlackBoxX ) >= b + 2 )
				uiX = ( gm.gmBlackBoxX - b ) / 2;

			for( UINT y = 0; y < gm.gmBlackBoxY; ++y )
			{
				for( UINT x = uiX; x < gm.gmBlackBoxX; ++x )
				{
					const int iXIndex = rgbaX + x + m_iBlur - uiX;

					if( iXIndex < rgbaWide && ( rgbaY + ( m_iAscent - gm.gmptGlyphOrigin.y ) ) < rgbaTall )
					{
						const int iPixelValue = pBuffer[ x ];

						auto pDest = &prgba[ 4 * ( iXIndex + ( rgbaWide * ( rgbaY + ( m_iAscent - gm.gmptGlyphOrigin.y ) ) ) ) ];

						if( iPixelValue )
						{
							pDest[ 0 ] = 0xFF;
							pDest[ 1 ] = 0xFF;
							pDest[ 2 ] = 0xFF;
							pDest[ 3 ] = iPixelValue * 0.015625 * 0xFF;

							if( pDest[ 3 ] > 0xFF )
								pDest[ 3 ] = 0xFF;
						}
						else
						{
							pDest[ 0 ] = 0;
							pDest[ 1 ] = 0;
							pDest[ 2 ] = 0;
							pDest[ 3 ] = 0;
						}

						if( ch == '\t' )
						{
							pDest[ 0 ] = 0;
							pDest[ 1 ] = 0;
							pDest[ 2 ] = 0;
						}
					}
				}
			}

			bSuccess = true;

			stackfree( pBuffer );
		}
	}

	if( !bSuccess )
	{
		SetBkColor( m_hDC, RGB( 0, 0, 0 ) );
		SetTextColor( m_hDC, RGB( 255, 255, 255 ) );
		SetBkMode( m_hDC, OPAQUE );

		if( m_bUnderlined )
		{
			MoveToEx( m_hDC, 0, 0, nullptr );
		}
		else
		{
			MoveToEx( m_hDC, -a, 0, nullptr );
		}

		if( s_bSupportsUnicode )
		{
			wchar_t szBuffer[ 2 ];
			szBuffer[ 0 ] = ch;

			ExtTextOutW( m_hDC, 0, 0, 0, nullptr, szBuffer, 1, nullptr );
		}
		else
		{
			//Draw a rectangle and draw the character on top.
			RECT rect;

			rect.right = iWidth;
			rect.left = 0;
			rect.top = 0;
			rect.bottom = m_iHeight;

			ExtTextOutA( m_hDC, 0, 0, ETO_OPAQUE, &rect, nullptr, 0, nullptr );

			char szBuffer[ 6 ] = {};

			wchar_t wChar = ch;

			WideCharToMultiByte( CP_ACP, 0, &wChar, 1, szBuffer, sizeof( szBuffer ), nullptr, nullptr );
			ExtTextOutA( m_hDC, 0, 0, 0, nullptr, szBuffer, strlen( szBuffer ), nullptr );
		}

		SetBkMode( m_hDC, TRANSPARENT );

		if( iWidth > m_rgiBitmapSize[ 0 ] )
			iWidth = m_rgiBitmapSize[ 0 ];

		auto iHeight = m_iHeight;

		if( iHeight > m_rgiBitmapSize[ 1 ] )
			iHeight = m_rgiBitmapSize[ 1 ];

		for( int y = 0; y < iHeight; ++y )
		{
			for( int x = 0; x < iWidth - m_iDropShadowOffset; ++x )
			{
				if( ( x + rgbaX ) < rgbaWide && ( y + rgbaY ) < rgbaTall )
				{
					auto pBuf = &m_pBuf[ 4 * ( x + static_cast<int>( y ) * m_rgiBitmapSize[ 0 ] ) ];
					auto pDest = &prgba[ 4 * ( x + rgbaX + rgbaWide * ( rgbaY + y ) ) ];

					pDest[ 0 ] = pBuf[ 0 ] / 255.0;
					pDest[ 1 ] = pBuf[ 1 ] / 255.0;
					pDest[ 2 ] = pBuf[ 2 ] / 255.0;

					if( ch == '\t' )
					{
						pDest[ 0 ] = 0;
						pDest[ 1 ] = 0;
						pDest[ 2 ] = 0;
					}

					pDest[ 3 ] = ( pDest[ 0 ] * 0.55 + pDest[ 1 ] * 0.34 + pDest[ 2 ] * 0.11 ) * 255.0;

					pDest[ 0 ] *= 255.0;
					pDest[ 1 ] *= 255.0;
					pDest[ 2 ] *= 255.0;
				}
			}
		}

		if( m_iDropShadowOffset )
		{
			auto pDest = &prgba[ 4 * ( rgbaX + rgbaWide * ( m_iHeight - 1 ) ) ];

			for( int iX = iWidth; iX; --iX, pDest += 4 )
			{
				pDest[ 0 ] = 0;
				pDest[ 1 ] = 0;
				pDest[ 2 ] = 0;
				pDest[ 3 ] = 0;
			}
		}
	}

	//ApplyDropShadowToTexture( rgbaX, rgbaY, rgbaWide, rgbaTall, prgba );
	//ApplyGaussianBlurToTexture( rgbaX, rgbaY, rgbaWide, rgbaTall, prgba );
	//ApplyScanlineEffectToTexture( rgbaX, rgbaY, rgbaWide, rgbaTall, prgba );
	//ApplyRotaryEffectToTexture( rgbaX, rgbaY, rgbaWide, rgbaTall, prgba );
}

bool CWin32Font::IsEqualTo( const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags )
{
	return !stricmp( windowsFontName, m_szName ) &&
		m_iTall == tall &&
		m_iWeight == weight &&
		m_iBlur == blur &&
		m_iFlags == flags;
}