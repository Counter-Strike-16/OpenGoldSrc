#ifndef GAMEUI_TASKFRAME_H
#define GAMEUI_TASKFRAME_H

#include <vgui/ILocalize.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Frame.h>

#include "GameUI_Interface.h"
#include "Taskbar.h"

class CTaskFrame : public vgui2::Frame
{
public:
	DECLARE_CLASS_SIMPLE( CTaskFrame, vgui2::Frame );

public:
	using BaseClass::BaseClass;

	void SetTitle( const char* title, bool surfaceTitle ) override
	{
		BaseClass::SetTitle( title, surfaceTitle );

		if( g_pTaskbar )
		{
			wchar_t w_szTitle[ 1024 ];
			auto pszString = vgui2::localize()->Find( title );

			if( !pszString )
			{
				vgui2::localize()->ConvertANSIToUnicode( title, w_szTitle, sizeof( w_szTitle ) );
				pszString = w_szTitle;
			}

			g_pTaskbar->SetTitle( GetVPanel(), w_szTitle );
		}
	}

private:
	CTaskFrame( const CTaskFrame& ) = delete;
	CTaskFrame& operator=( const CTaskFrame& ) = delete;
};

#endif //GAMEUI_TASKFRAME_H
