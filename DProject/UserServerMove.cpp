#include "stdafx.h"
#include "UserServerMove.h"

CUserServerMove*		g_pServerMoveOwner = NULL;

void CUserServerMove::Init( CsC_AvObject* pParent )
{
	m_pParent = pParent;

	m_LastSyncPos.m_nX = (int)m_pParent->GetPos().x;
	m_LastSyncPos.m_nY = (int)m_pParent->GetPos().y;

	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		m_SendMoveTimeSeq.SetDeltaTime( 300 );
		m_RetryKeyCheckTimeSeq.SetDeltaTime( 100 );
		break;
	case nLIB::SVR_BATTLE:
		m_SendMoveTimeSeq.SetDeltaTime( 200 );
		m_RetryKeyCheckTimeSeq.SetDeltaTime( 100 );
		break;
	default:
		assert_cs( false );
	}
	m_SendMoveTimeSeq.Reset();
	m_RetryKeyCheckTimeSeq.OffReset();

	m_KeyboardMoveLock = 0;

	KeyReset();
}

void CUserServerMove::Delete()
{
	if( g_pServerMoveOwner == this )
		g_pServerMoveOwner = NULL;
}

//=============================================================================================================
//
//	Owner
//
//=============================================================================================================
void CUserServerMove::SetKeyBoardMoveDir()
{ 
	m_vMoveUpDir = CAMERA_ST.GetWorldDirection(); 
	m_vMoveRightDir = CAMERA_ST.GetWorldRightVector(); 
}

void CUserServerMove::UpdateCamera()
{
	NiPoint3 vPos = m_pParent->GetPos();

	vPos.z += (m_pParent->GetToolHeight() * 0.8f);
	//vPos.z += 120.0f;

#ifdef KEYBOARD_MOVE
	if( g_pServerMoveOwner->m_bCurState != g_pServerMoveOwner->m_bBackupState )
	{
		NiPoint3 vViewPos = CAMERA_ST.GetViewerPos();
		float fLength = (vPos - vViewPos).Length();
		if( fLength < 50.0f )
			g_pServerMoveOwner->m_bBackupState = g_pServerMoveOwner->m_bCurState;
		else
		{
			NiPoint3 vDir = vPos - vViewPos;
			vDir.Unitize();
			//vPos = vViewPos + vDir * 50.0f;
 			vPos.x = vViewPos.x + vDir.x * 50.f;
 			vPos.y = vViewPos.y + vDir.y * 50.f;
		}		
	}
	SetKeyBoardMoveDir();
	KeyAutoMapping();
#endif
	nsCsGBTerrain::g_pCurRoot->SetViewerPos( vPos );
	CAMERA_ST.SetTranslate( vPos );
}

void CUserServerMove::DBClick_Object( cType type, CsC_AvObject* pObject )
{
	// 무브 포인트 캔슬
	g_pCharResMng->ReleaseMovePoint();


	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		{
			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			switch( type.m_nClass )
			{
			case nClass::Npc:
				pTamer->SetArriveTarget( pObject );
				pTamer->SetTargetPos( pObject->GetPos(), 200.0f, false, false );					
				break;
			case nClass::Item:
				// 아이템은 같은거 클릭으로 움직이지 않는다
				break;
			case nClass::Digimon:
				pTamer->ReleaseArriveTarget();
				pTamer->SetTargetPos( pObject->GetPos(), 200.0f, false, false );
				break;
			case nClass::Tamer:
				pTamer->SetArriveTarget( pObject );
				pTamer->SetTargetPos( pObject->GetPos(), 200.0f, false, false );
				break;
			case nClass::CommissionShop:
				pTamer->SetArriveTarget( pObject );
				pTamer->SetTargetPos( pObject->GetPos(), 200.0f, false, false );
				break;
			case nClass::Monster:					
				pTamer->SetAttackPos( pObject );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	case nLIB::SVR_BATTLE:
		{
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
			switch( type.m_nClass )
			{
			case nClass::Npc:
				break;
			case nClass::Item:
				break;
			case nClass::Digimon:
				if( pDigimonUser->GetBattleTeam() != ( (CDigimon*)pObject )->GetBattleTeam() )
				{
					pDigimonUser->SetDigimonUserState( CDigimonUser::DU_ATTACK, pObject );
					// 공격하기 쿨타임
					g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkill( 2 )->s_CoolTime.Start();
				}
				else
				{
					pDigimonUser->SetTargetPos( pObject->GetPos(), 200.0f, false );
				}
				break;
			case nClass::Tamer:
				break;
			case nClass::CommissionShop:
				break;
			case nClass::Monster:
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	default:
		assert_cs( false );
	}	
}

void CUserServerMove::SetPos_FromMouse( POINT ptMouse, float fConnectLength, bool bRenderMoveModel, bool bAttack )
{
	if( ( m_KeyboardMoveLock & eMouseLock ) == eMouseLock )
		return;

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_Terrain( CAMERA_ST.GetCameraObj(), ptMouse.x, ptMouse.y );
	SAFE_POINTER_RET( pRecord );

	NiPoint3 vIntersect = pRecord->GetIntersection();
	if( !nsCsGBTerrain::g_pCurRoot->IsInTerrain( vIntersect.x, vIntersect.y ) )
		return;

	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		{
#ifdef KEYBOARD_MOVE
			if( g_pResist->m_Global.s_bMoveDigimon )
			{
				CDigimonUser* pDigimon = (CDigimonUser*)m_pParent;
				pDigimon->SetDigimonUserState( CDigimonUser::DU_KEYBOARD_MOVE );
				//=====================================================================================
				// 디지몬 이동
				//=====================================================================================	
				// 디지몬 거리 체크
				if( ( NiPoint2( vIntersect.x, vIntersect.y ) - pDigimon->GetPos2D() ).Length() > 20000.0f )
				{
					cDigimonTalk::Print( 30016 );
					return;
				}

				vIntersect.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vIntersect );
				pDigimon->SetTargetPos( vIntersect, fConnectLength, bRenderMoveModel );

				//=====================================================================================
				// 테이머 이동
				//=====================================================================================
				g_pCharMng->GetTamerUser()->SetTamerUserState( CTamerUser::TU_FOLLOW_DIGIMON );
			}
			else
			{
	  			//assert_cs( m_pParent->GetLeafRTTI() == RTTI_TAMER_USER );
	  			CTamerUser* pTamer = (CTamerUser*)m_pParent;
	  
	  			//=====================================================================================
	  			// 테이머 이동
	  			//=====================================================================================	
	  			// 테이머 와의 거리 체크
	  			if( ( NiPoint2( vIntersect.x, vIntersect.y ) - pTamer->GetPos2D() ).Length() > 20000.0f )
	  			{
	  				cDigimonTalk::Print( 30016 );
	  				return;
	  			}
	  			vIntersect.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vIntersect );
	  			pTamer->SetTargetPos( vIntersect, fConnectLength, bRenderMoveModel, bAttack );
	  
	  			//=====================================================================================
	  			// 디지몬 이동
	  			//=====================================================================================
// 	  			if( pTamer->IsRide() == false )
// 	  			{
// 	  				CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
// 	  				if( pDigimonUser->GetDigimonUserState() != CDigimonUser::DU_FOLLOW )
// 	  				{				
// 	  					pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
// 	  				}
// 	  			}
			}
#else
			assert_cs( m_pParent->GetLeafRTTI() == RTTI_TAMER_USER );
			CTamerUser* pTamer = (CTamerUser*)m_pParent;

			//=====================================================================================
			// 테이머 이동
			//=====================================================================================	
			// 테이머 와의 거리 체크
			if( ( NiPoint2( vIntersect.x, vIntersect.y ) - pTamer->GetPos2D() ).Length() > 20000.0f )
			{
				cDigimonTalk::Print( 30016 );
				return;
			}
			vIntersect.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vIntersect );
			pTamer->SetTargetPos( vIntersect, fConnectLength, bRenderMoveModel, bAttack );

			//=====================================================================================
			// 디지몬 이동
			//=====================================================================================
			if( pTamer->IsRide() == false )
			{
				CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
				if( pDigimonUser->GetDigimonUserState() != CDigimonUser::DU_FOLLOW )
				{				
					pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
				}
			}
#endif	// KEYBOARD_MOVE
		}
		break;
	case nLIB::SVR_BATTLE:
		{
			vIntersect.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vIntersect );
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
			pDigimonUser->SetTargetPos( vIntersect, fConnectLength, bRenderMoveModel );
			pDigimonUser->SetDigimonUserState( CDigimonUser::DU_MANUAL_CONTROL );
			KeyReset();
		}
		break;
	default:
		assert_cs( false );
	}
}
#ifdef ZONEMAP_CLICK_MOVE
void CUserServerMove::SetPos_FromZoneMap( CsPoint ptPos, float fConnectLength /*= 0.f*/, bool bRenderMoveModel /*= true*/, bool bAttack /*= false*/ )
{
	NiPoint3 vIntersect = NiPoint3( float(ptPos.x), float(ptPos.y), 0.f);

	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		{
			assert_cs( m_pParent->GetLeafRTTI() == RTTI_TAMER_USER );
			CTamerUser* pTamer = (CTamerUser*)m_pParent;

			//=====================================================================================
			// 테이머 이동
			//=====================================================================================	
			// 테이머 와의 거리 체크
			if( ( NiPoint2( vIntersect.x, vIntersect.y ) - pTamer->GetPos2D() ).Length() > 50000.0f )
			{
				cDigimonTalk::Print( 30016 );
				return;
			}
			vIntersect.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vIntersect );
			pTamer->SetTargetPos( vIntersect, fConnectLength, bRenderMoveModel, bAttack );

			//=====================================================================================
			// 디지몬 이동
			//=====================================================================================
			if( pTamer->IsRide() == false )
			{
				CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
				if( pDigimonUser->GetDigimonUserState() == CDigimonUser::DU_FOLLOW )
				{				
					pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
				}
			}
		}
		break;

	case nLIB::SVR_BATTLE:
		break;

	default:
		assert_cs( false );
	}
}
#endif


//=============================================================================================================
//
//	키보드 이동
//
//=============================================================================================================

void CUserServerMove::ViewReset()
{	
	float fDeltaRot = m_pParent->GetCurRot() + NI_PI - CAMERA_ST.GetCurRoll();
	CsInFi( fDeltaRot );
	float fPositiveDeltaRot = abs( fDeltaRot );
	assert_cs( fPositiveDeltaRot >= 0.0f );

	float fRotSpeed = g_pResist->m_Character.s_fStaticCameraRotSpeed;
	CAMERA_ST.SetAniRotation_Roll( 0.0f, m_pParent->GetCurRot() + NI_PI, fRotSpeed + fPositiveDeltaRot/2.0f );
}

void CUserServerMove::KeyReset()
{
	m_vKeyDir = NiPoint3::ZERO;
	m_dwKeyCheck = KEY_NONE;
}

void CUserServerMove::KeyAutoMapping()
{
#ifdef KEYBOARD_MOVE
	NiPoint3 vUp = m_vMoveUpDir;
	NiPoint3 vRight = m_vMoveRightDir;
#else
	CAMERA_ST._UpdateCamera();

	NiPoint3 vRight = CAMERA_ST.GetWorldRightVector();

	NiPoint3 vUp = CAMERA_ST.GetWorldDirection();
#endif

	vRight.z = 0;
	vRight.Unitize();
	vUp.z = 0;
	vUp.Unitize();

	bool bUpdate = false;
	if( -vRight != m_vMappingKeyDir[ KMAP_LEFT ] )
	{
		m_vMappingKeyDir[ KMAP_LEFT ]	= -vRight;
		bUpdate = true;
	}

	if( vRight != m_vMappingKeyDir[ KMAP_RIGHT ] )
	{
		m_vMappingKeyDir[ KMAP_RIGHT ] = vRight;
		bUpdate = true;
	}

	if( vUp != m_vMappingKeyDir[ KMAP_UP ] )
	{
		m_vMappingKeyDir[ KMAP_UP ]		= vUp;
		bUpdate = true;
	}

	if( -vUp != m_vMappingKeyDir[ KMAP_DOWN ] )
	{
		m_vMappingKeyDir[ KMAP_DOWN ] = -vUp;
		bUpdate = true;
	}

	if( bUpdate )
		RetryKeyCheck();
}

void CUserServerMove::_Update_RetryKeyCheck()
{
	if( m_dwKeyCheck == KEY_NONE )
	{
		return;
	}

	if( m_RetryKeyCheckTimeSeq.IsEnable() == false )
		return;

	RetryKeyCheck();

}

void CUserServerMove::RetryKeyCheck()
{
	if( m_dwKeyCheck == KEY_NONE )
		return;
#ifdef KEYBOARD_MOVE
	if( cMessageBox::IsMessageBox() )
	{
		m_dwKeyCheck = KEY_NONE;
		return;
	}
#endif
	DWORD nOld = m_dwKeyCheck;
	m_dwKeyCheck = -1;
	SetKeyCheck( nOld );
}

bool CUserServerMove::CheckStateTamerMoved()
{
	if( ( m_KeyboardMoveLock & eKeyboardLock ) == eKeyboardLock )
	{
		m_dwKeyCheck = KEY_NONE;
		return false;
	}

	if( NULL == m_pParent )
	{
		m_dwKeyCheck = KEY_NONE;
		return false;
	}

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	if( NULL == pTamerUser )
	{
		m_dwKeyCheck = KEY_NONE;
		return false;
	}

	cCondition* pCondition = pTamerUser->GetCondition();
	if( NULL == pCondition )
	{
		m_dwKeyCheck = KEY_NONE;
		return false;
	}

	if( pCondition->IsCondition( nSync::Shop ) )
	{
		m_dwKeyCheck = KEY_NONE;
		return false;
	}

	if( pCondition->IsCondition( nSync::ShopReady ) )
	{
		m_dwKeyCheck = KEY_NONE;
		return false;
	}

	if( pCondition->IsCondition( nSync::Die ) )
	{
		m_dwKeyCheck = KEY_NONE;
		return false;
	}

	return true;
}

void CUserServerMove::SetKeyCheck( DWORD dwKey )
{
#ifdef KEYBOARD_MOVE
	if( g_pServerMoveOwner != this )	// 이동 주체가 자신이 아닐 때
		return;

	if( m_dwKeyCheck == dwKey )
		return;

	if( !CheckStateTamerMoved() )
		return;

	m_dwKeyCheck = dwKey;
	m_vKeyDir = NiPoint3::ZERO;

	CsC_AvObject* pMoveObject = m_pParent;
	switch( m_pParent->GetLeafRTTI() )
	{
	case RTTI_TUTORIAL_TAMER:
	case RTTI_TAMER_USER:	
		{
			if( static_cast<CTamerUser*>(m_pParent)->IsRide() )
				pMoveObject = g_pCharMng->GetDigimonUser( 0 );
		}		
		break;
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_DIGIMON_USER:
		break;
	default:
		return;
	}

	SAFE_POINTER_RET( pMoveObject );

	g_pCharResMng->ReleaseMovePoint();

	if( KEY_NONE == dwKey )
	{
		pMoveObject->DeletePath();
		return;
	}

	if( m_dwKeyCheck & KEY_LEFT )		m_vKeyDir += m_vMappingKeyDir[ KMAP_LEFT ];
	if( m_dwKeyCheck & KEY_RIGHT )		m_vKeyDir += m_vMappingKeyDir[ KMAP_RIGHT ];
	if( m_dwKeyCheck & KEY_UP )			m_vKeyDir += m_vMappingKeyDir[ KMAP_UP ];
	if( m_dwKeyCheck & KEY_DOWN )		m_vKeyDir += m_vMappingKeyDir[ KMAP_DOWN ];

	m_vKeyDir.Unitize();

	float fMoveSpeed = 0.0f;
	CsC_AvObject::sBASE_STAT* pBaseStat = pMoveObject->GetBaseStat();
	if( pBaseStat )
		fMoveSpeed = pBaseStat->GetMoveSpeed();

//	float fDist = __max(fMoveSpeed * 0.035f, 80.0f);

	float fDist = ( fMoveSpeed*0.75f > 400.0f ) ? fMoveSpeed*0.75f : 400.0f ;

	NiPoint3 vSrc = pMoveObject->GetPos();
	vSrc.z = 0;
	NiPoint3 vDst = vSrc + m_vKeyDir*fDist;
	vDst.z = 0;

	// 갱신 시간 설정
	m_RetryKeyCheckTimeSeq.OffReset();

	CsPath* pPath = pMoveObject->GetProp_Path()->GetPath();
	if( g_pCsPathEngine->PathTestLine( pPath, vDst.x, vDst.y ) == false )
	{
		// 충돌
		CsPoint res;
		if( g_pCsPathEngine->GetFirstCollisionPoint( pPath, vDst.x, vDst.y, res ) == true )
		{
			// 첫 충돌지점 검출
			NiPoint2 vCollision( (float)res.x - vSrc.x, (float)res.y - vSrc.y );			
			vDst = vSrc + m_vKeyDir*vCollision.Length();
		}
	}

	vDst.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vDst );
	switch( pMoveObject->GetLeafRTTI() )
	{
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_DIGIMON_USER:	
		{
			if( static_cast<CDigimonUser*>(pMoveObject)->GetDigimonUserState() != CDigimonUser::DU_KEYBOARD_MOVE )
			{
				static_cast<CDigimonUser*>(pMoveObject)->SetDigimonUserState( CDigimonUser::DU_KEYBOARD_MOVE );
			}
			static_cast<CDigimonUser*>(pMoveObject)->SetTargetPos( vDst, 0.0f, false );
			static_cast<CDigimonUser*>(pMoveObject)->TamerDistCheck();	//2017-01-03-nova 키보드 입력시에도 테이머와의 거리를 계속 측정하여 일정간격 유지
		}
		break;
	case RTTI_TUTORIAL_TAMER:
	case RTTI_TAMER_USER:
		{
			if( static_cast<CTamerUser*>(pMoveObject)->GetTamerUserState() != CTamerUser::TU_KEYBOARD_MOVE  )
			{
				static_cast<CTamerUser*>(pMoveObject)->SetTamerUserState( CTamerUser::TU_KEYBOARD_MOVE );	
			}
			static_cast<CTamerUser*>(pMoveObject)->SetTargetPos( vDst, 0.0f, false, false );
		}
		break;
	default:
		return;
	}

#endif	
}




//=============================================================================================================
//
//	서버 이동
//
//=============================================================================================================
void CUserServerMove::DeletePath()
{
	if( g_pServerMoveOwner == this || g_pCharMng->GetTamerUser()->IsRide() )
	{
		g_pCharResMng->ReleaseMovePoint();
	}

	//m_LastSyncPos.m_nX = (int)0;
	//m_LastSyncPos.m_nY = (int)0;

	if( net::game )
	{
		NiPoint2 vPos = m_pParent->GetPos2D();
		__SendMoveTo( m_pParent->GetUniqID(), vPos, vPos, m_pParent->GetCurRot(), 0 );
	}
}

void CUserServerMove::SetServerMoveReady()
{
//#ifdef CRASHDUMP_0305			//chu8820 - Path 정보 로드 실패 ( C011 )
	if( m_pParent == NULL )
	{
		//DUMPLOG( " C011 - 1 " );
		return;
	}
	else if( m_pParent->GetProp_Path() == NULL )
	{
		//DUMPLOG( " C011 - 2 " );
		return;
	}
//#endif

	CsC_AvObject* pParent = m_pParent;

#ifdef KEYBOARD_MOVE
	if( g_pResist->m_Global.s_bMoveDigimon == false && g_pCharMng->GetTamerUser()->IsRide() )
	{
 		pParent = g_pCharMng->GetDigimonUser(0);
		g_pCharMng->GetDigimonUser(0)->GetUserServerMove()->_SetIsNewPath( true );
	}
#endif
	if( pParent->GetProp_Path()->IsMove() == true )
	{
		m_nOldTotalSize = pParent->GetProp_Path()->GetPath()->m_iPath->size();
		m_bNewPath = true;
	}
}

#ifdef CRASHDUMP_0424
void CUserServerMove::SetServerMoveReady2( CsC_PathProp* pPathProp )
{
	if( pPathProp == NULL )
	{
		DUMPLOG( "L0004 패쓰 정보가 NULL" );
		return;
	}

	if( pPathProp->IsMove() == true )
	{
		m_nOldTotalSize = pPathProp->GetPath()->m_iPath->size();
		m_bNewPath = true;
	}
}
#endif

void CUserServerMove::SetServerMove( uint nUID, float fCurX, float fCurY, float fRot, iPath* pPath )
{
	// 서버상에서의 이동
#ifndef _GIVE
	if( net::game == NULL )
	{
		m_LastSyncPos.m_nX = (int)fCurX;
		m_LastSyncPos.m_nY = (int)fCurY;
		return;
	}
#endif

	if( pPath == NULL )
	{
		if( ( m_LastSyncPos.m_nX == (int)fCurX )&&( m_LastSyncPos.m_nY == (int)fCurY ) )
		{
			return;
		}

		NiPoint2 Sync = NiPoint2( (float)m_LastSyncPos.m_nX, (float)m_LastSyncPos.m_nY );
		if( __SendMoveTo( nUID, Sync, NiPoint2( fCurX, fCurY ), fRot, 0 ) == false )
		{
			m_pParent->SetPos( Sync );
		}
		else
		{
			Sync = NiPoint2( (float)m_LastSyncPos.m_nX, (float)m_LastSyncPos.m_nY );
			m_pParent->SetPos( Sync );
		}
		return;
	}

	bool bNewPath = m_bNewPath;
#ifdef KEYBOARD_MOVE
	if( g_pResist->m_Global.s_bMoveDigimon == false && g_pCharMng->GetTamerUser()->IsRide() )
	{
		bNewPath = g_pCharMng->GetDigimonUser(0)->GetUserServerMove()->_IsNewPath();
	}
#endif
	// 새롭게 설정된 패쓰가 아니라면
	if( ( bNewPath == false )&&( pPath->size() > 1 ) )
	{		
		// 현재 무브포인트 갯수 알아 온다
		long nCurTotalSize = pPath->size();
		// 예전 무브포인트 갯수와 틀리다면
		while( m_nOldTotalSize != nCurTotalSize )
		{
			if( m_nOldTotalSize <= nCurTotalSize 
				|| nCurTotalSize <= 1 )
				break;
			assert_cs( m_nOldTotalSize > nCurTotalSize );
			assert_cs( nCurTotalSize > 1 );
			--m_nOldTotalSize;
			assert_cs( pPath->size() > 1 );
			cPosition cPos = pPath->position( 1 );
			NiPoint2 vPos = NiPoint2( (float)cPos.x, (float)cPos.y );
			__SendMoveTo( nUID, vPos, vPos, fRot, 0 );
		}
	}
	else
	{
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon == false && g_pCharMng->GetTamerUser()->IsRide() )
			g_pCharMng->GetDigimonUser(0)->GetUserServerMove()->_SetIsNewPath( false );
		else
#endif
		m_bNewPath = false;

		if( pPath->size() > 1 )
		{
			cPosition cPosStart = pPath->position( 0 );
			cPosition cPosTarget = pPath->position( 1 );
			NiPoint2 vStart = NiPoint2( (float)cPosStart.x, (float)cPosStart.y );
			NiPoint2 vEnd = NiPoint2( (float)cPosTarget.x, (float)cPosTarget.y );
			__SendMoveTo( nUID, vStart, vEnd, vStart, vEnd, fRot );
		}
		else
		{
			assert_cs( pPath->size() == 1 );
			cPosition cPos = pPath->position( 0 );
			NiPoint2 vPos = NiPoint2( (float)cPos.x, (float)cPos.y );
			__SendMoveTo( nUID, vPos, vPos, fRot, 0 );
		}
	}
}

void CUserServerMove::SetServerInitPos( NiPoint3 vPos )
{
	assert_cs( nsCsGBTerrain::g_pCurRoot != NULL );
	__SetServerInitPos( NiPoint2( vPos.x, vPos.y ), NiPoint2( vPos.x, vPos.y ), 0 );
}


void CUserServerMove::__GetNextPos( NiPoint2 vOrg, NiPoint2& vPos, int nIndex )
{
	switch( nIndex )
	{
	case 0:
		vPos.x  = vOrg.x;
		vPos.y  = vOrg.y;
		break;
	case 1:
		vPos.x = vOrg.x - nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		vPos.y = vOrg.y;
		break;
	case 2:
		vPos.x = vOrg.x + nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		vPos.y = vOrg.y;
		break;
	case 3:
		vPos.x = vOrg.x;
		vPos.y = vOrg.y - nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		break;
	case 4:
		vPos.x = vOrg.x;
		vPos.y = vOrg.y + nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		break;
	case 5:
		vPos.x = vOrg.x - nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		vPos.y = vOrg.y - nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		break;
	case 6:
		vPos.x = vOrg.x + nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		vPos.y = vOrg.y - nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		break;
	case 7:
		vPos.x = vOrg.x - nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		vPos.y = vOrg.y + nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		break;
	case 8:
		vPos.x = vOrg.x + nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		vPos.y = vOrg.y + nsCsGBTerrain::g_pCurRoot->GetEmrBScale();
		break;
	case 9:
		vPos.x = vOrg.x - nsCsGBTerrain::g_pCurRoot->GetEmrBScale()*2;
		vPos.y = vOrg.y;
		break;
	case 10:
		vPos.x = vOrg.x + nsCsGBTerrain::g_pCurRoot->GetEmrBScale()*2;
		vPos.y = vOrg.y;
		break;
	case 11:
		vPos.x = vOrg.x;
		vPos.y = vOrg.y - nsCsGBTerrain::g_pCurRoot->GetEmrBScale()*2;
		break;
	case 12:
		vPos.x = vOrg.x;
		vPos.y = vOrg.y + nsCsGBTerrain::g_pCurRoot->GetEmrBScale()*2;
		break;
	default:
		assert_csm2( false, "__GetNextPos 를 찾을수 없습니다\nPosX = %d, posY = %d", (int)vOrg.x, (int)vOrg.y );
	}
}

void CUserServerMove::__SetServerInitPos( NiPoint2 vOrg, NiPoint2 vPos, int nIndex )
{
	while(true)
	{
		if( nsCsGBTerrain::g_pCurRoot->IsValidEmr2( (uint)vPos.x, (uint)vPos.y ) )
		{
			nSync::Pos pos;
			pos.m_nX = (int)vPos.x;
			pos.m_nY = (int)vPos.y;
			net::game->SendReady( pos );
			return;
		}

		if( nIndex == 12 )
		{
			nSync::Pos pos = g_pCharMng->GetTamerUser()->GetUserServerMove()->GetLastSyncPos();
			net::game->SendReady( pos );
			return;
		}
		__GetNextPos( vOrg, vPos, ++nIndex );
	}
}

bool CUserServerMove::__SendMoveTo( uint nUID, NiPoint2 vOrg, NiPoint2 vPos, float fRot, int nIndex )
{
	if( ( m_LastSyncPos.m_nX == (int)vPos.x )&&( m_LastSyncPos.m_nY == (int)vPos.y ) )
	{
		return false;
	}

	if( nsCsGBTerrain::g_pCurRoot->IsValidEmr( vPos ) == true )
	{
		m_LastSyncPos.m_nX = (int)vPos.x;
		m_LastSyncPos.m_nY = (int)vPos.y;

		_SendMove_Server( nUID, m_LastSyncPos, fRot );
		return true;
	}

	if( nIndex == 12 )
	{
		m_LastSyncPos.m_nX = (int)vOrg.x;
		m_LastSyncPos.m_nY = (int)vOrg.y;

		_SendMove_Server( nUID, m_LastSyncPos, fRot );		
		return true;
	}

	__GetNextPos( vOrg, vPos, ++nIndex );
	if( __SendMoveTo( nUID, vOrg, vPos, fRot, nIndex ) == true )
		return true;

	return false;


	/*if( nIndex<24 )
	{
	__GetNextPos( vOrg, vPos, ++nIndex );
	__SendMoveTo( nUID, vOrg, vPos, fRot, nIndex );
	}
	else
	{
	m_LastSyncPos.m_nX = (int)vOrg.x;
	m_LastSyncPos.m_nY = (int)vOrg.y;
	net::game->SendMoveTo( nUID, m_LastSyncPos.m_nX, m_LastSyncPos.m_nY, fRot );
	}*/
}

bool CUserServerMove::__SendMoveTo( uint nUID, NiPoint2 vOrgStart, NiPoint2 vOrgEnd, NiPoint2 vStart, NiPoint2 vEnd, float fRot )
{
	for( int i_1=0; i_1<13; ++i_1 )
	{
		__GetNextPos( vOrgStart, vStart, i_1 );
		if( nsCsGBTerrain::g_pCurRoot->IsValidEmr( vStart ) == false )
			continue;
		for( int i_2=0; i_2<13; ++i_2 )
		{			
			__GetNextPos( vOrgEnd, vEnd, i_2 );

			if( nsCsGBTerrain::g_pCurRoot->IsValidEmr( vEnd ) == false )
				continue;

			m_LastSyncPos.m_nX = (int)vEnd.x;
			m_LastSyncPos.m_nY = (int)vEnd.y;
			nSync::Pos sp;
			sp.m_nX = (int)vStart.x;
			sp.m_nY = (int)vStart.y;

			_SendMove_Server( nUID, sp, m_LastSyncPos, fRot );
			return true;			
		}
	}

	//assert_cs( false );
	m_LastSyncPos.m_nX = (int)vOrgEnd.x;
	m_LastSyncPos.m_nY = (int)vOrgEnd.y;

	_SendMove_Server( nUID, m_LastSyncPos, fRot );
	return false;
}


//=============================================================================================================
//
//	서버 이동 전송
//
//=============================================================================================================

void CUserServerMove::_SendMove_Server( uint nUID, nSync::Pos SyncPos, float fRot )
{
	if( m_SendMoveTimeSeq.IsEnable() )
	{
		m_SendMoveInfo.s_nType = 0;
		net::game->SendMoveTo( nUID, SyncPos, fRot );
	}
	else
	{
		m_SendMoveInfo.s_nType = 1;
		m_SendMoveInfo.s_nUID = nUID;
		m_SendMoveInfo.s_TargetPos = SyncPos;
		m_SendMoveInfo.s_fRot = fRot;
	}
}

void CUserServerMove::_SendMove_Server( uint nUID, nSync::Pos sp, nSync::Pos tp, float fRot )
{
	if( m_SendMoveTimeSeq.IsEnable() )
	{
		m_SendMoveInfo.s_nType = 0;
		net::game->SendMoveTo( nUID, tp, fRot );
	}
	else
	{
		m_SendMoveInfo.s_nType = 1;
		m_SendMoveInfo.s_nUID = nUID;
		m_SendMoveInfo.s_StartPos = sp;
		m_SendMoveInfo.s_TargetPos = tp;
		m_SendMoveInfo.s_fRot = fRot;
	}
}

void CUserServerMove::Update_SendMove_Server()
{
	_Update_RetryKeyCheck();

	if( m_SendMoveInfo.s_nType == 0 )
		return;

	if( m_SendMoveTimeSeq.IsEnable() == false )
		return;

	switch( m_SendMoveInfo.s_nType )
	{
	case 1:
		net::game->SendMoveTo( m_SendMoveInfo.s_nUID, m_SendMoveInfo.s_TargetPos, m_SendMoveInfo.s_fRot );
		break;
	case 2:
		net::game->SendMoveTo( m_SendMoveInfo.s_nUID, m_SendMoveInfo.s_StartPos, m_SendMoveInfo.s_TargetPos, m_SendMoveInfo.s_fRot );
		break;
	default:
		assert_cs( false );
	}

	m_SendMoveInfo.s_nType = 0;
}

void CUserServerMove::SetMoveLock(short const& lockType)
{
	m_KeyboardMoveLock = lockType;
}

#ifdef KEYBOARD_MOVE
void CUserServerMove::SetMoveOption( bool bDigimonMove )
{
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pTamerUser);
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET(pDigimonUser);

	CUserServerMove* pChangeMovePointer = NULL;
	if( bDigimonMove )
		pChangeMovePointer = pDigimonUser->GetUserServerMove();
	else
		pChangeMovePointer = pTamerUser->GetUserServerMove();

	if( pChangeMovePointer == g_pServerMoveOwner )
		return;

	g_pServerMoveOwner = pChangeMovePointer;
	SAFE_POINTER_RET(g_pServerMoveOwner);

	CsC_AvObject::sBASE_STAT* pTBaseST = pTamerUser->GetBaseStat();
	SAFE_POINTER_RET(pTBaseST);
	CsC_AvObject::sBASE_STAT* pDBaseST = pDigimonUser->GetBaseStat();
	SAFE_POINTER_RET(pDBaseST);

	// 이동 스피트 스왑
	float fSpeed = pTBaseST->GetMoveSpeed();
	pTBaseST->SetMoveSpeed( pDBaseST->GetMoveSpeed() );
	pDBaseST->SetMoveSpeed( fSpeed );

	if( bDigimonMove )
	{
		// 디지몬 상태
		pDigimonUser->SetDigimonUserState( CDigimonUser::DU_WAIT );
		// 테이머 상태
		pTamerUser->SetTamerUserState( CTamerUser::TU_FOLLOW_DIGIMON );
	}
	else
	{
		// 디지몬 상태
		pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
		// 테이머 상태
		pTamerUser->SetTamerUserState( CTamerUser::TU_NONE );
	}

	g_pServerMoveOwner->m_bCurState = bDigimonMove;
	g_pServerMoveOwner->m_bBackupState = !bDigimonMove;
	g_pServerMoveOwner->SetKeyCheck( KEY_NONE );
}
#endif