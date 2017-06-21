#include <KeyValues.h>

#include "labelslidercombo.h"

LabelSliderCombo::LabelSliderCombo( vgui2::Panel* parent, const char* sliderName, const char* textEntryName )
	: BaseClass( parent, sliderName )
{

	m_pTextEntry = new vgui2::TextEntry( parent, textEntryName );

	m_pTextEntry->SetEnabled( false );
	SetValue( 0 );

	UpdateTextEntry();
}

LabelSliderCombo::~LabelSliderCombo()
{
}

void LabelSliderCombo::SetVisible( bool state )
{
	BaseClass::SetVisible( state );

	if( m_pTextEntry )
		m_pTextEntry->SetVisible( state );
}

void LabelSliderCombo::UpdateTextEntry( bool bTriggerChangeMessage )
{
	const int iValue = GetValue();

	if( m_currentValueShown != iValue )
	{
		if( bTriggerChangeMessage )
		{
			PostActionSignal( new KeyValues( "ControlModified" ) );
		}

		wchar_t scratch[ 64 ];
		swprintf( scratch, ARRAYSIZE( scratch ), L"%d", iValue );

		if( m_pTextEntry )
		{
			m_pTextEntry->SetText( scratch );
			m_currentValueShown = iValue;
		}
	}
}

void LabelSliderCombo::SetValue( int value, bool bTriggerChangeMessage )
{
	BaseClass::SetValue(value, bTriggerChangeMessage );
	UpdateTextEntry( bTriggerChangeMessage );
}

void LabelSliderCombo::OnCursorMoved( int x, int y )
{
	BaseClass::OnCursorMoved( x, y );
	UpdateTextEntry();
}
