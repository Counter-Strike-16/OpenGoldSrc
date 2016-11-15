#include "TutorAPI.h"

#ifndef OGS_NULL_TUTOR_API_IMPL
void EngFunc_RegisterTutorMessageShown(int mid)
{
};

int EngFunc_GetTimesTutorMessageShown(int mid)
{
};

void EngFunc_ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
};

void EngFunc_ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
};

void EngFunc_ResetTutorMessageDecayData()
{
};
#else // OGS_NULL_TUTOR_API_IMPL
void EngFunc_RegisterTutorMessageShown(int mid)
{
};

int EngFunc_GetTimesTutorMessageShown(int mid)
{
};

void EngFunc_ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
};

void EngFunc_ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
};

void EngFunc_ResetTutorMessageDecayData()
{
};
#endif // OGS_NULL_TUTOR_API_IMPL