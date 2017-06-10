#ifndef GAMEUI_LOADINGDIALOG_H
#define GAMEUI_LOADINGDIALOG_H

#include <vgui_controls/Frame.h>
#include <vgui_controls/RichText.h>

#include "GameUI/IGameUI.h"

class CBitmapImagePanel;

class CareerRichText : public vgui2::RichText
{
public:
	using BaseClass = vgui2::RichText;

	CareerRichText( vgui2::Panel* parent );
	~CareerRichText();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void PerformLayout() override;

private:
	int m_fontHeight = 0;
	bool m_addedScrollbar = false;

private:
	CareerRichText( const CareerRichText& ) = delete;
	CareerRichText& operator=( const CareerRichText& ) = delete;
};

class CLoadingDialog : public vgui2::Frame
{
public:
	DECLARE_CLASS_SIMPLE( CLoadingDialog, vgui2::Frame );

public:
	CLoadingDialog( vgui2::Panel* parent );
	~CLoadingDialog();

	void ApplySchemeSettings( vgui2::IScheme* pScheme ) override;

	void OnClose() override;

	void PerformLayout() override;

	void OnKeyCodePressed( vgui2::KeyCode code ) override;

	void OnCommand( const char* command ) override;

	void DisplayProgressBar();

	void SetupControlSettingsForErrorDisplay( const char* settingsFile );

	void DisplayGenericError( const char* failureReason, const char* extendedReason );

	void DisplayVACBannedError();

	void DisplayNoSteamConnectionError();

	void DisplaySteamConnectionError( const char* username, ESteamLoginFailure steamLoginFailure );

	void SetStatusText( const char* statusText );

	void SetProgressPoint( int progressPoint );

	void SetProgressRange( int min, int max );

	void SetSecondaryProgress( float progress );

	void SetSecondaryProgressText( const char* statusText );

	void SwitchToHttpDownloadDialog();

	void SwitchToPausedCareerDialog();

	void CompleteSwitchToPausedCareerDialog();

	void OnThink() override;

private:
	vgui2::ProgressBar* m_pProgress;
	vgui2::ProgressBar* m_pProgress2;

	vgui2::Label* m_pInfoLabel;
	vgui2::Label* m_pTimeRemainingLabel;

	vgui2::Button* m_pCancelButton;

	CareerRichText* m_pCareerMapDescLabel;

	vgui2::Label* m_pCareerMapLabel;
	vgui2::Label* m_pCareerScenarioInfo;

	CBitmapImagePanel* m_pCareerMapImagePanel;

	float m_switchTime = 0;

	int m_iRangeMin = 0;
	int m_iRangeMax = 0;

	bool m_bShowingSecondaryProgress = false;
	float m_flProgress = 0;
	float m_flSecondaryProgress = 0;
	float m_flLastSecondaryProgressUpdateTime = 0;
	float m_flSecondaryProgressStartTime = 0;

	bool m_isCareerLoadingDialog = false;
	bool m_isHttpDownloadDialog = false;

	bool m_bShowingVACInfo = false;
	bool m_bShowingVACError = false;

	char m_szUserName[ 64 ] = { '\0' };

private:
	CLoadingDialog( const CLoadingDialog& ) = delete;
	CLoadingDialog& operator=( const CLoadingDialog& ) = delete;
};

CLoadingDialog* LoadingDialog();

#endif //GAMEUI_LOADINGDIALOG_H
