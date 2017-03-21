#include "quakedef.h"

extern int desired_speed;
extern int desired_bits;

qboolean SNDDMA_Init(void)
{
	int size;

	size = 16384 + sizeof(dma_t);
	shm = malloc (size);
	memset((void*)shm, 0, size);

	shm->buffer = (char*)shm + sizeof(dma_t);
	shm->channels = 2;
	shm->speed = desired_speed;
	shm->samplebits = desired_bits;
	shm->samples = 16384 / (desired_bits / 8);
	shm->submission_chunk = 1;

	return true;
}

// return the current sample position (in mono samples read)
// inside the recirculating dma buffer
int SNDDMA_GetDMAPos(void)
{
	shm->samplepos = (int)(realtime*shm->speed*shm->channels) & (shm->samples-1);
	
	return shm->samplepos;
}

void SNDDMA_Shutdown(void)
{
}
