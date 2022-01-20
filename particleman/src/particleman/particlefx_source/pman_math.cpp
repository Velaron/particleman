#include "mathlib.h"

void VectorScale( float const *in, float scale, float *out )
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

void VectorAngles( const float *forward, float *angles )
{
    double tmp, yaw, pitch;

	if ( forward[1] == 0 && forward[0] == 0 )
	{
		yaw = 0;
		if ( forward[2] <= 0 )
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = ( atan2( forward[1], forward[0] ) * 180 / M_PI );
		if ( yaw < 0 )
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = ( atan2( forward[2], tmp ) * 180 / M_PI );
		if ( pitch < 0 )
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

float VectorNormalize( float *v )
{
	float result, ilength;

	result = sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );

	if ( result != 0.0f )
	{
		ilength = 1 / result;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return result;
}

void VectorMA( const float *veca, float scale, const float *vecb, float *vecc )
{
	vecc[0] = veca[0] + scale * vecb[0];
	vecc[1] = veca[1] + scale * vecb[1];
	vecc[2] = veca[2] + scale * vecb[2];
}