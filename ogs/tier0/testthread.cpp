//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "platform.h"
#include "testthread.h"

struct CTestHarness 
{
#ifdef WIN32
#else
	THREAD_HANDLE m_hThreadTestDriver;
	pthread_mutex_t m_hRunTestThreadMutex;
	pthread_cond_t m_hRunTestThread;
	pthread_mutex_t m_hRunMainThreadMutex;
	pthread_cond_t m_hRunMainThread;
#endif
	bool m_bTestActive;
	bool m_bTestThreadRunning;
	bool m_bStopTestThread;
	bool m_bLetTestThreadRun;
	bool m_bTestFailed;
#ifdef WIN32
#else
	threadid_t m_ulTestThreadID;
	threadid_t m_ulMainThreadID;
#endif
	void *m_pvTestThreadArg;
	TestFunc m_pTestFunc;
};

static CTestHarness g_TestHarness;

DBG_INTERFACE void Test_RunTest( TestFunc func, void *pvArg );

DBG_INTERFACE void Test_RunFrame();

DBG_INTERFACE bool Test_IsActive()
{
	return g_TestHarness.m_bTestActive;
}

DBG_INTERFACE void Test_SetFailed()
{
	g_TestHarness.m_bTestFailed = true;
}

DBG_INTERFACE bool Test_HasFailed()
{
	return g_TestHarness.m_bTestFailed;
}

DBG_INTERFACE bool Test_HasFinished()
{
	return !g_TestHarness.m_bTestThreadRunning ? g_TestHarness.m_bTestActive : false;
}

DBG_INTERFACE void Test_TerminateThread();

DBG_INTERFACE void TestThread_Yield();