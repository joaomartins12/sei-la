

#include "stdafx.h"
#include "CsGBPick.h"

NiPick* CsGBPick::m_pNiTriPick = NULL;
NiPick* CsGBPick::m_pNiBoundPick = NULL;

CsGBPick::CsGBPick( bool bObserveAppCullFlag /* = true  */,
					NiPick::SortType eSort /* = NiPick::SORT */,
					bool bFrontOnly /*= true*/,
					bool bReturnColor /*= false*/	)
{
	assert_cs( m_pNiTriPick != NULL );
	assert_cs( m_pNiBoundPick != NULL );
	Clear();

	m_pNiTriPick->SetSortType( eSort );
	m_pNiTriPick->SetObserveAppCullFlag( bObserveAppCullFlag );
	m_pNiTriPick->SetFrontOnly( bFrontOnly );
	m_pNiTriPick->SetReturnColor( bReturnColor );
	m_pNiBoundPick->SetSortType( eSort );
	m_pNiBoundPick->SetObserveAppCullFlag( bObserveAppCullFlag );
}

CsGBPick::~CsGBPick()
{

}

void CsGBPick::Delete()
{
	NiDelete m_pNiTriPick;
	NiDelete m_pNiBoundPick;
}

void CsGBPick::Create( NiPick::PickType ePick, NiPick::CoordinateType eCoordinate, bool bFrontOnly, bool bReturnNormal )
{
	assert_cs( m_pNiTriPick == NULL );
	assert_cs( m_pNiBoundPick == NULL );

	m_pNiTriPick = NiNew NiPick;
	m_pNiTriPick->SetPickType( ePick );
	m_pNiTriPick->SetCoordinateType( eCoordinate );
	m_pNiTriPick->SetFrontOnly( bFrontOnly );
	m_pNiTriPick->SetReturnNormal( bReturnNormal );
	m_pNiTriPick->SetIntersectType( NiPick::TRIANGLE_INTERSECT );	

	m_pNiBoundPick = NiNew NiPick;
	m_pNiBoundPick->SetPickType( ePick );	
	m_pNiBoundPick->SetCoordinateType( eCoordinate );
	m_pNiBoundPick->SetFrontOnly( bFrontOnly );
	m_pNiBoundPick->SetReturnNormal( bReturnNormal );	
	m_pNiBoundPick->SetIntersectType( NiPick::BOUND_INTERSECT );
}

void CsGBPick::SetFrontOnly( bool bFrontOnly )
{ 
	if( m_pNiTriPick )
		m_pNiTriPick->SetFrontOnly( bFrontOnly ); 
}

void CsGBPick::SetObserveAppCull( bool bAppCull )
{ 
	if( m_pNiTriPick )
		m_pNiTriPick->SetObserveAppCullFlag( bAppCull ); 
}

bool CsGBPick::Pick( NiAVObject* pTarget, NiPoint3 origin, NiPoint3 dir, bool bAppend )
{
	assert_cs( pTarget != NULL );
	m_pNiTriPick->SetTarget( pTarget );
	bool bPick = m_pNiTriPick->PickObjects( origin, dir, bAppend );
	return  ( m_pNiTriPick->GetResults().GetSize() != 0 );
}

bool CsGBPick::Pick_IsBound( NiAVObject* pTarget, NiPoint3 origin, NiPoint3 dir )
{
	assert_cs( pTarget != NULL );
	m_pNiBoundPick->SetTarget( pTarget );
	return m_pNiBoundPick->PickObjects( origin, dir, false );
}

void CsGBPick::Clear()
{
	if( m_pNiTriPick )
		m_pNiTriPick->ClearResultsArray();
	if( m_pNiBoundPick )
		m_pNiBoundPick->ClearResultsArray();
}

NiPick::Record* CsGBPick::GetRecord()
{	
	if( m_pNiTriPick->GetResults().GetSize() != 0 )
		return m_pNiTriPick->GetResults().GetAt( 0 );
	return NULL;
}

