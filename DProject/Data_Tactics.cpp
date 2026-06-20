
#include "stdafx.h"
#include "Data_Tactics.h"

cData_Tactics::cData_Tactics()
{

}

void cData_Tactics::Delete()
{
	for( int i=0; i<nLimit::Memory; ++i )
	{
		m_Tactics[ i ].Delete();
	}

#ifdef CROSSWARS_SYSTEM
	m_TempTactics.Delete();
#endif
}

void cData_Tactics::Init()
{
	for( int i=0; i<nLimit::Memory; ++i )
	{
		m_Tactics[ i ].Delete();
	}

#ifdef CROSSWARS_SYSTEM
	m_TempTactics.Delete();
#endif
	// 용병슬롯 갯수 - 임시
	SetTacticsCount( nLimit::DigimonBaseSlot - 1 );// (파트너몬+용병몬) - 1(파트너몬)
	m_nOpenSlotCnt = 0;
}

void cData_Tactics::InitNetOff()
{
	m_Tactics[ 0 ].s_Type.m_nType = 31007;
	_stprintf_s( m_Tactics[ 0 ].s_szName, Language::pLength::name + 1, _T( "용병1" ) );

	m_Tactics[ 1 ].s_Type.m_nType = 31138;
	_stprintf_s( m_Tactics[ 1 ].s_szName, Language::pLength::name + 1, _T( "용병2" ) );

	for( int i=0; i<2; ++i )
	{
		m_Tactics[ i ].s_Type.m_nIDX = cType::GetTestIDX( nClass::Digimon );
		m_Tactics[ i ].s_dwBaseDigimonID = m_Tactics[ i ].s_Type.m_nType;

		m_Tactics[ i ].s_nLevel = 75;
		m_Tactics[ i ].s_fScale = 1.0f;		
		m_Tactics[ i ].s_nMaxEvoUnit = nLimit::EvoUnit - 1;

		for( int e=0; e<nLimit::EvoUnit; ++e )
		{
			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExp = 0;
			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillExpLevel = 0;
			m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillPoint = 100;
			m_Tactics[ i ].s_EvoUnit[ e ].m_nSlotState = 0;
			for( int s=0; s<nLimit::Skill; ++s )
				m_Tactics[ i ].s_EvoUnit[ e ].m_nSkillLevel[ s ] = 1;		
		}
		m_Tactics[ i ].s_EvoUnit[ 1 ].m_nSlotState = 1;
		m_Tactics[ i ].s_EvoUnit[ 2 ].m_nSlotState = 1;
		m_Tactics[ i ].s_EvoUnit[ 3 ].m_nSlotState = 1;
		m_Tactics[ i ].s_EvoUnit[ 4 ].m_nSlotState = 1;
		m_Tactics[ i ].s_EvoUnit[ 5 ].m_nSlotState = 0;
		m_Tactics[ i ].s_EvoUnit[ 6 ].m_nSlotState = 1;
	}	

}

cData_PostLoad::sDATA*
cData_Tactics::GetEmptyTactics()
{
	for( int i=0; i<m_nOpenSlotCnt; ++i )
	{
		if( m_Tactics[ i ].s_Type.m_nType == 0 )
			return &m_Tactics[ i ];
	}
	return NULL;
}

void cData_Tactics::LinkTactics()
{
	for( int i=0; i<m_nTacticsCount; ++i )
	{
		g_pGameIF->GetTacticsWindow( i )->SetTargetObject( m_Tactics[ i ].s_dwBaseDigimonID );
	}	
}

int cData_Tactics::GetMercenaryID( DWORD dwMercenaryID )
{
	for( int i=0; i<m_nTacticsCount; ++i )
	{		
		DWORD dwID = g_pGameIF->GetTacticsWindow( i )->GetTargetDigimonFTID();
		if(dwMercenaryID  == dwID)
			return i;
	}
	return INVALIDE_TACTICS_INDEX;
}

void cData_Tactics::AddTactics( cData_PostLoad::sDATA* pSrcTactics, int nTacticsIndex )
{
	assert_cs( nTacticsIndex < m_nTacticsCount );

	cData_PostLoad::sDATA* pDestTactics = GetTactics( nTacticsIndex );
	assert_cs( pDestTactics->s_Type.m_nType == 0 );
	memcpy( pDestTactics, pSrcTactics, sizeof( cData_PostLoad::sDATA ) );

	// IF에 연결
	g_pGameIF->GetTacticsWindow( nTacticsIndex )->SetTargetObject( pDestTactics->s_dwBaseDigimonID );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

void cData_Tactics::DeleteTactics( int nTacticsIndex )
{
	assert_cs( nTacticsIndex < m_nTacticsCount );		
	m_Tactics[ nTacticsIndex ].Delete();
	// IF에 연결
	g_pGameIF->GetTacticsWindow( nTacticsIndex )->SetTargetObject( 0 );	
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}
// pScr 현재 들고있는 디지몬 // pCurData 현재들고있는 용병 정보 // 
void cData_Tactics::ChangeTactics( CsC_AvObject* pSrc, int nDestTacticsIndex )
{
	assert_cs( nDestTacticsIndex < m_nTacticsCount );
	assert_cs( pSrc->GetLeafRTTI() == RTTI_DIGIMON_USER );	

	// 백업
	cData_PostLoad::sDATA* pCurData = g_pDataMng->GetPostLoad()->GetDigimonData();

	// 기존 정보 변경.	DATA_TRADE_SYSTEM 데이터 교환시스템을 사용시에는 필요 없음..
	pCurData->s_nLevel = pSrc->GetBaseStat()->GetLevel();
	pCurData->s_nExp = pSrc->GetBaseStat()->GetExp();
	pCurData->s_fScale = pSrc->GetOrgGameScale();
	memcpy( pCurData->s_EvoUnit, ( (CDigimonUser*)pSrc )->GetAttributeEvoUnit(), sizeof( cEvoUnit)*nLimit::EvoUnit );

	
	cData_PostLoad::sDATA* pData = GetTactics( nDestTacticsIndex );
	pData->ToObject( pSrc, false ); // pSrc 스텟이 pData값으로 변경된다.

	cData_PostLoad::sDATA backup = *pData;
	*pData = *pCurData;
	*pCurData = backup;

	// IF에 연결
	g_pGameIF->GetTacticsWindow( nDestTacticsIndex )->SetTargetObject( pData->s_dwBaseDigimonID );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

void cData_Tactics::ChangeStatus(cData_PostLoad::sDATA*	pData, CsC_AvObject* pSrc)
{
	// void cData_Tactics::ChangeTactics( CsC_AvObject* pSrc, int nDestTacticsIndex )
	// ChangeTactics() 함수에서 이전 데이터 교체 해주던 부분을 제거함.
	// 최신 데이터를 교체하기 이전에 DigimonUser 정보가 교체 되어 증발 하기 때문에 여기서 모두 교체.
	CDigimon* pCDigimon = (CDigimon*)pSrc;
	CDigimon::sENCHENT_STAT* pEStat = pCDigimon->GetEnchantStat();

	// 레벨 경험치 정보 교환.
	pData->s_nLevel = pSrc->GetBaseStat()->GetLevel();
	pData->s_nExp = pSrc->GetBaseStat()->GetExp();

	// 크기 정보 최신 정보로 교체.
	//pData->s_fScale = pSrc->GetOrgGameScale();
	pData->s_fScale = pSrc->GetGameScale();

	// 강화 정보 최신 정보로 교체.
	pData->s_ExtendAttributeLV[ET_AT] = pEStat->GetEnchantLv( ET_AT );
	pData->s_ExtendAttributeLV[ET_BL] = pEStat->GetEnchantLv( ET_BL );
	pData->s_ExtendAttributeLV[ET_CR] = pEStat->GetEnchantLv( ET_CR );
	pData->s_ExtendAttributeLV[ET_EV] = pEStat->GetEnchantLv( ET_EV );
	pData->s_ExtendAttributeLV[ET_HP] = pEStat->GetEnchantLv( ET_HP );
	pData->s_nEnchantLevel = pData->s_ExtendAttributeLV[ET_AT] +
							 pData->s_ExtendAttributeLV[ET_BL] +
							 pData->s_ExtendAttributeLV[ET_CR] +
							 pData->s_ExtendAttributeLV[ET_EV] +
							 pData->s_ExtendAttributeLV[ET_HP] + 1;

	// 진화 정보 최신 정보로 교체.
	memcpy( pData->s_EvoUnit, ( (CDigimonUser*)pSrc )->GetAttributeEvoUnit(), sizeof( cEvoUnit)*nLimit::EvoUnit );

}


cData_PostLoad::sDATA* cData_Tactics::GetTactics( int nTacticsIndex )
{
	if( nTacticsIndex >= nLimit::Memory )
		return NULL;
	return &m_Tactics[ nTacticsIndex ]; 
}

bool cData_Tactics::IsTactics( int nTacticsIndex ) const
{
	if( nTacticsIndex >= nLimit::Memory )
		return false;
	return ( m_Tactics[ nTacticsIndex ].s_Type.m_nType != 0 ); 
}

#ifdef CROSSWARS_SYSTEM
// 크로스에서 일반 디지몬으로..
void cData_Tactics::ChangeCrossToTactics( CsC_AvObject* pSrc, int nDestTacticsIndex )
{	
	assert_cs( pSrc->GetLeafRTTI() == RTTI_DIGIMON_USER );	

	// 이름쪽은 노 신경
	/*cData_PostLoad::sDATA SrcData;
	SrcData.FromObject( pSrc );*/	

	cData_PostLoad::sDATA* pCurData = g_pDataMng->GetPostLoad()->GetDigimonData();	

	pCurData->s_nLevel = pSrc->GetBaseStat()->GetLevel();
	pCurData->s_nExp = pSrc->GetBaseStat()->GetExp();
	pCurData->s_fScale = pSrc->GetOrgGameScale();
	memcpy( pCurData->s_EvoUnit, ( (CDigimonUser*)pSrc )->GetAttributeEvoUnit(), sizeof( cEvoUnit)*nLimit::EvoUnit );
	
	cData_PostLoad::sDATA* pData = g_pDataMng->GetTactics()->GetTempTactics(); // RecvCrossWarsRelease 할때 서버에서 받은 일반디지몬 정보 
	pData->ToObject( pSrc, false ); // pSrc 스텟이 pData값으로 변경된다.

	//(갱신된 정보)pCurData 를 해당 크로스 디지몬 슬롯에 넣어줘야한다.

	bool IsCrossCombine = nsCsFileTable::g_pDigimonMng->IsCrossCombine(pCurData->s_dwBaseDigimonID);
	
	if( !IsCrossCombine ){
		cData_PostLoad::sDATA* pCrossData = g_pDataMng->GetCrossTactics()->GetTacticsByBaseDigimonID( pCurData->s_dwBaseDigimonID );
		*pCrossData = *pCurData;
	}
	
	*pCurData = *pData;


	// IF에 연결
	//g_pGameIF->GetTacticsWindow( nDestTacticsIndex )->SetTargetObject( pData->s_dwBaseDigimonID );
}

#endif

void cData_Tactics::_SetOpenSlot( int nCnt ) 
{ 
	m_nOpenSlotCnt = nCnt;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}
int	 cData_Tactics::_GetOpenSlot() { return m_nOpenSlotCnt; }

bool cData_Tactics::Check_EnableChange()
{
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	// 로드 체크
	if( pUser->IsLoad() == false )
	{
		return false;
	}

	// 디지몬 상태이상 체크
	if( pUser->GetBuff()->IsUnusualCond( cBuffData::UNUSUAL_CHANGE ) == true ) 
	{
		cPrintMsg::PrintMsg( 30455 );
		return false;
	}

	// 노말 상태 인지 체크
	switch( pUser->GetDigimonState() )
	{	
	case CDigimon::DIGIMON_NORMAL:
		break;
	case CDigimon::DIGIMON_BATTLE:
		{
			SAFE_POINTER_RETVAL( nsCsMapTable::g_pMapListMng, false );
			SAFE_POINTER_RETVAL( nsCsGBTerrain::g_pCurRoot, false );
			CsGBTerrainRoot::sINFO* pMapInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
			SAFE_POINTER_RETVAL( pMapInfo, false );
			
			if( !nsCsMapTable::g_pMapListMng->IsBattleTagUseMap( pMapInfo->s_dwMapID ) )
			{
				cPrintMsg::PrintMsg( 30344 );
				return false;
			}

			// 전투중 교체 아이템 있는지 체크		
			if( g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( nItem::BattleTag, 0 ) == cData_Inven::INVALIDE_INVEN_INDEX )
			{
				cPrintMsg::PrintMsg( 30057 );
				return false;
			}
		}
		break;
	default:
		cPrintMsg::PrintMsg( 30058 );
		return false;
	}

	// 진화가능한 상태인지 체크 - 진화가 가능한상태여야 교체도 가능
	if( pUser->GetProp_Animation()->IsEnableEvol() == false )
	{
		cPrintMsg::PrintMsg( 30058 );
		return false;
	}

	// 라이딩 상태인지 체크
	if( pUser->IsRide() )
	{
		cPrintMsg::PrintMsg( 30014 );
		return false;
	}

	// 일반 화면이 아닐때 ( 닷본, 용병부화, 이벤트중 ) 
	if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_NONE )
	{
		cPrintMsg::PrintMsg( 30058 );
		return false;
	}

	return true;
}


// 조그레스 상대방 용병중 슬롯이 뚫려있고 가장 낮은 레벨을 리턴한다.
cData_PostLoad::sDATA*
cData_Tactics::GetJoGressMercenaries( DWORD const& dwOpenedJogressID, USHORT nMercenary, int* arrMercenaries, int& nSlotIndex, int& nGetError )	/*인자값1: 상대 디지몬 용병수, 인자값2: 상대 디지몬 용병 배열*/
{	
	cData_PostLoad::sDATA*		pMercenary;
	int			iHave=0, iHaveSlot=0 ,iLowLevel = 0;
	int			i=0,j =0;	
	
	int			ntemp[ nLimit::Memory ];	// 가지고있는 용병 인덱스 저장
	memset(ntemp, -1, sizeof(int)*nLimit::Memory);
	int			ntemp2[ nLimit::Memory ];	// 슬롯뚫린 용병 인덱스 저장
	memset(ntemp2, -1, sizeof(int)*nLimit::Memory);

	
	// 조그레스 상대방 용병있는지 확인
	for( i = 0; i < nMercenary; i++ )	// 테이블의 값을 돌려서
	{
		int usMer = arrMercenaries[i];	// 테이블정의된 용병값
		for( j =0; j < m_nTacticsCount; j ++ )	// 자기의 용병슬롯에 가지고 있는 값
		{
			if(m_Tactics[j].s_dwBaseDigimonID == usMer)	// 용병슬롯인덱스와 지정된 테이블값이 일치하면 첫번째 조건(내 슬롯에 상대 조그레스 디지몬이 있나) 이 성립
			{
				ntemp[iHave++] = j;	// 용병 가지고 있으면 인덱스 저장
				nGetError = 1;	// 다음 에러 값으로 변경
			}
		}
	}
	if(0 == iHave) return NULL;

	if( iHave < nMercenary ) // 진화에 필요한 디지몬 개체 수와 가지고 있는 디지몬 수가 같지 않다면...
	{
		nGetError = 0;
		return NULL;
	}

	// 용병들중 조그레스 슬롯이 뚤려있는지?
	for( i = 0; i < iHave; i++ )	
	{
		if( IsSlotOpenedJoGressMercenaries(dwOpenedJogressID, &m_Tactics[ntemp[i]]) ) // 그 용병의 조그레스 슬롯이 뚫려있으면
		{
			ntemp2[iHaveSlot++] = i;	// 용병들 중 슬롯뚤려 있으면 인덱스 저장
			nGetError = 2;	// 다음 에러 값으로 변경
		}
	}
	if(0 == iHaveSlot) return NULL;

	if( iHaveSlot < nMercenary ) // 진화에 필요한 디지몬의 진화 슬롯이 뚫려 있지 않다면..
	{
		nGetError = 1;
		return NULL;
	}

	n2	lv = 999; 
	// 슬롯 뚫린 용병들중 가장 낮은 레벨
	for( i = 0; i < iHaveSlot; i++ )	
	{

		if(lv >  m_Tactics[ ntemp[ ntemp2[i] ] ].s_nLevel)
		{
			lv = m_Tactics[ ntemp[ ntemp2[i] ] ].s_nLevel;
			pMercenary = &m_Tactics[ ntemp[ ntemp2[i] ] ];
			nSlotIndex = ntemp[ ntemp2[i] ];	// 해당되는 디지몬의 슬롯인덱스
/*			iLowLevel++;*/
			nGetError = 3;
		}
	}
	return pMercenary; 
	
}

cData_PostLoad::sDATA*
cData_Tactics::GetJoGressMercenaries( DWORD const& dwOpenedJogressID, USHORT nMercenary, int* arrMercenaries, int& nSlotIndex, int& nGetError, int* pIndex )
{	
	cData_PostLoad::sDATA*		pMercenary;
	int			iHave=0, iHaveSlot=0 ,iLowLevel = 0;
	int			i=0,j =0;	

	int			ntemp[ nLimit::Memory ];	// 가지고있는 용병 인덱스 저장
	memset(ntemp, -1, sizeof(int)*nLimit::Memory);
	int			ntemp2[ nLimit::Memory ];	// 슬롯뚫린 용병 인덱스 저장
	memset(ntemp2, -1, sizeof(int)*nLimit::Memory);


	// 조그레스 상대방 용병있는지 확인
	for( i = 0; i < nMercenary; i++ )	// 테이블의 값을 돌려서
	{
		int usMer = arrMercenaries[i];	// 테이블정의된 용병값
		for( j =0; j < m_nTacticsCount; j ++ )	// 자기의 용병슬롯에 가지고 있는 값
		{
			if(m_Tactics[j].s_dwBaseDigimonID == usMer)	// 용병슬롯인덱스와 지정된 테이블값이 일치하면 첫번째 조건(내 슬롯에 상대 조그레스 디지몬이 있나) 이 성립
			{
				ntemp[iHave++] = j;	// 용병 가지고 있으면 인덱스 저장
				nGetError = 1;	// 다음 에러 값으로 변경
				pIndex[i] = j;
			}
		}
	}
	if(0 == iHave) return NULL;

	if( iHave < nMercenary ) // 진화에 필요한 디지몬 개체 수와 가지고 있는 디지몬 수가 같지 않다면...
	{
		nGetError = 0;
		return NULL;
	}

	// 용병들중 조그레스 슬롯이 뚤려있는지?
	for( i = 0; i < iHave; i++ )	
	{
		if( IsSlotOpenedJoGressMercenaries(dwOpenedJogressID, &m_Tactics[ntemp[i]]) ) // 그 용병의 조그레스 슬롯이 뚫려있으면
		{
			ntemp2[iHaveSlot++] = i;	// 용병들 중 슬롯뚤려 있으면 인덱스 저장
			nGetError = 2;	// 다음 에러 값으로 변경
		}
	}
	if(0 == iHaveSlot) return NULL;
	n2	lv = 999; 
	// 슬롯 뚫린 용병들중 가장 낮은 레벨
	for( i = 0; i < iHaveSlot; i++ )	
	{

		if(lv >  m_Tactics[ ntemp[ ntemp2[iLowLevel] ] ].s_nLevel)
		{
			lv = m_Tactics[ ntemp[ ntemp2[iLowLevel] ] ].s_nLevel;
			pMercenary = &m_Tactics[ ntemp[ ntemp2[iLowLevel] ] ];
			nSlotIndex = ntemp[ ntemp2[iLowLevel] ];	// 해당되는 디지몬의 슬롯인덱스
			iLowLevel++;
			nGetError = 3;
		}
	}

	return pMercenary; 

}

cData_PostLoad::sDATA* 	cData_Tactics::GetXJogressMercenaries( DWORD const& dwOpenedJogressID,  USHORT	nMercenary, int* arrMercenaries, int& nSlotIndex, int& nGetError)
{
#ifdef SDM_TAMER_XGUAGE_20180628
	cData_PostLoad::sDATA*		pMercenary;
	int			iHave=0, iOpenedSlot=0 ,iLowLevel = 0;
	int			i=0,j =0;	
	int			ntemp[ nLimit::Memory ];	// 가지고있는 용병 인덱스 저장
	memset(ntemp, -1, sizeof(int)*nLimit::Memory);
	int			ntemp2[ nLimit::Memory ];	// 슬롯뚫린 용병 인덱스 저장
	memset(ntemp2, -1, sizeof(int)*nLimit::Memory);
///////////////////////////////////////////////////////////////////////////////////////////////
	// 조그레스 상대방 용병있는지 확인
	for( i = 0; i < nMercenary; i++ )	// 테이블의 값을 돌려서
	{
		int usMer = arrMercenaries[i];	// 테이블정의된 용병값
		for( j =0; j < m_nTacticsCount; j ++ )	// 자기의 용병슬롯에 가지고 있는 값
		{
			if(m_Tactics[j].s_dwBaseDigimonID == usMer)	// 용병슬롯인덱스와 지정된 테이블값이 일치하면 첫번째 조건(내 슬롯에 상대 조그레스 디지몬이 있나) 이 성립
			{
				ntemp[iHave++] = j;	// 용병 가지고 있으면 인덱스 저장
				nGetError = 1;	// 다음 에러 값으로 변경
			}
		}
	}
	if(0 == iHave) 
	{
		cPrintMsg::PrintMsg( 30509 );	// 조그레스 진화 상대 디지몬을 소유하고 있지 않습니다
		return NULL;
	}
///////////////////////////////////////////////////////////////////////////////////////////////
	if( iHave < nMercenary ) // 진화에 필요한 디지몬 개체 수와 가지고 있는 디지몬 수가 같지 않다면...
	{
		cPrintMsg::PrintMsg( 30509 );	// 조그레스 진화 상대 디지몬을 소유하고 있지 않습니다
		return NULL;
	}
///////////////////////////////////////////////////////////////////////////////////////////////
	// 용병들중 조그레스 슬롯이 뚤려있는지?
	for( i = 0; i < iHave; i++ )	
	{
		if( IsSlotOpenedJoGressMercenaries(dwOpenedJogressID, &m_Tactics[ntemp[i]])) // 그 용병의 조그레스 슬롯 및 X조그레스 슬롯이 뚫려있으면
		{
			ntemp2[iOpenedSlot++] = i;	// 용병들 중 슬롯뚫려 있으면 인덱스 저장
			nGetError = 2;	// 다음 에러 값으로 변경
		}
	}
//////////////////////////////////////////////////////////////////////////////////////////////
	if(0 == iOpenedSlot)
	{
		cPrintMsg::PrintMsg( 30510 );	// 조그레스 진화 상대 디지몬의 슬롯이 열려있지 않습니다
		return NULL;
	}
	n2	MinLv = 999; 
	for( i = 0; i < iOpenedSlot; i++ )	
	{
		if(MinLv >  m_Tactics[ ntemp[ ntemp2[iLowLevel] ] ].s_nLevel)
		{
			MinLv = m_Tactics[ ntemp[ ntemp2[iLowLevel] ] ].s_nLevel;
			pMercenary = &m_Tactics[ ntemp[ ntemp2[iLowLevel] ] ];
			nSlotIndex = ntemp[ ntemp2[iLowLevel] ];	// 해당되는 디지몬의 슬롯인덱스
			iLowLevel++;
			nGetError = 3;
		}
	}
	return pMercenary; 
#endif

	return NULL;
}

// 상대방의 용병에 조그레스 슬롯과 X조그레스 슬롯이 열려있냐?	
bool cData_Tactics::IsSlotOpenedJoGressMercenaries( DWORD const& dwOpenedJogressID, cData_PostLoad::sDATA* pSrcMercenary )
{
	// 1가지고 있는 용병 검사, 2그 용병에 테이블 진화정보 검사. 칩셋정보있고 퀘스트 정보있으면 조그레스 용병
	// 3 테이블에 퀘스트 정보가지고 퀘스트 완료했는지 검사.

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pEvolMng, false );
	SAFE_POINTER_RETVAL( pSrcMercenary, false );

	for( int i =0; i < nLimit::EvoUnit; i++ )
	{
		// 조그레스 하기위한 상대 디지몬의 테이블정보를 얻는다.
		CDigimonEvolveInfo* pEvInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pSrcMercenary->s_dwBaseDigimonID) ;
		SAFE_POINTER_CON( pEvInfo );

		CDigimonEvolveObj* pFTEvolObj = pEvInfo->GetEvolveObjByEvoSlot( i );
		SAFE_POINTER_CON( pFTEvolObj );

		if( dwOpenedJogressID != pFTEvolObj->m_dwID )
			continue;

		if( pFTEvolObj->m_nEvolutionTree != CDigimonUser::TREE_JOGRESS )
			continue;

		if(pSrcMercenary->s_EvoUnit[i].m_nSlotState & 1 )
			return true;
	}

	return false;
}
