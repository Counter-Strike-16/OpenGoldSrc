#pragma once

/* <42b91> ../common/mathlib.h:6 */
#if !defined DID_VEC3_T_DEFINE && !defined vec3_t
#endif

//typedef union DLONG_u
//{
//} DLONG;

#ifdef __cplusplus
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef clamp
#undef clamp
#endif

template <typename T>
inline T min(T a, T b)
{
	return (a < b) ? a : b;
}

template <typename T>
inline T max(T a, T b)
{
	return (a < b) ? b : a;
}

template <typename T>
inline T clamp(T a, T min, T max)
{
	return (a > max) ? max : (a < min) ? min : a;
}

template <typename T>
inline T bswap(T s)
{
	switch(sizeof(T))
	{
#ifdef _WIN32
	case 2:
	{
		auto res = _byteswap_ushort(*(uint16 *)&s);
		return *(T *)&res;
	}
	case 4:
	{
		auto res = _byteswap_ulong(*(uint32 *)(&s));
		return *(T *)&res;
	}
	case 8:
	{
		auto res = _byteswap_uint64(*(uint64 *)&s);
		return *(T *)&res;
	}
#else
	case 2:
	{
		auto res = _bswap16(*(uint16 *)&s);
		return *(T *)&res;
	}
	case 4:
	{
		auto res = _bswap(*(uint32 *)&s);
		return *(T *)&res;
	}
	case 8:
	{
		auto res = _bswap64(*(uint64 *)&s);
		return *(T *)&res;
	}
#endif
	default:
		return s;
	}
}
#else // __cplusplus
	#ifndef max
		#define max(a, b) (((a) > (b)) ? (a) : (b))
	#endif

	#ifndef min
		#define min(a, b) (((a) < (b)) ? (a) : (b))
	#endif

	#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#endif // __cplusplus