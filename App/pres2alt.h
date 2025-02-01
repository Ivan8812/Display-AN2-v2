#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdbool.h"
#include "math.h"

float pres2alt(float presure, float ref)
{
	static const float T0 = 288.15f;
	static const float L = 6.5e-3f;
	static const float R = 8.31446f;
	static const float G = 9.80665f;
	static const float M = 28.98e-3f;

	return T0/L*(1.0f-powf((presure/ref), (R*L/G/M)));
}

float pres2vspeed(float presure, float ref)
{
	static const float dt = 0.1f;
	static const float T0 = 288.15f;
	static const float L = 6.5e-3f;
	static const float R = 8.31446f;
	static const float G = 9.80665f;
	static const float M = 28.98e-3f;
	static float prev_pres = 0.0f;

	float diff_pres = (presure - prev_pres)*dt;
	prev_pres = presure;

	return -T0*powf(presure/ref, R*L/(G*M))*R*diff_pres/(G*M*presure);
}

#ifdef __cplusplus
} // extern "C"
#endif
