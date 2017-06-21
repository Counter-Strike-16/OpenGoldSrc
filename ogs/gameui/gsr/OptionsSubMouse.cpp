#include <KeyValues.h>
#include <vgui_controls/TextEntry.h>

#include "CvarNegateCheckButton.h"
#include "CvarSlider.h"
#include "CvarToggleCheckButton.h"
#include "EngineInterface.h"
#include "KeyToggleCheckButton.h"

#include "OptionsSubMouse.h"

COptionsSubMouse::COptionsSubMouse( vgui2::Panel* parent )
	: BaseClass( parent, nullptr )
{
	m_pReverseMouseCheckBox = new CCvarNegateCheckButton( this, "ReverseMouse", "#GameUI_ReverseMouse", "m_pitch" );
	m_pMouseLookCheckBox = new CKeyToggleCheckButton( this, "MouseLook", "#GameUI_MouseLook", "in_mlook", "mlook" );
	m_pMouseFilterCheckBox = new CCvarToggleCheckButton( this, "MouseFilter", "#GameUI_MouseFilter", "m_filter" );
	m_pJoystickCheckBox = new CCvarToggleCheckButton( this, "Joystick", "#GameUI_Joystick", "joystick" );
	m_pJoystickLookCheckBox = new CKeyToggleCheckButton( this, "JoystickLook", "#GameUI_JoystickLook", "in_jlook", "jlook" );
	m_pMouseSensitivitySlider = new CCvarSlider( this, "Slider", "#GameUI_MouseSensitivity", 0.2, 20.0, "sensitivity", 1 );
	m_pMouseRawInputCheckBox = new CCvarToggleCheckButton( this, "RawInput", "#GameUI_RawInput", "m_rawinput" );
	m_pMouseSensitivityLabel = new vgui2::TextEntry( this, "SensitivityLabel" );
	m_pMouseSensitivityLabel->AddActionSignalTarget( this );
	m_pAutoAimCheckBox = new CCvarToggleCheckButton( this, "Auto-Aim", "#GameUI_AutoAim", "sv_aim" );

	LoadControlSettings( "Resource\\OptionsSubMouse.res" );

	char buf[ 64 ];
	snprintf( buf, ARRAYSIZE( buf ), " %.1f", engine->pfnGetCvarFloat( "sensitivity" ) );

	m_pMouseSensitivityLabel->SetText( buf );
}

void COptionsSubMouse::ApplySchemeSettings( vgui2::IScheme* pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_pMouseSensitivityLabel->SetBorder( pScheme->GetBorder( "ButtonDepressedBorder" ) );
}

void COptionsSubMouse::OnApplyChanges()
{
	m_pReverseMouseCheckBox->ApplyChanges();
	m_pMouseLookCheckBox->ApplyChanges();
	m_pMouseFilterCheckBox->ApplyChanges();
	m_pJoystickCheckBox->ApplyChanges();
	m_pJoystickLookCheckBox->ApplyChanges();
	m_pMouseSensitivitySlider->ApplyChanges();
	m_pAutoAimCheckBox->ApplyChanges();
	m_pMouseRawInputCheckBox->ApplyChanges();
}

void COptionsSubMouse::OnResetData()
{
	m_pReverseMouseCheckBox->Reset();
	m_pMouseLookCheckBox->Reset();
	m_pMouseFilterCheckBox->Reset();
	m_pJoystickCheckBox->Reset();
	m_pJoystickLookCheckBox->Reset();
	m_pMouseSensitivitySlider->Reset();
	m_pMouseRawInputCheckBox->Reset();
	m_pAutoAimCheckBox->Reset();
}

void COptionsSubMouse::OnTextChanged( vgui2::Panel* panel )
{
	if( m_pMouseSensitivityLabel == panel )
	{
		char buf[ 64 ];
		m_pMouseSensitivityLabel->GetText( buf, ARRAYSIZE( buf ) );

		const auto flValue = atof( buf );

		if( flValue >= 1.0 )
		{
			m_pMouseSensitivitySlider->SetSliderValue( flValue );
			PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );
		}
	}
}

void COptionsSubMouse::OnControlModified( vgui2::Panel* panel )
{
	PostActionSignal( new KeyValues( "ApplyButtonEnable" ) );

	if( m_pMouseSensitivitySlider == panel
		&& m_pMouseSensitivitySlider->HasBeenModified() )
	{
		UpdateSensitivityLabel();
	}
}

void COptionsSubMouse::UpdateSensitivityLabel()
{
	char buf[ 64 ];
	snprintf( buf, ARRAYSIZE( buf ), " %.1f", m_pMouseSensitivitySlider->GetSliderValue() );

	m_pMouseSensitivityLabel->SetText( buf );
}
