/*
=================
DemoApi implementation
=================
*/

demo_api_t gDemoApi =
{
	Demo_IsRecording,
	Demo_IsPlayingback,
	Demo_IsTimeDemo,
	Demo_WriteBuffer
};

/*
=================
Demo_IsRecording
=================
*/
static int Demo_IsRecording()
{
	return cls.demorecording;
};

/*
=================
Demo_IsPlayingback
=================
*/
static int Demo_IsPlayingback()
{
	return cls.demoplayback;
};

/*
=================
Demo_IsTimeDemo
=================
*/
static int Demo_IsTimeDemo()
{
	return cls.timedemo;
};

/*
=================
Demo_WriteBuffer
=================
*/
static void Demo_WriteBuffer(int anSize, byte *buffer)
{
	CL_WriteDemoUserMessage(buffer, anSize);
};