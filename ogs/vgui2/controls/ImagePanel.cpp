//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include <stdio.h>

#include <vgui/IBorder.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/IBorder.h>
#include <KeyValues.h>

#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Image.h>
#include <vgui_controls/Controls.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui2;

DECLARE_BUILD_FACTORY( ImagePanel );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ImagePanel::ImagePanel(Panel *parent, const char *name) : Panel(parent, name)
{
	m_pImage = NULL;
	m_pszImageName = NULL;
	m_pszFillColorName = NULL;
	m_pszDrawColorName = NULL;	// HPE addition
	m_bCenterImage = false;
	m_bScaleImage = false;
	m_bTileImage = false;
	m_bTileHorizontally = false;
	m_bTileVertically = false;
	m_fScaleAmount = 0.0f;
	m_FillColor = SDK_Color(0, 0, 0, 0);
	m_DrawColor = SDK_Color( 255, 255, 255, 255 );

	SetImage(m_pImage);

	REGISTER_COLOR_AS_OVERRIDABLE( m_FillColor, "fillcolor_override" );
	REGISTER_COLOR_AS_OVERRIDABLE( m_DrawColor, "drawcolor_override" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ImagePanel::~ImagePanel()
{
	delete [] m_pszImageName;
	delete [] m_pszFillColorName;
	delete [] m_pszDrawColorName;	// HPE addition
}

//-----------------------------------------------------------------------------
// Purpose: handles size changing
//-----------------------------------------------------------------------------
void ImagePanel::OnSizeChanged(int newWide, int newTall)
{
	if (m_bScaleImage && m_pImage)
	{
		// scaling, force the image size to be our size
		m_pImage->SetSize(newWide, newTall);
	}
	BaseClass::OnSizeChanged(newWide, newTall);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImagePanel::SetImage(IImage *image)
{
	m_pImage = image;

	//Image scaling is now handled differently since Source 2013's code handles it better. See PaintBackground. - Solokiller
	/*
	if (m_pImage)
	{
		int wide, tall;
		if (m_bScaleImage)
		{
			// scaling, force the image size to be our size
			GetSize(wide, tall);
			m_pImage->SetSize(wide, tall);
		}
		else
		{
			// not scaling, so set our size to the image size
			m_pImage->GetSize(wide, tall);
			SetSize(wide, tall);
		}
	}
	*/

	Repaint();
}

//-----------------------------------------------------------------------------
// Purpose: sets an image by file name
//-----------------------------------------------------------------------------
void ImagePanel::SetImage(const char *imageName)
{
	int len = Q_strlen(imageName) + 1;
	m_pszImageName = new char[ len ];
	Q_strncpy(m_pszImageName, imageName, len );
	InvalidateLayout(false, true); // forrce applyschemesettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
IImage *ImagePanel::GetImage()
{
	return m_pImage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
SDK_Color ImagePanel::GetDrawColor( void )
{
	return m_DrawColor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImagePanel::SetDrawColor( SDK_Color drawColor )
{
	m_DrawColor = drawColor;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ImagePanel::PaintBackground()
{
	if( m_FillColor[ 3 ] > 0 )
	{
		// draw the specified fill color
		int wide, tall;
		GetSize( wide, tall );
		surface()->DrawSetColor( m_FillColor );
		surface()->DrawFilledRect( 0, 0, wide, tall );
	}
	if( m_pImage )
	{
		//=============================================================================
		// HPE_BEGIN:
		// [pfreese] Color should be always set from GetDrawColor(), not just when 
		// scaling is true (see previous code)
		//=============================================================================

		// surface()->DrawSetColor( 255, 255, 255, GetAlpha() );
		m_pImage->SetColor( GetDrawColor() );

		//=============================================================================
		// HPE_END
		//=============================================================================

		if( m_bCenterImage )
		{
			int wide, tall;
			GetSize( wide, tall );

			int imageWide, imageTall;
			m_pImage->GetSize( imageWide, imageTall );

			if( m_bScaleImage && m_fScaleAmount > 0.0f )
			{
				imageWide = static_cast<int>( static_cast<float>( imageWide ) * m_fScaleAmount );
				imageTall = static_cast<int>( static_cast<float>( imageTall ) * m_fScaleAmount );
			}

			m_pImage->SetPos( ( wide - imageWide ) / 2, ( tall - imageTall ) / 2 );
		}
		else
		{
			m_pImage->SetPos( 0, 0 );
		}

		if( m_bScaleImage )
		{
			// Image size is stored in the bitmap, so temporarily set its size
			// to our panel size and then restore after we draw it.

			int imageWide, imageTall;
			m_pImage->GetSize( imageWide, imageTall );

			if( m_fScaleAmount > 0.0f )
			{
				float wide, tall;
				wide = static_cast<float>( imageWide ) * m_fScaleAmount;
				tall = static_cast<float>( imageTall ) * m_fScaleAmount;
				m_pImage->SetSize( static_cast<int>( wide ), static_cast<int>( tall ) );
			}
			else
			{
				int wide, tall;
				GetSize( wide, tall );
				m_pImage->SetSize( wide, tall );
			}

			m_pImage->Paint();

			m_pImage->SetSize( imageWide, imageTall );
		}
		else if( m_bTileImage || m_bTileHorizontally || m_bTileVertically )
		{
			int wide, tall;
			GetSize( wide, tall );
			int imageWide, imageTall;
			m_pImage->GetSize( imageWide, imageTall );

			int y = 0;
			while( y < tall )
			{
				int x = 0;
				while( x < wide )
				{
					m_pImage->SetPos( x, y );
					m_pImage->Paint();

					x += imageWide;

					if( !m_bTileHorizontally )
						break;
				}

				y += imageTall;

				if( !m_bTileVertically )
					break;
			}
			m_pImage->SetPos( 0, 0 );
		}
		else
		{
			m_pImage->SetColor( GetDrawColor() );
			m_pImage->Paint();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets control settings for editing
//-----------------------------------------------------------------------------
void ImagePanel::GetSettings(KeyValues *outResourceData)
{
	BaseClass::GetSettings(outResourceData);
	if (m_pszImageName)
	{
		outResourceData->SetString("image", m_pszImageName);
	}
	if ( m_pszFillColorName )
	{
		outResourceData->SetString("fillcolor", m_pszFillColorName );
	}

	//=============================================================================
	// HPE_BEGIN:
	// [pfreese] Added support for specifying drawcolor
	//=============================================================================
	if( m_pszDrawColorName )
	{
		outResourceData->SetString( "drawcolor", m_pszDrawColorName );
	}
	//=============================================================================
	// HPE_END
	//=============================================================================

	if (GetBorder())
	{
		outResourceData->SetString("border", GetBorder()->GetName());
	}

	outResourceData->SetInt( "centerImage", m_bCenterImage );
	outResourceData->SetInt("scaleImage", m_bScaleImage);
	outResourceData->SetFloat( "scaleAmount", m_fScaleAmount );
	outResourceData->SetInt( "tileImage", m_bTileImage );
	outResourceData->SetInt( "tileHorizontally", m_bTileHorizontally );
	outResourceData->SetInt( "tileVertically", m_bTileVertically );
}

//-----------------------------------------------------------------------------
// Purpose: Applies designer settings from res file
//-----------------------------------------------------------------------------
void ImagePanel::ApplySettings(KeyValues *inResourceData)
{
	delete [] m_pszImageName;
	delete [] m_pszFillColorName;
	delete [] m_pszDrawColorName;	// HPE addition
	m_pszImageName = NULL;
	m_pszFillColorName = NULL;
	m_pszDrawColorName = NULL;		// HPE addition

	//Center image isn't implemented in Source 2013. - Solokiller
	m_bCenterImage = inResourceData->GetInt( "centerImage", 0 );
	m_bScaleImage = inResourceData->GetInt("scaleImage", 0);
	m_fScaleAmount = inResourceData->GetFloat( "scaleAmount", 0.0f );
	m_bTileImage = inResourceData->GetInt( "tileImage", 0 );
	m_bTileHorizontally = inResourceData->GetInt( "tileHorizontally", m_bTileImage );
	m_bTileVertically = inResourceData->GetInt( "tileVertically", m_bTileImage );
	const char *imageName = inResourceData->GetString("image", "");
	if (*imageName)
	{
		SetImage( imageName );
	}

	const char *pszFillColor = inResourceData->GetString("fillcolor", "");
	if (*pszFillColor)
	{
		int r = 0, g = 0, b = 0, a = 255;
		int len = Q_strlen(pszFillColor) + 1;
		m_pszFillColorName = new char[ len ];
		Q_strncpy( m_pszFillColorName, pszFillColor, len );

		if (sscanf(pszFillColor, "%d %d %d %d", &r, &g, &b, &a) >= 3)
		{
			// it's a direct color
			m_FillColor = SDK_Color(r, g, b, a);
		}
		else
		{
			IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
			m_FillColor = pScheme->GetColor(pszFillColor, SDK_Color(0, 0, 0, 0));
		}
	}

	//=============================================================================
	// HPE_BEGIN:
	// [pfreese] Added support for specifying drawcolor
	//=============================================================================
	const char *pszDrawColor = inResourceData->GetString( "drawcolor", "" );
	if( *pszDrawColor )
	{
		int r = 255, g = 255, b = 255, a = 255;
		int len = Q_strlen( pszDrawColor ) + 1;
		m_pszDrawColorName = new char[ len ];
		Q_strncpy( m_pszDrawColorName, pszDrawColor, len );

		if( sscanf( pszDrawColor, "%d %d %d %d", &r, &g, &b, &a ) >= 3 )
		{
			// it's a direct color
			m_DrawColor = SDK_Color( r, g, b, a );
		}
		else
		{
			IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
			m_DrawColor = pScheme->GetColor( pszDrawColor, SDK_Color( 255, 255, 255, 255 ) );
		}
	}
	//=============================================================================
	// HPE_END
	//=============================================================================

	const char *pszBorder = inResourceData->GetString("border", "");
	if (*pszBorder)
	{
		IScheme *pScheme = scheme()->GetIScheme( GetScheme() );
		SetBorder(pScheme->GetBorder(pszBorder));
	}

	BaseClass::ApplySettings(inResourceData);
}

//-----------------------------------------------------------------------------
// Purpose:  load the image, this is done just before this control is displayed
//-----------------------------------------------------------------------------
void ImagePanel::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings(pScheme);
	if ( m_pszImageName && strlen( m_pszImageName ) > 0 )
	{
		SetImage(scheme()->GetImage(m_pszImageName, m_bScaleImage));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Describes editing details
//-----------------------------------------------------------------------------
const char *ImagePanel::GetDescription()
{
	static char buf[1024];
	V_sprintf_safe(buf, "%s, string image, string border, string fillcolor, bool scaleImage", BaseClass::GetDescription());
	return buf;
}

//-----------------------------------------------------------------------------
// Purpose: sets whether or not the image should scale to fit the size of the ImagePanel (defaults to false)
//-----------------------------------------------------------------------------
void ImagePanel::SetShouldScaleImage( bool state )
{
	m_bScaleImage = state;
}

//-----------------------------------------------------------------------------
// Purpose: set the color to fill with, if no Image is specified
//-----------------------------------------------------------------------------
void ImagePanel::SetFillColor( SDK_Color col )
{
	m_FillColor = col;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
SDK_Color ImagePanel::GetFillColor()
{
	return m_FillColor;
}

char *ImagePanel::GetImageName()
{
	return m_pszImageName;
}
