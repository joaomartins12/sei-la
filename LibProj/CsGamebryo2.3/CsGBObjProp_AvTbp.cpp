
#include "stdafx.h"
#include "CsGBObjProp_AvTbp.h"

CSGBMEMPOOL_CPP( CsGBObjProp_AvTbp );

CsGBObjProp_AvTbp::CsGBObjProp_AvTbp()
{
	m_pNiAvObject = NULL;
}

CsGBObjProp_AvTbp::~CsGBObjProp_AvTbp()
{
	m_pNiAvObject = NULL;
}

CsGBObjProp* CsGBObjProp_AvTbp::GetInstance( CsGBObject* pParentObj )
{
	CsGBObjProp_AvTbp* pProp = CsGBObjProp_AvTbp::NewInstance();
	pProp->Init( pParentObj );
	return pProp;
}

CsGBObjProp* CsGBObjProp_AvTbp::GetInstance( NiNode* pParentNiNode )
{
	CsGBObjProp_AvTbp* pProp = CsGBObjProp_AvTbp::NewInstance();
	pProp->Init( pParentNiNode );
	return pProp;
}


void CsGBObjProp_AvTbp::Delete()
{
	CsGBObjProp::Delete();

	m_pNiAvObject = NULL;
}

void CsGBObjProp_AvTbp::Init( CsGBObject* pParentObj )
{
	CsGBObjProp::Init( pParentObj );

	assert_cs( m_pNiAvObject == NULL );

	m_pNiAvObject = nsCSGBFUNC::GetFirstAppCulledObject( m_pParentNiNode, "tbp", 3 );
	assert_csm1( m_pNiAvObject != NULL, "하이드된 tbp 이름의 오브젝트가 존재 하지 않습니다.\n\n%s", pParentObj->GetInfo()->s_cObjectPath );	
	m_pNiAvObject->Update( 0.0f );
}

void CsGBObjProp_AvTbp::Init( NiNode* pParentNiNode )
{
	CsGBObjProp::Init( pParentNiNode );

	assert_cs( m_pNiAvObject == NULL );

	m_pNiAvObject = nsCSGBFUNC::GetFirstAppCulledObject( m_pParentNiNode, "tbp", 3 );
	assert_csm( m_pNiAvObject != NULL, "하이드된 tbp 이름의 오브젝트가 존재 하지 않습니다." );	
	m_pNiAvObject->Update( 0.0f );
}

void CsGBObjProp_AvTbp::Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend )
{
	assert_cs( m_pNiAvObject != NULL );
	pCsPick->SetObserveAppCull( false );
	pCsPick->Pick( m_pNiAvObject, origin, dir, bAppend );
	pCsPick->SetObserveAppCull( true );
}
