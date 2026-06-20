#include "StdAfx.h"
#include "DigimonEvolveInfo.h"

CDigimonEvolveInfo::CDigimonEvolveInfo(void)
{	
}

void CDigimonEvolveInfo::Destroy()
{
	CDigimonEvolveObj::MAP_IT it = m_Map.begin();
	CDigimonEvolveObj::MAP_IT itEnd = m_Map.end();
	for ( ; it!=itEnd ; ++it )
	{
		delete it->second;
	}
	m_Map.clear();

}

void CDigimonEvolveInfo::Add( CDigimonEvolveObj* pObj )
{
	assert_cs( IsEvolveObjByID( pObj->m_dwID ) == false );
	m_Map[pObj->m_dwID] = pObj;
}

// =========================================================
// 진화 번호로 진화객체 찾기
// =========================================================
CDigimonEvolveObj* CDigimonEvolveInfo::GetEvolveObjByEvoSlot(int nEvoSlot)
{
	CDigimonEvolveObj::MAP_IT it = m_Map.begin();
	CDigimonEvolveObj::MAP_IT itEnd = m_Map.end();
	for ( ; it!=itEnd ; ++it )
	{
		if(it->second->m_nEvoSlot == nEvoSlot)
			return it->second;
	}
	return NULL;
}

// =========================================================
// 디지몬 인덱스로 진화객체 찾기
// =========================================================
CDigimonEvolveObj* CDigimonEvolveInfo::GetEvolveObjByID( DWORD const & nDwID )
{
	CDigimonEvolveObj::MAP_IT it = m_Map.find( nDwID );
	if( it == m_Map.end() )
		return NULL;
	return it->second;
}

bool CDigimonEvolveInfo::IsEvolveObjByID( DWORD const & nDwID ) const
{ 
	return ( m_Map.find( nDwID ) != m_Map.end() ); 
}