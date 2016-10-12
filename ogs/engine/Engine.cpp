#include "Engine.h"

CCEngine::CEngine()
{
};

CCEngine::~CEngine()
{
};

bool Load(bool dedicated, char *basedir, char *cmdline)
{
};

void Unload()
{
};

	void SetState(int nState);
	int GetState();

	void SetSubState(int nSubState);
	int GetSubState();

	int Frame();

	double GetFrameTime();
	double GetCurTime();

	void TrapKey_Event(int key, bool down);
	void TrapMouse_Event(int buttons, bool down);

	void StartTrapMode();
	bool IsTrapping();
	bool CheckDoneTrapping(int &buttons, int &key);

	int GetQuitting();
	void SetQuitting();