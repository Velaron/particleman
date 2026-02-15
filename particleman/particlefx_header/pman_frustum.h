#ifndef PMAN_FRUSTUM_H
#define PMAN_FRUSTUM_H

enum FrustumSide
{
	RIGHT = 0,  // The RIGHT side of the frustum
	LEFT = 1,   // The LEFT side of the frustum
	BOTTOM = 2, // The BOTTOM side of the frustum
	TOP = 3,    // The TOP side of the frustum
	BACK = 4,   // The BACK side of the frustum
	FRONT = 5   // The FRONT side of the frustum
};

enum PlaneData
{
	A = 0, // The X value of the plane's normal
	B = 1, // The Y value of the plane's normal
	C = 2, // The Z value of the plane's normal
	D = 3  // Distance between plane and origin
};

class CFrustum
{
private:
	float g_flFrustum[6][4];

public:
	void NormalizeFrustumPlane( float* frustum, int side );
	void CalculateFrustum();
	bool PointInsideFrustum( float x, float y, float z );
	bool SphereInsideFrustum( float x, float y, float z, float radius );
	bool PlaneInsideFrustum( float x, float y, float z, float size );
};

extern CFrustum g_cFrustum;

#endif //PMAN_FRUSTUM_H