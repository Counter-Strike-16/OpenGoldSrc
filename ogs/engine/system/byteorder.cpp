//#include "precompiled.hpp"
#include "system/common.hpp"

/*
============================================================================

BYTE ORDER FUNCTIONS

============================================================================
*/

qboolean bigendien;
short (*BigShort)(short l);
short (*LittleShort)(short l);
int (*BigLong)(int l);
int (*LittleLong)(int l);
float (*BigFloat)(float l);
float (*LittleFloat)(float l);

int LongSwap(int l)
{
	return bswap(l);
}

int LongNoSwap(int l)
{
	return l;
}

short ShortSwap(short l)
{
	return bswap(l);
}

short ShortNoSwap(short l)
{
	return l;
}

float FloatSwap(float f)
{
	/*union
  {
          float f;
          byte b[4];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];

  return dat2.f;*/
	// unsigned long u = bswap(*(unsigned long *)&f);
	// return *(float *)&u;
	return bswap(f);
}

float FloatNoSwap(float f)
{
	return f;
}