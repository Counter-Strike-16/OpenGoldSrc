#pragma once

class CMenu
{
public:
	void Init();
	
	void Draw();
	
	void Menu_Main_f();
	
	void ForceMenuOff();
	
	void AddToServerList(netadr_t adr, char *info);
};