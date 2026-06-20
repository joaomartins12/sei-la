
#include "stdafx.h"
#include "DigimonUser.h"

#define COMMON_EVOL_UPDATE_DELAY_TIME		0.8f

THREAD_MEMPOOL_CPP( CDigimon )
#define		PARENT_CLASS		CsC_DPartObject
CsCRTTI_CPP( PARENT_CLASS, CDigimon, RTTI_DIGIMON )

CDigimon::CDigimon() : 
m_pEnchentStat(NULL),
mToeName("")
{
	m_BuffData.Init( this );
	m_pCharImage = NULL;
	m_pFTRide = NULL;
	m_nTamerLinkID = 0;
	m_pCrossTempDigimon = NULL;
	m_nNextFTID = 0;
}

void CDigimon::Delete()
{
	SetDigimonState( DIGIMON_NORMAL );
	PARENT_CLASS::Delete();
}

void CDigimon::PreDelete()
{
	SetDigimonState( DIGIMON_NORMAL );
	g_pCharResMng->CharImageResDelete( &m_pCharImage );

	PARENT_CLASS::PreDelete();

	m_Option.Delete();
	m_SkillMng.Delete();
	m_Condition.Delete();
	m_BuffData.Delete();

	SAFE_NIDELETE( m_pEnchentStat );

	while( m_ReservationStack.empty() == false )
	{
		sRESERVATION* pReservation = m_ReservationStack.front();
		m_ReservationStack.pop();
		SAFE_NIDELETE( pReservation );
	}

	m_nTamerLinkID = 0;
	m_pCrossTempDigimon = NULL;
}

void CDigimon::Init( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp )
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	Init( nIDX, nFileTableID, vPos, fRot, szName, pProp );
}

void CDigimon::Init( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp )
{
	// 파일 테이블 로드
	_ResetFTDigimon( nFileTableID );

	m_Condition.Init( this );
	m_pEnchentStat = NiNew sENCHENT_STAT;

	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, pProp );

	_Init( szName );
#ifdef GM_CLOCKING
	m_bIsClocking = false;
#endif
}

CsC_AvObject::eCHECK_TYPE CDigimon::CheckModel()
{
	eCHECK_TYPE ct = PARENT_CLASS::CheckModel();
	if( ct != CHECK_LOAD_COMP )
		return ct;

	if( m_ReservationStack.empty() == false )
	{
		sRESERVATION* pReservation = m_ReservationStack.front();			
		switch( pReservation->s_eType )
		{
		case sRESERVATION::RV_Evol:
#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad2;	
			g_CsLastError.m_nIntValue1 = 10;	
			Scene_Evol( pReservation->s_nFTID, false );
			//g_pCharMng->ChangeDigimonKFM( pReservation->s_nIDX, pReservation->s_nFTID );
			break;
		case sRESERVATION::RV_Degenerate:
#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad2;	
			g_CsLastError.m_nIntValue1 = 11;
			Scene_Degenerate( pReservation->s_nFTID );
			//g_pCharMng->ChangeDigimonKFM( pReservation->s_nIDX, pReservation->s_nFTID );
			break;
		case sRESERVATION::RV_ChangePartner:
			assert_cs( pReservation->s_nIDX == GetIDX() );
			m_nNextFTID = pReservation->s_nFTID;
			_tcscpy_s( m_szNextName, Language::pLength::name + 1, pReservation->s_szNextName );
#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
			g_CsLastError.m_nIntValue1 = 1;	
			Scene_ChangePartner( pReservation->s_nFTID, pReservation->s_nTacticsIDX, pReservation->s_szNextName, pReservation->s_fScale, pReservation->s_nLevel );
			//g_pCharMng->ChangeDigimonModel( pReservation->s_nIDX, pReservation->s_nFTID, pReservation->s_szNextName, pReservation->s_fScale, pReservation->s_nLevel );
			break;
		case sRESERVATION::RV_Ride:
			{
				ChangeRideKFM( pReservation->s_nFTID, pReservation->s_dwChangeApplyModelID != 0 );						
			}			
			break;
		default:
			assert_cs( false );
		}
		
		m_ReservationStack.pop();
		SAFE_NIDELETE( pReservation );
		return CHECK_NOT_LOAD;
	}

	if( GetBaseStat() )
	{
		CheckLevelEffect();
		CheckEnchantEffect();
		CheckSizeEffect();
		CheckBattleEffect();

#ifdef  SDM_TAMER_XGUAGE_20180628
		CheckingXAntiEffect();
#endif
	}
	
	return ct;
}

void CDigimon::ChangeKFM( UINT nFTID )
{
	_ResetFTDigimon( nFTID );	

	PARENT_CLASS::ChangeKFM( nFTID );
}
#ifdef CROSSWARS_SYSTEM
void CDigimon::ChangeCrossModel( UINT nFTID, LPCTSTR szName, bool bUser )
{
	_ResetFTDigimon( nFTID );

	PARENT_CLASS::ChangeModel( nFTID, szName );

	m_Option.ResetName( szName );
}
void CDigimon::ChangeCrossKFM( UINT nFTID, bool bUser )
{
	_ResetFTDigimon( nFTID );	

	PARENT_CLASS::ChangeKFM( nFTID );
}
#endif

void CDigimon::ChangeModel( UINT nFTID, LPCTSTR szName )
{
	_ResetFTDigimon( nFTID );

	PARENT_CLASS::ChangeModel( nFTID, szName );

	m_Option.ResetName( szName );
}

bool CDigimon::ChangeRideKFM( UINT nRideID, bool bDoRide )
{
	if( IsLoad() == false )
	{
		sRESERVATION* pReservation = NiNew sRESERVATION;
		pReservation->s_eType = sRESERVATION::RV_Ride;
		pReservation->s_nIDX = GetIDX();
		pReservation->s_nFTID = nRideID;
		pReservation->s_dwChangeApplyModelID = 0;
		if( bDoRide )
			pReservation->s_dwChangeApplyModelID = nRideID;

		m_ReservationStack.push( pReservation );
		return false;
	}
	
	PARENT_CLASS::ChangeKFM( GetFTID() );

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File, nRideID );
	// 초기화 - Prop 초기화 포함
	pUint->s_Prop = m_CharProp;
	if( bDoRide == true )
		pUint->s_dwChangeApplyModelID = nRideID;

	pUint->s_pLoadedObject = (CsC_Object*)this;

	std::string strPath = g_pModelDataMng->GetKfmPath( nRideID );
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, strPath.c_str() );	

	// 우선 로드
	pUint->SetLoadType( sTCUnit::eFast );

	// 쓰레드 로드
	g_pThread->LoadChar( pUint );

	_SetDefault_IdleAnimation_Stand();

	return true;
}

void CDigimon::_Init( LPCTSTR szName )
{
	m_Option.Init( this, szName );

	m_eBackupDigimonState = m_eDigimonState = DIGIMON_NORMAL;
	m_eDigimonMove = DM_NONE;

	m_pCrossTempDigimon = NULL;
}


void CDigimon::CheckLevelEffect()
{
	if( g_pResist->m_Global.s_bViewLevelEffect == false )
	{
		GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_DLEVEL );
		return;
	}		

	if( GetBaseStat() )
	{
		int nLevel = GetBaseStat()->GetLevel();
		if( nLevel < 46 || GetFTDigimon()->GetInfo()->s_eEvolutionType == nsCsDigimonTable::ET_BUSTERMODE )
		{
			GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_DLEVEL );
		}
		else
		{
			if( nLevel < 51 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Green_L02.nif" );
			else if( nLevel < 56 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Sky_L01.nif" );
			else if( nLevel < 61 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Sky_L02.nif" );
			else if( nLevel < 65 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Blue_L01.nif" );
			else if( nLevel < 70 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Blue_L02.nif" );
			else if( nLevel < 75 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Pupple_L01.nif" );
			else if( nLevel < 80 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Pupple_L02.nif" );
			else
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\Red_L01.nif" );			
		}			
	}		
}

void CDigimon::CheckEnchantEffect()
{
	if( g_pResist->m_Global.s_bViewLevelEffect == false )
	{
		GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_ENCHANT );
		return;
	}	

	if( GetEnchantStat() )
	{
		int nEnchantLevel;
		int nOldLvCnt = 0; // 12강 능력치 카운트
		int nCurLvCnt = 0; // 15강 능력치 카운트

		m_pEnchentStat->GetEnchantLV_Info( nEnchantLevel, nOldLvCnt, nCurLvCnt );

		// 15강 능력치 카운트
		if( nCurLvCnt > 1 )
			GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_ENCHANT );

		switch( nCurLvCnt )
		{
		// 2개 능력치가 15강이 되었을 때
		case 2:	GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Red_K16.nif" );	return;
		// 3개 능력치가 15강이 되었을 때
		case 3:	GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Enchant_K45.nif" );	return;
		// 4개 능력치가 15강이 되었을 때
		case 4:	GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Enchant_K60.nif" );	return;
		default:	break;
		}

		// 12강 능력치 카운트
		switch( nOldLvCnt )
		{
		// 1개 능력치가 12강이 되었을 때
		case 1:	GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Pupple_K12.nif" );	return;
		// 2개 능력치가 12강이 되었을 때
		case 2:	GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Red_K15.nif" );		return;
		// 3개 능력치가 12강이 되었을 때
		case 3:	GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Red_K15.nif" );		return;
		// 4개 능력치가 12강이 되었을 때
		case 4:	GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Red_K15.nif" );		return;
		default:	break;
		}

		if( nEnchantLevel < 4 )
		{
			GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_ENCHANT );			
		}
		else
		{
			if( nEnchantLevel < 5 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Sky_K04.nif" );
			else if( nEnchantLevel < 6 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Sky_K05.nif" );
			else if( nEnchantLevel < 7 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Sky_K06.nif" );
			else if( nEnchantLevel < 8 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Blue_K07.nif" );
			else if( nEnchantLevel < 9 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Blue_K08.nif" );
			else if( nEnchantLevel < 10 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Blue_K09.nif" );
			else if( nEnchantLevel < 11 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Pupple_K10.nif" );
			else if( nEnchantLevel < 12 )
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Pupple_K11.nif" );
			else
				GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_ENCHANT, "system\\LevelEffect\\Pupple_K12.nif" );				
		}	
	}
}

void CDigimon::CheckSizeEffect()
{
	if( g_pResist->m_Global.s_bViewLevelEffect == false )
	{
		GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_SIZE );
		return;
	}

	float fScale = GetGameScale() * 100.0f;
	if( fScale < 110.0f )
	{
		GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_SIZE );
	}
	else
	{
		if( fScale < 115.0f )
			GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_SIZE, "system\\LevelEffect\\Blue_D01.nif" );
		else if( fScale < 119.99999f )
			GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_SIZE, "system\\LevelEffect\\Blue_D02.nif" );
		else if( fScale < 125.0f )
			GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_SIZE, "system\\LevelEffect\\Pupple_D03.nif" );
		else if( fScale < 130.00001f )
			GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_SIZE, "system\\LevelEffect\\Pupple_D04.nif" );
		else
			GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_SIZE, "system\\LevelEffect\\Pupple_D04_Trans01.nif" );
	}	
}



void CDigimon::CheckingXAntiEffect()
{
#ifdef SDM_TAMER_XGUAGE_20180628
	GetProp_Effect()->DeleteLoopEffect(CsC_EffectProp::LE_XJOGRESS);
	if( g_pResist->m_Global.s_bViewLevelEffect == TRUE )
	{
		if(m_pFTDigimon != null)
		{
			switch( m_pFTDigimon->GetInfo()->s_eEvolutionType )
			{
			case nsCsDigimonTable::ET_X_ROOKIE:
			case nsCsDigimonTable::ET_X_CHAMPION:
			case nsCsDigimonTable::ET_X_ULTIMATE:
			case nsCsDigimonTable::ET_X_MEGA:
			case nsCsDigimonTable::ET_X_BUSTERMODE:
			case nsCsDigimonTable::ET_X_JOGRESS:
				{
					//attach effect
					GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_XJOGRESS, "system\\Digimon_X-antiAura.nif" );
					//find Name of effect Anchor Node
					mToeName.clear();
					CDigimonEvolveObj* pDigiObj = nsCsFileTable::g_pEvolMng->GetEvolObj( GetFTID() );
					if(pDigiObj != NULL)
					{
						std::string kBoneName = pDigiObj->m_szEnchant;
						size_t iBipID = kBoneName.find("Bip");
						if(iBipID >= 0 ){
							char szBuff[256] = {0,};
							kBoneName.copy(szBuff, iBipID + 5);//ex bip01 or bip02 ...
							mToeName = szBuff;
							mToeName += " L Toe0";
						}
					}
				}
			break;
			}
		}
	}
#endif
}
void CDigimon::CheckBattleEffect()
{
	if( nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_BATTLE )
		return;

	std::pair<int,bool>	checkBattleTeam;
	checkBattleTeam.first = m_nBattleTeam;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_MY_BATTLE_TEAM, &checkBattleTeam );

	if( checkBattleTeam.second )
		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_B.nif" );
	else
		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_R.nif" );

// 	if( g_pDataMng->GetBattle()->IsObserver() == true )
// 	{
// 		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_G.nif" );
// 		return;
// 	}

// 	if( g_pDataMng->GetBattle()->IsMyTeam( m_nBattleTeam ) == true )
// 		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_B.nif" );
// 	else
// 		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_R.nif" );
}	

void CDigimon::Update( float fDeltaTime, bool bAnimation )
{
	// 버프 데이타 업데이트
	m_BuffData.Update();

	switch( m_eDigimonState )
	{
	case DIGIMON_EVOL:
		_UpdateEvolScene( fDeltaTime );
		break;
	case DIGIMON_EVOL_JOINTPROGRESS:
		_UpdateEvol_JointProgressScene( fDeltaTime );
		break;
	case DIGIMON_DEGENERATE:
		_UpdateDegenerateScene( fDeltaTime );
		break;
#ifdef CROSSWARS_SYSTEM
	case DIGIMON_CROSSCHANGE_PARTNER_START:
		_UpdateCrossChangePartnerStartScene( fDeltaTime );
		break;
	case DIGIMON_CROSSCHANGE_PARTNER_END:
		_UpdateCrossChangePartnerEndScene( fDeltaTime );
		break;

	case DIGIMON_CROSSCHANGE_PARTNER2_START:
		_UpdateCrossChangePartnerStartScene2( fDeltaTime );
		break;
	case DIGIMON_CROSSCHANGE_PARTNER2_END:
		_UpdateCrossChangePartnerEndScene2( fDeltaTime );
		break;

	case DIGIMON_CROSSRECOVER_START:
		_UpdateCrossRecoverStartScene( fDeltaTime );
		break;
	case DIGIMON_CROSSRECOVER_END:
		_UpdateCrossRecoverEndScene( fDeltaTime );
		break;
#endif
	case DIGIMON_CHANGE_PARTNER_START:
		_UpdateChangePartnerStartScene( fDeltaTime );
		break;
	case DIGIMON_CHANGE_PARTNER_END:
		_UpdateChangePartnerEndScene( fDeltaTime );
		break;
	}

	// 움직이고 있다면 움직이는 애니메이션으로 바꾸어 주자
	if( m_pProperty[ CsC_Property::PATH ] )
	{
		assert_cs( IsProperty( CsC_Property::PATH ) == true );
		if( ( GetProp_Path()->IsMove() == true )&&( IsPause( PAUSE_PATH ) == false ) )
		{
			_SetDefault_IdleAnimation_Move();		
		}	
	}

	_SetRideRot();
	_Update_Region();

	PARENT_CLASS::Update( fDeltaTime, bAnimation );		
}

void CDigimon::_Update_EffectPos()
{
	if( IsLoad() == true && g_pResist->m_Global.s_bViewLevelEffect == TRUE )
	{		
		CDigimonEvolveObj* pDigiObj = nsCsFileTable::g_pEvolMng->GetEvolObj( GetFTID() );
		// 레벨 이펙트 위치
		if( GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_DLEVEL ) != NULL ) 
		{			  
			NiAVObject* pPosLE = GetCsNode()->m_pNiNode->GetObjectByName( pDigiObj->m_szLevel );
			if( pPosLE != NULL )
			{
				GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_DLEVEL )->SetPos( pPosLE->GetWorldTranslate() );	
			}			
		}

		// 강화 이펙트 위치
		if( GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_ENCHANT ) != NULL ) 
		{			
			NiAVObject* pPosLE = GetCsNode()->m_pNiNode->GetObjectByName( pDigiObj->m_szEnchant );
			if( pPosLE != NULL )
			{
				GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_ENCHANT )->SetPos( pPosLE->GetWorldTranslate() );	
			}
		}
#ifdef SDM_TAMER_XGUAGE_20180628
		// X항체 이펙트 위치
		if( GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_XJOGRESS ) != NULL ) 
		{			
			NiAVObject* pPosLE = GetCsNode()->m_pNiNode->GetObjectByName( pDigiObj->m_szEnchant );
			NiAVObject* pToe = GetCsNode()->m_pNiNode->GetObjectByName(mToeName.c_str());
			NiPoint3 kFinalPos = NiPoint3::ZERO;
			if( pPosLE != NULL )
				kFinalPos = pPosLE->GetWorldTranslate();

			if( pToe == NULL )
			{
				assert_cs(false);
				}
			else
				kFinalPos.z = pToe->GetWorldTranslate().z;

			GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_XJOGRESS )->SetPos( kFinalPos );	
		}
#endif


		// 크기 이펙트 위치
		if( GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_SIZE ) != NULL ) 
		{			
			NiAVObject* pPosLE = GetCsNode()->m_pNiNode->GetObjectByName( pDigiObj->m_szSize );
			if( pPosLE != NULL )
			{
				GetProp_Effect()->GetLoopEffect( CsC_EffectProp::LE_SIZE )->SetPos( pPosLE->GetWorldTranslate() );	
			}
		}
	}
}

void CDigimon::_Update_Region()
{
	SAFE_POINTER_RET( nsCsGBTerrain::g_pCurRoot );
	int nMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pMapRegionMng->IsLimitEvoGroup( nMapID ) == false )
		return;

	bool bInLimitEvo = nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( nMapID )->IsInRegion( CsPoint( (int)m_vPos.x, (int)m_vPos.y ) );
	if( bInLimitEvo )
	{
		if( GetBuff()->IsBuffData( nsBuff::BK_DARKTOWER ) == false )
			GetBuff()->SetBuff( nsBuff::BK_DARKTOWER, 0, UINT_MAX, 0 );
		if( GetLeafRTTI() == RTTI_DIGIMON_USER)
		{
			if( ( IsRide() == true ) )
			{

				SAFE_POINTER_RET(g_pDataMng);
				cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
				SAFE_POINTER_RET(pServerSync);
				
				//캡슐 진화 디지몬은 설정상 다크타워 근처에서도 진화가 유지된다. 20170208
				if( m_pFTDigimon->GetInfo()->s_eEvolutionType != nsCsDigimonTable::ET_CAPSULE &&
					pServerSync->IsEmptyRefCount(cData_ServerSync::RIDE_OFF) )	// 한번만 라이딩 해제기능 보내도록 검사.
				{
					cPrintMsg::PrintMsg( 30016 );
					pServerSync->SendServer( cData_ServerSync::RIDE_OFF, 0 );
				}
			}
		}		
	}
	else
	{
		GetBuff()->ReleaseBuff( nsBuff::BK_DARKTOWER );
	}
}

void CDigimon::Render()
{
#ifdef GM_CLOCKING
	if( IsClocking() && GetLeafRTTI() != RTTI_DIGIMON_USER )
		return;
#endif
	PARENT_CLASS::Render();

	if(NULL != m_pCrossTempDigimon){
		CsNodeObj* pNode;
		pNode = m_pCrossTempDigimon->GetCsNode();
		if( pNode )
		{			
			m_pCrossTempDigimon->CsC_AvObject::Render();
		}	
	}
}

void CDigimon::Render_PostChar()
{
#ifdef GM_CLOCKING
	if( IsClocking() && GetLeafRTTI() != RTTI_DIGIMON_USER )
		return;
#endif
	if( IsRide() == true )
		return;

	if( GetProp_Alpha()->IsHideDistOut() == false )
	{
		m_Option.RenderName();
	}
}

//====================================================================================
//
//		Path
//
//====================================================================================
void CDigimon::DeletePath()
{
	bool bOldMove = GetProp_Path()->IsMove();

	PARENT_CLASS::DeletePath();

	if( bOldMove == true )
	{
		_SetDefault_IdleAnimation_Stand();
	}
}

//====================================================================================
//
//		Property
//
//====================================================================================
void CDigimon::_SetDefault_IdleAnimation_Stand()
{
	switch( m_eDigimonMove )
	{
	case DM_NONE:
		switch( m_eDigimonState )
		{	
		case DIGIMON_NORMAL:		
			GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
			break;
		case DIGIMON_BATTLE:
			GetProp_Animation()->SetAnimation( ANI::IDLE_ATTACK );
			break;
		case DIGIMON_DEGENERATE:
			{
				switch( m_eBackupDigimonState )
				{
				case DIGIMON_NORMAL:
					GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
					break;	
				case DIGIMON_BATTLE:
					GetProp_Animation()->SetAnimation( ANI::IDLE_ATTACK );
					break;
				default:
					assert_cs( false );
				}
			}
			break;
		case DIGIMON_DIE:
			break;		
		}
		break;

	case DM_RIDE:
		{			
			CTamer* pTamer = g_pCharMng->GetTamer( GetTamerLink() );
			if( pTamer )
			{
				pTamer->SetPause( PAUSE_ANIMATION, false );
				switch( m_pFTRide->GetInfo()->s_nRideType )
				{
				case 1:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_1, 1.0f/GetGameScale() );		break;
				case 2:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_2, 1.0f/GetGameScale() );		break;
				case 3:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_3, 1.0f/GetGameScale() );		break;
				case 4:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_4, 1.0f/GetGameScale() );		break;
				case 5:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_5, 1.0f/GetGameScale() );		break;
				case 6:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_6, 1.0f/GetGameScale() );		break;
				case 7:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_7, 1.0f/GetGameScale() );		break;
				case 8:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_8, 1.0f/GetGameScale() );		break;
				case 9:		pTamer->SetAnimation( ANI::IDLE_TAMER_RIDE_9, 1.0f/GetGameScale() );		break;
				default:	assert_cs( false );
				}
				pTamer->SetPause( PAUSE_ANIMATION, true );
			}
			GetProp_Animation()->SetAnimation( ANI::IDLE_DIGIMON_RIDE );
		}
		break;
	default:
		assert_cs( false );
	}
	
}

void CDigimon::_SetDefault_IdleAnimation_Move()
{
	// 데미지 애니메이션 중일떄는 움직이는 애니메이션 잠시 패스
	if( GetProp_Animation()->IsDamage() == true )
		return;

	switch( m_eDigimonMove )
	{
	case DM_NONE:
		{
			switch( m_eDigimonState )
			{
			case DIGIMON_NORMAL:
				GetProp_Animation()->SetAnimation( ANI::MOVE_RUN );
				break;	
			case DIGIMON_BATTLE:
				GetProp_Animation()->SetAnimation( ANI::MOVE_ATTACKRUN );
				break;
			case DIGIMON_DEGENERATE:
				{
					switch( m_eBackupDigimonState )
					{
					case DIGIMON_NORMAL:
						GetProp_Animation()->SetAnimation( ANI::MOVE_RUN );
						break;	
					case DIGIMON_BATTLE:
						GetProp_Animation()->SetAnimation( ANI::MOVE_ATTACKRUN );
						break;
					default:
						assert_cs( false );
					}
				}
				break;
			case DIGIMON_DIE:
				break;			
			}
		}
		break;

	case DM_RIDE:
		{
			CTamer* pTamer = g_pCharMng->GetTamer( GetTamerLink() );			
			
			if( pTamer )
			{
				pTamer->SetPause( PAUSE_ANIMATION, false );
				switch( m_pFTRide->GetInfo()->s_nRideType )
				{
				case 1:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_1, 1.0f/GetGameScale() );		break;
				case 2:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_2, 1.0f/GetGameScale() );		break;
				case 3:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_3, 1.0f/GetGameScale() );		break;
				case 4:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_4, 1.0f/GetGameScale() );		break;
				case 5:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_5, 1.0f/GetGameScale() );		break;
				case 6:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_6, 1.0f/GetGameScale() );		break;
				case 7:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_7, 1.0f/GetGameScale() );		break;
				case 8:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_8, 1.0f/GetGameScale() );		break;
				case 9:		pTamer->SetAnimation( ANI::MOVE_TAMER_RIDE_9, 1.0f/GetGameScale() );		break;
				default:	assert_cs( false );
				}
				pTamer->SetPause( PAUSE_ANIMATION, true );
			}

			GetProp_Animation()->SetAnimation( ANI::MOVE_DIGIMON_RIDE );			
		}
		break;
	default:
		assert_cs( false );
	}
}



//====================================================================================
//
//		상태
//
//====================================================================================
void CDigimon::SetDigimonState( eDIGIMON_STATE state )
{
	if( m_eDigimonState == state )
		return;

	switch( m_eDigimonState )
	{
	case DIGIMON_EVOL:
	case DIGIMON_EVOL_JOINTPROGRESS:
	case DIGIMON_DEGENERATE:
#ifdef CROSSWARS_SYSTEM
	case DIGIMON_CROSSCHANGE_PARTNER_START:
	case DIGIMON_CROSSCHANGE_PARTNER_END:
	case DIGIMON_CROSSCHANGE_PARTNER2_START:
	case DIGIMON_CROSSCHANGE_PARTNER2_END:
	case DIGIMON_CROSSRECOVER_START:
	case DIGIMON_CROSSRECOVER_END:
#endif
	case DIGIMON_CHANGE_PARTNER_START:
	case DIGIMON_CHANGE_PARTNER_END:
		break;
	case DIGIMON_BATTLE:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGIMON_BATTLE_OFF, this );
		}
	default:
		m_eDigimonState = state;
	}

	switch( state )
	{
	case DIGIMON_EVOL:
	case DIGIMON_EVOL_JOINTPROGRESS:
	case DIGIMON_DEGENERATE:
#ifdef CROSSWARS_SYSTEM
	case DIGIMON_CROSSCHANGE_PARTNER_START:
	case DIGIMON_CROSSCHANGE_PARTNER_END:
	case DIGIMON_CROSSCHANGE_PARTNER2_START:
	case DIGIMON_CROSSCHANGE_PARTNER2_END:
	case DIGIMON_CROSSRECOVER_START:
	case DIGIMON_CROSSRECOVER_END:
#endif
	case DIGIMON_CHANGE_PARTNER_START:
	case DIGIMON_CHANGE_PARTNER_END:
		break;
	case DIGIMON_BATTLE:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGIMON_BATTLE_ON, this );
		}
	default:
		m_eBackupDigimonState = state;
	}
}

CDigimon::eDIGIMON_STATE CDigimon::GetDigimonState()
{ 
	return m_eDigimonState;
}
CsDigimon* CDigimon::GetFTDigimon()
{ 
	return m_pFTDigimon;
}	

DWORD	CDigimon::GetBaseDigimonFTID() const
{ 
	return 0;
}

void CDigimon::SetBattle()
{
	SetDigimonState( DIGIMON_BATTLE );
	SetDefault_IdleAnimation();
}

void CDigimon::SetBattleEnd()
{
	if( m_eDigimonState == DIGIMON_DIE )
		return;

	SetDigimonState( DIGIMON_NORMAL );
	SetDefault_IdleAnimation();
}


void CDigimon::SetDie()
{
	PARENT_CLASS::SetDie();

	m_BuffData.Delete();
	SetDigimonState( DIGIMON_DIE );
	SetPause( PAUSE_ANIMATION, false );
	SetAnimation( ANI::DAMAGE_DEAD );
	SetPauseAll( true );
}

bool CDigimon::IsBattle()
{ 
	return ( m_eDigimonState == DIGIMON_BATTLE );
}
bool CDigimon::IsDie()
{ 
	return ( m_eDigimonState == DIGIMON_DIE );
}

bool CDigimon::IsEvolDegener()
{ 
	return ( m_eDigimonState == DIGIMON_EVOL )||( m_eDigimonState == DIGIMON_DEGENERATE ) || ( m_eDigimonState == DIGIMON_EVOL_JOINTPROGRESS );
}

void CDigimon::SetMarble( DWORD mt, CsC_AvObject* pFollow, int nCnt )
{
	CEffectMng::eMARBLE_TYPE c;
	switch( mt )
	{
	case CsC_AttackProp::MB_EVOL:
		c = CEffectMng::MT_EVOLUTION;
		break;
	default:
		assert_cs( false );
	}

	g_pEffectMng->AddEffect_Marble( c, GetCenter(), pFollow, nCnt );
}
#ifdef GM_CLOCKING
bool CDigimon::IsClocking()
{ 
	return m_bIsClocking;
}
void CDigimon::SetClocking( bool bClocking )
{
	m_bIsClocking = bClocking;
}
#endif





void CDigimon::_ResetFTDigimon( uint nFileTableID )
{

	m_pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nFileTableID );
#ifdef COMPAT_487_2
	if (!m_pFTDigimon) {
		return;
	}
#endif
	m_eNatureType = m_pFTDigimon->GetInfo()->s_eBaseNatureType;
	memcpy(&m_eNatureTypes, m_pFTDigimon->GetInfo()->s_eBaseNatureTypes, sizeof(nsCsDigimonTable::eNATURE_TYPE)*3);

	g_pCharResMng->CharImageResDelete( &m_pCharImage );
	m_pCharImage = g_pCharResMng->CharImageResLoad( GetModelID() );

	// 파일 테이블 로드	
	m_SkillMng.Delete();
	m_SkillMng.SetDigimon( this, m_pFTDigimon );
	SetReservationSkillCoolTime(nFileTableID);
}

bool CDigimon::GetBuffSkillFlag() 
{ 
	return m_bSkillFlag;
}
void CDigimon::SetBuffSkillFlag( bool bSuccess ) 
{
	m_bSkillFlag = bSuccess;
}

//	Tamer Link
void CDigimon::SetTamerLink( int nTamerIDX )
{ 
	m_nTamerLinkID = nTamerIDX;
}
int CDigimon::GetTamerLink()
{
	return m_nTamerLinkID;
}
//====================================================================================
//
//		TamerLink
//
//====================================================================================

void CDigimon::SetEvent( int nType )
{
	switch( nType )			// 사용 아이템 이벤트
	{
	case 10000000:
		break;
	default:
		assert_csm1( false, _T( "지정되지 않은 이벤트 수신 : %d " ), nType );
		break;
	}
}

//====================================================================================
//
//		씬
//
//====================================================================================

void CDigimon::SceneLevelUp( int nLevel )
{
	PARENT_CLASS::SceneLevelUp( nLevel );

	PlaySound( SOUND_LEVEL_UP );
	DWORD dwPlag = nsEFFECT::POS_CHARPOS;
#ifdef LEVELUP_EFFECT_LOOP
	GetProp_Effect()->AddEffect( EFFECT_DIGIMON_LEVELUP, GetToolWidth()*0.02f, dwPlag, CsC_EffectObject::eLeveUp );
#else
	GetProp_Effect()->AddEffect( EFFECT_DIGIMON_LEVELUP, GetToolWidth()*0.02f, dwPlag );
#endif

	CheckLevelEffect();
}

bool CDigimon::Scene_Evol( UINT nNextFTID, bool bAbsoluteEvolution )
{
	// 전투 중지
	if( m_pProperty[ CsC_Property::ATTACK ] )
	{
		GetProp_Attack()->ReleaseAttackingTarget();
		GetProp_Attack()->SetHitTarget( NULL );
	}

	if( IsLoad() == false )
	{
		sRESERVATION* pReservation = NiNew sRESERVATION;
		pReservation->s_eType = sRESERVATION::RV_Evol;
		pReservation->s_nIDX = GetIDX();
		pReservation->s_nFTID = nNextFTID;
		pReservation->s_nLevel = GetBaseStat()->GetLevel();
		m_ReservationStack.push( pReservation );
		return false;
	}

	m_nNextFTID = nNextFTID;

	CDigimonEvolveObj* pFTEvolObj = nsCsFileTable::g_pEvolMng->GetEvolObj( nNextFTID );
	if( pFTEvolObj->m_nChipsetType != 0 && ( pFTEvolObj->m_nOpenQuest != 0 || pFTEvolObj->m_nEvolutionTree == CDigimonUser::TREE_JOGRESS ) ) 
	{		
		SetDigimonState( DIGIMON_EVOL_JOINTPROGRESS );	// 상태값 "조그레스 진화" 로 변경
		SetAnimation( ANI::DIGIMON_EVOLUTION_DOWN, 0.5f );	// 진화 에니 시작 901703 번 버로우 에니		
	}
	else
	{
		SetDigimonState( DIGIMON_EVOL );	// 상태값 "진화" 로 변경	_UpdateEvolScene() 에서 이펙트와 진화 에니메이션 update 처리
		SetAnimation( ANI::DIGIMON_EVOLUTION, 0.5f );	// 진화 에니 시작 901701 번 상승 에니
	}	
	
	SetPause( PAUSE_ANIMATION, false );	// 에니메이션 중지		
	PlaySound( SOUND_EVOLUTION );	// 진화 사운드
	SetPauseAll( true );	// 모두중지?

	m_nEvolStep = 0;		// 진화 에니 단계 0
	m_fEvolTime = 0.0f;		// 진화 시간 0

	PlayAniSound( ANI::DIGIMON_EVOLUTION );		// 진화 사운드 ?
	return true;
}

bool CDigimon::Scene_Degenerate( UINT nNextFTID )
{
	// 전투 중지
	GetProp_Attack()->ReleaseAttackingTarget();
	GetProp_Attack()->SetHitTarget( NULL );

	if( IsLoad() == false )
	{
		sRESERVATION* pReservation = NiNew sRESERVATION;
		pReservation->s_eType = sRESERVATION::RV_Degenerate;
		pReservation->s_nIDX = GetIDX();
		pReservation->s_nFTID = nNextFTID;
		pReservation->s_nLevel = GetBaseStat()->GetLevel();
		m_ReservationStack.push( pReservation );
		return false;
	}

	m_nNextFTID = nNextFTID;

	SetDigimonState( DIGIMON_DEGENERATE );

	SetPause( PAUSE_ANIMATION, false );
	
	//GetProp_Effect()->AddEffect( EFFECT_DEGENERATION, sqrt( GetToolWidth()*0.01f ), 0 );
	g_pEffectMng->AddEffect( EFFECT_DEGENERATION, GetPos(), sqrt( GetToolWidth()*0.01f ) );

	GetProp_Alpha()->SetDestAlphaTime( 0.4f );
	GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
	PlaySound( SOUND_DEGENERATION );

	m_nEvolStep = 0;
	m_fEvolTime = 0.0f;

	return true;
}
#ifdef CROSSWARS_SYSTEM
bool CDigimon::Scene_CrossChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
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
		return false;
	}

	m_nNextFTID = nNextFTID;
	m_fNextScale = fNextScale;
	m_nNextLevel = nNextLevel;

	_tcscpy_s( m_szNextName, Language::pLength::name + 1, szNextName );

	SetDigimonState( DIGIMON_CROSSCHANGE_PARTNER_START );
	SetPause( PAUSE_ANIMATION, false );

	SetAnimation( ANI::DIGIMON_EVOLUTION );
	g_pEffectMng->AddEffect( EFFECT_TACTICS, GetPos(), sqrt( GetToolWidth()*0.02f ) );
	PlaySound( SOUND_EVOLUTION );

	SetPauseAll( true );

	m_nEvolStep = 0;
	m_fEvolTime = 0.0f;

	return true;
}
bool CDigimon::Scene_CrossChangePartner2( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
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
		return false;
	}

	m_nNextFTID = nNextFTID;
	m_fNextScale = fNextScale;
	m_nNextLevel = nNextLevel;

	_tcscpy_s( m_szNextName, Language::pLength::name + 1, szNextName );

	SetDigimonState( DIGIMON_CROSSCHANGE_PARTNER2_START );
	SetPause( PAUSE_ANIMATION, false );

	SetAnimation( ANI::DIGIMON_EVOLUTION );
	g_pEffectMng->AddEffect( EFFECT_TACTICS, GetPos(), sqrt( GetToolWidth()*0.02f ) );
	PlaySound( SOUND_EVOLUTION );

	SetPauseAll( true );

	m_nEvolStep = 0;
	m_fEvolTime = 0.0f;

	return true;
}
bool CDigimon::Scene_CrossRecoverPartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
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
		return false;
	}

	m_nNextFTID = nNextFTID;
	m_fNextScale = fNextScale;
	m_nNextLevel = nNextLevel;

	_tcscpy_s( m_szNextName, Language::pLength::name + 1, szNextName );

	SetDigimonState( DIGIMON_CROSSRECOVER_START );
	SetPause( PAUSE_ANIMATION, false );

	SetAnimation( ANI::DIGIMON_EVOLUTION );
	g_pEffectMng->AddEffect( EFFECT_TACTICS, GetPos(), sqrt( GetToolWidth()*0.02f ) );
	PlaySound( SOUND_EVOLUTION );

	SetPauseAll( true );

	m_nEvolStep = 0;
	m_fEvolTime = 0.0f;

	return true;
}
#endif
bool CDigimon::Scene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
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
		return false;
	}

	m_nNextFTID = nNextFTID;
	m_fNextScale = fNextScale;
	m_nNextLevel = nNextLevel;

	_tcscpy_s( m_szNextName, Language::pLength::name + 1, szNextName );

	SetDigimonState( DIGIMON_CHANGE_PARTNER_START );
	SetPause( PAUSE_ANIMATION, false );

	SetAnimation( ANI::DIGIMON_EVOLUTION );

	if( nsCsFileTable::g_pDigimonMng )
	{
		CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nNextFTID );
		if( pDigimon )
		{
			CsDigimon::sINFO* pInfo = pDigimon->GetInfo();
			if( pInfo )
			{
				char szDir[ MAX_FILENAME ] = {0, };
				W2M( szDir, pInfo->s_szEvoEffectDir, MAX_FILENAME );
				if( strlen( szDir ) > 0 )
					g_pEffectMng->AddEffect( szDir, GetPos(), sqrt( GetToolWidth()*0.02f ) );
			}
		}
	}

	PlaySound( SOUND_EVOLUTION );

	SetPauseAll( true );

	m_nEvolStep = 0;
	m_fEvolTime = 0.0f;

	return true;
}

bool CDigimon::WithoutScene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel )
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
		return false;
	}

	m_nNextFTID = nNextFTID;
	m_fNextScale = fNextScale;
	m_nNextLevel = nNextLevel;

	_tcscpy_s( m_szNextName, Language::pLength::name + 1, szNextName );	

	return true;
}

bool CDigimon::_UpdateEvol_JointProgressScene( float fDeltaTime )
{

#define EVOL_UPDATE_START_TIME		1.0f
#define EVOL_UPDATE_MIDDLE_TIME		2.7f
#define JOINTPROGRESS_EVOL_UPDATE_DELAY_TIME		0.8f

	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:		
		{
			g_pEffectMng->AddEffect( EFFECT_EVOL, GetPos(), sqrt( GetToolWidth()*0.02f ) );			
			SetPause( PAUSE_ANIMATION, false );
			m_nEvolStep = 1;
		}
		break;
	case 1:
		if( m_fEvolTime > EVOL_UPDATE_START_TIME )
		{
			m_nEvolStep = 2;			
			m_fEvolTime -= EVOL_UPDATE_START_TIME;
			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );	// 이전에 진화 에니 나오고 이때쯤 사라진다.
		}		
		break;
	case 2:		
		if( m_fEvolTime > EVOL_UPDATE_START_TIME )
		{	
#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad2;	
			g_CsLastError.m_nIntValue1 = 12;				
			g_pCharMng->ChangeDigimonKFM( GetIDX(), m_nNextFTID, &m_PostParts);
			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );

			//어짜피 여기는 조그레스만 들어옴
			switch( m_nNextFTID )
			{
			case CODE_EXAMON:		GetProp_Effect()->AddEffect_FT( EFFECT_EVOL_EXAMON );		break; // 엑자몬
			case CODE_SUSANOOMON:	GetProp_Effect()->AddEffect_FT_WithoutScale( EFFECT_EVOL_SUSANOOMON, sqrt( GetToolWidth()*0.02f) ); break;
			default: GetProp_Effect()->AddEffect_FT_WithoutScale( EFFECT_EVOL_JOINTPROGRESS, sqrt( GetToolWidth()*0.02f) );	break;
			}
			m_nEvolStep = 3;
		}
		break;

	case 3:
		if( m_fEvolTime > EVOL_UPDATE_START_TIME + EVOL_UPDATE_MIDDLE_TIME )
		{
			m_nEvolStep = 4;			
			m_fEvolTime -= EVOL_UPDATE_START_TIME + EVOL_UPDATE_MIDDLE_TIME;

			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_INCREASE );
/*			
#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad2;	
			g_CsLastError.m_nIntValue1 = 12;				
			g_pCharMng->ChangeDigimonKFM( GetIDX(), m_nNextFTID );
*/			
		
		}		
		break;
	
	case 4:
		if( m_fEvolTime > JOINTPROGRESS_EVOL_UPDATE_DELAY_TIME*2.0f )
		{			
			PlayAniSound( ANI::DIGIMON_EVOLUTION_END ); 
			SetPauseAll( false );
			m_eDigimonState = m_eBackupDigimonState;

			// 움직이는 중이 아니라면
			if( GetProp_Path()->IsMove() == false )
			{
				SetAnimation( ANI::IDLE_CHAR );
			}	
			return true;
		}
		break;
	}

	return false;

#undef EVOL_UPDATE_START_TIME
#undef EVOL_UPDATE_MIDDLE_TIME
#undef JOINTPROGRESS_EVOL_UPDATE_DELAY_TIME
}

bool CDigimon::_UpdateEvolScene( float fDeltaTime )
{
#define EVOL_UPDATE_START_TIME		1.0f

	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > EVOL_UPDATE_START_TIME )
		{
			// 진화 이펙트
			//캡슐진화체는 각 캡슐별 다른 이펙트
			u4 nEvolTree = nsCsFileTable::g_pEvolMng->GetEvolObj( m_nNextFTID )->m_nEvolutionTree;

			if( nEvolTree == CDigimonUser::TREE_CAPSULE )
			{
				u4 nItemSType = nsCsFileTable::g_pEvolMng->GetEvolObj( m_nNextFTID )->m_nOpenItemTypeS;
				DWORD dwDispID = nsCsFileTable::g_pItemMng->TypeT_to_Disp( nItemSType );
				
				char cEffectPath[ITEM_FILE_LEN];
				sprintf_s( cEffectPath, ITEM_FILE_LEN, "System\\CapsuleEvo\\%s.nif", nsCsFileTable::g_pItemMng->GetItem( dwDispID )->GetInfo()->s_cNif );

 				g_pEffectMng->AddEffect( cEffectPath, GetPos(), sqrt( GetToolWidth()*0.02f ) );
			}
			else
			{
				switch( m_nNextFTID )
				{
				case CODE_AGUMON_BUST: 
					g_pEffectMng->AddEffect( "Shinegrey_BM/AgumonBurstEffect.nif", GetPos(), sqrt( GetToolWidth()*0.02f ) );
					break;
				case CODE_LALAMON_BUST:
					g_pEffectMng->AddEffect( "RosemonBM/LaLamonBurstEffect.nif", GetPos(), sqrt( GetToolWidth()*0.02f ) );
					break;
				case CODE_GAOMON_BUST:
					g_pEffectMng->AddEffect( "Mirage_BM/GaomonBurstEffect.nif", GetPos(), sqrt( GetToolWidth()*0.02f ) );
					break;
				case CODE_FALCOMON_BUST:
					g_pEffectMng->AddEffect( "Ravemon_BM/PalcomonBurstEffect.nif", GetPos(), sqrt( GetToolWidth()*0.02f ) );
					break;			
				default:
					g_pEffectMng->AddEffect( EFFECT_EVOL, GetPos(), sqrt( GetToolWidth()*0.02f ) );
					break;
				}
			}
			m_nEvolStep = 1;
		}
		break;
	case 1:
		if( m_fEvolTime > EVOL_UPDATE_START_TIME )
		{
			m_nEvolStep = 2;
			m_fEvolTime -= EVOL_UPDATE_START_TIME;

			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
		}		
		break;
	case 2:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME )
		{
			m_nEvolStep = 3;
			m_fEvolTime -= COMMON_EVOL_UPDATE_DELAY_TIME;	

#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad2;	
			g_CsLastError.m_nIntValue1 = 12;				
			g_pCharMng->ChangeDigimonKFM( GetIDX(), m_nNextFTID, &m_PostParts);
			SetPause( PAUSE_ANIMATION, false );
			SetAnimation( ANI::DIGIMON_EVOLUTION_END );
			SetPause( PAUSE_ANIMATION, true );
		}
		break;
	case 3:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME*2.0f )
		{
			PlayAniSound( ANI::DIGIMON_EVOLUTION_END );
			SetPauseAll( false );
			m_eDigimonState = m_eBackupDigimonState;

			// 움직이는 중이 아니라면
			if( GetProp_Path()->IsMove() == false )
			{
				SetAnimation( ANI::IDLE_CHAR );
			}
			return true;
		}
		break;
	}

	return false;

#undef EVOL_UPDATE_START_TIME
}

bool CDigimon::_UpdateDegenerateScene( float fDeltaTime )
{
#define EVOL_UPDATE_DELAY_TIME		0.4f

	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > EVOL_UPDATE_DELAY_TIME )
		{
			SetPauseAll( false );

			m_fEvolTime -= EVOL_UPDATE_DELAY_TIME;

			// 남상배 멋져
			/*
			if( IsLoad() == false )
			{
				sRESERVATION* pReservation = NiNew sRESERVATION;
				pReservation->s_eType = sRESERVATION::RV_Degenerate;
				pReservation->s_nIDX = GetIDX();
				pReservation->s_nFTID = m_nNextFTID;
				pReservation->s_nLevel = GetBaseStat()->GetLevel();
				m_ReservationStack.push( pReservation );
				return true;
			}
			*/

#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad2;	
			g_CsLastError.m_nIntValue1 = 13;				
			g_pCharMng->ChangeDigimonKFM( GetIDX(), m_nNextFTID, &m_PostParts);
			GetProp_Alpha()->SetDestAlphaTime( CHAR_DEST_ALPHA_TIME );
			m_eDigimonState = m_eBackupDigimonState;
			SetDefault_IdleAnimation();
			return true;
		}
		break;
	}
	return false;
}

#ifdef CROSSWARS_SYSTEM
bool CDigimon::_UpdateCrossChangePartnerStartScene( float fDeltaTime )
{
#define CP_UPDATE_START_TIME		1.0f

	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > CP_UPDATE_START_TIME )
		{
			m_nEvolStep = 1;
			m_fEvolTime -= CP_UPDATE_START_TIME;

			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
		}		
		break;
	case 1:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME )
		{
			m_nEvolStep = 2;
			m_fEvolTime -= COMMON_EVOL_UPDATE_DELAY_TIME;

#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
			g_CsLastError.m_nIntValue1 = 2;	
			g_pCharMng->ChangeCrossDigimonModel( GetIDX(), m_nNextFTID, NULL, m_szNextName, m_fNextScale, m_nNextLevel );

			m_eDigimonState = CDigimon::DIGIMON_CROSSCHANGE_PARTNER_END;

			SetPause( PAUSE_ANIMATION, false );
			SetAnimation( ANI::DIGIMON_EVOLUTION_END );
			SetPauseAll( true );
			m_nEvolStep = 0;
			m_fEvolTime = 0.0f;
			return true;
		}
		break;
	}
	return false;

#undef CP_UPDATE_START_TIME
}
bool CDigimon::_UpdateCrossChangePartnerEndScene( float fDeltaTime )
{
	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME*2.0f )
		{
			SetPauseAll( false );
			m_eDigimonState = m_eBackupDigimonState;

			// 움직이는 중이 아니라면
			if( GetProp_Path()->IsMove() == false )
			{
				SetAnimation( ANI::IDLE_CHAR );
			}
			else
			{
				SetDefault_IdleAnimation();
			}
			return true;
		}
		break;
	}
	return false;
}

bool CDigimon::_UpdateCrossChangePartnerStartScene2( float fDeltaTime )
{
#define CP_UPDATE_START_TIME		1.0f
#define CP_UPDATE_MIDDLE_TIME		2.7f
#define CP_UPDATE_DELAY_TIME		0.8f

	m_fEvolTime += fDeltaTime;

	if( NULL != m_pCrossTempDigimon  ){

		m_pCrossTempDigimon->CsC_AvObject::Update( g_fDeltaTime );			

	}

	switch( m_nEvolStep )
	{
		// 전 디지몬이 공중에 떴을때.
	case 0:
		if( m_fEvolTime > CP_UPDATE_START_TIME )
		{
			m_nEvolStep = 1;
			m_fEvolTime -= CP_UPDATE_START_TIME;

			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
			GetProp_Effect()->Reset();
		}		
		break;
		// 크로스합체체가 나타나 대기애니		
	case 1:
		if( m_fEvolTime > CP_UPDATE_START_TIME)
		{
			m_nEvolStep = 2;				
#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
			g_CsLastError.m_nIntValue1 = 2;	

			sCREATEINFO prop;
			prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
			prop.s_dwOptionPlag = OP_LIGHT;
			prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

			CsCrossDigimon::sINFO* Info = nsCsFileTable::g_pDigimonMng->GetCrossCombine(m_nNextFTID)->GetInfo();

			for(int i = 0; i < Info->s_nCrossRank; i++)
			{
				if(true == nsCsFileTable::g_pDigimonMng->IsCrossDigimon(Info->s_nCrossDigimon[i]))
				{
					CsCrossDigimon::sINFO* Info2 = nsCsFileTable::g_pDigimonMng->GetCrossDigimon(Info->s_nCrossDigimon[i])->GetInfo();
					if(2 == Info2->s_nCrossType)
					{
						m_pCrossTempDigimon = g_pCharMng->AddTempDigimon( &prop, Info2->s_dwDigimonID, NULL, GetPos(), GetCurRot(), NULL );
						m_pCrossTempDigimon->GetProp_Effect()->AddEffect( EFFECT_CROSSCOMBINE, GetPos(), sqrt( GetToolWidth()*0.02f ) );
						break;
					}
				}
			}

			

			//g_pCharMng->ChangeCrossDigimonModel( GetIDX(), 33001, m_szNextName, m_fNextScale, m_nNextLevel, false );
			//g_pCharMng->ChangeCrossDigimonKFM( GetIDX(), 33001, false );
			//g_pEffectMng->AddEffect( EFFECT_CROSSCOMBINE, GetPos(), sqrt( GetToolWidth()*0.02f ) );

			
		}
		break;
		
		// 크로스 합체 이펙트
	case 2:
		if( m_fEvolTime > CP_UPDATE_START_TIME + CP_UPDATE_MIDDLE_TIME )
		{
			m_nEvolStep = 3;
			//m_fEvolTime -= CP_UPDATE_START_TIME + CP_UPDATE_MIDDLE_TIME;			
	
		}
		break;	
		
		// 크로스 완성 이펙트 // 완료 애니
	case 3:
		if( m_fEvolTime > CP_UPDATE_DELAY_TIME )
		{
			m_nEvolStep = 4;
	
#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
			g_CsLastError.m_nIntValue1 = 2;	

			if( m_pCrossTempDigimon )
			{
				assert_cs( g_pCharMng );
				g_pCharMng->DeleteTempChar( m_pCrossTempDigimon );
				m_pCrossTempDigimon = NULL;
			}	

			g_pCharMng->ChangeCrossDigimonModel( GetIDX(), m_nNextFTID, NULL, m_szNextName, m_fNextScale, m_nNextLevel );

			g_pEffectMng->AddEffect( EFFECT_CROSSCOMBINE_COMPLATE, GetPos(), sqrt( GetToolWidth()*0.02f ) );

			m_eDigimonState = CDigimon::DIGIMON_CROSSCHANGE_PARTNER2_END;
			SetPause( PAUSE_ANIMATION, false );
			SetAnimation( ANI::DIGIMON_EVOLUTION_END );
			SetPause( PAUSE_ANIMATION, true );
			m_nEvolStep = 0;
			m_fEvolTime = 0.0f;

			SetPauseAll( false );
			// 움직이는 중이 아니라면
			if( GetProp_Path()->IsMove() == false )
			{
				SetAnimation( ANI::IDLE_CHAR );
			}	

			return true;
		}
	
	
		break;
	}
	return false;

#undef CP_UPDATE_START_TIME
#undef CP_UPDATE_MIDDLE_TIME
#undef CP_UPDATE_DELAY_TIME
}
bool CDigimon::_UpdateCrossChangePartnerEndScene2( float fDeltaTime )
{
	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME*2.5f )
		{
			SetPauseAll( false );
			m_eDigimonState = m_eBackupDigimonState;			
	
			// 움직이는 중이 아니라면
			if( GetProp_Path()->IsMove() == false )
			{
				SetAnimation( ANI::IDLE_CHAR );
			}
			else
			{
				SetDefault_IdleAnimation();
			}
			return true;
		}
		break;
	}
	return false;
}

bool CDigimon::_UpdateCrossRecoverStartScene( float fDeltaTime )
{
#define CP_UPDATE_START_TIME		1.0f

	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > CP_UPDATE_START_TIME )
		{
			m_nEvolStep = 1;
			m_fEvolTime -= CP_UPDATE_START_TIME;

			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
		}		
		break;
	case 1:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME )
		{
			m_nEvolStep = 2;
			m_fEvolTime -= COMMON_EVOL_UPDATE_DELAY_TIME;

#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
			g_CsLastError.m_nIntValue1 = 2;	
			g_pCharMng->ChangeCrossDigimonModel( GetIDX(), m_nNextFTID, NULL, m_szNextName, m_fNextScale, m_nNextLevel );

			m_eDigimonState = CDigimon::DIGIMON_CROSSRECOVER_END;

			SetPause( PAUSE_ANIMATION, false );
			SetAnimation( ANI::DIGIMON_EVOLUTION_END );
			SetPauseAll( true );
			m_nEvolStep = 0;
			m_fEvolTime = 0.0f;
			return true;
		}
		break;
	}
	return false;

#undef CP_UPDATE_START_TIME
}
bool CDigimon::_UpdateCrossRecoverEndScene( float fDeltaTime )
{
	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME*2.0f )
		{
			SetPauseAll( false );
			m_eDigimonState = m_eBackupDigimonState;

			// 움직이는 중이 아니라면
			if( GetProp_Path()->IsMove() == false )
			{
				SetAnimation( ANI::IDLE_CHAR );
			}
			else
			{
				SetDefault_IdleAnimation();
			}
			return true;
		}
		break;
	}
	return false;
}
#endif
bool CDigimon::_UpdateChangePartnerStartScene( float fDeltaTime )
{
#define CP_UPDATE_START_TIME		1.0f

	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > CP_UPDATE_START_TIME )
		{
			m_nEvolStep = 1;
			m_fEvolTime -= CP_UPDATE_START_TIME;

			GetProp_Alpha()->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
		}		
		break;
	case 1:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME )
		{
			m_nEvolStep = 2;
			m_fEvolTime -= COMMON_EVOL_UPDATE_DELAY_TIME;

#define CSLAST_ERROR
			g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
			g_CsLastError.m_nIntValue1 = 2;	
			g_pCharMng->ChangeDigimonModel( GetIDX(), m_nNextFTID, NULL, m_szNextName, m_fNextScale, m_nNextLevel );

			m_eDigimonState = CDigimon::DIGIMON_CHANGE_PARTNER_END;

			SetPause( PAUSE_ANIMATION, false );
			SetAnimation( ANI::DIGIMON_EVOLUTION_END );
			SetPauseAll( true );
			m_nEvolStep = 0;
			m_fEvolTime = 0.0f;
			return true;
		}
		break;
	}
	return false;

#undef CP_UPDATE_START_TIME
}

bool CDigimon::_WithoutUpdate_ChangePartner()
{
#define CSLAST_ERROR
	g_CsLastError.m_eType = CsLastError::__ThreadLoad1;	
	g_CsLastError.m_nIntValue1 = 2;	
	g_pCharMng->ChangeDigimonModel( GetIDX(), m_nNextFTID, NULL, m_szNextName, m_fNextScale, m_nNextLevel );
	
	SetPauseAll( false );	

	// 움직이는 중이 아니라면
	if( GetProp_Path()->IsMove() == false )
	{
		SetAnimation( ANI::IDLE_CHAR );
	}
	else
	{
		SetDefault_IdleAnimation();
	}

	return true;

}

UINT CDigimon::GetNextFTID()
{ 
	return m_nNextFTID;
}

TCHAR* CDigimon::GetNextName()
{ 
	return m_szNextName;
}
bool CDigimon::_UpdateChangePartnerEndScene( float fDeltaTime )
{
	m_fEvolTime += fDeltaTime;

	switch( m_nEvolStep )
	{
	case 0:
		if( m_fEvolTime > COMMON_EVOL_UPDATE_DELAY_TIME*2.0f )
		{
			SetPauseAll( false );
			m_eDigimonState = m_eBackupDigimonState;

			// 움직이는 중이 아니라면
			if( GetProp_Path()->IsMove() == false )
			{
				SetAnimation( ANI::IDLE_CHAR );
			}
			else
			{
				SetDefault_IdleAnimation();
			}
			return true;
		}
		break;
	}
	return false;
}
//	스킬
cSkill*	 CDigimon::GetSkillMng()
{ 
	return &m_SkillMng;
}
//	컨디션
cCondition* CDigimon::GetCondition()
{ 
	return &m_Condition;
}
void		CDigimon::SetCondition( uint nCondition )
{ 
	m_Condition.SetCondition( nCondition );
}
//	버프
cBuffData* CDigimon::GetBuff()
{
	return &m_BuffData;
}
//	스탯	- 항상 동적으로 구하겠다 - 공식을 변경하고 싶으면 이부분을 바꾸라
float CDigimon::GetAttackRange()
{ 
	return FMDigimon::GetAttackRange( GetFTID(), GetScale() );
}
float CDigimon::GetAttackCharSize()
{ 
	return GetFTDigimon()->GetInfo()->s_dwCharSize*GetScale();
}

// float CDigimon::GetAttackSpeed()
// { 
// 	float fAttackSpeed = CsC_AvObject::GetAttackSpeed();
// 	if( fAttackSpeed == 0.0f )
// 		return FMDigimon::GetAttackSpeed( GetFTID() ) * ( GetBaseStat()->IsFatigue() ? 1.5f : 1.0f );
// 	return fAttackSpeed;
// }

//====================================================================================
//		Sound
//====================================================================================
char* CDigimon::GetSoundDir()
{ 
	return m_pFTDigimon->GetInfo()->s_cSoundDirName;
}
void* CDigimon::PlaySound( char* cSoundPath )
{ 
	return g_pSoundMgr->PlaySystemSound( cSoundPath, GetPos(), this );
}
void* CDigimon::PlayStaticSound( char* cSoundPath )
{ 
	return g_pSoundMgr->PlayStaticSound( cSoundPath, GetPos() );
}

//====================================================================================
//
//		부가 정보 로드
//
//====================================================================================
//void CDigimon::_LoadExtraData( CsVectorPB< sLOAD_EXTRA* >* vpExtraData )
//{
//	int nSize = vpExtraData->Size();
//	for( int i=0; i<nSize; ++i )
//	{
//		switch( (*vpExtraData)[ i ]->s_nType )
//		{
//		case CET_IMAGE:
//			m_pCharImageL = ( (sCHAREXTRA_IMAGE*)(*vpExtraData)[ i ] )->s_pCharImg_L;
//			m_pCharImageS = ( (sCHAREXTRA_IMAGE*)(*vpExtraData)[ i ] )->s_pCharImg_S;
//			break;
//		default:
//			assert_cs( false );
//		}
//	}
//}

void CDigimon::SetBattleTeam( int nTeam )
{ 
	m_nBattleTeam = nTeam;
}
int	CDigimon::GetBattleTeam()
{ 
	return m_nBattleTeam;
}
UINT CDigimon::GetClass()
{ 
	return nClass::Digimon;
}
UINT CDigimon::GetUniqID()
{ 
	//cType type; type.m_nClass = GetClass(); type.m_nIDX = GetIDX(); return type.m_nUID;
	cType type;
	type.SetIDX(GetIDX(), GetClass());
	return type.m_nUID;
}
NiColor CDigimon::GetNameColor()
{
	std::pair<int, bool> isMember;
	isMember.first = GetUniqID();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_DIGIMONUIDX, &isMember );
	if( isMember.second )
		return NiColor( 88/255.0f, 152/255.0f, 231/255.0f );

	return NiColor( 166.0f/255.0f, 232.0f/255.0f, 151.0f/255.0f );
}

CCharOption* CDigimon::GetCharOption()
{ 
	return &m_Option;
}
sCHAR_IMAGE* CDigimon::GetCharImage()
{ 
	return m_pCharImage;
}
NiPoint3 CDigimon::GetRidePos()
{
	if( IsLoad() == false )
		return GetPos();

	NiAVObject* pRide = GetCsNode()->m_pNiNode->GetObjectByName( "ride" );
	if( pRide == NULL )
	{
		// 		assert_csm1( false, _T( "해당 디지몬에 ride 더미본이 존재 하지 않습니다. 디지몬ID = %d" ), GetFTID() );
		return GetPos();
	}
	return pRide->GetWorldTranslate();
}
float CDigimon::GetToolHeight()
{ 
	return ( g_pResist->m_Global.s_bFigureDigimon == false ) ? CsC_AvObject::GetToolHeight() : 117.0f; 
}//{ g_pResist->m_Global.s_bFigureDigimon == false ? return CsC_AvObject::GetToolHeight() : return 117.0f; }

UINT CDigimon::GetModelID() const
{ 
	SAFE_POINTER_RETVAL( m_pFTDigimon, 0 );
	CsDigimon::sINFO const* pInfo = m_pFTDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->s_dwModelID;
}

int	CDigimon::GetAttackRangeType()
{ 
	SAFE_POINTER_RETVAL( m_pFTDigimon, 0 );
	CsDigimon::sINFO* pInfo = m_pFTDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->s_nDigimonType;
}

int	CDigimon::GetEvoType()
{ 
	SAFE_POINTER_RETVAL( m_pFTDigimon, 0 );
	CsDigimon::sINFO* pInfo = m_pFTDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->s_eEvolutionType;
}

TCHAR* CDigimon::GetName()
{ 
	return m_Option.GetName();
}
bool CDigimon::IsRenderName()
{ 
	return g_pResist->m_Global.s_bOtherRenderName && IsEnableObject();
}
float CDigimon::_GetAniTimeMoveDist_Walk()
{ 
	return GetFTDigimon()->GetInfo()->s_fWakkLen;
}
float CDigimon::_GetAniTimeMoveDist_Run()
{ 
	return GetFTDigimon()->GetInfo()->s_fRunLen;
}
float CDigimon::_GetAniTimeMoveDist_ARun()
{ 
	return GetFTDigimon()->GetInfo()->s_fARunLen;
}


bool CDigimon::IsSameEvoultionType( nsCsDigimonTable::eEVOLUTION_TYPE eEvol ) const
{
	SAFE_POINTER_RETVAL( m_pFTDigimon, false );
	CsDigimon::sINFO* pinfo = m_pFTDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pinfo, false );

	return (pinfo->s_eEvolutionType == eEvol) ? true : false;
}

CDigimon::sENCHENT_STAT::sENCHENT_STAT()
{

}

CDigimon::sENCHENT_STAT::~sENCHENT_STAT()
{

}

//=============================================================================================================
//	강화 수치
//=============================================================================================================
void CDigimon::sENCHENT_STAT::GetEnchantLV_Info( int& nMaxLv, int& nMaxLvCnt ) const
{
	nMaxLv = 0;
	nMaxLvCnt = 0;

	std::map<int,sEnchantInfo>::const_iterator it = s_mapInfos.begin();
	for( ; it != s_mapInfos.end(); ++it )
	{
		if( nMaxLv < it->second.s_nLv )
			nMaxLv = it->second.s_nLv;

		if( 12 == it->second.s_nLv )
			nMaxLvCnt++;
	}
}

void CDigimon::sENCHENT_STAT::GetEnchantLV_Info( int& nMaxLv, int& nOldMaxLvCnt, int& nCurMaxLvCnt ) const
{
	nMaxLv = 0;
	nCurMaxLvCnt = 0;
	nOldMaxLvCnt = 0;

	std::map<int,sEnchantInfo>::const_iterator it = s_mapInfos.begin();
	for( ; it != s_mapInfos.end(); ++it )
	{
		if( nMaxLv < it->second.s_nLv )
			nMaxLv = it->second.s_nLv;

		if( 12 <= it->second.s_nLv )
			nOldMaxLvCnt++;

		if( 15 <= it->second.s_nLv )
			nCurMaxLvCnt++;
	}
}

void CDigimon::sENCHENT_STAT::SetEnchantLevel( int nLevel )
{ 
	s_nEnchantLevel = nLevel - 1; 
}

int  CDigimon::sENCHENT_STAT::GetEnchantLevel() const
{ 
	return s_nEnchantLevel; 
}

void CDigimon::sENCHENT_STAT::SetEnchantInfo( int const& nType, int const& nLv, int const& nValue )
{
	std::map<int,sEnchantInfo>::iterator it = s_mapInfos.find( nType );
	if( it == s_mapInfos.end() )
	{
		auto rib = s_mapInfos.insert( std::make_pair( nType, sEnchantInfo() ) );
		it = rib.first;
	}

	it->second.s_nLv = nLv;
	it->second.s_nValue = nValue;
}

int CDigimon::sENCHENT_STAT::GetEnchantLv( int const& nType ) const
{
	sEnchantInfo const* pInfo = GetEnchantInfo( nType );
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->s_nLv;
}

int CDigimon::sENCHENT_STAT::GetEnchantValue( int const& nType ) const
{
	sEnchantInfo const* pInfo = GetEnchantInfo( nType );
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->s_nValue;
}

CDigimon::sEnchantInfo const* CDigimon::sENCHENT_STAT::GetEnchantInfo( int const& nType ) const
{
	std::map<int,sEnchantInfo>::const_iterator it = s_mapInfos.find( nType );
	if( it == s_mapInfos.end() )
		return NULL;
	return &it->second;
}

// CDigimon::sENCHENT_STAT::sENCHENT_STAT()
// { 
// 	s_nEnchant_Att_LV = s_nEnchant_Def_LV = s_nEnchant_Critical_LV = s_nEnchant_Evade_LV = 0;
// 	s_nEnchant_Att = s_nEnchant_Def = s_nEnchant_Critical = s_nEnchant_Evade = s_nEnchant_AttSpeed = s_nEnchant_HitRate = 0;
// 	s_nEnchant_Hp_Lv = 0;
// 	s_nEnchant_Hp = 0;
// }
// 
// void CDigimon::sENCHENT_STAT::GetEnchantLV_Info( int& nMaxLv, int& nMaxLvCnt )
// {
// 	nMaxLv = ( s_nEnchant_Att_LV > s_nEnchant_Def_LV ) ? s_nEnchant_Att_LV : s_nEnchant_Def_LV;
// 	nMaxLv = ( nMaxLv > s_nEnchant_Critical_LV ) ? nMaxLv : s_nEnchant_Critical_LV;
// 	nMaxLv = ( nMaxLv > s_nEnchant_Evade_LV ) ? nMaxLv : s_nEnchant_Evade_LV;
// 	nMaxLv = ( nMaxLv > s_nEnchant_Hp_Lv ) ? nMaxLv : s_nEnchant_Hp_Lv;
// 
// 	nMaxLvCnt = 0;
// 	if( s_nEnchant_Att_LV == 12 ) nMaxLvCnt++;
// 	if( s_nEnchant_Def_LV == 12 ) nMaxLvCnt++;
// 	if( s_nEnchant_Critical_LV == 12 ) nMaxLvCnt++;
// 	if( s_nEnchant_Evade_LV == 12 ) nMaxLvCnt++;
// 	if( s_nEnchant_Hp_Lv == 12 ) nMaxLvCnt++;
// }
// 
// void CDigimon::sENCHENT_STAT::GetEnchantLV_Info( int& nMaxLv, int& nOldMaxLvCnt, int& nCurMaxLvCnt )
// {
// 	nMaxLv = ( s_nEnchant_Att_LV > s_nEnchant_Def_LV ) ? s_nEnchant_Att_LV : s_nEnchant_Def_LV;
// 	nMaxLv = ( nMaxLv > s_nEnchant_Critical_LV ) ? nMaxLv : s_nEnchant_Critical_LV;
// 	nMaxLv = ( nMaxLv > s_nEnchant_Evade_LV ) ? nMaxLv : s_nEnchant_Evade_LV;
// 	nMaxLv = ( nMaxLv > s_nEnchant_Hp_Lv ) ? nMaxLv : s_nEnchant_Hp_Lv;
// 
// 	// 스텟 중 15강 풀강된 스텟이 있는가?
// 	if( s_nEnchant_Att_LV == 15 ) nCurMaxLvCnt++;
// 	if( s_nEnchant_Def_LV == 15 ) nCurMaxLvCnt++;
// 	if( s_nEnchant_Critical_LV == 15 ) nCurMaxLvCnt++;
// 	if( s_nEnchant_Evade_LV == 15 ) nCurMaxLvCnt++;
// 	if( s_nEnchant_Hp_Lv == 15 ) nCurMaxLvCnt++;
// 
// 	// 스텟 중 12강 이상 강화된 스텟이 있는가?
// 	if( s_nEnchant_Att_LV >= 12 ) nOldMaxLvCnt++;
// 	if( s_nEnchant_Def_LV >= 12 ) nOldMaxLvCnt++;
// 	if( s_nEnchant_Critical_LV >= 12 ) nOldMaxLvCnt++;
// 	if( s_nEnchant_Evade_LV >= 12 ) nOldMaxLvCnt++;
// 	if( s_nEnchant_Hp_Lv >= 12 ) nOldMaxLvCnt++;
// }
// 
// void CDigimon::sENCHENT_STAT::SetEnchantLevel( int nLevel ){ s_nEnchantLevel = nLevel - 1; }
// int  CDigimon::sENCHENT_STAT::GetEnchantLevel(){ return s_nEnchantLevel; }
// int CDigimon::sENCHENT_STAT::GetAtt_Enchant(){ return s_nEnchant_Att; }
// int CDigimon::sENCHENT_STAT::GetBlock_Enchant(){ return s_nEnchant_Def; }
// int CDigimon::sENCHENT_STAT::GetCritical_Enchant(){ return s_nEnchant_Critical; }
// int CDigimon::sENCHENT_STAT::GetAttSpeed_Enchant(){ return s_nEnchant_AttSpeed; }
// int CDigimon::sENCHENT_STAT::GetEvade_Enchant(){ return s_nEnchant_Evade; }
// int CDigimon::sENCHENT_STAT::GetHitRate_Enchant(){ return s_nEnchant_HitRate; }		
// int CDigimon::sENCHENT_STAT::GetHP_Enchant(){ return s_nEnchant_Hp; }		
// 
// void CDigimon::sENCHENT_STAT::SetAtt_Enchant( int nValue ){ s_nEnchant_Att = nValue; }
// void CDigimon::sENCHENT_STAT::SetBlock_Enchant( int nValue ){ s_nEnchant_Def = nValue; }
// void CDigimon::sENCHENT_STAT::SetCritical_Enchant( int nValue ){ s_nEnchant_Critical = nValue; }
// void CDigimon::sENCHENT_STAT::SetAttSpeed_Enchant( int nValue ){ s_nEnchant_AttSpeed = nValue; }
// void CDigimon::sENCHENT_STAT::SetEvade_Enchant( int nValue ){ s_nEnchant_Evade = nValue; }
// void CDigimon::sENCHENT_STAT::SetHitRate_Enchant( int nValue ){ s_nEnchant_HitRate = nValue; }
// void CDigimon::sENCHENT_STAT::SetHP_Enchant( int nValue ){ s_nEnchant_Hp = nValue; }
// 
// int CDigimon::sENCHENT_STAT::GetAtt_Enchant_LV(){ return s_nEnchant_Att_LV; }
// int CDigimon::sENCHENT_STAT::GetBlock_Enchant_LV(){ return s_nEnchant_Def_LV; }
// int CDigimon::sENCHENT_STAT::GetCritical_Enchant_LV(){ return s_nEnchant_Critical_LV; }		
// int CDigimon::sENCHENT_STAT::GetEvade_Enchant_LV(){ return s_nEnchant_Evade_LV; }
// int	CDigimon::sENCHENT_STAT::GetHP_Enchant_LV(){ return s_nEnchant_Hp_Lv; }
// 
// void CDigimon::sENCHENT_STAT::SetAtt_Enchant_LV( int nValue ){ s_nEnchant_Att_LV = nValue; }
// void CDigimon::sENCHENT_STAT::SetBlock_Enchant_LV( int nValue ){ s_nEnchant_Def_LV = nValue; }
// void CDigimon::sENCHENT_STAT::SetCritical_Enchant_LV( int nValue ){ s_nEnchant_Critical_LV = nValue; }		
// void CDigimon::sENCHENT_STAT::SetEvade_Enchant_LV( int nValue ){ s_nEnchant_Evade_LV = nValue; }	
// void CDigimon::sENCHENT_STAT::SetHP_Enchant_LV( int nValue ){ s_nEnchant_Hp_Lv = nValue; }
CDigimon::sENCHENT_STAT* CDigimon::GetEnchantStat(){ return m_pEnchentStat; }
//=============================================================================================================
//
//	라이딩
//
//=============================================================================================================

void CDigimon::_SetRideRot()
{
	if( m_eDigimonState == DIGIMON_DIE )
		return;

	if( IsRide() == false )
		return;

	if( m_pFTRide->GetInfo()->s_nRideType == 5 )
	{
		NiPoint2 vDir( GetDir().x * 101, GetDir().y * 101 ); 
		NiPoint2 vFront = GetPos2D() + vDir;
		NiPoint2 vBack = GetPos2D() - vDir;

		if (!GetProp_Path()->PathTestLine(vFront))
			return;
		if (!GetProp_Path()->PathTestLine(vBack))
			return;

		float fFrontHeight = nsCsGBTerrain::g_pCurRoot->GetHeight( vFront.x, vFront.y );					
		float fBackHeight = nsCsGBTerrain::g_pCurRoot->GetHeight( vBack.x, vBack.y );

		float fSq = fFrontHeight - fBackHeight;			
		float fRotX = NiATan( fSq / 202 );
		
		SetRotX( fRotX );	

		CTamer* pTamer = g_pCharMng->GetTamer( GetTamerLink() );	
		if( pTamer )
		{
			pTamer->SetRotX( fRotX );
		}
	}
}

bool CDigimon::SetRide()
{
	if( m_eDigimonState == DIGIMON_DIE )
		return false;

	if( IsRide() == true )
		return false;

	if( nsCsFileTable::g_pDigimonMng->IsRide( GetFTID() ) == false )
		return false;	

	m_pFTRide = nsCsFileTable::g_pDigimonMng->GetRide( GetFTID() );	

	m_eDigimonMove = DM_RIDE;	

	if( m_pFTRide->GetInfo()->s_dwChangeRide != 0 )
	{		
		ChangeRideKFM( m_pFTRide->GetInfo()->s_dwChangeRide, true );
	}
	else
	{			
		_SetDefault_IdleAnimation_Stand();	
	}	

	// 현재 에이전트 제거 후 새로운 위치에서의 에이전트 생성
	CTamer* pTamer = g_pCharMng->GetTamer( GetTamerLink() );
	assert_cs( pTamer );
	if( pTamer )
	{		
		SetPos( pTamer->GetPos() );
		GetProp_Path()->ResetAgent();
	}

	return true;
}

bool CDigimon::SetRideEnd()
{
	if( m_eDigimonState == DIGIMON_DIE )
		return false;

	if( IsRide() == false )
		return false;

	SetRotX( 0.0f );
	m_eDigimonMove = DM_NONE;

	if( m_pFTRide->GetInfo()->s_dwChangeRide != 0 )
	{
		ChangeRideKFM( GetFTID(), false );
	}

	CTamer* pTamer = g_pCharMng->GetTamer( GetTamerLink() );
	assert_cs( pTamer );
	if( pTamer )
	{
		pTamer->SetPause( PAUSE_ANIMATION, false );
	}
	m_pFTRide = NULL;
	SetDefault_IdleAnimation();
	return true;
}

bool CDigimon::IsRide()
{ 
	return ( m_eDigimonMove == DM_RIDE );
}
CsRide*	CDigimon::GetFTRide()
{ 
	return m_pFTRide;
}

//=============================================================================================================
//	디지몬 파츠
//=============================================================================================================
void CDigimon::SetPostDParts(UINT const& nPartsItemType)
{
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	nsDPART::sDPART_INFO partsInfo;
	partsInfo.s_nFileTableID = nPartsItemType;
	if( !nsCsFileTable::g_pItemMng->GetDPartsInfo( nPartsItemType, GetNextFTID(), partsInfo.s_nPartIndex, partsInfo.s_File ) )
		return;

	std::list<nsDPART::sDPART_INFO>::iterator it = m_PostParts.begin();
	for( ; it != m_PostParts.end(); ++it )
	{
		if( it->s_nPartIndex == partsInfo.s_nPartIndex )
			break;
	}

	if( it == m_PostParts.end() )
		m_PostParts.push_back( partsInfo );
	else
		*it = partsInfo;
}

void CDigimon::ClearAllPostDParts()
{
	m_PostParts.clear();
}

#undef		PARENT_CLASS