
#include "stdafx.h"
#include "CardMng.h"
// 
// CCardMng*	g_pCardMng = NULL;
// 
// void CCardMng::Delete()
// {
// 	CardCont::iterator it = m_mapCards.begin();
// 	for( ; it != m_mapCards.end(); ++it )
// 	{
// 		SAFE_POINTER_CON( it->second );
// 		g_pCharResMng->DeleteChar( it->second );
// 		g_pCharResMng->ThreadDeleteChar( it->second );
// 	}
// 
// 	m_mapCards.clear();
// }
// 
// void CCardMng::Init()
// {
// 	Delete();
// }
// 
// void CCardMng::Reset()
// {
// 	Delete();
// }
// 
// void CCardMng::ResetShader()
// {
// 	CardCont::iterator it = m_mapCards.begin();
// 	for( ; it != m_mapCards.end(); ++it )
// 	{
// 		CsC_AvObject* pAvObject = it->second;
// 		SAFE_POINTER_CON( pAvObject );
// 		if( !pAvObject->IsLoad() )
// 			continue;
// 
// 		CsNodeObj* pNode = pAvObject->GetCsNode();
// 		SAFE_POINTER_CON( pNode );
// 
// 		pNode->ActiveShader();
// 	}
// }
// 
// void CCardMng::AddCard( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, CsC_CardObject::sCHANGE_PART_INFO* pPartCordArray )
// {
// 	if( IsCard( nIDX ) )
// 		return;
// 
// 	sCREATEINFO prop;
// 	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ANI_LOOP | PR_EFFECT_ENABLE;
// 	prop.s_dwOptionPlag = OP_USERLIGHT;
// 	//prop.s_dwOptionPlag = 0;
// 	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
// 	//prop.s_eInsertGeomType = CGeometry::Normal;
// 
// 	CCard* pCard = CCard::NewInstance();
// 	pCard->Init( nIDX, nFileTableID, vPos, fRot, &prop );
// 
// 	m_mapCards.insert( std::make_pair( nIDX, pCard ) );
// 
// 	// 쓰레드 로드	
// 	pCard->ThreadLoad_Part( pPartCordArray );
// }
// 
// bool CCardMng::IsCard( uint const& nIDX ) const
// { 
// 	CardCont::const_iterator it = m_mapCards.find(nIDX);
// 	if( it == m_mapCards.end() )
// 		return false;
// 
// 	return true;
// }
// 
// void CCardMng::DeleteCard( uint const& nIDX )
// {
// 	CardCont::iterator it = m_mapCards.find(nIDX);
// 	if( it == m_mapCards.end() )
// 		return;
// 
// 	SAFE_POINTER_RET( it->second );
// 	g_pCharResMng->DeleteChar( it->second );
// 	g_pCharResMng->ThreadDeleteChar( it->second );
// 	m_mapCards.erase(it);
// }
// 
// CCard* CCardMng::GetCard( uint const& nIDX )
// {
// 	CardCont::iterator it = m_mapCards.find(nIDX);
// 	if( it == m_mapCards.end() )
// 		return NULL;
// 	return it->second;
// }
// 
// void CCardMng::Update()
// {
// 	CardCont::iterator it = m_mapCards.begin();
// 	for( ; it != m_mapCards.end(); ++it )
// 	{
// 		SAFE_POINTER_CON( it->second );
// 		it->second->Update( g_fDeltaTime );
// 	}
// }
// 
// void CCardMng::Render()
// {
// 	CardCont::iterator it = m_mapCards.begin();
// 	for( ; it != m_mapCards.end(); ++it )
// 	{
// 		SAFE_POINTER_CON( it->second );
// 		it->second->Render();
// 	}
// }
// 
// 
// 
// 
// 
// //====================================================================================
// //
// //		Picking
// //
// //====================================================================================
// CsC_AvObject* CCardMng::PickCard( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength )
// {
// 	
// 	CsC_AvObject* pReturn = NULL;
// /*
// 	CsVectorLink< CCard* >::iterator* it = m_vnCard.Begin();
// 	CsVectorLink< CCard* >::iterator* itEnd = m_vnCard.End();
// 	for( ; it!=itEnd; it = it->s_pNext )
// 	{
// 		if( it->s_Element->PickObject( pCsGBPick, vOrigin, vDir, fLength ) == true )
// 		{
// 			type.m_nIDX = it->s_Element->GetIDX();
// 			type.m_nClass = nClass::Item;
// 			pReturn = it->s_Element;
// 		}
// 	}
// 	*/
// 	return pReturn;
// }
