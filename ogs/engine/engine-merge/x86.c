
#include <stdlib.h>
#include "client.h"

#if id386

static unsigned long bias;
static unsigned long *histogram;
static unsigned long start, range;
static unsigned long bias;

__declspec( naked ) void x86_TimerStart( void )
{
	__asm _emit 0fh
	__asm _emit 31h
	__asm mov  start, eax
	__asm ret
}

__declspec( naked ) void x86_TimerStop( void )
{
	__asm push edi
	__asm mov edi, histogram
	__asm _emit 0fh
	__asm _emit 31h
	__asm sub eax, start
	__asm sub eax, bias
	__asm js discard
	__asm cmp eax, range
	__asm jge  discard
	__asm lea edi, [edi + eax*4]
	__asm inc dword ptr [edi]
discard:
	__asm pop edi
	__asm ret
}

#pragma warning( disable: 4035 )
static __declspec( naked ) unsigned long x86_TimerStopBias( void )
{
	__asm push edi
	__asm mov edi, histogram
	__asm _emit 0fh
	__asm _emit 31h
	__asm sub eax, start
	__asm pop edi
	__asm ret
}
#pragma warning( default:4035 )

void x86_TimerInit( unsigned long smallest, unsigned length )
{
	int i;
	unsigned long biastable[100];

	range = length;
	bias = 10000;

	for ( i = 0; i < 100; i++ )
	{
		x86_TimerStart();
		biastable[i] = x86_TimerStopBias();

		if ( bias > biastable[i] )
			bias = biastable[i];
	}

	bias += smallest;
	histogram = Z_Malloc( range * sizeof( unsigned long ) );
}

unsigned long *x86_TimerGetHistogram( void )
{
	return histogram;
}

#endif
