#include "pman_main.h"

#include "interface.h"

#include "pman_force.h"
#include "particleman.h"
#include "pman_particlemem.h"

ForceList g_pForceList;

static cvar_t* cl_pmanstats = nullptr;

static bool g_iRenderMode = true;

bool IsGamePaused( void )
{
	return gEngfuncs.GetClientTime() == g_flOldTime;
}

void IParticleMan_Active::SetUp( cl_enginefuncs_s *pEngineFunc )
{
	//TODO: WHAMER
	cl_pmanstats = gEngfuncs.pfnRegisterVariable("cl_pmanstats", "0", 0);
}

void IParticleMan_Active::Update()
{
	if( g_pForceList.pFirst )
		return;
	
	if( g_pForceList.pFirst->m_flDieTime == 0.0f && m_flDieTime >= gEngfuncs.GetClientTime() )
//TODO: WHAMER( my logic is dead )
	{
		g_pForceList.pFirst->m_pNext;

		if( g_pForceList.pLast == g_pForceList.pFirst )
		{
			g_pForceList.pFirst = 0;
			g_pForceList.pCurrent = 0;
			g_pForceList.pLast = 0;
		}
		else
		{
			g_pForceList.pFirst->m_pNext->m_pPrevious = 0;

			if( g_pForceList.pFirst == g_pForceList.pLast )
			{
				g_pForceList.pFirst->m_pNext = 0;
				g_pForceList.pFirst = g_pForceList.pCurrent; 
				g_pForceList.pFirst = g_pForceList.pLast;
			}
		}
		--g_pForceList.m_iElements;
	}
	else
	{
		g_pForceList.pFirst->m_pNext->m_pPrevious = g_pForceList.pFirst->m_pPrevious;
	}

	for( hz )
	{
		CMiniMem::ApplyForce( &p_vOrigin, &p_vDirection, flRadius, flStrength );
	}

	if ( cl_pmanstats->value == 1.0f )
	{
		gEngfuncs.Con_NPrintf( 15, "Number of Particles: %d", CMiniMem::Instance()->GetTotalParticles() );
		gEngfuncs.Con_NPrintf( 16, "Particles Drawn: %d", CMiniMem::Instance()->GetDrawnParticles() );
		gEngfuncs.Con_NPrintf( 17, "CMiniMem Free: %d%%", CMiniMem::Instance()->PercentUsed() );
	}
}

void IParticleMan_Active::SetRender( int iRender )
{
	g_iRenderMode = iRender != 0;
}

void IParticleMan_Active::SetVariables( float flGravity, Vector *p_vViewAngles )
{
	g_flGravity = flGravity;

	if (gEngfuncs.GetClientTime() != g_flOldTime)
	{
		g_vViewAngles = *p_vViewAngles;
	}
}

void IParticleMan_Active::ResetParticles()
{
	if( g_pForceList.pFirst )
		return;
	
	if( !m_pNext )
	{
		--g_pForceList.m_iElements;
		g_pForceList.pFirst = 0;
	}
	
	while( 1 )
	{
		if( m_pNext == g_pForceList.pCurrent )
			g_pForceList.pCurrent = 0;
		
		--g_pForceList.m_iElements;

		if( m_pNext )
			break;
	}

	if( g_pForceList.pFirst )
	{
		--g_pForceList.m_iElements;
		g_pForceList.pFirst = 0;
	}
}

CBaseParticle *IParticleMan_Active::CreateParticle( Vector org, Vector normal, struct model_s * sprite, float size, float brightness, const char *classname )
{
	auto particle = new CBaseParticle();

	particle->InitializeSprite(org, normal, sprite, size, brightness);
	strncpy(particle->m_szClassname, classname, sizeof(particle->m_szClassname) - 1);
	particle->m_szClassname[sizeof(particle->m_szClassname) - 1] = '\0';

	return particle;
}

void IParticleMan_Active::ApplyForce( Vector *p_vOrigin, Vector *p_vDirection, float flRadius, float flStrength, float flDuration )
{
	if ( g_pForceList.m_iElements <= 127 )
	{
		return;
	}

	ForceMember member;

	member.m_vOrigin = *p_vOrigin;
	member.m_vDirection = *p_vDirection;
	member.m_flRadius = flRadius;
	member.m_flStrength = flStrength;
	member.m_flDieTime = gEngfuncs.GetClientTime() + flDuration;
}

void IParticleMan_Active::AddCustomParticleClassSize( unsigned int iSize )
{
	MaxParticleClassSize( iSize );
}

char *IParticleMan_Active::RequestNewMemBlock( int iSize )
{
	auto memory = CMiniMem::newBlock();

	if( !g_iRenderMode )
		return;
	
	return memory->Instance();
}

//idk what is this
auto CreateParticleMan()
{
	return (IBaseInterface *)new IParticleMan_Active();
}

EXPOSE_INTERFACE_FN(CreateParticleMan, IParticleMan_Active, PARTICLEMAN_INTERFACE);