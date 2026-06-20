#include "StdAfx.h"
#include "EventScene.h"

#define FILENAME		L"ToolData\\TempData.dat"
#define EVENTDATADIR	L"ToolData\\EventData\\"

CEventScene g_EventScene;

CEventScene::CEventScene(void)
{
}

CEventScene::~CEventScene(void)
{
}

void CEventScene::Delete()
{	
	if( g_pEventDataMng != NULL )
	{
		g_pEventDataMng->Delete();
		SAFE_NIDELETE( g_pEventDataMng );
	}
	if( g_EffectSoundMng != NULL )
	{
		g_EffectSoundMng->Delete();
		SAFE_NIDELETE( g_EffectSoundMng );
	}
	
	DeleteTempEventAll();
	
	_DeleteMovePoint();
	_DeleteAreaPoint();
	_DeleteEffectPoint();

	g_pTalkBallone->PreResetMap();
	cTalkBalloon::GlobalShotDown();

	cStringAnalysis::GlobalShotDown();
	cString::GlobalShotDown();

	DeleteFont();
	CFreeFont::GlobalRelease();		

	cWindow::GlobalShotDown();
	
	// Npc 매니져
	if( g_pNpcMng != NULL )
	{
		g_pNpcMng->Delete();
		SAFE_NIDELETE( g_pNpcMng );
	}

	DeleteCameraTarget();	
	EndEvent();

	if( m_pEventCall != NULL )
	{
		SAFE_NIDELETE( m_pEventCall );
	}
	
}

void CEventScene::DeleteCameraTarget()
{
	if( m_pCameraTarget != NULL )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_pCameraTarget;	

		g_pThread->DeleteChar( pUnit );
		m_pCameraTarget = NULL;
	}
}	

void CEventScene::DeleteEventTarget()
{
	if( m_pEventTarget != NULL )
	{
		if( m_pEventTarget->IsProperty( CsC_Property::MOVE_CONTROL ) == true )
			m_pEventTarget->GetProp_MoveControl()->SetFTEventMoveObject( NULL );

		if( m_pEventData->GetObjectInfo()->s_sType != CEventData::EO_Npc )
		{
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = m_pEventTarget;	

			g_pThread->DeleteChar( pUnit );
		}				
		m_pEventTarget = NULL;	
	}
}

void CEventScene::Init()
{	
	// 이벤트 데이터 로드
	g_pEventDataMng = NiNew CEventDataMng;
	g_pEventDataMng->Init();

	// 이펙트 데이터 로드
	g_EffectSoundMng = NiNew CsEffectSoundMng;
	g_EffectSoundMng->Init();		

	_LoadTempEventALL();

	//카메라 셋팅	
	sCAMERAINFO info;
	info.s_fFarPlane = 100000.0f;
	info.s_fInitPitch = -NI_HALF_PI;
	info.s_fInitRoll = 0.0f;	
	info.s_fDist = 400.0f;

	CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( 1 )->GetInfo();	
	
	g_Camera.Create( g_pRenderer, &info );		
	g_Camera.SetTranslate( NiPoint3( (float)pListInfo->s_nWidth/2, (float)pListInfo->s_nHeight/2, 15000 ) );

	_CreateMovePoint();
	_CreateAreaPoint();
	_CreateEffectPoint();	

	cWindow::GlobalInit();
	cStringAnalysis::GlobalInit();
	cString::GlobalInit();
	CFreeFont::GlobalInit();
	CreateFont();	
	
	g_pNpcMng = NiNew CNpcMng;
	g_pNpcMng->Init();
	g_pNpcMng->LoadMap( g_Terrain.m_dwMapId );

	cTalkBalloon::GlobalInit();	

	ResetFreeCamera();
	ReSetPlayTime();

	m_pEventCall = NiNew CEventCall;
	g_pEventCall = m_pEventCall;	

	m_pMoveProp = NULL;
	m_pEventData = NULL;
}

void CEventScene::ResetFreeCamera( bool bResetPos )
{
	g_Camera.SetDAAConstant( CAMERA_DIST_ANI_ACCEL_CONSTANT_TOOL );
	g_Camera.SetDistAccel( 3.5f );	
	g_Camera.ReleaseDistRange();	
	g_Camera.ReleaseRotationLimit();
	g_Camera.SetTranslateAccel( 3.5f );	
	g_Camera.SetDeltaHeight( 2000.0f );			
	g_Camera.SetAttachActor_ByTool( false );

	if( bResetPos )
	{
		CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( g_Terrain.m_dwMapId )->GetInfo();	
		g_Camera.SetTranslate( NiPoint3( (float)pListInfo->s_nWidth/2, (float)pListInfo->s_nHeight/2, 15000 ) );
	}		

	// Fog
	nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( false );
}

void CEventScene::ResetCharCamera()
{
	nsCsGBTerrain::g_pCurRoot->SetViewerPos_Tool( m_pCameraTarget->GetPos() );

	g_Camera.SetDAAConstant( CAMERA_DIST_ANI_ACCEL_CONSTANT_TOOL );
	g_Camera.SetDistAccel( 1.5f );
	g_Camera.SetDistRange( 250.0f, 3500.0f );
	g_Camera.SetRotationLimit( -CsD2R( 40 ), CsD2R( 70 ) );
	g_Camera.SetTranslateAccel( 0.0f );	
	g_Camera.SetDeltaHeight( 120.0f );		

	// Camera
	g_Camera.SetAttachActor_ByTool( true );	

	// Fog
	nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( true );
}

void CEventScene::SetCameraTargetPos( NiPoint3 vPos )
{
	//캐릭터 카메라모드 생성
	m_pCameraTarget = CsC_PartObject::NewInstance(); 

	sCREATEINFO Prop;
	Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE | PR_PATH_NORMAL;
	Prop.s_dwOptionPlag = _OP_ALL;
	Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CsC_PartObject::sCHANGE_PART_INFO pi[ nsPART::MAX_TOTAL_COUNT ];
	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
		pi[ i ].s_nPartIndex = i;

	( (CsC_PartObject*)m_pCameraTarget )->Init( 1, 80002, vPos, 0.0f, &Prop );	
	m_pCameraTarget->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );		

	( (CsC_PartObject*)m_pCameraTarget )->ThreadLoad_Part( pi, true );
	
}

void CEventScene::SetEventTarget()
{
	assert_cs( m_pEventData != NULL );

	//셋팅값
	DWORD dwModelID = m_pEventData->GetObjectInfo()->s_dwEventObjectID;
	float fRot = m_pEventData->GetObjectInfo()->s_fRot;	
	float fScale = m_pEventData->GetObjectInfo()->s_fScale;
	NiPoint3 vPos = m_pEventData->GetObjectInfo()->s_vPos + m_pEventData->GetObjectInfo()->s_vOffset;	

	switch( m_pEventData->GetObjectInfo()->s_sType )
	{
	case CEventData::EO_New:
		{
			if( dwModelID / 10000 == 8 )
			{			
				//신규캐릭터 생성
				CsC_EventTamerObject* pEvent = CsC_EventTamerObject::NewInstance();	
				pEvent->Init( 1, dwModelID, vPos, fRot, NULL );			
				pEvent->SetScale( fScale );
				pEvent->m_eObjectState = CsC_EventTamerObject::OBJECT_NORMAL;

				m_pEventTarget = pEvent;
			}
			else
			{
				// 로드시 필요한 초기화
				sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File );

				//신규캐릭터 생성
				CsC_EventObject* pTarget = CsC_EventObject::NewInstance();

				// 초기화 - Prop 초기화 포함				
				pUint->s_Prop.s_eInsertGeomType	= CGeometry::GetCharOutLineType();		
				pUint->s_Prop.s_dwPropPlag		= PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE;
				pUint->s_Prop.s_dwOptionPlag	= _OP_ALL;

				strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, g_pModelDataMng->GetData( dwModelID )->GetInfo()->s_cKfmPath );

				pTarget->Init( 1, dwModelID, vPos, fRot, &pUint->s_Prop );	
				pTarget->SetScale( fScale );
				pTarget->m_eObjectState = CsC_EventObject::OBJECT_NORMAL;

				pTarget->SetName( m_pEventData->GetObjectInfo()->s_strName );

				// 로드 설정
				pUint->s_pLoadedObject = (CsC_EventObject*)pTarget;	

				// 쓰레드 로드	
				g_pThread->LoadChar( pUint );

				m_pEventTarget = pTarget;
			}		
		}
		break;
	case CEventData::EO_Npc:
		{
			m_pEventTarget = g_pNpcMng->GetNpc( m_pEventData->GetObjectInfo()->s_dwEventObjectID );
		}
		break;
	}	

	m_fEndTime = m_pEventData->GetEventInfo()->s_fEventTime;
}

void CEventScene::Update()
{	
	g_pNpcMng->Update();
	g_pTalkBallone->Update();	

	if( g_pMainWnd->_GetDlgController()->_IsPlay() )
	{
		assert_cs( m_pEventData != NULL );
		assert_cs( m_pEventTarget != NULL );
		m_fPlayTime += g_fDeltaTime;

		if( m_pEventData->GetObjectInfo()->s_sType == CEventData::EO_New )
			m_pEventTarget->Update( g_fDeltaTime ); 
		
		if( PlayEvent() == false )
		{
			g_pMainWnd->_GetDlgController()->_SetPlay( false );
			EndEvent();		
		}			
	}		
	
	if( g_pMainWnd->_GetDlgController()->_IsChrCamera() || g_pMainWnd->_GetDlgController()->_IsDirectPlay() )
	{
		g_pEventSceneMng->Update( g_fDeltaTime );
		
		if( g_pMainWnd->_GetDlgController()->_IsChrCamera() && (m_pCameraTarget != NULL) )
		{
			g_pEventSceneMng->EventCheckCondition( m_pCameraTarget->GetPos2D(), g_Terrain.m_dwMapId );
		}		

		if( g_pMainWnd->_GetDlgController()->_IsChrCamera() && !g_pMainWnd->_GetDlgController()->m_bCharCamera )
		{		
			m_pCameraTarget->Update( g_fDeltaTime );
			m_pCameraTarget->SetDefault_IdleAnimation();
			g_Camera.SetTranslate( m_pCameraTarget->GetPos() );	
		}	
	}	
}

void CEventScene::Render()
{
	m_matView = g_pRenderer->GetD3DView();
	m_matProj = g_pRenderer->GetD3DProj();
	D3DXMatrixMultiply( &m_matViewProj, &m_matView, &m_matProj );
	g_pRenderer->GetD3DDevice()->GetViewport( &m_ViewPort );

	g_pNpcMng->Render();		
	_RenderMovePoint();

	if( g_pMainWnd->_GetDlgController()->_IsPlay() )
	{
		if( m_pEventTarget )
			m_pEventTarget->Render();
	}
	
	if( g_pMainWnd->_GetDlgController()->_IsChrCamera() || g_pMainWnd->_GetDlgController()->_IsDirectPlay() )
	{
		g_pEventSceneMng->Render();

		if( g_pMainWnd->_GetDlgController()->_IsChrCamera() && !g_pMainWnd->_GetDlgController()->m_bCharCamera )
		{
			m_pCameraTarget->Render();
			_RenderPlayAreaPoint();
		}		
	}	
	else if( g_pMainWnd->_GetDlgController()->m_bSetArea && g_pMainWnd->_GetDlgController()->_IsArea() )
	{		
		_RenderAreaPoint();		
	}
	
	_RenderEffectPoint();
}

bool CEventScene::PlayEvent()
{	
	assert_cs( m_pEventTarget != NULL );	

	if( m_fPlayTime > m_pEventData->GetEventInfo()->s_fEventTime )
	{	
		assert_cs( m_pEventData->GetObjectInfo()->s_sType < CEventData::EO_END );

		switch( m_pEventData->GetObjectInfo()->s_sType )
		{
		case CEventData::EO_Npc:
			{
				CsMapNpc*		m_pFT_MapNpc;		
				m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( m_pEventData->GetObjectInfo()->s_dwEventObjectID );

				// 위치값
				NiPoint3 vPos = NiPoint3( (float)m_pFT_MapNpc->GetInfo()->s_nInitPosX, (float)m_pFT_MapNpc->GetInfo()->s_nInitPosY, 0.0f );
				vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

				// 회전값
				m_pEventTarget->SetPos( vPos );
				m_pEventTarget->SetRotation( m_pFT_MapNpc->GetInfo()->s_fRotation, true );	
				m_pEventTarget->SetDefault_IdleAnimation();									
			}
			break;
		case CEventData::EO_New:
			{					
				m_pEventTarget->SetPos( g_pMainWnd->_GetDlgController()->_GetNewPos() );
				m_pEventTarget->SetRotation( g_pMainWnd->_GetDlgController()->_GetNewRot(), true );	
				m_pEventTarget->SetDefault_IdleAnimation();
			}
			break;
			
		}	
		return false;
	}	
	
	//이벤트 돌리기
	for( int i = m_nProgress; i < m_pEventData->GetEventSize(); i++ )
	{	
		if( m_pEventData->GetEventData( m_nProgress )->GetEventStartTime() >= m_fPlayTime - g_fDeltaTime &&
			m_pEventData->GetEventData( m_nProgress )->GetEventStartTime() < m_fPlayTime )	
		{		
			switch( m_pEventData->GetEventData( m_nProgress )->GetType() )
			{
			case CEventData::ET_Ani:
				{
					CEventData::sAnimation_Info* pAni = (CEventData::sAnimation_Info*)m_pEventData->GetEventData( m_nProgress );					
					m_pEventTarget->SetAnimation( pAni->s_dwMotionID );
				}
				break;
			case CEventData::ET_Move:
				{
					CEventData::sMovePos_Info* pMove = (CEventData::sMovePos_Info*)m_pEventData->GetEventData( m_nProgress );					

					if( m_pMoveProp == NULL )
					{
						m_pEventTarget->AttachProp( CsC_Property::MOVE_CONTROL );	

						m_pMoveProp = new CsMoveObject;
						m_pMoveProp->SetTotalTime( m_pEventData->GetEventInfo()->s_fEventTime );					

						CsMoveObject::sACT_START sStart;
						switch( m_pEventData->GetObjectInfo()->s_sType )
						{
						case CEventData::EO_New:
							{
								sStart.SetPosX( (int)m_pEventData->GetObjectInfo()->s_vPos.x );
								sStart.SetPosY( (int)m_pEventData->GetObjectInfo()->s_vPos.y );
							}
							break;
						case CEventData::EO_Npc:
							{
								CsMapNpc*		m_pFT_MapNpc;		
								m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( m_pEventData->GetObjectInfo()->s_dwEventObjectID );

								sStart.SetPosX( m_pFT_MapNpc->GetInfo()->s_nInitPosX );
								sStart.SetPosY( m_pFT_MapNpc->GetInfo()->s_nInitPosY );
							}
							break;
						default:
							assert_cs( false );
							break;
						}						
						m_pMoveProp->AddAct( &sStart );

						CsMoveObject::sACT_MOVE sMove;
						sMove.SetTime( (int)pMove->s_fSpeed );
						sMove.SetPosX( (int)pMove->s_vPos.x );
						sMove.SetPosY( (int)pMove->s_vPos.y );
						m_pMoveProp->AddAct( &sMove );
					}
					else
					{
						m_pMoveProp->ResetActMove();
						m_pMoveProp->SetTotalTime( m_pEventData->GetEventInfo()->s_fEventTime );		

						CsMoveObject::sACT_START sStart;
						sStart.SetPosX( (int)m_pEventTarget->GetPos2D().x );
						sStart.SetPosY( (int)m_pEventTarget->GetPos2D().y );
						m_pMoveProp->AddAct( &sStart );

						CsMoveObject::sACT_MOVE sMove;
						sMove.SetTime( (int)pMove->s_fSpeed );
						sMove.SetPosX( (int)pMove->s_vPos.x );
						sMove.SetPosY( (int)pMove->s_vPos.y );
						m_pMoveProp->AddAct( &sMove );
					}							

					m_pEventTarget->GetProp_MoveControl()->SetFTEventMoveObject( m_pMoveProp );

					switch( m_pEventTarget->GetLeafRTTI() )
					{
					case RTTI_EVENTOBJECT:
						{						
							( (CsC_EventObject*)m_pEventTarget )->m_eObjectState = CsC_EventObject::OBJECT_MOVE;
						}
						break;
					case RTTI_EVENTTAMEROBJECT:
						{						
							( (CsC_EventTamerObject*)m_pEventTarget )->m_eObjectState = CsC_EventTamerObject::OBJECT_MOVE;														
						}
						break;					
					}
					
				}
				break;
			case CEventData::ET_Talk:
				{
					CEventData::sEventTalk_Info* pTalk = (CEventData::sEventTalk_Info*)m_pEventData->GetEventData( m_nProgress );						

					switch( m_pEventData->GetObjectInfo()->s_sType )
					{
					case CEventData::EO_New:
						{
							g_pTalkBallone->SetBalloone( (cTalkBalloon::eTYPE)2, m_pEventData->GetEventInfo()->s_nEventNum, 
								nsCsFileTable::g_pTalkMng->GetTalk_Event( pTalk->s_dwTalkID )->GetInfo()->s_szText, 
								true, (cTalkBalloon::sBALLOONE::OBJ_TYPE)m_pEventData->GetObjectInfo()->s_sType );
						}
						break;
					case CEventData::EO_Npc:
						{
							g_pTalkBallone->SetBalloone( (cTalkBalloon::eTYPE)2, m_pEventTarget->GetModelID(), 
								nsCsFileTable::g_pTalkMng->GetTalk_Event( pTalk->s_dwTalkID )->GetInfo()->s_szText, 
								true, (cTalkBalloon::sBALLOONE::OBJ_TYPE)m_pEventData->GetObjectInfo()->s_sType );
						}
						break;
					default:
						assert_cs( false );
						break;
					}					
				}
				break;
			case CEventData::ET_Sound:
				{
					CEventData::sEventSound_Info* pSound = (CEventData::sEventSound_Info*)m_pEventData->GetEventData( m_nProgress );
					g_pSoundMgr->PlaySound( pSound->szFileName );						
				}
				break;
			case CEventData::ET_Effect:
				{
					CEventData::sEventEffect_Info* pEffect = (CEventData::sEventEffect_Info*)m_pEventData->GetEventData( m_nProgress );
					CsEffectSound* pEffectSound = g_EffectSoundMng->GetData( pEffect->s_EffectHashCode );					
					
					CsC_EffectObject* pEffectObj = m_pEventTarget->GetProp_Effect()->AddEffect( pEffectSound->m_sESInfo.szFilePath, 
						pEffectSound->m_sEffectOptin.fScale, pEffectSound->m_sEffectOptin.dwEffectFlag );

					if( ( pEffectSound->m_sEffectOptin.dwEffectFlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
					{
						pEffectObj->SetOffset( pEffectSound->m_sEffectOptin.vOffset );						
					}
				}
				break;
			default:
				assert_cs( false );
			}	

			m_nProgress++;
		}
		else
		{
			break;
		}		
	}	

	return true;
}

void CEventScene::EndEvent()
{
	ReSetPlayTime();
	DeleteEventTarget();
	DeleteMoveData();
	EventDelete();	
}


void CEventScene::EventDelete()
{
	if( m_pEventData != NULL )
	{
		m_pEventData->Delete();
		SAFE_NIDELETE( m_pEventData );
	}	
}

void CEventScene::DeleteMoveData()
{
	if( m_pMoveProp != NULL )
	{
		m_pMoveProp->Delete();
		SAFE_NIDELETE( m_pMoveProp );
	}
}

void CEventScene::EventSetComplete( CEventData* pEvent )
{	
	if( pEvent == NULL )
		return;

	{
		TCHAR szTemp[ 128 ];
		_stprintf_s( szTemp, 128, L"내용 : %s / 총시간 : %.2f", 
			pEvent->GetEventInfo()->s_strTitle, pEvent->GetEventInfo()->s_fEventTime );
		CString str( szTemp );

		g_pMainWnd->_GetDlgController()->_SetInsertListPlayer((DWORD)pEvent->GetEventInfo()->s_nEventNum, str );
	}	
}

void CEventScene::CreateEvent()
{	
	EventDelete();		
	m_pEventData = NiNew CEventData;
}

void CEventScene::SetEventInit( CEventData::sEvent_Info pEventInfo, CEventData::sEventObject_Info pObjectInfo )
{
	m_pEventData->Init( &pEventInfo, &pObjectInfo );
}

void CEventScene::SetEvent( CEventData::sBase_Info* sEventInfo )
{
	m_pEventData->InsertEventData( sEventInfo );	
}

//=====================================================================================
//
//	무브 포인트
//
//=====================================================================================

void CEventScene::_CreateMovePoint()
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

void CEventScene::_DeleteMovePoint()
{
	m_MovePoint.Delete();
}

void CEventScene::_RenderMovePoint()
{
	if( m_bMovePointRender == false )
		return;

	m_MovePoint.m_pNiNode->SetTranslate( m_vMovePoint );
	m_MovePoint.m_pNiNode->Update( (float)g_fAccumTime );
	m_MovePoint.Render();
}

void CEventScene::SetMovePoint( NiPoint3 vMovePoint )
{
	m_vMovePoint = vMovePoint;
	m_bMovePointRender = true;
	NiTimeController::StartAnimations( m_MovePoint.m_pNiNode, (float)g_fAccumTime );
	
}

//==============================================================================
//
//		폰트
//
//==================================================`============================

bool CEventScene::CreateFont()
{	
	HRESULT hr = D3DXCreateFont(	g_pRenderer->GetD3DDevice(), 11,0,FW_NORMAL,1,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"굴림체",&m_pD3DXFont );
	assert_cs( SUCCEEDED( hr ) );
	//g_Log.Log( L"기본 폰트 생성 종료 = %d", hr );

	if( m_FontSystem.Init( "data\\font\\2002L.ttf" ) == false )
	{
		CsMessageBox( MB_OK, L"Create Font Failed!" );
		return false;
	}
	if( m_FontText.Init( "data\\font\\2002L.ttf" ) == false )
	{
		CsMessageBox( MB_OK, L"Create Font Failed!" );
		return false;
	}
	
	return true;
}

void CEventScene::DeleteFont()
{
	SAFE_RELEASE( m_pD3DXFont );
	m_FontSystem.Release();
	m_FontText.Release();	
}


NiPoint3 CEventScene::Cal3D_To_2D( NiPoint3 vPos )
{
	D3DXVECTOR4 v;
	NiPoint3 vReturn;
	D3DXVec3Transform( &v, (D3DXVECTOR3*)&vPos, GetD3DViewProj() );
	v = v/v.w;
	vReturn.x = m_ViewPort.X + m_ViewPort.Width*(1+v.x)*0.5f + 0.5f;
	vReturn.y = m_ViewPort.Y + m_ViewPort.Height*(1-v.y)*0.5f + 0.5f;
	vReturn.z = m_ViewPort.MinZ + v.z*( m_ViewPort.MaxZ - m_ViewPort.MinZ );
	return vReturn;
}

//=====================================================================================
//
//	무브 포인트
//
//=====================================================================================

void CEventScene::_CreateAreaPoint()
{
#define AREA_OBJECT_PATH		"ToolData\\RegionCylinder.nif"

	NiStream kStream;
	if( !kStream.Load( AREA_OBJECT_PATH ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 무브포인트 오브젝트를 찾지 못했습니다.", AREA_OBJECT_PATH );
		return;
	}

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_AreaPoint.SetNiObject( pNode, CGeometry::Normal );	

	m_bAreaPointRender = false;	
}

void CEventScene::_DeleteAreaPoint()
{
	m_AreaPoint.Delete();
}

void CEventScene::_RenderAreaPoint()
{
	if( m_bAreaPointRender == false )
		return;
	
	float fScale = g_pMainWnd->_GetDlgController()->_GetAreaRad() / 100.0f;
	m_AreaPoint.m_pNiNode->SetTranslate( m_vAreaPoint );
	m_AreaPoint.m_pNiNode->Update( (float)g_fAccumTime );

	NiMatrix3 mat;
	mat.MakeYRotation( -NI_PI );
	m_AreaPoint.m_pNiNode->SetRotate( mat );
	m_AreaPoint.m_pNiNode->SetScale( fScale );

	nsCSGBFUNC::Set_Emittance( m_AreaPoint.m_pNiNode, NiColor( 1.0f, 1.0f, 0.0f ) );

	m_AreaPoint.Render();	
}

void CEventScene::_RenderPlayAreaPoint()
{	
	float fScale = m_fPlayAreaRad;
	m_AreaPoint.m_pNiNode->SetTranslate(  m_vPlayAreaPoint  );
	m_AreaPoint.m_pNiNode->Update( (float)g_fAccumTime );

	NiMatrix3 mat;
	mat.MakeYRotation( -NI_PI );
	m_AreaPoint.m_pNiNode->SetRotate( mat );
	m_AreaPoint.m_pNiNode->SetScale( fScale );	

	nsCSGBFUNC::Set_Emittance( m_AreaPoint.m_pNiNode, NiColor( 1.0f, 0.0f, 0.0f ) );

	m_AreaPoint.Render();	
}

void CEventScene::SetAreaPoint( NiPoint3 vAreaPoint )
{
	m_vAreaPoint = vAreaPoint;	
	m_bAreaPointRender = true;
	NiTimeController::StartAnimations( m_AreaPoint.m_pNiNode, (float)g_fAccumTime );	
}

void CEventScene::SetPlayAreaPoint( NiPoint3 vAreaPoint, float fRad )
{
	m_vPlayAreaPoint = vAreaPoint;		
	m_fPlayAreaRad = fRad / 100;
	m_vPlayAreaPoint.z += m_fPlayAreaRad / 7;
	NiTimeController::StartAnimations( m_AreaPoint.m_pNiNode, (float)g_fAccumTime );	
}

//=====================================================================================	
//	이펙트영역 표시
//=====================================================================================

void CEventScene::_CreateEffectPoint()
{
#define AXIS_ROTATION_OBJECT_PATH			".\\ToolData\\axis.nif"
#define AXIS_ROTATION_PLANE_OBJECT_PATH		".\\ToolData\\axisPlane.nif"

	NiStream kStream;
	if( !kStream.Load( AXIS_ROTATION_OBJECT_PATH ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 무브포인트 오브젝트를 찾지 못했습니다.", AXIS_ROTATION_OBJECT_PATH );
		return;
	}

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_pAxisNiNode.SetNiObject( pNode, CGeometry::Normal );	
	kStream.RemoveAllObjects();
	
	if( !kStream.Load( AXIS_ROTATION_PLANE_OBJECT_PATH ) )
	{
		CsMessageBoxA( MB_OK, "%s 경로\n에서 무브포인트 오브젝트를 찾지 못했습니다.", AXIS_ROTATION_PLANE_OBJECT_PATH );
		return;
	}

	pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_pAxisPlaneNiNode.SetNiObject( pNode, CGeometry::Normal );	

	m_vEffectPoint = NiPoint3::ZERO;

}

void CEventScene::_DeleteEffectPoint()
{
	m_pAxisNiNode.Delete();
	m_pAxisPlaneNiNode.Delete();
}

void CEventScene::_RenderEffectPoint()
{
	m_pAxisNiNode.m_pNiNode->SetTranslate( m_vEffectPoint );
	m_pAxisNiNode.m_pNiNode->Update( (float)g_fAccumTime );	
	
	m_pAxisNiNode.Render();	

	m_pAxisPlaneNiNode.m_pNiNode->SetTranslate( m_vEffectPoint );
	m_pAxisPlaneNiNode.m_pNiNode->Update( (float)g_fAccumTime );	

	m_pAxisPlaneNiNode.Render();	
}


//==============================================================================
//
//		이벤트 작업 로드및 세이브 임시저장
//
//==============================================================================

void CEventScene::DeleteTempEventAll()
{
	std::map< DWORD, MAP_EvntData >::iterator it = m_mapTempData.begin();
	std::map< DWORD, MAP_EvntData >::iterator itEnd = m_mapTempData.end();
	for( ; it != itEnd; it++ )
	{
		MAP_EvntData_IT it2 = it->second.begin();
		MAP_EvntData_IT it2End = it->second.end();		
		for( ;it2 != it2End; it2++ )
		{
			it2->second->Delete();			
			SAFE_NIDELETE( it2->second );
		}
		it->second.clear();
	}
	m_mapTempData.clear();
}


void CEventScene::DeleteTempEvent( UINT EventNum )
{
	std::map< DWORD, MAP_EvntData >::iterator it = m_mapTempData.find( g_Terrain.m_dwMapId );
	if( it != m_mapTempData.end() )
	{
		MAP_EvntData_IT pData = it->second.find( EventNum );
		if( pData != it->second.end() )
		{
			pData->second->Delete();	
			SAFE_NIDELETE( pData->second );
			it->second.erase( pData );		
		}		
	}	
}

CEventData* CEventScene::GetTempEventData( UINT EventNum )
{	
	std::map< DWORD, MAP_EvntData >::iterator it = m_mapTempData.begin();
	std::map< DWORD, MAP_EvntData >::iterator itEnd = m_mapTempData.end();
	for( ; it != itEnd; ++it ) 
	{
		MAP_EvntData_IT pData = it->second.find( EventNum );
		if( pData != it->second.end() )
		{
			return pData->second;
		}		
	}		
	assert_csm( false, L"TempEventData에 존재하지 않는 이벤트입니다" );
	return NULL;
}

void CEventScene::_InsertTempEvent( DWORD dwMapID )
{	
	if( m_pEventData )
		DeleteTempEvent( m_pEventData->GetEventInfo()->s_nEventNum );

	TCHAR sz[ MAX_PATH ];
	_stprintf_s( sz, MAX_PATH, L"%s.event", m_pEventData->GetEventInfo()->s_strTitle );
	_LoadTempEventData( sz );

	//CEventData*		pData = NiNew CEventData;
	//memcpy( pData, m_pEventData, sizeof( CEventData ) );

	//m_mapTempData[ dwMapID ][ m_pEventData->GetEventInfo()->s_nEventNum ] = pData;	
}

void CEventScene::SaveTempEventData()
{
	TCHAR szFilename[ MAX_PATH ];
	_stprintf_s( szFilename, MAX_PATH, L"%s%s.event", EVENTDATADIR, m_pEventData->GetEventInfo()->s_strTitle );
	FILE* fp = NULL;
	_tfopen_s( &fp, szFilename, L"wb" );

	int nMap = g_pTerrainMng->GetBackupMapID();
	fwrite( &nMap, sizeof( int ), 1, fp );

	//오프젝트 명령이벤트 수
	int nNum = (int)m_pEventData->GetEventSize();
	fwrite( &nNum, sizeof( int ), 1, fp );

	fwrite( m_pEventData->GetEventInfo(), sizeof( CEventData::sEvent_Info ), 1, fp );		
	fwrite( m_pEventData->GetObjectInfo(), sizeof( CEventData::sEventObject_Info ), 1, fp );	

	for( int i = 0; i < nNum; i++ )
	{
		switch( m_pEventData->GetEventData( i )->GetType() )
		{
		case CEventData::ET_Ani:
			fwrite( m_pEventData->GetEventData( i ), sizeof( CEventData::sAnimation_Info ), 1, fp );
			break;
		case CEventData::ET_Move:
			fwrite( m_pEventData->GetEventData( i ), sizeof( CEventData::sMovePos_Info ), 1, fp );
			break;
		case CEventData::ET_Talk:
			fwrite( m_pEventData->GetEventData( i ), sizeof( CEventData::sEventTalk_Info ), 1, fp );
			break;
		case CEventData::ET_Sound:
			fwrite( m_pEventData->GetEventData( i ), sizeof( CEventData::sEventSound_Info ), 1, fp );
			break;
		case CEventData::ET_Effect:
			fwrite( m_pEventData->GetEventData( i ), sizeof( CEventData::sEventEffect_Info ), 1, fp );
			break;
		default:
			assert_cs( false );
		}					
	}		

	fclose( fp );	

	_InsertTempEvent( g_Terrain.m_dwMapId );
		
	CsMessageBox( MB_OK, L"저장완료" );
}

void CEventScene::_LoadTempEventALL()
{
	CString strDrt( EVENTDATADIR );
	strDrt += "\\*.*";	
	CFileFind ff;

	BOOL bFound = ff.FindFile( strDrt );	

	while(bFound)
	{
		bFound = ff.FindNextFile();

		if(ff.IsDots()) continue;

		if( ff.GetFilePath().Find( _T(".event") ) != -1 )				
		{	
			_LoadTempEventData( (TCHAR*)(LPCTSTR)(ff.GetFileName()) ); 
		}
	}
}

void CEventScene::_LoadTempEventData( TCHAR* szFilename )
{
	TCHAR sz[ MAX_PATH ];
	_stprintf_s( sz, MAX_PATH, L"%s%s", EVENTDATADIR, szFilename );

	FILE* fp = NULL;
	_tfopen_s( &fp, sz, L"r+b" );
	assert_cs( fp != NULL );

	int nMap;
	fread( &nMap, sizeof( int ), 1, fp );

	int nNum;
	fread( &nNum, sizeof( int ), 1, fp );	

	CEventData* pEventData = NiNew CEventData;
	CEventData::sEvent_Info eInfo;
	CEventData::sEventObject_Info oInfo;

	fread( &eInfo , sizeof( CEventData::sEvent_Info ), 1, fp );
	fread( &oInfo, sizeof( CEventData::sEventObject_Info ), 1, fp );

	pEventData->Init( &eInfo, &oInfo );		

	for( int j = 0 ; j < nNum; j++ )			
	{
		CEventData::sBase_Info* pData = NiNew CEventData::sBase_Info;
		fread( pData, sizeof( CEventData::sBase_Info ), 1, fp );			
		int nsize = sizeof( CEventData::sBase_Info );

		switch( pData->GetType() )
		{
		case CEventData::ET_Ani:
			{					
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sAnimation_Info* pAni = NiNew CEventData::sAnimation_Info;
				fread( pAni, sizeof( CEventData::sAnimation_Info ), 1, fp );
				pEventData->InsertEventData( pAni );
			}
			break;
		case CEventData::ET_Move:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sMovePos_Info* pMove = NiNew CEventData::sMovePos_Info;
				fread( pMove, sizeof( CEventData::sMovePos_Info ), 1, fp );
				pEventData->InsertEventData( pMove );
			}
			break;
		case CEventData::ET_Talk:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sEventTalk_Info* pTalk = NiNew CEventData::sEventTalk_Info;
				fread( pTalk, sizeof( CEventData::sEventTalk_Info ), 1, fp );
				pEventData->InsertEventData( pTalk );
			}
			break;
		case CEventData::ET_Sound:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sEventSound_Info* pSound = NiNew CEventData::sEventSound_Info;
				fread( pSound, sizeof( CEventData::sEventSound_Info ), 1, fp );
				pEventData->InsertEventData( pSound );
			}
			break;
		case CEventData::ET_Effect:
			{
				fseek( fp, -nsize, SEEK_CUR );
				CEventData::sEventEffect_Info* pEffect = NiNew CEventData::sEventEffect_Info;
				fread( pEffect, sizeof( CEventData::sEventEffect_Info ), 1, fp );
				pEventData->InsertEventData( pEffect );						
			}
			break;
		default:
			assert_cs( false );
			break;
		}
		SAFE_NIDELETE( pData );	
	}

	m_mapTempData[ nMap ][ eInfo.s_nEventNum ] = pEventData;	
	fclose( fp );	
}

//==============================================================================
//
//		사운드 이벤트 플레이
//
//==============================================================================


void CEventScene::CALLBACK_EventSound( char* szFileName )
{
	g_pSoundMgr->PlaySound( szFileName );
}

