
#include "stdafx.h"
#include "CsGBObjProp_AvCap.h"

CSGBMEMPOOL_CPP( CsGBObjProp_AvCap );

CsGBObjProp_AvCap::CsGBObjProp_AvCap()
{
	m_pNiAvObject = NULL;
}

CsGBObjProp_AvCap::~CsGBObjProp_AvCap()
{
	m_pNiAvObject = NULL;
}


CsGBObjProp* CsGBObjProp_AvCap::GetInstance( CsGBObject* pParentObj )
{
	CsGBObjProp_AvCap* pProp = CsGBObjProp_AvCap::NewInstance();
	pProp->Init( pParentObj );
	return pProp;
}

CsGBObjProp* CsGBObjProp_AvCap::GetInstance( NiNode* pParentNiNode )
{
	CsGBObjProp_AvCap* pProp = CsGBObjProp_AvCap::NewInstance();
	pProp->Init( pParentNiNode );
	return pProp;
}

void CsGBObjProp_AvCap::Delete()
{
	CsGBObjProp::Delete();

	m_pNiAvObject = NULL;
}

void CsGBObjProp_AvCap::Init( CsGBObject* pParentObj )
{
	CsGBObjProp::Init( pParentObj );

	assert_cs( m_pNiAvObject == NULL );

	m_pNiAvObject = nsCSGBFUNC::GetFirstAppCulledObject( m_pParentNiNode, "cap", 3 );
	assert_csm1( m_pNiAvObject != NULL, "하이드된 cap 이름의 오브젝트가 존재 하지 않습니다.\n\n%s", pParentObj->GetInfo()->s_cObjectPath );	
	m_pNiAvObject->Update( 0.0f );
}

void CsGBObjProp_AvCap::Init( NiNode* pParentNiNode )
{
	CsGBObjProp::Init( pParentNiNode );

	assert_cs( m_pNiAvObject == NULL );

	m_pNiAvObject = nsCSGBFUNC::GetFirstAppCulledObject( m_pParentNiNode, "cap", 3 );
	assert_csm( m_pNiAvObject != NULL, "하이드된 cap 이름의 오브젝트가 존재 하지 않습니다" );
	m_pNiAvObject->Update( 0.0f );
}

void CsGBObjProp_AvCap::Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend )
{
	pCsPick->SetObserveAppCull( false );
	assert_cs( m_pNiAvObject != NULL );
	pCsPick->Pick( m_pNiAvObject, origin, dir, bAppend );
	pCsPick->SetObserveAppCull( true );
}
