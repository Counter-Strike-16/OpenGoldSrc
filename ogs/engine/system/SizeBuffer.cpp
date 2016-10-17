#include "system/SizeBuffer.h"

//===========================================================================

CSizeBuffer::CSizeBuffer()
{
};

CSizeBuffer::~CSizeBuffer()
{
};

void CSizeBuffer::Clear (sizebuf_t *buf)
{
	buf->cursize = 0;
	buf->overflowed = false;
}

void *CSizeBuffer::GetSpace (sizebuf_t *buf, int length)
{
	void	*data;
	
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

void CSizeBuffer::Write(sizebuf_t *buf, void *data, int length)
{
	Q_memcpy (SZ_GetSpace(buf,length),data,length);		
}

void CSizeBuffer::Print(sizebuf_t *buf, char *data)
{
	int		len;
	
	len = Q_strlen(data)+1;

	if (!buf->cursize || buf->data[buf->cursize-1])
		Q_memcpy ((byte *)SZ_GetSpace(buf, len),data,len); // no trailing 0
	else
		Q_memcpy ((byte *)SZ_GetSpace(buf, len-1)-1,data,len); // write over trailing 0
};