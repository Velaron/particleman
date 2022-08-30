/*
* pman_frustum.cpp
* Copyright (C) 2022 Velaron
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "triangleapi.h"
#include "util_vector.h"
#include "pman_main.h"
#include "pman_frustum.h"

#ifdef _WIN32
#define HSPRITE HSPRITE_win32
#include "windows.h"
#undef HSPRITE
#endif

#include <gl/GL.h>

CFrustum g_cFrustum;

void CFrustum::NormalizeFrustumPlane( float ( *frustum )[4], int side )
{
	float magnitude = Vector( frustum[side] ).Length();

	for ( int i = 0; i <= 3; i++ )
		frustum[side][i] /= magnitude;
}

void CFrustum::CalculateFrustum( void )
{
	float proj[16];
	float modl[16];
	float clip[16];

	gEngfuncs.pTriAPI->GetMatrix( GL_PROJECTION_MATRIX, proj );
	gEngfuncs.pTriAPI->GetMatrix( GL_MODELVIEW_MATRIX, modl );

	clip[0] = proj[0] * modl[0] + proj[1] * modl[4] + proj[2] * modl[8] + proj[3] * modl[12];
	clip[1] = proj[1] * modl[0] + proj[1] * modl[5] + proj[2] * modl[9] + proj[3] * modl[13];
	clip[2] = proj[2] * modl[0] + proj[1] * modl[6] + proj[2] * modl[10] + proj[3] * modl[14];
	clip[3] = modl[3] * proj[0] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = proj[0] * modl[4] + proj[4] * modl[5] + proj[6] * modl[8] + proj[7] * modl[12];
	clip[5] = proj[1] * modl[4] + proj[5] * modl[5] + proj[6] * modl[9] + proj[7] * modl[13];
	clip[6] = proj[2] * modl[4] + proj[5] * modl[6] + proj[6] * modl[10] + proj[7] * modl[14];
	clip[7] = modl[3] * proj[4] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[8] = proj[0] * modl[8] + proj[4] * modl[9] + proj[8] * modl[10] + proj[12] * modl[11];
	clip[9] = proj[1] * modl[8] + proj[5] * modl[9] + proj[9] * modl[10] + proj[11] * modl[13];
	clip[10] = proj[2] * modl[8] + proj[6] * modl[9] + proj[10] * modl[10] + proj[11] * modl[14];
	clip[11] = modl[3] * proj[8] + modl[7] * proj[9] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = proj[0] * modl[12] + proj[4] * modl[13] + proj[8] * modl[14] + proj[12] * modl[15];
	clip[13] = proj[1] * modl[12] + proj[5] * modl[13] + proj[9] * modl[14] + proj[13] * modl[15];
	clip[14] = proj[2] * modl[12] + proj[6] * modl[13] + proj[10] * modl[14] + proj[14] * modl[15];
	clip[15] = modl[3] * proj[12] + proj[7] * modl[13] + proj[11] * modl[14] + proj[15] * modl[15];

	g_flFrustum[RIGHT][A] = clip[3] - clip[0];
	g_flFrustum[RIGHT][B] = clip[7] - clip[4];
	g_flFrustum[RIGHT][C] = clip[11] - clip[8];
	g_flFrustum[RIGHT][D] = clip[15] - clip[12];

	NormalizeFrustumPlane( g_flFrustum, RIGHT );

	g_flFrustum[LEFT][A] = clip[0] + clip[3];
	g_flFrustum[LEFT][B] = clip[4] + clip[7];
	g_flFrustum[LEFT][C] = clip[8] + clip[11];
	g_flFrustum[LEFT][D] = clip[12] + clip[15];

	NormalizeFrustumPlane( g_flFrustum, LEFT );

	g_flFrustum[BOTTOM][A] = clip[1] + clip[3];
	g_flFrustum[BOTTOM][B] = clip[5] + clip[7];
	g_flFrustum[BOTTOM][C] = clip[9] + clip[11];
	g_flFrustum[BOTTOM][D] = clip[15] + clip[13];

	NormalizeFrustumPlane( g_flFrustum, BOTTOM );

	g_flFrustum[TOP][A] = clip[3] - clip[1];
	g_flFrustum[TOP][B] = clip[7] - clip[5];
	g_flFrustum[TOP][C] = clip[11] - clip[9];
	g_flFrustum[TOP][D] = clip[15] - clip[13];

	NormalizeFrustumPlane( g_flFrustum, TOP );

	g_flFrustum[BACK][A] = clip[3] - clip[2];
	g_flFrustum[BACK][B] = clip[7] - clip[6];
	g_flFrustum[BACK][C] = clip[11] - clip[10];
	g_flFrustum[BACK][D] = clip[15] - clip[14];

	NormalizeFrustumPlane( g_flFrustum, BACK );

	g_flFrustum[FRONT][A] = clip[2] + clip[3];
	g_flFrustum[FRONT][B] = clip[6] + clip[7];
	g_flFrustum[FRONT][C] = clip[10] + clip[11];
	g_flFrustum[FRONT][D] = clip[14] + clip[15];

	NormalizeFrustumPlane( g_flFrustum, FRONT );
}

// Velaron: TODO
bool CFrustum::PointInsideFrustum( float x, float y, float z )
{
	for ( int i = 0; i < 6; ++i )
	{
		if ( ( ( g_flFrustum[i][0] * x ) + ( g_flFrustum[i][1] * y ) + ( g_flFrustum[i][2] * z ) + g_flFrustum[i][3] ) <= 0 )
		{
			return false;
		}
	}

	return true;
}

// Velaron: TODO
bool CFrustum::SphereInsideFrustum( float x, float y, float z, float radius )
{
	for ( int i = 0; i < 6; ++i )
	{
		if ( ( ( g_flFrustum[i][0] * x ) + ( g_flFrustum[i][1] * y ) + ( g_flFrustum[i][2] * z ) + g_flFrustum[i][3] ) <= -radius )
		{
			return false;
		}
	}

	return true;
}

// Velaron: TODO
bool CFrustum::PlaneInsideFrustum( float x, float y, float z, float size )
{
	const float sizes[3][2] = {
		{ x - size, x + size },
		{ y - size, y + size },
		{ z - size, z + size }
	};

	for ( int i = 0; i < 6; ++i )
	{
		for ( int zAxis = 0; zAxis < 2; ++zAxis )
		{
			for ( int yAxis = 0; yAxis < 2; ++yAxis )
			{
				for ( int xAxis = 0; xAxis < 2; ++xAxis )
				{
					if ( ( ( sizes[0][xAxis] * g_flFrustum[i][0] ) + ( sizes[1][yAxis] * g_flFrustum[i][1] ) + ( sizes[2][zAxis] * g_flFrustum[i][2] ) + g_flFrustum[i][3] ) <= 0 )
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}