
#include "stdafx.h"
#include "CsGBObjProp_ABV.h"

CSGBMEMPOOL_CPP( CsGBObjProp_ABV );

CsGBObjProp_ABV::CsGBObjProp_ABV()
{
	m_pNiBoundingVolume = NULL;
}

CsGBObjProp_ABV::~CsGBObjProp_ABV()
{
	m_pNiBoundingVolume = NULL;
}

CsGBObjProp* CsGBObjProp_ABV::GetInstance( CsGBObject* pParentObj )
{
	CsGBObjProp_ABV* pProp = CsGBObjProp_ABV::NewInstance();
	pProp->Init( pParentObj );
	return pProp;
}

CsGBObjProp* CsGBObjProp_ABV::GetInstance( NiNode* pParentNiNode )
{
	CsGBObjProp_ABV* pProp = CsGBObjProp_ABV::NewInstance();
	pProp->Init( pParentNiNode );
	return pProp;
}


void CsGBObjProp_ABV::Delete()
{
	CsGBObjProp::Delete();
	m_pNiBoundingVolume = NULL;
}

void CsGBObjProp_ABV::Init( CsGBObject* pParentObj )
{
	assert_cs( m_pNiBoundingVolume == NULL );

	CsGBObjProp::Init( pParentObj );

	//if( pParentObj->UDF_IsDynamic_Pos() == false )
	{
		m_pNiBoundingVolume = NiGetCollisionData( pParentObj->GetNiNode() )->GetWorldSpaceABV();
		assert_cs( m_pNiBoundingVolume != NULL );
	}

	// 다이나믹 포스라면 매번 구해준다.
}

void CsGBObjProp_ABV::Init( NiNode* pParentNiNode )
{
	assert_cs( m_pNiBoundingVolume == NULL );

	CsGBObjProp::Init( pParentNiNode );
	
	// 흠.. 일단 이거 예외처리 힘드네... 보류..
	assert_cs( false );
	//if( pParentObj->UDF_IsDynamic_Pos() == false )
	{
		m_pNiBoundingVolume = NiGetCollisionData( pParentNiNode )->GetWorldSpaceABV();
		assert_cs( m_pNiBoundingVolume != NULL );
	}
	// 다이나믹 포스라면 매번 구해준다.
}
