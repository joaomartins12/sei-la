
#include "stdafx.h"
#include "AttachProperty.h"

//THREAD_MEMPOOL_CPP( CsC_AttachProp )

//#define PARENT_CALSS		CsC_Property


//CsC_AttachProp::CsC_AttachProp()
//{
//	m_eType = CsC_Property::ATTACH;	
//}
//
//void CsC_AttachProp::Delete()
//{
//	PARENT_CALSS::Delete();
//
//	for( int i=0; i<ATT_MAX_TYPE; ++i )
//	{
//		m_AttachArray[ i ].Delete();
//		m_DeleteReadyRefCnt[ i ] = 0;
//	}
//}
//
//void CsC_AttachProp::PreDelete()
//{
//	PARENT_CALSS::PreDelete();	
//}
//
//bool CsC_AttachProp::IsDelete()
//{
//	for( int i=0; i<ATT_MAX_TYPE; ++i )
//	{
//		if( m_DeleteReadyRefCnt[ i ] > 0 )
//			return false;
//	}
//	return true;
//}
//
//void CsC_AttachProp::Init( CsC_AvObject* pParent )
//{
//	PARENT_CALSS::Init( pParent );
//
//	for( int i=0; i<ATT_MAX_TYPE; ++i )
//	{
//		m_DeleteReadyRefCnt[ i ] = 0;
//	}
//}
//
//void CsC_AttachProp::AddAttach( void* pData )
//{
//	CsC_AvObject::sTHREAD_READY_ATTACH* pThread = (CsC_AvObject::sTHREAD_READY_ATTACH*)pData;
//
//	assert_cs( m_pParent->IsLoad() == true );
//	assert_cs( m_pParent->GetCsNode() != NULL );
//	CsNodeObj* pCsNodeObj = m_pParent->GetCsNode();
//
//	// 이미 기존게 있다면
//	if( m_AttachArray[ pThread->s_nAttachType ].s_pNiNode != NULL )
//	{
//		// 기존거 제거
//		pCsNodeObj->m_pNiNode->DetachChild( m_AttachArray[ pThread->s_nAttachType ].s_pNiNode ) = 0;
//		m_AttachArray[ pThread->s_nAttachType ].Delete();
//	}
//
//	m_AttachArray[ pThread->s_nAttachType ].s_pNiNode = pThread->s_pNiNode;
//	m_AttachArray[ pThread->s_nAttachType ].s_pRefLoad = pThread->s_pRefLoad;
//	m_AttachArray[ pThread->s_nAttachType ].s_pValue1 = pThread->s_pFTValue;
//
//
//	if( m_DeleteReadyRefCnt[ pThread->s_nAttachType ] > 0 )
//	{
//		--m_DeleteReadyRefCnt[ pThread->s_nAttachType ];		
//		m_AttachArray[ pThread->s_nAttachType ].Delete();
//	}
//	else
//	{
//		m_AttachArray[ pThread->s_nAttachType ].s_pNiNode->UpdateNodeBound();
//	}
//}
//
//void CsC_AttachProp::DelAttach( eATTACH_TYPE at )
//{
//	// 아직 로드 되지 않았다.
//	// 또는 부모가 로드 중이다
//	if( ( m_pParent->IsLoad() == false ) || ( m_AttachArray[ at ].s_pNiNode == NULL ) )
//	{
//		++m_DeleteReadyRefCnt[ at ];
//		return;
//	}
//
//	m_AttachArray[ at ].Delete();
//}
//
//
//
//
//void CsC_AttachProp::GlobalEffectProp( sCharProp* pProp, CsC_AttachProp::eATTACH_TYPE at )
//{
//	pProp->s_bABVWire = false;
//	pProp->s_bCharImg = false;
//	pProp->s_bEffectProp = false;
//	pProp->s_bEnableAttack = false;
//	pProp->s_bFog = false;
//	pProp->s_bLight = false;
//	pProp->s_bShadow = false;
//	pProp->s_eAniType = ANI_NORMAL_LOOP;
//	pProp->s_eInsertGeomType = CGeometry::Normal;
//	pProp->s_ePathEngine = PATH_NONE;
//}

//#undef PARENT_CLASS
