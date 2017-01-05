#include "EngineInterface.h"
#include "WebExplorer.h"
#include "VGUI\ILocalize.h"
#include "VGUI\IScheme.h"

using namespace vgui;

CWebExplorer::CWebExplorer(vgui::Panel *parent, const char *panelName) : Frame(parent, panelName)
{
	m_pMessage = new HTML(this, "Message");
	m_pMessage->SetContextMenuEnabled(false);

	m_iXOffset = vgui::scheme()->GetProportionalScaledValue(7);
	m_iYOffset = vgui::scheme()->GetProportionalScaledValue(22);
}

CWebExplorer::~CWebExplorer(void)
{
}

void CWebExplorer::PerformLayout(void)
{
	Frame::PerformLayout();

	m_pMessage->SetBounds(m_iXOffset, m_iYOffset, GetWide() - (m_iXOffset * 2), GetTall() - (m_iXOffset * 1.5 + m_iYOffset));
}

void CWebExplorer::Activate(const char *msg)
{
	Frame::Activate();

	if (IsURL(msg))
		m_pMessage->OpenURL(msg);
}

void CWebExplorer::Activate(const wchar_t *msg)
{
	char ansiURL[256];
	g_pVGuiLocalize->ConvertUnicodeToANSI(msg, ansiURL, sizeof(ansiURL));

	Activate(ansiURL);
}

bool CWebExplorer::IsURL(const char *str)
{
	bool isUrl = false;

	if (strlen(str) > 7 && str[0] == 'h' && str[1] == 't' && str[2] == 't' && str[3] == 'p' && str[4] == ':' && str[5] == '/' && str[6] == '/')
		isUrl = true;

	return isUrl;
}

void CWebExplorer::OnFinishURL(void)
{
}