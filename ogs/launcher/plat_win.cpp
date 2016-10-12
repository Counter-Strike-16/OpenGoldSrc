#include "platform.h"
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

int Plat_MessageBox(eMsgBoxType eType, const char *sTitle, const char *sMsg)
{
	int nFlags = MB_ICONERROR;
	
	switch(eType)
	{
	case eMsgBoxType_Error:
		nFlags = MB_ICONERROR;
	case eMsgBoxType_Warning:
		nFlags = MB_ICONWARNING | MB_OKCANCEL;
	default:
		nFlags = MB_ICONERROR;
	};
	
	return MessageBox(NULL, sMsg, sTitle, nFlags);
};

void Plat_Init()
{
	WSAData WSAData;
	WSAStartup(2, &WSAData);
};

void Plat_Shutdown()
{
	WSACleanup();
};