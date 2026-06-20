
#include "stdafx.h"
#include "05_EffectObject.h"

THREAD_MEMPOOL_CPP( CsC_EffectObject )

#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CsC_EffectObject, RTTI_EFFECTOBJECT )

CsC_EffectObject::CsC_EffectObject():m_pParent(NULL)
{
	m_vOffset = NiPoint3::ZERO;
	m_dwPlag = 0;
	m_fStartTime = 0.0f;
	m_fEndAniTime = 0.0f;
	m_fLifeTime = 0.0f;
	m_eState = LIVE;
#ifdef LEVELUP_EFFECT_LOOP
	m_eLevelEffect = eNormal;
#endif

	m_pHitTarget = NULL;
	m_pChainStartTarget = NULL;
	m_pChainEndTarget = NULL;
	m_pDefaultTest = NULL;

	m_vTargetMissOffset = NiPoint3::ZERO;
	m_vStartPos = NiPoint3::ZERO;
	m_vSpline0 = NiPoint3::ZERO;
	m_vSpline1 = NiPoint3::ZERO;
	m_fMaxScale = 0.0f;
	m_fTotalDist = 0.0f;
	m_fRangeAccumeTime = 0.0f;	
	m_fClashAccumeTime = 0.0f;
	m_fClashConstantTime = 0.0f;
	m_effectDelayTime = 0.0f;
}

CsC_EffectObject::~CsC_EffectObject()
{

}

void CsC_EffectObject::Delete()
{
	PARENT_CLASS::Delete();

	while( !m_queueClashEffect.empty() )
		m_queueClashEffect.pop();

	while( !m_queueClashSound.empty() )
		m_queueClashSound.pop();

	m_vOffset = NiPoint3::ZERO;
	m_dwPlag = 0;
	m_fStartTime = 0.0f;
	m_fEndAniTime = 0.0f;
	m_fLifeTime = 0.0f;
	m_eState = LIVE;
#ifdef LEVELUP_EFFECT_LOOP
	m_eLevelEffect = eNormal;
#endif

	m_pHitTarget = NULL;
	m_pChainStartTarget = NULL;
	m_pChainEndTarget = NULL;
	m_pDefaultTest = NULL;

	m_vTargetMissOffset = NiPoint3::ZERO;
	m_vStartPos = NiPoint3::ZERO;
	m_vSpline0 = NiPoint3::ZERO;
	m_vSpline1 = NiPoint3::ZERO;
	m_fMaxScale = 0.0f;
	m_fTotalDist = 0.0f;
	m_fRangeAccumeTime = 0.0f;	
	m_fClashAccumeTime = 0.0f;
	m_fClashConstantTime = 0.0f;
	m_effectDelayTime = 0.0f;
}

void CsC_EffectObject::DeleteUpdate()
{
	CheckEffectModel( 0.0f );
}

void CsC_EffectObject::Init( CsC_AvObject* pParent, sCREATEINFO* pProp, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption )
{
	memcpy( &m_EventOption, pOption, sizeof( CsC_AniProp_Sequence::sEVENT_OPTION ) );

	m_pHitTarget = NULL;
	
	m_fLifeTime = 0.0f;
	m_pParent = pParent;
	m_dwPlag = pEvent->s_dwPlag;
	m_vOffset = pEvent->s_vOffset;
	m_vSpline0 = pEvent->s_vValue1;
	m_vSpline1 = pEvent->s_vValue2;

	NiPoint3 pos;
	float fRot = 0.0f;

	// 시작위치 값
	switch( m_dwPlag & nsEFFECT::SPOS_MASK )
	{
	case nsEFFECT::SPOS_CHARWORLD:
		pos = m_pParent->GetPos();
		break;
	case nsEFFECT::SPOS_BOUND_CENTER:
		if( m_pParent->IsLoad() == true )
		{
			pos = m_pParent->GetCenter();
		}
		else
		{
			pos = m_pParent->GetPos();
			pos.z += m_pParent->GetToolHeight()*0.5f;
		}
		break;
	case nsEFFECT::SPOS_HALF_THEIGHT:
		pos = m_pParent->GetPos();
		pos.z += m_pParent->GetToolHeight()*0.5f;
		break;
	case nsEFFECT::SPOS_THEIGHT:
		pos = m_pParent->GetPos();
		pos.z += m_pParent->GetToolHeight();
		break;
	}

	// 위치값
	if( m_EventOption.s_bRangeAttack == true )
	{
		m_dwPlag &= ~ nsEFFECT::POS_MASK;
		m_dwPlag |= nsEFFECT::POS_SHOT;
		m_fRangeAccumeTime = 0.0f;
		m_fClashAccumeTime = 0.0f;
		
		assert_cs( m_queueClashEffect.empty() == true );
		assert_cs( m_queueClashSound.empty() == true );
		m_fClashConstantTime = 0.0f;		
	}

	// 회전값
	switch( m_dwPlag & nsEFFECT::DIR_MASK )
	{
	case nsEFFECT::DIR_CHAR:
		fRot = m_pParent->GetCurRot();
		break;
	}

	// 오프셋
	switch( m_dwPlag & nsEFFECT::OFFSET_MASK )
	{
	case nsEFFECT::OFFSET_USE:
		{
			NiPoint3 vOffset = m_vOffset;
			switch( m_dwPlag & nsEFFECT::DIR_MASK )
			{
			case nsEFFECT::DIR_CHAR:
				{
					NiMatrix3 mat;
					mat.MakeZRotation( m_pParent->GetCurRot() );
					vOffset = mat*vOffset;
				}
				break;
			}
			pos += vOffset;
		}		
		break;
	}

	m_eState = LIVE;
	m_DieTimeSeq.SetEndTime( pEvent->s_fFadeoutTime );
#ifdef LEVELUP_EFFECT_LOOP
	m_eLevelEffect = eNormal;
#endif
	PARENT_CLASS::Init( 0, CsC_Object::INAVLIDE_FT_ID, pos, fRot, pProp );
}

void CsC_EffectObject::Init( CsC_AvObject* pParent, sCREATEINFO* pProp, DWORD dwPlag )
{
	m_pParent = pParent;
	m_dwPlag = dwPlag;
	m_vOffset = NiPoint3::ZERO;
	m_fLifeTime = 0.0f;

	NiPoint3 pos;
	float fRot = 0.0f;

	// 시작위치 값
	switch( m_dwPlag & nsEFFECT::SPOS_MASK )
	{
	case nsEFFECT::SPOS_CHARWORLD:
		pos = m_pParent->GetPos();
		break;
	case nsEFFECT::SPOS_BOUND_CENTER:
		if( m_pParent->IsLoad() == true )
		{
			pos = m_pParent->GetCenter();
		}
		else
		{
			pos = m_pParent->GetPos();
			pos.z += m_pParent->GetToolHeight()*0.5f;
		}
		break;
	case nsEFFECT::SPOS_HALF_THEIGHT:
		{
			pos = m_pParent->GetPos();
			pos.z += m_pParent->GetToolHeight()*0.5f;
		}
		break;
	case nsEFFECT::SPOS_THEIGHT:
		pos = m_pParent->GetPos();
		pos.z += m_pParent->GetToolHeight();
		break;
	}

	// 회전값
	switch( m_dwPlag & nsEFFECT::DIR_MASK )
	{
	case nsEFFECT::DIR_CHAR:
		fRot = m_pParent->GetCurRot();
		break;
	}

	// 오프셋
	switch( m_dwPlag & nsEFFECT::OFFSET_MASK )
	{
	case nsEFFECT::OFFSET_USE:
		pos += m_vOffset;
		break;
	}
  
	m_eState = LIVE;
#ifdef LEVELUP_EFFECT_LOOP
	m_eLevelEffect = eNormal;
	//CsC_AniProp_Sequence::sEVENT_OPTION  TempEventOption;
	//m_EventOption.s_fAniAccelRate = 0.5f;
#endif
	PARENT_CLASS::Init( 0, CsC_Object::INAVLIDE_FT_ID, pos, fRot, pProp );
}

void CsC_EffectObject::Init( CsC_AvObject* pParent, NiPoint3 vPos, sCREATEINFO* pProp )
{
	m_pParent = pParent;
	m_dwPlag = 0;
	m_vOffset = NiPoint3::ZERO;
	m_fLifeTime = 0.0f;

	NiPoint3 pos = vPos;
	float fRot = 0.0f;

	m_eState = LIVE;
#ifdef LEVELUP_EFFECT_LOOP
	m_eLevelEffect = eNormal;
#endif
	PARENT_CLASS::Init( 0, CsC_Object::INAVLIDE_FT_ID, pos, fRot, pProp );
}


void CsC_EffectObject::InitForChainEffect( CsC_AvObject* pParent, sCREATEINFO* pProp, const NiPoint3& StartPos )
{
	m_pHitTarget = NULL;
	m_pChainStartTarget = NULL;
	m_pChainEndTarget = NULL;
	m_effectDelayTime = 0.0f;
	m_fLifeTime = 1.50f;
	m_pParent = pParent;
	m_vOffset = NiPoint3::ZERO;
	m_vSpline0 = NiPoint3::ZERO;
	m_vSpline1 = NiPoint3::ZERO;
	NiPoint3 pos;
	float fRot = 0.0f;
	m_fMaxScale = 0.0f;

	// 시작위치값
	pos = StartPos;

	// 위치값
	m_dwPlag &= ~ nsEFFECT::POS_MASK;
	m_dwPlag &= ~ nsEFFECT::SPOS_MASK;
	m_dwPlag = nsEFFECT::POS_SHOT | nsEFFECT::SPOS_CHAIN;
	m_fRangeAccumeTime = 0.0f;
	m_fClashAccumeTime = 0.0f;

	assert_cs( m_queueClashEffect.empty() == true );
	assert_cs( m_queueClashSound.empty() == true );
	m_fClashConstantTime = 0.0f;		
	m_eState = LIVE;
	m_DieTimeSeq.SetEndTime( 1.0f );

	PARENT_CLASS::Init( 0, CsC_Object::INAVLIDE_FT_ID, pos, fRot, pProp );
}

void CsC_EffectObject::Update( float fDeltaTime, bool bAnimation )
{
	switch( m_eState )
	{
	case LIVE:
		{
			// 이동과 회전에 관한 처리
			if( _PreUpdate_Effect( fDeltaTime ) == false )
				return;

			// CheckModel 대신에	
			if( CheckEffectModel( fDeltaTime ) == false )
				return;

			bool bBackup = m_bViewSight;
			m_bViewSight = true;
			if( m_pProperty[ CsC_Property::ANIMATION ] )
			{
				m_pProperty[ CsC_Property::ANIMATION ]->Update( fDeltaTime );
			}
			m_bViewSight = bBackup;

			if( !_CheckLifeTime(fDeltaTime) )
				return;

			// 로드가 완료 되고나서 부터 흐른 시간
			float fAniTime = GetAniTime();

			if( _CheckEffectLive( fAniTime ) == false )
			{				
				assert_cs( m_eState == DIE );
				return;
			}			

			m_bViewSight = CAMERA_ST.UserCulling( m_Node.m_pNiNode );

			_UpdateRotation( fDeltaTime );
			// 이펙트는 보이던 안보이던 업데이트 ㅡ.,ㅡㅋ
			m_Node.m_pNiNode->Update( fAniTime );
			UpdateVisible( fAniTime );
		}		
		break;
	case DIE_READY:
		{
			// CheckModel 대신에	
			if( CheckEffectModel( fDeltaTime ) == false )
				return;
			
			m_bViewSight = true;
			if( m_pProperty[ CsC_Property::ANIMATION ] )
			{
				m_pProperty[ CsC_Property::ANIMATION ]->Update( fDeltaTime );
			}
			m_bViewSight = CAMERA_ST.UserCulling( m_Node.m_pNiNode );

			if( m_DieTimeSeq.IsCoolTime() )
				nsCSGBFUNC::SetMaterial_Alpha( m_Node.m_pNiNode, (float)m_DieTimeSeq.GetRemainTime()/0.4f );
			else
				nsCSGBFUNC::SetMaterial_Alpha( m_Node.m_pNiNode, 0 );

			//_UpdateRotation( fDeltaTime );

			float fAniTime = GetAniTime();
			m_Node.m_pNiNode->Update( fAniTime );
			UpdateVisible( fAniTime );

			_UpdateClash( fDeltaTime );
			
		}
		break;	
	}	
}

void CsC_EffectObject::Render()
{
	if( m_bViewSight == false )
		return;

	m_Node.Render();
}


bool CsC_EffectObject::CheckEffectModel( float fDeltaTime )
{
	_SYNCHRONIZE_THREAD_( &m_dwLoadPlag.m_CS );

	if( GetRefLinkObject() == 0 )
	{
		if( m_dwLoadPlag.m_nDWORD == LOAD_COMPLEATE )
			return true;

		if( ( m_dwLoadPlag.m_nDWORD & LOAD_FIRST_UPDATE ) == 0 )
			return false;

		if( _IsInitialLoad() )
		{
			m_dwLoadPlag.m_nDWORD |= LOAD_INITITAL_LOAD;
			GetProp_Animation()->CheckModelComp();
			return true;
		}
		return false;
	}

	// 모델 파일이 쓰레드로 부터 로드 되었다면
	if( m_dwLoadPlag.m_nDWORD & LOAD_MODEL_FILE )
	{		
		_LoadedFile( GetRTTI() );

		assert_cs( m_Node.m_pNiNode != NULL );

		// 지워지는 중이 아니라면
		if( ( m_dwLoadPlag.m_nDWORD & LOAD_DELETE ) == 0 )
		{			
			switch( m_dwPlag & nsEFFECT::LIVE_MASK )
			{
			case nsEFFECT::LIVE_LOOP:
				nsCSGBFUNC::InitAnimation( m_Node.m_pNiNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
				break;
			case nsEFFECT::LIVE_ONCE:
				nsCSGBFUNC::EndAnimationTime( m_fEndAniTime, m_Node.m_pNiNode );
				assert_cs( m_fEndAniTime != 0.0f );
				break;
			}
		}

		m_dwLoadPlag.m_nDWORD |= LOAD_FIRST_UPDATE;
		if( _IsInitialLoad() )
			m_dwLoadPlag.m_nDWORD |= LOAD_INITITAL_LOAD;

		DecreaseRefLinkObject();
		PostLinkObject();

		if( m_dwLoadPlag.m_nDWORD == LOAD_COMPLEATE )
		{
			GetProp_Animation()->CheckModelComp();
			return true;
		}
		return false;
	}
	return false;
}

bool CsC_EffectObject::_CheckEffectLive( float fAniTime )
{
	// 회전값
	switch( m_dwPlag & nsEFFECT::LIVE_MASK )
	{
	case nsEFFECT::LIVE_ONCE:
		if( fAniTime > m_fEndAniTime )
		{
			m_eState = DIE;
			return false;
		}
		break;
	}
	return ( m_eState != DIE );
}
bool CsC_EffectObject::IsEndEffect()
{
	return ( m_eState == DIE );
}


float CsC_EffectObject::GetEndTime() const
{
	return m_fEndAniTime;
}

DWORD CsC_EffectObject::GetEffectPlag()
{
	return m_dwPlag;
}
void CsC_EffectObject::SetEffectPlag( DWORD dwPlag)
{ 
	m_dwPlag = dwPlag;
}

void CsC_EffectObject::SetLifeTime( float fLifeTime )
{
	m_fLifeTime = fLifeTime;
}

bool CsC_EffectObject::_CheckLifeTime( float fDeltaTime )
{
	if( m_fLifeTime <= 0.0f )
		return true;

	m_fLifeTime -= fDeltaTime;
	if( m_fLifeTime <= 0.0f )
	{
		m_eState = DIE;
		m_fLifeTime = 0.0f;
		return false;
	}

	return true;
}

bool CsC_EffectObject::_PreUpdate_Effect( float fDeltaTime )
{
	// 이동과 관한
	switch( m_dwPlag & nsEFFECT::POS_MASK )
	{
	//라이딩일 때, 자리비움일 때 오라 위치 수정 chu8820
	case nsEFFECT::POS_AURA:
		{
			NiPoint3 pos = m_pParent->GetPos();

			pos = m_pParent->GetPos();
			pos.z += m_pParent->GetToolHeight()*0.5f;

			switch( m_pParent->GetProp_Animation()->GetAnimationID() )
			{
			case ANI::IDLE_TAMER_RIDE_1:		//여기서부터
			case ANI::MOVE_TAMER_RIDE_1:
			case ANI::IDLE_TAMER_RIDE_2:
			case ANI::MOVE_TAMER_RIDE_2:
			case ANI::IDLE_TAMER_RIDE_3:
			case ANI::MOVE_TAMER_RIDE_3:
			case ANI::IDLE_TAMER_RIDE_4:
			case ANI::MOVE_TAMER_RIDE_4:
			case ANI::IDLE_TAMER_RIDE_5:
			case ANI::MOVE_TAMER_RIDE_5:
			case ANI::IDLE_TAMER_RIDE_6:
			case ANI::MOVE_TAMER_RIDE_6:
			case ANI::IDLE_TAMER_RIDE_7:
			case ANI::MOVE_TAMER_RIDE_7:
			case ANI::IDLE_TAMER_RIDE_8:
			case ANI::MOVE_TAMER_RIDE_8:
			case ANI::IDLE_TAMER_RIDE_9:
			case ANI::MOVE_TAMER_RIDE_9:	//여기까지 라이딩 상태
				pos = m_pParent->GetPos();
				pos.z += m_pParent->GetToolHeight()*0.1f;
				break;
			case ANI::IDLE_SIT_DOWN:			//자리비움 앉는 애니메이션 상태
			case ANI::IDLE_SIT:						//자리비움 앉아 있는 상태
				pos = m_pParent->GetPos();
				pos.z += m_pParent->GetToolHeight()*0.15f;
				break;
			}

			if( ( m_dwPlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
			{
				// 회전에 관한
				NiMatrix3 mat;
				switch( m_dwPlag & nsEFFECT::DIR_MASK )
				{
				case nsEFFECT::DIR_CAMERA_AXIS_Z:
					mat.MakeZRotation( CAMERA_ST.GetCurRoll() );
					break;
				case nsEFFECT::DIR_CHAR:
					mat.MakeZRotation( m_pParent->GetCurRot() );										
					break;
				default:
					mat = NiMatrix3::IDENTITY;
				}	

				NiPoint3 vOffset = mat*m_vOffset;
				pos += vOffset;
				SetPos( pos );
				return true;
			}

			SetPos( pos );
			// 아래서 회전값 적용
		}
		break;
	case nsEFFECT::POS_CHARPOS:
		{
			NiPoint3 pos = m_pParent->GetPos();

			switch( m_dwPlag & nsEFFECT::SPOS_MASK )
			{
			case nsEFFECT::SPOS_CHARWORLD:
				pos = m_pParent->GetPos();
				break;
			case nsEFFECT::SPOS_BOUND_CENTER:
				if( m_pParent->IsLoad() == true )
				{
					pos = m_pParent->GetCenter();
				}
				else
				{
					pos = m_pParent->GetPos();
					pos.z += m_pParent->GetToolHeight()*0.5f;
				}
				break;
			case nsEFFECT::SPOS_HALF_THEIGHT:
				{
					pos = m_pParent->GetPos();
					pos.z += m_pParent->GetToolHeight()*0.5f;
				}
				break;
			case nsEFFECT::SPOS_THEIGHT:
				pos = m_pParent->GetPos();
				pos.z += m_pParent->GetToolHeight();
				break;
			}

			if( ( m_dwPlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
			{
				// 회전에 관한
				NiMatrix3 mat;
				switch( m_dwPlag & nsEFFECT::DIR_MASK )
				{
				case nsEFFECT::DIR_CAMERA_AXIS_Z:
					mat.MakeZRotation( CAMERA_ST.GetCurRoll() );
					break;
				case nsEFFECT::DIR_CHAR:
					mat.MakeZRotation( m_pParent->GetCurRot() );										
					break;
				default:
					mat = NiMatrix3::IDENTITY;
				}	

				NiPoint3 vOffset = mat*m_vOffset;
				pos += vOffset;
				SetPos( pos );
				return true;
			}

			SetPos( pos );
			// 아래서 회전값 적용
		}
		break;
	case nsEFFECT::POS_SHOT:

		if( m_pHitTarget )
		{
			if( ( m_pHitTarget->GetFTID() == 0 ) || ( m_pHitTarget->IsLoad() == false ) )
				m_pHitTarget = NULL;
		}
		m_fRangeAccumeTime += fDeltaTime;
		switch( m_dwPlag & nsEFFECT::SPOS_MASK )
		{
		case nsEFFECT::SPOS_CHAIN:
			if( m_pChainStartTarget != NULL && m_pChainEndTarget != NULL )
			{
				UpdateChainEffect(fDeltaTime);
			}
			else
			{
				m_eState = DIE;
				return false;
			}
			return true;
		default:
			{
				m_fRangeAccumeTime += fDeltaTime;
				if( m_pHitTarget != NULL )
				{
					NiPoint3 vEndPos = m_pHitTarget->GetPos() + m_vTargetMissOffset;

					float fCurDIst = _CalCurDist();
					NiPoint3 pos = nsCSGBFUNC::CalMullRom( &m_vSpline0, &m_vStartPos, &vEndPos, &m_vSpline1, CsMin( 1.0f, fCurDIst / m_fTotalDist ) );

					SetRotation_AllAxis( pos );
					SetPos( pos );

					if( fCurDIst > m_fTotalDist )
					{
						_UpdateClash( fDeltaTime );
						return false;
					}
				}
				else
				{
					m_eState = DIE;
					return false;
				}
				return true;		
			}
			break;
		}
	}

	// 회전에 관한
	switch( m_dwPlag & nsEFFECT::DIR_MASK )
	{
	case nsEFFECT::DIR_CAMERA_AXIS_Z:
		SetRotation( CAMERA_ST.GetWorldTranslate(), true );
		break;
	case nsEFFECT::DIR_CHAR:
		SetRotation( m_pParent->GetCurRot(), true );
		break;
	case nsEFFECT::DIR_CHAR_ALL:
		SetRotation( m_pParent->GetCurRot(), true );
		SetRotX( m_pParent->GetCurRotX() );		
		break;
	}

	return true;
}

void CsC_EffectObject::_UpdateClash( float fDeltaTime )
{
	assert_cs( m_pHitTarget != NULL );
	m_fClashAccumeTime += fDeltaTime;	
	
	if( m_eState == LIVE )
	{
		m_eState = ( m_queueClashEffect.empty() == true )&&( m_queueClashSound.empty() == true ) ? DIE : DIE_READY;		
		m_DieTimeSeq.Start();
	}
	else if( m_DieTimeSeq.IsCoolTime() == false )
	{
		m_eState = ( m_queueClashEffect.empty() == true )&&( m_queueClashSound.empty() == true ) ? DIE : DIE_READY;
	}	

	// 이펙트
	while( m_queueClashEffect.empty() == false )
	{
		CsC_LoadSequence::sTEXTKEY_EVENT* pEvent = m_queueClashEffect.front();
		if( pEvent->s_fEventTime - m_fClashConstantTime > m_fClashAccumeTime )
		{
			break;
		}
		m_queueClashEffect.pop();
		m_EventOption.s_bRangeAttack = false;

		assert_cs( pEvent->s_eType == CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC );
		switch( pEvent->s_nStaticIndex )
		{
		case nsSTATIC_EVENT::DYE_NORMAL_ATTACK:
		case nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE:
		case nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE:
			m_pParent->Event_NormalAttack_Clash( pEvent, &m_EventOption, m_pHitTarget );
			break;
		}
		m_EventOption.s_bRangeAttack = true;
	}

	// 사운드
	while( m_queueClashSound.empty() == false )
	{
		CsC_LoadSequence::sTEXTKEY_EVENT* pEvent = m_queueClashSound.front();
		if( pEvent->s_fEventTime - m_fClashConstantTime > m_fClashAccumeTime )
		{
			break;
		}
		m_queueClashSound.pop();
		m_pParent->Event_Sound( pEvent, &m_EventOption, m_pHitTarget );
	}	
}
float CsC_EffectObject::_CalVelocity( float fDeltaTime )
{
	return ( m_EventOption.s_fVelocity + m_fRangeAccumeTime*m_EventOption.s_fAccelVelocity )*fDeltaTime;
}

void CsC_EffectObject::SetOffset( NiPoint3 vOffset )
{
	assert_cs( ( m_dwPlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE );
	NiPoint3 pos = GetPos() - m_vOffset;
	SetPos( pos + vOffset );
	m_vOffset = vOffset;
}

void CsC_EffectObject::SetWorldPos( NiPoint3 vPos )
{
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );
	SetPos( vPos );
}

void CsC_EffectObject::SetHitTarget( CsC_AvObject* pHitTarget )
{
	m_pHitTarget = pHitTarget;
	if( m_pHitTarget != NULL )
	{		
		m_vTargetMissOffset = NiPoint3::ZERO;
		if( m_EventOption.s_eHitType == CsC_AttackProp::HT_Miss )
		{
			m_vTargetMissOffset = NiPoint3( (float)( rand()%100 - 50 ), (float)( rand()%110 - 55 ), ( float )( rand()%20 - 10 ) );
			m_vTargetMissOffset.Unitize();
			m_vTargetMissOffset *= m_pHitTarget->GetToolWidth();

			NiPoint3 vBackOffset = m_pHitTarget->GetPos() - GetPos();
			vBackOffset.z = 0.0f;
			vBackOffset.Unitize();
			m_vTargetMissOffset += vBackOffset*200.0f;
		}
		m_vTargetMissOffset.z += m_pHitTarget->GetCenter().z - m_pHitTarget->GetPos().z;

		NiPoint3 vEndPos = m_pHitTarget->GetPos() + m_vTargetMissOffset;
		SetRotation_AllAxis( vEndPos );

		m_fTotalDist = ( vEndPos - GetPos() ).Length();		
		m_vStartPos = GetPos();

		CsNodeObj* pCsNode = m_pParent->GetCsNode();
		if( pCsNode )
		{
			m_vSpline0 = pCsNode->m_pNiNode->GetWorldTransform()*m_vSpline0;
		}
		else
		{
			m_vSpline0 = GetPos();
		}

		pCsNode = m_pHitTarget->GetCsNode();
		if( pCsNode )
		{
			m_vSpline1 = pCsNode->m_pNiNode->GetWorldTransform()*m_vSpline1;
		}
		else
		{
			m_vSpline1 = m_pHitTarget->GetCenter();
		}
	}
}

void CsC_EffectObject::SetChainTarget(CsC_AvObject* pStartTarget , CsC_AvObject* pEndTarget )
{
	m_pHitTarget = NULL;
	m_pChainStartTarget = pStartTarget;
	m_pChainEndTarget = pEndTarget;
	if(m_pChainStartTarget != NULL && m_pChainEndTarget != NULL)
	{
		//m_vStartPos = pStartTarget->GetPos();
		//NiPoint3 vEndPos = pEndTarget->GetPos();
		//m_fTotalDist = ( vEndPos - GetPos() ).Length();		
		//SetRotation_AniDir(m_vStartPos - vEndPos);
	}
}

void CsC_EffectObject::SetClashConstantTime( float fTime )
{ 
	m_fClashConstantTime = fTime;
}
void CsC_EffectObject::SetDelayTime(const float fTime)
{
	m_effectDelayTime = fTime;
}
void CsC_EffectObject::AddClashEvent( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent )
{ 
	m_queueClashEffect.push( pEvent );
}
void CsC_EffectObject::AddClashSound( CsC_LoadSequence::sTEXTKEY_EVENT* pClashSound )
{
	m_queueClashSound.push( pClashSound );
}

void	CsC_EffectObject::UpdateChainEffect(float fDeltaTime)
{
	CsNodeObj* pCsNode = GetCsNode();
	if( NULL == pCsNode || NULL == pCsNode->m_pNiNode )
		return;

	if( NULL == m_pChainStartTarget || NULL == m_pChainEndTarget )
		return;

	CsNodeObj* STargetNode = m_pChainStartTarget->GetCsNode();
	CsNodeObj* ETargetNode = m_pChainEndTarget->GetCsNode();
	if( NULL == STargetNode || NULL == ETargetNode )
		return;

	if( NULL == STargetNode->m_pNiNode || NULL == ETargetNode->m_pNiNode )
		return;

	const float DefaultEffectLenght = 250.0f;
	const float DefaultEffectHeight = 50.0f;
	const float DefaultEffectSize = 0.3f;
	const float DefaultEffectScale = 3.5f;
	const float DefaulEffectSpeed= 2.5f;

	m_vStartPos = STargetNode->m_pNiNode->GetWorldBound().GetCenter();
	NiPoint3 vEndPos = ETargetNode->m_pNiNode->GetWorldBound().GetCenter();
	NiPoint3 vDir = vEndPos - m_vStartPos;
	NiPoint3::UnitizeVector(vDir);
	UpdateBaseTransform(fDeltaTime, vDir);
	if(m_fMaxScale == 0.0f)
	{
		NiTransform kTr = pCsNode->m_pNiNode->GetWorldTransform();
		kTr.m_Rotate.SetCol(1, NiPoint3::ZERO);

		NiPoint3 DestTranslate = vEndPos;
		DestTranslate.z += DefaultEffectHeight;
		kTr.m_Translate = DestTranslate;

		pCsNode->m_pNiNode->SetLocalTransform(kTr);
		pCsNode->m_pNiNode->Update(0.0f);
		
		if(DefaultEffectScale * DefaultEffectLenght != 0)
			m_fMaxScale = m_fTotalDist / (DefaultEffectScale * DefaultEffectLenght);
		else
			m_fMaxScale = 0.0f;
	}
	else
	{
		m_fTotalDist = ( vEndPos - m_vStartPos ).Length();		

		float scale = 0.0f;
		if( (DefaultEffectScale * DefaultEffectLenght) != 0.0f)
			scale = m_fTotalDist / (DefaultEffectScale * DefaultEffectLenght);

		//float currTime = NiMin(0.3f + m_fRangeAccumeTime * m_fRangeAccumeTime, 1.0f);
		//float fSin = (sin(m_fRangeAccumeTime) + 1.0f) * 0.25f + 0.5f;
		NiTransform kTr = pCsNode->m_pNiNode->GetWorldTransform();
		NiTransform kBaseTr = m_BaseTransform.GetWorldTransform();

		kTr.m_Translate =m_vStartPos;
		NiPoint3 BaseRot = NiPoint3::ZERO;
		kBaseTr.m_Rotate.GetCol(1, BaseRot);
		NiPoint3 DestRot = BaseRot * scale;

		NiPoint3 DestTranslate = m_vStartPos;
		DestTranslate.z += DefaultEffectHeight;
		kTr.m_Translate = DestTranslate;
		kTr.m_Rotate = kBaseTr.m_Rotate;
		kTr.m_Rotate.SetCol(1, DestRot);
		
		kTr.m_fScale = DefaultEffectScale;
		pCsNode->m_pNiNode->SetLocalTransform(kTr);
		pCsNode->m_pNiNode->Update(m_fRangeAccumeTime);

		//BHPRT("Dest : [%f] SCALE : [%f] CurrTime : [%f] DestRotCol1 X1[%f] X2[%f] X3[%f]", 
		//	m_fTotalDist, scale, currTime, DestRot.x, DestRot.y, DestRot.z);

	}
}

void	CsC_EffectObject::UpdateBaseTransform(const float& fDeltaTime, const NiPoint3& vDir )
{
	NiPoint3 vDestDir = NiPoint3( 0.0f, 1.0f, 0.0f );
	float fDeltaRot = NiACos( vDir.Dot( vDestDir ) );
	if( vDir.Cross( vDestDir ).z > 0 )
		fDeltaRot = -fDeltaRot;

	NiMatrix3 mat;
	mat.MakeZRotation( fDeltaRot * -1.0f);
	//BHPRT("CurRot : [%f]", m_fCurRot);
	m_BaseTransform.SetRotate( mat );
	m_BaseTransform.Update(0.0f);
}

#undef		PARENT_CLASS
