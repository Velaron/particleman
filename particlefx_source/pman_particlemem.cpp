/*
* pman_particlemem.cpp
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
#include "cl_entity.h"
#include "pman_particlemem.h"
#include "pman_main.h"

VectorOfMemoryBlocks CMiniMem::m_vecMemoryPool;
long CMiniMem::m_lMemoryPoolSize;
long CMiniMem::m_lMemoryBlockSize;
long CMiniMem::m_lMaxBlocks;
CMiniMem *CMiniMem::_instance;

long g_lMaxParticleClassSize;
MemList m_FreeMem;
MemList m_ActiveMem;
float g_flOldTime;

long MaxParticleClassSize( unsigned long lSize )
{
	unsigned long lMaxSize;

	lMaxSize = sizeof( CBaseParticle );

	if ( g_lMaxParticleClassSize >= sizeof( CBaseParticle ) )
		lMaxSize = g_lMaxParticleClassSize;

	if ( lSize && g_lMaxParticleClassSize < lSize )
		lMaxSize = lSize;

	g_lMaxParticleClassSize = lMaxSize;

	return lMaxSize;
}

CMiniMem::CMiniMem( long lMemoryPoolSize, long lMaxBlockSize )
{
	m_lMemoryPoolSize = lMemoryPoolSize;
	m_lMemoryBlockSize = lMaxBlockSize;
	m_lMaxBlocks = lMemoryPoolSize >> sizeof( visibleparticles_t );

	for ( long i = 0; i < m_lMaxBlocks; i++ )
	{
		MemoryBlock *pNewBlock = new MemoryBlock( m_lMemoryBlockSize );

		m_vecMemoryPool.push_back( pNewBlock );
		m_FreeMem.Push( pNewBlock );
	}

	m_pVisibleParticles = new visibleparticles_t[m_lMaxBlocks];
}

CMiniMem::~CMiniMem( void )
{
	m_FreeMem.Reset();
	m_vecMemoryPool.clear();

	if ( m_pVisibleParticles )
		delete[] m_pVisibleParticles; // Velaron: set to NULL?
}

void CMiniMem::Shutdown( void )
{
	if ( _instance )
		delete _instance;
}

char *CMiniMem::AllocateFreeBlock( void )
{
	MemoryBlock *pNewBlock = m_FreeMem.Pop();

	m_ActiveMem.Push( pNewBlock );

	return pNewBlock->Memory();
}

bool CMiniMem::CheckSize( int iSize )
{
	return iSize <= CMiniMem::Instance()->m_lMemoryBlockSize;
}

char *CMiniMem::newBlock( void )
{
	if ( !m_FreeMem.Front() )
	{
		gEngfuncs.Con_DPrintf( "Particleman is out of memory, too many particles" );
		return NULL;
	}

	return AllocateFreeBlock();
}

void CMiniMem::deleteBlock( MemoryBlock *p )
{
	m_ActiveMem.Delete( p );
	m_FreeMem.Push( p );
}

long CMiniMem::PercentUsed( void )
{
	MemoryBlock *pFreeBlock = m_FreeMem.Front();
	long iFreeNum = 0;

	while ( pFreeBlock )
	{
		pFreeBlock = pFreeBlock->next;
		iFreeNum++;
	}

	return ( iFreeNum * 100 ) / m_lMaxBlocks;
}

long CMiniMem::MaxBlockSize( void )
{
	return m_lMemoryBlockSize;
}

CMiniMem *CMiniMem::Instance( void )
{
	if ( !_instance )
		_instance = new CMiniMem( 30000, 30000 / 16 ); // Velaron: what are these?

	return _instance;
}

int ParticleSortFn( const void *elem1, const void *elem2 )
{
	CCoreTriangleEffect *pParticle1 = (CCoreTriangleEffect *)elem1;
	CCoreTriangleEffect *pParticle2 = (CCoreTriangleEffect *)elem2;

	if ( pParticle1->GetPlayerDistance() <= pParticle2->GetPlayerDistance() )
		return pParticle1->GetPlayerDistance() != pParticle2->GetPlayerDistance();

	return -1;
}

// Velaron: TODO
void CMiniMem::ProcessAll( void )
{
	MemoryBlock *currentBlock = m_ActiveMem.Front();
	float time = gEngfuncs.GetClientTime();

	m_iTotalParticles = m_iParticlesDrawn = 0;

	if ( m_lMaxBlocks * sizeof( visibleparticles_t ) > sizeof( visibleparticles_t ) )
	{
	}

	while ( pNode )
	{
		CCoreTriangleEffect *pEffect = (CCoreTriangleEffect *)pNode->Memory();

		if ( !pEffect )
			continue;

		m_iTotalParticles++;

		if ( pEffect->CheckVisibility() )
		{
			pEffect->SetPlayerDistance( ( gEngfuncs.GetLocalPlayer()->origin - pEffect->m_vOrigin ).Length() );
			m_pVisibleParticles[m_iParticlesDrawn].pVisibleParticle = pEffect;
			CMiniMem::Instance()->IncreaseParticlesDrawn();
		}

		if ( !IsGamePaused() )
			pEffect->Think( time );

		if ( pEffect->m_flDieTime != 0.0f && time >= pEffect->m_flDieTime )
		{
			pEffect->Die();
			deleteBlock( pNode );
		}

		pNode = pNode->next;
	}

	qsort( m_pVisibleParticles, m_iParticlesDrawn, sizeof( visibleparticles_t ), ParticleSortFn );

	for ( int i = 0; i < m_iParticlesDrawn; i++ )
		m_pVisibleParticles[i].pVisibleParticle->Draw();

	g_flOldTime = time;
}

// Velaron: TODO
int CMiniMem::ApplyForce( Vector p_vOrigin, Vector p_vDirection, float flRadius, float flStrength )
{
	Vector mins, maxs;
	float radius = flRadius * flRadius;

	for( MemoryBlock *currentBlock = m_ActiveMem.Front(); currentBlock; currentBlock = currentBlock->next )
	{
		CCoreTriangleEffect *pEffect = (CCoreTriangleEffect *)currentBlock->Memory();

		if ( !pEffect || !pEffect->m_bAffectedByForce )
			continue;

		float size = pEffect->m_flSize / 5.0f;

		mins = pEffect->m_vOrigin - Vector( size, size, size );
		maxs = pEffect->m_vOrigin + Vector( size, size, size );
	}
}

void CMiniMem::Reset( void )
{
	m_FreeMem.Reset();
	m_ActiveMem.Reset();

	for ( size_t i = 0; i < m_vecMemoryPool.size(); i++ )
		m_FreeMem.Push( m_vecMemoryPool[i] );
}

void UpdateParticles( void )
{
	CMiniMem::Instance()->ProcessAll();
}