#include <KeyValues.h>

#include "CvarSlider.h"
#include "EngineInterface.h"

CCvarSlider::CCvarSlider( vgui2::Panel* parent, const char* panelName, const char* caption, float minValue, float maxValue, const char* cvarname, bool bAllowOutOfRange )
	: BaseClass( parent, panelName )
{
	AddActionSignalTarget( this );

	SetRange(
		static_cast<int>( floor( minValue * 100.0 ) ),
		static_cast<int>( floor( maxValue * 100.0 ) )
	);

	char szMin[ 32 ];
	char szMax[ 32 ];
	snprintf( szMin, ARRAYSIZE( szMin ), "%.2f", minValue );
	snprintf( szMax, ARRAYSIZE( szMax ), "%.2f", maxValue );
	SetTickCaptions( szMin, szMax );

	strcpy( m_szCvarName, cvarname );

	m_bAllowOutOfRange = bAllowOutOfRange;
	m_bModifiedOnce = false;

	m_fStartValue = engine->pfnGetCvarFloat( m_szCvarName );
	m_fCurrentValue = m_fStartValue;

	SetValue( static_cast<int>( floor( m_fStartValue * 100.0 ) ) );
	m_iStartValue = GetValue();
	m_iLastSliderValue = m_iStartValue;
}

CCvarSlider::~CCvarSlider()
{
}

void CCvarSlider::Paint()
{
	const auto flValue = engine->pfnGetCvarFloat( m_szCvarName );

	if( m_fStartValue != flValue )
	{
		m_fCurrentValue = m_fStartValue = flValue;

		SetValue( static_cast<int>( floor( flValue * 100.0 ) ) );
		m_iStartValue = GetValue();
	}

	BaseClass::Paint();
}

void CCvarSlider::OnSliderMoved()
{
	if( GetValue() == m_iStartValue )
	{
		if( !m_bModifiedOnce )
			return;
	}
	else
	{
		m_bModifiedOnce = true;
	}

	if( m_iLastSliderValue != GetValue() )
	{
		m_iLastSliderValue = GetValue();
		m_fCurrentValue = m_iLastSliderValue / 100.0;
	}

	PostActionSignal( new KeyValues( "ControlModified" ) );
}

void CCvarSlider::ApplyChanges()
{
	if( m_bModifiedOnce )
	{
		m_iStartValue = GetValue();

		if( !m_bAllowOutOfRange )
		{
			m_fStartValue = m_iStartValue / 100.0;
		}
		else
		{
			m_fStartValue = m_fCurrentValue;
		}

		engine->Cvar_SetValue( m_szCvarName, m_fStartValue );
	}
}

float CCvarSlider::GetSliderValue()
{
	if( m_bAllowOutOfRange )
		return m_fCurrentValue;
	else
		return GetValue() / 100.0;
}

void CCvarSlider::SetSliderValue( float fValue )
{
	SetValue( static_cast<int>( floor( 100.0 * fValue ) ), false );

	m_iLastSliderValue = GetValue();

	if( m_fCurrentValue != fValue )
	{
		m_fCurrentValue = fValue;
		m_bModifiedOnce = true;
	}
}

void CCvarSlider::Reset()
{
	m_fStartValue = engine->pfnGetCvarFloat( m_szCvarName );
	m_fCurrentValue = m_fStartValue;

	SetValue( static_cast<int>( floor( m_fStartValue * 100.0 ) ) );
	m_iStartValue = GetValue();
	m_iLastSliderValue = m_iStartValue;
}

bool CCvarSlider::HasBeenModified()
{
	if( GetValue() == m_iStartValue )
	{
		return m_bModifiedOnce;
	}
	else
	{
		m_bModifiedOnce = true;
		return true;
	}
}
