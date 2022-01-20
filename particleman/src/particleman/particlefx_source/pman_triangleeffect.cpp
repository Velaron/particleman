#include "pman_triangleffect.h"
#include "pman_main.h"
#include "pman_frustum.h"

#include "particleman.h"
#include "triangleapi.h"
#include "mathlib.h"

#include "pm_defs.h"


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
    m_iRendermode = kRenderNormal;
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
    m_bInWater = false;
    m_flTimeCreated = gEngfuncs.GetClientTime();
    m_vOrigin = *p_org;
    m_vPrevOrigin = *p_org;
    m_vAngles = *p_normal;
    m_pTexture = sprite;
    m_flOriginalSize = size;
    m_flOriginalBrightness = brightness;
    m_flSize = size;
    m_flBrightness = brightness;

    gEngfuncs.pfnAngleVectors( p_normal, forward, right, up );

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

        m_bInPVS = gEngfuncs.pTriAPI->BoxInPVS(mins, max) != 0;
        m_flNextPVSCheck = gEngfuncs.GetClientTime() + 0.1f;
    }

    if( ( m_iRenderFlags & CULL_FRUSTUM_SPHERE ) != 0 )
    {
        if( !g_cFrustum.SphereInsideFrustum( &g_cFrustum, m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) )
            return false;
        
        if( m_bInPVS )
            return true;
        
        return ( m_iRenderFlags & CULL_PVS ) == 0;
    }

    if( ( m_iRenderFlags & CULL_FRUSTUM_PLANE ) != 0 )
    {
        if( !g_cFrustum.PlaneInsideFrustum( &g_cFrustum, m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) ) 
            return false;

        if( m_bInPVS )
            return true;
        
        return ( m_iRenderFlags & CULL_PVS ) == 0;
    }

    if( ( m_iRenderFlags & CULL_FRUSTUM_POINT ) != 0 )
    {
        if( !g_cFrustum.PointInsideFrustum( &g_cFrustum, m_vOrigin.x, m_vOrigin.y, m_vOrigin.z, radius ) )
            return false;

        if( m_bInPVS )
            return true;
        
        return ( m_iRenderFlags & CULL_PVS ) == 0;
    }

    return true;
}

void CCoreTriangleEffect::Draw( void )
{
    Vector forward, right, up, vColor;
    float x, y, z;

    if( m_flDieTime == gEngfuncs.GetClientTime() )
        return;

    if( ( m_iRenderFlags & LIGHT_NONE ) != 0 )
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
        gEngfuncs.pTriAPI->LightAtPoint( m_vOrigin, m_vColor );

        if( ( m_iRenderFlags & 384 ) != 0 )
        {
            m_vAngles.x = g_vViewAngles.x;
            m_vAngles.y = g_vViewAngles.y;

            if( ( m_iRenderFlags & 256 ) != 0 )
                m_vAngles.z = g_vViewAngles.z;
    }

    if( ( m_iRenderFlags & LIGHT_NONE ) != 0 )
    {
        x = m_vColor.x;
        y = m_vColor.y;
        z = m_vColor.z;
    }

    else if( ( m_iRenderFlags & LIGHT_COLOR ) != 0 )
    {
        x = vColor.x / m_vColor.x * 255.0f;
        y = vColor.y / m_vColor.y * 255.0f;
        z = vColor.z / m_vColor.z * 255.0f;
    }

    else if( ( m_iRenderFlags & LIGHT_INTENSITY ) != 0 )
    {
        x = ((vColor.x + vColor.y + vColor.z) / 3.0f) / m_vColor.x * 255.0f;
        y = ((vColor.x + vColor.y + vColor.z) / 3.0f) / m_vColor.y * 255.0f;
        z = ((vColor.x + vColor.y + vColor.z) / 3.0f) / m_vColor.z * 255.0f;
    }

	
	gEngfuncs.pfnAngleVectors( m_vAngles, forward, right, up );

	const float radius = m_flSize;
	const Vector width = right * radius * m_flStretchX;
	const Vector height = up * radius * m_flStretchY;

	const Vector lowLeft = m_vOrigin - (width * 0.5) - (up * radius * 0.5);

	const Vector lowRight = lowLeft + width;
	const Vector topLeft = lowLeft + height;
	const Vector topRight = lowRight + height;

    gEngfuncs.pTriAPI->SpriteTexture( m_pTexture, m_iFrame );
    gEngfuncs.pTriAPI->RenderMode( m_iRendermode );
    gEngfuncs.pTriAPI->CullFace( TRI_NONE );
    gEngfuncs.pTriAPI->Begin( TRI_QUADS );
    gEngfuncs.pTriAPI->Color4f( x / 255.0f, y / 255.0f, z / 255.0f, m_flBrightness / 255.0f );
    gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
    gEngfuncs.pTriAPI->Vertex3fv( topLeft );
    gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
    gEngfuncs.pTriAPI->Vertex3fv( lowLeft );
    gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
    gEngfuncs.pTriAPI->Vertex3fv( lowRight );
    gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
    gEngfuncs.pTriAPI->Vertex3fv( topRight );
    gEngfuncs.pTriAPI->End( );
    gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
    gEngfuncs.pTriAPI->CullFace( TRI_FRONT );
}

void CCoreTriangleEffect::Animate( float time )
{
    if ( m_iFramerate != 0 && m_iNumFrames != 0 )
	{
		m_iFrame = ( m_iFramerate * ( time - m_flTimeCreated ) ) % m_iNumFrames;
	}
}

void CCoreTriangleEffect::AnimateAndDie( float time )
{
    	if ( m_iFramerate != 0 && m_iNumFrames != 0 )
	{
		const float frame = m_iFramerate * ( time - m_flTimeCreated );

		if ( frame > m_iNumFrames )
		{
			m_flDieTime = time;
		}
		else
		{
			m_iFrame = frame;
		}

		if ( m_iFrame > m_iNumFrames )
		{
			m_iFrame = m_iNumFrames - 1;
		}
	}
}

void CCoreTriangleEffect::Expand( float time )
{
	if ( m_flScaleSpeed != 0 )
	{
		m_flSize = ( m_flScaleSpeed * 30.0 * ( time - m_flTimeCreated ) ) + m_flOriginalSize;

		if ( m_flSize < 0.0001 )
		{
			m_flDieTime = time;
		}
	}
}

void CCoreTriangleEffect::Contract( float time )
{
    	if ( m_flContractSpeed != 0 )
	{
		m_flSize = m_flOriginalSize - ( m_flContractSpeed * 30.0 * ( time - m_flTimeCreated ) );

		if ( m_flSize < 0.0001 )
		{
			m_flDieTime = time;
		}
	}
}

void CCoreTriangleEffect::Fade( float time )
{
	if ( m_flFadeSpeed >= -0.5 )
	{
		if ( m_flFadeSpeed == 0 )
		{
			m_flBrightness = ( 1.0 - ( time - m_flTimeCreated ) / ( m_flDieTime - m_flTimeCreated ) );
				* m_flOriginalBrightness;
		}
		else
		{
			m_flBrightness = m_flOriginalBrightness - m_flFadeSpeed * 30.0 * ( time - m_flTimeCreated );
		}

		if ( m_flBrightness < 1 )
		{
			m_flDieTime = time;
		}
	}
}

void CCoreTriangleEffect::Spin( float time )
{
    Vector point = m_vAVelocity;

    if ( m_vAVelocity.x != 0.0f )
	{   
        m_vAngles = m_vOriginalAngles + m_vAVelocity * ( VectorNormalize ( point ) * 30.0 * ( time - m_flTimeCreated ) );
		return;
	}

        if ( m_vAVelocity.y != 0.0f )
	{
        m_vAngles = m_vOriginalAngles + m_vAVelocity * ( VectorNormalize( point ) * 30.0 * ( time - m_flTimeCreated ) );
		return;
	}

        if ( m_vAVelocity.z != 0.0f )
	{
        m_vAngles = m_vOriginalAngles + m_vAVelocity * ( VectorNormalize( point ) * 30.0 * ( time - m_flTimeCreated ) );
		return;
	}


}

void CCoreTriangleEffect::CalculateVelocity( float time )
{
	const float deltaTime = time - g_flOldTime;
	const float gravity = -deltaTime * g_flGravity * m_flGravity;

	if ( m_vVelocity == g_vecZero && gravity == 0 )
	{
		return;
	}

	if ( ( m_iCollisionFlags & TRI_SPIRAL) != 0 )
	{
		m_vOrigin = m_vOrigin + m_vVelocity * deltaTime;

		m_vOrigin.x += sin( time * 5.0 + reinterpret_cast<std::intptr_t>( this ) ) * 2;
		m_vOrigin.y += sin( time * 7.5 + reinterpret_cast<std::intptr_t>( this ) );
	}
	else
	{
		m_vOrigin = m_vOrigin + m_vVelocity * deltaTime;
	}

	m_vVelocity.z = m_vVelocity.z + gravity;
}

void CCoreTriangleEffect::CheckCollision( float time )
{
	if (m_iCollisionFlags == 0)
	{
		return;
	}

	m_flNextCollisionTime = time;

	if ((m_iCollisionFlags & (TRI_WATERTRACE | TRI_COLLIDEALL | TRI_COLLIDEWORLD)) == 0)
	{
		return;
	}

	pmtrace_t trace;

	bool collided = false;

	if ((m_iCollisionFlags & TRI_COLLIDEALL) != 0)
	{
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(m_vPrevOrigin, m_vOrigin, PM_STUDIO_BOX, -1, &trace);

		if (trace.fraction != 1.0)
		{
			//Called but never used, probably unfinished code for colliding with other entities.
			//auto entity = gEngfuncs.pEventAPI->EV_GetPhysent(trace.ent);

			//Collided with something other than world, ignore.
			if (0 == trace.ent)
			{
				collided = true;
			}
		}
	}
	else if ((m_iCollisionFlags & TRI_COLLIDEWORLD) != 0)
	{
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(m_vPrevOrigin, m_vOrigin, PM_WORLD_ONLY | PM_STUDIO_BOX, -1, &trace);

		if (trace.fraction != 1.0)
		{
			m_vVelocity = m_vVelocity * 0.6;

			if (m_vVelocity.Length() < 10)
			{
				m_iCollisionFlags = 0;
				m_vVelocity = g_vecZero;
				m_vAVelocity = g_vecZero;
				m_vOrigin = trace.endpos;
			}

			collided = true;
		}
	}

	if (collided)
	{
		const float frametime = time - g_flOldTime;

		m_vOrigin = m_vPrevOrigin + m_vVelocity * (trace.fraction * frametime);

		float bounce;

		bool dead = false;

		if (trace.plane.normal.z <= 0.9
			|| m_vVelocity.z > 0
			|| (m_vVelocity.z < -frametime * g_flGravity * m_flGravity * 3.0))
		{
			if ((m_iCollisionFlags & TRI_COLLIDEKILL) != 0)
			{
				m_flDieTime = gEngfuncs.GetClientTime();
				dead = true;
			}
			else
			{
				bounce = m_flBounceFactor * 0.5;

				if (bounce != 0)
				{
					const float dot = DotProduct(trace.plane.normal, m_vVelocity) * -2;
					m_vVelocity = m_vVelocity + trace.plane.normal * dot;
					m_vAngles.y = -m_vAngles.y;
				}
			}
		}
		else
		{
			//Particle fell on an (almost) flat surface and has no velocity to bounce back up; disable collisions from now on.
			bounce = 0;
			m_vVelocity = g_vecZero;
			m_vAVelocity = g_vecZero;
			m_iCollisionFlags = 0;
		}

		if (!dead && bounce != 1)
		{
			m_vVelocity = m_vVelocity * bounce;
			m_vAngles = m_vAngles * 0.9;
		}

		Touch(trace.endpos, trace.plane.normal, trace.ent);
	}
	else if ((m_iCollisionFlags & TRI_WATERTRACE) != 0)
	{
		if (gEngfuncs.PM_PointContents(m_vOrigin, nullptr) == CONTENTS_WATER && !m_bInWater)
		{
			Touch(m_vOrigin, {0, 0, 1}, 0);

			m_bInWater = true;

			if ((m_iCollisionFlags & TRI_COLLIDEKILL) != 0)
			{
				m_flDieTime = gEngfuncs.GetClientTime();
			}
		}
	}

	m_vPrevOrigin = m_vOrigin;
}

void CCoreTriangleEffect::Touch( Vector pos, Vector normal, int index )
{
    // nothing
}

void CCoreTriangleEffect::Die( void )
{
    //nothing
}

void CCoreTriangleEffect::Force( void )
{
    //nothing
}

void CCoreTriangleEffect::Think( float time )
{
	if ((m_iCollisionFlags & TRI_ANIMATEDIE) != 0)
	{
		AnimateAndDie(time);
	}
	else
	{
		Animate(time);
	}

	Contract(time);
	Expand(time);
	Fade(time);
	Spin(time);
	CalculateVelocity(time);
	CheckCollision(time);
}