#include "EngineInterface.h"
#include "BinkPanel.h"
#include <vgui/ISurface.h>
#include <IFileSystem.h>
#include "Bink.h"

using namespace vgui;

CBinkPanel::CBinkPanel(Panel *parent, char const *panelName)
    : Panel(parent, panelName)
{
	m_pPixelBuffer = NULL;
	m_iTextureId   = surface()->CreateNewTextureID();
	m_color        = Color(255, 255, 255, 255);
}

CBinkPanel::~CBinkPanel(void)
{
	CloseBink();
}

void CBinkPanel::PerformLayout(void)
{
	BaseClass::PerformLayout();
}

void CBinkPanel::Paint(void)
{
	int wide, tall;
	GetSize(wide, tall);

	if(m_hBink)
	{
		if(m_iCurrentFrame != m_iLastFrame)
		{
			BinkGoto(m_hBink, m_iCurrentFrame, 0);
			BinkDoFrame(m_hBink);
			BinkCopyToBuffer(m_hBink, m_pPixelBuffer, m_hBink->Width * m_iPixelSize, m_hBink->Height, 0, 0, m_iBinkFlags);

			if(m_iPixelSize == 3)
				surface()->DrawSetTextureRGB(m_iTextureId, m_pPixelBuffer, m_hBink->Width, m_hBink->Height, true, true);
			else
				surface()->DrawSetTextureRGBA(m_iTextureId, m_pPixelBuffer, m_hBink->Width, m_hBink->Height, true, true);
		}

		surface()->DrawSetColor(Color(m_color.r(), m_color.g(), m_color.b(), m_color.a() * (GetAlpha() / 255.0)));
		surface()->DrawSetTexture(m_iTextureId);
		surface()->DrawTexturedRect(0, 0, wide, tall);
	}
	else
	{
		surface()->DrawSetColor(Color(0, 0, 0, 255));
		surface()->DrawFilledRect(0, 0, wide, tall);
	}
}

void CBinkPanel::OnThink(void)
{
	BaseClass::OnThink();

	if(!m_bPlaying)
		return;

	if(m_hBink)
	{
		float time = engine->GetAbsoluteTime();

		if(time - m_flLastPaintTime > (float)m_hBink->FrameRateDiv / m_hBink->FrameRate)
		{
			m_iLastFrame = m_iCurrentFrame;
			m_iCurrentFrame++;
			m_flLastPaintTime = time;

			if(m_iCurrentFrame > m_hBink->Frames)
				Stop();
		}
	}
}

bool CBinkPanel::OpenBink(const char *filename, int flags)
{
	char fullPath[MAX_PATH];

	if(!g_pFullFileSystem->GetLocalPath(filename, fullPath, sizeof(fullPath)))
		return false;

	m_hBink = BinkOpen(fullPath, BINKNOTHREADEDIO | BINKALPHA);

	if(!m_hBink)
		return false;

	BinkSetSoundOnOff(m_hBink, 0);

	if(flags & (BINKSURFACE24 | BINKSURFACE24R))
	{
		m_iPixelSize = 3;
	}
	else if(flags & (BINKSURFACE32 | BINKSURFACE32R | BINKSURFACE32A))
	{
		m_iPixelSize = 4;
	}

	m_iBinkFlags   = flags;
	m_pPixelBuffer = new byte[m_hBink->Width * m_hBink->Height * m_iPixelSize];
	return true;
}

void CBinkPanel::CloseBink(void)
{
	if(m_pPixelBuffer)
	{
		delete[] m_pPixelBuffer;
		m_pPixelBuffer = NULL;
	}

	if(m_hBink)
		BinkClose(m_hBink);
}

float CBinkPanel::GetPlayTime(void)
{
	return m_hBink ? (float)m_hBink->Frames / (float)m_hBink->FrameRate / m_hBink->FrameRateDiv : 0;
}

void CBinkPanel::SetColor(Color col)
{
	m_color = col;
}

void CBinkPanel::Play(void)
{
	m_iCurrentFrame   = 0;
	m_flLastPaintTime = engine->GetAbsoluteTime();
	m_bPlaying        = true;
}

void CBinkPanel::Pause(void)
{
	m_bPlaying = false;
}

void CBinkPanel::Continue(void)
{
	m_bPlaying = true;
}

void CBinkPanel::Stop(void)
{
	m_iCurrentFrame = 0;
	m_bPlaying      = false;
}