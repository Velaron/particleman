/*
* pman_main.h
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

#ifndef __PMAN_MAIN_H__
#define __PMAN_MAIN_H__

#include "cdll_int.h"
#include "particleman.h"
#include "pman_force.h"

extern ForceList g_pForceList;
extern cl_enginefunc_t gEngfuncs;

extern long g_lMaxParticleClassSize;

extern Vector g_vViewAngles;
extern float g_flGravity;

bool IsGamePaused( void );
void UpdateParticles( void );
long MaxParticleClassSize( unsigned long lSize );

class IParticleMan_Active : public IParticleMan
{
public:
	void SetUp( cl_enginefunc_t *pEnginefuncs );
	void Update( void );
	void SetVariables( float flGravity, Vector p_vViewAngles );
	void ResetParticles( void );
	void ApplyForce( Vector p_vOrigin, Vector p_vDirection, float flRadius, float flStrength, float flDuration );
	void AddCustomParticleClassSize( unsigned long lSize );

	CBaseParticle *CreateParticle( Vector p_org, Vector p_normal, struct model_s *sprite, float size, float brightness, const char *classname );

	char *RequestNewMemBlock( int iSize );

	void CoreInitializeSprite( CCoreTriangleEffect *pParticle, Vector p_org, Vector p_normal, struct model_s *sprite, float size, float brightness );
	void CoreThink( CCoreTriangleEffect *pParticle, float time );
	void CoreDraw( CCoreTriangleEffect *pParticle );
	void CoreAnimate( CCoreTriangleEffect *pParticle, float time );
	void CoreAnimateAndDie( CCoreTriangleEffect *pParticle, float time );
	void CoreExpand( CCoreTriangleEffect *pParticle, float time );
	void CoreContract( CCoreTriangleEffect *pParticle, float time );
	void CoreFade( CCoreTriangleEffect *pParticle, float time );
	void CoreSpin( CCoreTriangleEffect *pParticle, float time );
	void CoreCalculateVelocity( CCoreTriangleEffect *pParticle, float time );
	void CoreCheckCollision( CCoreTriangleEffect *pParticle, float time );
	void CoreTouch( CCoreTriangleEffect *pParticle, Vector p_pos, Vector p_normal, int index );
	void CoreDie( CCoreTriangleEffect *pParticle );
	void CoreForce( CCoreTriangleEffect *pParticle );
	bool CoreCheckVisibility( CCoreTriangleEffect *pParticle );
	void SetRender( int iRender );
};

#endif // __PMAN_MAIN_H__
