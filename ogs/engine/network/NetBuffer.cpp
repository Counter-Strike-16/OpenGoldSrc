#include "network/NetBuffer.h"

//===========================================================================

CNetBuffer::CNetBuffer()
{
};

CNetBuffer::~CNetBuffer()
{
};

void CNetBuffer::Clear(sizebuf_t *buf)
{
	buf->cursize = 0;
	buf->overflowed = false;
}

void *CNetBuffer::GetSpace(sizebuf_t *buf, int length)
{
	void *data;
	
	if (buf->cursize + length > buf->maxsize)
	{
		if (!buf->allowoverflow)
			Sys_Error ("SZ_GetSpace: overflow without allowoverflow set (%d)", buf->maxsize);
		
		if (length > buf->maxsize)
			Sys_Error ("SZ_GetSpace: %i is > full buffer size", length);
			
		Sys_Printf ("SZ_GetSpace: overflow\n");	// because Con_Printf may be redirected
		SZ_Clear (buf); 
		buf->overflowed = true;
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;
	
	return data;
}

void CNetBuffer::Write(sizebuf_t *buf, void *data, int length)
{
	Q_memcpy (SZ_GetSpace(buf,length),data,length);		
}

void CNetBuffer::Print(sizebuf_t *buf, char *data)
{
	int		len;
	
	len = Q_strlen(data)+1;

	if (!buf->cursize || buf->data[buf->cursize-1])
		Q_memcpy ((byte *)SZ_GetSpace(buf, len),data,len); // no trailing 0
	else
		Q_memcpy ((byte *)SZ_GetSpace(buf, len-1)-1,data,len); // write over trailing 0
};