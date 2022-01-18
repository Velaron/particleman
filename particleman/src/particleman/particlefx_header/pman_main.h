#pragma once

#include "particleman.h"
#include "pman_force.h"

float g_flOldTime;

float g_flGravity;
Vector g_vViewAngles;

float m_flDieTime;	//time to remove an object

extern ForceList g_pForceList;

class IParticleMan_Active : public IParticleMan
{
public:
	IParticleMan_Active()
	{
		g_pForceList.m_iElements = 0;
		g_pForceList.pFirst = 0;
		g_pForceList.pCurrent = 0;
		g_pForceList.pLast = 0;
	}

	void SetUp(cl_enginefunc_t *pEnginefuncs)
	{

	}

	void Update(void)
	{

	}

	void SetVariables(float flGravity, Vector *p_vViewAngles)
	{

	}

	void ResetParticles(void)
	{

	}

	void ApplyForce(Vector *p_vOrigin, Vector *p_vDirection, float flRadius, float flStrength, float flDuration)
	{

	}

	void AddCustomParticleClassSize(unsigned int lSize)
	{

	}

	CBaseParticle *CreateParticle(Vector org, Vector normal, struct model_s * sprite, float size, float brightness, const char *classname)
	{
		return nullptr;
	}

	char *RequestNewMemBlock(int iSize)
	{
		return nullptr;
	}

	void CoreInitializeSprite(CCoreTriangleEffect *pParticle, Vector *p_org, Vector *p_normal, model_s *sprite, float size, float brightness)
	{

	}

	void CoreThink(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreDraw(CCoreTriangleEffect *pParticle)
	{

	}

	void CoreAnimate(CCoreTriangleEffect *pParticle, float time)
	{

	}
	void CoreAnimateAndDie(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreExpand(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreContract(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreFade(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreSpin(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreCalculateVelocity(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreCheckCollision(CCoreTriangleEffect *pParticle, float time)
	{

	}

	void CoreTouch(CCoreTriangleEffect *pParticle, Vector pos, Vector normal, int index)
	{

	}

	void CoreDie(CCoreTriangleEffect *pParticle)
	{

	}

	void CoreForce(CCoreTriangleEffect *pParticle)
	{

	}

	bool CoreCheckVisibility(CCoreTriangleEffect *pParticle)
	{
		return false;
	}

	void SetRender(int iRender)
	{

	}
};

int MaxParticleClassSize( unsigned int iSize );