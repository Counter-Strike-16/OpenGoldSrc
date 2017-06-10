#include <KeyValues.h>

#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Slider.h>

#include "CvarSlider.h"
#include "CvarToggleCheckButton.h"
#include "EngineInterface.h"
#include "OptionsSubVoice.h"

COptionsSubVoice::COptionsSubVoice( vgui2::Panel* parent )
	: BaseClass( parent, nullptr )
{
	m_pVoiceTweak = engine->pVoiceTweak;

	m_pMicMeter = new vgui2::ImagePanel( this, "MicMeter" );

	m_pMicMeter2 = new vgui2::ImagePanel( this, "MicMeter2" );

	m_pReceiveSliderLabel = new vgui2::Label( this, "ReceiveLabel", "#GameUI_VoiceReceiveVolume" );

	m_pReceiveVolume = new CCvarSlider( this, "VoiceReceive", "#GameUI_ReceiveVolume", 0.0, 1.0, "voice_scale", false );

	m_pMicrophoneSliderLabel = new vgui2::Label( this, "MicrophoneLabel", "#GameUI_VoiceTransmitVolume" );

	m_pMicrophoneVolume = new vgui2::Slider( this, "#GameUI_MicrophoneVolume" );
	m_pMicrophoneVolume->SetRange( 0, 100 );
	m_pMicrophoneVolume->AddActionSignalTarget( this );

	m_pVoiceEnableCheckButton = new CCvarToggleCheckButton( this, "voice_modenable", "#GameUI_EnableVoice", "voice_modenable" );

	m_pMicBoost = new vgui2::CheckButton( this, "MicBoost", "#GameUI_BoostMicrophone" );
	m_pMicBoost->AddActionSignalTarget( this );

	m_pTestMicrophoneButton =new vgui2::Button( this, "TestMicrophone", "#GameUI_TestMicrophone" );

	LoadControlSettings( "Resource\\OptionsSubVoice.res" );

	m_bVoiceOn = false;
	m_pMicMeter2->SetVisible( false );

	if( m_pVoiceTweak )
	{
		OnResetData();
	}
	else
	{
		m_pReceiveVolume->SetEnabled( false );
		m_pMicrophoneVolume->SetEnabled( false );
		m_pVoiceEnableCheckButton->SetEnabled( false );
		m_pMicBoost->SetEnabled( false );
		m_pTestMicrophoneButton->SetEnabled( false );
	}
}

COptionsSubVoice::~COptionsSubVoice()
{
	if( m_bVoiceOn && m_pVoiceTweak )
		EndTestMicrophone();
}

void COptionsSubVoice::OnControlModified()
{
	PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
}

void COptionsSubVoice::OnApplyChanges()
{
	if( m_pVoiceTweak )
	{
		m_nMicVolumeValue = m_pMicrophoneVolume->GetValue();
		m_pVoiceTweak->SetControlFloat( MicrophoneVolume, m_nMicVolumeValue / 100.0 );

		m_bMicBoostSelected = m_pMicBoost->IsSelected();
		m_pVoiceTweak->SetControlFloat( MicBoost, m_bMicBoostSelected );

		m_pReceiveVolume->ApplyChanges();
		m_fReceiveVolume = m_pReceiveVolume->GetSliderValue();

		m_pVoiceEnableCheckButton->ApplyChanges();
	}
}

void COptionsSubVoice::OnResetData()
{
	if( m_pVoiceTweak )
	{
		m_pMicrophoneVolume->SetValue( floor( m_pVoiceTweak->GetControlFloat( MicrophoneVolume ) * 100.0 ) );
		m_nMicVolumeValue = m_pMicrophoneVolume->GetValue();

		m_pMicBoost->SetSelected( m_pVoiceTweak->GetControlFloat( MicBoost ) != 0 );
		m_bMicBoostSelected = m_pMicBoost->IsSelected();

		m_pReceiveVolume->Reset();
		m_fReceiveVolume = m_pReceiveVolume->GetSliderValue();

		m_pVoiceEnableCheckButton->Reset();
	}
}

void COptionsSubVoice::OnThink()
{
	BaseClass::OnThink();

	if( m_bVoiceOn )
	{
		//TODO: define constants if possible - Solokiller
		const int iValue = 160 * m_pVoiceTweak->GetSpeakingVolume() / 32768;

		int iWidth = iValue + 7;

		if( iWidth < 0 )
			iWidth += 7;

		int wide, tall;
		m_pMicMeter2->GetSize( wide, tall );
		m_pMicMeter2->SetSize( iWidth, tall );
		m_pMicMeter2->Repaint();
	}
}

void COptionsSubVoice::OnCheckButtonChecked( int state )
{
	if( m_pVoiceTweak )
	{
		if( m_pMicBoost->IsSelected() != m_bMicBoostSelected )
		{
			PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
		}
	}
}

void COptionsSubVoice::OnSliderMoved( int position )
{
	if( m_pVoiceTweak )
	{
		if( m_pMicrophoneVolume->GetValue() != m_nMicVolumeValue )
		{
			PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
		}
	}
}

void COptionsSubVoice::StartTestMicrophone()
{
	if( m_pVoiceTweak && !m_bVoiceOn )
	{
		m_bVoiceOn = true;

		UseCurrentVoiceParameters();

		if( m_pVoiceTweak->StartVoiceTweakMode() )
		{
			m_pTestMicrophoneButton->SetText( "#GameUI_StopTestMicrophone" );

			m_pReceiveVolume->SetEnabled( false );
			m_pMicrophoneVolume->SetEnabled( false );
			m_pVoiceEnableCheckButton->SetEnabled( false );
			m_pMicBoost->SetEnabled( false );
			m_pMicrophoneSliderLabel->SetEnabled( false );
			m_pReceiveSliderLabel->SetEnabled( false );

			m_pMicMeter2->SetVisible( true );
		}
		else
		{
			ResetVoiceParameters();

			m_bVoiceOn = false;
		}
	}
}

void COptionsSubVoice::EndTestMicrophone()
{
	if( m_pVoiceTweak && m_bVoiceOn )
	{
		m_pVoiceTweak->EndVoiceTweakMode();

		ResetVoiceParameters();

		m_pTestMicrophoneButton->SetText( "#GameUI_TestMicrophone" );

		m_bVoiceOn = false;
		m_pReceiveVolume->SetEnabled( true );
		m_pMicrophoneVolume->SetEnabled( true );
		m_pVoiceEnableCheckButton->SetEnabled( true );
		m_pMicBoost->SetEnabled( true );
		m_pMicrophoneSliderLabel->SetEnabled( true );
		m_pReceiveSliderLabel->SetEnabled( true );
		m_pMicMeter2->SetVisible( false );
	}
}

void COptionsSubVoice::OnPageHide()
{
	if( m_bVoiceOn && m_pVoiceTweak )
	{
		EndTestMicrophone();
	}

	BaseClass::OnPageHide();
}

void COptionsSubVoice::UseCurrentVoiceParameters()
{
	m_pVoiceTweak->SetControlFloat( MicrophoneVolume, m_pMicrophoneVolume->GetValue() / 100.0 );

	m_pVoiceTweak->SetControlFloat( MicBoost, m_pMicBoost->IsEnabled() );

	m_nReceiveSliderValue = m_pReceiveVolume->GetValue();

	m_pReceiveVolume->ApplyChanges();
}

void COptionsSubVoice::OnCommand( const char* command )
{
	if( !stricmp( command, "TestMicrophone" ) )
	{
		if( m_bVoiceOn )
		{
			if( m_pVoiceTweak )
				EndTestMicrophone();
		}
		else
		{
			StartTestMicrophone();
		}
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void COptionsSubVoice::ResetVoiceParameters()
{
	m_pVoiceTweak->SetControlFloat( MicrophoneVolume, m_nMicVolumeValue / 100.0 );
	m_pVoiceTweak->SetControlFloat( MicBoost, m_bMicBoostSelected );

	engine->Cvar_SetValue( "voice_scale", m_fReceiveVolume );

	m_pReceiveVolume->Reset();
	m_pReceiveVolume->SetValue( m_nMicVolumeValue );
}
