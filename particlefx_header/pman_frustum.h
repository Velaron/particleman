/*
* pman_frustum.h
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

#ifndef __PMAN_FRUSTUM_H__
#define __PMAN_FRUSTUM_H__

enum FrustumSide
{
	RIGHT = 0,
	LEFT,
	BOTTOM,
	TOP,
	BACK,
	FRONT
};

enum PlaneData
{
	A = 0,
	B,
	C,
	D
};

class CFrustum
{
public:
	float g_flFrustum[6][4];

	void NormalizeFrustumPlane( float ( *frustum )[4], int side );
	void CalculateFrustum( void );
	bool PointInsideFrustum( float x, float y, float z );
	bool SphereInsideFrustum( float x, float y, float z, float radius );
	bool PlaneInsideFrustum( float x, float y, float z, float size );
};

extern CFrustum g_cFrustum;

#endif // __PMAN_FRUSTUM_H__
