#include "pman_triangleffect.h"
#include "pman_main.h"
#include "pman_frustum.h"

#include "particleman.h"


void CCoreTriangleEffect::InitializeSprite( Vector *p_org, Vector *p_normal, model_s *sprite, float size, float brightness )
{
    Vector forward, right, up;

    m_flOriginalSize = 10.0f;
    m_flSize = 10.0f;
    m_flScaleSpeed = 0.0f;
    m_flContractSpeed = 0.0f;
    m_flStretchX = 1.0f;
    m_flStretchY = 1.0f;
    m_pTexture = 0;
    m_flOriginalBrightness = 255.0f;
    m_flBrightness = 255.0f;

    strcpy( m_szClassname, "particle" );

    m_flDieTime = 0.0f;
    m_flDampingTime = 0.0f;
    m_flNextCollisionTime = 0.0f;
    m_bInPVS = true;
    m_flFadeSpeed = -1.0f;
    m_iFramerate = 0;
    m_iNumFrames = 0;
    m_iFrame = 0;
    m_iCollisionFlags = 0;
    m_iRendermode = 0;
    m_iRenderFlags = 0;
    m_vVelocity.x = 0.0f;
    m_vVelocity.y = 0.0f;
    m_vVelocity.z = 0.0f;
    m_flBounceFactor = 1.0f;
    m_vOriginalAngles.x = 0.0f;
    m_vOriginalAngles.y = 0.0f;
    m_vOriginalAngles.z = 0.0f;
    m_vAngles.x = 0.0f;
    m_vAngles.y = 0.0f;
    m_vAngles.z = 0.0f;
    m_vAVelocity.x = 0.0f;
    m_vAVelocity.y = 0.0f;
    m_vAVelocity.z = 0.0f;
    m_vColor.x = 255.0f;
    m_vColor.y = 255.0f;
    m_vColor.z = 255.0f;
    m_bAffectedByForce = false;
    m_bInWater = 0;
    m_flTimeCreated = gEngfuncs.GetClientTime();
    m_vOrigin = *p_org;
    m_vPrevOrigin = *p_org;
    m_vAngles = *p_normal;
    m_pTexture = sprite;
    m_flOriginalSize = size;
    m_flOriginalBrightness = brightness;
    m_flSize = size;
    m_flBrightness = brightness;

    gEngfuncs.pfnAngleVectors( p_normal, &forward, &right, &up );

    m_vOriginalAngles = m_vAngles;

    m_vLowLeft.x = (p_org->x - (size * right.x) * 0.5f) - 0.5f * (up.x * size);
    m_vLowLeft.y = p_org->y - (size * right.y) * 0.5f - (up.y * size) * 0.5f;
    m_vLowLeft.z = p_org->z - (size * right.z) * 0.5f - (up.z * size) * 0.5f;

    m_vLowLeft.x = (size * right.x) + (size * right.x) + (p_org->x - (size * right.x) * 0.5f) - 0.5f * (up.x * size);
    m_vLowLeft.y = (size * right.y) + (size * right.y) + (p_org->y - (size * right.y) * 0.5f - (up.y * size) * 0.5f);
    m_vLowLeft.z = (size * right.z) + (size * right.z) + (p_org->z - (size * right.z) * 0.5f - (up.z * size) * 0.5f);

    m_vTopLeft.x = (size * up.x) + (size * up.x) + (p_org->x - (size * right.x) * 0.5f) - 0.5f * (up.x * size);
    m_vTopLeft.y = (size * up.y) + (size * up.y) + (p_org->y - (size * right.y) * 0.5f - (up.y * size) * 0.5f);
    m_vTopLeft.z = (size * up.z) + (size * up.z) + (p_org->z - (size * right.z) * 0.5f - (up.z * size) * 0.5f);
}

bool CCoreTriangleEffect::CheckVisibility( void )
{
    Vector mins, max;
    float radius;

    radius = m_flSize / 5.0f;

    if( gEngfuncs.GetClientTime() >= m_flNextPVSCheck )
    {
        mins.x = m_vOrigin.x - radius;
        mins.y = m_vOrigin.y - radius;
        mins.z = m_vOrigin.z - radius;

        max.x = m_vOrigin.x + radius;
        max.y = m_vOrigin.y + radius;
        max.z = m_vOrigin.z + radius;

        m_bInPVS = gEngfuncs.pTriAPI()->BoxInPVS( &mins.x, &max.x ) != 0;
        m_flNextPVSCheck = gEngfuncs.GetClientTime() + 0.1f;
    }

    if( ( m_iRenderFlags & 2 ) != 0 )
    {
        if( !( CFustnum::SphereInsideFrustum( &g_cFrustum, m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) ) )
            return false;
        
        if( m_bInPVS )
            return true;
        
        return ( m_iRenderFlags & 8 ) == 0;
    }

    if( ( m_iRenderFlags & 4 ) != 0 )
    {
        if( !( CFustnum::PlaneInsideFrustum( &g_cFrustum, m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) ) )
            return false;

        if( m_bInPVS )
            return true;
        
        return ( m_iRenderFlags & 8 ) == 0;
    }

    if( ( m_iRenderFlags & 1 ) != 0 )
    {
        if( !( CFustnum::PointInsideFrustum( &g_cFrustum, m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) ) )
            return false;

        if( m_bInPVS )
            return true;
        
        return ( m_iRenderFlags & 8 ) == 0;
    }

    return true;
}

void CCoreTriangleEffect::Draw( void )
{
    Vector forward, right, up;

    if( m_flDieTime == gEngfuncs.GetClientTime() )
        return;

    if( ( m_iRenderFlags & 16 ) != 0 )
    {
        if( ( m_iRenderFlags & 384 ) != 0 )
        {
            m_vAngles.x = g_vViewAngles.x;
            m_vAngles.y = g_vViewAngles.y;

            if( ( m_iRenderFlags & 256 ) != 0 )
                m_vAngles.z = g_vViewAngles.z;
        }
    }
    else
    {
        gEngfuncs.pTriAPI->LightAtPoint( &m_vOrigin, &m_vColor );

        if( ( m_iRenderFlags & 384 ) != 0 )
        {
            m_vAngles.x = g_vViewAngles.x;
            m_vAngles.y = g_vViewAngles.y;

            if( ( m_iRenderFlags & 256 ) != 0 )
                m_vAngles.z = g_vViewAngles.z;
    }

    if( ( m_iRenderFlags & 32 ) != 0 )
}

void CCoreTriangleEffect::Animate( float )
{

}

void CCoreTriangleEffect::AnimateAndDie( float )
{

}

void CCoreTriangleEffect::Expand( float )
{

}

void CCoreTriangleEffect::Contract( float )
{

}

void CCoreTriangleEffect::Fade( float )
{

}

void CCoreTriangleEffect::Spin( float )
{

}

void CCoreTriangleEffect::CalculateVelocity( float )
{

}

void CCoreTriangleEffect::CheckCollision( float )
{

}

void CCoreTriangleEffect::Touch( Vector, Vector, int )
{

}

void CCoreTriangleEffect::Die( void )
{

}

void CCoreTriangleEffect::Force( void )
{

}

void CCoreTriangleEffect::Think( float )
{

}