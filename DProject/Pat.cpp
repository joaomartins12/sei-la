
#include "stdafx.h"
#include "Pat.h"


THREAD_MEMPOOL_CPP( CPat )
#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CPat, RTTI_PAT )

#define MIN_MOVESPEED		100.0f

CPat::CPat()
{
	m_pFT_Digimon = NULL;
	m_pParent = NULL;
	m_pCurPos = NULL;
	m_nDestPosArrayIndex = 0;
}

void CPat::Delete()
{
	PARENT_CLASS::Delete();

	m_pFT_Digimon = NULL;	
	m_pParent = NULL;

	m_vpDestPos.Destroy();
}

void CPat::PreDelete()
{
	PARENT_CLASS::PreDelete();
}

void CPat::Init( CsC_AvObject* pParent, uint nUID, uint nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
{
	// 파일 테이블
	m_pFT_Digimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nFileTableID );	
	// 초기화
	PARENT_CLASS::Init( nUID, nFileTableID, vPos, fRot, pProp );
	m_pParent = pParent;

	_Init();
}

void CPat::_Init()
{
	assert_cs( g_pPatAIMng );
	GetProp_Animation()->SetUseIdleAni( true );
	//_UpdatePos();

// 	NiPoint3 vPath = m_pParent->GetPos();
// 	NiPoint3 vDir = NiPoint3( 0, m_pParent->GetDir().y, 0 );
// 	vPath -= vDir * 100.0f;
// 	vPath += NiPoint3( 0, 0, 1 ).Cross( vDir ) * 30.f;
// 	vPath.z += m_pParent->GetToolHeight();
// 	SetPos( vPath );

	NiPoint3 vPath = m_pParent->GetPos();
	vPath.y += m_pParent->GetToolWidth() * 2.0f;
	vPath.z += m_pParent->GetToolHeight();
	SetPos( vPath );
	SetRotation( m_pParent->GetCurRot(), true );

	//CPat_AI_Mng::sAI_Container* pTemp = g_pPatAIMng->GetContainer( g_pPatAIMng->GetContainerStartID() );

	//_SetAI( (CPat_AI_Mng::sAI_Element*)pTemp->GetNext() );
}

void CPat::_UpdatePos()
{
	//====================================================================================================
	// 위치 셋팅
	//====================================================================================================
	

	CsNodeObj* pNodeObj = m_pParent->GetCsNode();
	if( pNodeObj == NULL )
		return;

	NiAVObject* pNode  = m_pParent->GetCsNode()->m_pNiNode->GetObjectByName( "Bip01 L Clavicle" );
	
	if( pNode == NULL )
	{
		//assert_csm1( false, _T( "해당 디지몬에 ride 더미본이 존재 하지 않습니다. 디지몬ID = %d" ), GetFTID() );
		SetPos( m_pParent->GetPos() );
		return ;
	}

	
	NiPoint3 vPath = pNode->GetWorldTranslate();
	NiPoint3 vDir = NiPoint3( m_pParent->GetDir().x, m_pParent->GetDir().y, 0 );
	vPath -= vDir * 10.0f;
	vPath += NiPoint3( 0, 0, 1 ).Cross( vDir ) * 30.f;
	vPath.z += (m_pParent->GetToolHeight() *0.1f);
	SetPos( vPath );

	SetRotation( m_pParent->GetCurRot(), true );

}

void CPat::Update( float fDeltaTime, bool bAnimation )
{
	_UpdatePos();

	

/*

	switch( m_pAI->s_eCheckType )
	{
	case CPat_AI_Mng::CHK_ANI:													break;
	case CPat_AI_Mng::CHK_TIME:		_Update_AI_Time( fDeltaTime );				break;
//	case CPat_AI_Mng::CHK_MOVE:		_Update_AI_Move( fDeltaTime );				break;
	default:						assert_cs( false );
	
	}	
*/
	PARENT_CLASS::Update( fDeltaTime, bAnimation );

}


void CPat::_SetAI( CPat_AI_Mng::sAI_Element* pAI )
{
	assert_cs( pAI != NULL );
	m_pAI = pAI;

	switch( m_pAI->s_eCheckType )
	{
//	case CPat_AI_Mng::CHK_MOVE:		_Set_AI_Move( m_pAI );			break;
	case CPat_AI_Mng::CHK_ANI:		_Set_AI_Animation( m_pAI );		break;
	case CPat_AI_Mng::CHK_TIME:		_Set_AI_Time( m_pAI );			break;
	default:						assert_cs( false );
	}
}

//===============================================================================================================
//
//		시간
//
//===============================================================================================================
void CPat::_Set_AI_Time( CPat_AI_Mng::sAI_Element* pAI )
{
	assert_cs( m_pAI->s_eCheckType == CPat_AI_Mng::CHK_TIME );
	SetAnimation( m_pAI->s_eAnimation );
	m_TimeSeq.SetDeltaTime( CsFloat2Int( pAI->s_Time.s_fTimeSec*1000 ) );
	m_TimeSeq.OffReset();
}

void CPat::_Update_AI_Time( float fDeltaTime )
{
	assert_cs( m_pAI->s_eCheckType == CPat_AI_Mng::CHK_TIME );
	if( m_TimeSeq.IsEnable() == false )
		return;

	_SetAI( (CPat_AI_Mng::sAI_Element*)m_pAI->GetNext() );
}


//===============================================================================================================
//
//		애니메이션
//
//===============================================================================================================

void CPat::_Set_AI_Animation( CPat_AI_Mng::sAI_Element* pAI )
{
	assert_cs( m_pAI->s_eCheckType == CPat_AI_Mng::CHK_ANI );
	SetAnimation( m_pAI->s_eAnimation );

	//GetProp_Animation->Get
	m_nAniLoopCount = pAI->s_Animation.s_nLoopCount;
}

void CPat::_Check_AI_Animation( bool& bResetAni )
{
	bResetAni = false;
	if( m_pAI->s_eCheckType != CPat_AI_Mng::CHK_ANI )
		return;

	if( (--m_nAniLoopCount) != 0 )
	{
		bResetAni = true;
		return;
	}	

	ANI::eTYPE oldAni = m_pAI->s_eAnimation;
	CPat_AI_Mng::sAI_Element* pNext = (CPat_AI_Mng::sAI_Element*)m_pAI->GetNext();
	// 다음의 체크타입이 ANI이고 - 애니메이션이 똑같다면 리셋
	if( pNext->s_eCheckType == CPat_AI_Mng::CHK_ANI )
		bResetAni = ( pNext->s_eAnimation == oldAni );

	_SetAI( pNext );
}

//===============================================================================================================
//
//		이동
//
//===============================================================================================================
void CPat::_Set_AI_Move( CPat_AI_Mng::sAI_Element* pAI )
{
	// 루아쪽에 Act 이벤트 발생


	
	m_nDestPosArrayIndex = 0;
	m_vpDestPos.ClearElement();
	SetAnimation( pAI->s_eAnimation );
	

	_SetAI_Move_Tamer_Forward( pAI );			

	
	if( _NextMovePoint() == false )
	{
		assert_cs( false );
	}
}

bool CPat::_NextMovePoint()
{
	if( m_pAI->s_eCheckType != CPat_AI_Mng::CHK_MOVE )
	{
		m_pCurPos = NULL;
		return false;
	}

	if( m_nDestPosArrayIndex >= m_vpDestPos.Size() )
	{
		m_pCurPos = NULL;
		return false;
	}

	m_pCurPos = &m_vpDestPos[ m_nDestPosArrayIndex ];
	if(GetBaseStat())
		GetBaseStat()->SetMoveSpeed( m_pCurPos->s_fSpeed );
	++m_nDestPosArrayIndex;
	return true;
}


void CPat::_Update_AI_Move( float fDeltaTime )
{
	assert_cs( m_pAI->s_eCheckType == CPat_AI_Mng::CHK_MOVE );
	assert_cs( m_pCurPos != NULL );

	bool bArrive = false;
	m_pCurPos->s_fCurTime += fDeltaTime;
	if( m_pCurPos->s_fCurTime > m_pCurPos->s_fEndTime )
	{
		m_pCurPos->s_fCurTime = m_pCurPos->s_fEndTime;
		bArrive = true;
	}

	NiPoint3 vResult = nsCSGBFUNC::CalMullRom(	&m_pCurPos->s_vBegin,
												&m_pCurPos->s_vSrcPos,
												&m_pCurPos->s_vDestPos,
												&m_pCurPos->s_vEnd,
												m_pCurPos->s_fCurTime / m_pCurPos->s_fEndTime
											);
	vResult.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vResult );
	SetRotation_Ani( vResult );
	SetPos( vResult );

	if( bArrive == true )
	{
		NiPoint3 vEndPos = m_pCurPos->s_vEnd;
		if( _NextMovePoint() == false )
		{
			SetRotation_Ani( vEndPos );
			_SetAI( (CPat_AI_Mng::sAI_Element*)m_pAI->GetNext() );
		}
	}
}

void CPat::Render()
{
	PARENT_CLASS::Render();
}

void CPat::Render_PostChar()
{
}

//=============================================================================================================
//
//	CallBack
//
//=============================================================================================================

bool CPat::CallBack_EndofSequence( DWORD dwAniID )
{
	/*
	bool bResetAni = false;

	switch( m_pAI->s_eCheckType )
	{
	case CPat_AI_Mng::CHK_MOVE:
	case CPat_AI_Mng::CHK_TIME:
		bResetAni = true;
		break;
	case CPat_AI_Mng::CHK_ANI:
		_Check_AI_Animation( bResetAni );
		break;
	default:
		assert_cs( false );
	}
	

	if( bResetAni == true )
		GetProp_Animation()->ResetAnimation();
	*/
	return PARENT_CLASS::CallBack_EndofSequence( dwAniID );
}

//=============================================================================================================
//
//	Path
//
//=============================================================================================================
void CPat::DeletePath()
{
	_SetDefault_IdleAnimation_Stand();
}

void CPat::ArriveTarget()
{
	_NextMovePoint();
}

//====================================================================================
//		Property
//====================================================================================
void CPat::_SetDefault_IdleAnimation_Stand()
{
	GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
}

void CPat::_SetDefault_IdleAnimation_Move()
{
	GetProp_Animation()->SetAnimation( ANI::MOVE_WALK );
}

//====================================================================================
//		Sound
//====================================================================================
void* CPat::PlaySound( char* cSoundPath )
{
	return g_pSoundMgr->PlaySystemSound( cSoundPath, GetPos(), this );
}
//====================================================================================================
//	부가 정보
//====================================================================================================
bool CPat::IsPickEnable()
{ 
	return false; 
}
UINT CPat::GetModelID() const
{ 
	return m_pFT_Digimon->GetInfo()->s_dwModelID;
}
float CPat::_GetAniTimeMoveDist_Walk()
{ 
	return m_pFT_Digimon->GetInfo()->s_fWakkLen;
}
float CPat::_GetAniTimeMoveDist_Run()
{ 
	return m_pFT_Digimon->GetInfo()->s_fRunLen
		; }
float CPat::_GetAniTimeMoveDist_ARun()
{ 
	return m_pFT_Digimon->GetInfo()->s_fARunLen;
}
float CPat::CalIdleAniTime()
{ 
	return 3.0f + (float)( rand()%2 );
}
ANI::eTYPE CPat::GetRandomIdle()
{ 
	return ( rand()%5 == 0 ) ? ANI::IDLE_SHAKE : ANI::IDLE_CHAR;
}

#undef		PARENT_CLASS