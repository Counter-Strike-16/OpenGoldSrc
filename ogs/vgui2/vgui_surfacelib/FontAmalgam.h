#ifndef VGUI2_VGUI_SURFACELIB_FONTAMALGAM_H
#define VGUI2_VGUI_SURFACELIB_FONTAMALGAM_H

#include <UtlVector.h>

class CWin32Font;

class CFontAmalgam
{
public:
	CFontAmalgam();
	~CFontAmalgam();

	char* Name() { return m_szName; }

	void SetName( const char* name );

	void AddFont( CWin32Font* font, int lowRange, int highRange );

	CWin32Font* GetFontForChar( int ch );

	int GetFontHeight();

	int GetFontMaxWidth();

	const char* GetFontName( int i );

	int GetFlags( int i );

	int GetCount();

	bool GetUnderlined();

	void RemoveAll();

private:
	struct TFontRange
	{
		int lowRange;
		int highRange;
		CWin32Font* font;
	};

	CUtlVector<TFontRange> m_Fonts;

	char m_szName[ 32 ];

	int m_iMaxWidth = 0;
	int m_iMaxHeight = 0;

private:
	CFontAmalgam( const CFontAmalgam& ) = delete;
	CFontAmalgam& operator=( const CFontAmalgam& ) = delete;
};

#endif //VGUI2_VGUI_SURFACELIB_FONTAMALGAM_H