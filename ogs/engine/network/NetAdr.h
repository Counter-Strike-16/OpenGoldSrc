// netadr_t struct wrapper

#pragma once

typedef struct netadr_s netadr_t;

class CNetAdr
{
public:
	CNetAdr(netadr_t *apAdr) : mpAdr(apAdr){}
	~CNetAdr();
	
	bool IsLocal();
	
	char *ToString();
private:
	netadr_t *mpAdr;
};