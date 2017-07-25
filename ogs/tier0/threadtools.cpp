//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include <exception>

#include "platform.h"
#include "threadtools.h"

#ifdef WIN32
//Windows header defines ARRAYSIZE, need to undefine to prevent warnings. - Solokiller
//TODO: refactor all Windows/Linux header includes - Solokiller
#undef ARRAYSIZE
#include "winlite.h"
#else
#include <pthread.h>
#endif

#ifdef WIN32
static DWORD g_ThreadMainThreadID = GetCurrentThreadId();
#else
static pthread_t g_ThreadMainThreadID = pthread_self();
#endif

static CThreadLocalPtr<CThread> g_pCurThread;

TT_INTERFACE void ThreadSleep( unsigned duration )
{
#ifdef WIN32
	Sleep( duration );
#else
	usleep( 1000 * duration );
#endif
}

TT_INTERFACE uint ThreadGetCurrentId()
{
#ifdef WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

TT_INTERFACE uint ThreadGetCurrentProcessId()
{
#ifdef WIN32
	return GetCurrentProcessId();
#else
	return 0;
#endif
}

TT_INTERFACE int ThreadGetPriority( ThreadHandle_t hThread )
{
#ifdef WIN32
	HANDLE hTargetThread = hThread;

	if( !hThread )
		hTargetThread = GetCurrentThread();

	return GetThreadPriority( hTargetThread );
#else
	return 0;
#endif
}

TT_INTERFACE bool ThreadInMainThread()
{
#ifdef WIN32
	return GetCurrentThreadId() == g_ThreadMainThreadID;
#else
	return pthread_self() == g_ThreadMainThreadID;
#endif
}

TT_INTERFACE long ThreadInterlockedIncrement( long volatile * pDest ) NOINLINE
{
#ifdef WIN32
	return InterlockedIncrement( pDest );
#else
#endif
}

TT_INTERFACE long ThreadInterlockedDecrement( long volatile * pDest ) NOINLINE
{
#ifdef WIN32
	return InterlockedDecrement( pDest );
#else
#endif
}

TT_INTERFACE long ThreadInterlockedExchange( long volatile * pDest, long value ) NOINLINE
{
#ifdef WIN32
	return InterlockedExchange( pDest, value );
#else
#endif
}

TT_INTERFACE long ThreadInterlockedExchangeAdd( long volatile * pDest, long value ) NOINLINE
{
#ifdef WIN32
	return InterlockedExchangeAdd( pDest, value );
#else
#endif
}

TT_INTERFACE long ThreadInterlockedCompareExchange( long volatile * pDest, long value, long comperand ) NOINLINE
{
#ifdef WIN32
	return InterlockedCompareExchange( pDest, value, comperand );
#else
#endif
}

TT_INTERFACE void *ThreadInterlockedExchangePointer( void * volatile * pDest, void *value ) NOINLINE
{
#ifdef WIN32
	return InterlockedExchangePointer( pDest, value );
#else
#endif
}

TT_INTERFACE void *ThreadInterlockedCompareExchangePointer( void * volatile * pDest, void *value, void *comperand ) NOINLINE
{
#ifdef WIN32
	return InterlockedCompareExchangePointer( pDest, value, comperand );
#else
#endif
}

TT_INTERFACE int64 ThreadInterlockedIncrement64( int64 volatile * pDest ) NOINLINE
{
#ifdef WIN32
	return InterlockedIncrement64( pDest );
#else
#endif
}

TT_INTERFACE int64 ThreadInterlockedDecrement64( int64 volatile * pDest ) NOINLINE
{
#ifdef WIN32
	return InterlockedDecrement64( pDest );
#else
#endif
}

TT_INTERFACE int64 ThreadInterlockedCompareExchange64( int64 volatile * pDest, int64 value, int64 comperand ) NOINLINE
{
#ifdef WIN32
	return InterlockedCompareExchange64( pDest, value, comperand );
#else
#endif
}

TT_INTERFACE int64 ThreadInterlockedExchange64( int64 volatile * pDest, int64 value ) NOINLINE
{
#ifdef WIN32
	return InterlockedExchange64( pDest, value );
#else
#endif
}

TT_INTERFACE int64 ThreadInterlockedExchangeAdd64( int64 volatile * pDest, int64 value ) NOINLINE
{
#ifdef WIN32
	return InterlockedExchangeAdd64( pDest, value );
#else
#endif
}

CThreadLocalBase::CThreadLocalBase()
{
	m_index = TlsAlloc();

	AssertMsg( m_index != -1, "Bad thread local" );
}

CThreadLocalBase::~CThreadLocalBase()
{
	if( m_index != -1 )
		TlsFree( m_index );

	m_index = -1;
}

void * CThreadLocalBase::Get() const
{
	if( m_index != -1 )
	{
		return TlsGetValue( m_index );
	}
	else
	{
		AssertMsg( false, "Bad thread local" );
		return nullptr;
	}
}

void CThreadLocalBase::Set( void * value )
{
	if( m_index == -1 )
	{
		AssertMsg( false, "Bad thread local" );
	}
	else
	{
		TlsSetValue( m_index, value );
	}
}

bool CThreadMutex::TryLock()
{
#ifdef WIN32
	return TryEnterCriticalSection( reinterpret_cast<LPCRITICAL_SECTION>( m_CriticalSection ) ) != FALSE;
#else
	return EBUSY != pthread_mutex_trylock( &m_Mutex );
#endif
}

CThreadSyncObject::CThreadSyncObject()
{
#ifdef WIN32
	m_hSyncObject = NULL;
	m_bInitalized = true;
#else
	m_bInitalized = false;
#endif
}

CThreadSyncObject::~CThreadSyncObject()
{
#ifdef WIN32
	if( m_hSyncObject )
	{
		if( m_bInitalized )
		{
			BOOL bResult = CloseHandle( m_hSyncObject );
			Assert( bResult );
		}
	}
#else
	if( m_bInitalized )
	{
		pthread_cond_destroy( &m_Condition );
		pthread_mutex_destroy( &m_Mutex );
		m_bInitalized = false;
	}
#endif
}

bool CThreadSyncObject::operator!() const
{
#ifdef WIN32
	return m_hSyncObject == NULL;
#else
	return !m_bInitalized;
#endif
}

bool CThreadSyncObject::Wait( unsigned int dwMilliseconds )
{
	AssertUseable();
#ifdef WIN32
	return WaitForSingleObject( m_hSyncObject, dwMilliseconds ) == 0;
#else
	//TODO: needs class declaration update - Solokiller
	bool v2; // al@2
	bool v3; // dl@2
	bool v4; // ST14_1@6
	unsigned int v6; // ebp@12
	uint32 v7; // edi@12
	unsigned __int64 nNanoSec; // ST20_8@13
	timeval tv; // [sp+2Ch] [bp-30h]@11
	volatile timespec tm; // [sp+34h] [bp-28h]@13
	volatile int ret; // [sp+3Ch] [bp-20h]@8

	pthread_mutex_lock( &m_Mutex );
	if( this->m_cSet <= 0 )
	{
		if( dwTimeout )
		{
			ret = 0;
			v3 = 0;
			while( 1 )
			{
				do
				{
					if( this->m_bWakeForEvent || ret == 110 )
					{
						v2 = this->m_bManualReset;
						goto LABEL_22;
					}
					gettimeofday( &tv, 0 );
					if( dwTimeout != -1 || ( v6 = 0, v7 = 10000000, !this->m_bManualReset ) )
					{
						v6 = 1000000 * ( unsigned __int64 ) dwTimeout >> 32;
						v7 = 1000000 * dwTimeout;
					}
					nNanoSec = __PAIR__( v6, v7 ) + 1000LL * tv.tv_usec;
					tm.tv_sec = tv.tv_sec + nNanoSec / 0x3B9ACA00;
					tm.tv_nsec = nNanoSec % 0x3B9ACA00;
					do
						ret = pthread_cond_timedwait( &this->m_Condition, &this->m_Mutex, ( const struct timespec * )&tm );
					while( ret == 4 );
					v3 = ret == 0;
				}
				while( !this->m_bManualReset );
				if( this->m_cSet )
					break;
				if( dwTimeout == -1 && ret == 110 )
					ret = 0;
			}
			v2 = 1;
		LABEL_22:
			if( v3 )
				this->m_bWakeForEvent = 0;
		}
		else
		{
			v2 = this->m_bManualReset;
			v3 = 0;
		}
	}
	else
	{
		v2 = this->m_bManualReset;
		v3 = 1;
		this->m_bWakeForEvent = 0;
	}
	if( !v2 && v3 )
		this->m_cSet = 0;
	v4 = v3;
	pthread_mutex_unlock( &this->m_Mutex );
	return v4;
#endif
}

void CThreadSyncObject::AssertUseable()
{
	AssertMsg( m_hSyncObject, "Thread synchronization object is unuseable" );
}

CThreadFullMutex::CThreadFullMutex( bool bEstablishInitialOwnership, const char * pszName )
{
#ifdef WIN32
	m_hSyncObject = CreateMutexA( nullptr, bEstablishInitialOwnership, pszName );

	AssertMsg1( m_hSyncObject, "Failed to create mutex (error 0x%x)", GetLastError() );
#else
#error
#endif
}

bool CThreadFullMutex::Release()
{
#ifdef WIN32
	return ReleaseMutex( m_hSyncObject ) != 0;
#else
#endif
}

CThreadEvent::CThreadEvent( bool fManualReset )
{
	m_hSyncObject = CreateEventA( nullptr, fManualReset, FALSE, nullptr );
	AssertMsg1( m_hSyncObject != NULL, "Failed to create event (error 0x%x)", GetLastError() );
}

CThreadEvent::CThreadEvent( void* pEvent, bool fManualReset )
{
	m_hSyncObject = pEvent;
	m_bInitalized = fManualReset;

	AssertFatalMsg1( m_hSyncObject != NULL, "Bad Event handle (handle 0x%x)", m_hSyncObject );
}

bool CThreadEvent::Set()
{
	AssertUseable();
	return SetEvent( m_hSyncObject ) != FALSE;
}

bool CThreadEvent::Wait( uint32 dwTimeout )
{
	AssertUseable();
	return WaitForSingleObject( m_hSyncObject, dwTimeout ) == WAIT_OBJECT_0;
}

CThread::CThread()
{
	m_hThread = NULL;
	m_threadId = 0;
	m_result = 0;
	m_szName[ 0 ] = '\0';
	m_pStackBase = nullptr;
}

CThread::~CThread()
{
	if( m_hThread != NULL )
	{
		DWORD ExitCode;

		if( GetExitCodeThread( m_hThread, &ExitCode ) && ExitCode == STILL_ACTIVE )
		{
			AssertMsg( false, "Illegal termination of worker thread!." );
		}

		Stop();

		if( m_hThread != NULL &&
			GetExitCodeThread( m_hThread, &ExitCode ) && ExitCode == STILL_ACTIVE &&
			static_cast<CThread*>( g_pCurThread ) != this )
		{
			Join();
		}
	}
}

const char* CThread::GetName()
{
	m_Lock.Lock();

	if( !*( m_szName ) )
	{
		snprintf( m_szName, ARRAYSIZE( m_szName ), "Thread(0x%x/0x%x)", reinterpret_cast<unsigned int>( this ), static_cast<unsigned int>( m_threadId ) );
	}

	m_Lock.Unlock();

	return m_szName;
}

void CThread::SetName( const char* pszName )
{
	m_Lock.Lock();

	strncpy( m_szName, pszName, ARRAYSIZE( m_szName ) - 1 );
	m_szName[ ARRAYSIZE( m_szName ) - 1 ] = '\0';

	m_Lock.Unlock();
}

bool CThread::Start( unsigned nBytesStack )
{
	DWORD ExitCode;

	m_Lock.Lock();

	if( m_hThread && GetExitCodeThread( m_hThread, &ExitCode ) && ExitCode == STILL_ACTIVE )
	{
		AssertFatalMsg( false, "Tried to create a thread that has already been created!" );
		m_Lock.Unlock();
		return false;
	}

	CThreadEvent event( false );

	bool bInitSuccess = false;

	ThreadInit_t* pInit = new ThreadInit_t;

	if( pInit )
	{
		pInit->pThread = this;
		pInit->pInitCompleteEvent = &event;
		pInit->pfInitSuccess = &bInitSuccess;
	}

	m_hThread = CreateThread( nullptr, nBytesStack, ( LPTHREAD_START_ROUTINE )( GetThreadProc() ), pInit, 0, &m_threadId );

	if( m_hThread == NULL )
	{
		AssertMsg1( false, "Failed to create thread (error 0x%x)", GetLastError() );
		m_Lock.Unlock();
		return false;
	}

	if( !WaitForCreateComplete( &event ) )
	{
		Msg( "Thread failed to initialize\n" );
		m_hThread = NULL;
		m_Lock.Unlock();
		return false;
	}

	if( !bInitSuccess )
	{
		Msg( "Thread failed to initialize (2)\n" );
		m_hThread = NULL;
		m_Lock.Unlock();
		return false;
	}

	if( m_hThread == NULL )
	{
		Msg( "Thread exited immediately\n" );
	}

	m_Lock.Unlock();

	return m_hThread != NULL;
}

bool CThread::IsAlive()
{
	DWORD exitCode;

	return m_hThread && GetExitCodeThread( m_hThread, &exitCode ) && exitCode == STILL_ACTIVE;
}

bool CThread::Join( unsigned timeout )
{
	if( m_hThread == NULL )
		return true;

	AssertMsg( static_cast<CThread*>( g_pCurThread ) != this, "Thread cannot be joined with self" );

	const auto dwWait = WaitForSingleObject( m_hThread, timeout );
	if( dwWait == WAIT_TIMEOUT )
		return false;

	if( dwWait == WAIT_FAILED )
	{
		Assert( !"\"CThread::Join WAIT_FAILED\"" );

		return true;
	}

	Assert( dwWait == WAIT_OBJECT_0 );

	return dwWait == WAIT_OBJECT_0;
}

#ifdef WIN32
HANDLE CThread::GetThreadHandle()
{
	return m_hThread;
}

uint CThread::GetThreadId()
{
	return m_threadId;
}
#endif

int CThread::GetResult()
{
	return m_result;
}

void CThread::Stop( int exitCode )
{
	DWORD ExitCode;

	if( m_hThread != NULL && GetExitCodeThread( m_hThread, &ExitCode ) && ExitCode == STILL_ACTIVE )
	{
		m_Lock.Lock();

		if( static_cast<CThread*>( g_pCurThread ) == this )
		{
			throw std::exception();
		}

		AssertMsg( false, "Only thread can stop self: Use a higher-level protocol" );
		
		m_Lock.Unlock();
	}
}

bool CThread::Init()
{
	return true;
}

void CThread::OnExit()
{
}

#ifdef WIN32
bool CThread::WaitForCreateComplete( CThreadEvent *pEvent )
{
	if( !pEvent->Wait( 60000 ) )
	{
		AssertMsg( false, "Probably deadlock or failure waiting for thread to initialize." );
		return false;
	}
	else
	{
		return true;
	}
}
#endif

CThread::ThreadProc_t CThread::GetThreadProc()
{
	return ThreadProc;
}

unsigned __stdcall CThread::ThreadProc( void * pv )
{
	ThreadInit_t* pData = reinterpret_cast<ThreadInit_t*>( pv );

	g_pCurThread = pData->pThread;

	if( pData->pfInitSuccess )
		*pData->pfInitSuccess = false;

	bool bInitSuccess = pData->pThread->Init();

	if( pData->pfInitSuccess )
		*pData->pfInitSuccess = bInitSuccess;

	pData->pInitCompleteEvent->Set();

	unsigned result = 0;

	if( bInitSuccess )
	{
		pData->pThread->m_result = -1;

		pData->pThread->m_result = pData->pThread->Run();

		pData->pThread->OnExit();

		g_pCurThread = nullptr;

		pData->pThread->m_threadId = 0;

		result = pData->pThread->m_result;
	}

	delete pData;

	return result;
}
