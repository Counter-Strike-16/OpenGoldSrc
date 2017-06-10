#ifndef GAMEUI_CONTENTCONTROLDIALOG_H
#define GAMEUI_CONTENTCONTROLDIALOG_H

#include "taskframe.h"

class CContentControlDialog : public CTaskFrame
{
private:
	enum
	{
		MAX_GORE_PW = 64
	};

	using BaseClass = CTaskFrame;

public:
	CContentControlDialog( vgui2::Panel* parent );

	void OnClose() override;

	void ResetPassword();

	void Explain( const char* fmt, ... );

	char* BinPrintf( byte* buf, int nLen );

	void HashPassword( const char* newPW, char* hashBuffer );

	bool EnablePassword( const char* newPW );
	bool DisablePassword( const char* oldPW );

	void OnCommand( const char* command ) override;

	bool IsPasswordEnabledInDialog();

	void UpdateContentControlStatus();

	void WriteToken( const char* str );

	void Activate() override;

	void ApplyPassword();

private:
	char m_szGorePW[ MAX_GORE_PW ];
	bool m_bDefaultPassword;

	vgui2::Label* m_pStatus;
	vgui2::Button* m_pOK;
	vgui2::TextEntry* m_pPassword;
	vgui2::Label* m_pPasswordLabel;
	vgui2::Label* m_pPassword2Label;
	vgui2::TextEntry* m_pPassword2;
	vgui2::Label* m_pExplain;

private:
	CContentControlDialog( const CContentControlDialog& ) = delete;
	CContentControlDialog& operator=( const CContentControlDialog& ) = delete;
};


#endif //GAMEUI_CONTENTCONTROLDIALOG_H
