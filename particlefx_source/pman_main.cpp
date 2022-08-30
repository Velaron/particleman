/*
* pman_main.cpp
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

#include "interface.h"
#include "cvardef.h"
#include "util_vector.h"
#include "const.h"
#include "com_model.h"
#include "cdll_int.h"
#include "particleman.h"
#include "pman_frustum.h"
#include "pman_main.h"
#include "pman_misc.h"
#include "pman_force.h"
#include "pman_particlemem.h"

IParticleMan *g_pParticleMan;
cvar_t *cl_pmanstats;
ForceList g_pForceList;
cl_enginefunc_t gEngfuncs;
Vector g_vViewAngles;
float g_flGravity;

int g_iRenderMode = 1;

bool IsGamePaused( void )
{
	return gEngfuncs.GetClientTime() == g_flOldTime;
}

void IParticleMan_Active::SetUp( cl_enginefunc_t *pEnginefuncs )
{
	memcpy( &gEngfuncs, pEnginefuncs, sizeof( cl_enginefunc_t ) );

	cl_pmanstats = gEngfuncs.pfnRegisterVariable( "cl_pmanstats", "0", 0 );

	g_lMaxParticleClassSize = 0;
	MaxParticleClassSize( 0 );
}

void IParticleMan_Active::Update( void )
{
	g_pParticleMan = this;

	g_pForceList.UpdateForce( gEngfuncs.GetClientTime() );
	g_cFrustum.CalculateFrustum();
	UpdateParticles();

	if ( cl_pmanstats && cl_pmanstats->value == 1.0f )
	{
		gEngfuncs.Con_NPrintf( 15, "Number of Particles: %d", CMiniMem::Instance()->GetTotalParticles() );
		gEngfuncs.Con_NPrintf( 16, "Particles Drawn: %d", CMiniMem::Instance()->GetDrawnParticles() );
		gEngfuncs.Con_NPrintf( 17, "CMiniMem Free: %d%%", CMiniMem::Instance()->PercentUsed() );
	}
}

void IParticleMan_Active::SetRender( int iRender )
{
	g_iRenderMode = iRender;
}

void IParticleMan_Active::SetVariables( float flGravity, Vector p_vViewAngles )
{
	g_flGravity = flGravity;

	if ( !IsGamePaused() )
		g_vViewAngles = p_vViewAngles;
}

void IParticleMan_Active::ResetParticles( void )
{
	CMiniMem::Instance()->Reset();
	g_pForceList.ClearForceMembers();
}

CBaseParticle *IParticleMan_Active::CreateParticle( Vector p_org, Vector p_normal, struct model_s *sprite, float size, float brightness, const char *classname )
{
	CBaseParticle *pEffect = new CBaseParticle();

	pEffect->InitializeSprite( p_org, p_normal, sprite, size, brightness );
	strcpy( pEffect->m_szClassname, classname );

	return pEffect;
}

void IParticleMan_Active::ApplyForce( Vector p_vOrigin, Vector p_vDirection, float flRadius, float flStrength, float flDuration )
{
	ForceMember *pNewForce;

	if ( g_pForceList.m_iElements >= 128 )
		return;

	pNewForce = g_pForceList.AddForceMember();

	if ( pNewForce )
	{
		pNewForce->m_vOrigin = p_vOrigin;
		pNewForce->m_vDirection = p_vDirection;
		pNewForce->m_flRadius = flRadius;
		pNewForce->m_flStrength = flStrength;
		pNewForce->m_flDieTime = gEngfuncs.GetClientTime() + flDuration;
	}
}

void IParticleMan_Active::AddCustomParticleClassSize( unsigned long lSize )
{
	MaxParticleClassSize( lSize );
}

char *IParticleMan_Active::RequestNewMemBlock( int iSize )
{
	if ( !g_iRenderMode )
		return NULL;

	return CMiniMem::Instance()->newBlock();
}

void IParticleMan_Active::CoreInitializeSprite( CCoreTriangleEffect *pParticle, Vector p_org, Vector p_normal, struct model_s *sprite, float size, float brightness )
{
	pParticle->CCoreTriangleEffect::InitializeSprite( p_org, p_normal, sprite, size, brightness );
}

void IParticleMan_Active::CoreThink( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::Think( time );
}

void IParticleMan_Active::CoreDraw( CCoreTriangleEffect *pParticle )
{
	pParticle->CCoreTriangleEffect::Draw();
}

void IParticleMan_Active::CoreAnimate( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::Animate( time );
}

void IParticleMan_Active::CoreAnimateAndDie( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::AnimateAndDie( time );
}

void IParticleMan_Active::CoreExpand( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::Expand( time );
}

void IParticleMan_Active::CoreContract( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::Contract( time );
}

void IParticleMan_Active::CoreFade( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::Fade( time );
}

void IParticleMan_Active::CoreSpin( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::Spin( time );
}

void IParticleMan_Active::CoreCalculateVelocity( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::CalculateVelocity( time );
}

void IParticleMan_Active::CoreCheckCollision( CCoreTriangleEffect *pParticle, float time )
{
	pParticle->CCoreTriangleEffect::CheckCollision( time );
}

void IParticleMan_Active::CoreTouch( CCoreTriangleEffect *pParticle, Vector p_pos, Vector p_normal, int index )
{
	pParticle->CCoreTriangleEffect::Touch( p_pos, p_normal, index );
}

void IParticleMan_Active::CoreDie( CCoreTriangleEffect *pParticle )
{
	pParticle->CCoreTriangleEffect::Die();
}

void IParticleMan_Active::CoreForce( CCoreTriangleEffect *pParticle )
{
	pParticle->CCoreTriangleEffect::Force();
}

bool IParticleMan_Active::CoreCheckVisibility( CCoreTriangleEffect *pParticle )
{
	return pParticle->CCoreTriangleEffect::CheckVisibility();
}

IParticleMan *CreateActiveMan( void )
{
	return (IParticleMan *)new IParticleMan_Active();
}

IBaseInterface *CreateParticleMan( void )
{
	return (IBaseInterface *)new IParticleMan_Active();
}

EXPOSE_INTERFACE_FN( CreateParticleMan, IParticleMan_Active, PARTICLEMAN_INTERFACE );