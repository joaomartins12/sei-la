#include "StdAfx.h"
#include "EventScene.h"

CEventScene*		g_pEventScene = NULL;

#define SCENE_MYTAMER		10001
#define SCENE_MYDIGIMON		10002

CEventScene::CEventScene(void)
{
	m_pSprite1 = NULL;	
	m_pSprite2 = NULL;	
	m_pSkip = NULL;	
	m_pNext = NULL;
	m_pTitle = NULL;
}

CEventScene::~CEventScene(void)
{
}

void CEventScene::Delete()
{
	_DeleteTarget();

	SAFE_NIDELETE( m_pRenderTex1 );
	SAFE_NIDELETE( m_pRenderTex2 );
	SAFE_NIDELETE( m_pRenderTex3 );
	SAFE_NIDELETE( m_pRenderTex4 );
	SAFE_NIDELETE( m_pRenderTex5 );	

	SAFE_NIDELETE( m_pSprite1 );
	SAFE_NIDELETE( m_pSprite2 );
	SAFE_NIDELETE( m_pSprite3 );

	SAFE_NIDELETE( m_pSkip );
	SAFE_NIDELETE( m_pNext );
	SAFE_NIDELETE( m_pBack );	

	SAFE_NIDELETE( m_pTitle );
	
	m_StringList.Delete();
}

void CEventScene::ResetDevice()
{
	if( m_pSkip )
		m_pSkip->ResetDevice();
	if( m_pNext )
		m_pNext->ResetDevice();
	if( m_StringList.GetSize() > 0 )
		m_StringList.ResetDevice();	

	if( m_pRenderTex1 )
		m_pRenderTex1->ResetDevice();
	if( m_pRenderTex2 )
		m_pRenderTex2->ResetDevice();
	if( m_pRenderTex3 )
		m_pRenderTex3->ResetDevice();
	if( m_pRenderTex4 )
		m_pRenderTex4->ResetDevice();
	if( m_pRenderTex5 )
		m_pRenderTex5->ResetDevice();

	if( m_pTitle )
		m_pTitle->ResetDevice();
}

void CEventScene::EventStart( int nSceneNum )
{	
	m_nSceneNum = nSceneNum;
	if( net::game )
	{
		if( g_pResist->m_CharEvent.Check( m_nSceneNum ) == true )
			return;
	}

	// 이동 중지	
	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();		
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();	

	if( g_pGameIF )
	{
		// 채팅입력창을 띄운 후 이벤트 씬이 발생하면 채팅창이 사라지지 않고
		// 입력이 가능한 버그로 이벤트 시작시 채팅창을 닫도록 함.
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
		CProtocol.m_value= 0;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	}
#ifdef KEYBOARD_MOVE
	// 키보드로 이동중 이벤트 씬이 시작되고 끝나면 계속 전진하지 않도록 이벤트 씬이 시작되면 키보드값 초기화 시킴
	if(g_pServerMoveOwner->GetKeyCheck() )
		g_pServerMoveOwner->SetKeyCheck( CUserServerMove::KEY_NONE );
#endif

	m_bLoad = false;	
	m_pTarget1 = m_pTarget2 = m_pTarget3 = m_pTarget4 = m_pTarget5 = NULL;		
	m_pScene = nsCsFileTable::g_pSceneDataMng->GetSceneData( m_nSceneNum );
	assert_cs( m_pScene != NULL );
	SAFE_POINTER_RET( m_pScene );

	CSceneData::sSceneInfo* pSceneInfo = m_pScene->GetInfo();
	SAFE_POINTER_RET( pSceneInfo )

	assert_csm( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_EVENT, L"이벤트 중복" );
	g_pMngCollector->SetSceneState( CMngCollector::eSCENE_EVENT );	
	
	m_it = m_pScene->GetList()->begin();

	std::list< CSceneData::sData* >::iterator it = m_pScene->GetList()->begin();
	std::list< CSceneData::sData* >::iterator itEnd = m_pScene->GetList()->end();

	for( ; it != itEnd; it++ )
	{
		for( int i = 0; i < MAX_SCENE_CHARNUM; i++ )
		{
			if( (*it)->s_dwTarget[ i ] == 0 ||
				(*it)->s_dwTarget[ i ] == 1 ||
				(*it)->s_dwTarget[ i ] == 2 )
				continue;

			DWORD dwTarget = (*it)->s_dwTarget[ i ];
			if( _IsTarget( dwTarget ) )
				continue;

			CsSceneDataMng::sTargetData* pTargetData = nsCsFileTable::g_pSceneDataMng->GetSceneTarget( dwTarget );
			SAFE_POINTER_CON( pTargetData );
			
			_InitTarget( pTargetData->s_dwUID, pTargetData->s_dwModelID, pTargetData->s_szName );
		}		
	}			
	
	if( strlen( pSceneInfo->s_szMusic ) > 0 )
		PlayMusic( pSceneInfo->s_szMusic );
	
	CsC_AvObject::g_bEnableVoice = false;	

	m_bIsSound = g_pResist->IsSound();
	g_pResist->SetEnableSound( false );
	g_pSoundMgr->Play_Stop_Sound();				

	g_pGameIF->SetRenderIF( false );	
	if( m_pTitle )
	{
		m_pTitle->SetText( pSceneInfo->s_szTitle );
		m_pTitle->SetAlpha( 0.0f );
	}

	m_fTitleAlpha = 0.0f;	
	m_bAlpha = true;	

	if( net::game )
		net::game->SendViewEventSeen();

	// 페이드 아웃	
//	SAFE_DELETE( g_pFade );
//	g_pFade = NiNew CFade( FADE_IN, 0.5f );		
}

void CEventScene::EventEnd()
{
	_DeleteTarget();

	// 페이드 아웃	
//	SAFE_DELETE( g_pFade );
//	g_pFade = NiNew CFade( FADE_IN, 0.6f );
	
	g_pGameIF->SetRenderIF( true );		

	g_pResist->SetEnableSound( m_bIsSound );
	CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;
	g_pCharMng->GetTamerUser()->PlayBGM();

	PlayMusic( _LoadMusic() );
	
	m_StringList.Delete();
	g_pMngCollector->ReleaseScene();

	g_pResist->m_CharEvent.on( m_nSceneNum );
	g_pResist->SaveGameEvent();
		
}

void CEventScene::NextStep()
{	
	m_it++;
	if( m_it == m_pScene->GetList()->end() )
		EventEnd();
	else
		_NextStep();
}

void CEventScene::Init()
{
	// 이벤트 바 생성			
	float fViewSize = (float)g_nScreenHeight * 0.2f;
	m_pSprite1 = NiNew cSprite;			
	m_pSprite1->Init( NULL, CsPoint::ZERO, CsPoint( (int)g_nScreenWidth, (int)fViewSize ), false, NiColor::BLACK );				

	fViewSize = (float)g_nScreenHeight * 0.24f;
	m_pSprite2 = NiNew cSprite;			
	m_pSprite2->Init( NULL, CsPoint( 0, (int)(g_nScreenHeight - fViewSize) ), CsPoint( (int)g_nScreenWidth, (int)fViewSize ), false, NiColor::BLACK );						

	m_pSprite3 = NiNew cSprite;			
	m_pSprite3->Init( NULL, CsPoint( 0, (int)(g_nScreenHeight - fViewSize) ), CsPoint( (int)g_nScreenWidth, (int)fViewSize ), "Scene\\Win.tga", false );						

	m_pSkip = NiNew cButton;
	m_pSkip->Init( NULL, CsPoint( 10, 10 ), CsPoint( 85, 19 ), "Scene\\Skip.tga", true );					
	m_pSkip->SetTexToken( CsPoint( 67, 17 ) );

	m_pNext = NiNew cButton;
	m_pNext->Init( NULL, CsPoint( 880, 600 ), CsPoint( 85, 19 ), "Scene\\Next.tga", true );				
	m_pNext->SetTexToken( CsPoint( 78, 17 ) );

	m_pBack = NiNew cSprite;
	m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), true, NiColor::BLACK);
	m_pBack->SetAlpha( 0.5f );

	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_32, FONT_WHITE );					
	ti.s_bOutLine = false;		
	ti.s_eTextAlign = DT_CENTER;

	m_pTitle = NiNew cText;
	m_pTitle->Init( NULL, CsPoint( 512, 170 ), &ti, true );

	m_pRenderTex1 = NiNew cRenderTex;
	if( m_pRenderTex1->Init( CsPoint( 1024, 768 ) ) == false )
		SAFE_NIDELETE( m_pRenderTex1 );

	m_pRenderTex2 = NiNew cRenderTex;
	if( m_pRenderTex2->Init( CsPoint( 1024, 768 ) ) == false )
		SAFE_NIDELETE( m_pRenderTex2 );

	m_pRenderTex3 = NiNew cRenderTex;
	if( m_pRenderTex3->Init( CsPoint( 1024, 768 ) ) == false )
		SAFE_NIDELETE( m_pRenderTex3 );

	m_pRenderTex4 = NiNew cRenderTex;
	if( m_pRenderTex4->Init( CsPoint( 1024, 768 ) ) == false )
		SAFE_NIDELETE( m_pRenderTex4 );

	m_pRenderTex5 = NiNew cRenderTex;
	if( m_pRenderTex5->Init( CsPoint( 1024, 768 ) ) == false )
		SAFE_NIDELETE( m_pRenderTex5 );
	
}

void CEventScene::Update( float fDeltaTime )
{
	std::map< DWORD, CsC_AvObject* >::iterator it = m_mapTarget.begin();
	std::map< DWORD, CsC_AvObject* >::iterator itEnd = m_mapTarget.end();
	for( ; it != itEnd ; it++ )
	{		
		switch( it->second->GetLeafRTTI() )
		{		
		case RTTI_EVENTOBJECT:
			( (CsC_EventObject*)it->second )->Update( fDeltaTime );
			break;
		case RTTI_EVENTTAMEROBJECT:
			( (CsC_EventTamerObject*)it->second )->Update( fDeltaTime );
			break;
		default:
			assert_cs( false );
			break;
		}
	}

	_LoadCheck( fDeltaTime );
	if( m_bLoad == false )
		return;

	if( m_bAlpha == true )
	{
		if( ( m_fTitleAlpha += g_fDeltaTime / 1.0f ) > 1.0f )
		{
			m_bAlpha = false;
		}
	}
	else
	{
		if( ( m_fTitleAlpha -= g_fDeltaTime / 0.5f ) < 0.0f )
		{
			m_fTitleAlpha = 0.0f;			
		}
	}
	
	m_pTitle->SetAlpha( m_fTitleAlpha );	
	
	// 이벤트 스킵 버튼
	if( m_pScene->GetInfo()->s_bSkip == true )
	{
		if( m_pSkip->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			EventEnd();
			return;
		}
	}
	

	// 다음 버튼
	if( m_pNext && m_pNext->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		NextStep();
		return;
	}

	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
	{
		NextStep();
		return;
	}
}

void CEventScene::Render()
{
	g_pEngine->ScreenSpace();

	m_pBack->Render();
	m_pSprite1->Render();	

	if( m_bLoad == true )
	{
		int nWidth = g_nScreenWidth / 5;
		//2015-12-02-nova RenderObject 카메라 11번
		if( m_pRenderTex4 != NULL && m_pTarget4 != NULL )
		{
			m_pRenderTex4->BeginRender();
			m_pRenderTex4->SetHeightConstant( m_pTarget4->s_fHeight );
			m_pRenderTex4->RenderObjectEventScene(GET_SUBCAMERA(CAMERA_12), _GetTarget( m_pTarget4->s_dwUID ), m_pTarget4->s_fDist, 0.2f );		
			m_pRenderTex4->EndRender( GET_SUBCAMERA(CAMERA_12), CsPoint( nWidth - 260, 20 ) );
		}

		if( m_pRenderTex5 != NULL && m_pTarget5 != NULL )
		{
			m_pRenderTex5->BeginRender();
			m_pRenderTex5->SetHeightConstant( m_pTarget5->s_fHeight );
			m_pRenderTex5->RenderObjectEventScene(GET_SUBCAMERA(CAMERA_12), _GetTarget( m_pTarget5->s_dwUID ), m_pTarget5->s_fDist, -0.2f );
			m_pRenderTex5->EndRender( GET_SUBCAMERA(CAMERA_12), CsPoint( nWidth * 3 - 500, 20 ) );
		}

		if( m_pRenderTex1 != NULL && m_pTarget1 != NULL )
		{
			m_pRenderTex1->BeginRender();
			m_pRenderTex1->SetHeightConstant( m_pTarget1->s_fHeight );
			m_pRenderTex1->RenderObjectEventScene(GET_SUBCAMERA(CAMERA_12), _GetTarget( m_pTarget1->s_dwUID ), m_pTarget1->s_fDist, 0.4f );
			m_pRenderTex1->EndRender( GET_SUBCAMERA(CAMERA_12), CsPoint( -150, 20 ) );
		}

		if( m_pRenderTex2 != NULL && m_pTarget2 != NULL )
		{
			m_pRenderTex2->BeginRender();
			m_pRenderTex2->SetHeightConstant( m_pTarget2->s_fHeight );
			m_pRenderTex2->RenderObjectEventScene(GET_SUBCAMERA(CAMERA_12), _GetTarget( m_pTarget2->s_dwUID ), m_pTarget2->s_fDist, 0.0f );
			m_pRenderTex2->EndRender( GET_SUBCAMERA(CAMERA_12), CsPoint( nWidth * 2 - 400, 20 ) );
		}

		if( m_pRenderTex3 != NULL && m_pTarget3 != NULL )
		{
			m_pRenderTex3->BeginRender();
			m_pRenderTex3->SetHeightConstant( m_pTarget3->s_fHeight );
			m_pRenderTex3->RenderObjectEventScene(GET_SUBCAMERA(CAMERA_12), _GetTarget( m_pTarget3->s_dwUID ), m_pTarget3->s_fDist, -0.4f );
			m_pRenderTex3->EndRender( GET_SUBCAMERA(CAMERA_12), CsPoint( nWidth * 4 - 700, 20 ) );
		}		
	}

	GET_SUBCAMERA(CAMERA_12)->_UpdateCamera();
	
	m_pSprite2->Render();	
	m_pSprite3->Render();

	float fViewSize = (float)g_nScreenHeight * 0.24f;
	switch( m_eAlpa )
	{
	case ALPA_INCREASE:
		{
			m_fAlpaValue += g_fDeltaTime * 0.8f;
			if( m_fAlpaValue >= 1.0f )
			{
				m_fAlpaValue = 1.0f;
				m_eAlpa = ALPA_STOP;
			}
			m_StringList.SetAlpha( m_fAlpaValue );
			m_StringList.Render( CsPoint( g_nScreenWidth / 8, (int)( g_nScreenHeight - fViewSize ) ), 5 );			
		}			
		break;
	case ALPA_DECREASE:
		{
			m_fAlpaValue -= g_fDeltaTime * 0.8f;
			if( m_fAlpaValue <= 0.0f )
			{
				m_fAlpaValue = 0.0f;
				m_StringList.Delete();
				break;
			}
			m_StringList.SetAlpha( m_fAlpaValue );
			m_StringList.Render( CsPoint( g_nScreenWidth / 8, (int)( g_nScreenHeight - fViewSize ) ), 5 );			
		}			
		break;
	case ALPA_STOP:
		m_StringList.Render( CsPoint( g_nScreenWidth / 8, (int)( g_nScreenHeight - fViewSize ) ), 5 );			
		break;
	}		

	if( m_pScene->GetInfo()->s_bSkip == true )
	{
		m_pSkip->Render();	
	}
	m_pNext->Render();

	m_pTitle->Render();
}

char const* CEventScene::_LoadMusic()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	if( nsCsMapTable::g_pMapListMng->IsList( dwMapID ) == false )
		return NULL;

	CsMapRegion::sINFO* pInfo = NULL;
	float fLength;
	NiPoint2 ptSrc = NiPoint2( g_pCharMng->GetTamerUser()->GetPos().x, g_pCharMng->GetTamerUser()->GetPos().y );
	CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( dwMapID );
	CsMapRegion::LIST* pList = pGroup->GetRegionList();
	CsMapRegion::LIST_IT it = pList->begin();
	for( ; it!=pList->end(); ++it )
	{
		pInfo = (*it)->GetInfo();
		fLength = ( ptSrc - NiPoint2( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY ) ).Length();
		if( fLength < pInfo->s_nRadius )
		{
			if( pInfo->s_cBGSound[ 0 ] != NULL )
				return pInfo->s_cBGSound;
			break;
		}
	}

	CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( dwMapID )->GetInfo();	
	if( pListInfo->s_cBGSound[ 0 ] != NULL )
		return pListInfo->s_cBGSound.c_str();
	return NULL;
}

void CEventScene::_LoadCheck( float fDeltaTime )
{
	if( m_bLoad == true )
		return;

	std::map< DWORD, CsC_AvObject* >::iterator it = m_mapTarget.begin();
	std::map< DWORD, CsC_AvObject* >::iterator itEnd = m_mapTarget.end();
	for( ; it != itEnd ; it++ )
	{		
		if( it->second->IsLoad() == false )
		{
			m_pNext->SetEnable( false );
			m_pSkip->SetEnable( false );
			m_bLoad = false;
			return;
		}				
	}

	m_pNext->SetEnable( true );
	m_pSkip->SetEnable( true );	
	_NextStep();
	m_bLoad  = true;
}

void CEventScene::_NextStep()
{
	_SetSceneTarget();
	_SetAni();
	_SetEffect();
	_SetText();	

	if( strlen( (*m_it)->s_szSound ) > 0 )
		PlaySound( (*m_it)->s_szSound );
}

void CEventScene::_SetAlpha( eALPA_TYPE eAlpha, float fAlpha )
{
	m_eAlpa = eAlpha;
	m_fAlpaValue = fAlpha;
	m_StringList.SetAlpha( fAlpha );
}

void CEventScene::_SetText()
{	
	assert_cs( (*m_it)->s_dwMainTarget != 0 );

	m_StringList.Delete();
	m_StringList.AddTailEmpty( CFont::FS_8 );

	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20, NiColor( 1.0f, 1.0f, 170.f / 255.0f ) );					
	ti.s_bOutLine = false;		
	ti.s_eTextAlign = DT_LEFT;

	CsC_AvObject* pTarget = _GetTarget( (*m_it)->s_dwMainTarget );
	if( pTarget != NULL )
	{			
		switch( pTarget->GetLeafRTTI() )
		{		
		case RTTI_EVENTOBJECT:			
			g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 300, ((CsC_EventObject*)pTarget)->GetName(), &ti );
			break;		
		case RTTI_EVENTTAMEROBJECT:			
			g_pStringAnalysis->Cut( &m_StringList, g_nScreenWidth - 300, ((CsC_EventTamerObject*)pTarget)->GetName(), &ti );
			break;			
		default:
			assert_cs( false );
			break;
		}	
	}		

	m_StringList.AddTailEmpty( CFont::FS_8 );	

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, FONT_WHITE );					
	ti.s_bOutLine = false;
	//ti.s_eTextAlign = DT_CENTER;	

	TCHAR sz[ 256 + 64 ];
	g_pStringAnalysis->StringTransfer( sz, 256 + 64, (*m_it)->s_szTalk );	
	g_pStringAnalysis->Cut_Parcing( &m_StringList, g_nScreenWidth - 300, sz, &ti );	

	_SetAlpha( ALPA_INCREASE, 0.0f );
}

void CEventScene::_SetEffect()
{	
	if( strlen( (*m_it)->s_szEffect ) < 1 )
		return;

	CsC_AvObject* pTarget = _GetTarget( (*m_it)->s_dwMainTarget );

	DWORD dwMask = nsEFFECT::LIVE_ONCE | nsEFFECT::DIR_CAMERA_AXIS_Z | nsEFFECT::SPOS_CHARWORLD;
	pTarget->GetProp_Effect()->AddEffect( (*m_it)->s_szEffect, 1.5f, dwMask, true );	
}

void CEventScene::_SetAni()
{
	if( (*m_it)->s_dwAni == 0 )
		return;

	CsC_AvObject* pTarget = _GetTarget( (*m_it)->s_dwMainTarget );
	pTarget->SetAnimation( (*m_it)->s_dwAni );
}

void CEventScene::_DeleteTarget()
{
	std::map< DWORD, CsC_AvObject* >::iterator it = m_mapTarget.begin();
	std::map< DWORD, CsC_AvObject* >::iterator itEnd = m_mapTarget.end();
	for( ; it != itEnd ; it++ )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = it->second;	
		g_pThread->DeleteChar( pUnit );			
	}
	m_mapTarget.clear();
}

void CEventScene::_InitTarget( DWORD dwIDX, DWORD dwModelID, TCHAR* szName )
{	
	NiPoint3 vPos = g_pCharMng->GetTamerUser()->GetPos();		
	CsC_AvObject*	pTarget = NULL;

	switch( dwIDX )
	{
	case SCENE_MYTAMER: // 내테이머
		{
			sCREATEINFO Prop;
			Prop.s_dwOptionPlag = OP_LIGHT | OP_STAT;
			Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_MOVECON_ENABLE;	
			Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

			cItemInfo* pItemInfo = NULL;
			CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];
			for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
			{
				if( i >= nTamer::MaxParts )
					pItemInfo = g_pDataMng->GetTEquip()->GetDigiviceItem();
				else
					pItemInfo = g_pDataMng->GetTEquip()->GetData( i );
				SAFE_POINTER_CON( pItemInfo );

				cp[ i ].s_nFileTableID = pItemInfo->GetType();
				cp[ i ].s_nPartIndex = i;
				cp[ i ].s_nRemainTime = pItemInfo->m_nEndTime;

			}

			//내 캐릭터 생성
			CsC_AvObject* pTamer = g_pCharMng->GetTamerUser();
			CsC_EventTamerObject* pEvent = CsC_EventTamerObject::NewInstance();	
			pEvent->Init( dwIDX, pTamer->GetModelID(), vPos, 0.0f, &Prop, cp );	
			pEvent->SetName( pTamer->GetName() );		
			pEvent->m_eObjectState = CsC_EventTamerObject::OBJECT_NORMAL;

			pTarget = (CsC_AvObject*)pEvent;	
		}
		break;
	case SCENE_MYDIGIMON: // 내디지몬
		{
			// 로드시 필요한 초기화
			sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, dwIDX );

			// 초기화 - Prop 초기화 포함
			pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
			pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_MOVECON_ENABLE;
			pUint->s_Prop.s_dwOptionPlag = OP_LIGHT | OP_STAT;

			DWORD dwModelID = g_pCharMng->GetDigimonUser( 0 )->GetModelID();
			std::string strPath = g_pModelDataMng->GetKfmPath( dwModelID );
			strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );	

			//신규캐릭터 생성
			CsC_EventObject* pEvent = CsC_EventObject::NewInstance();	
			pEvent->Init( dwIDX, g_pCharMng->GetDigimonUser( 0 )->GetModelID(), vPos, 0.0f, &pUint->s_Prop );			
			pEvent->SetName( g_pCharMng->GetDigimonUser( 0 )->GetName() );		
			pEvent->m_eObjectState = CsC_EventObject::OBJECT_NORMAL;

			// 로드 설정
			pUint->s_pLoadedObject = (CsC_EventObject*)pEvent;	
			//우선로드 설정
			pUint->SetLoadType( sTCUnit::eFast );

			pTarget = (CsC_AvObject*)pEvent;		

			// 쓰레드 로드		
			g_pThread->LoadChar( pUint );
		}
		break;
	default:
		{
			if( dwModelID / 10000 == 8 )
			{	
				sCREATEINFO Prop;
				Prop.s_dwOptionPlag = OP_LIGHT | OP_STAT;
				Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_MOVECON_ENABLE;	
				Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

				CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];
				for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
					cp[ i ].s_nPartIndex = i;

				//신규캐릭터 생성
				CsC_EventTamerObject* pEvent = CsC_EventTamerObject::NewInstance();	
				pEvent->Init( dwIDX, dwModelID, vPos, 0.0f, &Prop, cp );	
				pEvent->SetName( szName );		
				pEvent->m_eObjectState = CsC_EventTamerObject::OBJECT_NORMAL;

				pTarget = (CsC_AvObject*)pEvent;
			}
			else
			{
				// 로드시 필요한 초기화
				sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, dwIDX );

				// 초기화 - Prop 초기화 포함
				pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
				pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_MOVECON_ENABLE;
				pUint->s_Prop.s_dwOptionPlag = OP_LIGHT | OP_STAT;

				std::string strPath = g_pModelDataMng->GetKfmPath( dwModelID );
				strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );	

				//신규캐릭터 생성
				CsC_EventObject* pEvent = CsC_EventObject::NewInstance();	
				pEvent->Init( dwIDX, dwModelID, vPos, 0.0f, &pUint->s_Prop );			
				pEvent->SetName( szName );		
				pEvent->m_eObjectState = CsC_EventObject::OBJECT_NORMAL;

				// 로드 설정
				pUint->s_pLoadedObject = (CsC_EventObject*)pEvent;	
				//우선로드 설정
				pUint->SetLoadType( sTCUnit::eFast );

				pTarget = (CsC_AvObject*)pEvent;		

				// 쓰레드 로드		
				g_pThread->LoadChar( pUint );						
			}

		}
		break;
	}
	
	pTarget->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_STOP, 1.0f );

	m_mapTarget.insert( std::make_pair( dwIDX, pTarget ) );
}

bool CEventScene::_IsTarget( DWORD dwIdx )
{
	return ( m_mapTarget.find( dwIdx ) != m_mapTarget.end() );
}

CsC_AvObject* CEventScene::_GetTarget( DWORD dwIdx )
{
	if( _IsTarget( dwIdx ) == true )
		return m_mapTarget[ dwIdx ];

	return NULL;
}

void CEventScene::_SetSceneTarget()
{
	m_pTarget1 = m_pTarget2 = m_pTarget3 = m_pTarget4 = m_pTarget5 = NULL;	
	
	if( (*m_it)->s_dwTarget[ 0 ] != 0 )
	{
		if( _IsTarget( (*m_it)->s_dwTarget[ 0 ] ) == true )
			m_pTarget1 = nsCsFileTable::g_pSceneDataMng->GetSceneTarget( (*m_it)->s_dwTarget[ 0 ] );

		if( (*m_it)->s_dwTarget[ 0 ] == SCENE_MYDIGIMON )
		{
			m_pTarget1->s_fDist = _GetDist();
			m_pTarget1->s_fHeight = _GetHeight();
		}
	}

	if( (*m_it)->s_dwTarget[ 1 ] != 0 )
	{
		if( _IsTarget( (*m_it)->s_dwTarget[ 1 ] ) == true )
			m_pTarget2 = nsCsFileTable::g_pSceneDataMng->GetSceneTarget( (*m_it)->s_dwTarget[ 1 ] );	

		if( (*m_it)->s_dwTarget[ 1 ] == SCENE_MYDIGIMON )
		{
			m_pTarget2->s_fDist = _GetDist();
			m_pTarget2->s_fHeight = _GetHeight();
		}
	}

	if( (*m_it)->s_dwTarget[ 2 ] != 0 )
	{
		if( _IsTarget( (*m_it)->s_dwTarget[ 2 ] ) == true )
			m_pTarget3 = nsCsFileTable::g_pSceneDataMng->GetSceneTarget( (*m_it)->s_dwTarget[ 2 ] );	

		if( (*m_it)->s_dwTarget[ 2 ] == SCENE_MYDIGIMON )
		{
			m_pTarget3->s_fDist = _GetDist();
			m_pTarget3->s_fHeight = _GetHeight();
		}
	}

	if( (*m_it)->s_dwTarget[ 3 ] != 0 )
	{
		if( _IsTarget( (*m_it)->s_dwTarget[ 3 ] ) == true )
			m_pTarget4 = nsCsFileTable::g_pSceneDataMng->GetSceneTarget( (*m_it)->s_dwTarget[ 3 ] );	

		if( (*m_it)->s_dwTarget[ 3 ] == SCENE_MYDIGIMON )
		{
			m_pTarget4->s_fDist = _GetDist();
			m_pTarget4->s_fHeight = _GetHeight();
		}
	}

	if( (*m_it)->s_dwTarget[ 4 ] != 0 )
	{
		if( _IsTarget( (*m_it)->s_dwTarget[ 4 ] ) == true )
			m_pTarget5 = nsCsFileTable::g_pSceneDataMng->GetSceneTarget( (*m_it)->s_dwTarget[ 4 ] );	

		if( (*m_it)->s_dwTarget[ 4 ] == SCENE_MYDIGIMON )
		{
			m_pTarget5->s_fDist = _GetDist();
			m_pTarget5->s_fHeight = _GetHeight();
		}
	}	

	std::map< DWORD, CsC_AvObject* >::iterator it = m_mapTarget.begin();
	std::map< DWORD, CsC_AvObject* >::iterator itEnd = m_mapTarget.end();	
	for( ; it != itEnd ; it++ )
	{	
		SAFE_POINTER_CON( it->second );

		CsNodeObj* pNode = it->second->GetCsNode();
		SAFE_POINTER_CON( pNode );
		if( (*m_it)->s_dwMainTarget == it->second->GetIDX() )
			pNode->ChangeShader( CGeometry::GetCharOutLineType(), false );		
		else
			pNode->ChangeShader( CGeometry::GetCharOutLineGrayType(), false );		
	}
}

float CEventScene::_GetDist()
{
	CsSceneDataMng::MAP_TAR::iterator it = nsCsFileTable::g_pSceneDataMng->GetTargetMap()->begin();
	CsSceneDataMng::MAP_TAR::iterator itEnd = nsCsFileTable::g_pSceneDataMng->GetTargetMap()->end();

	for( ;it != itEnd; it++ )
	{
		if( it->second->s_dwModelID == g_pCharMng->GetDigimonUser( 0 )->GetModelID() )
		{
			return it->second->s_fDist;
		}
	}

	CsC_AvObject* pObject = g_pCharMng->GetDigimonUser( 0 );		
	return pObject->GetToolHeight()*sqrt( ( pObject->GetToolHeight()*0.3f )*0.05f );	
}

float CEventScene::_GetHeight()
{
	CsSceneDataMng::MAP_TAR::iterator it = nsCsFileTable::g_pSceneDataMng->GetTargetMap()->begin();
	CsSceneDataMng::MAP_TAR::iterator itEnd = nsCsFileTable::g_pSceneDataMng->GetTargetMap()->end();

	for( ;it != itEnd; it++ )
	{
		if( it->second->s_dwModelID == g_pCharMng->GetDigimonUser( 0 )->GetModelID() )
		{
			return it->second->s_fHeight;
		}
	}

	return 1.2f;	
}

void CEventScene::LoadScene( DWORD dwMapId )
{
	m_mapSceneCond.clear();

	CsSceneDataMng::MAP_IT it = nsCsFileTable::g_pSceneDataMng->GetSceneMap()->begin();
	CsSceneDataMng::MAP_IT itEnd = nsCsFileTable::g_pSceneDataMng->GetSceneMap()->end();
	for( ; it != itEnd; it++ )
	{
		if( ( it->second->GetInfo()->s_nMap ) == dwMapId )
		{
			DWORD dwCon = it->second->GetInfo()->s_nMap * 100 + it->second->GetInfo()->s_nResion; 
			m_mapSceneCond[ dwCon ] = it->first;
		}
	}
}