#include <KeyValues.h>

#include "CvarSlider.h"
#include "EngineInterface.h"
#include "LabeledCommandComboBox.h"
#include "ModInfo.h"
#include "OptionsSubAudio.h"

COptionsSubAudio::COptionsSubAudio( vgui2::Panel* parent )
	: BaseClass( parent, nullptr )
{
	m_pSFXSlider = new CCvarSlider( this, "SFX Slider", "#GameUI_SoundEffectVolume", 0.0, 2.0, "volume", false );
	m_pHEVSlider = new CCvarSlider( this, "Suit Slider", "#GameUI_HEVSuitVolume", 0.0, 2.0, "suitvolume", false );
	m_pMP3Slider = new CCvarSlider( this, "MP3 Volume", "#GameUI_MP3Volume", 0.0, 1.0, "mp3volume", false );

	m_pSoundQualityCombo = new CLabeledCommandComboBox( this, "Sound Quality" );

	m_pSoundQualityCombo->AddItem( "#GameUI_High", "hisound 1" );
	m_pSoundQualityCombo->AddItem( "#GameUI_Low", "hisound 0" );

	engine->pfnGetCvarFloat( "hisound" );

	m_pSoundQualityCombo->SetInitialItem( engine->pfnGetCvarFloat( "hisound" ) ? 0 : 1 );

	LoadControlSettings( "Resource\\OptionsSubAudio.res" );

	if( ModInfo().IsMultiplayerOnly() )
	{
		auto pChild = FindChildByName( "suit label" );
		if( pChild )
			pChild->SetVisible( false );

		pChild = FindChildByName( "Suit Slider" );
		if( pChild )
			pChild->SetVisible( false );
	}
}

void COptionsSubAudio::OnControlModified()
{
	PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
}

void COptionsSubAudio::OnApplyChanges()
{
	m_pSFXSlider->ApplyChanges();
	m_pHEVSlider->ApplyChanges();
	m_pMP3Slider->ApplyChanges();
	m_pSoundQualityCombo->ApplyChanges();
}

void COptionsSubAudio::OnResetData()
{
	m_pSFXSlider->Reset();
	m_pHEVSlider->Reset();
	m_pMP3Slider->Reset();
	m_pSoundQualityCombo->Reset();
}
