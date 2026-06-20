
#include "stdafx.h"
#include "CharResMng.h"

CCharResMng*	g_pCharResMng = NULL;


void CALLBack_KeyAutoMapping(void)
{
	g_pServerMoveOwner->KeyAutoMapping();
}


bool CCharResMng::CALLBACK_ThreadDelete( sTCUnit* pUnit )
{
	CsC_Object* pObject = pUnit->s_pLoadedObject;
	switch( pObject->GetLeafRTTI() )
	{
	case RTTI_TAMER:
		CTamer::DeleteInstance( (CTamer*)pObject );
		return true;
	case RTTI_TAMER_USER:
		NiDelete (CTamerUser*)pObject;
		return true;
	case RTTI_DIGIMON:
		CDigimon::DeleteInstance( (CDigimon*)pObject );
		return true;
	case RTTI_DIGIMON_USER:
		NiDelete (CDigimonUser*)pObject;
		return true;
	case RTTI_ITEM:
		CItem::DeleteInstance( (CItem*)pObject );
		return true;
	case RTTI_NPC:
		CNpc::DeleteInstance( (CNpc*)pObject );
		return true;
	case RTTI_MONSTER:
		CMonster::DeleteInstance( (CMonster*)pObject );
		return true;
	case RTTI_EMPLOYMENT:
		CEmployment::DeleteInstance( (CEmployment*)pObject );
		return true;
	case RTTI_TACTICSOBJECT:
		NiDelete (cTacticsObject*)pObject;
		return true;
	case RTTI_PAT:
		CPat::DeleteInstance( (CPat*)pObject );
		return true;
	case RTTI_TUTORIAL_TAMER:
		NiDelete (CTutorialTamerUser*)pObject;
		return true;
	case RTTI_TUTORIAL_DIGIMON:
		NiDelete (CTutorialDigimonUser*)pObject;
		return true;
	case RTTI_TUTORIAL_MONSTER:
		NiDelete (CTutorialMonster*)pObject;
		return true;
// 	case RTTI_CARD:
// 		CCard::DeleteInstance( (CCard*)pObject );
// 		return true;
	default:
		assert_cs( false );
	}
	return false;
}


CCharResMng::CCharResMng()
{
	m_pDefaultLight = NULL;
	m_pMouseOnObject = NULL;
	m_pMouseOnLastObject = NULL;	
}


void CCharResMng::Destroy()
{
	CEffectMng::GlobalShotDown();
	while( m_queueDeleteReady.empty() == false )
	{
		m_queueDeleteReady.pop();
	}

	_DeleteDefaultLight();
	_DeleteMonsterCreateScene();
	_DeleteMovePoint();	
	_ReleaseMouseOnObject();
	_DeleteTargetMark();
	_DeleteNpcMark();
	_DeleteFigure();
	_DeleteCharImage();

	CsC_Thread::GlobalShotDown();
}

void CCharResMng::Init()
{
	CsC_Thread::GlobalInit();
	// 콜백 등록
	g_pThread->ResistCallBack_DeleteUnit( CALLBACK_ThreadDelete );
	g_pThread->ResistCallBack_LoadFileTable( Thread_LoadFileTable );
	CAMERA_ST.ResistCallBack_KeyAutoMapping( CALLBack_KeyAutoMapping );

	CEffectMng::GlobalInit();

	_CreateDefaultLight();
	_CreateMonsterCreateScene();
	_CreateMovePoint();	
	_CreateTargetMark();
	_CreateNpcMark();
	_CreateFigure();
	_InitCharImage();
}

void CCharResMng::Reset()
{
	g_pEffectMng->Reset();
	_ResetMonsterCreateScene();
	_ResetFigure();
	ReleaseMovePoint();	
	ReleaseTargetMark( true );	
}


void CCharResMng::PostLoadMap()
{	
	_PostLoadMapFigure();	
}

void CCharResMng::LoadChar( sTCUnit* pUnit )
{
	g_pThread->LoadChar( pUnit );
}	

void CCharResMng::Update( cGlobalInput::eMOUSE_INPUT mi )
{
	g_pEffectMng->Update( g_fDeltaTime );

	_UpdateMouseOnObject( mi );
	_UpdateDeleteReady();
	_UpdateTargetMark();
	_UpdateCharImage();
}

void CCharResMng::Update_Tutorial(cGlobalInput::eMOUSE_INPUT mi)
{
	g_pEffectMng->Update( g_fDeltaTime );

	_UpdateMouseOnObject_Tutorial( mi );
	_UpdateDeleteReady();
	_UpdateTargetMark();
	_UpdateCharImage();
}

void CCharResMng::Render_PostTR()
{	
}

void CCharResMng::Render()
{
	g_pEffectMng->Render();

	_RenderMovePoint();	
	_RenderMonsterCreateScene();
	_RenderMouseOnObject();
	_RenderFigure();
}

void CCharResMng::Render_PostChar()
{
	_EndRenderMouseOnObject();

	// SEffectMg 외곽선 렌더
	if( g_pSEffectMgr )
		g_pSEffectMgr->OutLineRender();

	_RenderNpcMark();

	_RenderTargetMark();
}

void CCharResMng::ResetDevice()
{
	_ResetDeviceCharImage();	
}

//=====================================================================================
//
//	케릭터 제거
//
//=====================================================================================
void CCharResMng::DeleteChar( CsC_AvObject* pObject )
{
	_CheckTargetMark( pObject );

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	if( pDigimonUser != NULL )
	{
		assert_cs( pObject->IsKindOf( RTTI_AVOBJECT ) == true );
		pDigimonUser->ReleaseFollowTarget( (CsC_AvObject*)pObject );
	}
}

void CCharResMng::InsertDeleteReady( CsC_AvObject* pObject )
{ 
	m_queueDeleteReady.push( pObject ); 
}

void CCharResMng::ThreadDeleteChar( CsC_AvObject* pObject )
{
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
	pUnit->s_pLoadedObject = (CsC_Object*)pObject;
	g_pThread->DeleteChar( pUnit );
}

void CCharResMng::_UpdateDeleteReady()
{
	CsC_AvObject* pObject;
	while( m_queueDeleteReady.empty() == false )
	{
		pObject = m_queueDeleteReady.front();
		m_queueDeleteReady.pop();

		switch( pObject->GetLeafRTTI() )
		{
		case RTTI_TUTORIAL_MONSTER:
		case RTTI_MONSTER:
			g_pCharMng->DeleteMonster( pObject->GetIDX(), CsC_AvObject::DS_IMMEDEATE );
			break;
		default:
			assert_cs( false );
		}
	}
}

//=====================================================================================
//
//	마우스 온 오브젝트
//
//=====================================================================================
void CCharResMng::_UpdateMouseOnObject( cGlobalInput::eMOUSE_INPUT mi )
{
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient( GAMEAPP_ST.GetHWnd(), &pt );

	switch( mi )
	{
	case cGlobalInput::MOUSEINPUT_CAMERA:
	case cGlobalInput::MOUSEINPUT_DISABLE:
		{
			m_pMouseOnLastObject = NULL;
			_ReleaseMouseOnObject();
			CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
		}
		return;
	case cGlobalInput::MOUSEINPUT_ENABLE:
		break;
	default:
		assert_cs( false );
	}

	cType type;
	if( g_pMngCollector->PickObject( type, pt, false ) == false )
	{
		m_pMouseOnLastObject = NULL;
		_ReleaseMouseOnObject();
		CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
		return;
	}

	CsC_AvObject* pMouseOn = g_pMngCollector->GetObject( type );
	if( pMouseOn == NULL )
	{
		m_pMouseOnLastObject = NULL;
		_ReleaseMouseOnObject();
		CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
		return;
	}
#ifdef GM_CLOCKING
	if( ((CTamer*)pMouseOn)->IsClocking() )
		return;
#endif

	if( g_pResist->m_Global.s_bRightToolTip )
		TOOLTIPMNG_STPTR->GetRightTooltip()->SetRightTooltip( pMouseOn );

	CURSOR_ST.SetCursorRes( pMouseOn );
	switch( pMouseOn->GetLeafRTTI() )
	{
	case RTTI_DIGIMON:
		if( g_pResist->m_Global.s_bFigureDigimon == true )
		{
			m_pMouseOnLastObject = NULL;
			_ReleaseMouseOnObject();
			return;
		}
		break;
	case RTTI_TAMER:
		{
			CTamer* pTamer = (CTamer*)pMouseOn;
			if( ( ( pTamer->GetCondition()->IsCondition( nSync::Shop ) == true )&&
				( g_pResist->m_Global.s_bFigureEmployment == TRUE ) )||
				( g_pResist->m_Global.s_bFigureTamer == true ) )
			{
				m_pMouseOnLastObject = NULL;
				_ReleaseMouseOnObject();
				return;
			}
		}		
		break;
	case RTTI_EMPLOYMENT:
		if( g_pResist->m_Global.s_bFigureEmployment == TRUE )
		{
			m_pMouseOnLastObject = NULL;
			_ReleaseMouseOnObject();
			return;
		}
		break;
	case RTTI_MONSTER:
		{
			// 퀘스트 툴팁 체크
		}
		break;
	}

	if( pMouseOn != m_pMouseOnLastObject )
	{
		m_pMouseOnLastObject = pMouseOn;
		m_pMouseOnObject = pMouseOn;

		m_fMouseOnObjectAlpha = 0.6f;		
	}
	else if( m_pMouseOnObject != NULL )
	{		
		m_fMouseOnObjectAlpha -= 0.6f*g_fDeltaTime/3.5f;

		if( m_fMouseOnObjectAlpha <= 0.0f )
		{
			m_fMouseOnObjectAlpha = 0.0f;
			_ReleaseMouseOnObject();
		}
	}
}

void CCharResMng::_UpdateMouseOnObject_Tutorial(cGlobalInput::eMOUSE_INPUT mi)
{
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient( GAMEAPP_ST.GetHWnd(), &pt );

	switch( mi )
	{
	case cGlobalInput::MOUSEINPUT_CAMERA:
	case cGlobalInput::MOUSEINPUT_DISABLE:
		{
			m_pMouseOnLastObject = NULL;
			_ReleaseMouseOnObject();
			CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
		}
		return;
	case cGlobalInput::MOUSEINPUT_ENABLE:
		break;
	default:
		assert_cs( false );
	}

	cType type;
	if( g_pMngCollector->PickObject( type, pt, false ) == false )
	{
		m_pMouseOnLastObject = NULL;
		_ReleaseMouseOnObject();
		CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
		return;
	}

	CsC_AvObject* pMouseOn = g_pMngCollector->GetObject( type );
	if( pMouseOn == NULL )
	{
		m_pMouseOnLastObject = NULL;
		_ReleaseMouseOnObject();
		CURSOR_ST.SetCursorRes( CURSOR_RES::NORMAL );
		return;
	}
#ifdef GM_CLOCKING
	if( ((CTamer*)pMouseOn)->IsClocking() )
		return;
#endif

	switch( pMouseOn->GetLeafRTTI() )
	{
	case RTTI_TAMER_USER:
	case RTTI_TUTORIAL_TAMER:
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_TUTORIAL_MONSTER:
		break;
	default:
		return;
	}

	CURSOR_ST.SetCursorRes( pMouseOn );
	if( pMouseOn != m_pMouseOnLastObject )
	{
		m_pMouseOnLastObject = pMouseOn;
		m_pMouseOnObject = pMouseOn;

		m_fMouseOnObjectAlpha = 0.6f;
	}
	else if( m_pMouseOnObject != NULL )
	{		
		m_fMouseOnObjectAlpha -= 0.6f*g_fDeltaTime/3.5f;

		if( m_fMouseOnObjectAlpha <= 0.0f )
		{
			m_fMouseOnObjectAlpha = 0.0f;
			_ReleaseMouseOnObject();
		}
	}
}

void CCharResMng::_RenderMouseOnObject()
{
	if( m_pMouseOnObject == NULL )
		return;		
}

void CCharResMng::_EndRenderMouseOnObject()
{
	if( m_pMouseOnObject == NULL )
		return;

	if( m_fMouseOnObjectAlpha == 0.0f )
		return;

	if( nsCsGBTerrain::g_Device.g_bUseMyShader )
	{
		CsNodeObj* pCsNode = m_pMouseOnObject->GetCsNode();
		if( pCsNode == NULL )
			return;

		// 쉐이더 설정
		float afSize[4] = { m_pMouseOnObject->GetNameColor().r, m_pMouseOnObject->GetNameColor().g, m_pMouseOnObject->GetNameColor().b, m_fMouseOnObjectAlpha };
		NiD3DShaderFactory::UpdateGlobalShaderConstant( "BlurColor", sizeof(afSize), &afSize );
		pCsNode->SetShader( "Char_OutLine_Blur", "OutLine_Blur" );
		pCsNode->RenderAbsolute_ExceptPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE );
		pCsNode->ActiveShader();
	}	
}

void CCharResMng::_ReleaseMouseOnObject()
{
	m_pMouseOnObject = NULL;
}


//====================================================================================
//
//	케릭터 이미지 로드
//
//====================================================================================

#define CHAR_IMAGE_REG_CHECK_TIME		1000*60*3

void CCharResMng::_InitCharImage()
{
	m_CharImageRefCheckTimeSeq.SetDeltaTime( CHAR_IMAGE_REG_CHECK_TIME );
}

void CCharResMng::_DeleteCharImage()
{
	std::map< DWORD, sCHAR_IMAGE* >::iterator it = m_mapCharImage.begin();
	std::map< DWORD, sCHAR_IMAGE* >::iterator itEnd = m_mapCharImage.end();
	for( ;it!=itEnd; ++it )
	{
		it->second->Delete();
		NiDelete it->second;
	}
	m_mapCharImage.clear();
}

void CCharResMng::_UpdateCharImage()
{
	if( m_CharImageRefCheckTimeSeq.IsEnable() == false )
		return;

	DWORD dwCurTime = GetTickCount();
	std::map< DWORD, sCHAR_IMAGE* >::iterator it = m_mapCharImage.begin();
	std::map< DWORD, sCHAR_IMAGE* >::iterator itEnd = m_mapCharImage.end();
	for( ;it!=itEnd; ++it )
	{
		if( it->second->GetRefCount() > 0 )
			continue;

		// 일정시간이상 지났다면
		if( ( dwCurTime - it->second->GetLastTime() ) > CHAR_IMAGE_REG_CHECK_TIME )
		{
			it->second->Delete();
			NiDelete it->second;
			m_mapCharImage.erase( it );
			m_CharImageRefCheckTimeSeq.Reset();
			return;
		}
	}
}

void CCharResMng::_ResetDeviceCharImage()
{
	std::map< DWORD, sCHAR_IMAGE* >::iterator it = m_mapCharImage.begin();
	std::map< DWORD, sCHAR_IMAGE* >::iterator itEnd = m_mapCharImage.end();
	for( ;it!=itEnd; ++it )
	{
		it->second->ResetDevice();
	}
}

void CCharResMng::CharImageResDelete( sCHAR_IMAGE** ppCharImage )
{
	if( *ppCharImage == NULL )
	{
		return;
	}

	assert_cs( m_mapCharImage.find( (*ppCharImage)->s_nID ) != m_mapCharImage.end() );
	(*ppCharImage)->DecreaseRef();
	(*ppCharImage) = NULL;
}

sCHAR_IMAGE* CCharResMng::CharImageResLoad( const DWORD nModelID )
{
	if( nModelID == 0 )
		return NULL;
	DWORD RealMID = nModelID;
	if( g_pModelDataMng->IsData( nModelID ) == false )
	{
		
#ifdef COMPAT_487_2
		RealMID = 90081;
#else
		assert_csm1(false, L"nModelID = %d", nModelID);
		CsMessageBox(MB_OK, L"%d Model Is Not Exit In Model.dat", nModelID);
		return NULL;
#endif
		
	}

	sCHAR_IMAGE* pCharImg = NULL;
	// 로드된 이미지가 있는지 체크
	if( m_mapCharImage.find(RealMID) != m_mapCharImage.end() )
	{
		// 있다면 포인터 리턴
		pCharImg = m_mapCharImage[RealMID];
		pCharImg->IncreaseRef();
		return pCharImg;
	}

	// 없다면 생성
	pCharImg = NiNew sCHAR_IMAGE;
	pCharImg->s_nID = RealMID;

	std::string strLImgPath = g_pModelDataMng->GetLargeModelIconFile(RealMID);
	
	if( CsFPS::CsFPSystem::IsExist( 0, strLImgPath.c_str() ) )
	{
		pCharImg->s_pLargeImg = NiNew cSprite;
		pCharImg->s_pLargeImg->Init( NULL, CsPoint::ZERO, CsPoint( 83, 72 ), strLImgPath.c_str(), true, FONT_WHITE, false );
	}
	else
	{
		pCharImg->s_pLargeImg = NiNew cSprite;
		pCharImg->s_pLargeImg->Init( NULL, CsPoint::ZERO, CsPoint( 83, 72 ), "Data\\Dummy\\dummyL.tga", true, FONT_WHITE, false );
	}	

	std::string strSImgPath = g_pModelDataMng->GetSmallModelIconFile(RealMID);
	if( CsFPS::CsFPSystem::IsExist( 0, strSImgPath.c_str() ) )
	{
		pCharImg->s_pSmallImg = NiNew cSprite;
		pCharImg->s_pSmallImg->Init( NULL, CsPoint::ZERO, CsPoint( 50, 43 ), strSImgPath.c_str(), true, NiColor::WHITE, false );
	}
	else
	{
		pCharImg->s_pSmallImg = NiNew cSprite;
		pCharImg->s_pSmallImg->Init( NULL, CsPoint::ZERO, CsPoint( 50, 43 ), "Data\\Dummy\\dummyS.tga", true, NiColor::WHITE, false );
	}

	std::string strBossImgPath = g_pModelDataMng->GetBossModelIconFile(RealMID);
	// 보스아이콘 이미지
#ifdef _GIVE
	if( CsFPS::CsFPSystem::IsExist( 0, strBossImgPath.c_str() ) )
#endif
	{
		pCharImg->s_pBossImg = NiNew cSprite;
		pCharImg->s_pBossImg->Init( NULL, CsPoint::ZERO, CsPoint( 35, 35 ), strBossImgPath.c_str(), true, NiColor::WHITE, false );
	}

	pCharImg->IncreaseRef();
	m_mapCharImage[RealMID] = pCharImg;

	return pCharImg;
}


//=====================================================================================
//
//	디폴트 라이트
//
//=====================================================================================
void CCharResMng::_CreateDefaultLight()
{
	assert_cs( m_pDefaultLight == NULL );
	m_pDefaultLight = NiNew NiDirectionalLight;
	m_pDefaultLight->SetDiffuseColor( NiColor(0.3f, 0.3f, 0.3f) );
	m_pDefaultLight->SetAmbientColor( NiColor(0.9f, 0.9f, 0.9f) );	

	NiMatrix3 mat;
	mat.MakeYRotation( -NI_HALF_PI*0.5f );
	m_pDefaultLight->SetRotate( mat );
	m_pDefaultLight->Update( 0.0f );

	// 쓰레드 리소스 매니져에 연결
	g_pThread->GetResMng()->SetDefaultLight( m_pDefaultLight );
}

void CCharResMng::_DeleteDefaultLight()
{
	m_pDefaultLight = 0;
}


//=====================================================================================
//
//	무브 포인트
//
//=====================================================================================

void CCharResMng::_CreateMovePoint()
{
#define MOVEPOINT_OBJECT_PATH		"Data\\EtcObject\\MovePoint.nif"

	NiStream kStream;
	if( !kStream.Load( MOVEPOINT_OBJECT_PATH ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 무브포인트 오브젝트를 찾지 못했습니다.", MOVEPOINT_OBJECT_PATH );
		return;
	}

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_MovePoint.SetNiObject( pNode, CGeometry::Normal );

	m_bMovePointRender = false;
}

void CCharResMng::_DeleteMovePoint()
{
	m_MovePoint.Delete();
}

void CCharResMng::_RenderMovePoint()
{
	if( m_bMovePointRender == false )
		return;

	m_MovePoint.m_pNiNode->SetTranslate( m_vMovePoint );
	m_MovePoint.m_pNiNode->Update( (float)g_fAccumTime );
	m_MovePoint.Render();
}

void CCharResMng::SetMovePoint( NiPoint3 vMovePoint )
{
	m_vMovePoint = vMovePoint;
	m_bMovePointRender = true;
	NiTimeController::StopAnimations( m_MovePoint.m_pNiNode );
	NiTimeController::StartAnimations( m_MovePoint.m_pNiNode, (float)g_fAccumTime );
}

void CCharResMng::ReleaseMovePoint()
{ 
	m_bMovePointRender = false; 
	NiTimeController::StopAnimations( m_MovePoint.m_pNiNode );
}

void CCharResMng::SetEnableMovePoint( bool bEnable )
{ 
	m_bEnableMovePoint = bEnable; 
}

bool CCharResMng::IsEnableMovePoint()
{ 
	return m_bEnableMovePoint; 
}

//=====================================================================================
//
//	타겟팅
//
//=====================================================================================

void CCharResMng::_CreateTargetMark()
{
#define TARGETMARK1_OBJECT_PATH		"Data\\EtcObject\\TargetMark_Green.nif"
#define TARGETMARK2_OBJECT_PATH		"Data\\EtcObject\\TargetMark_Red.nif"
#define TARGETMARK3_OBJECT_PATH		"Data\\EtcObject\\TargetMark_Blue.nif"

	{
		NiStream kStream;
		if( !kStream.Load( TARGETMARK1_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 타겟팅 오브젝트를 찾지 못했습니다.", TARGETMARK1_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_TargetMarkG.SetNiObject( pNode, CGeometry::Normal );
		m_TargetMarkG.m_pNiNode->Update( 0.0f );
	}

	{
		NiStream kStream;
		if( !kStream.Load( TARGETMARK2_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 타겟팅 오브젝트를 찾지 못했습니다.", TARGETMARK2_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_TargetMarkR.SetNiObject( pNode, CGeometry::Normal );
		m_TargetMarkR.m_pNiNode->Update( 0.0f );
	}

	{
		NiStream kStream;
		if( !kStream.Load( TARGETMARK3_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 타겟팅 오브젝트를 찾지 못했습니다.", TARGETMARK3_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_TargetMarkB.SetNiObject( pNode, CGeometry::Normal );
		m_TargetMarkB.m_pNiNode->Update( 0.0f );
	}

	m_pTargetObject = NULL;
}

void CCharResMng::_DeleteTargetMark()
{
	m_TargetMarkG.Delete();
	m_TargetMarkR.Delete();
	m_TargetMarkB.Delete();
	m_pTargetObject = NULL;
}

void CCharResMng::SetTargetMark( CsC_AvObject* pObject, bool bOnEvent )
{
	m_pTargetObject = pObject;

	if( m_pTargetObject )
	{
		if( bOnEvent )
		{
			UINT nTargetUIDX = m_pTargetObject->GetUniqID();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_TARGET_MARK, &nTargetUIDX );
		}
	}
	
	switch( m_pTargetObject->GetClass() )
	{
	case nClass::Monster:
		m_eTargetType = TMT_RED;
		m_fKeepCenterHeight = m_pTargetObject->GetCenter().z - m_pTargetObject->GetPos().z;
		break;
	
	case nClass::Digimon:
		{
#ifdef GM_CLOCKING
			if(( (CDigimon*)m_pTargetObject )->IsClocking() )
			{
				m_eTargetType = TMT_NONE;
				m_pTargetObject = NULL;
			}
			else
#endif
			switch( nsCsGBTerrain::g_nSvrLibType )
			{
			case nLIB::SVR_DUNGEON:
			case nLIB::SVR_GAME:
				m_eTargetType = TMT_GREEN;
				break;
			case nLIB::SVR_BATTLE:
				// 같은 팀이라면
				if( ( (CDigimon*)m_pTargetObject )->GetBattleTeam() == g_pCharMng->GetDigimonUser( 0 )->GetBattleTeam() )
					m_eTargetType = TMT_GREEN;
				else
					m_eTargetType = TMT_RED;
				break;
			default:
				assert_cs( false );
			}
		}		
		break;
	case nClass::Tamer:
#ifdef GM_CLOCKING
		if(( (CTamer*)m_pTargetObject )->IsClocking() )
		{
			m_eTargetType = TMT_NONE;
			m_pTargetObject = NULL;
		}
		else
			m_eTargetType = TMT_GREEN;
		break;
#endif
	case nClass::Npc:
	case nClass::CommissionShop:
		m_eTargetType = TMT_GREEN;
		break;
	default:
		m_eTargetType = TMT_NONE;
		m_pTargetObject = NULL;
	}	
}

void CCharResMng::ReleaseTargetMark( bool bAbsoluteRelease )
{
	if( m_pTargetObject != NULL )
	{
		UINT nTargetUIDX = m_pTargetObject->GetUniqID();
		GAME_EVENT_ST.OnEvent( EVENT_CODE::RELEASE_TARGET_MARK, &nTargetUIDX );
		GLOBALINPUT_ST.ReleasePickData( m_pTargetObject );
	}
	m_pTargetObject = NULL;
}

void CCharResMng::_UpdateTargetMark()
{
	if( m_pTargetObject == NULL )
		return;

	// 거리 바깥으로 나갔다면
	if( m_pTargetObject->GetProp_Alpha()->IsHideDistOut() == true )
	{
		ReleaseTargetMark( false );
		return;
	}

	// 타겟별 처리
	switch( m_pTargetObject->GetLeafRTTI() )
	{
	case RTTI_NPC:
		if( ( (CNpc*)m_pTargetObject )->IsEnableUse() == false )
		{
			ReleaseTargetMark( false );
			return;
		}
		break;
	}
}

void CCharResMng::_RenderTargetMark()
{
	if( m_pTargetObject == NULL )
		return;

	switch( m_pTargetObject->GetLeafRTTI() )
	{
	case RTTI_TUTORIAL_MONSTER:
	case RTTI_MONSTER:
		{
			// 죽을는 타겟팅 마크 안보이게
			if( ((CMonster*)m_pTargetObject)->GetMonsterState() == CMonster::MONSTER_DIE )
			{
				return;
			}
		}
		break;
	}

	NiPoint3 vPos;
	CsNodeObj* pRenderNode;

	float fScale = sqrt( m_pTargetObject->GetToolWidth()*0.025f );
	switch( m_eTargetType )
	{
	case TMT_GREEN:
		pRenderNode = &m_TargetMarkG;
		vPos = m_pTargetObject->GetPos();
		pRenderNode->m_pNiNode->SetTranslate( vPos );
		pRenderNode->m_pNiNode->SetScale( fScale );
		pRenderNode->m_pNiNode->Update( (float)g_fAccumTime );

		break;
	case TMT_RED:
		{
			switch( nsCsGBTerrain::g_nSvrLibType )
			{
			case nLIB::SVR_DUNGEON:
			case nLIB::SVR_GAME:
				if( g_pCharMng->GetDigimonUser( 0 )->GetProp_Attack()->GetFollowTarget() == m_pTargetObject )
				{
					pRenderNode = &m_TargetMarkR;			
					vPos = m_pTargetObject->GetPos();

					pRenderNode->m_pNiNode->SetTranslate( vPos );
					pRenderNode->m_pNiNode->SetScale( fScale );
					pRenderNode->m_pNiNode->Update( (float)g_fAccumTime );
				}
				else
				{
					pRenderNode = &m_TargetMarkB;
					vPos = m_pTargetObject->GetPos();
#ifdef MONSTER_SKILL_GROWTH
					m_fKeepCenterHeight = m_pTargetObject->GetCenter().z - m_pTargetObject->GetPos().z;
#endif
					vPos.z += m_fKeepCenterHeight;

					pRenderNode->m_pNiNode->SetTranslate( vPos );
					pRenderNode->m_pNiNode->SetScale( fScale );
					pRenderNode->m_pNiNode->Update( (float)g_fAccumTime );
					CsGBVisible::OnVisible( (CsNodeObj*)pRenderNode, pRenderNode->m_pNiNode, CsGBVisible::VP_BILLBOARD, (float)g_fAccumTime );
				}
				break;
			case nLIB::SVR_BATTLE:
				{
					pRenderNode = &m_TargetMarkR;
					vPos = m_pTargetObject->GetPos();
					pRenderNode->m_pNiNode->SetTranslate( vPos );
					pRenderNode->m_pNiNode->SetScale( fScale );
					pRenderNode->m_pNiNode->Update( (float)g_fAccumTime );
				}
				break;
			default:
				assert_cs( false );
			}			
		}
		break;
	default:
		assert_cs( false );
	}	
	
	if( CAMERA_ST.UserCulling( pRenderNode->m_pNiNode ) == true )
	{		
		pRenderNode->RenderAbsolute();
	}
}

void CCharResMng::_CheckTargetMark( CsC_Object* pObject )
{
	if( m_pTargetObject == pObject )
		ReleaseTargetMark( false );
}


//=====================================================================================
//
//	Npc 마크
//
//=====================================================================================

CSGBMEMPOOL_CPP( CCharResMng::sNPC_MARK );

void CCharResMng::_DeleteNpcMark()
{
	m_NpcMarkTrain.Delete();
	m_NpcMarkTalk.Delete();
	m_NpcMarkRecv_Main.Delete();
	m_NpcMarkRecv_Sub.Delete();
	m_NpcMarkRecv_EventRepeat.Delete();
	m_NpcMarkRecv_Repeat.Delete();
	m_NpcMarkLevelSoon.Delete();
	m_NpcMarkQustionG.Delete();
	m_NpcMarkQustionR.Delete();
	m_NpcMarkPortal.Delete();
	assert_cs( m_vpNpcMark.Size() == 0 );
	m_vpNpcMark.Destroy();
	CSGBMEMPOOL_DELETE( CCharResMng::sNPC_MARK );
}

void CCharResMng::_CreateNpcMark()
{
	CSGBMEMPOOL_INIT( CCharResMng::sNPC_MARK, 8 );

	//=============================================================================
	//
	//		포탈
	//
	//=============================================================================
	{
#define MARK_TRAIN_OBJECT_PATH		"Data\\EtcObject\\Mark_Train.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_TRAIN_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_TRAIN_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkTrain.SetNiObject( pNode, CGeometry::Normal );
	}

	{
#define MARK_PORTAL_OBJECT_PATH		"Data\\EtcObject\\Mark_Normal.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_PORTAL_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_PORTAL_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkPortal.SetNiObject( pNode, CGeometry::Normal );
	}


	//=============================================================================
	//
	//		대화
	//
	//=============================================================================
	{
#define MARK_TALK_OBJECT_PATH		"Data\\EtcObject\\Mark_Talk.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_TALK_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_TALK_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkTalk.SetNiObject( pNode, CGeometry::Normal );
	}


	//=============================================================================
	//
	//		퀘스트 아직 못받음 ( 회색 느낌표 )
	//
	//=============================================================================
	{
#define MARK_EXCLAM_G_OBJECT_PATH		"Data\\EtcObject\\Mark_QT_LevelSoon.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_EXCLAM_G_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_EXCLAM_G_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkLevelSoon.SetNiObject( pNode, CGeometry::Normal );
	}

	//=============================================================================
	//
	//		메인 퀘스트 받음 ( 보라색 느낌표 )
	//
	//=============================================================================
	{
#define MARK_QT_RECV_MAIN		"Data\\EtcObject\\Mark_QT_RecvMain.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_QT_RECV_MAIN ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_QT_RECV_MAIN );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkRecv_Main.SetNiObject( pNode, CGeometry::Normal );
	}


	//=============================================================================
	//
	//		서브 퀘스트 받음 ( 녹색 느낌표 )
	//
	//=============================================================================
	{
#define MARK_QT_RECV_SUB			"Data\\EtcObject\\Mark_QT_RecvSub.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_QT_RECV_SUB ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_QT_RECV_SUB );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkRecv_Sub.SetNiObject( pNode, CGeometry::Normal );
	}

	//=============================================================================
	//
	//		이벤트 퀘스트 받음 ( 파랑 느낌표 )
	//
	//=============================================================================
	{
#define MARK_QT_RECV_EVENT_REPEAT		"Data\\EtcObject\\Mark_QT_Event.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_QT_RECV_EVENT_REPEAT ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_QT_RECV_EVENT_REPEAT );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkRecv_EventRepeat.SetNiObject( pNode, CGeometry::Normal );
	}


	//=============================================================================
	//
	//		반복 퀘스트 받음 ( 파랑 느낌표 )
	//
	//=============================================================================
	{
#define MARK_QT_RECV_REPEAT		"Data\\EtcObject\\Mark_QT_RecvRepeat.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_QT_RECV_REPEAT ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_QT_RECV_REPEAT );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkRecv_Repeat.SetNiObject( pNode, CGeometry::Normal );
	}

	//=============================================================================
	//
	//		회색 물음표
	//
	//=============================================================================
	{		
#define MARK_QUESTION_G_OBJECT_PATH		"Data\\EtcObject\\Mark_Qustion_G.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_QUESTION_G_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_QUESTION_G_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkQustionG.SetNiObject( pNode, CGeometry::Normal );
	}

	//=============================================================================
	//
	//		빨간 물음표
	//
	//=============================================================================
	{		
#define MARK_QUESTION_R_OBJECT_PATH		"Data\\EtcObject\\Mark_Qustion_R.nif"
		NiStream kStream;
		if( !kStream.Load( MARK_QUESTION_R_OBJECT_PATH ) )
		{
			CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", MARK_QUESTION_R_OBJECT_PATH );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_NpcMarkQustionR.SetNiObject( pNode, CGeometry::Normal );
	}
}

void CCharResMng::NpcMarkInsert( int nNpcIDX, NiPoint3 vPos, cData_QuestOwner::sNpcOwner::eREV_TYPE eDispType )
{
	assert_cs( nNpcIDX < NPC_FT_MASK );

	sNPC_MARK* pNpcMark = sNPC_MARK::NewInstance();
	pNpcMark->s_nNpcIDX = nNpcIDX;
	pNpcMark->s_vPos = vPos;	
	pNpcMark->s_eDiplayType = eDispType;

	m_vpNpcMark.PushBack( pNpcMark );
}

void CCharResMng::_RenderNpcMark()
{
	CsNodeObj* pRenderNode = NULL;
	sNPC_MARK* pNpcMark = NULL;
	int nCount = m_vpNpcMark.Size();	
	for( int i=0; i<nCount; ++i )
	{
		pNpcMark = m_vpNpcMark[ i ];
		float fBaseScale = 1.0f;
		switch( pNpcMark->s_eDiplayType )
		{
		case cData_QuestOwner::sNpcOwner::TRAIN:					pRenderNode = &m_NpcMarkTrain;		fBaseScale = 2.0f;		break;
		case cData_QuestOwner::sNpcOwner::NORMAL:					pRenderNode = &m_NpcMarkPortal;		fBaseScale = 2.0f;		break;
		case cData_QuestOwner::sNpcOwner::TALK:						pRenderNode = &m_NpcMarkTalk;								break;
		case cData_QuestOwner::sNpcOwner::REQUITE:					pRenderNode = &m_NpcMarkQustionR;							break;
		case cData_QuestOwner::sNpcOwner::PROCESS:					pRenderNode = &m_NpcMarkQustionG;							break;		
		case cData_QuestOwner::sNpcOwner::SOON_REV_LEVEL:			pRenderNode = &m_NpcMarkLevelSoon;							break;
		case cData_QuestOwner::sNpcOwner::ENABLE_REV_MAIN:			pRenderNode = &m_NpcMarkRecv_Main;							break;
		case cData_QuestOwner::sNpcOwner::ENABLE_REV_SUB:			pRenderNode = &m_NpcMarkRecv_Sub;							break;
		case cData_QuestOwner::sNpcOwner::ENABLE_REV_REPEAT:		pRenderNode = &m_NpcMarkRecv_Repeat;						break;
		case cData_QuestOwner::sNpcOwner::ENABLE_REV_EVENTREPEAT:	pRenderNode = &m_NpcMarkRecv_EventRepeat;					break;
		default:
			assert_csm1( false, _T( "디스플레이 타입 미스 : type - %d" ), pNpcMark->s_eDiplayType );
			continue;
		}

		NiPoint3 pos = pNpcMark->s_vPos;
		float fLength = ( CAMERA_ST.GetWorldTranslate() - pos ).Length();
		float fScale = CCharOption::GetNameScaleConstant()*pow( fLength*fLength*15.0f, 1.0f/3.0f )*0.36f * fBaseScale;

		if( fScale > 1.0f )
			pos.z += 90.0f + 40.0f*sqrt( fScale );
		else
			pos.z += 90.0f + 40.0f*fScale;
		pRenderNode->m_pNiNode->SetScale( fScale );
		pRenderNode->m_pNiNode->SetTranslate( pos );
		pRenderNode->m_pNiNode->Update( (float)g_fAccumTime );

		CsGBVisible::OnVisible( pRenderNode, pRenderNode->m_pNiNode, CsGBVisible::VP_BILLBOARD, (float)g_fAccumTime );
		if( CAMERA_ST.UserCulling( pRenderNode->m_pNiNode ) )
		{
			pRenderNode->RenderAbsolute();
		}
	}

	// 클리어 시켜 주자
	for( int i=0; i<nCount; ++i )
	{
		sNPC_MARK::DeleteInstance( m_vpNpcMark[ i ] );
	}
	m_vpNpcMark.ClearElement();
}

//=====================================================================================
//
//	몬스터 생성 원
//
//=====================================================================================

void CCharResMng::_DeleteMonsterCreateScene()
{
	m_MonsterCreateScene.Delete();

	std::list< sMONSTER_CREATE_SCENE_INFO* >::iterator it = m_listMonsterCreateSceneInfo.begin();
	std::list< sMONSTER_CREATE_SCENE_INFO* >::iterator itEnd = m_listMonsterCreateSceneInfo.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete *it;
	}
	m_listMonsterCreateSceneInfo.clear();

	sMONSTER_CREATE_SCENE_INFO* pDelInfo;
	while( m_stackMonsterCreateScenePool.empty() == false )
	{
		pDelInfo = m_stackMonsterCreateScenePool.top();
		NiDelete pDelInfo;
		m_stackMonsterCreateScenePool.pop();
	}
}

void CCharResMng::_CreateMonsterCreateScene()
{
	NiStream kStream;
	if( !kStream.Load( EFFECT_GATE ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", EFFECT_GATE );
		return;
	}
	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	m_MonsterCreateScene.SetNiObject( pNode, CGeometry::Normal );

	nsCSGBFUNC::EndAnimationTime( m_fMonsterCreateSceneAniLength, m_MonsterCreateScene.m_pNiNode );
}

void CCharResMng::_ResetMonsterCreateScene()
{
	std::list< sMONSTER_CREATE_SCENE_INFO* >::iterator it = m_listMonsterCreateSceneInfo.begin();
	std::list< sMONSTER_CREATE_SCENE_INFO* >::iterator itEnd = m_listMonsterCreateSceneInfo.end();
	for( ; it!=itEnd; ++it )
	{
		m_stackMonsterCreateScenePool.push( *it );
	}
	m_listMonsterCreateSceneInfo.clear();
}

void CCharResMng::_RenderMonsterCreateScene()
{
	std::list< sMONSTER_CREATE_SCENE_INFO* >::iterator it = m_listMonsterCreateSceneInfo.begin();
	std::list< sMONSTER_CREATE_SCENE_INFO* >::iterator itEnd = m_listMonsterCreateSceneInfo.end();
	while( it!=itEnd )
	{
		if( ( (*it)->s_fAniTime += g_fDeltaTime ) < m_fMonsterCreateSceneAniLength )
		{
			m_MonsterCreateScene.m_pNiNode->SetTranslate( (*it)->s_vPos );
			m_MonsterCreateScene.m_pNiNode->SetScale( (*it)->s_fScale );
			m_MonsterCreateScene.m_pNiNode->Update( (*it)->s_fAniTime, true );
			m_MonsterCreateScene.RenderAbsolute();
			++it;
		}
		else
		{
			m_stackMonsterCreateScenePool.push( *it );
			it = m_listMonsterCreateSceneInfo.erase( it );
		}
	}
}

void CCharResMng::MonsterCreateSceneInsert( NiPoint3 vPos, float fScale, CsC_AvObject* pTarget )
{
	sMONSTER_CREATE_SCENE_INFO* pInfo = NULL;
	if( m_stackMonsterCreateScenePool.empty() == false )
	{
		pInfo = m_stackMonsterCreateScenePool.top();
		m_stackMonsterCreateScenePool.pop();
	}
	else
	{
		pInfo = NiNew sMONSTER_CREATE_SCENE_INFO;
	}
	pInfo->s_vPos = vPos;
	pInfo->s_fAniTime = 0.0f;
	pInfo->s_fScale = fScale;

	m_listMonsterCreateSceneInfo.push_back( pInfo );
	g_pSoundMgr->PlaySound( SOUND_GATE, vPos, pTarget );
}


//=====================================================================================
//
//	피규어 이미지
//
//=====================================================================================

void CCharResMng::_DeleteFigure()
{
	m_FigureTamerNode.Delete();
	m_FigureDigimonNode.Delete();
	m_FigureEmployment.Delete();
	m_FigurePatNode.Delete();
	int nCount = m_vpFigure.Size();
	for( int i=0; i<nCount; ++i )
	{
		NiDelete m_vpFigure[ i ];
	}
	m_vpFigure.Destroy();

	sFIGURE_INFO* pDelInfo;
	while( m_stackFigurePool.empty() == false )
	{
		pDelInfo = m_stackFigurePool.top();
		NiDelete pDelInfo;
		m_stackFigurePool.pop();
	}
}

void CCharResMng::_CreateFigure()
{
	{
		NiStream kStream;
		if( !kStream.Load( "Data\\EtcObject\\FigureTamer.nif" ) )
		{
			CsMessageBoxA( MB_OK, "Data\\EtcObject\\FigureTamer.nif 경로\n에서 오브젝트를 찾지 못했습니다." );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_FigureTamerNode.SetNiObject( pNode, CGeometry::Normal );
	}

	{
		NiStream kStream;
		if( !kStream.Load( "Data\\EtcObject\\FigureDigimon.nif" ) )
		{
			CsMessageBoxA( MB_OK, "Data\\EtcObject\\FigureDigimon.nif 경로\n에서 오브젝트를 찾지 못했습니다." );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_FigureDigimonNode.SetNiObject( pNode, CGeometry::Normal );
	}
	{
		NiStream kStream;
		if( !kStream.Load( "Data\\EtcObject\\FigureDigimon.nif" ) )
		{
			CsMessageBoxA( MB_OK, "Data\\EtcObject\\FigureDigimon.nif 경로\n에서 오브젝트를 찾지 못했습니다." );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_FigurePatNode.SetNiObject( pNode, CGeometry::Normal );
		
	}
	{
		NiStream kStream;
		if( !kStream.Load( "Data\\EtcObject\\FigureEmployment.nif" ) )
		{
			CsMessageBoxA( MB_OK, "Data\\EtcObject\\FigureEmployment.nif 경로\n에서 오브젝트를 찾지 못했습니다." );
			return;
		}
		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		m_FigureEmployment.SetNiObject( pNode, CGeometry::Normal );
		m_FigureEmployment.m_pNiNode->SetScale( 0.7f );
	}
}

void CCharResMng::_ResetFigure()
{
	int nCount = m_vpFigure.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_stackFigurePool.push( m_vpFigure[ i ] );
	}
	m_vpFigure.ClearElement();
}

void CCharResMng::_PostLoadMapFigure()
{
	m_FigureTamerNode.m_pNiNode->DetachAllEffects();
	m_FigureDigimonNode.m_pNiNode->DetachAllEffects();
	m_FigureEmployment.m_pNiNode->DetachAllEffects();
	m_FigurePatNode.m_pNiNode->DetachAllEffects();
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyChar( m_FigureTamerNode.m_pNiNode );
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyChar( m_FigureDigimonNode.m_pNiNode );

	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyChar( m_FigureEmployment.m_pNiNode );
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyChar( m_FigurePatNode.m_pNiNode );
	m_FigureTamerNode.m_pNiNode->UpdateEffects();
	m_FigureDigimonNode.m_pNiNode->UpdateEffects();

	m_FigureEmployment.m_pNiNode->UpdateEffects();
	m_FigurePatNode.m_pNiNode->UpdateEffects();
}

void CCharResMng::_RenderFigure()
{
	CsNodeObj* pCsNode;
	sFIGURE_INFO* pInfo;

	int nCount = m_vpFigure.Size();
	for( int i=0; i<nCount; ++i )
	{
		pInfo = m_vpFigure[ i ];
		switch( pInfo->s_nType )
		{
		case 0:			pCsNode = &m_FigureTamerNode;			break;
		case 1:			pCsNode = &m_FigureDigimonNode;			break;
		case 2:			pCsNode = &m_FigureEmployment;			break;
		case 3:			pCsNode = &m_FigurePatNode;				break;
		default:		assert_cs( false );
		}
		pCsNode->m_pNiNode->SetTranslate( pInfo->s_vPos );
		pCsNode->m_pNiNode->SetRotate( pInfo->s_fRot, 0, 0, 1 );
		pCsNode->m_pNiNode->Update( pInfo->s_fAniTime );
		pCsNode->RenderAbsolute();

		m_stackFigurePool.push( pInfo );
	}
	m_vpFigure.ClearElement();
}

void CCharResMng::FigureInsert( NiPoint3 vPos, float fRot, float fAniTime, sFIGURE_INFO::eFIGURETYPE eFigureType )
{
	sFIGURE_INFO* pInfo = NULL;
	if( m_stackFigurePool.empty() == false )
	{
		pInfo = m_stackFigurePool.top();
		m_stackFigurePool.pop();
	}
	else
	{
		pInfo = NiNew sFIGURE_INFO;
	}
	pInfo->s_nType = eFigureType;
	pInfo->s_vPos = vPos;
	pInfo->s_fRot = fRot;
	pInfo->s_fAniTime = fAniTime;	

	m_vpFigure.PushBack( pInfo );	
}

