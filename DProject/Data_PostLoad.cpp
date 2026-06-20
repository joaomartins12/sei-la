
#include "stdafx.h"
#include "Data_PostLoad.h"

cData_PostLoad::sDATA::sDATA()
{
	ResetData();
}

cData_PostLoad::sDATA::~sDATA()
{
	ResetData();
}

void cData_PostLoad::sDATA::ResetData()
{
	s_SlotIdx = 0;
	s_Type.Reset(); 
	s_nBattleTeam = 0;
	s_dwBaseDigimonID = 0;
	s_nUID = 0;
	s_HatchLevel = 0;
	s_fScale = 1.0f;
	s_nEnchantLevel = 0;
	s_nLevel = 0;
	s_nExp = 0;
	s_nTranscendenceExp = 0;
	s_nPat = 0;
	s_nNewSkillPoint = 0;
	s_nMaxEvoUnit = 0;
	memset( s_szName, 0, sizeof( s_szName ) ); 
	memset( &s_EvoUnit, 0, sizeof( cEvoUnit ) * nLimit::EvoUnit );	
	memset( &s_Attribute, 0, sizeof(n4) * eExtStats::MaxExtStat );
	memset( &s_ExtendAttribute, 0, sizeof(u2) * nExtendState::MaxExtendStat );
	memset( &s_ExtendAttributeLV, 0, sizeof(u2) * nExtendState::MaxExtendStat );

	memset( &s_AttributeExp, 0, sizeof(u2) * NewAttribute::MaxDigitalType );
	memset( &s_NatureExp, 0, sizeof(u2) * NewAttribute::MaxNatualType );


	memset( &s_nTamerSkillCode, 0, sizeof(u4) * nLimit::Skill );
	memset( &s_nTamerSkillCoolTime, 0, sizeof(u4) * nLimit::Skill );
	memset( &s_nTamerCashSkill_Index, 0, sizeof(u4) * nLimit::Skill );
	memset( &s_nTamerCashSkill_LifeTime, 0, sizeof(u4) * nLimit::Skill );
	memset( &s_nTamerCashSkill_CoolTime, 0, sizeof(u4) * nLimit::Skill );
	memset( &s_DCashSkill, 0, sizeof(sDCashSkill) * nLimit::EvoUnit );

#ifdef MEGAPHONE_BAN
	s_bMegaPhoneBan = false;
#endif
}

//////////////////////////////////////////////////////////////////////////
cData_PostLoad::cData_PostLoad():m_nServerRelocateCompletedTime(0)
{
	m_sPos.m_nX = 0;
	m_sPos.m_nY = 0;
	m_sBackupPos.m_nX = 0;							// 다음맵 카메라를 위치시키기 위해서
	m_sBackupPos.m_nY = 0;
}

cData_PostLoad::~cData_PostLoad()
{

}

void cData_PostLoad::sDATA::ToObject( CsC_AvObject* pAvObject, bool bInit )
{
	switch( pAvObject->GetLeafRTTI() )
	{
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_DIGIMON_USER:
		{
			CDigimonUser::sUSER_STAT* pBaseStat = (CDigimonUser::sUSER_STAT*)pAvObject->GetBaseStat();	
			CDigimon::sENCHENT_STAT* pEStat = ( (CDigimon*)pAvObject )->GetEnchantStat();
			if( net::game )
			{
				if( bInit )
				{
					pBaseStat->SetMaxHP( s_Attribute[ MHP ] );
					pBaseStat->SetMaxDS( s_Attribute[ MDS ] );
					pBaseStat->SetHP( s_Attribute[ HP ] );
					pBaseStat->SetDS( s_Attribute[ DS ] );
					pBaseStat->SetMoveSpeed( (float)s_Attribute[ MS ] );
					pBaseStat->SetFriendShip( s_Attribute[ FS ] );

					pBaseStat->SetAtt( s_Attribute[ AP ] );
					pBaseStat->SetDef( s_Attribute[ DP ] );
					pBaseStat->SetCritical( s_Attribute[ CA ] );
					pBaseStat->SetAttackSpeed( s_Attribute[ AS ] );
					pBaseStat->SetEvade( s_Attribute[ EV ] );
					pBaseStat->SetHitRate( s_Attribute[ HT ] );
					pBaseStat->SetAttRange( s_Attribute[ AR ] );
					pBaseStat->SetBL( s_Attribute[BL] );

					pEStat->SetEnchantLevel( s_nEnchantLevel );

					for( int i = nExtendState::ET_AT; i < nExtendState::MaxExtendStat; ++i )
					{
						pEStat->SetEnchantInfo( i, s_ExtendAttributeLV[ i ], s_ExtendAttribute[ i ] );
					}

					( (CDigimonUser*)pAvObject )->SetBattleTeam( s_nBattleTeam );
				}
				pBaseStat->SetExp( s_nExp );
			}
			
			pBaseStat->SetLevel( s_nLevel );			
			( (CDigimonUser*)pAvObject )->SetNewSkillPoint( s_nNewSkillPoint );
			( (CDigimonUser*)pAvObject )->SetAttributeEvoUnit( s_nMaxEvoUnit, s_EvoUnit );			
			( (CDigimonUser*)pAvObject )->SetAttributeExp( s_AttributeExp );	
			( (CDigimonUser*)pAvObject )->SetNatureExp( s_NatureExp );
			pAvObject->SetScale( s_fScale );

			GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL );
		}
		break;
	case RTTI_TUTORIAL_TAMER:
	case RTTI_TAMER_USER:
		{
			assert_cs( bInit == true );
			CTamerUser::sUSER_STAT* pBaseStat = (CTamerUser::sUSER_STAT*)pAvObject->GetBaseStat();
			pBaseStat->SetLevel( s_nLevel );
			if( net::game )
			{			
				pBaseStat->SetMaxHP( s_Attribute[ MHP ] );
				pBaseStat->SetMaxDS( s_Attribute[ MDS ] );
				pBaseStat->SetHP( s_Attribute[ HP ] );
				pBaseStat->SetDS( s_Attribute[ DS ] );				
				pBaseStat->SetFP( s_Attribute[ FP ] );
				pBaseStat->SetExp( s_nExp );
				pBaseStat->SetMoveSpeed( (float)s_Attribute[ MS ] );

				pBaseStat->SetAtt( s_Attribute[ AP ] );
				pBaseStat->SetDef( s_Attribute[ DP ] );
				GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
			}
			pAvObject->SetScale( s_fScale );
		}
		break;
	default:
		assert_cs( false );
	}
}

#ifdef CROSSWARS_SYSTEM
void cData_PostLoad::sTAMERDATA::ToObject( CsC_AvObject* pAvObject, bool bInit )
{

	switch( pAvObject->GetLeafRTTI() )
	{
	case RTTI_TAMER_USER:
		{
			assert_cs( bInit == true );
			CTamerUser::sUSER_STAT* pBaseStat = (CTamerUser::sUSER_STAT*)pAvObject->GetBaseStat();			
			if( net::game )
			{	
				pBaseStat->SetMaxCR( s_MCR );
				pBaseStat->SetCR( s_CR );

			}
		}
		break;
	}
	sDATA::ToObject(pAvObject, bInit);
}
#endif

//void cData_PostLoad::sDATA::FromObject( CsC_AvObject* pAvObject )
//{
//	switch( pAvObject->GetLeafRTTI() )
//	{
//	case RTTI_DIGIMON_USER:
//		{
//			CDigimonUser* pDigimon = (CDigimonUser*)pAvObject;
//
//			s_Type.m_nUID = pDigimon->GetUniqID();
//			s_Type.SetType( pDigimon->GetPrevBaseDigimonFTID() );
//			s_dwBaseDigimonID = pDigimon->GetPrevBaseDigimonFTID();
//
//			s_fScale = pDigimon->GetGameScale();
//
//			CsC_AvObject::sBASE_STAT* pBaseStat = pDigimon->GetBaseStat();
//			// 스탯포인트 빠져있다.. 쓸데가 없음.  ( 유저가 스탯찍는게 아니므로	
//			s_nLevel = pBaseStat->GetLevel();
//			s_Attribute[ MHP ] = pBaseStat->GetMaxHP();
//			s_Attribute[ MDS ] = pBaseStat->GetMaxDS();
//			s_Attribute[ HP ] = pBaseStat->GetHP();
//			s_Attribute[ DS ] = pBaseStat->GetDS();
//			s_nExp = pBaseStat->GetExp();
//			s_Attribute[ MS ] = (int)pBaseStat->GetMoveSpeed();
//			s_nMaxEvoUnit = pDigimon->GetMaxEvoUnit();
//			memcpy( s_EvoUnit, pDigimon->GetAttributeEvoUnit(), sizeof( cEvoUnit )*nLimit::EvoUnit );			
//		}
//		break;
//	case RTTI_TAMER_USER:
//		{
//			CTamerUser* pTamer = (CTamerUser*)pAvObject;
//
//			s_Type.m_nUID = pTamer->GetUniqID();
//			s_Type.SetType( pTamer->GetFTID() );
//
//			CsC_AvObject::sBASE_STAT* pBaseStat = pTamer->GetBaseStat();
//			// 스탯포인트 빠져있다.. 쓸데가 없음.  ( 유저가 스탯찍는게 아니므로	
//			s_nLevel = pBaseStat->GetLevel();
//			s_Attribute[ MHP ] = pBaseStat->GetMaxHP();
//			s_Attribute[ MDS ] = pBaseStat->GetMaxDS();
//			s_Attribute[ HP ] = pBaseStat->GetHP();
//			s_Attribute[ DS ] = pBaseStat->GetDS();
//			s_nExp = pBaseStat->GetExp();
//			s_Attribute[ MS ] = (int)pBaseStat->GetMoveSpeed();			
//		}
//		break;
//	default:
//		assert_cs( false );
//	}	
//}


void cData_PostLoad::Delete()
{
	m_Tamer.Delete();
	m_Digimon.Delete();
	m_mapDParts.clear();
}

void cData_PostLoad::Init()
{
	Reset();

	if( IsNetOff() == true )
	{
		m_dwServerOption = 0;

		cProfile x("./dmo.ini");
		net::next_map_no = x.GetInt("MAIN", "InitMapID");

		if( ( m_sPos.m_nX = x.GetInt( "MAIN", "InitPosX") ) == 0 )
		{
			m_sPos.m_nX = 22550;
		}
		if( ( m_sPos.m_nY = x.GetInt( "MAIN", "InitPosY") ) == 0 )
		{
			m_sPos.m_nY = 15646;
		}

		switch( net::next_map_no )
		{
		case 9101:
		case 9102:
		case 9103:
			{
				nsCsGBTerrain::g_nSvrLibType = nLIB::SVR_BATTLE;							
			}			
			break;
		default:
			nsCsGBTerrain::g_nSvrLibType = nLIB::SVR_GAME;
			break;
		}
	}
}

void cData_PostLoad::InitNetOff()
{
	m_bNetOffInitPostLoad = true;

	_stprintf_s( m_Tamer.s_szName, Language::pLength::name + 1, _T( "Tamer_Tamer" ) );
	_stprintf_s( m_Digimon.s_szName, Language::pLength::name + 1, _T( "디지몬" ) );

	cProfile x("./dmo.ini");

	m_Tamer.s_Type.m_nIDX = cType::GetTestIDX( nClass::Tamer );
	if( ( m_Tamer.s_Type.m_nType = x.GetInt( "CHAR", "tamer") ) == 0 )
	{
		m_Tamer.s_Type.m_nType = CODE_MASARU;
	}

	m_Digimon.s_Type.m_nIDX = cType::GetTestIDX( nClass::Digimon );
	if( ( m_Digimon.s_Type.m_nType = x.GetInt( "CHAR", "digimon") ) == 0 )
	{
		m_Digimon.s_Type.m_nType = CODE_AGUMON;
	}
	m_Digimon.s_dwBaseDigimonID = m_Digimon.s_Type.m_nType;

	m_Tamer.s_nLevel = 120;
	m_Tamer.s_nPat = 0;
	m_Digimon.s_nLevel = 71;	
	m_Digimon.s_fScale = 1.0f;

	m_Digimon.s_nMaxEvoUnit = nLimit::EvoUnit - 1;
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		m_Digimon.s_EvoUnit[ i ].m_nSkillExp = 0;
		m_Digimon.s_EvoUnit[ i ].m_nSkillExpLevel = 0;
		m_Digimon.s_EvoUnit[ i ].m_nSkillPoint = 100;
		m_Digimon.s_EvoUnit[ i ].m_nSlotState = 0;
		for( int s=0; s<nLimit::Skill; ++s )
			m_Digimon.s_EvoUnit[ i ].m_nSkillLevel[ s ] = 1;	
	}

	//if( ( m_Digimon.s_EvoUnit[ 1 ].m_nSlotState = x.GetInt( "CHAR", "dslot1") ) == 0 )//{ m_Digimon.s_EvoUnit[ 1 ].m_nSlotState = 1;}
	//if( ( m_Digimon.s_EvoUnit[ 2 ].m_nSlotState = x.GetInt( "CHAR", "dslot2") ) == 0 )//{ m_Digimon.s_EvoUnit[ 2 ].m_nSlotState = 1;}
	//if( ( m_Digimon.s_EvoUnit[ 3 ].m_nSlotState = x.GetInt( "CHAR", "dslot3") ) == 0 )//{ m_Digimon.s_EvoUnit[ 3 ].m_nSlotState = 1;}
	//if( ( m_Digimon.s_EvoUnit[ 4 ].m_nSlotState = x.GetInt( "CHAR", "dslot4") ) == 0 )//{ m_Digimon.s_EvoUnit[ 4 ].m_nSlotState = 1;}
	//if( ( m_Digimon.s_EvoUnit[ 5 ].m_nSlotState = x.GetInt( "CHAR", "dslot5") ) == 0 )//{ m_Digimon.s_EvoUnit[ 5 ].m_nSlotState = 1;}
	//if( ( m_Digimon.s_EvoUnit[ 6 ].m_nSlotState = x.GetInt( "CHAR", "dslot6") ) == 0 )//{ m_Digimon.s_EvoUnit[ 6 ].m_nSlotState = 1;}	

	m_Digimon.s_EvoUnit[ 1 ].m_nSlotState = x.GetInt( "CHAR", "dslot1");
	m_Digimon.s_EvoUnit[ 2 ].m_nSlotState = x.GetInt( "CHAR", "dslot2");
	m_Digimon.s_EvoUnit[ 3 ].m_nSlotState = x.GetInt( "CHAR", "dslot3");
	m_Digimon.s_EvoUnit[ 4 ].m_nSlotState = x.GetInt( "CHAR", "dslot4");
	m_Digimon.s_EvoUnit[ 5 ].m_nSlotState = x.GetInt( "CHAR", "dslot5");
	m_Digimon.s_EvoUnit[ 6 ].m_nSlotState = x.GetInt( "CHAR", "dslot6");

	ChannelContents::CHANNELINFO ChannelData;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::GET_CHANNELINFO, &ChannelData );
	ChannelData.channel_idx = 2;
	for( int i=0; i<nLimit::Channel; ++i )
	{
		ChannelData.channel[ i ] = rand()%6-1;
	}

	/*
	n2			s_AttributeExp[ NewAttribute::MaxDigitalType ];
	n2			s_NatureExp[ NewAttribute::MaxNatualType ];
	*/
	for( int i=0; i < NewAttribute::MaxDigitalType; i++ )
	{
		m_Digimon.s_AttributeExp[i] = 10000;
	}

	m_Digimon.s_AttributeExp[1] = 10000;
	
	for( int i=0; i < NewAttribute::MaxNatualType; i++ )
	{
		m_Digimon.s_NatureExp[i] = 10000;
	}
	m_Digimon.s_NatureExp[0] = 10000;
	m_Digimon.s_NatureExp[5] = 4000;
	m_Digimon.s_NatureExp[6] = 9900;
}

void cData_PostLoad::_NetOff_InitPostLoad()
{
	m_bNetOffInitPostLoad = false;
}

void cData_PostLoad::Reset()
{
// 	if( IsNetOff() == true )
// 	{
// 		g_eLoopPlayState = LPS_POST_LOAD;
// 	}

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	ClearPartsInfo();
#endif
}

void cData_PostLoad::SetServerRelocateCompletedTime( unsigned int const& nTime )
{
	m_nServerRelocateCompletedTime = nTime;
}

void cData_PostLoad::SetTamerData(uint const& nItemID)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_TEquip* pTEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RET( pTEquip );
	cItemInfo* pCostume = pTEquip->GetData( nTamer::Costume );
	SAFE_POINTER_RET( pCostume );
	pCostume->SetType( nItemID );
}

void cData_PostLoad::SetDigimonData(uint const& nFTID, uint const& nBaseDigimonID, std::wstring const& wsName)
{
	m_Digimon.s_Type.SetType( nFTID );
	m_Digimon.s_dwBaseDigimonID = nBaseDigimonID;
	m_Digimon.s_nLevel = nLimit::DigimonLevelMax;
	m_Digimon.s_Attribute[ MHP ] = 99999999;
	m_Digimon.s_Attribute[ MDS ] = 99999999;
	m_Digimon.s_Attribute[ HP ] = 99999999;
	m_Digimon.s_Attribute[ DS ] = 99999999;
	wcscpy_s( m_Digimon.s_szName, Language::pLength::name + 1, wsName.c_str() );
}

void cData_PostLoad::PostLoad()
{	
	if( m_bNetOffInitPostLoad == true )
	{
		_NetOff_InitPostLoad();
	}

	// 테이머 생성
	//assert_cs( nsPART::MAX_TOTAL_COUNT == nLimit::Equip );	

	cItemInfo* pItemInfo = NULL;
	CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];

	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
	{
		if( i >= nTamer::MaxParts )
			pItemInfo = g_pDataMng->GetTEquip()->GetDigiviceItem();
		else
			pItemInfo = g_pDataMng->GetTEquip()->GetData( i );
		SAFE_POINTER_CON( pItemInfo );

		cp[ i ].s_nFileTableID = pItemInfo->GetType();
		cp[ i ].s_nPartIndex = i;
		cp[ i ].s_nRemainTime = pItemInfo->m_nEndTime;
	}

#ifdef PC_BANG_SERVICE
	if( g_pDataMng->GetPCbang()->GetPcBang() )
	{
		if( cp[nsPART::Costume].s_nFileTableID == PCBANG_COSTUME_ITEM_ID )
			cp[5].s_nFileTableID = g_pDataMng->GetPCbang()->GetCostume();
	}
#endif

	CTamerUser* pTamerUser = g_pCharMng->AddTamerUser( m_Tamer.s_Type.m_nIDX, m_Tamer.s_Type.m_nType, m_sPos, 0.0f, m_Tamer.s_szName, cp, m_dwSyncOption );
	m_Tamer.ToObject( pTamerUser, true );	
	//DWORD dwPlag = nsEFFECT::POS_CHARPOS;
	pTamerUser->SetServerOption( m_dwServerOption );
	pTamerUser->SetServerRelocateCompletedTime( m_nServerRelocateCompletedTime );

	DWORD dispTitle;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_GET_DISPTITLE, &dispTitle);

	pTamerUser->GetCharOption()->SetAchieve( dispTitle );
	pTamerUser->SetDigimonLink( m_Digimon.s_Type.m_nIDX );
	pTamerUser->GetProp_Effect()->AddEffect_FT( EFFECT_CREATE_SCENE );	
	pTamerUser->ResetOldFatiguePoint();
#ifdef MEGAPHONE_BAN
	pTamerUser->SetMegaPhoneBan( m_Tamer.s_bMegaPhoneBan );
#endif

	for( int i=0; i<nLimit::Skill; ++i )
	{
		if( m_Tamer.s_nTamerCashSkill_LifeTime[i] == 0 || m_Tamer.s_nTamerCashSkill_Index[i] == 0)
			continue;

		// 캐쉬 테이머 스킬 세팅
		g_pCharMng->GetTamerUser()->GetSkillMng()->SetTamerCashSkill( m_Tamer.s_nTamerCashSkill_Index[i], m_Tamer.s_nTamerCashSkill_CoolTime[i], m_Tamer.s_nTamerCashSkill_LifeTime[i] );
	}

	// 세팅 후 초기화 해주자
	for( int i=0; i<nLimit::Skill; ++i )
	{
		m_Tamer.s_nTamerCashSkill_Index[i] = 0;
		m_Tamer.s_nTamerCashSkill_CoolTime[i] = 0;
		m_Tamer.s_nTamerCashSkill_LifeTime[i] = 0;
	}

	// Cash 스킬 퀵슬롯 등록
	cResist::sCHAR_QUICKSLOT::sCHAR* pCashQuickSlot = g_pDataMng->GetTamerCashQuickSlot();
	if( pCashQuickSlot->s_bInitial == true )
	{
		cResist::sCHAR_QUICKSLOT::sCHAR* pTamerQuick = g_pCharMng->GetTamerUser()->GetQuickSlot();
		for( int i=0; i<8; ++i )
		{
			if( pCashQuickSlot->s_QuickSlotInfo[i].s_nID == pTamerQuick->s_QuickSlotInfo[i].s_nID )
				continue;

			if(pCashQuickSlot->s_QuickSlotInfo[i].s_eType == cResist::ITEM )
				continue;
			
			pTamerQuick->s_QuickSlotInfo[i].s_eType = pCashQuickSlot->s_QuickSlotInfo[i].s_eType;
			pTamerQuick->s_QuickSlotInfo[i].s_nID = pCashQuickSlot->s_QuickSlotInfo[i].s_nID;
		}
	}

	int nCount = g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkillCount();	// 테이머 스킬 총 개수

	for( int i=0; i<nLimit::Skill; ++i )
	{
		if( m_Tamer.s_nTamerSkillCode[i] == 0 )
			continue;

		for( int k=0; k<nCount; ++k )
		{			
			int nSkill_Index = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerSkill_Index( m_Tamer.s_nTamerSkillCode[i] );
			if( nSkill_Index == -1 )
				continue;

			if( m_Tamer.s_nTamerSkillCoolTime[i] < _TIME_TS + 1 )
				break;

			cSkill::sINFO* pSkill_Info = pTamerUser->GetSkillMng()->GetSkill( nSkill_Index );	// 사용한 테이머스킬 정보 얻어옴

			pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );
			double fOffset = pSkill_Info->s_CoolTime.GetEndTime() - ( m_Tamer.s_nTamerSkillCoolTime[i] - _TIME_TS ) - 1;
			pSkill_Info->s_CoolTime.Start( fOffset );
		}
	}

	for( int i=0; i<nLimit::Skill; ++i )	// 사용했으면 초기화 해주자!
	{
		m_Tamer.s_nTamerSkillCode[i] = 0;
		m_Tamer.s_nTamerSkillCoolTime[i] = 0;
	}

	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( 5 );
	pDataQuickSlot->SetChar( NULL );

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	// 디지몬 파츠 정보
	std::list<nsDPART::sDPART_INFO> parts;
	std::map< int, sPartsData >::iterator itParts = m_mapDParts.begin();
	for( ; itParts != m_mapDParts.end(); ++itParts )
	{
		nsDPART::sDPART_INFO partsInfo;
		partsInfo.s_nPartIndex = itParts->first;
		partsInfo.s_nFileTableID = itParts->second.s_nPartsItemID;
		partsInfo.s_File = itParts->second.s_strFile;
		parts.push_back( partsInfo );
	}

	// 디지몬 생성
	CDigimonUser* pDigimonUser = g_pCharMng->AddDigimonUser( 0, m_Digimon.s_Type.m_nIDX, m_Digimon.s_Type.m_nType, m_Digimon.s_dwBaseDigimonID, &parts, m_sPos, 0.0f, m_Digimon.s_szName, true );
#else
	// 디지몬 생성
	
	CDigimonUser* pDigimonUser = g_pCharMng->AddDigimonUser( 0, m_Digimon.s_Type.m_nIDX, m_Digimon.s_Type.m_nType, m_Digimon.s_dwBaseDigimonID, NULL, m_sPos, 0.0f, m_Digimon.s_szName, true );
#endif

	// QuickSlot에 정상적으로 보여주기 위해 디지몬 정보 생성 후 바로 캐시 스킬 정보 등록
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		if( pDigimonUser->GetFTEvolCurObj()->m_nEvoSlot != m_Digimon.s_DCashSkill[i].s_nDigimonEvoStatus )
			continue;

		for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )
			pDigimonUser->GetSkillMng()->SetDigimonCashSkill( m_Digimon.s_DCashSkill[i].s_nDigimonCashSkillCode[j], m_Digimon.s_DCashSkill[i].s_nSkillCoolTime[j] );
	}
	m_Digimon.ToObject( pDigimonUser, true );
	pDigimonUser->SetTamerLink( m_Tamer.s_Type.m_nIDX );
	pDigimonUser->GetProp_Effect()->AddEffect_FT( EFFECT_CREATE_SCENE );
	
	if( IsNetOff() == false )
	{
		g_pCharMng->GetDigimonUser( 0 )->SetServerInitPos();

//		// 처음 접속 하면
// 		bool bServerOptionCheck = false;
// #ifdef THAI_TUTORIAL
// 		if( pTamerUser->GetServerOption() == 0)
// 			bServerOptionCheck = true;
// 
// 		if( g_bFirstConnetChar == true || bServerOptionCheck )
// #else
// 		bServerOptionCheck = false;
// #endif
// 
// #if ( defined VERSION_KOR || defined VERSION_USA )				//2016-03-09-nova 홍콩 튜토리얼 스킵후 접속 안되는 문제 
// 		if( (g_pCharMng->GetMyTamerLv() == 1 && g_pCharMng->GetMyTamerExp() == 0) )
// 			bServerOptionCheck = true;
// #endif
// 
// 		if( g_bFirstConnetChar == true || bServerOptionCheck )
// 		{
// 			pTamerUser->AddServerOption( CTamerUser::SO_DO_TUTORIAL );
// 			net::game->SendClientOptionSave( pTamerUser->GetServerOption() );
// 			//g_pResist->SetTutorial( true );
// 			//cCredit::Begin();
// 		}
	}	

	//===================================================================================================================================
	// 인터페이스 관련
	//===================================================================================================================================	
	g_pDataMng->GetMapRegion()->GetMapRegion()->SetOpenMap( nsCsMapTable::g_pMapListMng->GetList( net::next_map_no )->GetInfo()->s_nMapRegionID );
	g_pDataMng->GetQuest()->PostRevPostLoad( net::next_map_no );
	g_pGameIF->GetQuestHelper()->ResetHelperString();
	if( g_pGameIF->GetQuestList() )
		g_pGameIF->GetQuestList()->ResetQuestList( true );	
	g_pGameIF->GetMiniMap()->LoadMiniMap();
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP ) )
		g_pGameIF->GetSmallMap()->LoadSmallMap();

	//캐릭터 위치찾기 사용시..
	if( g_pHelpArrow->IsArrowRender() )
		g_pHelpArrow->FindRoot(); // 캐릭터 위치 사용해야 하므로 일로 이동..
	else if( g_bFirstConnetChar )
	{
		if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() == 1 &&
			g_pCharMng->GetTamerUser()->GetBaseStat()->GetExp() == 0 &&
			nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == 1 )
		{
			if( !g_pDataMng->GetQuest()->IsProcess( 143 ) &&
				!g_pDataMng->GetQuest()->IsCompleate( 143 ) )
			{
				CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( 90018 ); //고동혁
				g_pHelpArrow->SetArrowPoint( 90018, 1, cQuestFindWay::FindWay_Npc, pFTNpc->GetInfo()->s_szName );
			}
		}
	}	


	//===================================================================================================================================
	// 카메라
	//===================================================================================================================================
	// 게임내 카메라 셋팅
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
// 		{	// 
// 			NiPoint3 vSrc = pTamerUser->GetPos();
// 			vSrc.z += 500.0f;
// 
// 			CsMapPortalGroup* pProtalGroup = nsCsMapTable::g_pMapPortalMng->GetGroup( net::prev_map_no );
// 			SAFE_POINTER_BEK( pProtalGroup );
// 
// 			NiPoint3 vTarget = NiPoint3::ZERO;
// 			CsMapPortal* pPortalFT = pProtalGroup->GetNearPortal( net::next_map_no, (int)vSrc.x, (int)vSrc.y );
// 			if( pPortalFT )
// 			{
// 				CsMapPortal::sINFO* pInfo = pPortalFT->GetInfo();
// 				if( pInfo )
// 				{
// 					vTarget = NiPoint3( (float)pInfo->s_nViewTargetX, (float)pInfo->s_nViewTargetY, 0 );
// 					vTarget.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vTarget );
// 				}				
// 			}
// 			pTamerUser->SetRotation( vTarget, true );
// 			pDigimonUser->SetRotation( vTarget, true );
// 
// 			CAMERA_ST.SetRotation( vTarget - vSrc );
// 			CAMERA_ST._UpdateCamera();
// 		}

		if( ( net::bPortalMove == true )&&( net::prev_map_no != 0 ) )
		{
			NiPoint3 vSrc = NiPoint3( (float)GetBackupPos().m_nX, (float)GetBackupPos().m_nY, 0 );
			CsMapPortal* pPortalFT = nsCsMapTable::g_pMapPortalMng->GetGroup( net::prev_map_no )->GetNearPortal( net::next_map_no, (int)vSrc.x, (int)vSrc.y );
			if( pPortalFT != NULL )
			{
				vSrc.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vSrc ) + 500.0f;
				NiPoint3 vTarget = NiPoint3( (float)pPortalFT->GetInfo()->s_nViewTargetX, (float)pPortalFT->GetInfo()->s_nViewTargetY, 0 );
				vTarget.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vTarget );
				if( net::next_map_no == 3 )
				{
					vSrc.z = 400.f;	// 카메라 높이를 낮추자...
					CAMERA_ST.SetRotation( NiPoint3(19970.f, 16500.f, 0.f) - vSrc );
					// NiPoint3(19970.f, 16500.f, 0.f) ==> 닷본은 vTarget이 없는 포탈이기 때문에 포탈 앞 특정 포인트를 vTarget처럼 활용하여 계산하도록 함.
				}
				else
					CAMERA_ST.SetRotation( vTarget - vSrc );

				pTamerUser->SetRotation( vTarget, true );
				pDigimonUser->SetRotation( vTarget, true );
				CAMERA_ST._UpdateCamera();
			}
			else
			{
				if(net::prev_map_no != net::next_map_no)	//2017-06-07-nova	
				{
					assert_csm( false, _T( "포탈 발견 못함, 송명수를 찾아주세요" ) );
				}
			}
		}
		else	// 포탈 이동이 아닌 최초 로그인 입장 시...
		{
			if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == 3 )
			{
				CAMERA_ST.SetRotation( NiPoint3(419.f, 1598.f, -400.f) );
				CAMERA_ST._UpdateCamera();
			}
		}
		break;
	case nLIB::SVR_BATTLE:
		{
			CsGBObject* pActor = nsCsGBTerrain::g_pCurRoot->GetActor_Object_NearPos( "door", pDigimonUser->GetPos2D() );
			if( pActor )
			{
				NiPoint3 vSrc = pDigimonUser->GetPos();
				vSrc.z += 500.0f;
				NiPoint3 vTarget = pActor->GetCsNode()->m_pNiNode->GetWorldTranslate();
				CAMERA_ST.SetRotation( vTarget - vSrc );
				pTamerUser->SetRotation( vTarget, true );
				pDigimonUser->SetRotation( vTarget, true );
				CAMERA_ST._UpdateCamera();
			}
		}
		break;
	default:
		assert_cs( false );
	}

	CAMERA_ST.ApplyFrustumPlane( false );

	g_bFirstConnetChar = false;
	Reset();

#ifdef USE_BARCODE_REDER
	// 바코드 상태 리셋
	nsBARCODE::ResetOldState();
#endif

// 	//친구목록 요청
// 	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::SEND_FRIENDLIST, null);
	if( g_bFirstLoding == true && g_pDataMng->GetPCbang()->GetPcBang() == true ) 
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_PCBANG, cBGSprite::PCBANG );	

#ifdef PLAY_PANELTY
	if( g_pDataMng->PlayTime_Get_PaneltyType() == nPlayTime::MAX_PENALTY )
	{
		g_pDataMng->PlayTime_GameResetTime_PrintMsg();
	}
#endif

	// 추가 설정
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon )
		{
			g_pServerMoveOwner = pDigimonUser->GetUserServerMove();
		}
		else
		{
			g_pServerMoveOwner = pTamerUser->GetUserServerMove();
		}
		g_pServerMoveOwner->SetKeyBoardMoveDir();
		g_pServerMoveOwner->KeyAutoMapping();

		if( ( net::bPortalMove == false )||( net::prev_map_no == 0 ) )
		{
			NiPoint3 vCamPos = g_pServerMoveOwner->GetParent()->GetPos();
			vCamPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vCamPos ) + 500.0f;

			CAMERA_ST.SetTranslate( vCamPos );
		}
#else
		g_pServerMoveOwner = pTamerUser->GetUserServerMove();
#endif // KEYBOARD_MOVE
		break;
	case nLIB::SVR_BATTLE:
		g_pServerMoveOwner = pDigimonUser->GetUserServerMove();
#ifdef KEYBOARD_MOVE
		g_pServerMoveOwner->SetKeyBoardMoveDir();
#endif
		g_pServerMoveOwner->KeyAutoMapping();
//		g_pDataMng->GetBattle()->SetBattle_BaseDigimonLv( pDigimonUser->GetBaseStat()->GetLevel() );
		pDigimonUser->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_B.nif" );
		break;
	}

	// 테이머 버프정보 받기
	std::list< sPostBuff >::iterator it;	
	std::list< sPostBuff >::iterator itEnd;
	it = g_pDataMng->GetPostLoad()->GetTBuffList()->begin();
	itEnd = g_pDataMng->GetPostLoad()->GetTBuffList()->end();
	for( ; it != itEnd; it++ )
	{
		( (CTamer*)pTamerUser )->GetBuff()->SetBuff( it->s_nBuffCode, 0, it->s_nBuffEndTS, it->s_nBuffClassLevel );
	}
	g_pDataMng->GetPostLoad()->GetTBuffList()->clear();

	it = g_pDataMng->GetPostLoad()->GetDBuffList()->begin();
	itEnd = g_pDataMng->GetPostLoad()->GetDBuffList()->end();
	for( ; it != itEnd; it++ )
	{
		( (CDigimon*)pDigimonUser )->GetBuff()->SetBuff( it->s_nBuffCode, 0, it->s_nBuffEndTS, it->s_nBuffClassLevel );
	}
	g_pDataMng->GetPostLoad()->GetDBuffList()->clear();

	g_bFirstLoding = false;
}

#ifdef ADVENTURE_TAMER_CHANGE
void cData_PostLoad::PostLoadT()
{	
	// 테이머 생성
	//assert_cs( nsPART::MAX_TOTAL_COUNT == nLimit::Equip );	

	cItemInfo* pItemInfo = NULL;
	CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];
	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
	{
		if( i >= nTamer::MaxParts )
			pItemInfo = g_pDataMng->GetTEquip()->GetDigiviceItem();
		else
			pItemInfo = g_pDataMng->GetTEquip()->GetData( i );
		SAFE_POINTER_CON( pItemInfo );

		cp[ i ].s_nFileTableID = pItemInfo->GetType();
		cp[ i ].s_nPartIndex = i;
		cp[ i ].s_nRemainTime = pItemInfo->m_nEndTime;

	}

	CTamerUser* pTamerUser = g_pCharMng->AddTamerUser( m_Tamer.s_Type.m_nIDX, m_Tamer.s_Type.m_nType, m_sPos, 0.0f, m_Tamer.s_szName, cp, m_dwSyncOption );
	m_Tamer.ToObject( pTamerUser, true );	
	//DWORD dwPlag = nsEFFECT::POS_CHARPOS;
	pTamerUser->SetServerOption( m_dwServerOption );
	pTamerUser->GetCharOption()->SetAchieve( g_pDataMng->GetAchieve()->GetDispTitle() );
	pTamerUser->SetDigimonLink( m_Digimon.s_Type.m_nIDX );
	pTamerUser->GetProp_Effect()->AddEffect_FT( EFFECT_CREATE_SCENE );	
	pTamerUser->ResetOldFatiguePoint();
	pTamerUser->SetServerRelocateCompletedTime( m_nServerRelocateCompletedTime );

	for( int i=0; i<nLimit::Skill; ++i )
	{
		if( m_Tamer.s_nTamerCashSkill_LifeTime[i] == 0 || m_Tamer.s_nTamerCashSkill_Index[i] == 0)
			continue;

		// 캐쉬 테이머 스킬 세팅
		g_pCharMng->GetTamerUser()->GetSkillMng()->SetTamerCashSkill( m_Tamer.s_nTamerCashSkill_Index[i], m_Tamer.s_nTamerCashSkill_CoolTime[i], m_Tamer.s_nTamerCashSkill_LifeTime[i] );
	}

	// 세팅 후 초기화 해주자
	for( int i=0; i<nLimit::Skill; ++i )
	{
		m_Tamer.s_nTamerCashSkill_Index[i] = 0;
		m_Tamer.s_nTamerCashSkill_CoolTime[i] = 0;
		m_Tamer.s_nTamerCashSkill_LifeTime[i] = 0;
	}

	// Cash 스킬 퀵슬롯 등록
	cResist::sCHAR_QUICKSLOT::sCHAR* pCashQuickSlot = g_pDataMng->GetTamerCashQuickSlot();
	if( pCashQuickSlot->s_bInitial == true )
	{
		cResist::sCHAR_QUICKSLOT::sCHAR* pTamerQuick = g_pCharMng->GetTamerUser()->GetQuickSlot();
		for( int i=0; i<8; ++i )
		{
			if( pCashQuickSlot->s_QuickSlotInfo[i].s_nID == pTamerQuick->s_QuickSlotInfo[i].s_nID )
				continue;

			if(pCashQuickSlot->s_QuickSlotInfo[i].s_eType == cResist::ITEM )
				continue;
			
			pTamerQuick->s_QuickSlotInfo[i].s_eType = pCashQuickSlot->s_QuickSlotInfo[i].s_eType;
			pTamerQuick->s_QuickSlotInfo[i].s_nID = pCashQuickSlot->s_QuickSlotInfo[i].s_nID;
		}
	}

	int nCount = g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkillCount();	// 테이머 스킬 총 개수

	for( int i=0; i<nLimit::Skill; ++i )
	{
		if( m_Tamer.s_nTamerSkillCode[i] == 0 )
			continue;

		for( int k=0; k<nCount; ++k )
		{			
			int nSkill_Index = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerSkill_Index( m_Tamer.s_nTamerSkillCode[i] );
			if( nSkill_Index == -1 )
				continue;

			if( m_Tamer.s_nTamerSkillCoolTime[i] < _TIME_TS + 1 )
				break;

			cSkill::sINFO* pSkill_Info = pTamerUser->GetSkillMng()->GetSkill( nSkill_Index );	// 사용한 테이머스킬 정보 얻어옴

			pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );
			double fOffset = pSkill_Info->s_CoolTime.GetEndTime() - ( m_Tamer.s_nTamerSkillCoolTime[i] - _TIME_TS ) - 1;
			pSkill_Info->s_CoolTime.Start( fOffset );
		}
	}

	for( int i=0; i<nLimit::Skill; ++i )	// 사용했으면 초기화 해주자!
	{
		m_Tamer.s_nTamerSkillCode[i] = 0;
		m_Tamer.s_nTamerSkillCoolTime[i] = 0;
	}

#ifdef PC_BANG_SERVICE
	if( g_pDataMng->GetPCbang()->GetCostume() != 0 )
		pTamerUser->SetPcbangCostume( g_pDataMng->GetPCbang()->GetCostume() );
#else
// 	if( g_pDataMng->GetPCbang()->GetCostume() != 0 )
// 		pTamerUser->SetPcbangCostume( g_pDataMng->GetPCbang()->GetCostume() );
#endif
	
	// 디지몬 생성
	CDigimonUser* pDigimonUser = g_pCharMng->AddDigimonUser( 0, m_Digimon.s_Type.m_nIDX, m_Digimon.s_Type.m_nType, m_Digimon.s_dwBaseDigimonID, m_sPos, 0.0f, m_Digimon.s_szName, true );
	m_Digimon.ToObject( pDigimonUser, true );
	pDigimonUser->SetTamerLink( m_Tamer.s_Type.m_nIDX );
	pDigimonUser->GetProp_Effect()->AddEffect_FT( EFFECT_CREATE_SCENE );
	


	// 추가 설정
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		g_pServerMoveOwner = pTamerUser->GetUserServerMove();
		break;
	case nLIB::SVR_BATTLE:
		g_pServerMoveOwner = pDigimonUser->GetUserServerMove();
		g_pServerMoveOwner->KeyAutoMapping();
		g_pDataMng->GetBattle()->SetBattle_BaseDigimonLv( pDigimonUser->GetBaseStat()->GetLevel() );
		pDigimonUser->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_B.nif" );
		break;
	}

	// 테이머 버프정보 받기
	std::list< sPostBuff >::iterator it;	
	std::list< sPostBuff >::iterator itEnd;
	it = g_pDataMng->GetPostLoad()->GetTBuffList()->begin();
	itEnd = g_pDataMng->GetPostLoad()->GetTBuffList()->end();
	for( ; it != itEnd; it++ )
	{
		( (CTamer*)pTamerUser )->GetBuff()->SetBuff( it->s_nBuffCode, 0, it->s_nBuffEndTS, it->s_nBuffClassLevel );
	}
	g_pDataMng->GetPostLoad()->GetTBuffList()->clear();

	it = g_pDataMng->GetPostLoad()->GetDBuffList()->begin();
	itEnd = g_pDataMng->GetPostLoad()->GetDBuffList()->end();
	for( ; it != itEnd; it++ )
	{
		( (CDigimon*)pDigimonUser )->GetBuff()->SetBuff( it->s_nBuffCode, 0, it->s_nBuffEndTS, it->s_nBuffClassLevel );
	}
	g_pDataMng->GetPostLoad()->GetDBuffList()->clear();

	g_bFirstLoding = false;
}
#endif

void cData_PostLoad::PostLoad_Tutorial()
{
	if( m_bNetOffInitPostLoad == true )
	{
		_NetOff_InitPostLoad();
	}

	// 테이머 생성
	cItemInfo* pItemInfo = NULL;
	CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];

	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
	{
		if( i >= nTamer::MaxParts )
			pItemInfo = g_pDataMng->GetTEquip()->GetDigiviceItem();
		else
			pItemInfo = g_pDataMng->GetTEquip()->GetData( i );
		SAFE_POINTER_CON( pItemInfo );

		cp[ i ].s_nFileTableID = pItemInfo->GetType();
		cp[ i ].s_nPartIndex = i;
		cp[ i ].s_nRemainTime = pItemInfo->m_nEndTime;
	}

	CTamerUser* pTamerUser = g_pCharMng->AddTamerUser_Tutorial( m_Tamer.s_Type.m_nIDX, m_Tamer.s_Type.m_nType, m_sPos, 0.0f, m_Tamer.s_szName, cp, m_dwSyncOption );
	m_Tamer.ToObject( pTamerUser, true );	
	//DWORD dwPlag = nsEFFECT::POS_CHARPOS;
	pTamerUser->SetServerOption( m_dwServerOption );
	pTamerUser->SetServerRelocateCompletedTime( m_nServerRelocateCompletedTime );

	DWORD dispTitle;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_GET_DISPTITLE, &dispTitle);

	pTamerUser->GetCharOption()->SetAchieve( dispTitle );
	pTamerUser->SetDigimonLink( m_Digimon.s_Type.m_nIDX );
	pTamerUser->GetProp_Effect()->AddEffect_FT( EFFECT_CREATE_SCENE );	
	pTamerUser->ResetOldFatiguePoint();
#ifdef MEGAPHONE_BAN
	pTamerUser->SetMegaPhoneBan( m_Tamer.s_bMegaPhoneBan );
#endif

	int nCount = g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkillCount();	// 테이머 스킬 총 개수

	for( int i=0; i<nLimit::Skill; ++i )
	{
		if( m_Tamer.s_nTamerSkillCode[i] == 0 )
			continue;

		for( int k=0; k<nCount; ++k )
		{			
			int nSkill_Index = g_pCharMng->GetTamerUser()->GetSkillMng()->GetTamerSkill_Index( m_Tamer.s_nTamerSkillCode[i] );
			if( nSkill_Index == -1 )
				continue;

			if( m_Tamer.s_nTamerSkillCoolTime[i] < _TIME_TS + 1 )
				break;

			cSkill::sINFO* pSkill_Info = pTamerUser->GetSkillMng()->GetSkill( nSkill_Index );	// 사용한 테이머스킬 정보 얻어옴

			pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );
			double fOffset = pSkill_Info->s_CoolTime.GetEndTime() - ( m_Tamer.s_nTamerSkillCoolTime[i] - _TIME_TS ) - 1;
			pSkill_Info->s_CoolTime.Start( fOffset );
		}
	}

	for( int i=0; i<nLimit::Skill; ++i )	// 사용했으면 초기화 해주자!
	{
		m_Tamer.s_nTamerSkillCode[i] = 0;
		m_Tamer.s_nTamerSkillCoolTime[i] = 0;
	}

	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( 5 );
	pDataQuickSlot->SetChar( NULL );

	// 디지몬 생성
	CDigimonUser* pDigimonUser = g_pCharMng->AddDigimonUser_Tutorial( 0, m_Digimon.s_Type.m_nIDX, m_Digimon.s_Type.m_nType, m_Digimon.s_dwBaseDigimonID, NULL, m_sPos, 0.0f, m_Digimon.s_szName, true );
	m_Digimon.ToObject( pDigimonUser, true );
	pDigimonUser->SetTamerLink( m_Tamer.s_Type.m_nIDX );
	pDigimonUser->GetProp_Effect()->AddEffect_FT( EFFECT_CREATE_SCENE );

	if( IsNetOff() == false )
	{
		g_pCharMng->GetDigimonUser( 0 )->SetServerInitPos();
	}

	// 퀘스트 초기화
	cData_Quest* pDataQuest = g_pDataMng->GetQuest();
	if( pDataQuest )
		pDataQuest->PostRevPostLoad( net::next_map_no );

	//===================================================================================================================================
	// 카메라
	//===================================================================================================================================
	// 게임내 카메라 셋팅
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		if( ( net::bPortalMove == true )&&( net::prev_map_no != 0 ) )
		{
			NiPoint3 vSrc = NiPoint3( (float)GetBackupPos().m_nX, (float)GetBackupPos().m_nY, 0 );
			CsMapPortal* pPortalFT = nsCsMapTable::g_pMapPortalMng->GetGroup( net::prev_map_no )->GetNearPortal( net::next_map_no, (int)vSrc.x, (int)vSrc.y );
			if( pPortalFT != NULL )
			{
				vSrc.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vSrc ) + 500.0f;
				NiPoint3 vTarget = NiPoint3( (float)pPortalFT->GetInfo()->s_nViewTargetX, (float)pPortalFT->GetInfo()->s_nViewTargetY, 0 );
				vTarget.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vTarget );
				if( net::next_map_no == 3 )
				{
					vSrc.z = 400.f;	// 카메라 높이를 낮추자...
					CAMERA_ST.SetRotation( NiPoint3(19970.f, 16500.f, 0.f) - vSrc );
					// NiPoint3(19970.f, 16500.f, 0.f) ==> 닷본은 vTarget이 없는 포탈이기 때문에 포탈 앞 특정 포인트를 vTarget처럼 활용하여 계산하도록 함.
				}
				else
					CAMERA_ST.SetRotation( vTarget - vSrc );

				pTamerUser->SetRotation( vTarget, true );
				pDigimonUser->SetRotation( vTarget, true );
				CAMERA_ST._UpdateCamera();
			}
			else
			{
				if(net::prev_map_no != net::next_map_no)	//2017-06-07-nova	
				{
					assert_csm( false, _T( "포탈 발견 못함, 송명수를 찾아주세요" ) );
				}
			}
		}
		else	// 포탈 이동이 아닌 최초 로그인 입장 시...
		{
			if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == 3 )
			{
				CAMERA_ST.SetRotation( NiPoint3(419.f, 1598.f, -400.f) );
				CAMERA_ST._UpdateCamera();
			}
		}
		break;
	}

	CAMERA_ST.ApplyFrustumPlane( false );

	g_bFirstConnetChar = false;
	Reset();

#ifdef USE_BARCODE_REDER
	// 바코드 상태 리셋
	nsBARCODE::ResetOldState();
#endif

	// 추가 설정
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon )
		{
			g_pServerMoveOwner = pDigimonUser->GetUserServerMove();
		}
		else
		{
			g_pServerMoveOwner = pTamerUser->GetUserServerMove();
		}
		g_pServerMoveOwner->SetKeyBoardMoveDir();
		g_pServerMoveOwner->KeyAutoMapping();

		if( ( net::bPortalMove == false )||( net::prev_map_no == 0 ) )
		{
			NiPoint3 vCamPos = g_pServerMoveOwner->GetParent()->GetPos();
			vCamPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vCamPos ) + 500.0f;

			CAMERA_ST.SetTranslate( vCamPos );
		}
#else
		g_pServerMoveOwner = pTamerUser->GetUserServerMove();
#endif // KEYBOARD_MOVE
		break;
	}

	// 버프정보 초기화
	m_PostTBuff.clear();
	m_PostDBuff.clear();

	g_bFirstLoding = false;
}

void cData_PostLoad::AddPartsInfo(UINT const& nDigimonFTID, int const& nPartsItemID)
{
	std::string strFile;
	int nPartsIndex = nsDPART::None;
	if( nsCsFileTable::g_pItemMng->GetDPartsInfo( nPartsItemID, nDigimonFTID, nPartsIndex, strFile ) )
	{
		nsDPART::sDPART_INFO partsInfo;
		partsInfo.s_nPartIndex = nPartsIndex;
		partsInfo.s_nFileTableID = nDigimonFTID;
		partsInfo.s_File = strFile;

		std::map< int, sPartsData >::iterator it = m_mapDParts.find( nPartsIndex );
		if( it == m_mapDParts.end() )
		{
			auto ib = m_mapDParts.insert( std::make_pair( nPartsIndex, sPartsData() ) );
			if( !ib.second )
				return;

			it = ib.first;
		}

		it->second.s_nPartsItemID = nPartsItemID;
		it->second.s_strFile = strFile;
	}
}

void cData_PostLoad::ClearPartsInfo()
{
	m_mapDParts.clear();
}
