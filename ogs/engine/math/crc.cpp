#include "math/crc.h"

#if !(OGS_NULL_CRC_API_IMPL || OGS_NULL_EXPORT_API_IMPL)
void CRC32_Init(CRC32_t *pulCRC)
{
};

void CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
};

void CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
};

CRC32_t CRC32_Final(CRC32_t pulCRC)
{
	return 0;
};
#else // OGS_NULL_CRC_API_IMPL || OGS_NULL_EXPORT_API_IMPL
void CRC32_Init(CRC32_t *pulCRC)
{
};

void CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
};

void CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
};

CRC32_t CRC32_Final(CRC32_t pulCRC)
{
	return 0;
};
#endif // OGS_NULL_CRC_API_IMPL || OGS_NULL_EXPORT_API_IMPL