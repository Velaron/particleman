/*
* pman_force.h
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

#ifndef __PMAN_FORCE_H__
#define __PMAN_FORCE_H__

#include "util_vector.h"

class ForceMember
{
public:
	int m_iIndex;
	ForceMember *m_pNext;
	ForceMember *m_pPrevious;
	Vector m_vOrigin;
	Vector m_vDirection;
	float m_flRadius;
	float m_flStrength;
	float m_flDieTime;

	ForceMember() :
	    m_pNext( NULL ), m_pPrevious( NULL ),
	    m_flRadius( 0.0f ), m_flStrength( 0.0f ), m_flDieTime( 0.0f ) { }
};

class ForceList
{
public:
	int m_iElements;

	ForceMember *pFirst;
	ForceMember *pCurrent;
	ForceMember *pLast;

	ForceMember *AddForceMember( void )
	{
		if ( pFirst )
		{
			pCurrent->m_pNext = new ForceMember();
			pCurrent = pCurrent->m_pNext;
			pCurrent->m_iIndex = ++m_iElements;
			pCurrent->m_pPrevious = pCurrent;
		}
		else
		{
			pFirst = new ForceMember();
			pCurrent = pFirst;
			pCurrent->m_iIndex = ++m_iElements;
		}

		pLast = pCurrent;

		return pCurrent;
	}

	void ClearForceMembers( void )
	{
		if ( !pFirst )
			return;

		ForceMember *pTemp = pFirst;

		while ( pTemp )
		{
			ForceMember *pTemp2 = pTemp->m_pNext;

			if ( !pTemp2 )
				pFirst = NULL;

			if ( pTemp == pCurrent )
				pCurrent = NULL;

			delete pTemp;
			m_iElements--;

			pTemp = pTemp2;
		}
	}

	void UpdateForce( float m_flTime )
	{
		if ( !pFirst )
			return;

		ForceMember *pTemp = pFirst;

		while ( pTemp )
		{
			if ( pTemp->m_flDieTime == 0.0f || m_flTime <= pTemp->m_flDieTime )
			{
				pTemp = pTemp->m_pNext;
				continue;
			}

			if ( pFirst == pTemp )
			{
				if ( pTemp == pLast )
				{
					delete pLast;
					pLast = NULL;

					delete pCurrent;
					pCurrent = NULL;

					delete pFirst;
					pFirst = NULL;

					pTemp = NULL;
				}
				else
				{
					pTemp = pTemp->m_pNext;

					delete pTemp->m_pPrevious;
					pTemp->m_pPrevious = NULL;

					pFirst = pTemp;

					if ( pTemp == pLast )
					{
						pTemp->m_pNext = NULL;
						pCurrent = pLast;
					}
				}

				m_iElements--;
			}
			else
			{
				if ( pTemp == pLast )
				{
					pTemp = pTemp->m_pPrevious;
					pTemp->m_pNext = NULL;

					pLast = pTemp;
					pCurrent = pTemp;
				}
				else
				{
					ForceMember *pTemp2 = pTemp->m_pNext;

					pTemp2->m_pPrevious = pTemp->m_pPrevious;
					pTemp->m_pPrevious->m_pNext = pTemp->m_pNext;

					pTemp = pTemp->m_pNext;
				}

				m_iElements--;
			}
		}

		for ( pTemp = pFirst; pTemp; pTemp = pTemp->m_pNext )
			CMiniMem::ApplyForce( pTemp->m_vOrigin, pTemp->m_vDirection, pTemp->m_flRadius, pTemp->m_flStrength );
	}
};

#endif // __PMAN_FORCE_H__
