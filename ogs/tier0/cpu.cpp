//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: win32 dependant ASM code for CPU capability detection
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//
#include "platform.h"

#ifdef POSIX
#include "cpu_posix.cpp"
#elif _WIN32

#undef ARRAYSIZE
#include "winlite.h"

#pragma optimize( "", off )
#pragma warning( disable: 4800 ) //'int' : forcing value to bool 'true' or 'false' (performance warning)

// --------------------------------------------------------------------------
bool CheckMMXTechnology( void )
{
	int retval = true;
	unsigned int RegEDX = 0;

#ifdef CPUID
	_asm pushad;
#endif

	__try
	{
		_asm
		{
#ifdef CPUID
			xor edx, edx	// Clue the compiler that EDX is about to be used.
#endif
			mov eax, 1      // set up CPUID to return processor version and features
							//      0 = vendor string, 1 = version info, 2 = cache info
							CPUID           // code bytes = 0fh,  0a2h
							mov RegEDX, edx // features returned in edx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		retval = false;
	}

	// If CPUID not supported, then certainly no MMX extensions.
	if( retval )
	{
		if( RegEDX & 0x800000 )          // bit 23 is set for MMX technology
		{
			__try
			{
				// try executing the MMX instruction "emms"
				_asm EMMS
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				retval = false;
			}
		}

		else
			retval = false;           // processor supports CPUID but does not support MMX technology

									  // if retval == 0 here, it means the processor has MMX technology but
									  // floating-point emulation is on; so MMX technology is unavailable
	}

#ifdef CPUID
	_asm popad;
#endif

	return retval;
}
// --------------------------------------------------------------------------
bool CheckSSETechnology( void )
{
	int retval = true;
	unsigned int RegEDX = 0;

#ifdef CPUID
	_asm pushad;
#endif

	// Do we have support for the CPUID function?
	__try
	{
		_asm
		{
#ifdef CPUID
			xor edx, edx			// Clue the compiler that EDX is about to be used.
#endif
			mov eax, 1				// set up CPUID to return processor version and features
									//      0 = vendor string, 1 = version info, 2 = cache info
									CPUID					// code bytes = 0fh,  0a2h
									mov RegEDX, edx			// features returned in edx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		retval = false;
	}

	// If CPUID not supported, then certainly no SSE extensions.
	if( retval )
	{
		// Do we have support for SSE in this processor?
		if( RegEDX & 0x2000000L )		// bit 25 is set for SSE technology
		{
			// Make sure that SSE is supported by executing an inline SSE instruction

			// BUGBUG, FIXME - Visual C Version 6.0 does not support SSE inline code YET (No macros from Intel either)
			// Fix this if VC7 supports inline SSE instructinons like "xorps" as shown below.
#if 1
			__try
			{
				_asm
				{
					// Attempt execution of a SSE instruction to make sure OS supports SSE FPU context switches
					xorps xmm0, xmm0
					// This will work on Win2k+ (Including masking SSE FPU exception to "normalized" values)
					// This will work on Win98+ (But no "masking" of FPU exceptions provided)
				}
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
#endif

			{
				retval = false;
			}
		}
		else
			retval = false;
	}
#ifdef CPUID
	_asm popad;
#endif

	return retval;
}
bool CheckSSE2Technology( void )
{
	int retval = true;
	unsigned int RegEDX = 0;

#ifdef CPUID
	_asm pushad;
#endif

	// Do we have support for the CPUID function?
	__try
	{
		_asm
		{
#ifdef CPUID
			xor edx, edx			// Clue the compiler that EDX is about to be used.
#endif
			mov eax, 1				// set up CPUID to return processor version and features
									//      0 = vendor string, 1 = version info, 2 = cache info
									CPUID					// code bytes = 0fh,  0a2h
									mov RegEDX, edx			// features returned in edx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		retval = false;
	}

	// If CPUID not supported, then certainly no SSE extensions.
	if( retval )
	{
		// Do we have support for SSE in this processor?
		if( RegEDX & 0x04000000 )		// bit 26 is set for SSE2 technology
		{
			// Make sure that SSE is supported by executing an inline SSE instruction

			__try
			{
				_asm
				{
					// Attempt execution of a SSE2 instruction to make sure OS supports SSE FPU context switches
					xorpd xmm0, xmm0
				}
			}
			__except( EXCEPTION_EXECUTE_HANDLER )

			{
				retval = false;
			}
		}
		else
			retval = false;
	}
#ifdef CPUID
	_asm popad;
#endif

	return retval;
}

// --------------------------------------------------------------------------
bool Check3DNowTechnology( void )
{
	int retval = true;
	unsigned int RegEAX = 0;

#ifdef CPUID
	_asm pushad;
#endif

	// First see if we can execute CPUID at all
	__try
	{
		_asm
		{
#ifdef CPUID
			//			xor edx, edx			// Clue the compiler that EDX is about to be used.
#endif
			mov eax, 0x80000000     // setup CPUID to return whether AMD >0x80000000 function are supported.
									// 0x80000000 = Highest 0x80000000+ function, 0x80000001 = 3DNow support
									CPUID					// code bytes = 0fh,  0a2h
									mov RegEAX, eax			// result returned in eax
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		retval = false;
	}

	// If CPUID not supported, then there is definitely no 3DNow support
	if( retval )
	{
		// Are there any "higher" AMD CPUID functions?
		if( RegEAX > 0x80000000L )
		{
			__try
			{
				_asm
				{
					mov			eax, 0x80000001		// setup to test for CPU features
					CPUID							// code bytes = 0fh,  0a2h
					shr			edx, 31				// If bit 31 is set, we have 3DNow support!
					mov			retval, edx			// Save the return value for end of function
				}
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				retval = false;
			}
		}
		else
		{
			// processor supports CPUID but does not support AMD CPUID functions
			retval = false;
		}
	}

#ifdef CPUID
	_asm popad;
#endif

	return retval;
}

/**
*	Wrapper around CPUID.
*/
bool cpuid( unsigned int instruction, unsigned int& outEax, unsigned int& outEbx, unsigned int& outEcx, unsigned int& outEdx )
{
	int retval = true;
	unsigned int RegEAX = 0;
	unsigned int RegEBX = 0;
	unsigned int RegECX = 0;
	unsigned int RegEDX = 0;

#ifdef CPUID
	_asm pushad;
#endif

	// First see if we can execute CPUID at all
	__try
	{
		_asm
		{
#ifdef CPUID
			//			xor edx, edx			// Clue the compiler that EDX is about to be used.
#endif
			mov eax, instruction

			CPUID
			mov RegEAX, eax
			mov RegEBX, ebx
			mov RegECX, ecx
			mov RegEDX, edx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		retval = false;
	}

#ifdef CPUID
	_asm popad;
#endif

	outEax = RegEAX;
	outEbx = RegEBX;
	outEcx = RegECX;
	outEdx = RegEDX;

	return retval;
}

tchar* GetProcessorVendorId()
{
	static tchar VendorID[ 13 ];

	static unsigned int unused, VendorIDSegment[ 3 ];

	memset( VendorID, 0, sizeof( VendorID ) );

	unused = 0;

	if( cpuid( 0, unused, VendorIDSegment[ 0 ], VendorIDSegment[ 2 ], VendorIDSegment[ 1 ] ) )
	{
		*reinterpret_cast<unsigned int*>( VendorID ) = VendorIDSegment[ 0 ];
		*( reinterpret_cast<unsigned int*>( VendorID ) + 1 ) = VendorIDSegment[ 1 ];
		*( reinterpret_cast<unsigned int*>( VendorID ) + 2 ) = VendorIDSegment[ 2 ];
	}
	else
	{
		_tcscpy( VendorID, _T( "Generic_x86" ) );
	}

	return VendorID;
}

bool CheckGenuineIntel()
{
	bool result = false;
	unsigned int v2;
	unsigned int v3 = 0;
	unsigned int v4 = 0;
	unsigned int v5 = 0;
	unsigned int v6 = 0;
	unsigned int v7 = 0;

	if( cpuid( 0, v2, v5, v7, v6 )
		&& cpuid( 1, v3, v2, v2, v4 )
		&& ( ( v3 & 0xF00 ) == 0xF00 || v3 & 0xF00000 )
		&& v5 == 'uneG'
		&& v6 == 'Ieni'
		&& v7 == 'letn' )
		result = ( v4 >> 28 ) & 1;
	else
		result = 0;

	return result;
}

uint64 CalculateCPUFreq();

PLATFORM_INTERFACE const CPUInformation& GetCPUInformation()
{
	//TODO: i have no idea if this is correct, this function may need to be redone from scratch. - Solokiller
	static CPUInformation pi{};

	if( pi.m_Size != sizeof( CPUInformation ) )
	{
		pi.m_Size = sizeof( CPUInformation );

		pi.m_Speed = CalculateCPUFreq();

		unsigned int unused, EBX;

		if( CheckGenuineIntel() )
		{
			if( cpuid( 1, unused, EBX, unused, unused ) )
				pi.m_nLogicalProcessors = EBX >> 16;
			else
				pi.m_nLogicalProcessors = 1;
		}
		else
		{
			pi.m_nLogicalProcessors = 1;
		}

		SYSTEM_INFO SystemInfo;
		memset( &SystemInfo, 0, sizeof( SystemInfo ) );
		GetSystemInfo( &SystemInfo );

		pi.m_nPhysicalProcessors = SystemInfo.dwNumberOfProcessors / pi.m_nLogicalProcessors;

		if( !pi.m_nLogicalProcessors && !pi.m_nPhysicalProcessors )
		{
			pi.m_nLogicalProcessors = pi.m_nPhysicalProcessors = 1;
		}

		pi.m_bMMX = CheckMMXTechnology();
		pi.m_bSSE = CheckSSETechnology();
		pi.m_bSSE2 = CheckSSE2Technology();
		//TODO: missing the other ones. - Solokiller

		pi.m_szProcessorID = GetProcessorVendorId();
	}

	return pi;
}

#pragma optimize( "", on )

#endif // _WIN32
