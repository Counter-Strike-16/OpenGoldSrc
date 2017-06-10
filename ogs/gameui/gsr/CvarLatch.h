#ifndef GAMEUI_CVARLATCH_H
#define GAMEUI_CVARLATCH_H

struct CvarLatch
{
	CvarLatch( const char* cvarName, const char* defaultValue );
	~CvarLatch();

	char* m_cvarName;
	char* m_oldValue;
};

#endif //GAMEUI_CVARLATCH_H
