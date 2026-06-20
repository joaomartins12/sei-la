
#include "stdafx.h"
#include "DigimonUser.h"

#define		PARENT_CLASS		CDigimon
CsCRTTI_CPP( PARENT_CLASS, CDigimonUser, RTTI_DIGIMON_USER )

CDigimonUser::sUSER_STAT::sUSER_STAT(CsC_AvObject* pParent):sBASE_STAT( pParent ),
s_nHP(CsC_AvObject::INVALIDE_STAT),s_nDS(CsC_AvObject::INVALIDE_STAT),
s_nMaxHP(CsC_AvObject::INVALIDE_STAT),s_nMaxDS(CsC_AvObject::INVALIDE_STAT),
s_dwExp(CsC_AvObject::INVALIDE_STAT),s_fBattleEP(0.0f),s_nBL(0.0f)
{
}

CDigimonUser::sUSER_STAT::~sUSER_STAT()
{

}

CsC_AvObject::eSTAT	CDigimonUser::sUSER_STAT::GetType() const
{ 
	return CsC_AvObject::USER_STAT; 
}

int CDigimonUser::sUSER_STAT::GetMaxHP() const
{ 
	return s_nMaxHP; 
}

int CDigimonUser::sUSER_STAT::GetMaxDS() const
{ 
	return s_nMaxDS; 
}

int CDigimonUser::sUSER_STAT::GetHP() const
{ 
	return s_nHP; 
}

int CDigimonUser::sUSER_STAT::GetDS() const
{ 
	return s_nDS; 
}

float CDigimonUser::sUSER_STAT::GetHPRate() const
{ 
	return s_nHP/(float)s_nMaxHP; 

}

float CDigimonUser::sUSER_STAT::GetDSRate() const
{
	return s_nDS/(float)s_nMaxDS; 
}

int CDigimonUser::sUSER_STAT::GetFriendShip() const
{ 
	return s_nFriendShip; 
}

void CDigimonUser::sUSER_STAT::SetMaxHP( int nMaxHP )
{ 
	s_nMaxHP = nMaxHP; 
}

void CDigimonUser::sUSER_STAT::SetMaxDS( int nMaxDS )
{ 
	s_nMaxDS = nMaxDS; 
}

void CDigimonUser::sUSER_STAT::SetDeltaMaxHP( int nDelta )
{ 
	s_nMaxHP = CsMax( 0, s_nMaxHP+nDelta ); 
}

void CDigimonUser::sUSER_STAT::SetHP( int nHP )
{ 
	s_nHP = nHP; 
	SetBackupHP( nHP );
}

void CDigimonUser::sUSER_STAT::SetDS( int nDS )
{ 
	s_nDS = nDS; 
}

void CDigimonUser::sUSER_STAT::SetDeltaHP( int nDelta )
{ 
	s_nHP = CsMin( GetMaxHP(), CsMax( 0, s_nHP+nDelta ) ); 
}

void CDigimonUser::sUSER_STAT::SetDeltaBackupHP( int nDelta )
{ 
	s_nHPBackup = CsMin( GetMaxHP(), CsMax( 0, s_nHPBackup+nDelta ) ); 
}

void CDigimonUser::sUSER_STAT::SetDeltaDS( int nDelta )
{ 
	s_nDS = CsMin( GetMaxDS(), CsMax( 0, s_nDS+nDelta ) ); 
}

void CDigimonUser::sUSER_STAT::SetDeltaBackupDS( int nDelta )
{ 
	s_nDSBackup = CsMin( GetMaxDS(), CsMax( 0, s_nDSBackup+nDelta ) ); 
}

__int64	CDigimonUser::sUSER_STAT::GetExp() const
{ 
	return s_dwExp;
}

void	CDigimonUser::sUSER_STAT::SetExp( __int64 dwExp )
{ 
	s_dwExp = dwExp; 
}

void CDigimonUser::sUSER_STAT::SetFriendShip( int nFS )
{ 
	s_nFriendShip = nFS; 
}

int	CDigimonUser::sUSER_STAT::GetAtt() const
{ 
	return s_nAtt; 
}

int CDigimonUser::sUSER_STAT::GetDef() const
{ 
	return s_nDef;
}

int CDigimonUser::sUSER_STAT::GetCritical() const
{ 
	return s_nCritical; 
}

//int GetAttSpeed() const{ return s_nAttSpeed; }
int CDigimonUser::sUSER_STAT::GetEvade() const
{ 
	return s_nEvade; 
}

int CDigimonUser::sUSER_STAT::GetHitRate() const
{ 
	return s_nHitRate; 
}

int CDigimonUser::sUSER_STAT::GetAttRange() const
{ 
	return s_nAttRange; 
}

void CDigimonUser::sUSER_STAT::SetAtt( int nValue )
{ 
	s_nAtt = nValue; 
}

void CDigimonUser::sUSER_STAT::SetDef( int nValue )
{ 
	s_nDef = nValue; 
}

void CDigimonUser::sUSER_STAT::SetCritical( int nValue )
{ 
	s_nCritical = nValue; 
}
//void CDigimonUser::sUSER_STAT::SetAttSpeed( int nValue ){ s_nAttSpeed = nValue; }
void CDigimonUser::sUSER_STAT::SetEvade( int nValue )
{
	s_nEvade = nValue; 
}

void CDigimonUser::sUSER_STAT::SetHitRate( int nValue )
{ 
	s_nHitRate = nValue; 
}

void CDigimonUser::sUSER_STAT::SetAttRange( int nValue )
{ 
	s_nAttRange = nValue; 
}

void CDigimonUser::sUSER_STAT::SetBattleEP( float fEP )
{ 
	s_fBattleEP = fEP; 
}

float CDigimonUser::sUSER_STAT::GetBattleEP() const
{ 
	return s_fBattleEP; 
}

void CDigimonUser::sUSER_STAT::SetBL(int nValue)
{
	s_nBL = nValue;

}
int CDigimonUser::sUSER_STAT::GetBL() const
{
	return s_nBL;
}
//////////////////////////////////////////////////////////////////////////
CDigimonUser::CDigimonUser()
:m_dwLastNearPortalID(0),m_pFT_Evol(NULL),m_pFT_BattleEvo(NULL),m_pFT_BattleEvoElement(NULL)
{
	m_nJointProgressFlag	=	0;
	m_nEvoTree = 0;
	memset( &m_EvoUnit, 0, sizeof( cEvoUnit ) * nLimit::EvoUnit );	

	m_pQuickSlot = NULL;
	memset( &m_EvoStateArray, 0, sizeof( DWORD ) * nLimit::EvoUnit );

	memset( &m_AttributeExp, 0, sizeof( n2 ) * NewAttribute::MaxDigitalType );	
	memset( &m_NatureExp, 0, sizeof( n2 ) * NewAttribute::MaxNatualType );	
}

CDigimonUser::~CDigimonUser()
{

}

void CDigimonUser::Delete()
{
	PARENT_CLASS::Delete();
	m_pQuickSlot = NULL;
}

void CDigimonUser::PreDelete()
{
	assert_cs( m_pBaseStat != NULL );
	NiDelete (sUSER_STAT*)m_pBaseStat;
	m_pBaseStat = NULL;

	PARENT_CLASS::PreDelete();

	m_UserServerMove.Delete();

	if( g_pGameIF )
	{
		g_pGameIF->GetBuffInfo( IF_BUFF_MY_D )->SetBuffData( NULL );
	}	
}

void CDigimonUser::ChangeKFM( UINT nFTID )
{
	PARENT_CLASS::ChangeKFM( nFTID );

	_Init( false );

	// PostLoad 에 있는 디지몬 정보가 파트너몬, Tactics에 있는 디지몬 정보가 용병 디지몬.
	// 용병 슬롯에 있던 디지몬을 파트너몬으로 교체하면 Tactics에 있던 용병 정보가 PostLoad 로 이동된다.
	cData_PostLoad::sDATA* sDigimonData = g_pDataMng->GetPostLoad()->GetDigimonData();

	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		u1	evoslot = g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot;
		u1	evostat = sDigimonData->s_DCashSkill[i].s_nDigimonEvoStatus;
		if( evoslot != evostat)
			continue;

		for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )
			g_pCharMng->GetDigimonUser( 0 )->GetSkillMng()->SetDigimonCashSkill( sDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode[j], sDigimonData->s_DCashSkill[i].s_nSkillCoolTime[j]);
		
		break;
	}

	// 퀵슬롯
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( m_nDigimonUserIndex + 1 );
	pDataQuickSlot->SetChar( this );
	m_pQuickSlot = pDataQuickSlot->GetData( GetFTEvolCurObj()->m_nEvoSlot );
 	cData_QuickSlot::CheckAllSlot( this, m_pQuickSlot, m_SkillMng.GetSkillCount() );

	// 퀵에볼
	if( m_nDigimonUserIndex == 0 )
		g_pGameIF->GetQuickEvol()->SetTargetObject( this );

	// 디지몬 상태창
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );
}

#ifdef CROSSWARS_SYSTEM
void CDigimonUser::ChangeCrossModel( UINT nFTID, LPCTSTR szName, bool bUser )
{
	PARENT_CLASS::ChangeModel( nFTID, szName );

	if(!bUser)
		return;

	assert_cs( m_nTacticsIndex >= 0 );
	m_dwPrevBaseDigimonID = m_dwBaseDigimonID;
	m_dwBaseDigimonID = nFTID;

	m_pFT_Evol = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_dwBaseDigimonID );

	_Init( false );

	// 퀵슬롯
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( m_nDigimonUserIndex + 1 );
	pDataQuickSlot->SetChar( this );

	m_pQuickSlot = pDataQuickSlot->GetData( GetFTEvolCurObj()->m_nEvoSlot );

	// 퀵에볼
	if( m_nDigimonUserIndex == 0 )
		g_pGameIF->GetQuickEvol()->SetTargetObject( this );				
}


void CDigimonUser::ChangeKFM( UINT nFTID, bool bUser )
{
	PARENT_CLASS::ChangeKFM( nFTID );

	if(!bUser)
		return;

	_Init( false );

	// 퀵슬롯
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( m_nDigimonUserIndex + 1 );
	pDataQuickSlot->SetChar( this );
	m_pQuickSlot = pDataQuickSlot->GetData( GetFTEvolCurObj()->m_nEvoSlot );
	cData_QuickSlot::CheckAllSlot( this, m_pQuickSlot, m_SkillMng.GetSkillCount() );

	// 퀵에볼
	if( m_nDigimonUserIndex == 0 )
		g_pGameIF->GetQuickEvol()->SetTargetObject( this );

	// 디지몬 상태창
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );
}
#endif

void CDigimonUser::ChangeModel( UINT nFTID, LPCTSTR szName )
{
	PARENT_CLASS::ChangeModel( nFTID, szName );

	assert_cs( m_nTacticsIndex >= 0 );
	m_dwPrevBaseDigimonID = m_dwBaseDigimonID;
	m_dwBaseDigimonID = g_pDataMng->GetTactics()->GetTactics( m_nTacticsIndex )->s_dwBaseDigimonID;

	m_pFT_Evol = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_dwBaseDigimonID );

	_Init( false );

	// 용병 슬롯에 있는 디지몬으로 교체하기 때문에 Tactics 의 정보를 가지고 비교함.
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		if( g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot != g_pDataMng->GetTactics()->GetTactics( m_nTacticsIndex )->s_DCashSkill[i].s_nDigimonEvoStatus )
			continue;

		for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )
			g_pCharMng->GetDigimonUser( 0 )->GetSkillMng()->SetDigimonCashSkill( g_pDataMng->GetTactics()->GetTactics( m_nTacticsIndex )->s_DCashSkill[i].s_nDigimonCashSkillCode[j] );
	}

	// 퀵슬롯
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( m_nDigimonUserIndex + 1 );
	pDataQuickSlot->SetChar( this );

	m_pQuickSlot = pDataQuickSlot->GetData( GetFTEvolCurObj()->m_nEvoSlot );
#ifdef UI_INVENTORY_RENEWAL
	cData_QuickSlot::CheckAllSlot( this, m_pQuickSlot, m_SkillMng.GetSkillCount() );
#endif

	// 퀵에볼
	if( m_nDigimonUserIndex == 0 )
		g_pGameIF->GetQuickEvol()->SetTargetObject( this );		
}

void CDigimonUser::Init( uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos )
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	Init( nIDX, nFileTableID, dwBaseDigimonID, vPos, fRot, szName, pProp, nUserIndex, bInitPos );
}

void CDigimonUser::Init( uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos )
{
	assert_cs( m_pBaseStat == NULL );
	assert_cs( pProp->IsOptionPlag( OP_STAT ) );
	m_pBaseStat = NiNew sUSER_STAT( this );

	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, szName, pProp );

	m_dwBaseDigimonID = dwBaseDigimonID;
	m_dwPrevBaseDigimonID = dwBaseDigimonID;
	m_nDigimonUserIndex = nUserIndex;	
	m_UserServerMove.Init( this );

	if( bInitPos == true )
		_InitStartPos();

	m_pFT_Evol = nsCsFileTable::g_pEvolMng->GetEvolveInfo( dwBaseDigimonID );

	m_IdleCheckTimeSeq.SetDeltaTime( 60000 );
	_Init( true );

	// 인터페이스에서 배틀 종료	
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIN_DIGIMON_WINDOW ) );
	g_pGameIF->GetMainDigimonWindow()->SetBattleEnd();

	// 디지몬창 열려 있으면 셋팅 해주자
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_DIGIMONSTATUS ) )
		GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_DIGIMONSTATUS, NULL );

	m_MoveCoolTimeSeq.SetEndTime( 0.3f );
	m_MoveOnceTimeSeq.SetDeltaTime( 200 );
	m_BattleEPDecrTimeSeq.SetDeltaTime( 5000 );
	m_BattleEPDecrTimeSeq.SetEnable();

	// 상태
	m_pFindTarget = NULL;

	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon )
		{
			m_eDigimonUserState = DU_WAIT;
			m_eDigimonUserState_NoneSkill = DU_WAIT;
		}
		else
		{
			m_eDigimonUserState = DU_FOLLOW;
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;
		}
#else
		m_eDigimonUserState = DU_FOLLOW;
		m_eDigimonUserState_NoneSkill = DU_FOLLOW;
#endif
		break;
	case nLIB::SVR_BATTLE:
		m_eDigimonUserState = DU_MANUAL_CONTROL;
		m_eDigimonUserState_NoneSkill = DU_MANUAL_CONTROL;
		break;
	default:
		assert_cs( false );
	}	

	// 퀵슬롯
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( m_nDigimonUserIndex + 1 );
	pDataQuickSlot->SetChar( this );

	m_pQuickSlot = pDataQuickSlot->GetData( GetFTEvolCurObj()->m_nEvoSlot );
	

	// 버프창
	g_pGameIF->GetBuffInfo( IF_BUFF_MY_D )->SetBuffData( &m_BuffData );

	m_SkillRevTimeSeq.SetDeltaTime( 3000 );
	m_SkillRevTimeSeq.OffEnable();

	//스킬 쿨 초기화 옵션 관련 타임 변수
	m_ResetTime.SetEnableTime(false);
	m_ResetTime.Reset();
}

void CDigimonUser::_Init( bool bCreate )
{
	// 여긴 모델 변경, 진화, 초기화 다 들어 온다.
	if( net::game == NULL )
		_NetOffInit();

	// 인터페이스에 설정
	if( m_nDigimonUserIndex == 0 )
	{
		g_pGameIF->GetMainDigimonWindow()->SetTargetObject( this );
		g_pDataMng->GetTactics()->LinkTactics();
	}

	// 조그래스 진화시 용병창을 오메가몬 아이콘으로	
	// 조그레스 진화 중일때 // 현재 조그레스 일때
	if( IsJointProgressing() || IsCurJointProgress())
	{
		CDigimonEvolveObj* pFTEvolObj = NULL;
		if(IsJointProgressing())
			pFTEvolObj = GetFTEvol()->GetEvolveObjByEvoSlot( GetJointProgressFlag() );	// Set 해준정보를 이용해 진화 슬롯정보를 얻는다. 현재 디지몬의 조그레스 진화정보 
		else if(IsCurJointProgress())
			pFTEvolObj = GetFTEvolCurObj();	// 현재 조그레스 이니까 현제상테의 진화정보를 얻는다.


		cData_PostLoad::sDATA*	pMercenary = NULL;
		int		pGetError = 0;
		int		pSlotIndex = -1;
		int		ArryIndex[3] = { -1, };	// 조그레스 진화하기 위해 2개 이상의 디지몬이 필요할때 각각의 Tactices에서 slot index를 저장
		pMercenary = g_pDataMng->GetTactics()->GetJoGressMercenaries(pFTEvolObj->m_dwID, pFTEvolObj->m_nJoGressesNum, pFTEvolObj->m_nJoGress_Tacticses, pSlotIndex, pGetError, ArryIndex );
		
		assert_cs(pMercenary);	// 값을 얻었을 경우만 처리
		assert_cs(-1 != pSlotIndex); // 값을 얻었을 경우만 처리


		// 이거 안씀. 디지몬아이디만으로 슬롯을 찾기에는 한계가 있다.
		// 해당 슬롯인덱스 얻기
		//int nCPIndex  = g_pDataMng->GetTactics()->GetMercenaryID( pMercenary->s_dwBaseDigimonID );
		//assert_cs( cData_Tactics::INVALIDE_TACTICS_INDEX != nCPIndex );		

		// 해당 슬롯에 아이콘바꾸기
		if( pFTEvolObj->m_nJoGressesNum == 1 )
		{
			g_pGameIF->GetTacticsWindow( pSlotIndex )->SetTargetObject( pFTEvolObj->m_dwID );	
			g_pGameIF->GetTacticsWindow( pSlotIndex )->SetDisable();
		}
		else
		{
			for( int i=0; i<pFTEvolObj->m_nJoGressesNum; ++i )
			{
				if( ArryIndex[i] == -1 )	// 값이 -1은 초기값(조그레스 진화에 필요한 디지몬이라면 값이 -1이 될 수 없음.)
					continue;

				g_pGameIF->GetTacticsWindow( ArryIndex[i] )->SetTargetObject( pFTEvolObj->m_dwID );	
				g_pGameIF->GetTacticsWindow( ArryIndex[i] )->SetDisable();
			}
		}
	}	

	g_pGameIF->GetQuickSlot( cQuickSlot::Q_DIGIMON )->SetTargetObject( this );
	m_bWarningHP = m_bWarningDS = false;
	m_IdleCheckTimeSeq.OffReset();

	// 디지몬 대화 로드		
	TCHAR sz[ MAX_FILENAME ];
	M2W( sz, GetSoundDir(), MAX_FILENAME );	
	g_pResist->LoadDigimonTalk( sz );

	m_fClientAttackTime = 8000.0f;

	_ResetBattleEvo( bCreate );
	m_dwLastNearPortalID = 0;
}

void CDigimonUser::_ResetBattleEvo( bool bCreate )
{
	// 배틀 에볼 테이블 로드
	int nBattleEvoType = m_pFT_Evol->GetInfo()->s_nBattleEvoType;
	m_pFT_BattleEvo = nsCsFileTable::g_pEvolMng->GetBattleEvo( nBattleEvoType );
	m_pFT_BattleEvoElement = NULL;
	for( int i=0; i<FT_BATTLE_EVO_ELEMENT_CNT; ++i )
	{
		if( m_pFT_BattleEvo->s_Element[ i ].s_bEnable == false )
			continue;
		if( m_pFT_BattleEvo->s_Element[ i ].s_nEvolStep != GetFTDigimon()->GetInfo()->s_eEvolutionType )
			continue;

		m_pFT_BattleEvoElement = &m_pFT_BattleEvo->s_Element[ i ];
	}
}

void CDigimonUser::_NetOffInit()
{
	cProfile x( "dmo.ini" );
	GetBaseStat()->SetMoveSpeed( (float)x.GetInt( "CHAR", "Dspeed" ) );

	/*GetBaseStat()->SetMaxHP( 1000 );
	GetBaseStat()->SetHP( 800 );

	GetBaseStat()->SetMaxDS( 1000 );
	GetBaseStat()->SetDS( 800 );*/

	if( x.GetInt( "CHAR", "Battle" ) == 1 )
	{
		SetBattle();
	}
}

void CDigimonUser::_InitStartPos()
{
	NiPoint2 niTargetPos = _GetArrivePos_ByTamer();
	NiPoint2 vResultPos;
	if( GetProp_Path()->CalTargetPath( vResultPos, NiPoint3(niTargetPos.x, niTargetPos.y, 0.0f), 0 ) == true )
	{
		//if( GetProp_Path()->SetPath( vResultPos ) )
		//{
			NiPoint3 startPos;
			startPos.x=vResultPos.x;
			startPos.y=vResultPos.y;
			startPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vResultPos );
			SetPos( startPos );

			GetProp_Path()->ResetAgent();
			m_UserServerMove.SetLastSyncPos( (int)startPos.x, (int)startPos.y );
		//}
	}
	//-------------------------------------------//
	//#6350 디지몬 조건이 걸려있는 퀘스트가 특정 조건에서 발생이 되지 않는 오류 와 연관 있음 20170621
	//맵 이동시 퀘스트 상태를 체크한다.
	//g_pDataMng->GetQuest()->RevQuestCheck();
	//-------------------------------------------//
// 	//====================================================================================================
// 	// 초기 위치 셋팅
// 	//====================================================================================================
// 	NiPoint3 vPath = NiPoint3( m_vPos.x, m_vPos.y, 0.0f );
// 	NiPoint3 vPath2( 0, 0, 0 );	
// 	NiPoint3 vDir = NiPoint3( g_pCharMng->GetTamerUser()->GetDir().x, g_pCharMng->GetTamerUser()->GetDir().y, 0 );
// 	vPath -= vDir * 150.f;
// 	vPath2 = vPath;
// 
// 	NiPoint3 cross = NiPoint3( 0, 0, 1 );
// 	NiPoint3 v = cross.Cross( vDir );
// 	switch( m_nDigimonUserIndex )
// 	{
// 	case 0:		vPath -= v * 100.f;			break;
// 	case 1:		vPath += v * 100.f;			break;
// 	case 2:		vPath -= vDir * 150.f;		break;
// 	}
// 
// 	if( GetProp_Path()->PathTest( vPath ) == true )
// 	{
// 		vPath.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPath );
// 		SetPos( vPath );
// 
// 		GetProp_Path()->ResetAgent();		
// 	}
// 	else
// 	{
// 		assert_cs( vPath2.x != 0 && vPath2.y != 0 );
// 		if( GetProp_Path()->PathTest( vPath2 ) == true )
// 		{
// 			vPath2.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPath2 );
// 			SetPos( vPath2 );
// 
// 			GetProp_Path()->ResetAgent();
// 		}
// 	}
// 	m_UserServerMove.SetLastSyncPos( (int)m_vPos.x, (int)m_vPos.y );
}

void CDigimonUser::Update( float fDeltaTime, bool bAnimation )
{
	bool bBackupAni = CsC_AniProp::g_bUpdateActor;
	CsC_AniProp::g_bUpdateActor = true;

	if( m_SkillRevTimeSeq.IsEnable() == true )
	{
		RecvSkill( false, 0 );
	}

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
	{
		_Update_Battle( fDeltaTime );
	}

#ifdef KEYBOARD_MOVE
	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_NONE 
		|| g_pCharMng->GetTamerUser()->GetProp_Animation()->GetAnimationID() == ANI::DIGIVICE_CHAGE
		|| g_pCharMng->GetTamerUser()->GetProp_Animation()->GetAnimationID() == ANI::DIGIMON_EVOLUTION
		|| g_pCharMng->GetTamerUser()->GetProp_Animation()->GetAnimationID() == ANI::DIGIMON_EVOLUTION_DOWN
		|| g_pCharMng->GetTamerUser()->GetProp_Animation()->GetAnimationID() == ANI::DIGIMON_EVOLUTION_END )
	{
		if( GetProp_Path()->IsMove() )
			GetProp_Path()->DeletePath();
	}

	if( GetProp_Path()->IsMove() == false && GetProp_Animation()->GetAnimationID() == ANI::MOVE_RUN
		&& IsRide() == false )
		GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
#endif

	// 디지몬 유저 스테이트 적용 가능하면 적용
	if( _IsApplyDUState( m_eDigimonUserState ) &&( IsRide() == false ) )
	{
		switch( m_eDigimonUserState )
		{
		case DU_MANUAL_CONTROL:
			{

			}
			break;
		case DU_WAIT:
			{
				assert_cs( GetProp_Attack()->GetFollowTarget() == NULL );
				_UpdateTamerDist();
			}
			break;		
		case DU_FOLLOW:
			{
				assert_cs( GetProp_Attack()->GetFollowTarget() == NULL );
				_UpdateMove();
			}
			break;
		case DU_SKILL:
		case DU_ATTACK:
			{
				switch( nsCsGBTerrain::g_nSvrLibType )
				{
				case nLIB::SVR_DUNGEON:
				case nLIB::SVR_GAME:
					{
						if( net::game )
							_UpdateAttack_Game( fDeltaTime );
					}
					break;
				case nLIB::SVR_BATTLE:
					if( net::game )
						_UpdateAttack_Battle( fDeltaTime );
					break;
				default:
					assert_cs( false );
				}				
			}
			break;
		case DU_WAIT_REV_SKILL:
			{
			}
			break;
		case DU_NPCFIND:
			{
				_UpdateNpcFind();
			}
			break;
#ifdef KEYBOARD_MOVE
		case DU_KEYBOARD_MOVE:
			{
				if( g_pServerMoveOwner->GetCurState() == g_pServerMoveOwner->GetBackupState() )
				{
					_UpdatePortal(fDeltaTime);

					DWORD nKeyCheck = m_UserServerMove.GetKeyCheck();
					if( nKeyCheck & CUserServerMove::KEY_LEFT ||
						nKeyCheck & CUserServerMove::KEY_RIGHT ||
						nKeyCheck & CUserServerMove::KEY_UP ||
						nKeyCheck & CUserServerMove::KEY_DOWN )
					{
						if( g_pCharMng->GetTamerUser()->IsFollowPortalMove() == false
							&& cMessageBox::IsMessageBox() )
							g_pCharMng->GetTamerUser()->SetFollowPortalMove( true );

						_StopAttack( false );
						GetProp_Attack()->ReleaseFollowTarget();
						if(	GetProp_Animation()->GetAnimationID() != ANI::MOVE_RUN )
							GetProp_Animation()->SetAnimation( ANI::MOVE_RUN );
					}
					else
					{
						m_eDigimonUserState = DU_WAIT;
						GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
					}
				}
				else if( GetProp_Animation()->GetAnimationID() == ANI::MOVE_RUN )
				{
					GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
				}
			}
			break;
#endif
		default:
			assert_cs( false );
		}
	}	

	// HP, DS 체크
	if( ( m_bWarningHP == false )&&( GetBaseStat()->GetHPRate() < 0.3f ) )
	{
		cDigimonTalk::Print( 13000 );
		m_bWarningHP = true;
	}
	if( GetBaseStat()->GetHPRate() > 0.3f )
	{
		m_bWarningHP = false;
	}
	if( ( m_bWarningDS == false )&&( GetBaseStat()->GetDSRate() < 0.1f ) )
	{
		cDigimonTalk::Print( 13005 );
		m_bWarningDS = true;
	}
	if( GetBaseStat()->GetDSRate() > 0.1f )
	{
		m_bWarningDS = false;
	}

	if( IsRide() == false )
	{
		g_pHpBar->SetBuddy( cHpBar::HPDS, GetUniqID() );
	}	

	PARENT_CLASS::Update( fDeltaTime, bAnimation );

	// 서버 이동
	if( IsPause( PAUSE_PATH ) == false )
		m_UserServerMove.SetServerMove( GetUniqID(), GetPos().x, GetPos().y, GetCurRot(), GetProp_Path()->GetPath()->m_iPath );
	m_UserServerMove.Update_SendMove_Server();

	_Update_DigimonTalk();
	//_Update_Region();	

	if( GetDigimonState() == CDigimon::DIGIMON_DIE )
	{
		if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_GAME || nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_DUNGEON )
		{
			if( ( cMessageBox::IsMessageBox( cPrintMsg::GAME_DEAD ) == false )&&( net::game->IsSendBlock() == false ) )
			{
				cPrintMsg::PrintMsg(cPrintMsg::GAME_DEAD);
			}
		}		
	}

	CsC_AniProp::g_bUpdateActor = bBackupAni;
}

void CDigimonUser::_Update_Battle( float fDeltaTime )
{
	if( m_BattleEPDecrTimeSeq.IsEnable() == false )
	{
		return;
	}

	float fEP = CsMax( 0, GetBaseStat()->GetBattleEP() - GetFTBattleEvoCurElement()->s_nDecEP*fDeltaTime );
	GetBaseStat()->SetBattleEP( fEP );
}

void CDigimonUser::_UpdateAttack_Game( float fDeltaTime )
{
	m_IdleCheckTimeSeq.OffReset();
	// 전투중 인가
	CsC_AvObject* pTarget = GetProp_Attack()->GetFollowTarget();
	if( NULL == pTarget )// 공격하려고 하는데 타겟이 없는 경우
	{
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon )
			SetDigimonUserState( DU_WAIT );
		else
			SetDigimonUserState( DU_FOLLOW );
#else
		SetDigimonUserState( DU_FOLLOW );
#endif
		return;
	}

	CsC_AvObject* pRotationTarget = GetProp_Attack()->GetHitTarget();
	if( pRotationTarget == NULL )
		pRotationTarget = pTarget;

	CsC_AttackProp::eFOLLOW_ITERATOR eFollowIterator;
#ifdef KEYBOARD_MOVE
	if( GetDigimonUserState() == DU_KEYBOARD_MOVE )
	{
		GetMoveCoolTimeSeq()->Reset();
		SetDigimonUserState( DU_FOLLOW );
		GetMoveCoolTimeSeq()->Reset();
	}
#endif
	switch( GetProp_Attack()->UpdateAttack( fDeltaTime, eFollowIterator ) )
	{	
		// 공격 시작
	case CsC_AttackProp::SUCCESS_ATTACK_START:		
		{
			SetRotation_Ani( pRotationTarget );			

			switch( eFollowIterator )
			{
			case CsC_AttackProp::FI_ATTACK:				
				if(GetUniqID() !=  pTarget->GetUniqID())
					net::game->SendAttack( GetUniqID(), pTarget->GetUniqID() );				
				break;
			case CsC_AttackProp::FI_SKILL1:
				SendSkill( 0, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL2:
				SendSkill( 1, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL3:
				SendSkill( 2, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL4:
				SendSkill( 3, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL5:
				SendSkill( 4, pTarget );
				break;
			}
		}
		break;

		// 공격중
	case CsC_AttackProp::SUCCESS_ATTACK_ING:
		SetRotation_Ani( pRotationTarget );
		break;

		// 나 이동중이구나~ 대기
	case CsC_AttackProp::SUCCESS_MOVING:	
		{
			// 몬스터 타겟을 잡아놓고 공격 명령 시 Actor Object 가 사이에 있을 때 벽을 그냥 통과하는 현상 발생
			// 이동 포지션상에 Actor Object가 있으면 어택 해제되도록 수정. 
			NiPoint3 vPos = m_vPos;
// 			vPos.x += m_vDir.x * 25;
// 			vPos.y += m_vDir.y * 25;

			if( m_pABVWire == NULL )	// 탈 것이 있는 라이딩 시 m_pAVWire가 null이 되어 Client 종료됨. null 체크 삽입
				break;

			nsCSGBFUNC::sTERRAIN_BOUND s_DigimonBound;
			float fTemp = m_pABVWire->GetWorldBound().GetRadius();
			s_DigimonBound.s_ptMin = NiPoint2( vPos.x - m_pABVWire->GetWorldBound().GetRadius(), vPos.y - m_pABVWire->GetWorldBound().GetRadius() );
			s_DigimonBound.s_ptMax = NiPoint2( vPos.x + m_pABVWire->GetWorldBound().GetRadius(), vPos.y + m_pABVWire->GetWorldBound().GetRadius() );

			LIST_GBTERRAIN_OBJECT* listActorObj = nsCsGBTerrain::g_pCurRoot->GetList_ActorObj();
			if( listActorObj->empty() == false )
			{
				LIST_GBTERRAIN_OBJECT_IT it = listActorObj->begin();
				LIST_GBTERRAIN_OBJECT_IT itEnd = listActorObj->end();
				CsGBObject* pObject = NULL;
				for( ; it!=itEnd; ++it )
				{		
					//if( (*it)->GetInfo()->s_Bound.InBoundCheck( vPos.x , vPos.y ) )
					if( (*it)->GetInfo()->s_Bound.InBoundCheck( s_DigimonBound.s_ptMin.x, s_DigimonBound.s_ptMin.y ) || 
						(*it)->GetInfo()->s_Bound.InBoundCheck( s_DigimonBound.s_ptMax.x, s_DigimonBound.s_ptMax.y ) )
					{
						// 파트너몬 어택 정보 해제
						CsC_AttackProp* pPropAttack = GetProp_Attack();	
						pPropAttack->SetHitTarget( NULL );
						pPropAttack->ReleaseFollowTarget();	
						SetDigimonUserState( DU_FOLLOW );

						// 테이머 어택 정보 해제
						g_pCharMng->GetTamerUser()->DeletePath();
						g_pCharMng->GetTamerUser()->ReleaseArriveTarget();

						// 타겟 해제
						if( g_pCharResMng )
							g_pCharResMng->ReleaseTargetMark( true );

						cPrintMsg::PrintMsg( 11101 );

					}
				}
			}
		}

		break;

		// 떄릴라는데 장애물이 등장
	case CsC_AttackProp::FAILE_BLOCKING:
		cPrintMsg::PrintMsg( cPrintMsg::TARGET_EYE_OUT );
		SetDigimonUserState( DU_FOLLOW );
		break;

		// 위치가 틀리다 - 다시 움직이자
	case CsC_AttackProp::FAILE_POS:		
		DoAttackTargetPos();
		break;

		// 계속 패킷 보내기 미안해서... 잘못됬지만 홀드
	case CsC_AttackProp::FAILE_HOLDBACK:
		SetRotation_Ani( pRotationTarget );		
		break;

		// 머야 이상한값 - 한번더 확인하기 위해서 재호출 하자 ( 디버깅 할때를 위해 )
	case CsC_AttackProp::FAILE_ASSERT:
		assert_cs( false );
		GetProp_Attack()->GetFollowTarget();
		break;

	default:
		assert_cs( false );
	}

	// 테이머와의 거리 체크
	_UpdateTamerDist();
}


void CDigimonUser::_UpdateAttack_Battle( float fDeltaTime )
{
	m_IdleCheckTimeSeq.OffReset();
	// 전투중 인가
	CsC_AvObject* pTarget = GetProp_Attack()->GetFollowTarget();
	if( pTarget == NULL )
	{
		SetDigimonUserState( DU_MANUAL_CONTROL );
		return;
	}

	CsC_AvObject* pRotationTarget = GetProp_Attack()->GetHitTarget();
	if( pRotationTarget == NULL )
		pRotationTarget = pTarget;

	CsC_AttackProp::eFOLLOW_ITERATOR eFollowIterator;
	switch( GetProp_Attack()->UpdateAttack( fDeltaTime, eFollowIterator ) )
	{	
		// 공격 시작
	case CsC_AttackProp::SUCCESS_ATTACK_START:		
		{
			SetRotation_Ani( pRotationTarget );
			switch( eFollowIterator )
			{
			case CsC_AttackProp::FI_ATTACK:				
				net::game->SendAttack( GetUniqID(), pTarget->GetUniqID() );
				break;
			case CsC_AttackProp::FI_SKILL1:
				SendSkill( 0, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL2:
				SendSkill( 1, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL3:
				SendSkill( 2, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL4:
				SendSkill( 3, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL5:
				SendSkill( 4, pTarget );
				break;
			}
		}
		break;

		// 공격중
	case CsC_AttackProp::SUCCESS_ATTACK_ING:
		SetDigimonUserState( DU_MANUAL_CONTROL );
		break;

		// 나 이동중이구나~ 대기
	case CsC_AttackProp::SUCCESS_MOVING:			
		break;

		// 떄릴라는데 장애물이 등장
	case CsC_AttackProp::FAILE_BLOCKING:
		cPrintMsg::PrintMsg( cPrintMsg::TARGET_EYE_OUT );
		SetDigimonUserState( DU_MANUAL_CONTROL );
		break;

		// 위치가 틀리다 - 다시 움직이자
	case CsC_AttackProp::FAILE_POS:		
		DoAttackTargetPos();
		break;

		// 계속 패킷 보내기 미안해서... 잘못됬지만 홀드
	case CsC_AttackProp::FAILE_HOLDBACK:
		SetRotation_Ani( pRotationTarget );		
		break;

		// 머야 이상한값 - 한번더 확인하기 위해서 재호출 하자 ( 디버깅 할때를 위해 )
	case CsC_AttackProp::FAILE_ASSERT:
		assert_cs( false );
		GetProp_Attack()->GetFollowTarget();
		break;

	default:
		assert_cs( false );
	}
}


void CDigimonUser::_UpdateAttack_ClientMode( float fDeltaTime )
{
	m_IdleCheckTimeSeq.OffReset();
	// 전투중 인가
	CsC_AvObject* pTarget = GetProp_Attack()->GetFollowTarget();
	assert_cs( pTarget != NULL );

	CsC_AvObject* pRotationTarget = GetProp_Attack()->GetHitTarget();
	if( pRotationTarget == NULL )
		pRotationTarget = pTarget;

	CsC_AttackProp::eFOLLOW_ITERATOR eFollowIterator;
	switch( GetProp_Attack()->UpdateAttack( fDeltaTime, eFollowIterator ) )
	{	
		// 공격 시작
	case CsC_AttackProp::SUCCESS_ATTACK_START:
		{
			SetRotation_Ani( pRotationTarget );

			sRECV_HITTER_INFO hi;
			hi.s_nUID = GetUniqID();			
			hi.s_HitType = CsC_AttackProp::HT_Skill;

			sRECV_TARGET_INFO ti;
			ti.s_nUID = GetProp_Attack()->GetFollowTarget()->GetUniqID();
			ti.s_DamageType = CsC_AttackProp::DT_Normal;
			ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;

			switch( eFollowIterator )
			{
			case CsC_AttackProp::FI_SKILL1:		hi.s_nSkillIndex = 0;			break;
			case CsC_AttackProp::FI_SKILL2:		hi.s_nSkillIndex = 1;			break;
			case CsC_AttackProp::FI_SKILL3:		hi.s_nSkillIndex = 2;			break;
			case CsC_AttackProp::FI_SKILL4:		hi.s_nSkillIndex = 3;			break;
			case CsC_AttackProp::FI_SKILL5:		hi.s_nSkillIndex = 4;			break;
			default:															return;
			}

			m_fClientAttackTime = 8000.0f;

			if( pTarget->GetClientPlag() & CP_DMG_SKILL )
			{
				ti.s_nValue[ 0 ] = -150;
				pTarget->GetBaseStat()->SetDeltaHP( -150 );
			}
			else
			{
				ti.s_nValue[ 0 ] = 0;
				pTarget->GetBaseStat()->SetDeltaHP( 0 );
			}

			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::USE_DIGIMON_SKILL, 0, &eFollowIterator );
			cCliGame::SetSkillInfo( &hi, &ti, false );

			if( pTarget->GetBaseStat()->GetHP() == 0 )
				cCliGame::SetDieInfo( GetProp_Attack()->GetFollowTarget() );
		}		
		break;

		// 공격중
	case CsC_AttackProp::SUCCESS_ATTACK_ING:
		{			
			SetRotation_Ani( pRotationTarget );

			if( eFollowIterator == CsC_AttackProp::FI_ATTACK )
			{
				m_fClientAttackTime += g_fDeltaTime;				

				if( ( m_fClientAttackTime > GetAttackSpeed() )&&( pTarget->GetBaseStat()->GetHP() > 0 ) )
				{
					sRECV_HITTER_INFO hitter;
					hitter.s_nUID = GetUniqID();	
					hitter.s_HitType = CsC_AttackProp::HT_Normal;					

					sRECV_TARGET_INFO target;
					target.s_nUID = pTarget->GetUniqID();
					target.s_DamageType = CsC_AttackProp::DT_Normal;	
					target.s_nNumEffectType = NUMTYPE::ET_NORMAL;

					if( pTarget->GetClientPlag() & CP_DMG_NORMAL )
					{
						target.s_nValue[ 0 ] = -60;
						pTarget->GetBaseStat()->SetDeltaHP( -60 );
					}
					else
					{
						target.s_nValue[ 0 ] = 0;
						pTarget->GetBaseStat()->SetDeltaHP( 0 );
					}

					cCliGame::SetAttackInfo( &hitter, &target, false );

					if( pTarget->GetBaseStat()->GetHP() == 0 )
						cCliGame::SetDieInfo( GetProp_Attack()->GetFollowTarget() );

					m_fClientAttackTime = 0.0f;
				}
			}
		}
		break;

		// 나 이동중이구나~ 대기
	case CsC_AttackProp::SUCCESS_MOVING:			
		break;

		// 떄릴라는데 장애물이 등장
	case CsC_AttackProp::FAILE_BLOCKING:
		cPrintMsg::PrintMsg( cPrintMsg::TARGET_EYE_OUT );
		SetDigimonUserState( DU_FOLLOW );
		break;

		// 위치가 틀리다 - 다시 움직이자
	case CsC_AttackProp::FAILE_POS:		
		DoAttackTargetPos();
		break;

		// 계속 패킷 보내기 미안해서... 잘못됬지만 홀드
	case CsC_AttackProp::FAILE_HOLDBACK:
		SetRotation_Ani( pRotationTarget );		
		break;

		// 머야 이상한값 - 한번더 확인하기 위해서 재호출 하자 ( 디버깅 할때를 위해 )
	case CsC_AttackProp::FAILE_ASSERT:
		assert_cs( false );
		GetProp_Attack()->GetFollowTarget();
		break;

	default:
		assert_cs( false );
	}

	// 테이머와의 거리 체크
	_UpdateTamerDist();
}

void CDigimonUser::_UpdateMove()
{
	if( m_MoveCoolTimeSeq.IsCoolTime() == true )
		return;

	// 디지몬이 앞서가는거 방지
	_MoveStop_ByArriveDist();

	// 디지몬이 이동해야 하는지 체크해서 이동
	if( ( _IsDoMove() == true )||( m_MoveOnceTimeSeq.IsEnable() ) )
	{
		// 테이머 도착점으로 보낸다.
		DoPos_ByTamer();
	}	
}

void CDigimonUser::_UpdateNpcFind()
{
	// 전투중이라면 다시 테이머에게
	if( IsBattle() == true )
	{
		cPrintMsg::PrintMsg( 30027 );
		SetDigimonUserState( DU_FOLLOW );
		return;
	}

	assert_cs( m_pFindTarget != NULL );
	m_IdleCheckTimeSeq.OffReset();

	switch( m_eFindState )
	{
	case NFS_DOING:
		{
			if( CalTamerDist() > 2000.0f )
			{
				m_eFindState = NFS_WAIT;
				m_FindCheckTimeSeq.Reset();
			}

			if( GetProp_Path()->IsMove() == false )
			{
				m_eFindState = NFS_COMPLEATE;
				m_FindCheckTimeSeq.Reset();
			}
		}
		break;
	case NFS_WAIT:
		{
			if( CalTamerDist() < 1000.0f )
			{
				DoPos_ByNpcFind( m_pFindTarget->GetFTID() );				
			}
			else
			{
				DeletePath();
				SetRotation_Ani( g_pCharMng->GetTamerUser() );
			}
		}
		break;
	case NFS_COMPLEATE:
		{
			SetRotation_Ani( g_pCharMng->GetTamerUser() );
			if( CalTamerDist() < 500.0f )
			{
#ifdef KEYBOARD_MOVE
				if( g_pResist->m_Global.s_bMoveDigimon )
					SetDigimonUserState( DU_WAIT );
				else
					SetDigimonUserState( DU_FOLLOW );
#else
				SetDigimonUserState( DU_FOLLOW );
#endif
				m_pFindTarget = NULL;
			}
		}		
		break;
	default:
		assert_cs( false );
	}

	if( m_FindCheckTimeSeq.IsEnable() == true )
	{		
		cDigimonTalk::sTalkEle sEle;
		sEle.s_dwNpc = m_pFindTarget->GetFTID();
		switch( m_eFindState )
		{
		case NFS_DOING:			cDigimonTalk::Print( 50012, &sEle );		break;
		case NFS_WAIT:			cDigimonTalk::Print( 50013, &sEle );		break;
		case NFS_COMPLEATE:		cDigimonTalk::Print( 50014, &sEle );		break;	
		default:				assert_cs( false );
		}
	}	

	_UpdateTamerDist();
}

void CDigimonUser::_UpdateTamerDist()
{
	if( CheckTamerDist() == false )
	{
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon )
			g_pCharMng->GetTamerUser()->SetTamerUserState( CTamerUser::TU_FOLLOW_DIGIMON );
		else
#endif
		SetDigimonUserState( DU_FOLLOW );
	}
}

void CDigimonUser::_Update_DigimonTalk()
{
	// 디지몬 대화
	if( m_IdleCheckTimeSeq.IsEnable() == true )
	{
		m_IdleCheckTimeSeq.OffReset();
		switch( rand()%3 )
		{
		case 0:			cDigimonTalk::Print( 50009 );		break;
		case 1:			cDigimonTalk::Print( 50010 );		break;
		case 2:			cDigimonTalk::Print( 50011 );		break;
		default:
			assert_cs( false );
		}
	}
}
#ifdef KEYBOARD_MOVE
void CDigimonUser::_UpdatePortal(const float& fDeltaTime)
{
	if(isEnablePortal() == false)
		return;

	DWORD dwNearPortalID = GetNearPortalID(fDeltaTime);
	if( m_dwLastNearPortalID != 0 ){
		if( dwNearPortalID == 0 ){
			m_dwLastNearPortalID = 0;
			g_pCharMng->GetTamerUser()->SetFollowPortalMove( false );
			return;	
		}
	}
	if(m_dwLastNearPortalID == 0)
	{
		if(m_dwLastNearPortalID !=dwNearPortalID)
		{
			m_dwLastNearPortalID = dwNearPortalID;
			g_pCharMng->GetTamerUser()->SetFollowPortalMove( true );
		}
	}
}
#endif	// KEYBOARD_MOVE
bool				CDigimonUser::isEnablePortal(void)
{
	if( GetProp_Path()->IsMove() == false )
		return false;

	if( g_pDataMng->IsSort() == true )
		return false;

	// 메시지박스가 띄워진 상태면 리턴.
	if(1 <= cMessageBox::GetCount())
		return false;

	if( IsLoad() == false )
		return false;	

	return true;

}
const DWORD  CDigimonUser::GetNearPortalID(const float& fDeltaTime) const
{
	NiBoxBV bound;
	NiCollisionData* pkCD = NiGetCollisionData( m_Node.m_pNiNode );
	NiBoundingVolume* pBoundVolume = pkCD->GetModelSpaceABV();

	bound.UpdateWorldData( *pBoundVolume, m_Node.m_pNiNode->GetWorldTransform() );	
	return nsCsGBTerrain::g_pCurRoot->CheckPortal( fDeltaTime, m_vPos, &bound );
}

void CDigimonUser::Render()
{
	PARENT_CLASS::Render();	
}

//====================================================================================
//
//		패쓰
//
//====================================================================================
void CDigimonUser::DeletePath()
{
	bool bOldMove = GetProp_Path()->IsMove();

	PARENT_CLASS::DeletePath();

	if( ( bOldMove == true )&&( m_eDigimonState != DIGIMON_DIE ) )
		m_UserServerMove.DeletePath();

	if( IsRide()
#ifdef KEYBOARD_MOVE
		&& g_pResist->m_Global.s_bMoveDigimon 
#endif
		)
		g_pCharMng->GetTamerUser()->DeletePath();
}

void CDigimonUser::ArriveTarget()
{
	PARENT_CLASS::ArriveTarget();

	if( IsRide() )
		g_pCharMng->GetTamerUser()->ArriveTarget();
#ifdef KEYBOARD_MOVE
	if( g_pResist->m_Global.s_bMove_KeyBoard )
	{
		if( g_pServerMoveOwner )
		{
			DWORD nKeyCheck = g_pServerMoveOwner->GetKeyCheck();
			if( !( nKeyCheck & CUserServerMove::KEY_LEFT 
				|| nKeyCheck & CUserServerMove::KEY_RIGHT
				|| nKeyCheck & CUserServerMove::KEY_UP
				|| nKeyCheck & CUserServerMove::KEY_DOWN ) )
			{
				if( m_eDigimonUserState == DU_KEYBOARD_MOVE )
				{
					m_eDigimonUserState = DU_WAIT;
					m_eDigimonUserState_NoneSkill = DU_WAIT;
					if( IsRide() == false )
						GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
				}
			}
		}
	}
#endif
}

void CDigimonUser::SetTargetPos( NiPoint3 vTargetPos, float fConnectLength, bool bRenderMoveModel )
{
	NiPoint2 vResultPos;
	if( GetProp_Path()->CalTargetPath( vResultPos, vTargetPos, fConnectLength ) == true )
	{
		if( GetProp_Path()->SetPath( vResultPos ) == true )
		{
			_StopAttack( false );
			GetProp_Attack()->ReleaseFollowTarget();
			GetProp_Attack()->ReleaseAttackingTarget();

			//=======================================================================
			//		무브 포인트
			//=======================================================================
			if( bRenderMoveModel == true )
				g_pCharResMng->SetMovePoint( NiPoint3( vResultPos.x, vResultPos.y, nsCsGBTerrain::g_pCurRoot->GetHeight( vResultPos ) ) );

			//=======================================================================
			//	서버 이동
			//=======================================================================			
			m_UserServerMove.SetServerMoveReady();
		}
	}
}

//====================================================================================
//
//		스킬
//
//====================================================================================
void CDigimonUser::UseSkill( int nSkillIndex )
{
	if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::USE_DIGIMON_SKILL ) == false )
	{
		if( g_pDataMng->GetServerSync()->GetSyncData( cData_ServerSync::USE_DIGIMON_SKILL )->s_nEtcValue1 != nSkillIndex )
		{
			cPrintMsg::PrintMsg( 30015 );
		}
		return;	
	}		

	m_SkillMng.__UseSkill( nSkillIndex );
}

bool CDigimonUser::SendSkill( int nSkillIndex, CsC_AvObject* pTarget )
{
	assert_csm( m_eDigimonUserState != DU_WAIT_REV_SKILL, _T( "송명수를 바로 찾아주세요~!! 이게 나온 당신은 이미 영웅~!!\n끄지 말고 꼭 송명수를 찾아주세요" ) );
	assert_csm( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::USE_DIGIMON_SKILL ) == true, _T( "송명수를 바로 찾아주세요~!! 이게 나온 당신은 이미 영웅~!!\n끄지 말고 꼭 송명수를 찾아주세요" ) );
	assert_csm( m_SkillMng.GetSkill( nSkillIndex )->s_CoolTime.IsCoolTime() == false, _T( "송명수를 바로 찾아주세요~!! 이게 나온 당신은 이미 영웅~!!\n끄지 말고 꼭 송명수를 찾아주세요" ) );

	if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
	{
		return false;
	}

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::USE_DIGIMON_SKILL, 0, &nSkillIndex );
	cSkill::sINFO* pInfo = m_SkillMng.GetSkill( nSkillIndex );
	if( pInfo->s_pFTSkill->GetInfo()->s_nMemorySkill )	// 메모리 스킬이라면...
		net::game->SendUseDigimonMemorySkill( GetUniqID(), (u1)GetFTEvolCurObj()->m_nEvoSlot, pInfo->s_pFTSkill->GetInfo()->s_dwID, pTarget->GetUniqID() );
	else
		net::game->SendSkill( nSkillIndex, GetUniqID(), pTarget->GetUniqID() );

	SetDigimonUserState( DU_WAIT_REV_SKILL );
	SetPause( PAUSE_PATH, true );
	DeletePath();
	
	return true;
}

bool CDigimonUser::RecvSkill( bool bSuccess, int nSkillIndex )
{
	if( bSuccess == true )
	{
		CsSkill* pFTSkill = m_SkillMng.GetFTSkill( nSkillIndex );
		if( pFTSkill == NULL || pFTSkill->GetInfo() == NULL || GetBaseStat() == NULL || GetSkillMng() == NULL || GetSkillMng()->GetSkill( nSkillIndex ) == NULL )
		{
			SetPauseAll( false );
			return false;
		}
		CsSkill::sINFO* pFTSkillInfo = pFTSkill->GetInfo();
		// HP감소
		GetBaseStat()->SetDeltaHP( -pFTSkillInfo->s_nUseHP );
		// DS감소
		// HP/DS 회복스킬(버프스킬 발동 체크)
		if( GetBuffSkillFlag() )// DS 회복스킬 발동했다면 해당 스킬 소모 DS 감소 안함
		{
			bool bDecrease = true;

			for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
			{
				if( pFTSkillInfo->s_Apply[ i ].s_nA == APPLY_DS )
				{
					bDecrease = false;	//감소 안시킬거야
					SetBuffSkillFlag( false );	//Flag는 다시 false로, DS회복스킬 발동 시 true
					break;	//다른건 검사할 필요 없겠지
				}
			}

			if( bDecrease )
				GetBaseStat()->SetDeltaDS( -pFTSkillInfo->s_nUseDS );
		}
		else
			GetBaseStat()->SetDeltaDS( -pFTSkillInfo->s_nUseDS );
		// 타겟 없애주고
		GetProp_Attack()->SetHitTarget( NULL );
		// 움직임 가능하게
		SetPauseAll( false );

		// 스킬 쿨타임 셋팅
		GetSkillMng()->GetSkill( nSkillIndex )->s_CoolTime.Start();

		// 메세지 출력
		cPrintMsg::PrintMsg( 30004, GetName(), pFTSkillInfo->s_szName );	

		cDigimonTalk::sTalkEle sEle;
		sEle.s_dwSkill = pFTSkillInfo->s_dwID;
		cDigimonTalk::Print( 30004, &sEle );

	}
	else if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::USE_DIGIMON_SKILL ) == false )
	{
		cPrintMsg::PrintMsg( 30015 );

		// 움직임 가능하게
		SetPauseAll( false );

		// 스킬실패 하고 나서
		//SetDigimonUserState( m_eDigimonUserState_NoneSkill, GetProp_Attack()->GetFollowTarget() );		
	}

	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::USE_DIGIMON_SKILL, 0 );
	ReleaseSrvSendSkill();
	return true;
}

//====================================================================================
//
//		CallBack
//
//====================================================================================

void CDigimonUser::CallBack_StartSequence( DWORD dwAniID )
{
	switch( dwAniID )
	{
	case ANI::IDLE_CHAR:
	case ANI::IDLE_SHAKE:
		PlayAniSound( dwAniID );
		return;
	}

	PARENT_CLASS::CallBack_StartSequence( dwAniID );
}

bool CDigimonUser::CallBack_EndofSequence( DWORD dwAniID )
{
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		switch( dwAniID )
		{
		case ANI::ATTACK_SKILL1:
		case ANI::ATTACK_SKILL2:
		case ANI::ATTACK_SKILL3:
		case ANI::ATTACK_SKILL4:
		case ANI::ATTACK_SKILL5:
			// 스킬쓰고나서
			SetDigimonUserState( m_eDigimonUserState_NoneSkill, GetProp_Attack()->GetFollowTarget() );
			break;
		}
		break;
	case nLIB::SVR_BATTLE:
		switch( dwAniID )
		{
		case ANI::ATTACK_ATTACK1:
		case ANI::ATTACK_ATTACK2:		
			g_pServerMoveOwner->RetryKeyCheck();	
			break;
		case ANI::ATTACK_SKILL1:
		case ANI::ATTACK_SKILL2:
		case ANI::ATTACK_SKILL3:
		case ANI::ATTACK_SKILL4:
		case ANI::ATTACK_SKILL5:
			g_pServerMoveOwner->RetryKeyCheck();
			SetDigimonUserState( DU_MANUAL_CONTROL, NULL );
			break;
		}
		break;
	default:
		assert_cs( false );
	}	

	return PARENT_CLASS::CallBack_EndofSequence( dwAniID );
}


//====================================================================================
//
//		Property
//
//====================================================================================

void CDigimonUser::_SetDefault_IdleAnimation_Move()
{
	m_IdleCheckTimeSeq.OffReset();

	// 데미지 애니메이션 중일떄는 움직이는 애니메이션 잠시 패스
	assert_cs( GetProp_Animation() != NULL );
	if( GetProp_Animation()->IsDamage() == true )
		return;

	assert_cs( GetProp_Attack() != NULL );
	if( GetProp_Attack()->GetFollowTarget() != NULL )
	{
		GetProp_Animation()->SetAnimation( ANI::MOVE_ATTACKRUN );
		return;
	}
	PARENT_CLASS::_SetDefault_IdleAnimation_Move();
}


//====================================================================================
//
//		상태
//
//====================================================================================

void CDigimonUser::SetBattle()
{
	PARENT_CLASS::SetBattle();

	g_pGameIF->GetMainDigimonWindow()->SetBattle();
	// 채널이동 메세지 삭제
	cMessageBox::DelMsg( 10017, false );

	// 배틀이 시작되면 씰 마스터 UI 닫기 처리
	int mode = 0;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SEALMASTER_UI_OPEN_CLOSE, &mode );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIN_BAR, 0 ) )
	{
		// Seal Master UI Lock, disabled
		//g_pGameIF->GetMainBar()->SetButtonEnable(cMainBar::MAIN_BT_SEAL, false);
	}
}

void CDigimonUser::SetBattleEnd()
{
	PARENT_CLASS::SetBattleEnd();

	CsC_AttackProp* pPropAttack = GetProp_Attack();
	CsC_AvObject* pTarget = pPropAttack->GetFollowTarget();

	pPropAttack->ReleaseFollowTarget();

	// 전투가 끝났으면 다시 나를 쫒아 오게 만든다
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		{
			if( IsServerDie() == false )
			{
				SetDigimonUserState( m_eDigimonUserState_NoneSkill, pTarget );
			}
		}
		break;
	case nLIB::SVR_BATTLE:
		break;
	default:
		assert_cs( false );
	}

	g_pGameIF->GetMainDigimonWindow()->SetBattleEnd();
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIN_BAR, 0 ) )
	{
		// 씰 마스터 UI 정상화
		//g_pGameIF->GetMainBar()->SetButtonEnable(cMainBar::MAIN_BT_SEAL, true);
	}
}

void CDigimonUser::SetDie()
{
	if ( cMessageBox::IsMessageBox( cPrintMsg::GAME_DEAD ) )	//2017-07-10-nova 죽었을때 2번처리되서 나오는 문제때문에 메시지창이 있을경우 무시
	{
		return;
	}

	PARENT_CLASS::SetDie();

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = _T("SetDie");
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	// 상점 관련 인터페이스를 닫아준다
	//( 단. 서버에 이미 요청한 창들은 닫히지 않는다. ) - 서버에서 실패 받으면 닫고, 성공 받으면 성공 처리 후에 닫는다.
	// 더이상  추가로 패킷을 보내지 않기 위해 닫는용도.
	g_pGameIF->UserDie();
	cDigimonTalk::Print( 12001 );

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_GAME || nsCsGBTerrain::g_nSvrLibType ==  nLIB::SVR_DUNGEON )
		cPrintMsg::PrintMsg(cPrintMsg::GAME_DEAD);
}

void CDigimonUser::SetServerDie( bool bDie )
{
	PARENT_CLASS::SetServerDie( bDie );

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_GAME || nsCsGBTerrain::g_nSvrLibType ==  nLIB::SVR_DUNGEON )
		net::game->SetSendBlock( true );
}

void CDigimonUser::SetDigimonUserState( eDIGIMONUSER_STATE duState, void* pValue /*=NULL*/ )
{
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:		_SetDigimonUserState_Game( duState, pValue );			 break;
	case nLIB::SVR_BATTLE:		_SetDigimonUserState_Battle( duState, pValue );			 break;
	default:					assert_cs( false );
	}	
}

void CDigimonUser::_SetDigimonUserState_Game( eDIGIMONUSER_STATE duState, void* pValue )
{
	if( _IsApplyDUState( duState ) == true )
	{
		switch( duState )
		{
		case DU_WAIT:
			if( m_eDigimonUserState != duState )
			{
				// 공격 중지
				_StopAttack( false );
				GetProp_Attack()->ReleaseFollowTarget();				
				DeletePath();
			}		
			break;
		case DU_FOLLOW:
			{
				_StopAttack( false );
				GetProp_Attack()->ReleaseFollowTarget();				

				//// 공격, 스킬이 남아 있다면 포우즈 걸어주자
				//if( GetProp_Attack()->IsAttack_SkillProcess() == true )
				//	SetPause( PAUSE_PATH, true );

				_SetTargetPos();
			}			
			break;
		case DU_SKILL:
			_StopAttack( true );
			// 모든 처리는 되서 들어온다.
			if(GetProp_Attack()->GetFollowTarget() != NULL)
				DoAttackTargetPos();
			break;
		case DU_ATTACK:
			if( ( pValue != NULL )&&( ((CsC_AvObject*)pValue)->IsServerDie() == false ) )
			{
				if( GetProp_Attack()->GetFollowTarget() != pValue )
				{
					_StopAttack( false );
					_SetAttack( (CsC_AvObject*)pValue );

					// 디지몬 대화 체크
					_CheckAttackDigimonTalk();
				}
				else if( m_eDigimonUserState != DU_ATTACK )
				{
					_SetAttack( (CsC_AvObject*)pValue );
				}
			}
			else
			{
#ifdef KEYBOARD_MOVE
				//전투 종료 후 행동 - 디지몬은 가만히, 테이머가 디지몬에게 오도록
				if( g_pResist->m_Global.s_bMoveDigimon )
				{
					_StopAttack( false );
					GetProp_Attack()->ReleaseFollowTarget();
					SetDigimonUserState( DU_WAIT );
					g_pCharMng->GetTamerUser()->SetTamerUserState( CTamerUser::TU_FOLLOW_DIGIMON );
				}
				else
				{
					SetDigimonUserState( DU_FOLLOW );
				}
#else	// KEYBOARD_MOVE
				SetDigimonUserState( DU_FOLLOW );
#endif	// KEYBOARD_MOVE
				return;
			}
			break;
		case DU_WAIT_REV_SKILL:
			m_SkillRevTimeSeq.Reset();
			break;
		case DU_NPCFIND:
			if( pValue == NULL )
			{
				SetDigimonUserState( DU_FOLLOW );
				return;
			}
			else
			{
				DoPos_ByNpcFind( *(int*)pValue );
			}			
			break;
#ifdef KEYBOARD_MOVE
		case DU_KEYBOARD_MOVE:
			_StopAttack( false );
			GetProp_Attack()->ReleaseFollowTarget();
			SetDigimonUserState( DU_WAIT );
			break;
#endif
		default:
			assert_cs( false );
		}
	}
	else
	{
		switch( duState )
		{
		case DU_WAIT:
		case DU_FOLLOW:			
			GetProp_Attack()->ReleaseFollowTarget();
			break;
		case DU_SKILL:
		case DU_ATTACK:
			if( pValue != NULL )
			{
				// 몬스터가 아닌가
				assert_csm1( ( (CsC_AvObject*)pValue )->GetLeafRTTI() == RTTI_MONSTER, L"SetDigimonUserState : 몬스터가 아님 : RTTI = %d", ( (CsC_AvObject*)pValue )->GetLeafRTTI() );
				GetProp_Attack()->SetFollowTarget( (CsC_AvObject*)pValue, GetAttackRange(), CsC_AttackProp::FI_ATTACK, IsAttackRangeType() );
			}			
			break;
		case DU_WAIT_REV_SKILL:
			m_SkillRevTimeSeq.Reset();
			break;
		case DU_NPCFIND:
			if( pValue == NULL )
			{
				SetDigimonUserState( DU_FOLLOW );
				return;
			}
			else
			{
				DoPos_ByNpcFind( *(int*)pValue );
			}
			break;
#ifdef KEYBOARD_MOVE
		case DU_KEYBOARD_MOVE:
			_StopAttack( false );
			GetProp_Attack()->ReleaseFollowTarget();
			break;
#endif
		default:
			assert_cs( false );
		}
	}	

	if( ( duState != DU_SKILL )&&( duState != DU_WAIT_REV_SKILL ) )
	{
		m_eDigimonUserState_NoneSkill = duState;
	}
	m_eDigimonUserState = duState;
	m_IdleCheckTimeSeq.OffReset();	

	if( m_eDigimonUserState == DU_SKILL )
	{
		m_eDigimonUserState_NoneSkill = DU_ATTACK;
	}
}

void CDigimonUser::_SetDigimonUserState_Battle( eDIGIMONUSER_STATE duState, void* pValue )
{
	if( _IsApplyDUState( duState ) == false )
		return;

	// 들어와선 안된다
	// DU_SKILL

	switch( duState )
	{
	case DU_WAIT:
	case DU_FOLLOW:	
		return;

	case DU_ATTACK:
		if( ( pValue != NULL )&&( ((CsC_AvObject*)pValue)->IsServerDie() == false ) )
		{
			if( GetProp_Attack()->GetFollowTarget() != pValue )
			{
				_StopAttack( false );
				_SetAttack( (CsC_AvObject*)pValue );
			}
			else if( m_eDigimonUserState != DU_ATTACK )
			{
				_SetAttack( (CsC_AvObject*)pValue );
			}
		}
		break;
	case DU_SKILL:
		GetProp_Attack()->ReleaseAttackingTarget();
		DoAttackTargetPos();
		break;

	case DU_WAIT_REV_SKILL:
		m_SkillRevTimeSeq.Reset();
		break;

	case DU_MANUAL_CONTROL:
#ifdef KEYBOARD_MOVE
	case DU_KEYBOARD_MOVE:
#endif
		GetProp_Attack()->ReleaseFollowTarget();
		break;

	default:
		assert_cs( false );
	}

	m_eDigimonUserState = duState;
}

void CDigimonUser::_CheckAttackDigimonTalk()
{
	if( GetProp_Attack()->GetFollowTarget()->GetLeafRTTI() != RTTI_MONSTER )
		return;

	CMonster* pMonster = (CMonster*)GetProp_Attack()->GetFollowTarget();
	assert_cs( pMonster->GetLeafRTTI() == RTTI_MONSTER );

	DWORD dwOwner = GetFTDigimon()->GetInfo()->s_dwDigimonID;
	DWORD dwTarget = pMonster->GetDigimonFT()->GetInfo()->s_dwDigimonID;

	// 디지몬 속성 체크
	CsDigimonMng::eCOMPARE eAttCompare = nsCsFileTable::g_pDigimonMng->AttibuteCompare( dwOwner, dwTarget );

	int i=0;
	if(0 != pMonster->GetDigimonFT()->GetInfo()->s_eBaseNatureTypes[0])
	{
		 i =0;
	}
	else if(0 != pMonster->GetDigimonFT()->GetInfo()->s_eBaseNatureTypes[1])
	{
		i =1;
	}
	else if(0 != pMonster->GetDigimonFT()->GetInfo()->s_eBaseNatureTypes[2])
	{
		i =2;
	}

	switch( nsCsFileTable::g_pDigimonMng->NatureCompare2( dwOwner, dwTarget, i ) )
	{
	case CsDigimonMng::BEST:
	case CsDigimonMng::UP:
		switch( eAttCompare )
		{
		case CsDigimonMng::UP:
			if( cDigimonTalk::Print( 50003 ) == true )
				return;
			break;
		case CsDigimonMng::EQUAL:
			if( cDigimonTalk::Print( 50002 ) == true )
				return;
			break;
		}
		break;
	case CsDigimonMng::EQUAL:
		switch( eAttCompare )
		{
		case CsDigimonMng::UP:
			if( cDigimonTalk::Print( 50002 ) == true )
				return;
			break;
		case CsDigimonMng::DOWN:
			if( cDigimonTalk::Print( 50000 ) == true )
				return;
			break;
		}
		break;
	case CsDigimonMng::DOWN:
	case CsDigimonMng::WORST:
		switch( eAttCompare )
		{
		case CsDigimonMng::EQUAL:
			if( cDigimonTalk::Print( 50000 ) == true )
				return;
			break;
		case CsDigimonMng::DOWN:
			if( cDigimonTalk::Print( 50001 ) == true )
				return;
			break;
		}
		break;
	default:
		assert_cs( false );
	}

	// 레벨차
	int nLevelDelta = pMonster->GetBaseStat()->GetLevel() - GetBaseStat()->GetLevel();
	if( nLevelDelta >= 3 )
	{
		if( cDigimonTalk::Print( 50004 ) == true )
			return;
	}
	else if( nLevelDelta <= -3 )
	{
		if( cDigimonTalk::Print( 50005 ) == true )
			return;
	}

	// 이름
	cDigimonTalk::sTalkEle sEle;
	sEle.s_dwDigimon = pMonster->GetDigimonFT()->GetInfo()->s_dwDigimonID;
	cDigimonTalk::Print( 30005, &sEle );	
}

void CDigimonUser::ReleaseFollowTarget( CsC_AvObject* pDeleteTarget )
{
	if( GetProp_Attack()->GetHitTarget() == pDeleteTarget )
	{
		GetProp_Attack()->SetHitTarget( NULL );
		GetProp_Attack()->ReleaseAttackingTarget();
	}

	if( GetProp_Attack()->GetFollowTarget() == pDeleteTarget )
	{
		GetProp_Attack()->ReleaseFollowTarget();		
		SetDigimonUserState( DU_FOLLOW );
	}
}

bool CDigimonUser::_IsApplyDUState( eDIGIMONUSER_STATE duState )
{
	switch( m_eDigimonState )
	{
	case DIGIMON_DIE:
	case DIGIMON_EVOL:
	case DIGIMON_EVOL_JOINTPROGRESS:
	case DIGIMON_DEGENERATE:	
		return false;	
	}		
	
	if( m_BuffData.GetUnusualCond() != 0 ) // 상태이상일때
	{
		switch( duState )
		{
		case DU_ATTACK:
		case DU_SKILL:
		case DU_WAIT_REV_SKILL:	// 공격류는 상태이상 레벨이 2레벨 이상이면 사용불가
			return ( m_BuffData.IsUnusualCond( cBuffData::UNUSUAL_ATTACK ) == false );			
		case DU_WAIT:
			return true;
		default:
			return false;
		}
	}

	return ( IsServerDie() == false );
}

//=============================================================================================================
//
//	씬
//
//=============================================================================================================

void CDigimonUser::SceneAttackPropEvent( CsC_AttackProp::sEVENT_INFO* pEvent )
{
	switch( pEvent->s_eEventType )
	{
	case CsC_AttackProp::EVENT_QUEST:
		if( g_pDataMng && g_pDataMng->GetQuest() )
			g_pDataMng->GetQuest()->UpdateQuestCondition( pEvent->s_nQuestIDX, pEvent->s_nQuestCondition, pEvent->s_nQuestValue );
// 		g_pDataMng->GetQuest()->GetInfo()->GetExeInfo_ByID( pEvent->s_nQuestIDX )->m_nCondition[ pEvent->s_nQuestCondition ] = pEvent->s_nQuestValue;
// 		g_pDataMng->GetQuest()->CalProcess();
		return;
	}

	PARENT_CLASS::SceneAttackPropEvent( pEvent );
}

void CDigimonUser::_DecreaseChipset(USHORT const& nChipsetType, USHORT const& nChipsetTypeC, int const& nChipsetCount)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	SAFE_POINTER_RET( pDigivice );

	// 디지바이스에 소모 칩셋이 있는지 검사 후 인덱스 리턴/* 인자값: 테이블에 정의된 칩셋 타입, 소모 인덱스*/
	int nRemoveIndex = pDigivice->GetChipsetIndex_TypeLT( nChipsetType, nChipsetTypeC );
	if( cData_Digivice::INVALIDE_DIGIVICE_INDEX == nRemoveIndex )
		return;

	// 인덱스 값을 가지고 아이템 개수 감소
	cItemInfo* pChipset = pDigivice->GetChipset( nRemoveIndex );
	SAFE_POINTER_RET( pChipset );
	if( !pChipset->IsEnable() )
		return;

	pChipset->DecreaseCount( nChipsetCount );
}

bool CDigimonUser::Scene_Evol( UINT nNextFTID, bool bAbsoluteEvolution )
{
	if( PARENT_CLASS::Scene_Evol( nNextFTID, bAbsoluteEvolution ) == false )
		return false;

	GetProp_Attack()->AbsoluteSkillProcess();
	DeletePath();

	CDigimonEvolveObj* pObj = GetFTEvol()->GetEvolveObjByID( nNextFTID );	

	cDigimonTalk::sTalkEle sEle;
	sEle.s_dwDigimon = nNextFTID;
	cDigimonTalk::Print( 30010, &sEle );

	// 쿨다운 적용
	g_pGameIF->GetQuickEvol()->GetEvolCoolTimeSeq()->Start();

	assert_cs( g_pGameIF );
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUICKEVOL ) );

	// == 강제 진화가 아닐시
	// 아이템 갯수 감소
	if( bAbsoluteEvolution == false )
	{
		CDigimonEvolveObj* pFTEvolObj = nsCsFileTable::g_pEvolMng->GetEvolveInfo( GetBaseDigimonFTID() )->GetEvolveObjByID( nNextFTID );
		if( pFTEvolObj->m_nUseItem != 0 )
		{
			g_pDataMng->GetInven()->DecreaseItem( pFTEvolObj->m_nUseItem, pFTEvolObj->m_nUseItemNum, false, false );
		}

		// 조그레스 진화중 이면 소모 칩셋 제거..여기에 정의 해도 되고..Req_ActivateEvolve() 에 해도 되고..
		if(IsJointProgressing())
		{
			_DecreaseChipset( pFTEvolObj->m_nChipsetType, pFTEvolObj->m_nChipsetTypeC, pFTEvolObj->m_nChipsetNum );
		}
	}	

	int nSuccess = 1;
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CHANGE_KFM, 0, &nSuccess );

	return true;
}

bool CDigimonUser::Scene_Degenerate( UINT nNextFTID )
{
	if( PARENT_CLASS::Scene_Degenerate( nNextFTID ) == false )
		return false;

	// 쿨다운 적용
	g_pGameIF->GetQuickEvol()->GetEvolCoolTimeSeq()->Start();	

	assert_cs( g_pGameIF );
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUICKEVOL ) );

	int nSuccess = 1;
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CHANGE_KFM, 0, &nSuccess );

	return true;
}
#ifdef CROSSWARS_SYSTEM
bool CDigimonUser::Scene_CrossChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
{
	if( PARENT_CLASS::Scene_CrossChangePartner( nNextFTID, nTacticsIndex, szNextName, fNextScale, nNextLevel ) == false )
		return false;

	GetProp_Attack()->AbsoluteSkillProcess();
	DeletePath();

	m_nTacticsIndex = nTacticsIndex;
	return true;
}
bool CDigimonUser::Scene_CrossChangePartner2( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
{
	if( PARENT_CLASS::Scene_CrossChangePartner2( nNextFTID, nTacticsIndex, szNextName, fNextScale, nNextLevel ) == false )
		return false;

	GetProp_Attack()->AbsoluteSkillProcess();
	DeletePath();

	m_nTacticsIndex = nTacticsIndex;
	return true;
}
void CDigimonUser::WithoutScene_CrossChangePartner2( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
{
	if( IsLoad() == false )
	{
		sRESERVATION* pReservation = NiNew sRESERVATION;
		pReservation->s_eType = sRESERVATION::RV_ChangePartner;
		pReservation->s_nIDX = GetIDX();
		pReservation->s_nFTID = nNextFTID;
		pReservation->s_fScale = fNextScale;
		pReservation->s_nLevel = nNextLevel;
		pReservation->s_nTacticsIDX = nTacticsIndex;
		_tcscpy_s( pReservation->s_szNextName, Language::pLength::name + 1, szNextName );
		m_ReservationStack.push( pReservation );
		return ;
	}

	m_nNextFTID = nNextFTID;
	m_fNextScale = fNextScale;
	m_nNextLevel = nNextLevel;

	_tcscpy_s( m_szNextName, Language::pLength::name + 1, szNextName );	

	GetProp_Attack()->AbsoluteSkillProcess();
	DeletePath();

	m_nTacticsIndex = nTacticsIndex;


	WithoutUpdate_CrossChange2Partner();
	
}
bool CDigimonUser::Scene_CrossRecoverPartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
{
	if( PARENT_CLASS::Scene_CrossRecoverPartner( nNextFTID, nTacticsIndex, szNextName, fNextScale, nNextLevel ) == false )
		return false;

	GetProp_Attack()->AbsoluteSkillProcess();
	DeletePath();

	m_nTacticsIndex = nTacticsIndex;
	return true;
}
#endif

bool CDigimonUser::Scene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
{
	if( PARENT_CLASS::Scene_ChangePartner( nNextFTID, nTacticsIndex, szNextName, fNextScale, nNextLevel ) == false )
		return false;

	GetProp_Attack()->AbsoluteSkillProcess();
	DeletePath();

	m_nTacticsIndex = nTacticsIndex;
	return true;
}

bool CDigimonUser::WithoutScene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
{	
	if( PARENT_CLASS::WithoutScene_ChangePartner( nNextFTID, nTacticsIndex, szNextName, fNextScale, nNextLevel ) == false )
		return false;	

	GetProp_Attack()->AbsoluteSkillProcess();
	DeletePath();

	m_nTacticsIndex = nTacticsIndex;

////////////////////////////////////////////////////////
/*
#define CSLAST_ERROR
	g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
	g_CsLastError.m_nIntValue1 = 2;	
	g_pCharMng->ChangeDigimonModel( GetIDX(), m_nNextFTID, m_szNextName, m_fNextScale, m_nNextLevel );

	assert_cs( m_nTacticsIndex >= 0 );
	
	if( m_nTacticsIndex != -1 )
	{
		g_pDataMng->GetTactics()->ChangeTactics( this, m_nTacticsIndex );
	}

	if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
		m_eDigimonUserState_NoneSkill = DU_FOLLOW;

	CsC_AvObject* pFollowTarget = GetProp_Attack()->GetFollowTarget();
	GetProp_Attack()->ReleaseFollowTarget();
	SetDigimonUserState( m_eDigimonUserState_NoneSkill, pFollowTarget );

	//==========================================================================================================
	// 퀘스트 체크
	//==========================================================================================================
	g_pDataMng->GetQuest()->RevQuestCheck();
	*/
////////////////////////////////////////////////////////	

	return true;
}

void CDigimonUser::SceneLevelUp( int nLevel )
{
	PARENT_CLASS::SceneLevelUp( nLevel );

	cPrintMsg::PrintMsg( 19003, &nLevel );

	//==========================================================================================================
	// 스킬 체크
	//==========================================================================================================
	for( int e=1; e<=m_nMaxEvoUnit; ++e )
	{
		if( IsEvoPlag( e, eActive ) == false ) //NEWEVO
			continue;
			
		CDigimonEvolveObj* pEvoObj = GetFTEvol()->GetEvolveObjByEvoSlot( e );
		SAFE_POINTER_CON( pEvoObj );

		for( int i=0; i<DIGIMON_MAX_SKILL_COUNT; ++i )
		{
			CsSkill* pSkill = m_SkillMng.GetFTSkill( pEvoObj->m_dwID, i );
			if( pSkill == NULL || ( pSkill->GetInfo()->s_nLimitLevel != nLevel ) )
				continue;

 			// 메세지			
			cPrintMsg::PrintMsg( 30003, nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID )->GetInfo()->s_szName, pSkill->GetInfo()->s_szName );
			if( pEvoObj->m_dwID == GetFTID() )
			{
				cDigimonTalk::sTalkEle sEle;
				sEle.s_dwSkill = pSkill->GetInfo()->s_dwID;
				cDigimonTalk::Print( 30003, &sEle );
			}

			// 빈 퀵 슬롯을 찾자
			cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( m_nDigimonUserIndex + 1 );
			for( int q=0; q<nLimit::QuickSlot; ++q )
			{
				if( pDataQuickSlot->GetData( e )->s_QuickSlotInfo[ q ].s_eType == cResist::NONE )
				{
					// 빈 퀵슬롯에 등록
					cData_QuickSlot::SkillToQuickSlot( m_nDigimonUserIndex + 1, i, m_nDigimonUserIndex + 1, pDataQuickSlot->GetData( e ), q );
					break;
				}
			}
		}
	}
	
	//==========================================================================================================
	// 진화 테이블 체크 - 레벨업 관련만 풀어 준다
	//==========================================================================================================
	// 0에는 값이 안들어 간다.
	int nOpenSlot = -1;
	bool bBustModeSlot = false;
	for( int i=1; i<=m_nMaxEvoUnit; ++i )
	{		
		if( IsEvoPlag( i, eActive ) ) //NEWEVO
			continue;

		if( m_EvoStateArray[i] == eClosed )	// 아구몬 클래식 렙업할때 조그레스 진화관련 lks007 12.12.18
			continue;

		CDigimonEvolveObj* pEvoObj = GetFTEvol()->GetEvolveObjByEvoSlot( i );
		//assert_csm1( pEvoObj != NULL, L"EvoSlot = %d", i ); 
		SAFE_POINTER_CON( pEvoObj );
		if( pEvoObj->m_nEnableSlot == 0 )
		{
			continue;
		}
		// 진화될수있는 레벨이 충족되면.
		if( nLevel >= pEvoObj->m_nOpenLevel )
		{
			// 진화체가 열렸다
			SetEvoPlag( false, pEvoObj->m_nEvoSlot, pEvoObj->m_nEvoSlot+1 );	

			// 진화 슬롯이 이미 열려있고 레벨업 했을 때 상점 할인 업데이트
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::STORE_APPLY_DISCOUNT, this );

			// 버스트 모드가 아닐경우에만 자동 진화
			for( int evo=0; evo<MAX_EVOLUTION; ++evo )
			{
				if( GetFTEvolCurObj()->m_nEvolutionList[ evo ].nSlot == i )
				{
					nOpenSlot = evo;					
					if( bBustModeSlot == false )
					{
						DWORD dwDestFTID = GetFTEvolCurObj()->m_nEvolutionList[ evo ].dwDigimonID;
						bBustModeSlot = ( nsCsFileTable::g_pDigimonMng->GetDigimon( dwDestFTID )->GetInfo()->s_eEvolutionType == nsCsDigimonTable::ET_BUSTERMODE );
					}
					break;
				}
			}
		}
	}

	if( nOpenSlot != -1 )
	{
		g_pGameIF->GetQuickEvol()->SetTargetObject( this );
		if( bBustModeSlot == false )
		{
			g_pGameIF->GetQuickEvol()->Req_ActivateEvolve( nOpenSlot, true );
		}		
	}

	//==========================================================================================================
	// 퀘스트 체크
	//==========================================================================================================
	g_pDataMng->GetQuest()->RevQuestCheck();

	//==========================================================================================================
	// 디지몬창 레벨 갱신
	//==========================================================================================================
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_DIGIMONSTATUS ) )
		GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL );
	
	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_DLevel;
	recv.nValue1 = nLevel;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
}

void CDigimonUser::SceneSkillUp( int nSkillLevel, int nSkillPoint, UINT nSkillExp, UINT nEvoIndex )
{
	cEvoUnit* pEvoUnit = GetAttributeEvoUnit( nEvoIndex );
	pEvoUnit->m_nSkillExp = nSkillExp;
	pEvoUnit->m_nSkillExpLevel = nSkillLevel;
	pEvoUnit->m_nSkillPoint = nSkillPoint;

	if( pEvoUnit->m_nSkillPoint >= 3 )
	{
		cDigimonTalk::Print( 13004 );
	}	

	cPrintMsg::PrintMsg( 19006 );

	PlaySound( SOUND_SKILL_UP );
	DWORD dwPlag = nsEFFECT::POS_CHARPOS;
	GetProp_Effect()->AddEffect( EFFECT_SKILLUP, GetToolWidth()*0.02f, dwPlag );
}

bool CDigimonUser::_UpdateEvol_JointProgressScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateEvol_JointProgressScene( fDeltaTime ) == true )
	{
		// 진화가 최종 끝나는 시점
		if(IsJointProgressing())		
		{			
			SetJointProgressFlag(0);	// 진화, 퇴화 할때 플래그 값을 반드시 0으로 설정
		}	

		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CHANGE_KFM_ING, 0 );

		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		SetDigimonUserState( m_eDigimonUserState_NoneSkill, GetProp_Attack()->GetFollowTarget() );


		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();

		return true;
	}
	return false;

}

bool CDigimonUser::_UpdateEvolScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateEvolScene( fDeltaTime ) == true )
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CHANGE_KFM_ING, 0 );

		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		SetDigimonUserState( m_eDigimonUserState_NoneSkill, GetProp_Attack()->GetFollowTarget() );

		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();
		return true;
	}
	return false;
}

bool CDigimonUser::_UpdateDegenerateScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateDegenerateScene( fDeltaTime ) == true )
	{
		// 퇴화가 최종 끝나는 시점
		if(IsJointProgressing())		
			SetJointProgressFlag(0);	// 진화, 퇴화 할때 플래그 값을 반드시 0으로 설정

		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CHANGE_KFM_ING, 0 );

		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		SetDigimonUserState( m_eDigimonUserState_NoneSkill, GetProp_Attack()->GetFollowTarget() );

		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();
		return true;
	}
	return false;
}

#ifdef CROSSWARS_SYSTEM
bool CDigimonUser::_UpdateCrossChangePartnerStartScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateCrossChangePartnerStartScene( fDeltaTime ) == true )
	{		
		assert_cs( m_nTacticsIndex >= 0 );
		if( m_nTacticsIndex != -1 )
		{
			g_pDataMng->GetCrossTactics()->ChangeTactics( this, m_nTacticsIndex );
			
		}
		return true;
	}
	return false;
}

bool CDigimonUser::_UpdateCrossChangePartnerEndScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateCrossChangePartnerEndScene( fDeltaTime ) == true )
	{
		// 내부 프로토콜 처리 (크로스 파트너 체인지 중이다)
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CROSSCHANGE_PARTNER_ING, 0 );

		// 파트너 디지몬 따르기모드 설정
		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		// 현재 타겟 지우기
		CsC_AvObject* pFollowTarget = GetProp_Attack()->GetFollowTarget();
		GetProp_Attack()->ReleaseFollowTarget();
		SetDigimonUserState( m_eDigimonUserState_NoneSkill, pFollowTarget );

		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();

		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_CROSSDIGIMON, NULL );
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );
		return true;
	}
	return false;
}

bool CDigimonUser::_UpdateCrossChangePartnerStartScene2( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateCrossChangePartnerStartScene2( fDeltaTime ) == true )
	{		
		assert_cs( m_nTacticsIndex >= 0 );
		if( m_nTacticsIndex != -1 )
		{
			g_pDataMng->GetCrossTactics()->ChangeTactics2( this, m_nTacticsIndex );

		}
		return true;
	}
	return false;
}

bool CDigimonUser::_UpdateCrossChangePartnerEndScene2( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateCrossChangePartnerEndScene2( fDeltaTime ) == true )
	{
		// 내부 프로토콜 처리 (크로스 파트너 체인지 중이다)
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CROSSCHANGE_PARTNER_ING, 0 );

		// 파트너 디지몬 따르기모드 설정
		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		// 현재 타겟 지우기
		CsC_AvObject* pFollowTarget = GetProp_Attack()->GetFollowTarget();
		GetProp_Attack()->ReleaseFollowTarget();
		SetDigimonUserState( m_eDigimonUserState_NoneSkill, pFollowTarget );

		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();
		// 크로스 업적체크
		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_Partner_Evo_CrossWars;
		recv.nValue1 = GetBaseDigimonFTID();
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_CROSSDIGIMON, NULL );
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );
		return true;
	}
	return false;
}


void CDigimonUser::WithoutUpdate_CrossChange2Partner()
{
	g_pCharMng->ChangeCrossDigimonModel( GetIDX(), m_nNextFTID, NULL, m_szNextName, m_fNextScale, m_nNextLevel );


	assert_cs( m_nTacticsIndex >= 0 );
	if( m_nTacticsIndex != -1 )
	{
		g_pDataMng->GetCrossTactics()->ChangeTactics2( this, m_nTacticsIndex );

	}

	// 내부 프로토콜 처리 (크로스 파트너 체인지 중이다)
	//g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CROSSCHANGE_PARTNER_ING, 0 );

	// 파트너 디지몬 따르기모드 설정
	if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
		m_eDigimonUserState_NoneSkill = DU_FOLLOW;

	// 현재 타겟 지우기
	CsC_AvObject* pFollowTarget = GetProp_Attack()->GetFollowTarget();
	GetProp_Attack()->ReleaseFollowTarget();
	SetDigimonUserState( m_eDigimonUserState_NoneSkill, pFollowTarget );

	//==========================================================================================================
	// 퀘스트 진행 체크
	//==========================================================================================================
	g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
	//==========================================================================================================
	// 퀘스트 체크
	//==========================================================================================================
	g_pDataMng->GetQuest()->RevQuestCheck();
	// 크로스 업적체크
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_Partner_Evo_CrossWars;
	recv.nValue1 = GetBaseDigimonFTID();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
}

bool CDigimonUser::_UpdateCrossRecoverStartScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateCrossRecoverStartScene( fDeltaTime ) == true )
	{
		assert_cs( m_nTacticsIndex >= 0 );
		if( m_nTacticsIndex != -1 )
		{
			g_pDataMng->GetTactics()->ChangeCrossToTactics( this, m_nTacticsIndex );			
		}

		// 디지몬 진화창
		/*if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGIMONEVOL ) )
		{
			g_pGameIF->GetDigimonEvol()->Reset( m_nDigimonUserIndex );
		}*/

		//SetDigimonUserState( DU_FOLLOW );
		return true;
	}
	return false;
}

bool CDigimonUser::_UpdateCrossRecoverEndScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateCrossRecoverEndScene( fDeltaTime ) == true )
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CROSSRECOVER_PARTNER_ING, 0 );

		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		CsC_AvObject* pFollowTarget = GetProp_Attack()->GetFollowTarget();
		GetProp_Attack()->ReleaseFollowTarget();
		SetDigimonUserState( m_eDigimonUserState_NoneSkill, pFollowTarget );

		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();

		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_CROSSDIGIMON, NULL );
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );
		return true;
	}
	return false;
}

#endif
bool CDigimonUser::_UpdateChangePartnerStartScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateChangePartnerStartScene( fDeltaTime ) == true )
	{
		assert_cs( m_nTacticsIndex >= 0 );
		if( m_nTacticsIndex != -1 )
		{
			g_pDataMng->GetTactics()->ChangeTactics( this, m_nTacticsIndex );

			// 파트너몬 교체 완료, 스탯 적용 완료 후 호출
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::STORE_APPLY_DISCOUNT, this );
		}

		// 디지몬 진화창
		/*if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGIMONEVOL ) )
		{
			g_pGameIF->GetDigimonEvol()->Reset( m_nDigimonUserIndex );
		}*/

		//SetDigimonUserState( DU_FOLLOW );
		return true;
	}
	return false;
}

bool CDigimonUser::_WithoutUpdate_ChangePartner()
{
	if( PARENT_CLASS::_WithoutUpdate_ChangePartner() == true )
	{
		assert_cs( m_nTacticsIndex >= 0 );
		if( m_nTacticsIndex != -1 )
		{
			g_pDataMng->GetTactics()->ChangeTactics( this, m_nTacticsIndex );
		}

		// 디지몬 진화창
		/*if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGIMONEVOL ) )
		{
			g_pGameIF->GetDigimonEvol()->Reset( m_nDigimonUserIndex );
		}*/

		//SetDigimonUserState( DU_FOLLOW );

		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CHANGE_PARTNER_ING, 0 );

		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		CsC_AvObject* pFollowTarget = GetProp_Attack()->GetFollowTarget();
		GetProp_Attack()->ReleaseFollowTarget();
		SetDigimonUserState( m_eDigimonUserState_NoneSkill, pFollowTarget );

		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();

		return true;
	}
	return false;
}

bool CDigimonUser::_UpdateChangePartnerEndScene( float fDeltaTime )
{
	if( PARENT_CLASS::_UpdateChangePartnerEndScene( fDeltaTime ) == true )
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CHANGE_PARTNER_ING, 0 );

		if( ( m_eDigimonUserState_NoneSkill == DU_ATTACK )&&( GetProp_Attack()->GetFollowTarget() == NULL ) )
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;

		CsC_AvObject* pFollowTarget = GetProp_Attack()->GetFollowTarget();
		GetProp_Attack()->ReleaseFollowTarget();
		SetDigimonUserState( m_eDigimonUserState_NoneSkill, pFollowTarget );

		//==========================================================================================================
		// 퀘스트 진행 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::D_TYPE);
		//==========================================================================================================
		// 퀘스트 체크
		//==========================================================================================================
		g_pDataMng->GetQuest()->RevQuestCheck();

		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );
		return true;
	}
	return false;
}
//=============================================================================================================
//
//	진화
//
//=============================================================================================================
void CDigimonUser::AddEvoPlag( int nSlot, eEvoState es )
{
	m_EvoStateArray[ nSlot ] |= es;
}

void CDigimonUser::SetEvoPlag( bool bInit, int nStartSlot, int nCount )
{	
	// 테이블값 검사로 정보를 세팅한다.
	for( int i = nStartSlot; i < nCount; ++i )
	{
		CDigimonEvolveObj* EvoObj = GetFTEvol()->GetEvolveObjByEvoSlot( i );	//	슬롯 번호로 테이블에 저장된 진화 값을 얻는다.
		if( EvoObj == NULL )
			continue;	
		if( EvoObj->m_nEnableSlot == 0 )
		{
			m_EvoStateArray[ i ] = eClosed;
			continue;
		}

		bool bOpend = ( m_EvoStateArray[ i ] & eActive ) != 0;
		bool bRided = ( m_EvoStateArray[ i ] & eRide ) != 0;

		cEvoUnit* pEvoUnit = GetAttributeEvoUnit( i );

		// 해당 슬롯에 오픈 아이템 조건이 있으면.(테이블값)
		if( EvoObj->m_nOpenItemTypeS != 0 && !(pEvoUnit->m_nSlotState & 1) ) 
		{
			m_EvoStateArray[ i ] = eClosed;
			continue;	// 닫혔으면 아래 검사 안하고 간다.			
		}

		bool bOpenedQuest = true;
		if( EvoObj->m_nOpenQuest != 0 )
		{
			if( !g_pDataMng->GetQuest()->IsCompleate( EvoObj->m_nOpenQuest ) )// 퀘스트 완료 못함
				bOpenedQuest = false;
		}

		bool bOpendedLv = true;
		if( GetBaseStat()->GetLevel() < EvoObj->m_nOpenLevel )
			bOpendedLv = false;

		bool bEquipedItem = true;
#ifdef SDM_TAMER_XGUAGE_20180628
		if(EvoObj->m_nOpenQualification == NEED_QUALITICATION::_XAI_SYSTEM)
		{
			if( !g_pDataMng->GetTEquip()->IsExistItem(nTamer::eParts::XAI) )
				bEquipedItem = false;
		}
#endif

		if( pEvoUnit->m_nSlotState & 1 )
		{
			if( EvoObj->m_nChipsetType != 0 )// 조그레스인 진화 인 경우 )
			{
				if( bOpendedLv && bEquipedItem )
					m_EvoStateArray[ i ] = eActive;
				else
					m_EvoStateArray[ i ] = eInActive;
			}
			else if( bOpenedQuest && bOpendedLv && bEquipedItem )
				m_EvoStateArray[ i ] = eActive;
			else
				m_EvoStateArray[ i ] = eInActive;
		}
		else if( EvoObj->m_nChipsetType != 0 )// 조그레스인 진화 인 경우
		{
			m_EvoStateArray[ i ] = eClosed;
		}
		else if( bOpenedQuest && bOpendedLv && bEquipedItem )
		{
			if( 0 != EvoObj->m_nOpenItemTypeS )
				m_EvoStateArray[ i ] = eClosed;
			else
				m_EvoStateArray[ i ] = eActive;
		}
		else
			m_EvoStateArray[ i ] = eInActive;		
		
		if( pEvoUnit->m_nSlotState & 8 )
			m_EvoStateArray[ i ] |= eRide;

		if(IsJogressTree(EvoObj->m_nEvolutionTree) && ( m_EvoStateArray[ i ] & eActive || m_EvoStateArray[ i ] & eInActive ))
			//		if(EvoObj->m_nEvolutionTree && ( m_EvoStateArray[ i ] & eActive || m_EvoStateArray[ i ] & eInActive ))
		{
			SetEvoTree(EvoObj->m_nEvolutionTree);
		}

		// 초기화 상태가 아닐경우 - 상태 변화
		if( bInit )
			continue;

		// 비활성화에서 활성화로 변하였다
		if( ( bOpend == false )&&( m_EvoStateArray[ i ] & eActive ) )
		{
			// 신규 슬롯 오픈
			TCHAR* pBaseName = nsCsFileTable::g_pDigimonMng->GetDigimon( m_dwBaseDigimonID )->GetInfo()->s_szName;
			TCHAR* pOpenEvoName = nsCsFileTable::g_pDigimonMng->GetDigimon( EvoObj->m_dwID )->GetInfo()->s_szName;
			cPrintMsg::PrintMsg( 30010, pBaseName, pOpenEvoName );	// 진화체습득 '@<tc:990>축하합니다. [str]이(가) [str]로 진화할 수 있습니다@</tc>%

			//==========================================================================================================
			// 업적 체크
			//==========================================================================================================			
			// 파트너몬
			if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( m_dwBaseDigimonID ) )
			{
				int nEvoType = nsCsFileTable::g_pDigimonMng->GetDigimon( EvoObj->m_dwID )->GetInfo()->s_eEvolutionType;
				if( nEvoType == nsCsDigimonTable::ET_MEGA )
				{
					GS2C_RECV_CHECKTYPE recv;
					recv.nType = AchieveContents::CA_Partner_Evo_Mega;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
				}
				else if( nEvoType == nsCsDigimonTable::ET_BUSTERMODE )
				{
					GS2C_RECV_CHECKTYPE recv;
					recv.nType = AchieveContents::CA_Partner_Evo_Bust;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
				}
			}
			// 다른 디지몬
			else
			{
				GS2C_RECV_CHECKTYPE recv;
				recv.nType = AchieveContents::CA_Tactics_Evo;
				recv.nValue1 = m_dwBaseDigimonID;
				recv.nValue2 = i;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
			}
		}

		// 비활성화에서 활성화로 변하였다
		if( ( bRided == false )&&( m_EvoStateArray[ i ] & eRide ) )
		{
			//==========================================================================================================
			// 업적 체크
			//==========================================================================================================
			GS2C_RECV_CHECKTYPE recv;
			recv.nType = AchieveContents::CA_RideOpen;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
		}	
	}
}

void CDigimonUser::UpdateXEvoFlag()
{
#ifdef SDM_TAMER_XGUAGE_20180628
	SetEvoPlag( true, 1, GetMaxEvoUnit() +1 );
#endif
}

void CDigimonUser::CompQuestJointProgress( DWORD dwOpenQuestID )
{				
	for( int i=1; i<nLimit::EvoUnit; ++i )
	{
		CDigimonEvolveObj* pFTEvolObj = GetFTEvol()->GetEvolveObjByEvoSlot( i );
		if( pFTEvolObj == NULL )
			continue;	

		// 조그레스 조건
		if( pFTEvolObj->m_nChipsetType != 0 && pFTEvolObj->m_nOpenQuest != 0 )
		{
			if( dwOpenQuestID == pFTEvolObj->m_nOpenQuest )	// 퀘스트 인덱스가 같아야한다.
			{
				GetAttributeEvoUnit(i)->m_nSlotState = 1;
				GS2C_RECV_ENCYCLOPEDIA_EVOLINFO recv;
				recv.iBaseDigimonID = GetBaseDigimonFTID();
				recv.iEvolSlot = i;
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_SET_EVOLINFO, &recv );				
			}			
		}
	}
}

bool CDigimonUser::IsOpenedEvoSlot( unsigned int const& nSlotNum )
{
	if( nLimit::EvoUnit < nSlotNum )
		return false;

	if(m_EvoUnit[nSlotNum].m_nSlotState & nEvo::Opened )	// 슬롯이 활성화이면 계속 
		return true;

	return false;
}

int CDigimonUser::GetCurEvoUnit()
{
	
	// 진화 테이블에 정의된 진화 슬롯으로 넘겨주기
	// 뒤에서 부터 읽는다.
	for(int i = m_nMaxEvoUnit; i > 0; i--)
	{
		if(m_EvoUnit[i].m_nSlotState)	// 슬롯이 활성화이면 계속 
		{			
			CDigimonEvolveObj* pEvoObj = GetFTEvol()->GetEvolveObjByEvoSlot( i );	
			if( pEvoObj->m_nEvolutionTree == TREE_CAPSULE ) // 캡슐진화체라면
				continue;
			return pEvoObj->m_nEvoSlot;
		}		
	}
	return -1;
	
/*
	// base 테이블에 정의된 진화체 값으로 넘겨주기
	// 뒤에서 부터 읽는다.
	int	nCurEvoSlot = 1;
	for(int i = m_nMaxEvoUnit; i > 1; i--)
	{
		if(m_EvoUnit[i].m_nSlotState)	// 슬롯이 활성화이면 계속 		
		{
			nCurEvoSlot = i;
			CDigimonEvolveObj* pEvoObj = GetFTEvol()->GetEvolveObjByEvoSlot( nCurEvoSlot );
			nCurEvoSlot = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID )->GetInfo()->s_eEvolutionType;
			return nCurEvoSlot;
		}
	}

	CDigimonEvolveObj* pEvoObj = GetFTEvol()->GetEvolveObjByEvoSlot( nCurEvoSlot );
	nCurEvoSlot = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID )->GetInfo()->s_eEvolutionType;

	return nCurEvoSlot;
*/

}


void CDigimonUser::SetAttributeEvoUnit( int nMaxEvoUnit, cEvoUnit* pEvoUnit )
{
#ifndef _GIVE
	if( m_pQuickSlot == NULL )
		return;
#endif

	SetEvoTree(TREE_COMMON);

	m_nMaxEvoUnit = nMaxEvoUnit;
	memcpy( &m_EvoUnit[ 1 ], &pEvoUnit[ 1 ], m_nMaxEvoUnit*sizeof( cEvoUnit ) );	

	//NEWEVO
	SetEvoPlag( true, 1, m_nMaxEvoUnit + 1 );

	// 인터페이스 설정
	if( m_nDigimonUserIndex == 0 )
		g_pGameIF->GetQuickEvol()->SetTargetObject( this );
}

int CDigimonUser::GetAttributeEvoUnitSlotState( int nSlot )
{ 
	if( nLimit::EvoUnit <= nSlot )
		return 0;

	return m_EvoUnit[ nSlot ].m_nSlotState; 
}

cEvoUnit* CDigimonUser::GetAttributeEvoUnit()
{ 
	return m_EvoUnit; 
}

cEvoUnit* CDigimonUser::GetAttributeEvoUnit( int nSlot )
{  
	if( nLimit::EvoUnit <= nSlot )
		return NULL;

	return &m_EvoUnit[ nSlot ]; 
}

cEvoUnit* CDigimonUser::GetCurAttributeEvoUnit()
{ 
	SAFE_POINTER_RETVAL( m_pFT_Evol, NULL );	
	CDigimonEvolveObj* pEvObj = m_pFT_Evol->GetEvolveObjByID( GetFTID() );
	SAFE_POINTER_RETVAL( pEvObj, NULL );
	return &m_EvoUnit[ pEvObj->m_nEvoSlot ]; 
}

int CDigimonUser::GetCurrentEvoMaxSkillLv()
{
	cEvoUnit* pEvoUnit = GetCurAttributeEvoUnit();
	SAFE_POINTER_RETVAL(pEvoUnit, 0);

	int nMaxLv = 0;
#ifdef SDM_DIGIMONSKILL_LV_EXPEND_20181206
	for( int n = 0; n < nLimit::Skill; ++n )
	{
		if( nMaxLv < pEvoUnit->m_nSkillMaxLevel[n] )
			nMaxLv = pEvoUnit->m_nSkillMaxLevel[n];
	}
#endif

	return nMaxLv;
}

bool CDigimonUser::IsCurJointProgress()
{
	CDigimonEvolveObj* pDigimonEv = GetFTEvolCurObj();
	SAFE_POINTER_RETVAL( pDigimonEv, false );
	if( 0 != pDigimonEv->m_nChipsetType  && (0 != pDigimonEv->m_nOpenQuest || pDigimonEv->m_nEvolutionTree == CDigimonUser::TREE_JOGRESS) )	
	{
		return true;
	}
	return false;
}

void CDigimonUser::SetAttributeExp( n2* pUnit )
{
	memcpy( m_AttributeExp, pUnit, sizeof(n2)*NewAttribute::NatualExpCount);
}

void CDigimonUser::SetNatureExp( n2* pUnit )
{
	memcpy( m_NatureExp, pUnit, sizeof(n2)*NewAttribute::MaxNatualType);
	//memset(&m_NatureExp, 10000, sizeof(n2)*NewAttribute::MaxNatualType);
}

n2 CDigimonUser::GetFTAttributeExp( int nFTAttributeType )
{
	switch(nFTAttributeType)
	{
	case nsCsDigimonTable::AT_DATA: nFTAttributeType = NewAttribute::Data; break;
	case nsCsDigimonTable::AT_VACCINE: nFTAttributeType = NewAttribute::Vaccine; break;
	case nsCsDigimonTable::AT_VIRUS: nFTAttributeType = NewAttribute::Virus; break;
	case nsCsDigimonTable::AT_UNIDENTIFIED:
		{
			// 내 디지몬이 언노운이 아니면
			if (nFTAttributeType == GetFTDigimon()->GetInfo()->s_eAttributeType )
			{
				return _GetUnknownExpforEvolRate(GetFTDigimon()->GetInfo()->s_eEvolutionType);
			}
			return 0;
			
		}
	default: return 0;

	}

	return m_AttributeExp[ nFTAttributeType ];

}

n2 CDigimonUser::_GetUnknownExpforEvolRate( nsCsDigimonTable::eEVOLUTION_TYPE eEvol )
{
	switch( eEvol )
	{
	case nsCsDigimonTable::ET_DIGITAMA:				return nMonster::DIgitama;
	case nsCsDigimonTable::ET_INTRAINING:			return nMonster::Training;
	case nsCsDigimonTable::ET_ROOKIE:				return nMonster::Rookie;
	case nsCsDigimonTable::ET_CHAMPION:				return nMonster::Champion;
	case nsCsDigimonTable::ET_ULTIMATE:				return nMonster::Ultimate;
	case nsCsDigimonTable::ET_MEGA:					return nMonster::Mega;
	case nsCsDigimonTable::ET_BUSTERMODE:			return nMonster::BusterMode;
	case nsCsDigimonTable::ET_JOINTPROGRESSMODE:	return nMonster::Joint;
	// 몬스터 경험치 비율
	case nsCsDigimonTable::ET_CAPSULE:				return nMonster::UPCapsuleEvo;
	case nsCsDigimonTable::ET_SPIRIT:				return nMonster::UPSpiritEvo;
#ifdef SDM_TAMER_XGUAGE_20180628
	case nsCsDigimonTable::ET_X_ROOKIE:				return nMonster::UPRookieX;
	case nsCsDigimonTable::ET_X_CHAMPION:			return nMonster::UPChampionX;
	case nsCsDigimonTable::ET_X_ULTIMATE:			return nMonster::UPUltimateX;
	case nsCsDigimonTable::ET_X_MEGA:				return nMonster::UPMegaX;
	case nsCsDigimonTable::ET_X_BUSTERMODE:			return nMonster::UPBurstModeX;
	case nsCsDigimonTable::ET_X_JOGRESS:			return nMonster::UPCombineModeX;
	case nsCsDigimonTable::ET_MIX:					return nMonster::UPExtraEvo;
#endif
	}
	assert_cs( false );
	return 0;
}

CDigimonEvolveInfo*	CDigimonUser::GetFTEvol()
{ 
	return m_pFT_Evol; 
}

CDigimonEvolveObj*	CDigimonUser::GetFTEvolCurObj()
{
	SAFE_POINTER_RETVAL( m_pFT_Evol, NULL );
	return m_pFT_Evol->GetEvolveObjByID( GetFTID() ); 
}

CDigimonEvolution::sBATTLE_EVO_INFO* CDigimonUser::GetFTBattleEvo()
{ 
	return m_pFT_BattleEvo; 
}

CDigimonEvolution::sBATTLE_EVO_INFO::sELEMENT*	CDigimonUser::GetFTBattleEvoCurElement()
{
	return m_pFT_BattleEvoElement; 
}

// 0~10 인덱스 11 개
n2 CDigimonUser::GetFTNatureExp( int nFTNatureType )
{
	if(nsCsDigimonTable::NT_NONE == nFTNatureType + nsCsDigimonTable::NT_ICE)
		return nFTNatureType = 0;

	return m_NatureExp[nFTNatureType];
}

bool CDigimonUser::IsCurrentElement( int nElementID )
{	
	if((0 != m_pFTDigimon->GetInfo()->s_eBaseNatureTypes[0]) && (nElementID == m_pFTDigimon->GetInfo()->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE)) return true;
	if((0 != m_pFTDigimon->GetInfo()->s_eBaseNatureTypes[1]) && (nElementID == m_pFTDigimon->GetInfo()->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE)) return true;
	if((0 != m_pFTDigimon->GetInfo()->s_eBaseNatureTypes[2]) && (nElementID == m_pFTDigimon->GetInfo()->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE)) return true;
	
	return false;
}

//=============================================================================================================
//
//	테이블
//
//=============================================================================================================

//=============================================================================================================
//
//	부가정보
//
//=============================================================================================================

float CDigimonUser::CalTamerDist()
{
	return (g_pCharMng->GetTamerUser()->GetPos2D() - GetPos2D()).Length();
}

void CDigimonUser::_StopAttack( bool bSkillCall )
{
	SAFE_POINTER_RET( net::game );

	CsC_AttackProp* pAttProp = GetProp_Attack();
	SAFE_POINTER_RET( pAttProp );

	if( !bSkillCall && !pAttProp->IsAttackingTarget() )
		return;
	
	net::game->SendStopAttack( GetUniqID() );
	
	pAttProp->ReleaseAttackingTarget();

	
//	if( bSkillCall == false )
// 	{
// 		if( ( GetProp_Attack()->IsAttackingTarget() == true )&&( net::game ) )
// 		{
// 			net::game->SendStopAttack( GetUniqID() );
// 		}
// 	}
// 	else
// 	{
// 		if( net::game )
// 		{
// 			net::game->SendStopAttack( GetUniqID() );
// 		}
// 	}
// 	
//	GetProp_Attack()->ReleaseAttackingTarget();
}

//=============================================================================================================
//
//	이동
//
//=============================================================================================================
void CDigimonUser::PauseEndTime( ePAUSE_TIME_END ePTEType, int nValue )
{
	switch( ePTEType )
	{
	case PTE_PATH:
		{
			switch( nValue )
			{
			case PTE_P_NONE:
				break;
			case PTE_P_KEYCHECKTIME_RESET:
				m_UserServerMove.KeyCheckTimeReset();
				m_UserServerMove.SetServerMoveReady();				
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	}
	PARENT_CLASS::PauseEndTime( ePTEType, nValue );
}

void CDigimonUser::DoPos_ByTamer()
{	
	NiPoint2 niTargetPos = _GetArrivePos_ByTamer();
	NiPoint2 vResultPos;
	if( GetProp_Path()->CalTargetPath( vResultPos, NiPoint3(niTargetPos.x, niTargetPos.y, 0.0f), 0 ) == true )
	{
		if( GetProp_Path()->SetPath( vResultPos ) == true )
		{
			m_UserServerMove.SetServerMoveReady();
			m_IdleCheckTimeSeq.OffReset();
		}
	}

// 	GetProp_Path()->SetPath( _GetArrivePos_ByTamer() );
// 
// 	//=======================================================================
// 	//	서버 이동
// 	//=======================================================================	
// 	m_UserServerMove.SetServerMoveReady();
// 
// 	m_IdleCheckTimeSeq.OffReset();
}

NiPoint2 CDigimonUser::_GetArrivePos_ByTamer()
{
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	NiPoint2 vTarget = pTamerUser->GetProp_Path()->GetArrivePos();

	NiPoint3 vPath = NiPoint3( vTarget.x, vTarget.y, 0.0f );
	NiPoint3 vPath2( 0, 0, 0 );
	NiPoint3 vDir = vPath - pTamerUser->GetPos();	
	vDir.z = 0.0f;
	if( vDir.Unitize() < 1.0f )
	{
		vDir = pTamerUser->GetDir();
		vDir.z = 0.0f;
		vDir.Unitize();
	}

	vPath -= vDir * DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT/2.f;
	vPath2 = vPath;

	NiPoint3 cross = NiPoint3( 0, 0, 1 );
	NiPoint3 v = cross.Cross( vDir );
	switch( m_nDigimonUserIndex )
	{
	case 0:		vPath -= v * DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT/2.f*GetToolWidth()*0.01f;			break;
	case 1:		vPath += v * 100.f;																				break;
	case 2:		vPath -= vDir * 150.f;																			break;
	}

	if( GetProp_Path()->PathTest( vPath ) == true )
	{
		return NiPoint2( vPath.x, vPath.y );
	}
	
	assert_cs( vPath2.x != 0 && vPath2.y != 0 );
	if( GetProp_Path()->PathTest( vPath2 ) == true )
	{
		return NiPoint2( vPath2.x, vPath2.y );
	}

	return vTarget;
}

void CDigimonUser::DoPos_ByNpcFind( int nNpcID )
{
	m_pFindTarget = g_pNpcMng->GetNpc( nNpcID );
	assert_cs( m_pFindTarget != NULL );

	NiPoint2 vResultPos;
	if( GetProp_Path()->CalTargetPath( vResultPos, m_pFindTarget->GetPos(), 500.0f ) == true )
	{
		if( GetProp_Path()->SetPath( vResultPos ) == true )
		{
			m_UserServerMove.SetServerMoveReady();
		}		
	}
	
	m_eFindState = NFS_DOING;
	m_FindCheckTimeSeq.SetDeltaTime( 10000 );
	m_FindCheckTimeSeq.Reset();
}

void CDigimonUser::DoAttackTargetPos()
{
	if( IsPause( PAUSE_PATH ) == true )
		return;

	// 스킬 패킷을 보냈다면
	if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::USE_DIGIMON_SKILL ) == false )
	{
		return;
	}

	CsC_AttackProp* pAttackProp = GetProp_Attack();
	SAFE_POINTER_RET( pAttackProp );
	CsC_AvObject* pTarget = pAttackProp->GetFollowTarget();
	SAFE_POINTER_RET( pTarget );

	CsC_PathProp* pPathProp = GetProp_Path();	
	SAFE_POINTER_RET( pPathProp );

	// 최하거리 0보다는 커야하고 어택 가능거리에서 PATH_IGNORE_DIST 만큼 더 가까이 간다. ( 최하 PATH_IGNORE_DIST 은 움직이지 않으므로 )
	float fAttackDist = CsMax( 0, pAttackProp->GetEnableFollowDist() - PATH_IGNORE_DIST );	

	// 상대방과의 거리 체크

	NiPoint3 vDir = GetPos() - pTarget->GetPos();
	vDir.z = 0.0f;
	float fTargetDist = vDir.Length();

	// 가깝다면 패스
	if( fTargetDist < fAttackDist )
		return;

	// 근접공격이라면
	if( pAttackProp->IsFollowIteratorRange() == false )
	{
#ifdef CRASHDUMP_0326
		// path 정보 여부 확인
		if( !pTarget->IsProperty( CsC_Property::PATH ) )
		{
			// path 프로퍼티 안의 m_Path는 포인터가 아니므로 NULL이 될 수 없음. m_Path.m_iPath는 NULL 이거나 data가 있거나 임.
			//DUMPLOG( "L0002 - Dist : %f, Target : %f, TargetID : %d", fAttackDist, fTargetDist, pTarget->GetModelID() );

			// 타겟의 path 프로퍼티 정보가 없다면 걍 리턴시키자
			return;
		}
#endif
		// 상대방의 움직임 체크 - 움직임에 따라 도착거리를 다르게 하자	
		if( pTarget->GetProp_Path()->IsMove() )	
		{
			float fDestRot = NiACos( vDir.Dot( pTarget->GetDir() ) );
			if( vDir.Cross( pTarget->GetDir() ).z > 0 )
				fDestRot = -fDestRot;


			// 뛰고 있는가 ( 몬스터라면 전투중 )
			if( pTarget->GetProp_Path()->GetMoveState() == CsC_PathProp::MS_RUN )
			{
				// 가까워진다
				if( abs( fDestRot ) < NI_HALF_PI )
				{
					if( abs( fDestRot ) < 0.15f )
						fAttackDist *= 1.5f;
					else
						fAttackDist *= 1.2f;
				}
				// 멀어진다
				else
				{
					if( abs( fDestRot ) > 3.0f )
						fAttackDist *= 0.6f;
					else
						fAttackDist *= 0.8f;
				}
			}
			else
			{
				// 멀어진다
				if( abs( fDestRot ) > 2.85f )
					fAttackDist *= 0.9f;
			}
		}
	}	

	NiPoint2 vResultPos;
	if( pPathProp->CalTargetPath( vResultPos, pAttackProp->GetFollowTarget()->GetPos(), fAttackDist ) == true )
	{
		if( pPathProp->SetPath( vResultPos ) == true )
		{
			//=======================================================================
			//	서버 이동
			//=======================================================================
#ifdef CRASHDUMP_0424
			m_UserServerMove.SetServerMoveReady2( pPathProp );
#else
			m_UserServerMove.SetServerMoveReady();
#endif
		}
	}	
}

bool CDigimonUser::CheckTamerDist()
{
	// 테이머 와의 거리
	NiPoint3 v = g_pCharMng->GetTamerUser()->GetPos() - GetPos();
	v.z = 0;
	float fTamerDist = v.Length();

	return ( fTamerDist < DIGIMONUSER_FOLLOW_RANGE );
}

bool CDigimonUser::CheckTargetDist( CsC_AvObject* pTarget, float fAttRange )
{
	// 타겟과의 거리
	NiPoint3 v = pTarget->GetPos() - GetPos();
	v.z = 0;
	float fDist = v.Length();

	return ( fDist < DIGIMONUSER_FOLLOW_RANGE + fAttRange );
}

void CDigimonUser::_SetTargetPos()
{
	assert_cs( _IsApplyDUState( DU_FOLLOW ) == true );

	if( m_MoveCoolTimeSeq.IsCoolTime() == true )
		return;

	m_MoveCoolTimeSeq.Start();
	if( GetProp_Path()->IsMove() == true )
	{
		m_MoveOnceTimeSeq.Reset();
	}
}

bool CDigimonUser::_SetAttack( CsC_AvObject* pAttackSrc )
{
	if( _IsApplyDUState( DU_ATTACK ) == false )
		return false;

	// 스킬 사용거리가 서버와 클라가 맞지 않아 서버의 공식에 클라를 맞춤.
	float fRange = .0f;
	if( GetAttackRangeType() == CsC_AvObject::ART_CLOSE_COMBAT )
	{
#ifdef _GIVE
		fRange = g_pModelDataMng->GetData( GetModelID() )->GetInfo()->s_fScale * GetAttackRange() * GetOrgGameScale();
#else
		fRange = g_pModelDataMng->GetData( GetModelID() )->GetInfo()->s_fScale * 
			nsCsFileTable::g_pDigimonMng->GetDigimon( GetModelID() )->GetInfo()->s_nBaseStat_AttRange * GetOrgGameScale();
#endif
	}
	else
		fRange = GetAttackRange();

	//// 거리가 너무 멀면 공격 하지 말자
	//if( CheckTargetDist( pAttackSrc, fRange ) == false )
	//	return false;

	assert_cs( pAttackSrc != NULL );
	GetProp_Attack()->SetFollowTarget( pAttackSrc, fRange, CsC_AttackProp::FI_ATTACK, IsAttackRangeType() );
	DoAttackTargetPos();
	return true;
}

void CDigimonUser::_MoveStop_ByArriveDist()
{
	if( g_pCharMng->GetTamerUser()->GetProp_Path()->IsMove() == false )
		return;

	if( GetProp_Path()->IsMove() == false )
		return;

	// 테이머가 이동하고 있고 디지몬도 이동하고 있다면
	// 테이머 도착점
	float fTamerDist = g_pCharMng->GetTamerUser()->GetProp_Path()->GetMoveDist();
	// 테이머 도착점을 기준으로한 디지몬 도착점
	NiPoint2 vDest = _GetArrivePos_ByTamer();
	NiPoint3 vCurPos = GetPos();	
	float fDigimonDist = ( NiPoint2( vCurPos.x, vCurPos.y ) - vDest ).Length();

	if( ( fDigimonDist + DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT*0.1f )< fTamerDist )
	{
		SetRotation_Ani( vDest );
		DeletePath();
	}
}

bool CDigimonUser::_IsDoMove()
{
#ifdef CROSSWARS_SYSTEM
	if( PARENT_CLASS::m_eDigimonState == CDigimon::DIGIMON_CROSSCHANGE_PARTNER_START ||
		PARENT_CLASS::m_eDigimonState == CDigimon::DIGIMON_CROSSCHANGE_PARTNER2_START ||
		PARENT_CLASS::m_eDigimonState == CDigimon::DIGIMON_CROSSRECOVER_START ||
		PARENT_CLASS::m_eDigimonState == CDigimon::DIGIMON_CROSSRECOVER_END )
		return false;
#endif

	float fDeltaLength = DIGIMONUSER_TAMER_RAMAIN_MOVE_LENGTH_CONSTANT*1.5f*GetToolWidth()*0.01f;

	NiPoint2 vPos;
	vPos.x = m_vPos.x;
	vPos.y = m_vPos.y;

//#ifdef ZONEMAP_CLICK_MOVE	// 존맵 클릭으로 이동 시 이동 거리가 넓어지므로 디지몬과 테이머의 간격 > 200 일 때 true 해준다.
	return (vPos - g_pCharMng->GetTamerUser()->GetPos2D()).Length() > PATH_IGNORE_DIST + fDeltaLength;
// #else// 기존의 문제점 테이머가 엄청 멀리가게 되면 거리 200이 남을 때까지 디지몬이 이동하지 않는다.
// 	NiPoint2 vTarget = g_pCharMng->GetTamerUser()->GetProp_Path()->GetArrivePos();
// 	float fTamerMoveDist = g_pCharMng->GetTamerUser()->GetProp_Path()->GetMoveDist();
// 
// 	float f1,f2;
// 	f1 = (vTarget - vPos).Length();			// 테이머 도착점과 디지몬과의 거리
// 	f2 = fTamerMoveDist + fDeltaLength;		// 테이머 남은 이동거리
// 	return ( f1 > f2 + PATH_IGNORE_DIST );
// #endif
}

NiPoint2 CDigimonUser::KnockBack( NiPoint2 vHitterPos )
{
	NiPoint2 vKnockBackPos = PARENT_CLASS::KnockBack( vHitterPos );

	if( net::game )
	{
		nSync::Pos SyncPos( (int)vKnockBackPos.x, (int)vKnockBackPos.y );		
		m_UserServerMove.SetLastSyncPos( SyncPos.m_nX, SyncPos.m_nY );		
		net::game->SendMoveToKnockBack( GetUniqID(), SyncPos );
	}
	return vKnockBackPos;
}

//=============================================================================================================
//
//	라이딩
//
//=============================================================================================================
bool CDigimonUser::SetRide()
{
	if( PARENT_CLASS::SetRide() == false )
		return false;

	g_pHpBar->DeleteBuddy( GetUniqID() );
	CsC_AttackProp* pPropAttack = GetProp_Attack();	

	pPropAttack->SetHitTarget( NULL );
	pPropAttack->ReleaseFollowTarget();	

	SetDigimonUserState( DU_FOLLOW );

	// 테이머가 움직이고 있다면 - 도착지점으로 디지몬도 움직이자
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();

	if( pTamerUser->GetProp_Path()->IsMove() )
	{
		GetProp_Path()->SetPath( pTamerUser->GetProp_Path()->GetArrivePos() );
		m_UserServerMove.SetServerMoveReady();
	}

	return true;
}

bool CDigimonUser::SetRideEnd()
{
	if( PARENT_CLASS::SetRideEnd() == false )
		return false;

	SetDigimonUserState( CDigimonUser::DU_FOLLOW );
	return true;
}

void CDigimonUser::ResetSkillCool()
{
	int nCnt = GetSkillMng()->GetSkillCount();
	for( int i = 0 ; i < nCnt ; i++ )
	{
		if( GetSkillMng()->GetSkill( i )->s_CoolTime.IsCoolTime() )
		{
			GetSkillMng()->GetSkill( i )->s_CoolTime.Reset();
			GetSkillMng()->GetSkill( i )->s_CoolTime.SetCoolReset( true );
		}
	}
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		CProtocol.m_wStr = _T("스킬 쿨타임 초기화 효과 발동");
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

#undef		PARENT_CLASS

void CDigimonUser::ReloadEffect()
{
	nsCsFileTable::g_pEvolMng->Reload();
	GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_DLEVEL );
	GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_ENCHANT );
	GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_SIZE );	

	CheckLevelEffect();
	CheckEnchantEffect();
	CheckSizeEffect();
#ifdef  SDM_TAMER_XGUAGE_20180628
	CheckingXAntiEffect();
#endif
}

DWORD CDigimonUser::GetCurrentDigimonType() const
{
#ifdef SDM_TAMER_XGUAGE_20180628
	if(m_pFTDigimon != NULL)
	{
		CsDigimon::sINFO* pkDigimonInfo = m_pFTDigimon->GetInfo();
		if(pkDigimonInfo != NULL)
		{
			if(nsCsDigimonTable::ET_X_ROOKIE <= pkDigimonInfo->s_eEvolutionType &&
				pkDigimonInfo->s_eEvolutionType <= nsCsDigimonTable::ET_X_JOGRESS)
			{
				return pkDigimonInfo->s_dwDigimonID;
			}
		}
	}
#endif
	// 크로스워즈..?
	if( 0 != m_nNextFTID )
		return m_nNextFTID;

	return m_dwBaseDigimonID;
}

void CDigimonUser::TamerDistCheck()
{
	_UpdateTamerDist();
}

void CDigimonUser::SetBattleEPDecrDisable()
{ 
	m_BattleEPDecrTimeSeq.SetDisable(); 
}

void CDigimonUser::SetNewSkillPoint( int nPoint )
{ 
	m_nNewSkillPoint = nPoint; 
}

DWORD CDigimonUser::GetNewSkillPoint() const
{ 
	return m_nNewSkillPoint; 
}

void CDigimonUser::DecNewSkillPoint( int nDec )
{ 
	m_nNewSkillPoint -= nDec; 
	assert_cs( m_nNewSkillPoint >= 0 ); 
}

float CDigimonUser::GetAttackRange()
{ 
	if( m_pBaseStat )
		return (float)( (sUSER_STAT*)m_pBaseStat )->GetAttRange(); 
	return CDigimon::GetAttackRange();
}
DWORD CDigimonUser::GetEvoValue( int nSlot, eEvoState mask )
{ 
	return m_EvoStateArray[ nSlot ]&mask; 
}	
// float CDigimonUser::GetAttackSpeed()
// { 
// 	if( m_pBaseStat )
// 	{
// 		float fAcc = m_pBaseStat->IsFatigue() ? 1.5f : 1.0f;
// 
// 		return (float)( (sUSER_STAT*)m_pBaseStat )->GetAttSpeed() * 0.001f * fAcc; 
// 	}
// 	return CDigimon::GetAttackSpeed();
// }

//////////////////////////////////////////////////////////////////////////

void CDigimonUser::SetReservationSkillReaminCoolTime( sSkillCoolTimeReservation const& reservationData )
{
	m_ReservationData = reservationData;	

	switch( m_eDigimonState )
	{
	case DIGIMON_EVOL:
	case DIGIMON_EVOL_JOINTPROGRESS:
#ifdef CROSSWARS_SYSTEM
	case DIGIMON_CROSSCHANGE_PARTNER_START:
	case DIGIMON_CROSSCHANGE_PARTNER2_START:
	case DIGIMON_CROSSRECOVER_START:
#endif
	case DIGIMON_CHANGE_PARTNER_START:
		return;
	}

	SetReservationSkillCoolTime(GetFTID());
}

void CDigimonUser::SetReservationSkillCoolTime(uint nFileTableID)
{
	if( nFileTableID != m_ReservationData.dwDigimonTBID )
		return;

	std::list<sReaminCoolTime>::iterator it = m_ReservationData.listReaminTime.begin();
	for( ; it != m_ReservationData.listReaminTime.end(); ++it )
		m_SkillMng.SetSkillCooltime( (*it).dwSkillCode, (*it).dwRemainTime );

	m_ReservationData.Clear();
}
