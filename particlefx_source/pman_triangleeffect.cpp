/*
* pman_triangleeffect.cpp
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

#include "util_vector.h"
#include "const.h"
#include "com_model.h"
#include "triangleapi.h"
#include "pman_triangleffect.h"
#include "pman_main.h"
#include "pman_frustum.h"
#include "pman_misc.h"

const Vector g_vecZero = Vector( 0.0f, 0.0f, 0.0f );

float VectorNormalize( float *v );

#define max( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#define min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

void CCoreTriangleEffect::InitializeSprite( Vector p_org, Vector p_normal, struct model_s *sprite, float size, float brightness )
{
	Vector forward, right, up;

	// m_flOriginalSize = 10.0f;
	// m_flSize = 10.0f;

	m_flScaleSpeed = 0.0f;
	m_flContractSpeed = 0.0f;

	m_flStretchX = 1.0f;
	m_flStretchY = 1.0f;

	// m_pTexture = NULL;

	// m_flOriginalBrightness = 255.0f;
	// m_flBrightness = 255.0f;

	strcpy( m_szClassname, "particle" );

	m_flDieTime = 0.0f;
	m_flDampingTime = 0.0f;
	m_flGravity = 0.0f;
	m_flNextCollisionTime = 0.0f;
	m_bInPVS = true;
	m_flFadeSpeed = -1.0f;
	m_iFramerate = 0;
	m_iNumFrames = 0;
	m_iFrame = 0;
	m_iCollisionFlags = 0;
	m_iRendermode = kRenderNormal;
	m_iRenderFlags = 0;

	m_vVelocity = Vector( 0.0f, 0.0f, 0.0f );

	m_flBounceFactor = 1.0f;

	// m_vOriginalAngles = Vector( 0.0f, 0.0f, 0.0f );
	// m_vAngles = Vector( 0.0f, 0.0f, 0.0f );
	m_vAVelocity = Vector( 0.0f, 0.0f, 0.0f );

	m_bInWater = false;
	m_bAffectedByForce = false;

	m_vColor = Vector( 255.0f, 255.0f, 255.0f );

	m_flNextPVSCheck = m_flTimeCreated = gEngfuncs.GetClientTime();

	m_vOrigin = p_org;
	m_vPrevOrigin = p_org;
	m_vAngles = p_normal;

	m_pTexture = sprite;
	m_flOriginalSize = size;
	m_flOriginalBrightness = brightness;
	m_flSize = size;
	m_flBrightness = brightness;

	gEngfuncs.pfnAngleVectors( p_normal, forward, right, up );

	m_vOriginalAngles = m_vAngles;

	// m_vLowLeft = Vector( p_org.x - ( right.x * size ) * 0.5f - ( up.x * size ) * 0.5f,
	//                      p_org.y - ( right.y * size ) * 0.5f - ( up.y * size ) * 0.5f,
	//                      p_org.z - ( right.z * size ) * 0.5f - ( up.z * size ) * 0.5f );
	m_vLowLeft = p_org - ( right * size ) * 0.5f - ( up * size ) * 0.5f;
	m_vLowRight = m_vLowLeft + ( right * size ) * 2.0f;
	m_vTopLeft = m_vLowLeft + ( up * size ) * 2.0f;
}

bool CCoreTriangleEffect::CheckVisibility( void )
{
	float radius;
	Vector mins, maxs;

	radius = m_flSize / 5.0f;

	if ( gEngfuncs.GetClientTime() >= m_flNextPVSCheck )
	{
		mins = Vector( m_vOrigin.x - radius, m_vOrigin.y - radius, m_vOrigin.z - radius );
		maxs = Vector( m_vOrigin.x + radius, m_vOrigin.y + radius, m_vOrigin.z + radius );
		m_bInPVS = gEngfuncs.pTriAPI->BoxInPVS( mins, maxs ) != 0;
		m_flNextPVSCheck = gEngfuncs.GetClientTime() + 0.1f;
	}

	if ( ( m_iRenderFlags & CULL_FRUSTUM_SPHERE ) )
	{
		if ( !g_cFrustum.SphereInsideFrustum( m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) )
			return false;

		if ( m_bInPVS )
			return true;

		return !( m_iRenderFlags & CULL_PVS );
	}

	if ( ( m_iRenderFlags & CULL_FRUSTUM_PLANE ) )
	{
		if ( !g_cFrustum.PlaneInsideFrustum( m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) )
			return false;

		if ( m_bInPVS )
			return true;

		return !( m_iRenderFlags & CULL_PVS );
	}

	if ( ( m_iRenderFlags & CULL_FRUSTUM_POINT ) )
	{
		if ( !g_cFrustum.PointInsideFrustum( m_vOrigin.x, m_vOrigin.y, m_vOrigin.z ) )
			return false;

		if ( m_bInPVS )
			return true;

		return !( m_iRenderFlags & CULL_PVS );
	}

	return true;
}

void CCoreTriangleEffect::Draw( void )
{
	Vector vColor;
	Vector forward, right, up;
	Vector lowLeft, lowRight, topLeft, topRight;
	float intensity;
	float x, y, z;

	if ( m_flDieTime == gEngfuncs.GetClientTime() )
		return;

	if ( !( m_iRenderFlags & LIGHT_NONE ) )
	{
		gEngfuncs.pTriAPI->LightAtPoint( m_vOrigin, vColor );
		intensity = ( vColor.x + vColor.y + vColor.z ) / 3.0f;
	}

	if ( ( m_iRenderFlags & ( RENDER_FACEPLAYER | RENDER_FACEPLAYER_ROTATEZ ) ) )
	{
		m_vAngles.x = g_vViewAngles.x;
		m_vAngles.y = g_vViewAngles.y;

		if ( !( m_iRenderFlags & RENDER_FACEPLAYER_ROTATEZ ) )
			m_vAngles.z = g_vViewAngles.z;
	}

	if ( ( m_iRenderFlags & LIGHT_NONE ) )
	{
		x = m_vColor.x;
		y = m_vColor.y;
		z = m_vColor.z;
	}
	else if ( ( m_iRenderFlags & LIGHT_COLOR ) )
	{
		x = vColor.x / m_vColor.x * 255.0f;
		y = vColor.y / m_vColor.y * 255.0f;
		z = vColor.z / m_vColor.z * 255.0f;
	}
	else if ( ( m_iRenderFlags & LIGHT_INTENSITY ) )
	{
		x = intensity / m_vColor.x * 255.0f;
		y = intensity / m_vColor.y * 255.0f;
		z = intensity / m_vColor.z * 255.0f;
	}

	x = min( max( 0.0f, x ), 255.0f );
	y = min( max( 0.0f, y ), 255.0f );
	z = min( max( 0.0f, z ), 255.0f );

	gEngfuncs.pfnAngleVectors( m_vAngles, forward, right, up );

	lowLeft = m_vOrigin - ( right * m_flSize * m_flStretchX * 0.5f ) - ( up * m_flSize * /* m_flStretchY * */ 0.5f );
	lowRight = lowLeft + ( right * m_flSize * m_flStretchX );
	topLeft = lowLeft + ( up * m_flSize * m_flStretchY );
	topRight = lowRight + ( up * m_flSize * m_flStretchY );

	gEngfuncs.pTriAPI->SpriteTexture( m_pTexture, m_iFrame );
	gEngfuncs.pTriAPI->RenderMode( m_iRendermode );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
	gEngfuncs.pTriAPI->Begin( TRI_QUADS );
	gEngfuncs.pTriAPI->Color4f( x / 255.0f, y / 255.0f, z / 255.0f, m_flBrightness / 255.0f );

	gEngfuncs.pTriAPI->TexCoord2f( 0.0f, 0.0f );
	gEngfuncs.pTriAPI->Vertex3fv( topLeft );

	gEngfuncs.pTriAPI->TexCoord2f( 0.0f, 1.0f );
	gEngfuncs.pTriAPI->Vertex3fv( lowLeft );

	gEngfuncs.pTriAPI->TexCoord2f( 1.0f, 1.0f );
	gEngfuncs.pTriAPI->Vertex3fv( lowRight );

	gEngfuncs.pTriAPI->TexCoord2f( 1.0f, 0.0f );
	gEngfuncs.pTriAPI->Vertex3fv( topRight );

	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
	gEngfuncs.pTriAPI->CullFace( TRI_FRONT );
}

void CCoreTriangleEffect::Animate( float time )
{
	if ( m_iFramerate && m_iNumFrames )
		m_iFrame = (int)( m_iFramerate * ( time - m_flTimeCreated ) ) % m_iNumFrames;
}

void CCoreTriangleEffect::AnimateAndDie( float time )
{
	float frame;

	if ( m_iFramerate && m_iNumFrames )
	{
		frame = m_iFramerate * ( time - m_flTimeCreated );

		if ( frame > m_iNumFrames )
			m_flDieTime = time;
		else
			m_iFrame = (int)frame;

		if ( m_iNumFrames < m_iFrame )
			m_iFrame = m_iNumFrames - 1;
	}
}

void CCoreTriangleEffect::Expand( float time )
{
	if ( m_flScaleSpeed )
	{
		m_flSize = m_flOriginalSize + m_flScaleSpeed * 30.0f * ( time - m_flTimeCreated );

		if ( m_flSize < 0.0001f )
			m_flDieTime = time;
	}
}

void CCoreTriangleEffect::Contract( float time )
{
	if ( m_flContractSpeed )
	{
		m_flSize = m_flOriginalSize - m_flContractSpeed * 30.0f * ( time - m_flTimeCreated );

		if ( m_flSize < 0.0001f )
			m_flDieTime = time;
	}
}

void CCoreTriangleEffect::Fade( float time )
{
	if ( m_flFadeSpeed >= -0.5f )
	{
		if ( m_flFadeSpeed == 0.0f )
			m_flBrightness = m_flOriginalBrightness * ( 1.0f - ( time - m_flTimeCreated ) / ( m_flDieTime - m_flTimeCreated ) );
		else
			m_flBrightness = m_flOriginalBrightness - m_flFadeSpeed * 30.0f * ( time - m_flTimeCreated );

		if ( m_flBrightness < 1.0f )
			m_flDieTime = time;
	}
}

void CCoreTriangleEffect::Spin( float time )
{
	Vector point;

	if ( m_vAVelocity != g_vecZero )
	{
		// m_vAngles = m_vAngles + m_vAVelocity;

		m_vAngles = m_vOriginalAngles + m_vAVelocity * ( VectorNormalize( point ) * 30.0f * ( time - m_flTimeCreated ) );
	}
}

void CCoreTriangleEffect::CalculateVelocity( float time )
{
	float timeDelta = time - g_flOldTime;
	float gravity = -timeDelta * g_flGravity * m_flGravity;

	if ( m_vVelocity == g_vecZero && gravity == 0.0f )
		return;

	if ( ( m_iCollisionFlags & TRI_SPIRAL ) )
	{
		m_vOrigin.x = m_vOrigin.x + m_vVelocity.x * timeDelta + (float)sin( time * 5.0f + (int)this ) * 2.0f;
		m_vOrigin.y = m_vOrigin.y + m_vVelocity.y * timeDelta + (float)sin( time * 7.5f + (int)this );
		m_vOrigin.z = m_vOrigin.z + m_vVelocity.z * timeDelta;
	}
	else
		m_vOrigin = m_vOrigin + m_vVelocity * timeDelta;

	m_vVelocity.z = m_vVelocity.z + gravity;
}

// Velaron: TODO
void CCoreTriangleEffect::CheckCollision( float time )
{
	if ( !m_iCollisionFlags )
		return;
}

void CCoreTriangleEffect::Touch( Vector p_pos, Vector p_normal, int index )
{
}

void CCoreTriangleEffect::Die( void )
{
}

void CCoreTriangleEffect::Force( void )
{
}

void CCoreTriangleEffect::Think( float time )
{
	if ( ( m_iCollisionFlags & TRI_ANIMATEDIE ) )
		AnimateAndDie( time );
	else
		Animate( time );

	Contract( time );
	Expand( time );
	Fade( time );
	Spin( time );
	CalculateVelocity( time );
	CheckCollision( time );
}