
#include "stdafx.h"
// 
// void CMngCollector::Update_Tutorial( cGlobalInput::eMOUSE_INPUT mi )
// {
// #ifndef VERSION_USA
// #ifndef VERSION_TW
// #ifndef VERSION_HK
// 	if( m_LongTimeNotice.IsEnable() )
// 	{
// 		//cUserGrade::BeginGrade( false );
// 		cPrintMsg::PrintMsg( 12000, _LAN( "장시간 게임 이용은 건강에 해로울 수 있습니다." ) );
// 		m_LongTimeNotice2.Reset();
// 	}
// 	if( m_LongTimeNotice2.IsEnable() )
// 	{
// 		cPrintMsg::PrintMsg( 12000, _LAN( "적당한 휴식을 취한후 게임이용을 권장합니다." ) );
// 	}
// #endif
// #endif
// #endif
// 
// 	if( m_TimeNotice.IsEnable() )
// 	{
// 		cPrintMsg::PrintMsg( 12000, _LAN( "욕설 및 타인 비방 등 무분별한 외치기는 다른 사람에게" ) );
// 		m_TimeNotice2.Reset();
// 	}
// 	if( m_TimeNotice2.IsEnable() )
// 	{
// 		cPrintMsg::PrintMsg( 12000, _LAN( "피해를 줄 수 있습니다. 테이머 여러분들의 많은 양해 부탁드립니다." ) );
// 	}
// 
// 	if( m_AutoSaveResistTime.IsEnable() )
// 	{
// 		g_pResist->SaveGlobal();
// 		g_pResist->m_bLoadChar = true;
// 	}
// 
// 	g_pDataMng->Update();
// 
// 	// 카메라 업데이트 선	
// 	g_pTerrainMng->Update( g_fDeltaTime );
// 	g_pCharMng->Update();
// 	g_pNpcMng->Update_Tutorial();
// 	g_pItemMng->Update();
// #if defined(CARD_MASTER) || defined(SEAL_MASTERS_RENEW_201703)
// 	g_pCardMng->Update();
// #endif
// 	g_pCharResMng->Update_Tutorial( mi );
// 	g_pHelpArrow->Update();
// 
// 	g_pNumber3DMng->Update();
// 
// 	g_pGameEventMng->Update( g_fDeltaTime );
// 
// }
// 
// void CMngCollector::Render_Tutorial( bool bClearGetterTerrain )
// {
// 	g_pTerrainMng->BeginRender();	
// 	g_pTerrainMng->Render( bClearGetterTerrain );
// 	g_pTerrainMng->EndRender();	
// 
// 	g_pCharResMng->Render_PostTR();
// 
// 	g_pDataMng->Render();
// 
// 	g_pCharMng->Render_Tutorial();
// 	g_pNpcMng->Render_Tutorial();
// 	g_pCharResMng->Render_Tutorial();
// 	g_pHelpArrow->Render();		
// 
// 	g_pNpcMng->Render_PostChar_Tutorial();
// 	g_pCharMng->Render_PostChar_Tutorial();
// 	g_pCharResMng->Render_PostChar();	
// 	
// 	g_pNumber3DMng->Render();		
// 	g_pWeather->Render();
// 
// 	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
// }
// 
// void CMngCollector::Render_Shadow_Tutorial( bool bClearGetterTerrain )
// {
// 	NiPoint3 vShadowCenter =  g_pServerMoveOwner->GetParent()->GetPos();
// 	if( g_pShadow->BeginShadow( vShadowCenter ) == true )
// 	{
// 		cObjectController::RenderShadow_Tutorial();
// 		g_pShadow->EndShadow( g_pEngine->GetCurrentRenderTargetGroup() );
// 	}		
// 
// 	g_pTerrainMng->BeginRender();
// 	g_pTerrainMng->Render( bClearGetterTerrain );
// 	g_pTerrainMng->EndRender();
// 
// 	g_pCharResMng->Render_PostTR();
// 
// 	g_pDataMng->Render();
// 
// 	g_pCharMng->Render_Tutorial();
// 	g_pNpcMng->Render_Tutorial();
// 	g_pItemMng->Render();
// #if defined(CARD_MASTER) || defined(SEAL_MASTERS_RENEW_201703)
// 	g_pCardMng->Render();
// #endif
// 	g_pCharResMng->Render_Tutorial();
// 	g_pHelpArrow->Render();		
// 
// 	g_pNpcMng->Render_PostChar_Tutorial();
// 	g_pCharMng->Render_PostChar_Tutorial();
// 	g_pCharResMng->Render_PostChar();		
// 
// 	g_pNumber3DMng->Render();		
// 	g_pWeather->Render();
// 
// 	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
// }
// 
// void CNpcMng::Update_Tutorial()
// {
// 	CsVectorLink< CNpc* >::iterator* it = m_vnNpc.Begin();
// 	CsVectorLink< CNpc* >::iterator* itEnd = m_vnNpc.End();
// 	for( ; it!=itEnd; it = it->s_pNext )
// 	{
// 		it->s_Element->Update_Tutorial( g_fDeltaTime );
// 	}	
// }
// 
// void CNpcMng::Render_Tutorial()
// {
// 	m_vnNpc[ 18 ]->Render();	
// 	m_vnNpc[ 4015 ]->Render();
// }
// 
// void CNpcMng::Render_PostChar_Tutorial()
// {
// 	m_vnNpc[ 18 ]->Render_PostChar();
// 	m_vnNpc[ 4015 ]->Render_PostChar();
// }
// 
// void CNpc::Update_Tutorial( float fDeltaTime )
// {
// 	CsC_AvObject::Update( fDeltaTime );
// 
// 	if( m_bEnableUse == false )
// 		return;
// 
// 	switch( m_pFT_Npc->GetInfo()->s_dwNpcID )
// 	{
// 	case 18+NPC_FT_MASK:
// 	case 4015+NPC_FT_MASK:
// 		break;
// 	default:
// 		return;
// 	}
// 
// 	switch( m_eQuestViewType )
// 	{
// 	case QV_NONE:	
// 		if( m_pQuestOwner && ( GetProp_Alpha()->IsHideDistOut() == false ) )
// 		{
// 			if( m_pQuestOwner->IsDisplayRev() )
// 			{
// 				NiPoint3 pos = GetPos();
// 				pos.z += GetToolHeight();
// 				g_pCharResMng->NpcMarkInsert( GetIDX(), pos, m_pQuestOwner->GetDisplayRev() );
// 			}
// 		}
// 		break;
// 	case QV_NPCTALK:
// 		if( GetProp_Alpha()->IsHideDistOut() == false )
// 		{
// 			NiPoint3 pos = GetPos();
// 			pos.z += GetToolHeight();
// 			g_pCharResMng->NpcMarkInsert( GetIDX(), pos, cData_QuestOwner::sNpcOwner::TALK );
// 		}
// 		break;
// 	default:
// 		assert_cs( false );
// 	}	
// }
// 
// void cObjectController::RenderShadow_Tutorial()
// {
// #ifndef _GIVE
// 	if( g_pCharMng->m_bRenderChar == false )
// 		return;
// #endif
// 
// 	_RenderShadowObject( g_pCharMng->GetTamerUser() );
// 	_RenderShadowObject( g_pCharMng->GetDigimonUser( 0 ) );
// 	_RenderShadowObject( g_pNpcMng->GetNpc( 18 ) );
// 	_RenderShadowObject( g_pNpcMng->GetNpc( 4015 ) );
// }
// 
// CsC_AvObject* CMngCollector::PickObject_Tutorial( cType& type, POINT ptMouse, bool bIncludeSelf )
// {
// 	CsGBPick pick( false, NiPick::NO_SORT );
// 	float fLenth = FLT_MAX;
// 	NiPoint3 origin, dir;
// 	CAMERA_ST.WindowPointToRay( ptMouse.x, ptMouse.y, origin, dir );
// 
// 	CsC_AvObject* pReturn = NULL;
// 	CsC_AvObject* pSrc = NULL;
// 
// 	// 엔피씨
// 	pSrc = g_pNpcMng->GetNpc( 18 );
// 	if( pSrc->PickObject( &pick, &origin, &dir, fLenth ) == true )
// 	{
// 		type.m_nIDX = pSrc->GetIDX();
// 		type.m_nClass = nClass::Npc;
// 		return pSrc;
// 	}
// 
// 	pSrc = g_pNpcMng->GetNpc( 4015 );
// 	if( pSrc->PickObject( &pick, &origin, &dir, fLenth ) == true )
// 	{
// 		type.m_nIDX = pSrc->GetIDX();
// 		type.m_nClass = nClass::Npc;
// 		return pSrc;
// 	}	
// 
// 	// 몬스터
// 	if( ( pReturn = g_pCharMng->PickMonster( &pick, &origin, &dir, type, fLenth ) ) != NULL )
// 		return pReturn;
// 
// 	type.m_nUID = 0;
// 	return NULL;
// }
// 
// void CCharResMng::Update_Tutorial( cGlobalInput::eMOUSE_INPUT mi )
// {
// 	g_pEffectMng->Update( g_fDeltaTime );
// 
// 	_UpdateMouseOnObject_Tutorial( mi );
// 	_UpdateDeleteReady();
// 	_UpdateTargetMark();
// 	_UpdateCharImage();
// }
// 
// 
// 
// //=====================================================================================
// //
// //	마우스 온 오브젝트
// //
// //=====================================================================================
// void CCharResMng::_UpdateMouseOnObject_Tutorial( cGlobalInput::eMOUSE_INPUT mi )
// {
// 	POINT pt;
// 	GetCursorPos( &pt );
// 	ScreenToClient( GAMEAPP_ST.GetHWnd(), &pt );
// 
// 	switch( mi )
// 	{
// 	case cGlobalInput::MOUSEINPUT_CAMERA:
// 	case cGlobalInput::MOUSEINPUT_DISABLE:
// 		{
// 			m_pMouseOnLastObject = NULL;
// 			_ReleaseMouseOnObject();
// 			CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
// 		}
// 		return;
// 	case cGlobalInput::MOUSEINPUT_ENABLE:
// 		break;
// 	default:
// 		assert_cs( false );
// 	}
// 
// 	cType type;
// 	if( g_pMngCollector->PickObject_Tutorial( type, pt, false ) == false )
// 	{
// 		m_pMouseOnLastObject = NULL;
// 		_ReleaseMouseOnObject();
// 		CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
// 		return;
// 	}
// 
// 	CsC_AvObject* pMouseOn = g_pMngCollector->GetObject( type );
// 	if( pMouseOn == NULL )
// 	{
// 		m_pMouseOnLastObject = NULL;
// 		_ReleaseMouseOnObject();
// 		CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
// 		return;
// 	}
// 
// 	if( g_pResist->m_Global.s_bRightToolTip )
// 		TOOLTIPMNG_STPTR->GetRightTooltip()->SetRightTooltip( pMouseOn );
// 
// 	CURSOR_ST.SetCursorRes( pMouseOn );
// 	switch( pMouseOn->GetLeafRTTI() )
// 	{
// 	case RTTI_DIGIMON:
// 		if( g_pResist->m_Global.s_bFigureDigimon == true )
// 		{
// 			m_pMouseOnLastObject = NULL;
// 			_ReleaseMouseOnObject();
// 			return;
// 		}
// 		break;
// 	case RTTI_TAMER:
// 		{
// 			CTamer* pTamer = (CTamer*)pMouseOn;
// 			if( ( ( pTamer->GetCondition()->IsCondition( nSync::Shop ) == true )&&
// 				( g_pResist->m_Global.s_bFigureEmployment == TRUE ) )||
// 				( g_pResist->m_Global.s_bFigureTamer == true ) )
// 			{
// 				m_pMouseOnLastObject = NULL;
// 				_ReleaseMouseOnObject();
// 				return;
// 			}
// 		}
// 		break;
// 	case RTTI_MONSTER:
// 		{
// 			// 퀘스트 툴팁 체크
// 		}
// 		break;
// 	case RTTI_EMPLOYMENT:
// 		if( g_pResist->m_Global.s_bFigureEmployment == TRUE )
// 		{
// 			m_pMouseOnLastObject = NULL;
// 			_ReleaseMouseOnObject();
// 			return;
// 		}
// 		break;
// 	}
// 
// 	// 일단 Dimmer 초기화 - 어차피 아래서 재셋팅 해준다. 머 이정도 연산은 우습기 문에 그냥 해주자
// 	if( m_pMouseOnObject != NULL )
// 	{
// 	}
// 
// 	if( pMouseOn != m_pMouseOnLastObject )
// 	{
// 		m_pMouseOnLastObject = pMouseOn;
// 		m_pMouseOnObject = pMouseOn;
// 		m_fMouseOnObjectAlpha = 0.6f;
// 	}
// 	else if( m_pMouseOnObject != NULL )
// 	{		
// 		m_fMouseOnObjectAlpha -= 0.6f*g_fDeltaTime/3.5f;
// 
// 		if( m_fMouseOnObjectAlpha < 0.0f )
// 			m_fMouseOnObjectAlpha = 0.0f;
// 
// 		if( m_fMouseOnObjectAlpha > 0.0f )
// 		{
// 		}
// 		else
// 		{
// 			_ReleaseMouseOnObject();
// 		}		
// 	}
// }
// 
// void CCharMng::Render_Tutorial()
// {
// #ifndef _GIVE
// 	if( m_bRenderChar == false )
// 		return;
// #endif
// 
// 	GetTamerUser()->Render();
// 	GetDigimonUser( 0 )->Render();
// 
// 	_RenderMonster();
// }
// 
// void CCharMng::Render_PostChar_Tutorial()
// {
// 	GetTamerUser()->Render_PostChar();
// 	GetDigimonUser( 0 )->Render_PostChar();
// }
// 
// void CCharResMng::Render_Tutorial()
// {
// 	_RenderMovePoint();	
// 	_RenderMonsterCreateScene();
// 	_RenderMouseOnObject();
// 	_RenderFigure();
// }