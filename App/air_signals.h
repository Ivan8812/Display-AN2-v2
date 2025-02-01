#pragma once

#include <math.h>
#include <stdint.h>


namespace air_signals
{
	static const float Rate = 10.0f;
	static const float Ks = 1225.057f;
	static const float P0 = 101325.0f;
	static const float T0 = 288.15f;
	static const float L = 6.5e-3f;
	static const float R = 8.31446f;
	static const float G = 9.80665f;
	static const float M = 28.98e-3f;
};

float altitude(float pres, float ref)
{
	using namespace air_signals;
	return T0/L*(1.0f-powf((pres/ref), (R*L/G/M)));
}

float air_speed(float pres)
{
	using namespace air_signals;
	return Ks*sqrtf(5.0f*(powf(pres/P0 + 1.0f, 2.0f/7.0f) - 1.0f));
}

float vert_speed(float pres, float ref, float dp)
{
	using namespace air_signals;
	return -T0*powf(pres/ref, R*L/(G*M))*R*dp/(G*M*pres);
}

