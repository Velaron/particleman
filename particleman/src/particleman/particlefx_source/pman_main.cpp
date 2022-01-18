#include "pman_main.h"

#include "interface.h"

#include "pman_force.h"
#include "particleman.h"

ForceList g_pForceList;

void DBG_AssertFunction(int, char const*, char const*, int, char const*)
{
	
}

void dll_unload()
{
	
}

bool IsGamePaused()
{
	
}

void IParticleMan_Active::SetUp(cl_enginefuncs_s*)
{
	
}

void IParticleMan_Active::Update()
{
	
}

void IParticleMan_Active::SetRender(int)
{
	
}

void IParticleMan_Active::SetVariables(float, Vector)
{
	
}

void IParticleMan_Active::ResetParticles()
{
	
}

CBaseParticle *IParticleMan_Active::CreateParticle(Vector, Vector, model_s*, float, float, char const*)
{
	
}

void IParticleMan_Active::ApplyForce(Vector, Vector, float, float, float)
{
	
}

void IParticleMan_Active::AddCustomParticleClassSize(unsigned long)
{
	
}

char *IParticleMan_Active::RequestNewMemBlock(int)
{
	
}

void IParticleMan_Active::CoreInitializeSprite(CCoreTriangleEffect*, Vector, Vector, model_s*, float, float)
{
	
}

void IParticleMan_Active::CoreThink(CCoreTriangleEffect*, float)
{
	
}

void IParticleMan_Active::CoreDraw(CCoreTriangleEffect*)
{
	
}

void IParticleMan_Active::CoreAnimate(CCoreTriangleEffect*, float)
{
	
}

void IParticleMan_Active::CoreAnimateAndDie(CCoreTriangleEffect*, float)
{
	
}

void IParticleMan_Active::CoreExpand(CCoreTriangleEffect*, float)
{
	
}

void IParticleMan_Active::CoreFade(CCoreTriangleEffect*, float)
{
	
}

void IParticleMan_Active::CoreSpin(CCoreTriangleEffect*, float)
{
	
}


void IParticleMan_Active::CoreCalculateVelocity(CCoreTriangleEffect*, float)
{
	
}

void IParticleMan_Active::CoreCheckCollision(CCoreTriangleEffect*, float)
{
	
}

void IParticleMan_Active::CoreTouch(CCoreTriangleEffect*, Vector, Vector, int)
{
	
}

void IParticleMan_Active::CoreDie(CCoreTriangleEffect*)
{
	
}

void IParticleMan_Active::CoreForce(CCoreTriangleEffect*)
{
	
}

bool IParticleMan_Active::CoreCheckVisibility(CCoreTriangleEffect*)
{
	
}

auto CreateParticleMan()
{
	return (IBaseInterface *)new IParticleMan_Active();
}

EXPOSE_INTERFACE_FN(CreateParticleMan, IParticleMan_Active, PARTICLEMAN_INTERFACE);