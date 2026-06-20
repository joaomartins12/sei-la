

#include "stdafx.h"
#include "AniProperty_Sequence.h"

CsC_AniProp_Sequence::CsC_AniProp_Sequence()
{
	m_dwSrcID = ANI::INVALIDE_ID;
	m_dwDestID = ANI::INVALIDE_ID;
	m_pLoadSequence = NULL;
	m_pParent = NULL;
	m_pNAtt_EffectIndexArray = NULL;
	m_dwLastID = 0;
	m_bChangedShader = false;
	m_pbLastID.ClearElement();
	m_dwLastDestID = 0;
	m_fAniTime = 0.0f;
	m_bLoop = false;
	m_nAniLoopCount = 0;
	m_fOldExceedTime = 0.0f;
	m_fStartTime = 0.0f;
	m_fEndTime = 0.0f;
	m_nActiveEventIndex = 0;
	m_nNAtt_Count = 0;
	m_nLast_NAtt_SoundIndex = 0;
	m_pSrcSequence = NULL;
}

CsC_AniProp_Sequence::~CsC_AniProp_Sequence()
{
	m_pParent = NULL;
	m_pLoadSequence = NULL;
	m_pSrcSequence = NULL;
}

void CsC_AniProp_Sequence::Delete()
{
	m_dwSrcID = ANI::INVALIDE_ID;
	m_dwDestID = ANI::INVALIDE_ID;
	// 쓰레드 리로스 매니져에서 지워주므로 포인터만 널로 만들면 된다.
	m_pLoadSequence = NULL;
	m_pParent = NULL;
	m_pSrcSequence = NULL;
	m_pNAtt_EffectIndexArray = NULL;
	m_dwLastID = 0;
	m_pbLastID.ClearElement();
	m_bChangedShader = false;
	m_dwLastDestID = 0;
	m_fAniTime = 0.0f;
	m_bLoop = false;
	m_nAniLoopCount = 0;
	m_fOldExceedTime = 0.0f;
	m_fStartTime = 0.0f;
	m_fEndTime = 0.0f;
	m_nActiveEventIndex = 0;
	m_nNAtt_Count = 0;
	m_nLast_NAtt_SoundIndex = 0;
}

void CsC_AniProp_Sequence::DeleteEventEffect()
{
	if( m_pParent->IsProperty( CsC_Property::EFFECT ) )
		m_pParent->GetProp_Effect()->DeleteEventEffect();
}

void CsC_AniProp_Sequence::Init( CsC_AvObject* pParent, CsC_LoadSequence* pLoadSequence )
{
	assert_cs( m_dwSrcID == ANI::INVALIDE_ID );
	assert_cs( m_pParent == NULL );
	assert_cs( m_pLoadSequence == NULL );

	m_pParent = pParent;
	m_pLoadSequence = pLoadSequence;

	m_pSrcSequence = NULL;
	m_nActiveEventIndex = CSC_ANIPROP_SEQUENCE_NONE_EVENT;
	
	m_fOldExceedTime = 0;
	m_nNAtt_Count = 0;

	m_EventOption.Reset();

	m_bChangedShader = false;
}

bool CsC_AniProp_Sequence::Update( float fDeltaTime )
{
	bool bReturn = false;
	if( m_dwDestID != m_dwLastDestID )
	{
		m_dwDestID = m_dwLastDestID;
		m_fStartTime = FLT_MAX;	
		DeleteEventEffect();
	}
	assert_cs( m_dwDestID != ANI::INVALIDE_ID );

	if( m_fStartTime == FLT_MAX )
	{
		_FirstUpdate( fDeltaTime );
		bReturn = true;
	}
	// FirstUpdate 에서 사용 하였으므로 계속 초기화 해주자
	m_fOldExceedTime = 0.0f;
	m_fAniTime += fDeltaTime;

	// 이벤트는 무조건 발생
	if( m_nActiveEventIndex != CSC_ANIPROP_SEQUENCE_NONE_EVENT )
	{
		while( m_pSrcSequence->s_vpTextKeyEvent.IsExistElement( m_nActiveEventIndex ) == true )
		{
			CsC_LoadSequence::sTEXTKEY_EVENT* pEvent = m_pSrcSequence->s_vpTextKeyEvent[ m_nActiveEventIndex ];
			if( m_fAniTime < pEvent->s_fEventTime )
				break;

			++m_nActiveEventIndex;
			
			if( ( pEvent->s_dwPlag & nsEFFECT::EVENT_ONCE ) != 0 ) 
			{
				if( m_dwSrcID != m_dwLastID ) 
				{
					m_pParent->CallBack_EventSequence( m_dwSrcID, pEvent, &m_EventOption );	
				}
			}
			else if( ( pEvent->s_dwPlag & nsEFFECT::EVENT_ONLY_ONE ) != 0 )
			{
				DWORD dwUID = m_dwSrcID * 1000 + m_nActiveEventIndex;
				if( m_pbLastID.IsExistData( dwUID ) == false )
				{
					m_pbLastID.PushBack( dwUID );
					m_pParent->CallBack_EventSequence( m_dwSrcID, pEvent, &m_EventOption );	
				}
			}
			else
			{
				m_pParent->CallBack_EventSequence( m_dwSrcID, pEvent, &m_EventOption );			
			}			

		}
	}

	if( m_fAniTime >= m_fEndTime )
	{
		m_dwLastID = m_dwSrcID;

		if( m_bLoop )
		{
			m_fAniTime -= m_fEndTime;
			if( m_nActiveEventIndex != CSC_ANIPROP_SEQUENCE_NONE_EVENT )
			{
				m_nActiveEventIndex = CSC_ANIPROP_SEQUENCE_INIT_EVENT;
			}
		}
		else
		{
			if( m_nAniLoopCount > 0 )
				--m_nAniLoopCount;

			if( m_nAniLoopCount == 0 )
			{
				int nPower = (int)( m_fAniTime / m_fEndTime );
				m_fOldExceedTime = m_fAniTime - nPower*m_fEndTime;
				if( m_pParent->CallBack_EndofSequence( m_dwSrcID ) == true )
				{
				}
			}
			else
			{
				m_fAniTime -= m_fEndTime;
				if( m_nActiveEventIndex != CSC_ANIPROP_SEQUENCE_NONE_EVENT )
				{
					m_nActiveEventIndex = CSC_ANIPROP_SEQUENCE_INIT_EVENT;
				}
				m_pParent->GetProp_Animation()->GetActorMng()->GetSequence( m_dwSrcID )->ResetSequence();
			}
		}
	}
	return bReturn;
}

void CsC_AniProp_Sequence::_FirstUpdate( float fDeltaTime )
{
	m_dwSrcID = m_dwDestID;
	m_fStartTime = fDeltaTime;
	m_dwLastID = 0;
	// 엑시드 타임 10% 늘려 준다.	
	m_fAniTime = m_fOldExceedTime*1.1f;

	m_pSrcSequence = m_pLoadSequence->GetSequence( m_dwSrcID );

	if( m_pSrcSequence == NULL )
	{
		m_bLoop = true;
		m_fEndTime = FLT_MAX;
		m_nActiveEventIndex = CSC_ANIPROP_SEQUENCE_NONE_EVENT;
		return;
	}

	// 콜백 호출
	m_pParent->CallBack_StartSequence( m_dwSrcID );

	m_fEndTime = m_pSrcSequence->s_fEndTime + m_fStartTime - 0.01f;
	// 루프
	m_bLoop = ( m_pSrcSequence->s_eCyleType == NiTimeController::LOOP );

	m_nAniLoopCount = m_pSrcSequence->s_nLoopCount;

	// 만약 이벤트가 존재 한다면 0으로 셋팅 존재 하지 않는다면 -1 로 셋팅
	if( m_pSrcSequence->s_vpTextKeyEvent.Size() != 0 )
	{
		m_nActiveEventIndex = CSC_ANIPROP_SEQUENCE_INIT_EVENT;
		m_nNAtt_Count = m_pSrcSequence->s_nNAtt_Count;
		m_pNAtt_EffectIndexArray = &m_pSrcSequence->s_vpNAtt_EffectIndexArray;
		m_nLast_NAtt_SoundIndex = m_pSrcSequence->s_nLast_NAtt_SoundIndex;
	}
	else
	{
		m_nActiveEventIndex = CSC_ANIPROP_SEQUENCE_NONE_EVENT;
		m_nNAtt_Count = 0;
	}
	
	CsNodeObj* pNodeObj = m_pParent->GetCsNode();
	if( pNodeObj == NULL )
		return;

	bool bCallActiveShader = false;	
	//============================================================================================
	// 기존에 쉐이더가 셋팅되었다면 뒤로 돌려 준다.
	//============================================================================================
	if( m_bChangedShader && pNodeObj->LoadNiBit() )
	{
		int nGeomCnt = pNodeObj->m_vpGeom.Size();
		for( int nGeom=0; nGeom<nGeomCnt; ++nGeom )
		{
			pNodeObj->m_vpGeom[ nGeom ]->DeleteShader();
		}
		bCallActiveShader = true;
	}

	//============================================================================================
	// 쉐이더 존재 한다면 쉐이더 셋팅.
	//============================================================================================
	int nCnt = m_pSrcSequence->s_vpTextKeyShader.Size();
	for( int i=0; i<nCnt; ++i )
	{
		CsC_LoadSequence::sTEXTKEY_SHADER* pShader = m_pSrcSequence->s_vpTextKeyShader[ i ];		

		switch( pShader->s_eShaderType )
		{
		case CsC_LoadSequence::sTEXTKEY_SHADER::ST_ALPHATEX:
			{
				CGeometry* pGeom = NULL;
				int nGeomCnt = pNodeObj->m_vpGeom.Size();
				for( int nGeom=0; nGeom<nGeomCnt; ++nGeom )
				{
					pGeom = pNodeObj->m_vpGeom[ nGeom ];
					pGeom->m_pGeometry->SetBit( true, NiAVObject::IS_ALPHATEX_SHADER );
				}

				bCallActiveShader = true;
				m_bChangedShader = true;
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_SHADER::ST_BLUR:
			{
				// 저장 되어진 메쉬를 찾는다
				NiAVObject* pNiObj = pNodeObj->m_pNiNode->GetObjectByName( pShader->s_cApplyObjectName );
				if( pNiObj )
				{
					CGeometry* pGeom = NULL;
					int nGeomCnt = pNodeObj->m_vpGeom.Size();
					for( int nGeom=0; nGeom<nGeomCnt; ++nGeom )
					{
						if( pNodeObj->m_vpGeom[ nGeom ]->m_pGeometry == pNiObj )
						{
							pGeom = pNodeObj->m_vpGeom[ nGeom ];
							break;
						}
					}
					if( pGeom != NULL )
					{					
						pNiObj->SetBit( true, NiAVObject::IS_BLUR_SHADER );

						sNODE_SHADER_BLUREFFECT* pNS = sNODE_SHADER_BLUREFFECT::NewInstance();
						pNS->Init( pShader->s_fValue1, pShader->s_fValue2, pShader->s_fValue3, pShader->s_nValue1 );
						pGeom->m_listShader.push_back( pNS );

						bCallActiveShader = true;
						m_bChangedShader = true;
					}
				}
				else
				{
					assert_csm1( false, "%s 이름의 메쉬가 존재 하지 않습니다", pShader->s_cApplyObjectName );
				}
			}
			break;
		default:
			assert_cs( false );
		}
	}

	if( bCallActiveShader == true )
		pNodeObj->ActiveShader();
}

void CsC_AniProp_Sequence::SetTargetAnimation( DWORD dwAniID, bool bUseStopFrame )
{
	assert_cs( dwAniID != ANI::INVALIDE_ID );
	m_dwLastDestID = dwAniID;
}

void CsC_AniProp_Sequence::ResetAnimation()
{
	assert_cs( m_dwSrcID != ANI::INVALIDE_ID );
	m_dwDestID = m_dwSrcID;

	m_fStartTime = FLT_MAX;
	m_fOldExceedTime = 0.0f;
	m_dwLastID = 0;
}


CsC_LoadSequence::sTEXTKEY_EVENT* CsC_AniProp_Sequence::NextEvent()
{
	if( m_nActiveEventIndex < m_pSrcSequence->s_vpTextKeyEvent.Size() )
	{
		CsC_LoadSequence::sTEXTKEY_EVENT* pEvent = m_pSrcSequence->s_vpTextKeyEvent[ m_nActiveEventIndex ];	
		++m_nActiveEventIndex;

		return pEvent;
	}

	return NULL;
}

ANI::eTYPE CsC_AniProp_Sequence::CalNormalAttackAni( CsC_AttackProp::eHIT_TYPE ht )
{
	assert_csm1( IsSequence( ANI::ATTACK_ATTACK2 ) == true, _T( "ID = %d, 공격 2가 없습니다" ), m_pParent->GetFTID() );

	switch( ht )
	{
	case CsC_AttackProp::HT_Miss:
		if( IsSequence( ANI::ATTACK_MISS ) == true )
			return ANI::ATTACK_MISS;
		break;
	}

	return rand()%2 == 0 ? ANI::ATTACK_ATTACK1 : ANI::ATTACK_ATTACK2;	
}


bool CsC_AniProp_Sequence::IsHaveRangeEvent(DWORD const& dwAniId)
{
	CsC_LoadSequence::sSEQUENCE_INFO * pInfo = GetSequence( dwAniId );
	if( NULL == pInfo )
		return false;

	for( int n = 0; n < pInfo->s_vpTextKeyEvent.Size(); ++n  )
	{ 
		CsC_LoadSequence::sTEXTKEY_EVENT* pEv = pInfo->s_vpTextKeyEvent[n];
		if( pEv->s_nStaticIndex == nsSTATIC_EVENT::DYE_RANGE_ATTACK_START )
			return true;
	}

	return false;
}

// 크리티컬 이펙트와 싸운드 재생에 쓰인다
bool CsC_AniProp_Sequence::IsLastNAtt_Effect()
{ 
	SAFE_POINTER_RETVAL( m_pNAtt_EffectIndexArray, false );
	assert_cs( m_pNAtt_EffectIndexArray ); 
	return m_nNAtt_Count != 0 ? ( ( m_pNAtt_EffectIndexArray->GetData( m_nNAtt_Count - 1 ) < m_nActiveEventIndex ) ) : false; 
}

bool CsC_AniProp_Sequence::IsLastNAtt_Sound()
{ 
	assert_cs( m_pNAtt_EffectIndexArray ); 
	return ( m_nLast_NAtt_SoundIndex < m_nActiveEventIndex ); 
}

// 공격 이벤트 전부 종료 되었는가 - 데미지 애니메이션 띄우는데 쓰인다
bool CsC_AniProp_Sequence::IsEmptyNAttEvent()
{
	SAFE_POINTER_RETVAL( m_pNAtt_EffectIndexArray, false );
	assert_cs( ( m_dwSrcID == ANI::ATTACK_ATTACK1 )||( m_dwSrcID == ANI::ATTACK_ATTACK2 ) );
	return ( m_pNAtt_EffectIndexArray->GetData( m_nNAtt_Count - 1 ) < m_nActiveEventIndex );
}