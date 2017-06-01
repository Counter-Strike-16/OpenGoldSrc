//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "VGUI.h"
#include "VGUI_WizardPanel.h"

using namespace vgui;

void WizardPanel::WizardPage::fireSwitchingToBackPageSignals()
{
}

void WizardPanel::WizardPage::fireSwitchingToNextPageSignals()
{
}

void WizardPanel::WizardPage::init()
{
}

WizardPanel::WizardPage::WizardPage()
{
}

WizardPanel::WizardPage::WizardPage(int wide,int tall)
{
}

void WizardPanel::WizardPage::setBackWizardPage(WizardPage* backWizardPage)
{
}

void WizardPanel::WizardPage::setNextWizardPage(WizardPage* nextWizardPage)
{
}

WizardPanel::WizardPage* WizardPanel::WizardPage::getBackWizardPage()
{
	return null;
}

WizardPanel::WizardPage* WizardPanel::WizardPage::getNextWizardPage()
{
	return null;
}

bool WizardPanel::WizardPage::isBackButtonEnabled()
{
	return false;
}

bool WizardPanel::WizardPage::isNextButtonEnabled()
{
	return false;
}

bool WizardPanel::WizardPage::isFinishedButtonEnabled()
{
	return false;
}

bool WizardPanel::WizardPage::isCancelButtonEnabled()
{
	return false;
}

void WizardPanel::WizardPage::setBackButtonEnabled(bool state)
{
}

void WizardPanel::WizardPage::setNextButtonEnabled(bool state)
{
}

void WizardPanel::WizardPage::setFinishedButtonEnabled(bool state)
{
}

void WizardPanel::WizardPage::setCancelButtonEnabled(bool state)
{
}

bool WizardPanel::WizardPage::isBackButtonVisible()
{
	return false;
}

bool WizardPanel::WizardPage::isNextButtonVisible()
{
	return false;
}

bool WizardPanel::WizardPage::isFinishedButtonVisible()
{
	return false;
}

bool WizardPanel::WizardPage::isCancelButtonVisible()
{
	return false;
}

void WizardPanel::WizardPage::setBackButtonVisible(bool state)
{
}

void WizardPanel::WizardPage::setNextButtonVisible(bool state)
{
}

void WizardPanel::WizardPage::setFinishedButtonVisible(bool state)
{
}

void WizardPanel::WizardPage::setCancelButtonVisible(bool state)
{
}

void WizardPanel::WizardPage::getBackButtonText(char* text,int textLen)
{
}

void WizardPanel::WizardPage::getNextButtonText(char* text,int textLen)
{
}

void WizardPanel::WizardPage::getFinishedButtonText(char* text,int textLen)
{
}

void WizardPanel::WizardPage::getCancelButtonText(char* text,int textLen)
{
}

void WizardPanel::WizardPage::setBackButtonText(const char* text)
{
}

void WizardPanel::WizardPage::setNextButtonText(const char* text)
{
}

void WizardPanel::WizardPage::setFinishedButtonText(const char* text)
{
}

void WizardPanel::WizardPage::setCancelButtonText(const char* text)
{
}

void WizardPanel::WizardPage::setWantedFocus(Panel* panel)
{
}

Panel* WizardPanel::WizardPage::getWantedFocus()
{
	return null;
}

void WizardPanel::WizardPage::addSwitchingToBackPageSignal(ActionSignal* s)
{
}

void WizardPanel::WizardPage::addSwitchingToNextPageSignal(ActionSignal* s)
{
}

void WizardPanel::WizardPage::setTitle(const char* title)
{
}

void WizardPanel::WizardPage::getTitle(char* buf,int bufLen)
{
}

void WizardPanel::fireFinishedActionSignal()
{
}

void WizardPanel::fireCancelledActionSignal()
{
}

void WizardPanel::firePageChangedActionSignal()
{
}

void WizardPanel::performLayout()
{
}

WizardPanel::WizardPanel(int x,int y,int wide,int tall)
{
}

void WizardPanel::setCurrentWizardPage(WizardPage* currentWizardPage)
{
}

void WizardPanel::addFinishedActionSignal(ActionSignal* s)
{
}

void WizardPanel::addCancelledActionSignal(ActionSignal* s)
{
}

void WizardPanel::addPageChangedActionSignal(ActionSignal* s)
{
}

void WizardPanel::doBack()
{
}

void WizardPanel::doNext()
{
}

void WizardPanel::getCurrentWizardPageTitle(char* buf,int bufLen)
{
}

WizardPanel::WizardPage* WizardPanel::getCurrentWizardPage()
{
	return null;
}
