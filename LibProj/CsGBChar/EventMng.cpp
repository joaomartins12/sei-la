#include "StdAfx.h"
#include "EventMng.h"
#include "EventDataMng.h"

CEventMng::CEventMng(void)
{		
}

CEventMng::~CEventMng(void)
{
	Delete();
}


void CEventMng::Delete()
{
	LIST_EVENT_IT it = m_ListEventDataMng.begin();
	LIST_EVENT_IT itEnd = m_ListEventDataMng.end();	
	while( it != itEnd )
	{			
		(*it)->Delete();
		NiDelete *it;		
		++it;		
	}
	m_ListEventDataMng.clear();		
}

bool CEventMng::Update( float fDeltaTime )
{
	LIST_EVENT_IT it = m_ListEventDataMng.begin();
	LIST_EVENT_IT itEnd = m_ListEventDataMng.end();	
	while( it != itEnd )
	{		
		if( (*it)->Update( fDeltaTime ) == false )
		{						
			(*it)->Delete();
			SAFE_NIDELETE( *it );
			it = m_ListEventDataMng.erase( it );
		}
		else
		{
			++it;			
		}			
	}

	return ( m_ListEventDataMng.empty() == false );
}

void CEventMng::Render()
{
	LIST_EVENT_IT it = m_ListEventDataMng.begin();
	LIST_EVENT_IT itEnd = m_ListEventDataMng.end();	
	for( ; it != itEnd; ++it )
	{
		(*it)->s_pTarget->Render();
	}
}

void CEventMng::Init( UINT SceneNum )
{
	m_SceneNum = SceneNum;

	CEventSceneData* pCon = g_pEventDataMng->GetData( SceneNum );
	for( int j = 0; j < pCon->GetEventSize(); j++ )
	{							
		_InsertData( pCon->GetEventDatabyIdx( j ) );											
	}	
}

void CEventMng::_InsertData( CEventData* pEvent )
{
	if( IsEventData( pEvent ) )
	{
		//이미 해당이벤트 존재
		//같은 이벤트 번호 존재		
		return;
	}

	sEventData* pEventData = NiNew sEventData();	
	pEventData->Init( pEvent );

	//이벤트 주체가 될대상 만들기
	switch( pEventData->GetEvent()->GetObjectInfo()->s_sType )
	{
	case CEventData::EO_New:
		{
			pEventData->SetTarget();
			assert_cs( pEventData->s_pTarget != NULL );
		}		
		break;	
	case CEventData::EO_Npc:
		{
			pEventData->s_pTarget = g_pEventSceneMng->m_CallBackEventObject( pEvent->GetObjectInfo()->s_dwEventObjectID );		
		}
		break;
	default:
		assert_cs( false );
	}

	//이벤트 등록
	m_ListEventDataMng.push_back( pEventData );	
}

bool CEventMng::IsEventData( CEventData* pEventData  )
{
	LIST_EVENT_IT it = m_ListEventDataMng.begin();
	LIST_EVENT_IT itEnd = m_ListEventDataMng.end();	
	for( ; it != itEnd; ++it )
	{
		if( (*it)->GetEvent()->GetEventInfo()->s_nEventNum == pEventData->GetEventInfo()->s_nEventNum )
			return true;
	}

	return false;
}


void CEventMng::SetEventCameraTarget( int nCount )
{
	LIST_EVENT_IT it = m_ListEventDataMng.begin();	
	for( int i = 0; i < nCount; ++i )
	{
		++it;	
	}
	m_pCameraTarget = (*it)->s_pTarget; 
	assert_cs( m_pCameraTarget );
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	이벤트 데이터 구조체
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CEventMng::sEventData::Delete()
{
	// 타겟 삭제
	switch( s_pEventData->GetObjectInfo()->s_sType )
	{
	case CEventData::EO_New: 
		{		
			sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
			pUnit->s_pLoadedObject = s_pTarget;	
			g_pThread->DeleteChar( pUnit );	
		}
		break;
	case CEventData::EO_Npc:
		{
			//npc는 NpcMng에서 삭제해주니까 Pass
			if( s_pTarget->IsProperty( CsC_Property::MOVE_CONTROL ) == true )
				s_pTarget->GetProp_MoveControl()->SetFTEventMoveObject( NULL );

			s_pTarget = NULL;			
		}
		break;
	default:
		assert_cs( false );
		break;
	}	

	// 이동 프러퍼티 데이타 삭제
	if( s_pMoveProp )
	{
		s_pMoveProp->Delete();	
		delete s_pMoveProp;
		s_pMoveProp = NULL;
	}
}

void CEventMng::sEventData::SetTarget()
{	
	 UINT nIDX = s_pEventData->GetEventInfo()->s_nEventNum;
	 DWORD dwModel = s_pEventData->GetObjectInfo()->s_dwEventObjectID;
	 NiPoint3 vPos = s_pEventData->GetObjectInfo()->s_vPos + s_pEventData->GetObjectInfo()->s_vOffset;
	 float fRot = s_pEventData->GetObjectInfo()->s_fRot;
	 float fScale = s_pEventData->GetObjectInfo()->s_fScale;	 		

	if( dwModel / 10000 == 8 )
	{		
		//신규캐릭터 생성
		CsC_EventTamerObject* pEvent = CsC_EventTamerObject::NewInstance();	
		pEvent->Init( nIDX, dwModel, vPos, fRot, NULL, NULL );	
		pEvent->SetName( s_pEventData->GetObjectInfo()->s_strName );
		pEvent->SetScale( fScale );		

		s_pTarget = (CsC_AvObject*)pEvent;
	}
	else
	{
		// 로드시 필요한 초기화
		sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nIDX );

		// 초기화 - Prop 초기화 포함
		pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
		pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_ENABLE | PR_EFFECT_ENABLE;
		pUint->s_Prop.s_dwOptionPlag = OP_LIGHT | OP_STAT;

		std::string strPath = g_pModelDataMng->GetKfmPath( dwModel );
		strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );	

		//신규캐릭터 생성
		CsC_EventObject* pEvent = CsC_EventObject::NewInstance();	
		pEvent->Init( nIDX, dwModel, vPos, fRot, &pUint->s_Prop );			
		pEvent->SetName( s_pEventData->GetObjectInfo()->s_strName );
		pEvent->SetScale( fScale );

		// 로드 설정
		pUint->s_pLoadedObject = (CsC_EventObject*)pEvent;	
		//우선로드 설정
		pUint->SetLoadType( sTCUnit::eFast );

		s_pTarget = (CsC_AvObject*)pEvent;		

		// 쓰레드 로드		
		g_pThread->LoadChar( pUint );						
	}
	
}

bool CEventMng::sEventData::Update( float fDeltaTime )
{
	//시간 업데이트
	s_fCurTime += fDeltaTime;		

	//딜레이타임 지났으면 플레이하자
	if( s_fCurTime < s_pEventData->GetEventInfo()->s_fStartTime )		
		return true;
	
	CsC_AvObject* pObject = s_pTarget;
	switch( s_pEventData->GetObjectInfo()->s_sType )
	{
	case CEventData::EO_New:
		{
			switch( pObject->GetLeafRTTI() )
			{
			case RTTI_EVENTOBJECT:
				{
					CsC_EventObject* pEventObject = ( CsC_EventObject* )pObject;
					if( pEventObject->m_eObjectState == CsC_EventObject::OBJECT_STOP ) 
					{
						pEventObject->m_eObjectState = CsC_EventObject::OBJECT_NORMAL;							
						pEventObject->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_ALPHA_INCREASE, 0.0f );							
					}					
				}
				break;
			case RTTI_EVENTTAMEROBJECT:
				{
					CsC_EventTamerObject* pEventTamerObject = ( CsC_EventTamerObject* )pObject;					
					if( pEventTamerObject->m_eObjectState == CsC_EventTamerObject::OBJECT_STOP )
					{
						pEventTamerObject->m_eObjectState = CsC_EventTamerObject::OBJECT_NORMAL;								
						pEventTamerObject->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_ALPHA_INCREASE, 0.0f );								
					}
				}
				break;
			default:
				assert_cs( false );
				break;
			}													
		}		
		break;
	case CEventData::EO_Npc:
		break;
	default:
		assert_cs( false );
		break;
	}			

	return PlayEvent( fDeltaTime );
}

bool CEventMng::sEventData::PlayEvent( float fDeltaTime )
{
	UpdateObject( fDeltaTime );

	if( IsLive( fDeltaTime ) )
	{
		//이벤트 체크				
		assert_cs( s_nProgress >= 0 );
		assert_cs( s_nProgress <= s_pEventData->GetEventSize() );		
		
		for( int i = s_nProgress; i < s_pEventData->GetEventSize(); i++ )
		{						
			if( ControlObject( fDeltaTime ) == false )
				break;			
		}		
	}
	else
	{
		// npc는 원래 자리로 돌아가게 한다.
		if( s_pEventData->GetObjectInfo()->s_sType == CEventData::EO_Npc )
		{
			CsMapNpc*		m_pFT_MapNpc;		
			m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( s_pEventData->GetObjectInfo()->s_dwEventObjectID );

			// 위치값
			NiPoint3 vPos = NiPoint3( (float)m_pFT_MapNpc->GetInfo()->s_nInitPosX, (float)m_pFT_MapNpc->GetInfo()->s_nInitPosY, 0.0f );
			vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

			// 회전값
			s_pTarget->SetPos( vPos );
			s_pTarget->SetRotation( m_pFT_MapNpc->GetInfo()->s_fRotation, true );	
			s_pTarget->SetDefault_IdleAnimation();							
			s_pTarget->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_ALPHA_INCREASE, 0.0f );	
		}	

		return false;		
	}

	return true;
}

bool CEventMng::sEventData::ControlObject( float fDeltaTime )
{
	//이벤트 돌리기
	//현재 시간과 deltatime 사이에 조작할명령이 있다면..
	if( s_pEventData->GetEventData( s_nProgress )->GetEventStartTime() + s_pEventData->GetEventInfo()->s_fStartTime >= s_fCurTime - fDeltaTime 
		&& s_pEventData->GetEventData( s_nProgress )->GetEventStartTime() + s_pEventData->GetEventInfo()->s_fStartTime < s_fCurTime )	
	{
		switch( s_pEventData->GetEventData( s_nProgress )->GetType() )
		{
		case CEventData::ET_Ani:
			{
				CEventData::sAnimation_Info* pAni = (CEventData::sAnimation_Info*)s_pEventData->GetEventData( s_nProgress );					
				s_pTarget->SetAnimation( pAni->s_dwMotionID );
			}
			break;
		case CEventData::ET_Move:
			{
				CEventData::sMovePos_Info* pMove = (CEventData::sMovePos_Info*)s_pEventData->GetEventData( s_nProgress );					

				if( s_pMoveProp == NULL )
				{
					s_pTarget->AttachProp( CsC_Property::MOVE_CONTROL );	

					s_pMoveProp = new CsMoveObject;
					s_pMoveProp->SetTotalTime( s_pEventData->GetEventInfo()->s_fEventTime );										

					CsMoveObject::sACT_START sStart;
					switch( s_pEventData->GetObjectInfo()->s_sType )
					{
					case CEventData::EO_New:
						{
							sStart.SetPosX( (int)s_pEventData->GetObjectInfo()->s_vPos.x );
							sStart.SetPosY( (int)s_pEventData->GetObjectInfo()->s_vPos.y );
						}
						break;
					case CEventData::EO_Npc:
						{
							CsMapNpc*		m_pFT_MapNpc;		
							m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( s_pEventData->GetObjectInfo()->s_dwEventObjectID );

							sStart.SetPosX( m_pFT_MapNpc->GetInfo()->s_nInitPosX );
							sStart.SetPosY( m_pFT_MapNpc->GetInfo()->s_nInitPosY );
						}
						break;
					default:
						assert_cs( false );
						break;
					}						
					s_pMoveProp->AddAct( &sStart );

					CsMoveObject::sACT_MOVE sMove;
					sMove.SetTime( (int)pMove->s_fSpeed );
					sMove.SetPosX( (int)pMove->s_vPos.x );
					sMove.SetPosY( (int)pMove->s_vPos.y );
					s_pMoveProp->AddAct( &sMove );
				}
				else
				{
					s_pMoveProp->ResetActMove();
					s_pMoveProp->SetTotalTime( s_pEventData->GetEventInfo()->s_fEventTime );		

					CsMoveObject::sACT_START sStart;
					sStart.SetPosX( (int)s_pTarget->GetPos2D().x );
					sStart.SetPosY( (int)s_pTarget->GetPos2D().y );
					s_pMoveProp->AddAct( &sStart );
					
					CsMoveObject::sACT_MOVE sMove;
					sMove.SetTime( (int)pMove->s_fSpeed );
					sMove.SetPosX( (int)pMove->s_vPos.x );
					sMove.SetPosY( (int)pMove->s_vPos.y );
					s_pMoveProp->AddAct( &sMove );
				}											
				
				s_pTarget->GetProp_MoveControl()->SetFTEventMoveObject( s_pMoveProp );	

				switch( s_pTarget->GetLeafRTTI() )
				{
				case RTTI_EVENTOBJECT:
					{						
						( (CsC_EventObject*)s_pTarget )->m_eObjectState = CsC_EventObject::OBJECT_MOVE;
					}
					break;
				case RTTI_EVENTTAMEROBJECT:
					{						
						( (CsC_EventTamerObject*)s_pTarget )->m_eObjectState = CsC_EventTamerObject::OBJECT_MOVE;														
					}
					break;				
				}
			}
			break;
		case CEventData::ET_Talk:
			{
				CEventData::sEventTalk_Info* pTalk = (CEventData::sEventTalk_Info*)s_pEventData->GetEventData( s_nProgress );						

				switch( s_pEventData->GetObjectInfo()->s_sType )
				{
				case CEventData::EO_New:
					g_pEventCall->EventTalk( s_pEventData->GetEventInfo()->s_nEventNum, nsCsFileTable::g_pTalkMng->GetTalk_Event( pTalk->s_dwTalkID )->GetInfo()->s_szText, 
						true, s_pEventData->GetObjectInfo()->s_sType );
					break;
				case CEventData::EO_Npc:
					g_pEventCall->EventTalk( s_pEventData->GetEventInfo()->s_nEventNum, nsCsFileTable::g_pTalkMng->GetTalk_Event( pTalk->s_dwTalkID )->GetInfo()->s_szText, 
						true, s_pEventData->GetObjectInfo()->s_sType );							
					break;
				default:
					assert_cs( false );
					break;
				}
			}
			break;
		case CEventData::ET_Sound:
			{
				CEventData::sEventSound_Info* pSound = (CEventData::sEventSound_Info*)s_pEventData->GetEventData( s_nProgress );
				assert_cs( g_pEventCall != NULL );						

				switch( pSound->nType )
				{
				case CEventData::sEventSound_Info::ES_SOUND:
					g_pEventCall->PlaySound( pSound->szFileName + 1 );
					break;
				case CEventData::sEventSound_Info::ES_MUSIC:
					g_pEventCall->PlayMusic( pSound->szFileName + 12 );
					break;
				default:
					assert_cs(false);
					break;
				}
			}
			break;
		case CEventData::ET_Effect:
			{
				CEventData::sEventEffect_Info* pEffect = (CEventData::sEventEffect_Info*)s_pEventData->GetEventData( s_nProgress );
				CsEffectSound* pEffectSound = g_EffectSoundMng->GetData( pEffect->s_EffectHashCode );					

				assert_csm( pEffectSound != NULL, _T( "이펙트 사운트 정보 없음" ) );
				if( pEffectSound != NULL )
				{
					CsC_EffectObject* pEffectObj = 	s_pTarget->GetProp_Effect()->AddEffect( pEffectSound->m_sESInfo.szFilePath, 
						pEffectSound->m_sEffectOptin.fScale, pEffectSound->m_sEffectOptin.dwEffectFlag );

					float fScale;
					if( pEffectSound->m_sEffectOptin.bChrScale == true )
					{
						fScale = pEffectSound->m_sEffectOptin.fScale * s_pTarget->GetToolWidth() * 0.027f;
					}
					else
					{
						fScale = pEffectSound->m_sEffectOptin.fScale;
					}	
					pEffectObj->SetScale( fScale );		

					if( ( pEffectSound->m_sEffectOptin.dwEffectFlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
					{
						pEffectObj->SetOffset( pEffectSound->m_sEffectOptin.vOffset );						
					}
				}				
			}
			break;			
		default:
			assert_cs( false );
		}

		s_nProgress++;
		return true;
	}	

	return false;
}


void CEventMng::sEventData::UpdateObject( float fDeltaTime )
{
	switch( s_pTarget->GetLeafRTTI() )
	{
	case RTTI_EVENTOBJECT:
		{
			CsC_EventObject* pEventObject = ( CsC_EventObject* )s_pTarget;					
			pEventObject->Update( fDeltaTime );		
		}
		break;
	case RTTI_EVENTTAMEROBJECT:
		{
			CsC_EventTamerObject* pEventTamerObject = ( CsC_EventTamerObject* )s_pTarget;					
			pEventTamerObject->Update( fDeltaTime );		
		}
		break;		
	}	
}

bool CEventMng::sEventData::IsLive( float fDeltaTime )
{	
	if( s_fCurTime - s_pEventData->GetEventInfo()->s_fEventTime > 2.0f )
		return false;

	//이벤트 종료된건지 확인
	if( s_fCurTime > s_pEventData->GetEventInfo()->s_fEventTime )
	{	
		assert_cs( s_pEventData->GetObjectInfo()->s_sType < CEventData::EO_END );		

		//이벤트 시간이 지났으면 사라지게한다	
		if( s_pTarget->GetProp_Alpha()->GetCurAlphaValue() == 1.0f )
		{
			s_pTarget->GetProp_Alpha()->SetDestAlphaTime( 1.3f );		
			s_pTarget->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_ALPHA_DECREASE, 0.9f );		
		}

		//완전히 사라지면 true 리턴..
		if( s_pTarget->GetProp_Alpha()->GetCurAlphaValue() > 0.0f )
			return true;		
		else		
			return false;		
	}	
	
	return true;
}

