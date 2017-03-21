
// sv_nchan.c, user reliable data stream writes

#include "qwsvdef.h"

// check to see if client block will fit, if not, rotate buffers
void ClientReliableCheckBlock(client_t *cl, int maxsize)
{
	if (cl->num_backbuf ||
		cl->netchan.message.cursize > 
		cl->netchan.message.maxsize - maxsize - 1) {
		// we would probably overflow the buffer, save it for next
		if (!cl->num_backbuf) {
			memset(&cl->backbuf, 0, sizeof(cl->backbuf));
			cl->backbuf.allowoverflow = true;
			cl->backbuf.data = cl->backbuf_data[0];
			cl->backbuf.maxsize = sizeof(cl->backbuf_data[0]);
			cl->backbuf_size[0] = 0;
			cl->num_backbuf++;
		}

		if (cl->backbuf.cursize > cl->backbuf.maxsize - maxsize - 1) {
			if (cl->num_backbuf == MAX_BACK_BUFFERS) {
				Con_Printf ("WARNING: MAX_BACK_BUFFERS for %s\n", cl->name);
				cl->backbuf.cursize = 0; // don't overflow without allowoverflow set
				cl->netchan.message.overflowed = true; // this will drop the client
				return;
			}
			memset(&cl->backbuf, 0, sizeof(cl->backbuf));
			cl->backbuf.allowoverflow = true;
			cl->backbuf.data = cl->backbuf_data[cl->num_backbuf];
			cl->backbuf.maxsize = sizeof(cl->backbuf_data[cl->num_backbuf]);
			cl->backbuf_size[cl->num_backbuf] = 0;
			cl->num_backbuf++;
		}
	}
}

// begin a client block, estimated maximum size
void ClientReliableWrite_Begin(client_t *cl, int c, int maxsize)
{
	ClientReliableCheckBlock(cl, maxsize);
	ClientReliableWrite_Byte(cl, c);
}

void ClientReliable_FinishWrite(client_t *cl)
{
	if (cl->num_backbuf) {
		cl->backbuf_size[cl->num_backbuf - 1] = cl->backbuf.cursize;

		if (cl->backbuf.overflowed) {
			Con_Printf ("WARNING: backbuf [%d] reliable overflow for %s\n",cl->num_backbuf,cl->name);
			cl->netchan.message.overflowed = true; // this will drop the client
		}
	}
}

void ClientReliableWrite_Angle(client_t *cl, float f)
{
	if (cl->num_backbuf) {
		MSG_WriteAngle(&cl->backbuf, f);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteAngle(&cl->netchan.message, f);
}

void ClientReliableWrite_Angle16(client_t *cl, float f)
{
	if (cl->num_backbuf) {
		MSG_WriteAngle16(&cl->backbuf, f);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteAngle16(&cl->netchan.message, f);
}

void ClientReliableWrite_Byte(client_t *cl, int c)
{
	if (cl->num_backbuf) {
		MSG_WriteByte(&cl->backbuf, c);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteByte(&cl->netchan.message, c);
}

void ClientReliableWrite_Char(client_t *cl, int c)
{
	if (cl->num_backbuf) {
		MSG_WriteChar(&cl->backbuf, c);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteChar(&cl->netchan.message, c);
}

void ClientReliableWrite_Float(client_t *cl, float f)
{
	if (cl->num_backbuf) {
		MSG_WriteFloat(&cl->backbuf, f);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteFloat(&cl->netchan.message, f);
}

void ClientReliableWrite_Coord(client_t *cl, float f)
{
	if (cl->num_backbuf) {
		MSG_WriteCoord(&cl->backbuf, f);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteCoord(&cl->netchan.message, f);
}

void ClientReliableWrite_Long(client_t *cl, int c)
{
	if (cl->num_backbuf) {
		MSG_WriteLong(&cl->backbuf, c);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteLong(&cl->netchan.message, c);
}

void ClientReliableWrite_Short(client_t *cl, int c)
{
	if (cl->num_backbuf) {
		MSG_WriteShort(&cl->backbuf, c);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteShort(&cl->netchan.message, c);
}

void ClientReliableWrite_String(client_t *cl, char *s)
{
	if (cl->num_backbuf) {
		MSG_WriteString(&cl->backbuf, s);
		ClientReliable_FinishWrite(cl);
	} else
		MSG_WriteString(&cl->netchan.message, s);
}

void ClientReliableWrite_SZ(client_t *cl, void *data, int len)
{
	if (cl->num_backbuf) {
		SZ_Write(&cl->backbuf, data, len);
		ClientReliable_FinishWrite(cl);
	} else
		SZ_Write(&cl->netchan.message, data, len);
}

