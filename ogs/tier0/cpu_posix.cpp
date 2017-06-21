//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#define cpuid(in,a,b,c,d)\
  asm("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

bool CheckMMXTechnology( void )
{
	unsigned long eax, ebx, edx, unused;
	cpuid( 1, eax, ebx, unused, edx );

	return ( edx & 0x800000 ) != 0;
}

bool CheckSSETechnology( void )
{
	unsigned long eax, ebx, edx, unused;
	cpuid( 1, eax, ebx, unused, edx );

	return ( edx & 0x2000000L ) != 0;
}

bool CheckSSE2Technology( void )
{
	unsigned long eax, ebx, edx, unused;
	cpuid( 1, eax, ebx, unused, edx );

	return ( edx & 0x04000000 ) != 0;
}

bool Check3DNowTechnology( void )
{
	unsigned long eax, unused;
	cpuid( 0x80000000, eax, unused, unused, unused );

	if( eax > 0x80000000L )
	{
		cpuid( 0x80000001, unused, unused, unused, eax );
		return ( eax & 1 << 31 ) != 0;
	}
	return false;
}

tchar* GetProcessorVendorId()
{
	static char unused;
	static int unused2, unused3;
	static tchar VendorID[ 13 ];

	_EAX = 0;
	asm( "cpuid" );
	unused = 0;

	*( int* ) VendorID = _EBX;

	unused2 = _EDX;
	unused3 = _ECX;

	return VendorID;
}