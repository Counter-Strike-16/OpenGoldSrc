#pragma once

extern "C"
{

void CRC32_Init(CRC32_t *pulCRC);
void CRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len);
void CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch);
CRC32_t CRC32_Final(CRC32_t pulCRC);

}; // extern "C"