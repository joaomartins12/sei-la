
#include "stdafx.h"
#include "02_AvObject.h"

#define		PARENT_CLASS		CsC_Object
CsCRTTI_CPP( PARENT_CLASS, CsC_AvObject, RTTI_AVOBJECT )
THREAD_MEMPOOL_CPP( CsC_AvObject )

//////////////////////////////////////////////////////////////////////////
CsC_AvObject::sBASE_STAT::sBASE_STAT(CsC_AvObject* pParent):s_pParent(pParent),s_nLevel(INVALIDE_STAT),s_nAttackSpeed(0),
s_nHPPercent(INVALIDE_STAT),s_fMoveSpeed(400.0f),s_bFatigue(false)
{
}

CsC_AvObject::sBASE_STAT::~sBASE_STAT()
{

}

CsC_AvObject::eSTAT CsC_AvObject::sBASE_STAT::GetType() const
{ 
	return BASE_STAT; 
}
/**< 레벨 반환 */		
int CsC_AvObject::sBASE_STAT::GetLevel() const
{ 
	return s_nLevel; 
}				
/**< 255분율 이기문에 맥스값 255 */
int CsC_AvObject::sBASE_STAT::GetMaxHP() const
{ 
	return 255; 
}
/**< 255분율 이기문에 맥스값 255 */
int CsC_AvObject::sBASE_STAT::GetMaxDS() const
{ 
	return 255; 
}					
/**< 255분율 반환 */
int CsC_AvObject::sBASE_STAT::GetHP() const
{ 
	return s_nHPPercent; 
}						
/**< 실제값 반환 */
int CsC_AvObject::sBASE_STAT::GetBackupHP() const
{ 
	return s_nHPBackup; 
}
/**< 255분율을 100분율로 변환하여 반환 */
float CsC_AvObject::sBASE_STAT::GetHPRate() const
{ 
	return s_nHPPercent/255.0f; 
}			

int	CsC_AvObject::sBASE_STAT::GetDS() const
{ 
	assert_cs( false ); 
	return 0; 
}			

float CsC_AvObject::sBASE_STAT::GetDSRate() const
{ 
	assert_cs( false ); 
	return 0; 
}
/**< 어텍 스피트 */
void CsC_AvObject::sBASE_STAT::SetAttackSpeed( int nSpeed ) 
{ 
	s_nAttackSpeed = nSpeed; 
}

void CsC_AvObject::sBASE_STAT::SetMaxHP( int nMaxHP )
{ 
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetMaxDS( int nMaxDS )
{ 
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetAtt(int nValue)
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetEvade( int nValue )
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetHitRate(int nValue )
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetDef( int nValue )
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetCritical( int nValue )
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetDeltaMaxHP( int nMaxHP )
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetDeltaMaxDS( int nMaxDS )
{ 
	assert_cs( false ); 
}
/**< 255분율로 HP정보 셋팅 */
void CsC_AvObject::sBASE_STAT::SetHP( int nHP )
{ 
	assert_csm1( ( nHP <= 255 )&&( nHP >= 0 ), _T( "hp = %d" ), nHP ); 
	s_nHPPercent = nHP; 
	SetBackupHP( nHP ); 
}		
/**< 실제값 셋팅 */
void CsC_AvObject::sBASE_STAT::SetBackupHP( int nHP )
{ 
	s_nHPBackup = nHP; 
}		

void CsC_AvObject::sBASE_STAT::SetDS( int nDS )
{ 
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetFP( int nFP )
{
	//assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetDeltaFP( int nDelta )
{
	assert_cs( false ); 
}
/**< HP 변위 셋팅 */
void CsC_AvObject::sBASE_STAT::SetDeltaHP( int nDelta )
{ 
	s_nHPPercent = CsMax( 0, CsMin( 255, nDelta + s_nHPPercent ) ); 
}		
/**< 실제 HP 변위 셋팅 */
void CsC_AvObject::sBASE_STAT::SetDeltaBackupHP( int nDelta )
{ 
	s_nHPBackup = CsMax( 0, CsMin( 255, nDelta + s_nHPBackup ) ); 
}

int	CsC_AvObject::sBASE_STAT::GetFP() const
{ 
	assert_cs( false ); return 0; 
}

int CsC_AvObject::sBASE_STAT::GetFriendShip() const
{ 
	assert_cs( false ); 
	return 0; 
}
float CsC_AvObject::sBASE_STAT::GetFPRate() const
{ 
	assert_cs( false ); 
	return 0; 
}

void CsC_AvObject::sBASE_STAT::SetDeltaDS( int nDelta )
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetDeltaBackupDS( int nDelta )
{
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetFriendShip( int nFS )
{ 
	assert_cs( false ); 
}

/**< 이동 속도 리턴 */
float CsC_AvObject::sBASE_STAT::GetMoveSpeed() const
{ 
	return s_fMoveSpeed; 
}				

float CsC_AvObject::sBASE_STAT::GetAttackSpeed() const
{ 
	return (float)s_nAttackSpeed * 0.001f; 
}

/**< 피로한지 리턴 */
bool CsC_AvObject::sBASE_STAT::IsFatigue() const
{ 
	return s_bFatigue; 
}				

/**< 이동속도 포인터 반환 */
float* CsC_AvObject::sBASE_STAT::GetPointer_MoveSpeed()
{ 
	return &s_fMoveSpeed; 
}
/**< 레벨 셋팅 */	
void CsC_AvObject::sBASE_STAT::SetLevel( int nLevel )
{ 
	s_nLevel = nLevel; 
}			
/**< 이동속도 셋팅 */
void CsC_AvObject::sBASE_STAT::SetMoveSpeed( float fMoveSpeed )
{ 
	s_fMoveSpeed = fMoveSpeed;
}		

void CsC_AvObject::sBASE_STAT::EnableFatigue( bool bFatigue )
{ 
	s_bFatigue = bFatigue; 
}

__int64	CsC_AvObject::sBASE_STAT::GetExp() const
{ 
	assert_cs( false ); 
	return 0; 
}

void CsC_AvObject::sBASE_STAT::SetExp( __int64 dwExp )
{ 
	assert_cs( false ); 
}

void CsC_AvObject::sBASE_STAT::SetBattleEP( float fEP )
{ 
	assert_cs( false );
}

float CsC_AvObject::sBASE_STAT::GetBattleEP() const
{ 
	assert_cs( false ); return 0; 
}

int CsC_AvObject::sBASE_STAT::GetEvade() const
{
	assert_cs( false ); return 0; 
}

int	CsC_AvObject::sBASE_STAT::GetAtt() const
{
	assert_cs( false ); return 0; 
}

int	CsC_AvObject::sBASE_STAT::GetDef() const
{
	assert_cs( false ); return 0; 
}

int	CsC_AvObject::sBASE_STAT::GetCritical() const
{
	assert_cs( false ); return 0; 
}

//////////////////////////////////////////////////////////////////////////

/**
 * \author      :	      song
 * \remarks	: 생성자 - 포인터 및 변수의 초기화  
 */
CsC_AvObject::CsC_AvObject()
{
	for( int i=0; i<PAUSE_COUNT; ++i )
	{
		m_PauseArray[ i ].Reset();
	}

	m_pABVWire = NULL;
	m_pBaseStat = NULL;

	m_fHeightConstant = 0.0f;
	m_fXRot = 0.0f;

	m_fScaleGame = m_fScaleFT = m_fScaleTool = m_fScaleOrgGame = 1.0f;
	m_eNatureType = nsCsDigimonTable::NT_NONE;
	for( int n = 0; n < 3; ++n )
		m_eNatureTypes[n] = nsCsDigimonTable::NT_NONE;
	//memset(&m_eNatureTypes, nsCsDigimonTable::NT_NONE, sizeof(nsCsDigimonTable::eNATURE_TYPE)*3);
	m_dwLoadID = 0;
	m_pThreadResLoad = NULL;

	m_bEnableObject = true;
	m_dwChangeApplyModelID = 0;

	m_bEnableColorEffect = false;
	m_bAniPause = false;
	m_bDoNotObject = false;
}


/**
 * \author      :	      song
 * \remarks	: 제거
 */
void CsC_AvObject::Delete()
{
	PARENT_CLASS::Delete();

	// Property 제거
	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		if( m_pProperty[ i ] != NULL )
		{
			m_pProperty[ i ]->Delete();
			CsC_Property::PushProp( m_pProperty[ i ] );
			m_pProperty[ i ] = NULL;
		}		
	}

	for( int i=0; i<PAUSE_COUNT; ++i )
	{
		m_PauseArray[ i ].Reset();
	}
	m_Node.Delete();
	m_pABVWire = NULL;

	m_dwLoadID = 0;
	m_fHeightConstant = 0.0f;	
	m_fScaleGame = m_fScaleFT = m_fScaleTool = m_fScaleOrgGame = 1.0f;

	m_dwChangeApplyModelID = 0;
	m_bEnableObject = true;
	m_bDoNotObject = false;
	RemoveShaderEffect_Highlight();
}

bool CsC_AvObject::IsDelete()
{
	if( CsC_Object::IsDelete() == false )
	{
		DeleteUpdate();
		return false;
	}

	if( IsProperty( CsC_Property::EFFECT) && ( GetProp_Effect()->IsDelete() == false ) )
		return false;
	
	assert_cs( m_pThreadResLoad == NULL );
	return true;
}

void CsC_AvObject::DeleteUpdate()
{
	CheckModel();
}

void CsC_AvObject::PreDelete()
{
	PARENT_CLASS::PreDelete();

	m_dwLoadPlag.Lock_AddOr( LOAD_DELETE );

	SAFE_NIDELETE( m_pBaseStat );
	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		if( m_pProperty[ i ] != NULL )
		{
			m_pProperty[ i ]->PreDelete();
		}
	}

	m_eNatureType = nsCsDigimonTable::NT_NONE;
	for( int n = 0; n < 3; ++n )
		m_eNatureTypes[n] = nsCsDigimonTable::NT_NONE;
	//memset(&m_eNatureTypes, nsCsDigimonTable::NT_NONE, sizeof(nsCsDigimonTable::eNATURE_TYPE)*3);
}

/**
 * \author      :	      song
 * \remarks	: 초기화 ( 파츠오브젝트면 CsC_PartObject::Init을 통과해서 와야한다
 * \param nIDX				IDX
 * \param nFileTableID		파일테이블ID
 * \param vPos				위치값
 * \param fRot				회전값
 * \param pProp				케릭터정보
 */
void CsC_AvObject::Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
{
	m_nClientPlag = CP_DMG_ALL;

	// 파츠 오브젝트면 파츠 이닛을 통과해서 와야 한다.
	assert_cs( m_fScaleGame == 1.0f );
	assert_cs( m_fScaleTool == 1.0f );
	assert_cs( m_fHeightConstant == 0.0f );
	assert_cs( IsKindOf( RTTI_PARTOBJECT ) == true ? (   ((CsC_PartObject*)this)->GetWorkingFolder()[ 0 ] != NULL ? true : false   ) : true );
//	assert_cs( IsKindOf( RTTI_CARDOBJECT ) == true ? (   ((CsC_CardObject*)this)->GetWorkingFolder()[ 0 ] != NULL ? true : false   ) : true );

	PARENT_CLASS::Init( nIDX, nFileTableID );
	m_dwLoadPlag.Lock_Set( LOAD_INIT );
	m_bViewSight = false;
	m_eDeleteState = DS_FADEOUT;
	m_CharProp = *pProp;

	m_bEnableObject = true;

	// 스탯
	if( m_CharProp.IsOptionPlag( OP_STAT )&&( m_pBaseStat == NULL ) )
	{
		m_pBaseStat = NiNew sBASE_STAT( this );
	}

	// Property 초기화
	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
		m_pProperty[ i ] = NULL;

	// Initial Position
	m_fXRot = 0.0f;
	SetPos( vPos );	
	SetRotation( fRot, true );

	//==========================================================================================
	//	툴에서 설정한 데이터
	//==========================================================================================
	if( g_pModelDataMng->IsData( GetModelID() ) == true )
	{
		CsModelData::sINFO* pModelInfo = g_pModelDataMng->GetData( GetModelID() )->GetInfo();
		m_fScaleTool = pModelInfo->s_fScale;
		m_fToolWidth = pModelInfo->s_fWidth * 100.0f;
		m_fToolHeight = pModelInfo->s_fHeight * 100.0f;
	}
	else
	{
		m_fScaleTool = 1.0f;
		m_fToolHeight = m_fToolWidth = 100.0f;
	}

	// 스케일
	SetScale( 1.0f );

	// 패쓰 엔진
	switch( m_CharProp.GetPropPlag( _PR_PATH_MASK ) )
	{
	case PR_PATH_NONE:
		break;
	case PR_PATH_NORMAL:
	case PR_PATH_USER:
		{
			CsC_PathProp* pProp = (CsC_PathProp*)CsC_Property::NewProp( CsC_Property::PATH );
			if( m_pBaseStat != NULL )
				pProp->Init( this, m_pBaseStat->GetPointer_MoveSpeed(), m_CharProp.IsPropPlag( PR_PATH_USER ) );
			else
				pProp->Init( this, NULL, m_CharProp.IsPropPlag( PR_PATH_USER ) );

			m_pProperty[ CsC_Property::PATH ] = pProp;
		}
		break;
	default:
		assert_cs ( false );
	}

	// 알파 프로퍼티
	CsC_AlphaProp* pAlphaProp = (CsC_AlphaProp*)CsC_Property::NewProp( CsC_Property::ALPHA );
	pAlphaProp->Init( this, CsC_AlphaProp::AT_ALPHA_INCREASE, 0.0f );
	m_pProperty[ CsC_Property::ALPHA ] = pAlphaProp;

	// 애니메이션 프로퍼티
	if( m_CharProp.IsPropPlag( PR_ANI_ENABLE ) )
	{
		CsC_AniProp* pAniProp = (CsC_AniProp*)CsC_Property::NewProp( CsC_Property::ANIMATION );
		pAniProp->Init( this );
		m_pProperty[ CsC_Property::ANIMATION ] = pAniProp;
	}	

	// 어택 프로퍼티
	if( m_CharProp.IsPropPlag( PR_ATTACK_ENABLE ) )
	{
		CsC_AttackProp* pAttackProp = (CsC_AttackProp*)CsC_Property::NewProp( CsC_Property::ATTACK );
		pAttackProp->Init( this );
		m_pProperty[ CsC_Property::ATTACK ] = pAttackProp;
	}

	// 이펙트 프로퍼티
	if( m_CharProp.IsPropPlag( PR_EFFECT_ENABLE ) )
	{
		CsC_EffectProp* pEffectProp = (CsC_EffectProp*)CsC_Property::NewProp( CsC_Property::EFFECT );
		pEffectProp->Init( this );
		m_pProperty[ CsC_Property::EFFECT ] = pEffectProp;
	}

	// 무브 컨트롤러 프로퍼티
	if( m_CharProp.IsPropPlag( PR_MOVECON_ENABLE ) )
	{
		CsC_MoveControlProp* pProp = (CsC_MoveControlProp*)CsC_Property::NewProp( CsC_Property::MOVE_CONTROL );
		pProp->Init( this );
		m_pProperty[ CsC_Property::MOVE_CONTROL ] = pProp;
	}

	m_bServerDie = false;
	m_bAniPause = false;
}

void CsC_AvObject::AttachProp( CsC_Property::eTYPE eType )
{
	if( IsProperty( eType ) == true )
		return;

	switch( eType )
	{		
	case CsC_Property::PATH:
		{
			m_CharProp.s_dwPropPlag |= PR_PATH_NORMAL;			
			CsC_PathProp* pProp = (CsC_PathProp*)CsC_Property::NewProp( CsC_Property::PATH );
			pProp->Init( this, NULL, m_CharProp.IsPropPlag( PR_PATH_NORMAL ) );
			m_pProperty[ CsC_Property::PATH ] = pProp;			
		}		
		break;
	case CsC_Property::ANIMATION:
		{
			// 애니메이션 프로퍼티
			m_CharProp.s_dwPropPlag |= PR_ANI_ENABLE;
			CsC_AniProp* pAniProp = (CsC_AniProp*)CsC_Property::NewProp( CsC_Property::ANIMATION );
			pAniProp->Init( this );
			m_pProperty[ CsC_Property::ANIMATION ] = pAniProp;			
		}
		break;
	case CsC_Property::ALPHA:
		{			
			CsC_AlphaProp* pAlphaProp = (CsC_AlphaProp*)CsC_Property::NewProp( CsC_Property::ALPHA );
			pAlphaProp->Init( this, CsC_AlphaProp::AT_ALPHA_INCREASE, 0.0f );
			m_pProperty[ CsC_Property::ALPHA ] = pAlphaProp;
		}
		break;
	case CsC_Property::ATTACK:
		{
			// 어택 프로퍼티	
			m_CharProp.s_dwPropPlag |= PR_ATTACK_ENABLE;
			CsC_AttackProp* pAttackProp = (CsC_AttackProp*)CsC_Property::NewProp( CsC_Property::ATTACK );
			pAttackProp->Init( this );
			m_pProperty[ CsC_Property::ATTACK ] = pAttackProp;			
		}
		break;
	case CsC_Property::EFFECT:
		{
			// 이펙트 프로퍼티			
			m_CharProp.s_dwPropPlag |= PR_EFFECT_ENABLE;
			CsC_EffectProp* pEffectProp = (CsC_EffectProp*)CsC_Property::NewProp( CsC_Property::EFFECT );
			pEffectProp->Init( this );
			m_pProperty[ CsC_Property::EFFECT ] = pEffectProp;			
		}
		break;
	case CsC_Property::MOVE_CONTROL:
		{
			// 무브 컨트롤러 프로퍼티			
			m_CharProp.s_dwPropPlag |= PR_MOVECON_ENABLE;
			CsC_MoveControlProp* pProp = (CsC_MoveControlProp*)CsC_Property::NewProp( CsC_Property::MOVE_CONTROL );
			pProp->Init( this );
			m_pProperty[ CsC_Property::MOVE_CONTROL ] = pProp;			
		}
		break;
	default:
		assert_csm( false, _T( "선언되어있지 않은 프로퍼티 생성" ) );
		break;
	}		
}

/**
 * \author      :	      song
 * \remarks	: 업데이트
 * \param fDeltaTime 변화시간
 * \param bAnimation 애니메이션여부
 */
void CsC_AvObject::Update( float fDeltaTime, bool bAnimation )
{
	_UpdatePause();
	_Update_EffectPos();

	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		if( m_bAniPause && i == CsC_Property::ANIMATION )
			continue;
		if( m_pProperty[ i ] != NULL )
			m_pProperty[ i ]->PreUpdate( fDeltaTime );
	}
	_UpdateRotation( fDeltaTime );

	if( CheckModel() == CHECK_NOT_LOAD )
		return;

	if( m_bEnableObject && ( GetProp_Alpha()->GetCurAlphaValue() != 0.0f ) )
	{
		m_bViewSight = CAMERA_ST.UserCulling( m_Node.m_pNiNode );
	}
	else
	{
		m_bViewSight = false;
	}

	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		if( m_bAniPause && i == CsC_Property::ANIMATION )
			continue;
		if( m_pProperty[ i ] != NULL )
			m_pProperty[ i ]->Update( fDeltaTime );
	}
	
	if( IsProperty( CsC_Property::ANIMATION ) )	// AniProperty 체크
	{
		float fAniTime = GetAniTime();	
		// 시야에 보이지 않는다면 애니메이션도 하지 않는다.

		m_Node.m_pNiNode->_UpdateSelect( fAniTime, m_bViewSight );
		if( m_bViewSight )
		{
			UpdateVisible( fAniTime );
		}
	}
}

void CsC_AvObject::SetDeleteState( eDELETE_STATE ds )
{
	m_eDeleteState = ds;
	switch( m_eDeleteState )
	{
	case DS_IMMEDEATE:
		break;
	case DS_FADEOUT:
		{
			CsC_AlphaProp* pProp = GetProp_Alpha();
			pProp->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
		}
		break;
	}
}

bool CsC_AvObject::UpdateDelete( float fDeltaTime )
{
	switch( m_eDeleteState )
	{
	case DS_IMMEDEATE:
		return false;
	case DS_FADEOUT:
		if( GetProp_Alpha()->GetCurAlphaValue() == 0 )
			return false;
		break;
	default:
		assert_cs( false );
	}
	return true;
}

/**
 * \author      :	      song
 * \remarks	: 랜더링
 */
void CsC_AvObject::Render()
{
	if( IsEnableObject() == false )
		return;

	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		if( m_pProperty[ i ] != NULL )
			m_pProperty[ i ]->Render();
	}

	if( m_bViewSight == false )
		return;

	if( m_bEnableColorEffect )
		g_pSEffectMgr->Render(this);
	else
		m_Node.Render();
}

/**
 * \author      :	      song
 * \remarks	: 실제 오브젝트 랜더링이 아닌 프로퍼티 랜더링만 실행
 */
void CsC_AvObject::RenderEtc()
{
	if( m_bViewSight == false )
		return;

	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		if( m_pProperty[ i ] != NULL )
			m_pProperty[ i ]->Render();
	}
}

//====================================================================================
//
//		Load
//
//====================================================================================
/**
 * \author      :	      song
 * \remarks 
 * 진화시 KFM 변경 사용 : 기존데이터 삭제, 스케일값  변경, 로드 플래그 초기화, 프로퍼티 ChangeKFM호출 등등
 * \param nFTID 파일테일블ID
 */
void CsC_AvObject::ChangeKFM( UINT nFTID )
{
	// 파츠는 교체 하면 안된다.
	g_CsLastError.m_nIntValue3 = 10001;
	PARENT_CLASS::ChangeKFM( nFTID );
	g_CsLastError.m_nIntValue3 = 10002;

	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		g_CsLastError.m_nIntValue3 = 10003+i;
		if( m_pProperty[ i ] != NULL )
			m_pProperty[ i ]->ChangeKFM();
	}

	g_CsLastError.m_nIntValue3 = 10011;
	m_dwLoadPlag.Lock_Set( LOAD_INIT );
	m_bViewSight = false;

	m_Node.Delete();
	m_pABVWire = NULL;

	//==========================================================================================
	//	툴에서 설정한 데이터
	//==========================================================================================
	if( g_pModelDataMng->IsData( GetModelID() ) == true )
	{
		CsModelData::sINFO* pModelInfo = g_pModelDataMng->GetData( GetModelID() )->GetInfo();
		m_fScaleTool = pModelInfo->s_fScale;
		m_fToolWidth = pModelInfo->s_fWidth * 100.0f;
		m_fToolHeight = pModelInfo->s_fHeight * 100.0f;
	}
	else
	{
		m_fScaleTool = 1.0f;
		m_fToolHeight = m_fToolWidth = 100.0f;
	}

	// 스케일
	SetScale( m_fScaleGame, false );

	g_CsLastError.m_nIntValue3 = 11111;

	/*for( int i=0; i<PAUSE_COUNT; ++i )
	{
		m_PauseArray[ i ].Reset();
	}*/

	g_CsLastError.m_nIntValue3 = 99999;
}

/**
 * \author      :	      song
 * \remarks	: 파트너몬 변경시 호출
 * \param nFTID 파일테이블 ID
 * \param szName 교체되는 몬스터의 이름
 */
void CsC_AvObject::ChangeModel( UINT nFTID, LPCTSTR szName )
{
	// 파츠는 교체 하면 안된다.
	assert_cs( IsKindOf( RTTI_PARTOBJECT ) == false );

	g_CsLastError.m_nIntValue3 = 1;
	PARENT_CLASS::ChangeModel( nFTID, szName );
	g_CsLastError.m_nIntValue3 = 2;

	for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
	{
		if( m_pProperty[ i ] != NULL )
		{
			g_CsLastError.m_nIntValue3 = 3+i;
			m_pProperty[ i ]->ChangeKFM();
		}
	}

	g_CsLastError.m_nIntValue3 = 11;
	m_dwLoadPlag.Lock_Set( LOAD_INIT );
	m_bViewSight = false;

	m_Node.Delete();
	m_pABVWire = NULL;

	//==========================================================================================
	//	툴에서 설정한 데이터
	//==========================================================================================
	if( g_pModelDataMng->IsData( GetModelID() ) == true )
	{
		CsModelData::sINFO* pModelInfo = g_pModelDataMng->GetData( GetModelID() )->GetInfo();
		m_fScaleTool = pModelInfo->s_fScale;
		m_fToolWidth = pModelInfo->s_fWidth * 100.0f;
		m_fToolHeight = pModelInfo->s_fHeight * 100.0f;
	}
	else
	{
		m_fScaleTool = 1.0f;
		m_fToolHeight = m_fToolWidth = 100.0f;
	}

	// 스케일
	SetScale( m_fScaleGame, false );

	g_CsLastError.m_nIntValue3 = 100;
	g_CsLastError.m_nIntValue3 = 999;
}

/**
 * \author      :	      song
 * \remarks	: 쓰레드에서 KFM 로드되는 시점에 호출
 * \param pLoad 로드된 정보를 담고 있는 구조체
 */
void CsC_AvObject::ThreadCallBack_LoadKFM( sLOAD_KFM* pLoad )
{
	PARENT_CLASS::ThreadCallBack_LoadKFM( pLoad );
	assert_cs( m_pThreadResLoad == NULL );
	m_pThreadResLoad = pLoad;
	assert_cs( pLoad->s_dwLoadID != 0 );
}

/**
 * \author      :	      song
 * \remarks	: 쓰레드에서 NIF 로드되는 시점에 호출
 * \param pLoad 로드된 정보를 담고 있는 구조체
 */
void CsC_AvObject::ThreadCallBack_LoadNIF( sLOAD_NIF* pLoad )
{
	PARENT_CLASS::ThreadCallBack_LoadNIF( pLoad );
	assert_cs( m_pThreadResLoad == NULL );
	m_pThreadResLoad = pLoad;

	assert_cs( pLoad->s_dwLoadID != 0 );
}

/**
 * \author      :	      song
 * \remarks	: ThreadCallBack_LoadCompleate 시점에 호출
 */
void CsC_AvObject::_LoadedFile( int nCallRTTI )
{
	PARENT_CLASS::_LoadedFile( nCallRTTI );

	assert_cs( m_pThreadResLoad != NULL );
	switch( m_pThreadResLoad->GetType() )
	{
	case sRES_LOAD::NIF:
		{
			sLOAD_NIF* pNif = (sLOAD_NIF*)m_pThreadResLoad;

			assert_cs( m_Node.m_pNiNode == NULL );
			m_dwLoadID = pNif->s_dwLoadID;
			assert_cs( m_dwLoadID != 0 );
			m_dwVisiblePlag = pNif->s_dwVisiblePlag;
			m_Node.SetNiObject( pNif->s_pNiNode, m_CharProp.s_eInsertGeomType );

			// 충돌 박스
			m_pABVWire = pNif->s_pABVWire;	

			pNif->s_dwLoadID = 0;
			pNif->s_pNiNode = NULL;
			pNif->s_pRefLoad = NULL;
			pNif->s_pABVWire = NULL;
			sLOAD_NIF::DeleteInstance( pNif );
		}
		break;
	case sRES_LOAD::KFM:
		{
			sLOAD_KFM* pKfm = (sLOAD_KFM*)m_pThreadResLoad;

			assert_cs( m_Node.m_pNiNode == NULL );
			m_dwLoadID = pKfm->s_dwLoadID;
			assert_cs( m_dwLoadID != 0 );
			m_dwVisiblePlag = pKfm->s_dwVisiblePlag;
			m_dwChangeApplyModelID = pKfm->s_dwChageApplyModel;
			m_Node.SetNiObject( NiDynamicCast( NiNode, pKfm->s_pActorMng->GetNIFRoot() ), m_CharProp.s_eInsertGeomType, true );
			GetProp_Animation()->SetActorMng( pKfm->s_pActorMng, pKfm->s_pSequenceMng );			

			// 충돌 박스
			m_pABVWire = pKfm->s_pABVWire;

			pKfm->s_dwLoadID = 0;
			pKfm->s_pActorMng = NULL;
			pKfm->s_pRefLoad = NULL;
			pKfm->s_pABVWire = NULL;
			pKfm->s_pSequenceMng = NULL;
			sLOAD_KFM::DeleteInstance( pKfm );
		}
		break;
	default:
		assert_cs( false );
	}
	m_pThreadResLoad = NULL;

	// 위치 정보 설정
	m_Node.m_pNiNode->SetTranslate( m_vPos );
	
	// 회전도착 방향으로 회전 시켜 준다.
	NiMatrix3 mat;
	mat.MakeZRotation( m_fCurRot );
	m_Node.m_pNiNode->SetRotate( mat );	
	m_Node.m_pNiNode->SetScale( m_fScaleResult );	
	m_Node.m_pNiNode->Update( 0.0f );	

#ifdef CRASHDUMP_0305			//chu8820 - 본 데이터 업데이트 실패 ( C005 )
	int nSize = m_Node.m_vpGeom.Size();
	for( int i = 0 ; i < nSize ; i++ )
	{
		NiGeometry* pGeom = (NiGeometry*)( m_Node.m_vpGeom[i] );
		if( pGeom == NULL )
			DUMPLOG( " C005 - 1 " );
		else
		{
			NiSkinInstance* pSkin = pGeom->GetSkinInstance();
			if( pSkin == NULL )
				DUMPLOG( " C005 - 2 " );
			else
			{
				NiSkinData::BoneData* pBoneData = pSkin->GetSkinData()->GetBoneData();
				if( pBoneData == NULL )	//본 데이터 없음
					DUMPLOG( " C005 - 3 " );
			}
		}
	}
#endif
	// 쉐이더 셋팅
	switch( nCallRTTI )
	{
	case RTTI_EFFECTOBJECT:
		break;
	default:
		m_Node.m_pNiNode->UpdateNodeBound();
		m_Node.SaveNiBit();
		m_Node.ActiveShader();
	}
}

/**
 * \author      :	      song
 * \remarks	: 실제 모델이 완료되었는지 체크
 * \return 모델파일 로드 완료시 true : 아직 아니라면 false 호출
 */
CsC_AvObject::eCHECK_TYPE CsC_AvObject::CheckModel()
{
	_SYNCHRONIZE_THREAD_( &m_dwLoadPlag.m_CS );

	if( GetRefLinkObject() == 0 )
	{
		if( m_dwLoadPlag.m_nDWORD == LOAD_COMPLEATE )
			return CHECK_LOADED;

		if( ( m_dwLoadPlag.m_nDWORD & LOAD_FIRST_UPDATE ) == 0 )
			return CHECK_NOT_LOAD;

		if( _IsInitialLoad() )
		{
			m_dwLoadPlag.m_nDWORD |= LOAD_INITITAL_LOAD;

			if( ( m_dwLoadPlag.m_nDWORD & LOAD_DELETE ) == 0 )
			{
				for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
				{
					if( m_pProperty[ i ] != NULL )
						m_pProperty[ i ]->CheckModelComp();
				}
			}

			return CHECK_LOAD_COMP;
		}
		return CHECK_NOT_LOAD;
	}

	assert_cs( GetLeafRTTI() != RTTI_EFFECTOBJECT );

	// 모델 파일이 쓰레드로 부터 로드 되었다면
	if( m_dwLoadPlag.m_nDWORD & LOAD_MODEL_FILE )
	{
		// CsNode 및 ActorMng 셋팅 - m_pThreadResLoad 포인터 초기화
		_LoadedFile( GetRTTI() );

		m_dwLoadPlag.m_nDWORD |= LOAD_FIRST_UPDATE;
		if( _IsInitialLoad() )
			m_dwLoadPlag.m_nDWORD |= LOAD_INITITAL_LOAD;

		DecreaseRefLinkObject();
		PostLinkObject();

		if( m_dwLoadPlag.m_nDWORD == LOAD_COMPLEATE )
		{
			for( int i=0; i<CsC_Property::MAX_COUNT; ++i )
			{
				if( m_pProperty[ i ] != NULL )
					m_pProperty[ i ]->CheckModelComp();
			}
			return CHECK_LOAD_COMP;
		}
		return CHECK_NOT_LOAD;
	}

	return CHECK_NOT_LOAD;
}

/**
 * \author      :	      song
 * \remarks	: 요청된값 완료시 쓰레드에서 호출
 */
void CsC_AvObject::ThreadCallBack_LoadCompleate( int unitType, int nAddRefCount )
{
	m_dwLoadPlag.Lock_AddOr( LOAD_MODEL_FILE );		
	PARENT_CLASS::ThreadCallBack_LoadCompleate( unitType, nAddRefCount );
}


//====================================================================================
//
//		Model
//
//====================================================================================
/**
 * \author      :	      song
 * \remarks	: 현재 모델의 중심값
 * \return 중심값 반환
 */
NiPoint3 CsC_AvObject::GetCenter()
{
	CsNodeObj* pNodeObj = GetCsNode();
	if( pNodeObj )
	{
		pNodeObj->m_pNiNode->UpdateNodeBound();
		pNodeObj->m_pNiNode->UpdateWorldBound();
		return pNodeObj->m_pNiNode->GetWorldBound().GetCenter();
	}
	// 로드가 완료 되지 않았다면 툴 중심값 
	NiPoint3 pos = GetPos();
	pos.z += GetToolHeight()*0.5f;
	return pos;
}

//====================================================================================
//
//		Path
//
//====================================================================================
/**
 * \author      :	      song
 * \remarks	: 패쓰 제거
 */
void CsC_AvObject::DeletePath()
{
	GetProp_Path()->DeletePath();	
}

NiPoint2 CsC_AvObject::KnockBack( NiPoint2 vHitterPos )
{
	NiPoint2 vKnockBackPos = CsC_AttackProp::CalKnockBackPos( vHitterPos, GetPos2D(), GetProp_Path(), 300.0f );
	GetProp_Path()->AbsolutePath( vKnockBackPos.x, vKnockBackPos.y, 2000 );

	ANI::eTYPE ani = GetProp_Animation()->IsEnableKnockBack();
	if( ani != ANI::INVALIDE_ID )
	{
		SetPause( PAUSE_ANIMATION, false );
		SetAnimation( ANI::DAMAGE_BLOCK );
	}

	SetPauseTime( PAUSE_PATH, 1.0f, PTE_PATH, PTE_P_KEYCHECKTIME_RESET );
	SetPauseTime( PAUSE_ANIMATION, 1.0f, PTE_ANIMATION, ANI::IDLE_ATTACK );
	return vKnockBackPos;
}

//====================================================================================
//
//		Sound
//
//====================================================================================

void* CsC_AvObject::PlayAniSound( DWORD dwAni )
{
	if( CsC_AvObject::g_bEnableVoice == false )
		return NULL;

	char* cDir = GetSoundDir();
	if( cDir == NULL )
		return NULL;

	char cPath[ MAX_PATH ];
	strcpy_s( cPath, MAX_PATH, cDir );
	switch( dwAni )
	{
	case ANI::IDLE_CHAR:
		strcat_s( cPath, MAX_PATH, "\\Idle_Char.wav" );
		break;
	case ANI::IDLE_SHAKE:
		strcat_s( cPath, MAX_PATH, "\\Idle_Shake.wav" );
		break;
	case ANI::DAMAGE_DEAD:
		strcat_s( cPath, MAX_PATH, "\\Die.wav" );
		break;
	case ANI::DIGIMON_EVOLUTION:
		strcat_s( cPath, MAX_PATH, "\\Evo01.wav" );
		break;
	case ANI::DIGIMON_EVOLUTION_END:
		strcat_s( cPath, MAX_PATH, "\\Evo02.wav" );
		break;
	case ANI::ATTACK_SKILL1:
		if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION )
			strcat_s( cPath, MAX_PATH, "\\Skill01_tran.wav" );
		else
			strcat_s( cPath, MAX_PATH, "\\Skill01.wav" );			
		break;
	case ANI::ATTACK_SKILL2:
		if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION )
			strcat_s( cPath, MAX_PATH, "\\Skill02_tran.wav" );
		else
			strcat_s( cPath, MAX_PATH, "\\Skill02.wav" );	
		break;
	case ANI::ATTACK_SKILL3:
		if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION )
			strcat_s( cPath, MAX_PATH, "\\Skill03_tran.wav" );
		else
			strcat_s( cPath, MAX_PATH, "\\Skill03.wav" );	
		break;
	case ANI::ATTACK_SKILL4:
		if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION )
			strcat_s( cPath, MAX_PATH, "\\Skill04_tran.wav" );
		else
			strcat_s( cPath, MAX_PATH, "\\Skill04.wav" );	
		break;
	case ANI::ATTACK_SKILL5:
		if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION )
			strcat_s( cPath, MAX_PATH, "\\Skill05_tran.wav" );
		else
			strcat_s( cPath, MAX_PATH, "\\Skill05.wav" );	
		break;
	case ANI::NPC_NO:
		strcat_s( cPath, MAX_PATH, "\\Tactics_Fail.wav" );
		break;
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		{
			if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION )
				strcat_s( cPath, MAX_PATH, "\\Skill05_tran.wav" );
			else
				strcat_s( cPath, MAX_PATH, "\\Skill05.wav" );	
		}break;
	default:
		assert_cs( false );
		return NULL;
	}	

	return PlayStaticSound( cPath );
}

//====================================================================================
//
//		Property
//
//====================================================================================
/**
 * \author      :	      song
 * \remarks	: 기본 애니메이션 호출
 */
void CsC_AvObject::SetDefault_IdleAnimation()
{	
	if( ( IsProperty( CsC_Property::PATH ) ) && ( IsPause( PAUSE_PATH ) == false ) && GetProp_Path()->IsMove() )
	{
		_SetDefault_IdleAnimation_Move();
	}
	else
	{
		_SetDefault_IdleAnimation_Stand();
	}
}

/**
 * \author      :	      song
 * \remarks	: 서있을 기본 애니메이션 호출
 */
void CsC_AvObject::_SetDefault_IdleAnimation_Stand()
{
	GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
}

/**
 * \author      :	      song
 * \remarks	: 움직일 때 기본 애니메이션 호출
 */
void CsC_AvObject::_SetDefault_IdleAnimation_Move()
{
	GetProp_Animation()->SetAnimation( ANI::MOVE_RUN );
}

bool CsC_AvObject::AttachEffect( char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad /*=false*/ )
{
	if( m_pProperty[ CsC_Property::EFFECT ] == NULL )
		return false;

	static_cast<CsC_EffectProp*>(m_pProperty[ CsC_Property::EFFECT ])->AddEffect( cEffectPath, fScale, dwPlag, bPreLoad );
	return true;
}

//====================================================================================
//
//		상태
//
//====================================================================================
// 디지몬의 속성이 하나만 있는 경우의 함수 2개이상이 되면 다른 함수만들어야함.
nsCsDigimonTable::eNATURE_TYPE	CsC_AvObject::GetNatureType2()
{
	for(int i=0; i < 3; i++)
	{
		if(0 != m_eNatureTypes[i])
		{
			return m_eNatureTypes[i];
		}
	}
	return nsCsDigimonTable::NT_NONE;
}

bool CsC_AvObject::IsStrongNature2( CsC_AvObject* pTarget )
{
	// 내가 논이면 무조건 강하지 않다
	if( GetNatureType2() == nsCsDigimonTable::NT_NONE )
		return false;

	nsCsDigimonTable::eNATURE_TYPE eTarget = pTarget->GetNatureType2();
	// 상대방이 논이면 무조건 강하다
	if( eTarget == nsCsDigimonTable::NT_NONE )
		return true;

	switch( GetNatureType2() )
	{
	case nsCsDigimonTable::NT_FIRE:
		if( eTarget == nsCsDigimonTable::NT_ICE )
			return true;
		return false;
	case nsCsDigimonTable::NT_ICE:
		if( eTarget == nsCsDigimonTable::NT_WATER )
			return true;
		return false;
	case nsCsDigimonTable::NT_WATER:
		if( eTarget == nsCsDigimonTable::NT_FIRE )
			return true;
		return false;
	case nsCsDigimonTable::NT_WIND:
		if( eTarget == nsCsDigimonTable::NT_WOOD )
			return true;
		return false;
	case nsCsDigimonTable::NT_WOOD:
		if( eTarget == nsCsDigimonTable::NT_EARTH )
			return true;
		return false;
	case nsCsDigimonTable::NT_EARTH:
		if( eTarget == nsCsDigimonTable::NT_WIND )
			return true;
		return false;
	case nsCsDigimonTable::NT_STEEL:
		if( eTarget == nsCsDigimonTable::NT_LIGHT )
			return true;
		return false;
	case nsCsDigimonTable::NT_LIGHT:
		if( eTarget == nsCsDigimonTable::NT_DARKNESS )
			return true;
		return false;
	case nsCsDigimonTable::NT_DARKNESS:
		if( eTarget == nsCsDigimonTable::NT_THUNDER )
			return true;
		return false;
	case nsCsDigimonTable::NT_THUNDER:
		if( eTarget == nsCsDigimonTable::NT_STEEL )
			return true;
		return false;
	}
	assert_cs( false );
	return false;
}

bool CsC_AvObject::IsStrongNature( CsC_AvObject* pTarget )
{
	// 내가 논이면 무조건 강하지 않다
	if( GetNatureType() == nsCsDigimonTable::NT_NONE )
		return false;

	nsCsDigimonTable::eNATURE_TYPE eTarget = pTarget->GetNatureType();
	// 상대방이 논이면 무조건 강하다
	if( eTarget == nsCsDigimonTable::NT_NONE )
		return true;

	switch( GetNatureType() )
	{
	case nsCsDigimonTable::NT_FIRE:
		if( eTarget == nsCsDigimonTable::NT_ICE )
			return true;
		return false;
	case nsCsDigimonTable::NT_ICE:
		if( eTarget == nsCsDigimonTable::NT_WATER )
			return true;
		return false;
	case nsCsDigimonTable::NT_WATER:
		if( eTarget == nsCsDigimonTable::NT_FIRE )
			return true;
		return false;
	case nsCsDigimonTable::NT_WIND:
		if( eTarget == nsCsDigimonTable::NT_WOOD )
			return true;
		return false;
	case nsCsDigimonTable::NT_WOOD:
		if( eTarget == nsCsDigimonTable::NT_EARTH )
			return true;
		return false;
	case nsCsDigimonTable::NT_EARTH:
		if( eTarget == nsCsDigimonTable::NT_WIND )
			return true;
		return false;
	case nsCsDigimonTable::NT_STEEL:
		if( eTarget == nsCsDigimonTable::NT_LIGHT )
			return true;
		return false;
	case nsCsDigimonTable::NT_LIGHT:
		if( eTarget == nsCsDigimonTable::NT_DARKNESS )
			return true;
		return false;
	case nsCsDigimonTable::NT_DARKNESS:
		if( eTarget == nsCsDigimonTable::NT_THUNDER )
			return true;
		return false;
	case nsCsDigimonTable::NT_THUNDER:
		if( eTarget == nsCsDigimonTable::NT_STEEL )
			return true;
		return false;
	}
	assert_cs( false );
	return false;
}

/**
 * \author      :	      song
 * \remarks	: AttackPropery 에서 호출 ( 공격 이벤트 처리 )
 * \param pEvent 이벤트
 */
void CsC_AvObject::SceneAttackPropEvent( CsC_AttackProp::sEVENT_INFO* pEvent )
{
	switch( pEvent->s_eEventType )
	{
	case CsC_AttackProp::EVENT_END_BATTLE:
		{
			SetBattleEnd();
		}
		break;
	case CsC_AttackProp::EVENT_SKILL_UP:
		{
			SceneSkillUp( pEvent->s_nLevel, pEvent->s_nPoint, pEvent->s_nExp, pEvent->s_nEvoIndex );
		}
		break;
	case CsC_AttackProp::EVENT_LEVEL_UP:
		{
			SceneLevelUp( pEvent->s_nLevel );
		}
		break;
	default:
		assert_cs( false );
	}
}

/**
 * \author      :	      song
 * \remarks	: 레벨업 시 호출
 * \param nLevel 레벨업 하고 나서의 레벨
 */
void CsC_AvObject::SceneLevelUp( int nLevel )
{	
	m_pBaseStat->SetLevel( nLevel );
}

//====================================================================================
//
//		Transform
//
//====================================================================================

void CsC_AvObject::PauseEndTime( ePAUSE_TIME_END ePTEType, int nValue )
{
	switch( ePTEType )
	{
	case PTE_NONE:
		break;
	case PTE_PATH:
		{
			switch( nValue )
			{
			case PTE_P_NONE:
				break;
			case PTE_P_KEYCHECKTIME_RESET:
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	case PTE_ANIMATION:
		{
			// 셋팅된 애니메이션 있는가
			if( nValue != ANI::INVALIDE_ID )
			{
				SetAnimation( (ANI::eTYPE)nValue );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

void CsC_AvObject::_UpdatePause()
{
	for( int i=0; i<PAUSE_COUNT; ++i )
	{
		if( m_PauseArray[ i ].s_bEnableTimeSeq == false )
			continue;

		if( m_PauseArray[ i ].s_PauseTimeSeq.IsEnable() == false )
			continue;

		m_PauseArray[ i ].s_bEnableTimeSeq = false;
		SetPause( (ePAUSE_TYPE)i, false );

		PauseEndTime( m_PauseArray[ i ].s_ePTEType, m_PauseArray[ i ].s_nValue );
	}
}

void  CsC_AvObject::SetDoNotMoveObject( bool bValue )
{ 
	SetPause( PAUSE_PATH, true );
	SetPause( PAUSE_ROTATION, true );
	m_bDoNotObject = bValue;
}

void CsC_AvObject::SetPauseAll( bool bPause )
{
	for( int i=0; i<PAUSE_COUNT; ++i )
		SetPause( (ePAUSE_TYPE)i, bPause );
}

void CsC_AvObject::SetPause( ePAUSE_TYPE type, bool bPause )
{
	assert_cs( type<PAUSE_COUNT );

	if( type >= PAUSE_COUNT )
		return;

	// 움직일 수 없는 오브젝트의 경우
	// 이동 및 회전을 풀수 없다.
	if( !bPause && m_bDoNotObject  )
	{
		switch( type )
		{
		case PAUSE_PATH:
		case PAUSE_ROTATION:
			return;
		}
	}

	m_PauseArray[ type ].s_bPause = bPause;
}

void CsC_AvObject::SetPauseTime( ePAUSE_TYPE type, float fTime, ePAUSE_TIME_END pte /*=PTE_NONE*/, int nValue /*=0*/ )
{
	m_PauseArray[ type ].s_bPause = true;
	m_PauseArray[ type ].s_PauseTimeSeq.SetDeltaTime( (DWORD)(fTime*1000) );
	m_PauseArray[ type ].s_PauseTimeSeq.OffReset();
	m_PauseArray[ type ].s_bEnableTimeSeq = true;
	m_PauseArray[ type ].s_ePTEType = pte;
	m_PauseArray[ type ].s_nValue = nValue;
}




/**
 * \author      :	      song
 * \remarks	: 위치 설정 함수
 * \param vPos 높이값이 제외된 위치
 */
void CsC_AvObject::SetPos( NiPoint2 vPos )
{
	assert_cs( nsCsGBTerrain::g_pCurRoot != NULL );
	NiPoint3 pos3;
	pos3.x = vPos.x;
	pos3.y = vPos.y;
	pos3.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	SetPos( pos3 );
}

/**
 * \author      :	      song
 * \remarks	: 위치 설정 함수
 * \param vPos 위치
 */
void CsC_AvObject::SetPos( NiPoint3 vPos )
{
	m_vPos = vPos;
	m_vPos.z += m_fHeightConstant;

	if( IsLoad() == true )
	{
		m_Node.m_pNiNode->SetTranslate( m_vPos );
	}	

	//3D사운드 업데이트
	if( nsCsGBTerrain::g_pCsSoundMng )
		nsCsGBTerrain::g_pCsSoundMng->Update_MoveSound( this, GetPos() );
}

/**
 * \author      :	      song
 * \remarks	: 게임 스케일 설정 함수 ( 실제 스케일 = 게임스케일*툴스케일*파일테이블스케일 )
 * \param fScale 
 */
void CsC_AvObject::SetScale( float fScale, bool bOrgScale /*=true*/ )
{
	if( bOrgScale == true )
		m_fScaleOrgGame = fScale;

	m_fScaleGame = fScale;
	m_fScaleResult = m_fScaleGame * _GetScaleFT() * _GetScaleTool();
	m_fScaleResultSqrt = sqrt( m_fScaleResult );

	if( IsLoad() == true )
	{
		m_Node.m_pNiNode->SetScale( m_fScaleResult );
	}
}

/**
 * \author      :	      song
 * \remarks	: 회전값 업데이트 - 시간에 따른 순차 애니메이션
 * \param fDeltaTime 변화시간
 */
void CsC_AvObject::_UpdateRotation( float fDeltaTime )
{
	if( IsPause( PAUSE_ROTATION ) == true )
		return;

//#define ROT_SPEED	6.25f
	#define ROT_SPEED	11.0f

	if( abs( m_fDestRot-m_fCurRot ) < 0.01f
		&& m_fXRot == 0.0f )
	{
		if( IsLoad() == false )
			return;
		if( m_Node.m_pNiNode == NULL )
			return;

		// 경사면에서 라이딩 해제 후 이동 전까지 x 회전값 초기화 안 됨
		// x축만 초기화
		float x, y, z;
		m_Node.m_pNiNode->GetRotate().ToEulerAnglesZXY( z, x, y );
		NiMatrix3 rot;
		rot.FromEulerAnglesZXY( z, 0.0f, y );
		m_Node.m_pNiNode->SetRotate( rot );
		return;
	}

	// 왼쪽으로 돌지 오른쪽으로 돌지
	NiMatrix3 mat;
	mat.MakeZRotation( m_fDestRot );
	NiPoint3 vDestDir = mat*NiPoint3( 0.0f, -1.0f, 0.0f );

	float fDeltaRot = NiACos( m_vDir.Dot( vDestDir ) );
	if( m_vDir.Cross( vDestDir ).z > 0 )
		fDeltaRot = -fDeltaRot;
	
	float fRotSpeed = 0.0f;
	if( m_pBaseStat )
		fRotSpeed = m_pBaseStat->GetMoveSpeed() * 0.002f;

	float fRot = (ROT_SPEED + fRotSpeed) * fDeltaTime;

	if( fabs( fDeltaRot ) < fRot )
	{
		fRot = fabs( fDeltaRot );
	}
	if( fDeltaRot < 0.0f )
	{
		fRot = -fRot;
	}

	SetRotation( m_fCurRot + fRot );
}

/**
 * \author      :	      song
 * \remarks	: 회전값 셋팅
 * \param pObject 해당 오브젝트를 바라보게 해당값 셋팅
 * \param bIncludeDestRot - true : 회전 애니업데이트 안되게 데스트 같이 셋팅 
 */
void CsC_AvObject::SetRotation( CsC_AvObject* pObject, bool bIncludeDestRot /*=false*/ )
{
	SetRotation( pObject->GetPos(), bIncludeDestRot );
}

/**
* \author      :	      song
* \remarks	: 회전값 셋팅
* \param pObject 해당 위치를 바라보게 해당값 셋팅
* \param bIncludeDestRot - true : 회전 애니업데이트 안되게 데스트 같이 셋팅 
*/
void CsC_AvObject::SetRotation( NiPoint3 vDest, bool bIncludeDestRot /*=false*/ )
{
	NiPoint3 vDestDir = vDest - m_vPos;
	vDestDir.z = 0.0f;
	vDestDir.Unitize();
	float fRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( vDestDir ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( vDestDir ).z > 0 )
		fRot = -fRot;

	SetRotation( fRot, bIncludeDestRot );
}

/**
 * \author      :	      song
 * \remarks	: 회전값 셋팅
 * \param fRot 회전값으로 셋팅
 * \param bIncludeDestRot - true : 회전 애니업데이트 안되게 데스트 같이 셋팅 
 */
void CsC_AvObject::SetRotation( float fRot, bool bIncludeDestRot /*=false*/ )
{
	m_fCurRot = fRot;
	if( bIncludeDestRot == true )
		m_fDestRot = fRot;

	while( m_fCurRot > NI_PI )		m_fCurRot -= NI_PI*2;
	while( m_fCurRot < -NI_PI )		m_fCurRot += NI_PI*2;	

	//NiMatrix3 mat;
	//mat.MakeZRotation( m_fCurRot );		
	//m_vDir = mat*NiPoint3( 0, -1, 0 );	

	//if( IsLoad() == true )
	//{
	//	m_Node.m_pNiNode->SetRotate( mat );
	//}

	_SetRotationNode();
}

void CsC_AvObject::_SetRotationNode()
{
	NiMatrix3 mat;
	NiMatrix3 matDir;
	
	if( m_fXRot != 0.0f ) //x축으로 기우는 회전이 있을시에
	{		
		mat.FromEulerAnglesZXY( m_fCurRot, m_fXRot, 0 );			
	}
	else //일반 적으로 x축으로만 회전하는경우
	{
		mat.MakeZRotation( m_fCurRot );					
	}			

	matDir.MakeZRotation( m_fCurRot );
	m_vDir = matDir*NiPoint3( 0, -1, 0 );

	if( IsLoad() == true )
	{
		m_Node.m_pNiNode->SetRotate( mat );
	}
}



/**
 * \author      :	      song
 * \remarks	: 벡터 v0에서 v1까지 호를 그리는 회전 사원수\n 
 * \param p 리턴되는 쿼터니언 
 * \param v0 단위벡터
 * \param v1 단위벡터
 */
void AVObjquatRotationArc( NiQuaternion* p, NiPoint3 v0, NiPoint3 v1)
{
	NiPoint3 c = v0.Cross( v1 );
	float s = sqrtf( ( 1.0f+ v0.Dot( v1 ) )*2.0f);
	p->SetX( c.x/s );
	p->SetY( c.y/s );
	p->SetZ( c.z/s );
	p->SetW( s/2.0f );
}

/**
 * \author      :	      song
 * \remarks	: 모든 축에서 해당 위치를 바라보는 회전( 다른 회전은 오직 z축 회전만 함 )
 * \param vDest 바라봐야 하는 해당 점
 */
void CsC_AvObject::SetRotation_AllAxis( NiPoint3 vDest )
{
	// 이후의 일반 로테이션은 무시하려는 강인한 의지
	m_fDestRot = m_fCurRot;

	NiPoint3 vDestDir = vDest - m_vPos;
	vDestDir.Unitize();

	NiPoint3 ptOrg( 0, -1, 0 );
	float fValue = vDestDir.Dot( ptOrg );

	NiMatrix3 mat;
	if( abs( fValue ) < 0.9998f )
	{
		NiQuaternion q;
		AVObjquatRotationArc( &q, ptOrg, vDestDir );		
		q.ToRotation( mat );
	}
	else
	{
		NiMatrix3 mat;
		if( fValue < 0.0f )
		{
			mat.MakeZRotation( -NI_PI );
		}
		else
		{
			mat.MakeIdentity();
		}
	}

	m_vDir = mat*NiPoint3( 0, -1, 0 );
	//m_vDir.x = -m_vDir.x;
	
	if( IsLoad() == true )
	{
		m_Node.m_pNiNode->SetRotate( mat );
	}
}


/**
 * \author      :	      song
 * \remarks	: 회전 애니값 셋팅
 * \param pObject 쳐다봐야 하는 오브젝트 
 */
void CsC_AvObject::SetRotation_Ani( CsC_AvObject* pObject )
{
	assert_cs( pObject != NULL );
	NiPoint3 vDstDir = pObject->GetPos() - m_vPos;
	SetRotation_AniDir( vDstDir );
}

/**
 * \author      :	      song
 * \remarks	: 회전 애니값 셋팅
 * \param vDestPos 쳐다봐야하는 점
 */
void CsC_AvObject::SetRotation_Ani( NiPoint3 vDestPos )
{
	NiPoint3 vDstDir = vDestPos - m_vPos;	
	SetRotation_AniDir( vDstDir );
}

/**
 * \author      :	      song
 * \remarks	: 회전 애니값 셋팅
 * \param fRot 회전해야하는 값
 */
void CsC_AvObject::SetRotation_Ani( float fRot )
{
	m_fDestRot = fRot;
	while( m_fDestRot < 0 )
	{
		m_fDestRot += NI_PI*2.0f;
	}
	while( m_fDestRot > NI_PI*2.0f )
	{
		m_fDestRot -= NI_PI*2.0f;
	}
}

/**
 * \author      :	      song
 * \remarks	: 회전 애니 변환 셋팅
 * \param fRotDelta 회전 변환 값
 */
void CsC_AvObject::SetRotation_AniDelta( float fRotDelta )
{
	m_fDestRot += fRotDelta;
	while( m_fDestRot < 0 )
	{
		m_fDestRot += NI_PI*2.0f;
	}
	while( m_fDestRot > NI_PI*2.0f )
	{
		m_fDestRot -= NI_PI*2.0f;
	}
}

/**
 * \author      :	      song
 * \remarks	: 회전 애니값 셋팅 - 방향에 따른
 * \param vDestDir 쳐다봐야하는 방향
 */
void CsC_AvObject::SetRotation_AniDir( NiPoint3 vDestDir )
{
	vDestDir.z = 0.0f;
	vDestDir.Unitize();
	float fDestRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( vDestDir ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( vDestDir ).z > 0 )
		fDestRot = -fDestRot;

	m_fDestRot = fDestRot;	
}

//====================================================================================
//
//	픽킹
//
//====================================================================================
/**
 * \author      :	      song
 * \remarks	: 오브젝트 픽킹
 * \param pCsGBPick 픽킹 포인터 - 픽킹된 모든 정보를 가지고 있다
 * \param vOrigin 픽킹시작점
 * \param vDir 픽킹방향
 * \param fLength 픽킹 거리
 * \return 픽킹 되면 true, 안되면 false
 */
bool CsC_AvObject::PickObject( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, float& fLength )
{
	if( IsPickEnable() == false )
		return false;

	if( pCsGBPick->Pick( m_pABVWire, *vOrigin, *vDir, false ) == true )
	{
		float fDist = pCsGBPick->GetRecord()->GetDistance();
		if( fDist < fLength )
		{
			fLength = fDist;
			return true;
		}
	}
	return false;
}


//====================================================================================
//
//		부가 정보
//
//====================================================================================
bool CsC_AvObject::IsHaveRangeEvent(DWORD const& dwAniId)
{
	if( m_pProperty[ CsC_Property::ANIMATION ] == NULL )
		return false;

	CsC_AniProp* pAniProp = dynamic_cast<CsC_AniProp*>(m_pProperty[ CsC_Property::ANIMATION ]);
	if( NULL == pAniProp )
		return false;

	CsC_AniProp_Sequence* pSequence = pAniProp->GetSequenceMng();
	if( NULL == pSequence )
		return false;

	return pSequence->IsHaveRangeEvent( dwAniId );
}

void CsC_AvObject::DeleteLoopEffectSkill( DWORD nSkillArrIDX )
{
	if( NULL == m_pProperty[ CsC_Property::EFFECT ] )
		return;

	static_cast<CsC_EffectProp*>( m_pProperty[ CsC_Property::EFFECT ] )->DeleteSkillLoopEffect( nSkillArrIDX );
}

void CsC_AvObject::SetShaderEffect_Highlight()
{
	SAFE_POINTER_RET( g_pSEffectMgr );
	SEffectMgr::st_SHADING_FLAG flag(SEffectMgr::FT_LOOPDEEPSOFT, SEffectMgr::DC_RED, 
									 CGeometry::Color_Sampling, 0, SEffectMgr::UT_INFINITY_TIME, 
									 SEffectMgr::ET_OUTLINE, SEffectMgr::PRIORITY_NONE);
	g_pSEffectMgr->AddSEffect(0.0f, this, flag, 0.5f);
}

void CsC_AvObject::RemoveShaderEffect_Highlight()
{
	SAFE_POINTER_RET( g_pSEffectMgr );
	g_pSEffectMgr->DeleteSEffect(this);
}
//////////////////////////////////////////////////////////////////////////
float CsC_AvObject::GetAttackSpeed()
{
	SAFE_POINTER_RETVAL( m_pBaseStat, 0.0f );
	return m_pBaseStat->GetAttackSpeed();
}

bool CsC_AvObject::IsNearObject( NiPoint2 const& targetPos, float const& fLen, float & fDist )
{
	if( !IsPickEnable() )
		return false;

	// 타겟과의 거리 계산
	fDist = ( targetPos - GetPos2D() ).Length();

	// 범위 계산
	if( fDist > fLen )
		return false;

	return true;
}

std::string CsC_AvObject::GetModelPath() const
{ 
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );
	return g_pModelDataMng->GetKfmPath( GetModelID() );
}

UINT CsC_AvObject::GetApplyModelID() const
{ 
	return ( m_dwChangeApplyModelID == 0 ) ? GetModelID() : m_dwChangeApplyModelID; 
}

/**< 애니메이션 시퀀스 모델적용 할때 사용하는 파일테이블 값 - 오토바이 라이딩때문에 생김 */
UINT CsC_AvObject::GetModelID() const
{ 
	return GetFTID(); 
}

#undef PARENT_CLASS
