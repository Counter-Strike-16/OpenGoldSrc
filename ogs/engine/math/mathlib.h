#pragma once
#ifndef OGS_MATH_HPP
#define OGS_MATH_HPP

// Export API funcs
extern "C"
{

float Math_VecToYaw(const float *rgflVector);
void Math_VecToAngles(const float *rgflVectorIn, float *rgflVectorOut);

void Math_MakeVectors(const float *rgflVector);
void Math_AngleVectors(const float *rgflVector, float *forward, float *right, float *up);

int32 Math_RandomLong(int32 lLow, int32 lHigh);
float Math_RandomFloat(float flLow, float flHigh);

};

#endif // OGS_MATH_HPP