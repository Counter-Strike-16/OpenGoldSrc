#include "network/netmsgapi.hpp"

#ifndef OGS_NULL_NETMSG_API_IMPL
void NetMsg_Begin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
};

void NetMsg_End()
{
};

void NetMsg_WriteByte(int iValue)
{
};

void NetMsg_WriteChar(int iValue)
{
};

void NetMsg_WriteShort(int iValue)
{
};

void NetMsg_WriteLong(int iValue)
{
};

void NetMsg_WriteAngle(float flValue)
{
};

void NetMsg_WriteCoord(float flValue)
{
};

void NetMsg_WriteString(const char *sz)
{
};

void NetMsg_WriteEntity(int iValue)
{
};
#else // OGS_NULL_NETMSG_API_IMPL
void NetMsg_Begin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
};

void NetMsg_End()
{
};

void NetMsg_WriteByte(int iValue)
{
};

void NetMsg_WriteChar(int iValue)
{
};

void NetMsg_WriteShort(int iValue)
{
};

void NetMsg_WriteLong(int iValue)
{
};

void NetMsg_WriteAngle(float flValue)
{
};

void NetMsg_WriteCoord(float flValue)
{
};

void NetMsg_WriteString(const char *sz)
{
};

void NetMsg_WriteEntity(int iValue)
{
};
#endif // OGS_NULL_NETMSG_API_IMPL