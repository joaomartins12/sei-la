
#include "stdafx.h"
#include "NpcMng.h"

CNpcMng*	g_pNpcMng = NULL;

CNpcMng::CNpcMng():m_pDigiviceNpc(NULL),m_pZoomNpc(NULL),m_pZoomNpcTemp(NULL)
,m_fZoomNpcAlpha(0.0f),m_bIncreaseZoomAlpha(false),m_nZoomNpcType(cBaseWindow::WT_NONE)
,m_bRenderPos(false),m_bRenderAni(false)
{
}

CNpcMng::~CNpcMng()
{

}

CsC_AvObject* CNpcMng::CALLBACK_GetNpcObject( DWORD dwId )
{
	SAFE_POINTER_RETVAL( g_pNpcMng, NULL );

	return g_pNpcMng->GetNpc( dwId );	
}

void CNpcMng::Delete()
{
	SAFE_POINTER_RET( g_pCharResMng );

	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		g_pCharResMng->DeleteChar( it->second );
		g_pCharResMng->ThreadDeleteChar( it->second );
	}
	m_mapNpc.clear();

	_DeleteZoomDigivice();
	_DeleteZoomNpcTemp();
}

void CNpcMng::Init()
{
	Delete();

	m_dwOldMapID = (DWORD)-1;
	m_pZoomNpc = NULL;
	m_pZoomNpcTemp = NULL;
	m_nZoomNpcType = cBaseWindow::WT_NONE;
	m_pDigiviceNpc = NULL;

	m_bRenderPos = false;
	m_bRenderAni = false;

#ifndef _GIVE
	m_bRenderNpc = true;
#endif

	if( g_pEventSceneMng )
		g_pEventSceneMng->ResistCallBack_EventObject( CALLBACK_GetNpcObject );
}

void CNpcMng::Reset()
{	
	m_pZoomNpc = NULL;
	_DeleteZoomDigivice();
	_DeleteZoomNpcTemp();	
	m_nZoomNpcType = cBaseWindow::WT_NONE;
}

void CNpcMng::ResetShader()
{
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		CsC_AvObject* pAvObject = it->second;
		SAFE_POINTER_CON( pAvObject );

		if( !pAvObject->IsLoad() )
			continue;

		CsNodeObj* pNode = pAvObject->GetCsNode();
		SAFE_POINTER_CON( pNode );

		pNode->ActiveShader();
	}
}

void CNpcMng::DeleteMapNpc( DWORD const& dwMapID )
{
	if( m_dwOldMapID == dwMapID )
	{
		NpcCont::iterator it = m_mapNpc.begin();
		for( ; it != m_mapNpc.end(); ++it )
		{
			SAFE_POINTER_CON(it->second);
			it->second->SetQuestOwner( NULL );
		}		
	}
	else
	{
		NpcCont::iterator it = m_mapNpc.begin();
		for( ; it != m_mapNpc.end(); ++it )
		{
			SAFE_POINTER_CON(it->second);
			g_pCharResMng->DeleteChar( it->second );
			g_pCharResMng->ThreadDeleteChar( it->second );
		}
		m_mapNpc.clear();
	}
}

bool CNpcMng::LoadMap( DWORD dwMapID )
{
	// 맵 엔피시 배치전에 시간 셋팅한번 해주자 - 차후 시간은 Update.cpp에서 업데이트 된다.
	if( net::game )
		nsCsGBTerrain::g_ServerTime.s_nServerTime = net::game->GetTimeTS();

	if( m_dwOldMapID == dwMapID )
		return true;

	m_dwOldMapID = dwMapID;

	// 엔피씨 생성 - 일단은 걍 다~!!
	MAP_MAPNPC* pmapNpc = nsCsMapTable::g_pMapNpcMng->GetGroup( dwMapID )->GetNpcMap();
	MAP_MAPNPC_IT itmapNpc = pmapNpc->begin();
	MAP_MAPNPC_IT itEndmapNpc = pmapNpc->end();
	for( ;itmapNpc!=itEndmapNpc; ++itmapNpc )
	{
		assert_csm1( nsCsFileTable::g_pNpcMng->IsNpc( itmapNpc->second->GetInfo()->s_dwNpcID ), _T( "Map.xls에는 존재 하지만 Npc.xls 에 %d Npc가 존재하지 않습니다." ), itmapNpc->second->GetInfo()->s_dwNpcID );
		assert_cs( itmapNpc->second->GetInfo()->s_dwNpcID > NPC_FT_MASK );
		AddNpc( itmapNpc->second->GetInfo()->s_dwNpcID%NPC_FT_MASK, itmapNpc->second->GetInfo()->s_dwNpcID );
	}

	return true;
} 
 

void CNpcMng::SetNpcEnableFromIdx( UINT const& nNpcIdx, bool const& bVale )
{
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		UINT iIDX = it->second->GetFTID();
		if( iIDX != nNpcIdx )
			continue;

		it->second->SetEnableUse(bVale);
		break;
	}	
}

void CNpcMng::NPCEnable(bool const& bVal)
{
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->SetEnableUse(bVal);
	}
}

void CNpcMng::CheckQuestExtra()
{ 
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->CheckQuestExtra();
	}
}

bool CNpcMng::_CheckAwardNpc( DWORD const& dwFTID, CsNpc::sINFO* pFTable, std::wstring & wsGuildName )
{
	SAFE_POINTER_RETVAL( pFTable, false );

	int nServerIDX = GLOBALDATA_ST.GetCurrentServerIDX();
	TB::sInfiniteAwardNpcInfo const * pAwardNpcInfo = nsCsFileTable::g_pInfiniteWarTBMng->GetAwardNpcInfo(nServerIDX, dwFTID);
	SAFE_POINTER_RETVAL( pAwardNpcInfo, false  );

	if( 0 == pAwardNpcInfo->s_dwModelID )
		return false;

	pFTable->s_dwModelID = pAwardNpcInfo->s_dwModelID;
	_tcscpy_s( pFTable->s_szName, sizeof(pFTable->s_szTalk), pAwardNpcInfo->s_wTamerName.c_str() );
	_tcscpy_s( pFTable->s_szPenName, sizeof(pFTable->s_szPenName), pAwardNpcInfo->s_wTitleName.c_str() );
	_tcscpy_s( pFTable->s_szTalk, sizeof(pFTable->s_szTalk), pAwardNpcInfo->s_wTalkMsg.c_str() );

	wsGuildName = pAwardNpcInfo->s_wGuildName;
	return true;	
}

void CNpcMng::AddNpc( uint nIDX, uint nFileTableID )
{
	SAFE_POINTER_RET( nsCsFileTable::g_pNpcMng );
	CsNpc* pNpcTB = nsCsFileTable::g_pNpcMng->GetNpc( nFileTableID );
	SAFE_POINTER_RET( pNpcTB );
	CsNpc::sINFO* pNpcTbInfo = pNpcTB->GetInfo();
	SAFE_POINTER_RET( pNpcTbInfo );

	std::wstring wsAwardGuildName;
	bool bIsAwarNpc = _CheckAwardNpc( nFileTableID, pNpcTbInfo, wsAwardGuildName );	

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nFileTableID );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	pUint->s_Prop.s_dwOptionPlag = OP_STAT | OP_LIGHT | OP_FOG | OP_ABVWIRE;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	// 무브 컨트롤러 가지고 있다면 프로퍼티 셋팅
	bool bMoveCon = ( pNpcTbInfo->s_eMoveType == nsCsNpcTable::MT_MOVE );
	if( bMoveCon )
	{
		assert_cs( nsCsFileTable::g_pMoveObjectMng->IsObject( CsMoveObject::MO_NPC, nFileTableID ) );
		pUint->s_Prop.s_dwPropPlag |= PR_MOVECON_ENABLE;
	}

	CNpc* pNpc = CNpc::NewInstance();	
	pNpc->Init( nIDX, nFileTableID, &pUint->s_Prop );

	if( bIsAwarNpc && !wsAwardGuildName.empty() )
	{
		CCharOption* pCharOption = pNpc->GetCharOption();
		if( pCharOption )
			pCharOption->SetGuildName( wsAwardGuildName.c_str() );

	}

	m_mapNpc.insert( std::make_pair( nIDX, pNpc ) );

	if( bMoveCon )
	{
		// 테이블 연결
		CsMoveObject* pFTMO = nsCsFileTable::g_pMoveObjectMng->GetObject( CsMoveObject::MO_NPC, nFileTableID );
		pNpc->GetProp_MoveControl()->SetFTMoveObject( pFTMO );
	}

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pNpc;


	DWORD dwModelID = pNpcTbInfo->s_dwModelID;	
	std::string strPath = g_pModelDataMng->GetKfmPath( dwModelID );
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );
}

void CNpcMng::DelNpc( uint const& nIDX)
{
	if( !IsNpc(nIDX) )
		return;

	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		UINT iIDX = it->second->GetFTID();
		if( iIDX != nIDX )
			continue;

		g_pCharResMng->DeleteChar( it->second );
		g_pCharResMng->ThreadDeleteChar( it->second );
		m_mapNpc.erase(it);
		break;
	}	
}

void CNpcMng::Update()
{
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Update( g_fDeltaTime );
	}
}

void CNpcMng::Render()
{
#ifndef _GIVE
	if( m_bRenderNpc == false )
		return;
#endif

	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render();
	}
}

bool CNpcMng::IsNpc( uint const& nIDX ) const
{ 
	NpcCont::const_iterator it = m_mapNpc.find( nIDX%NPC_FT_MASK );
	if( it == m_mapNpc.end() )
		return false;
	return true;
}

void CNpcMng::Render_PostChar()
{
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		it->second->Render_PostChar();
	}	
}

CNpc* CNpcMng::GetNpc( uint const& nIDX )
{
	NpcCont::iterator it = m_mapNpc.find( nIDX%NPC_FT_MASK );
	if( it == m_mapNpc.end() )
		return NULL;

	return it->second;
}

void CNpcMng::SetNpcSE_Highlight( DWORD const& dwTableID, bool const& bShow )
{
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( dwTableID != it->second->GetFTID() )
			continue;

		if( bShow )
			it->second->SetShaderEffect_Highlight();
		else
			it->second->RemoveShaderEffect_Highlight();
	}
}
//
//void CNpcMng::Render_Post_TR()
//{
//	CsVectorLink< CItem* >::iterator* it = m_vnItem.Begin();
//	CsVectorLink< CItem* >::iterator* itEnd = m_vnItem.End();
//	for( ; it!=itEnd; it = it->s_pNext )
//	{
//		//it->s_Element->Render_Post_TR();
//	}
//}
//
//void CNpcMng::Render_End()
//{
//	CsVectorLink< CItem* >::iterator* it = m_vnItem.Begin();
//	CsVectorLink< CItem* >::iterator* itEnd = m_vnItem.End();
//	for( ; it!=itEnd; it = it->s_pNext )
//	{
//		//it->s_Element->Render_End();
//	}
//}
//
//void CNpcMng::SetCharRender()
//{
//	CsVectorLink< CItem* >::iterator* it = m_vnItem.Begin();
//	CsVectorLink< CItem* >::iterator* itEnd = m_vnItem.End();
//	for( ; it!=itEnd; it = it->s_pNext )
//	{
//		//it->s_Element->SetCharRender();
//	}
//}


//====================================================================================
//
//		Zoom Npc
//
//====================================================================================

void CNpcMng::SetZoomNpc( CsC_AvObject* pNpc )
{
	SAFE_POINTER_RET( pNpc );
	if( m_pZoomNpc == pNpc )
		return;
	
	switch( pNpc->GetLeafRTTI() )
	{
	case RTTI_NPC:
		{
			CNpc* pTarget = static_cast< CNpc* >( pNpc );
			if( !pTarget->IsDisplayType( CsNpc::DP_ZOOM_VIEW ) )
				return;
		}
		break;
	}

	m_pZoomNpc = pNpc;
	m_fZoomNpcAlpha = 0.0f;
	m_bIncreaseZoomAlpha = true;
}

void CNpcMng::ResetZoomNpc()
{
	m_pZoomNpc = NULL;
	m_fZoomNpcAlpha = 0.0f;
	m_bIncreaseZoomAlpha = false;
	m_nZoomNpcType = cBaseWindow::WT_NONE;
}

CNpc* CNpcMng::LoadZoomNpcTemp( DWORD nModelID )
{
	m_pZoomNpc = NULL;
	m_fZoomNpcAlpha = 0.0f;
	m_bIncreaseZoomAlpha = true;

	if( m_pZoomNpcTemp )
	{
		if( m_pZoomNpcTemp->GetUniqID() == nModelID )
			return m_pZoomNpcTemp;
	}	

	_DeleteZoomNpcTemp();

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, 101 );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	pUint->s_Prop.s_dwOptionPlag = OP_LIGHT | OP_FOG;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	m_pZoomNpcTemp = CNpc::NewInstance();
	m_pZoomNpcTemp->Init_NoFT( nModelID, &pUint->s_Prop, CNpc::NRT_TEMP );
	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)m_pZoomNpcTemp;
	std::string strPath = g_pModelDataMng->GetKfmPath( nModelID );
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );
	// 쓰레드 로드
	g_pCharResMng->LoadChar( pUint );

	return m_pZoomNpcTemp;
}

void CNpcMng::EndZoomNpc()
{
	m_bIncreaseZoomAlpha = false;
	m_nZoomNpcType = cBaseWindow::WT_NONE;
}

void CNpcMng::RenderZoomNpc()
{
	if( m_pZoomNpc == NULL )
		return;	

	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_NONE )
	{		
		return;
	}

	if( m_nZoomNpcType == cBaseWindow::WT_BARCODE_CARD_SCRATCH )
	{
		if( g_pCharMng->GetTamerUser()->GetProp_Animation()->IsBarcodeCardAni() == false )
		{
			EndZoomNpc();			
		}
	}
	else if( ( g_pGameIF->IsSpeacialAct() == false ) && ( g_pGameIF->IsOpenNpcWindow() == false ) )
	{
		EndZoomNpc();
	}	

	if( m_nZoomNpcType == cBaseWindow::WT_MAKE_TACTICS )
		return;		

	if( m_pZoomNpc->GetLeafRTTI() == RTTI_NPC )
	{
		CNpc* pNpc = (CNpc*)m_pZoomNpc;
		if( pNpc->GetNpcRenderType() != CNpc::NRT_NORMAL )
		{
			m_pZoomNpc->CsC_AvObject::Update( g_fDeltaTime );
		}
	}

	if( m_pZoomNpc->IsLoad() == false )
		return;

	// 오브젝트 위치 설정	
	CsNodeObj* pCsNode = m_pZoomNpc->GetCsNode();
	if( pCsNode == NULL )
		return;

	if( m_bIncreaseZoomAlpha == true )
	{
		if( (m_fZoomNpcAlpha += g_fDeltaTime/0.22f) > 1.0f )
			m_fZoomNpcAlpha = 1.0f;	
	}
	else
	{
		if( (m_fZoomNpcAlpha -= g_fDeltaTime/0.22f) < 0.0f )
		{			
			m_pZoomNpc = NULL;
			m_nZoomNpcType = cBaseWindow::WT_NONE;
			return;	
		}
	}

	// ZBuffer만 클리어
	g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();	
	g_pEngine->m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ZBUFFER );

	// 오브젝트 알파 설정
	bool bBackupUseDistAlpha = m_pZoomNpc->GetProp_Alpha()->IsUseDistAlpha();
	if( m_fZoomNpcAlpha < 1.0f )
	{
		m_pZoomNpc->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, m_fZoomNpcAlpha );
		m_pZoomNpc->GetProp_Alpha()->SetUseDistAlpha( false );
		m_pZoomNpc->GetProp_Alpha()->Update( 0.0f );
	}

	float fHeight = m_pZoomNpc->GetToolHeight();
	float fWidth = m_pZoomNpc->GetToolWidth();
	float fCameraDist;
	float fCameraHeight;

	if( fHeight < 60 )						// 고양이
	{
		fCameraDist = 130.0f;
		fCameraHeight = 30.0f;
	}
	else if( fHeight < 100.0f )				// 마나미
	{
		fCameraDist = 130.0f;
		fCameraHeight = 80.0f;
	}
	else if( fHeight < 125.0f )				// 치카, 타카시
	{
		fCameraDist = 140.0f;
		fCameraHeight = 80.0f;
	}
	else if( fHeight < 140.0f )				// 하시바
	{
		fCameraDist = 140.0f;
		fCameraHeight = 95.0f;
	}
	else if( fHeight < 160.0f )
	{
		fCameraDist = 140.0f;
		fCameraHeight = 95.0f;
	}	
	else if( fHeight < 180.0f )				// 이노우에
	{
		fCameraDist	= 140.0f;
		fCameraHeight = 120.0f;
	}
	else if( fHeight < 200 )
	{
		fCameraDist	= 160.0f;
		fCameraHeight = 130.0f;
	}
	else if( fHeight < 210 )
	{
		fCameraDist	= 220.0f;
		fCameraHeight = 130.0f;
	}
	else if( fHeight < 250 )				// 머드프리지몬
	{
		fCameraDist	= 250.0f;
		fCameraHeight = 140.0f;
	}
	else if( fHeight < 305 )				// 전화기
	{
		fCameraDist	= 350.0f;
		fCameraHeight = 170.0f;
	}
	else
	{
		fCameraDist	= 370.0f;
		fCameraHeight = 210.0f;
	}
	
	float fMoveX = tan( 0.27f )*fCameraDist + fWidth*0.55f;

	// 해상도 배율에 따라서 fMoveX에 비율을 곱해준다
	// 기준은 1680/1050
	fMoveX *= ( (g_nScreenWidth/(float)g_nScreenHeight)*(1050/1680.0f) );


	if( m_pZoomNpc->GetViewSight() == false )	
	{
		// 시야에 보이지 않으면 애니메이션 업데이트 풀로 다해준다.
		float fAniTime = m_pZoomNpc->GetAniTime();
		m_pZoomNpc->GetProp_Animation()->GetActorMng()->Update( fAniTime );
		pCsNode->m_pNiNode->Update( fAniTime );
	}

	// 카메라 설정
	if( m_pZoomNpc->GetLeafRTTI() == RTTI_NPC )
	{
		CsNpcMng::sMODEL_INFO* pModelInfo =	nsCsFileTable::g_pNpcMng->GetModelInfo( m_pZoomNpc->GetModelID() ); 

		GET_SUBCAMERA(CAMERA_10)->SetRotation( m_pZoomNpc->GetCurRot() - 0.7f, -0.05f );	
		GET_SUBCAMERA(CAMERA_10)->_UpdateCamera();
		GET_SUBCAMERA(CAMERA_10)->SetDist( fCameraDist - pModelInfo->s_nOffset[ 1 ] );

		NiPoint3 vCameraPos = m_pZoomNpc->GetPos() + NiPoint3( 0, 0, fCameraHeight + 10.0f - pModelInfo->s_nOffset[ 2 ] ) - GET_SUBCAMERA(CAMERA_10)->GetWorldRightVector()*( fMoveX + pModelInfo->s_nOffset[ 0 ] );	

		// ZoomNPC 카메라 위치 변경
		if( m_bRenderPos == true )
		{
			if( nsCsFileTable::g_pNpcMng->GetCardGameInfo( ( (CNpc*)m_pZoomNpc )->GetFTNpc()->GetInfo()->s_dwNpcID ) )
			{
 				GET_SUBCAMERA(CAMERA_10)->SetRotation( m_pZoomNpc->GetCurRot(), -0.05f );
 				GET_SUBCAMERA(CAMERA_10)->SetDist( 500.0f );

				if( m_bRenderAni == false )
				{
					m_pZoomNpc->GetProp_Animation()->SetAnimation( ANI::NPC_NO );
					m_bRenderAni = true;
				}
				
				vCameraPos.x = m_pZoomNpc->GetPos().x;
				vCameraPos.y = m_pZoomNpc->GetPos().y;
				vCameraPos.z -= 25;
			}
		}

		//if(g_pGameIF->GetGotchaMachine() != NULL)
		if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_GOTCHA_MACHINE ) )
 		{
			GET_SUBCAMERA(CAMERA_10)->SetRotation( m_pZoomNpc->GetCurRot(), -0.05f );
			GET_SUBCAMERA(CAMERA_10)->SetDist( 300.0f );
 
 			if( m_bRenderAni == false )
 			{
				CsC_AniProp* pAniProp = m_pZoomNpc->GetProp_Animation();
				if( pAniProp )
				{
					pAniProp->SetAnimation( ANI::IDLE_NORMAL );
					m_bRenderAni = true;
				}
 			}
 
 			vCameraPos.x = m_pZoomNpc->GetPos().x;
 			vCameraPos.y = m_pZoomNpc->GetPos().y;
 			vCameraPos.z -= 25;
 
 		}
		GET_SUBCAMERA(CAMERA_10)->SetTranslate( vCameraPos );
		GET_SUBCAMERA(CAMERA_10)->_UpdateCamera();
	}
	else
	{
		GET_SUBCAMERA(CAMERA_10)->SetRotation( m_pZoomNpc->GetCurRot() - 0.7f, -0.05f );	
		GET_SUBCAMERA(CAMERA_10)->_UpdateCamera();
		GET_SUBCAMERA(CAMERA_10)->SetDist( fCameraDist );


		NiPoint3 vCameraPos = m_pZoomNpc->GetPos() + NiPoint3( 0, 0, fCameraHeight + 10.0f ) - GET_SUBCAMERA(CAMERA_10)->GetWorldRightVector()*fMoveX;	
		GET_SUBCAMERA(CAMERA_10)->SetTranslate( vCameraPos );
		GET_SUBCAMERA(CAMERA_10)->_UpdateCamera();

		if( m_bRenderPos == true )
		{
			if( m_pZoomNpc->GetModelID() == 95003 )
			{
				GET_SUBCAMERA(CAMERA_10)->SetRotation( m_pZoomNpc->GetCurRot(), -0.05f );
				GET_SUBCAMERA(CAMERA_10)->SetDist( 1500.0f );
				if( m_bRenderAni == false )
				{
					m_pZoomNpc->GetProp_Animation()->SetAnimation( ANI::NPC_NO );
					//m_pZoomNpc->GetProp_Animation()->ResetAnimation();
					m_bRenderAni = true;
				}
				//m_pZoomNpc->GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
				NiPoint3 vCameraPos = m_pZoomNpc->GetPos() + NiPoint3( 0, 0, fCameraHeight + 10.0f ) - GET_SUBCAMERA(CAMERA_10)->GetWorldRightVector();	

				vCameraPos.x = m_pZoomNpc->GetPos().x;
				vCameraPos.y = m_pZoomNpc->GetPos().y-500;
				vCameraPos.z -= 250;
				// 500 1024
				// 850 1280
				// 350 256
			
				GET_SUBCAMERA(CAMERA_10)->SetTranslate( vCameraPos );
				GET_SUBCAMERA(CAMERA_10)->_UpdateCamera();	
				
			}
		}
	}	

	g_pEngine->m_spRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_10)->GetCameraObj() );

	// 쉐이더 - 케쉬 사용 안시키기 위해
	NiSkinInstance* pSkin;
	int nCount = pCsNode->m_vpGeom.Size();	
	for( int i=0; i<nCount; ++i )
	{
		if( pCsNode->m_vpGeom[ i ]->IsPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE ) )
			continue;

		pSkin = pCsNode->m_vpGeom[ i ]->m_pGeometry->GetSkinInstance();
		if( pSkin == NULL )
			continue;
		pSkin->SetBoneMatrixInfo( 0, 0 );
	}

	// 케릭터 랜더링
	pCsNode->Render_GeomData_ExceptPlag( 0 );		//2016-03-07-nova

	if( m_pZoomNpc->IsProperty( CsC_Property::EFFECT ) )
		m_pZoomNpc->GetProp_Effect()->RenderAbsolute();

	// 카메라 복귀

	if( m_fZoomNpcAlpha < 1.0f )
		m_pZoomNpc->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );		

	m_pZoomNpc->GetProp_Alpha()->SetUseDistAlpha( bBackupUseDistAlpha );

	g_pEngine->m_spRenderer->SetScreenSpaceCameraData();	
}

void CNpcMng::_DeleteZoomNpcTemp()
{
	if( m_pZoomNpcTemp == NULL )
		return;

	g_pCharResMng->ThreadDeleteChar( m_pZoomNpcTemp );
	m_pZoomNpcTemp = NULL;
}

//====================================================================================
//
//		GetDigivice
//
//====================================================================================
CNpc* CNpcMng::GetDigivice()
{
	// 디지바이스
	if( m_pDigiviceNpc == NULL )
	{
		DWORD dwNpcID = 94000+g_pDataMng->GetPostLoad()->GetTamerData()->s_Type.m_nType%80000;
		DWORD dwModelID = nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_dwModelID;	

		// 로드시 필요한 초기화
		sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, 102 );
		pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE;
		pUint->s_Prop.s_dwOptionPlag = OP_STAT | OP_LIGHT | OP_FOG | OP_ABVWIRE;
		pUint->s_Prop.s_eInsertGeomType = CGeometry::Normal;

		m_pDigiviceNpc = CNpc::NewInstance();
		m_pDigiviceNpc->Init_NoMapFT( dwModelID, dwNpcID, &pUint->s_Prop, CNpc::NRT_DIGIVICE );
		m_pDigiviceNpc->GetProp_Alpha()->SetUseDistAlpha( false );

		// 로드 설정
		pUint->s_pLoadedObject = (CsC_Object*)m_pDigiviceNpc;	
		std::string strPath = g_pModelDataMng->GetKfmPath( dwModelID );	
		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );

		// 쓰레드 로드
		g_pCharResMng->LoadChar( pUint );
	}

	return m_pDigiviceNpc;
}

void CNpcMng::_DeleteZoomDigivice()
{
	if( m_pDigiviceNpc == NULL )
		return;

	g_pCharResMng->DeleteChar( m_pDigiviceNpc );
	g_pCharResMng->ThreadDeleteChar( m_pDigiviceNpc );
	m_pDigiviceNpc = NULL;
}

//====================================================================================
//
//		Picking
//
//====================================================================================
CsC_AvObject* CNpcMng::PickNpc( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength )
{
	CsC_AvObject* pReturn = NULL;
	NpcCont::iterator it = m_mapNpc.begin();
	for( ; it != m_mapNpc.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( !it->second->PickObject( pCsGBPick, vOrigin, vDir, fLength ) )
			continue;

		type.m_nIDX = it->second->GetIDX();
		type.m_nClass = nClass::Npc;
		pReturn = it->second;
	}
	return pReturn;
}