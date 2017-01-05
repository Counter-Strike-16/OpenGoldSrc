#ifndef BINKPLAYER_H
#define BINKPLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>

#ifndef __BINKH__
typedef struct BINK *HBINK;

#define BINKSURFACE24 1
#define BINKSURFACE24R 2
#define BINKSURFACE32 3
#define BINKSURFACE32R 4
#define BINKSURFACE32A 5
#define BINKSURFACE32RA 6
#endif

class CBinkPanel : public vgui::Panel
{
	typedef vgui::Panel BaseClass;

public:
	CBinkPanel(vgui::Panel *parent, char const *filename = NULL);
	~CBinkPanel(void);

public:
	void PerformLayout(void);
	void Paint(void);

public:
	void OnThink(void);

public:
	bool OpenBink(const char *filename, int flags);
	void CloseBink(void);
	float GetPlayTime(void);
	void SetColor(Color col);

public:
	void Play(void);
	void Pause(void);
	void Continue(void);
	void Stop(void);
	bool IsPlaying(void) { return m_bPlaying; }

private:
	HBINK m_hBink;
	int m_iBinkFlags;
	byte *m_pPixelBuffer;
	int m_iPixelSize;
	int m_iTextureId;
	unsigned int m_iLastFrame;
	unsigned int m_iCurrentFrame;
	float m_flLastPaintTime;
	bool m_bPlaying;
	Color m_color;
};

#endif