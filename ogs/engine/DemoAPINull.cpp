#include "demo_api.h"

namespace
{
int DemoAPI_IsRecording()
{
	return 0;
};

int DemoAPI_IsPlayingback()
{
	return 0;
};

int DemoAPI_IsTimeDemo()
{
	return 0;
};

void DemoAPI_WriteBuffer(int size, unsigned char *buffer)
{
};
}; // namespace

static demo_api_t *gpDemoAPINull =
{
	DemoAPI_IsRecording,
	DemoAPI_IsPlayingback,
	DemoAPI_IsTimeDemo,
	
	DemoAPI_WriteBuffer
};