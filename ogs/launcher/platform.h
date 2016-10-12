#ifndef PLATFORM_H
#define PLATFORM_H

int Plat_MessageBox(eMsgBoxType eType, const char *sTitle, const char *sMsg);

void Plat_Init();
void Plat_Shutdown();

void Plat_Main();

#endif // PLATFORM_H