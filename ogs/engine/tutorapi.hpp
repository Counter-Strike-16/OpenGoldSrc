#pragma once
#ifndef OGS_TUTORAPI_HPP
#define OGS_TUTORAPI_HPP

extern "C"
{

void EngFunc_RegisterTutorMessageShown(int mid);
int EngFunc_GetTimesTutorMessageShown(int mid);

void EngFunc_ProcessTutorMessageDecayBuffer(int *buffer, int bufferLength);
void EngFunc_ConstructTutorMessageDecayBuffer(int *buffer, int bufferLength);

void EngFunc_ResetTutorMessageDecayData();

};

#endif // OGS_TUTORAPI_HPP