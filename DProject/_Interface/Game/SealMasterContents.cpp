#pragma once 
#include "stdafx.h"
#include "SealMasterContents.h"

int const cSealMasterContents::IsContentsIdentity(void)
{
	return E_CT_SEALMASTER;
}


cSealMasterContents::cSealMasterContents(void):m_nSelectSealID(0),m_nLeader(0),m_nSeadFavoriteSealID(0)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEALMASTER_UI_OPEN_CLOSE, this, &cSealMasterContents::UIOpenClose);			// UI 열기/닫기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SEALMASTER_SEALINFO, this, &cSealMasterContents::RecvSealInfo);		// 씰정보 받기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SEALMASTER_FAVORITE, this, &cSealMasterContents::RecvFavorite);		// 즐겨찾기 정보 받기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SEALMASTER_LEADER, this, &cSealMasterContents::RecvLeader);			// 리더 설정 된 씰
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SEALOPEN, this, &cSealMasterContents::RecvSealOpen);					// 씰 오픈 받기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_RESEALING, this, &cSealMasterContents::RecvReSealing);					// 씰 재봉인 받기

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_SEALMASTER_LEADER, this, &cSealMasterContents::SendLeader);			// 리더 설정 검증 밑 보내기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHECK_SEALMASTER_UNSEAL, this, &cSealMasterContents::CheckUnSeal);			// 언씰 전 언씰 가능 여부 체크

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RE_SEALING_UISTART, this, &cSealMasterContents::ReSealing);					// 다시 씰로 만들기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_SEALMASTER_SELECTSEALID, this, &cSealMasterContents::GetSelectedSealID);// 현재 선택 중인 SEALID 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_SEALMASTER_PLUS_STATE, this, &cSealMasterContents::GetPlusValue);		// 추가된 능력치 합 가져오기
	
	for(int i = 0; i < STATEKINDS; ++i)
		m_pPlusValue[i] =0;
}

cSealMasterContents::~cSealMasterContents(void)
{
	if( GAME_EVENT_STPTR )
		GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const cSealMasterContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cSealMasterContents::Initialize(void)
{
	return true;
}

void cSealMasterContents::UnInitialize(void)
{
}

bool cSealMasterContents::IntraConnection(ContentsSystem& System)
{
	return true; 
}

void cSealMasterContents::Update(float const& fElapsedTime)
{
}

void cSealMasterContents::NotifyEvent(int const& iNotifiedEvt)
{
	return;
}

void cSealMasterContents::MakeMainActorData(void)
{
}

void cSealMasterContents::ClearMainActorData(void)
{
}

void cSealMasterContents::MakeWorldData(void)
{
	SetMapGroupMap();
}

void cSealMasterContents::ClearWorldData(void)
{
	ClearMapGroupMap();
	ClearSealInfoMap();
	for (int i = 0; i < STATEKINDS; ++i)
		m_pPlusValue[i] = 0;	
}

void cSealMasterContents::SetMapGroupMap()
{
	SAFE_POINTER_RET( nsCsFileTable::g_pMaster_CardMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pWorldMapMng );

	DWORD groupID = 0;
	DWORD mapID = 0;
	DWORD sealID = 0;
	CsMaster_Filter::MapGroup* DataMap = nsCsFileTable::g_pMaster_CardMng->GetMasterFilterMap();
	CsMaster_Filter::MapGroup_IT mapGroupItr = DataMap->begin();
	CsMaster_Filter::MapGroup_IT mapGroupItrEnd = DataMap->end();
	for( ; mapGroupItr!=mapGroupItrEnd; ++mapGroupItr )
	{
		groupID = mapGroupItr->first;
		CsMaster_Filter::Map_IT mapItr = mapGroupItr->second->s_sMap.begin();
		CsMaster_Filter::Map_IT mapItrEnd = mapGroupItr->second->s_sMap.end();
		for( ; mapItr!=mapItrEnd; ++mapItr )
		{
			mapID = mapItr->first;
			CsMaster_Filter::Seal_IT sealItr = mapItr->second->s_sSeal.begin();
			CsMaster_Filter::Seal_IT sealItrEnd = mapItr->second->s_sSeal.end();
			for( ; sealItr!=sealItrEnd; ++sealItr )
			{
				sealID = sealItr->first;
				if( IsHaveSeal(groupID, mapID, sealID) )
					continue;
				if( IsHaveGroup( groupID ) == false )
				{
					sMapGroup mapGroupInfo;
					mapGroupInfo.sGoupType = groupID;
					mapGroupInfo.sMapGroupName = nsCsFileTable::g_pWorldMapMng->GetWorldName( groupID );
					mapGroupInfo.sCount++;

					std::pair<DWORD, sMapGroup> mapGroupInfoPair;
					mapGroupInfoPair.first = groupID;
					mapGroupInfoPair.second = mapGroupInfo;
					m_MapGroupMap.insert(mapGroupInfoPair);

					SetMapInfo(groupID, mapID, sealID);
				}
				else
				{
					MGItr gItr = m_MapGroupMap.find(groupID);
					gItr->second.sCount++;
					SetMapInfo(groupID, mapID, sealID);
				}
			}
		}
	}
}
void cSealMasterContents::SetMapInfo(DWORD groupID, DWORD mapID, DWORD sealID)
{
	if( IsHaveSeal(groupID, mapID, sealID) )
		return;

	
	CsMaster_Filter::sINFO*	Data = nsCsFileTable::g_pMaster_CardMng->GetMasterFilter(groupID, mapID, sealID);
	if( IsHaveMap(groupID, mapID) == false)
	{
		sMapInfo mapInfo;
		mapInfo.sMapID = mapID;
		mapInfo.sMapName = nsCsFileTable::g_pWorldMapMng->GetAreaName( mapID );
		mapInfo.sCount++;

		std::pair<DWORD,sMapInfo> mapInfoPair;
		mapInfoPair.first = mapID;
		mapInfoPair.second = mapInfo;
		MGItr gItr = m_MapGroupMap.find(groupID);
		gItr->second.sMapInfoMap.insert(mapInfoPair);
		SetSealInfo(groupID, mapID, sealID);
	}
	else
	{
		MGItr gItr = m_MapGroupMap.find(groupID);
		MItr mItr = gItr->second.sMapInfoMap.find(mapID);
		mItr->second.sCount++;
		SetSealInfo(groupID, mapID, sealID);
	}
	return;
}
void cSealMasterContents::SetSealInfo(DWORD groupID, DWORD mapID, DWORD sealID)
{
	CsMaster_Filter::sINFO* Data = nsCsFileTable::g_pMaster_CardMng->GetMasterFilter(groupID, mapID, sealID);

	if( IsHaveSeal(groupID, mapID, sealID) == false )
	{
		std::pair<DWORD, std::wstring> sealPair;
		sealPair.first = sealID;
		sealPair.second = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(sealID)->GetInfo()->s_szName;
		MGItr gItr = m_MapGroupMap.find(groupID);
		MItr mItr = gItr->second.sMapInfoMap.find(mapID);
		mItr->second.sSealInfoMap.insert(sealPair);

		AddSealInfoVecter(groupID, mapID, sealID);
	}
	else
		return;
}

void cSealMasterContents::ClearMapGroupMap()
{
	m_MapGroupMap.clear();
}

void cSealMasterContents::ClearSealInfoMap()
{
	m_SealInfoMap.clear();
}

bool cSealMasterContents::AddSealInfoVecter(DWORD groupID, DWORD mapID, DWORD sealID)
{
	//g_pMaster_CardMng가 해당 정보를 가지고 있고 && m_SealInfoMap에 없을때만 추가
	if( !nsCsFileTable::g_pMaster_CardMng->IsMasterFilter(groupID, mapID, sealID) )
		return false;

	if( m_SealInfoMap.find(sealID) != m_SealInfoMap.end() )
		return false;

	sSealInfo sealInfo;
	sealInfo.sSealID = sealID;

	CsMaster_Card* pSealMasterInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(sealInfo.sSealID);
	sealInfo.sEffectType = pSealMasterInfo->GetInfo()->s_stGradeInfo[CsMaster_Card::FT_CARD_NORMAL].s_nEff1;
	sealInfo.sEffectValue = 0;
	
	sealInfo.sTableCode = pSealMasterInfo->GetInfo()->s_nID;
	sealInfo.sSealName = pSealMasterInfo->GetInfo()->s_szName;
	sealInfo.sGrade = -1;
	sealInfo.sFavorite = false;
	sealInfo.sModelID = pSealMasterInfo->GetInfo()->s_nDigimonID;
	sealInfo.sSealCount = 0;		

	m_SealInfoMap.insert( make_pair(sealInfo.sSealID, sealInfo) );
	
	return true;
}

void cSealMasterContents::ClearSealInfoVecter()
{
	m_SealInfoMap.clear();
}

const cSealMasterContents::MapGroup& cSealMasterContents::GetMapGroupMap()
{
	return m_MapGroupMap;
}

bool cSealMasterContents::IsHaveGroup(DWORD groupID)
{
	return m_MapGroupMap.find(groupID) != m_MapGroupMap.end() ? true : false;
}

bool cSealMasterContents::IsHaveMap(DWORD groupID, DWORD mapID)
{
	if( !IsHaveGroup(groupID) )
		return false;

	MGItr gItr = m_MapGroupMap.find(groupID);
	MItr mItr = gItr->second.sMapInfoMap.find(mapID);
	MItr mItrEnd = gItr->second.sMapInfoMap.end();
	return mItr != mItrEnd ? true : false;
}

bool cSealMasterContents::IsHaveSeal(DWORD groupID, DWORD mapID, DWORD sealID)
{
	if( !IsHaveMap(groupID, mapID) )
		return false;

	MGItr gItr = m_MapGroupMap.find(groupID);
	MItr mItr = gItr->second.sMapInfoMap.find(mapID);
	MItr mItrEnd = gItr->second.sMapInfoMap.end();
	return mItr->second.sSealInfoMap.find(sealID) != mItr->second.sSealInfoMap.end() ? true : false;
}

const cSealMasterContents::SealInfoMap& cSealMasterContents::GetSealInfoMap()
{
	return m_SealInfoMap;
}

// 리더 설정 된 씰
void cSealMasterContents::RecvLeader(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SEALMASTER_LEADER* pCastData = static_cast<GS2C_RECV_SEALMASTER_LEADER*>(pData);
	m_nLeader = pCastData->m_nLeader;
#ifdef CROSSWARS_SYSTEM
	cData_PostLoad::sTAMERDATA*			Tamer;
#else
	cData_PostLoad::sDATA*			Tamer;
#endif
	Tamer = g_pDataMng->GetPostLoad()->GetTamerData();
	SAFE_POINTER_RET(Tamer);
	Tamer->s_nPat = m_nLeader;
	if(0 != Tamer->s_nPat)
	{
		CsMaster_CardLeader::sINFO*	sInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCardLeader(Tamer->s_nPat)->GetInfo();

		CsMaster_Card::MAP_IT	it = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->begin();
		CsMaster_Card::MAP_IT	it_end = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->end();
		float fscale = 0.1f;

		for( ;it != it_end; ++it)
		{
			if(it->second->GetInfo()->s_nDigimonID == sInfo->s_nDigimonID)
			{
				fscale = it->second->GetInfo()->s_nScale * 0.01f;
			}
		}
		CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
		SAFE_POINTER_RET(pTamerUser);
		Tamer->ToObject( pTamerUser, true );	
		pTamerUser->DeletePat();
		pTamerUser->AddPat(sInfo->s_nDigimonID, fscale);
	}
}

const int cSealMasterContents::GetSealLeader() const
{
	return m_nLeader;
}

// 리더 설정 검증 밑 보내기
void cSealMasterContents::SendLeader(void* pData)
{
	// 서버로 요청하기 전 클라이언트에서 먼저 검사합니다.

	// 조건 검사
	// 마스터 등급이 아니면 리더로 선택할 수 없습니다.
	SealInfoMapItr selectSealItr = m_SealInfoMap.find(m_nSelectSealID);
	if( nCardGrade::Master != selectSealItr->second.sGrade )
	{
		cPrintMsg::PrintMsg( 30653 );

		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		CProtocol.m_wStr = _T( "마스터 씰만 리더로 설정할 수 있습니다.");
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	if( net::game )
	{
		// 선택된 리더가 아니였다면 리더 선택
		if( selectSealItr->second.sTableCode != m_nLeader )
			net::game->SendSetMasterCardLeader((u2)(selectSealItr->second.sTableCode));		
		else// 선택된 리더였다면 리더 해제
			net::game->SendUnSetMasterCardLeader((u2)(selectSealItr->second.sTableCode));
	}
	else
	{
		int nTamer = g_pCharMng->GetTamerUser()->GetUniqID();
		CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject( nTamer );	
		SAFE_POINTER_RET(pTamer);

		CsMaster_Card::MAP_IT	it = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->begin();
		CsMaster_Card::MAP_IT	it_end = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->end();
		float fscale = 0.1f;

		for( ;it != it_end; ++it)
		{
			if(it->second->GetInfo()->s_nDigimonID == selectSealItr->second.sModelID)
			{
				fscale = it->second->GetInfo()->s_nScale * 0.01f;
			}
		}

		pTamer->DeletePat();
		pTamer->AddPat(selectSealItr->second.sModelID, fscale);
		m_nLeader = selectSealItr->second.sSealID;
	}
}

// 다시 씰로 만들기
void cSealMasterContents::ReSealing(void* pData)
{
	CsPoint posSeparate = CURSOR_ST.GetPos() + CsPoint(-150, -40);

	int nSelectedCard = m_nSelectSealID;										// 선택 카드 아이템ID
	int nCurLeader = m_nLeader;				// 현재 리더
	int nFindLeader = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(nSelectedCard)->GetInfo()->s_nID;

	// 현재 리더와 선택한 카드 아이템이 리더인지 비교합니다.
	if( nFindLeader == nCurLeader )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		CProtocol.m_wStr = _T( "먼저 리더를 해제한 뒤 다시 시도해주세요." );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		cPrintMsg::PrintMsg( 30658 );
		return;
	}

	SealInfoMapItr SelectedSealItr = m_SealInfoMap.find(nSelectedCard);

	// 카드는 현재 1 ~ 보유수량  
	//20170411 null은 pIFIcon을 위한 값. WT_CARDINVENTORY의 경우는 cSeparate내부에서 pIFIcon을 사용하지 않는다.
	cSeparate::NewInstance( cBaseWindow::WT_CARDINVENTORY, 0, posSeparate, 1, SelectedSealItr->second.sSealCount, null, 0 );
}

const int cSealMasterContents::GetSelectSealID() const
{
	return m_nSelectSealID;
}

void cSealMasterContents::SetSelectSealID(int nSelectSealID)
{
	m_nSelectSealID = nSelectSealID;
}

// 현재 선택 중인 SEALID 가져오기
void cSealMasterContents::GetSelectedSealID(void* pData)
{
	SAFE_POINTER_RET(pData);
	int* pGetData = static_cast<int*>(pData);

	*pGetData = m_nSelectSealID;
}

// 씰정보 받기
void cSealMasterContents::RecvSealInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SEALMASTER_SEALINFO* pCastData = static_cast<GS2C_RECV_SEALMASTER_SEALINFO*>(pData);

	std::list<RecvCardInfoAll::SealInfo>::iterator SealInfoItr = pCastData->m_SealInfoList.begin();
	for (; SealInfoItr != pCastData->m_SealInfoList.end(); ++SealInfoItr)
	{
		SealInfoMapItr itr = m_SealInfoMap.find(SealInfoItr->m_nSealID);
		if (itr != m_SealInfoMap.end())
		{
			itr->second.sSealCount = SealInfoItr->m_nCardCount;
			itr->second.sGrade = CalculationGrade(SealInfoItr->m_nSealID, SealInfoItr->m_nCardCount);

			CsMaster_Card::sINFO*	sInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(SealInfoItr->m_nSealID)->GetInfo();
			if(itr->second.sGrade > -1)
			{
				itr->second.sEffectType = sInfo->s_stGradeInfo[itr->second.sGrade].s_nEff1;
				itr->second.sEffectValue = sInfo->s_stGradeInfo[itr->second.sGrade].s_nEff1val;
				SetPlusValue();
			}
		}
	}

	std::list<RecvCardInfoAll::FavoriteInfo>::iterator FavoriteInfoItr = pCastData->m_FavoriteInfoList.begin();
	for (; FavoriteInfoItr != pCastData->m_FavoriteInfoList.end(); ++FavoriteInfoItr)
	{
		int sealID = FindSealIDbyTableCode(FavoriteInfoItr->m_nCardCode);
		SealInfoMapItr itr = m_SealInfoMap.find(sealID);
		if (itr == m_SealInfoMap.end())
			return;
		bool bFavorite = static_cast<bool>(FavoriteInfoItr->m_nBookmark);

		itr->second.sFavorite = bFavorite;
		ContentsStream kStream;
		kStream << sealID << bFavorite;
		Notify(eSetFavorite, kStream);
	}
}

// 즐겨찾기 정보 받기
void cSealMasterContents::RecvFavorite(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SEALMASTER_FAVORITE* pCastData = static_cast<GS2C_RECV_SEALMASTER_FAVORITE*>(pData);
	int sealID = FindSealIDbyTableCode(pCastData->m_nCardCode);
	SealInfoMapItr itr = m_SealInfoMap.find(sealID);
	if (itr == m_SealInfoMap.end())
		return;

	bool bFavorite = static_cast<bool>(pCastData->m_nBookmark);

	itr->second.sFavorite = bFavorite;
	ContentsStream kStream;
	kStream << sealID << bFavorite;
	Notify(eSetFavorite, kStream);
}

int cSealMasterContents::CalculationGrade(DWORD SealID, int SealCount)
{
	//해당 카드가 SealCount 일 때 등급을 확인해서 리턴해줍니다.
	CsMaster_Card::sINFO*	sInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(SealID)->GetInfo();

	if(SealCount == 0)
		return -1;
	else if( SealCount < sInfo->s_stGradeInfo[CsMaster_Card::FT_CARD_BRONZE].s_nMax )
		return nCardGrade::Normal;
	else if( SealCount < sInfo->s_stGradeInfo[CsMaster_Card::FT_CARD_SHILVER].s_nMax )
		return nCardGrade::Bronze;
	else if( SealCount < sInfo->s_stGradeInfo[CsMaster_Card::FT_CARD_GOLD].s_nMax )
		return nCardGrade::Silver;
	else if( SealCount < sInfo->s_stGradeInfo[CsMaster_Card::FT_CARD_PLATINUM].s_nMax )
		return nCardGrade::Gold;
	else if( SealCount < sInfo->s_stGradeInfo[CsMaster_Card::FT_CARD_MASTER].s_nMax )
		return nCardGrade::Platinum;
	else
		return nCardGrade::Master;

	return nCardGrade::Normal;
}

int cSealMasterContents::FindSealIDbyTableCode(int nTableCode)
{
	CsMaster_Card::MAP* pCardTable = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap();
	CsMaster_Card::MAP_IT cardTableItr = pCardTable->begin();
	CsMaster_Card::MAP_IT cardTableEnd = pCardTable->end();
	for (; cardTableItr != cardTableEnd; ++cardTableItr)
	{
		if(cardTableItr->second->GetInfo()->s_nID == nTableCode)
		{
			return cardTableItr->second->GetInfo()->s_stGradeInfo[CsMaster_Card::FT_CARD_NORMAL].s_nItem;
		}
	}
	return null;
}

// 씰 오픈 받기
void cSealMasterContents::RecvSealOpen(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SEALMASTER_OPENSEAL* pCastData = static_cast<GS2C_RECV_SEALMASTER_OPENSEAL*>(pData);

	SealInfoMapItr itr = m_SealInfoMap.find(pCastData->m_nSealItemID);
	bool bChangeGrade = false;

	if ( itr != m_SealInfoMap.end() )
	{
		itr->second.sSealCount += pCastData->m_nSealAmount;

		int chageGrade = CalculationGrade(pCastData->m_nSealItemID, itr->second.sSealCount);
		if(itr->second.sGrade != chageGrade)
		{
			itr->second.sGrade = chageGrade;
			bChangeGrade = true;
			CsMaster_Card::sINFO*	sInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(pCastData->m_nSealItemID)->GetInfo();
			int dataGrade;
			if( chageGrade == -1)
			{
				dataGrade = 0;
				itr->second.sEffectType = sInfo->s_stGradeInfo[dataGrade].s_nEff1;
				itr->second.sEffectValue = 0;
			}
			else
			{
				dataGrade = chageGrade;
				//등급 변화 이펙트 타입과 수치도 갱신
				itr->second.sEffectType = sInfo->s_stGradeInfo[dataGrade].s_nEff1;
				itr->second.sEffectValue = sInfo->s_stGradeInfo[dataGrade].s_nEff1val;
			}

			SetPlusValue();
			Notify(eSetPlusValu);
		}
	}
	ContentsStream kStream;
	kStream << pCastData->m_nSealItemID << bChangeGrade;
	Notify(eSetSealCount, kStream);
}

// 다시 씰로 만들기
void cSealMasterContents::RecvReSealing(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SEALMASTER_RESEAL* pCastData = static_cast<GS2C_RECV_SEALMASTER_RESEAL*>(pData);
	SealInfoMapItr itr = m_SealInfoMap.find(pCastData->m_nSealItemID);
	bool bChangeGrade = false;

	if ( itr != m_SealInfoMap.end() )
	{
		itr->second.sSealCount -= pCastData->m_nSealAmount;
		int chageGrade = CalculationGrade(pCastData->m_nSealItemID, itr->second.sSealCount);
		if(itr->second.sGrade != chageGrade)
		{
			itr->second.sGrade = chageGrade;
			bChangeGrade = true;
			CsMaster_Card::sINFO*	sInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(pCastData->m_nSealItemID)->GetInfo();
			int dataGrade;
			if( chageGrade == -1)
			{
				dataGrade = 0;
				itr->second.sEffectType = sInfo->s_stGradeInfo[dataGrade].s_nEff1;
				itr->second.sEffectValue = 0;
			}
			else
			{
				dataGrade = chageGrade;
				//등급 변화 이펙트 타입과 수치도 갱신
				itr->second.sEffectType = sInfo->s_stGradeInfo[dataGrade].s_nEff1;
				itr->second.sEffectValue = sInfo->s_stGradeInfo[dataGrade].s_nEff1val;
			}
		
			SetPlusValue();
			Notify(eSetPlusValu);
		}
	}
	
	ContentsStream kStream;
	kStream << pCastData->m_nSealItemID << bChangeGrade;
	Notify(eSetSealCount, kStream);
}

void cSealMasterContents::SetPlusValue()
{
	for (int i = 0; i < STATEKINDS; ++i)
		m_pPlusValue[i] = 0;

	SealInfoMapItr Itr = m_SealInfoMap.begin();
	for (;Itr != m_SealInfoMap.end(); ++Itr)
	{
		if (Itr->second.sEffectValue > 0 && Itr->second.sEffectType > 0)
		{
			//%증가 타입에 대한 표현은 정의 되지 않았다. 20170417
			if(Itr->second.sEffectType%2 == 1)
			{
				int ArrayIndex = Itr->second.sEffectType/2;
				if(ArrayIndex > -1 && ArrayIndex < STATEKINDS)
				{
					m_pPlusValue[ArrayIndex] += Itr->second.sEffectValue;
				}
			}
		}
	}
}

int* cSealMasterContents::GetPlusValue()
{
	return m_pPlusValue;
}

// 씰정보 받기
void cSealMasterContents::UIOpenClose(void* pData)
{
	SAFE_POINTER_RET(pData);
	int mode = *( static_cast<int*>(pData) );
	if ( mode == 0)
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CARDINVENTORY );
	else if ( mode==1 )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIN_DIGIMON_WINDOW ) )
		{
			if( g_pGameIF->GetMainDigimonWindow()->IsBattle() )
			{
				cPrintMsg::PrintMsg( 16002 );	// 전투 중에는 사용할 수 없습니다.
				return;
			}
		}

		if( g_pNpcMng->GetZoomNpc() || g_pGameIF->IsOpenNpcWindow() )	// NPC 이용 중에는 더더욱 카드 UI를 사용 못하도록 막음 1차
		{
			cPrintMsg::PrintMsg( 10004 );	// 지금은 사용할 수 없습니다.
			return;
		}
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_CARDINVENTORY, 0 );
	}
	else//3
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CARDINVENTORY ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CARDINVENTORY );
		else
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIN_DIGIMON_WINDOW ) )
			{
				if( g_pGameIF->GetMainDigimonWindow()->IsBattle() )
				{
					cPrintMsg::PrintMsg( 16002 );	// 전투 중에는 사용할 수 없습니다.
					return;
				}
			}

			if( g_pNpcMng->GetZoomNpc() || g_pGameIF->IsOpenNpcWindow() )	// NPC 이용 중에는 더더욱 카드 UI를 사용 못하도록 막음 1차
			{
				cPrintMsg::PrintMsg( 10004 );	// 지금은 사용할 수 없습니다.
				return;
			}
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_CARDINVENTORY, 0 );
		}
	}
}

// 언씰 전 언씰 가능 여부 체크
void cSealMasterContents::CheckUnSeal(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int, int> sealInfo = *( static_cast<std::pair<int,int>*>(pData) );
	SealInfoMapItr itr = m_SealInfoMap.find(sealInfo.first);
	if (itr != m_SealInfoMap.end())
	{
		// 테이블에 없으면 등록 불가입니다.
		CsMaster_Card*	pCard = nsCsFileTable::g_pMaster_CardMng->GetMasterCard( sealInfo.first );

		if( pCard == NULL )
			return;

		CsMaster_Card::sINFO*	sInfo = pCard->GetInfo();

		// 보유량 + 등록할 양 < MAX양 ? true : false
		if( itr->second.sSealCount + sealInfo.second <= sInfo->s_stGradeInfo[CsMaster_Card::FT_CARD_MASTER].s_nMax )
		{
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( sealInfo.first );	// 씰 아이템
			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
			cPrintMsg::PrintMsg(30656, pFTInfo->s_szName); // 씰을 등록 하시겠습니까?
			return;
		}
	}

	cPrintMsg::PrintMsg(30651);
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = _T("같은 종류의 씰은 최대 3000개까지만 등록할 수 있습니다.");
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

// 추가된 능력치 합 가져오기
void cSealMasterContents::GetPlusValue(void* pData)
{
	SAFE_POINTER_RET(pData);

	std::pair<nTable_Parameter, int>* pGetData = static_cast<std::pair<nTable_Parameter, int>*>(pData);
	int PlusValue = 0;
	switch(pGetData->first)
	{
	case nTable_Parameter::APPLY_MAXHP:		PlusValue = m_pPlusValue[0];		break;
	case nTable_Parameter::APPLY_MAXDS:		PlusValue = m_pPlusValue[1];		break;
	case nTable_Parameter::APPLY_AP:		PlusValue = m_pPlusValue[2];		break;
	case nTable_Parameter::APPLY_AS:		PlusValue = m_pPlusValue[3];		break;
	case nTable_Parameter::APPLY_CA:		PlusValue = m_pPlusValue[4];		break;
	case nTable_Parameter::APPLY_HT:		PlusValue = m_pPlusValue[5];		break;
	case nTable_Parameter::APPLY_DP:		PlusValue = m_pPlusValue[6];		break;
	case nTable_Parameter::APPLY_BL:		PlusValue = m_pPlusValue[7];		break;
	case nTable_Parameter::APPLY_EV:		PlusValue = m_pPlusValue[8];		break;
	default:		PlusValue = 0;		break;
	}

	(*pGetData).second = PlusValue;
}

NiColor cSealMasterContents::GetSealItemColor( DWORD const & dwItemCode )
{
	SAFE_POINTER_RETVAL( g_pDataMng, NiColor::WHITE );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, NiColor::WHITE );

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( dwItemCode );	// 씰 아이템
	SAFE_POINTER_RETVAL( pFTItem, NiColor::WHITE );

	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, NiColor::WHITE );

	return g_pDataMng->GetItemColor( pFTInfo->s_nClass );
}

std::string cSealMasterContents::GetSealCardObjectFile_NifFile( DWORD const & dwSealID )
{
	std::string result;
	SealInfoMapItr it = m_SealInfoMap.find( dwSealID );
	if( it == m_SealInfoMap.end() )
		return result;

	if( nsCsFileTable::g_pMaster_CardMng )
		result = nsCsFileTable::g_pMaster_CardMng->GetSealGridNifFile( it->second.sGrade );

	return result;
}

std::string cSealMasterContents::GetSealCardObjectFile_GradBack( DWORD const & dwSealID )
{
	std::string result;
	SealInfoMapItr it = m_SealInfoMap.find( dwSealID );
	if( it == m_SealInfoMap.end() )
		return result;

	if( nsCsFileTable::g_pMaster_CardMng )
		result = nsCsFileTable::g_pMaster_CardMng->GetSealGridBackFile( it->second.sGrade );

	return result;
}

std::string cSealMasterContents::GetSealCardObjectFile_Elemental( DWORD const & dwSealID )
{
	std::string result;
	SealInfoMapItr it = m_SealInfoMap.find( dwSealID );
	if( it == m_SealInfoMap.end() )
		return result;

	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( it->second.sModelID );
	if( pDigimon )
	{
		CsDigimon::sINFO* pTInfo = pDigimon->GetInfo();
		if( pTInfo && nsCsFileTable::g_pMaster_CardMng )
			result = nsCsFileTable::g_pMaster_CardMng->GetSealElementalImgPath( pTInfo->s_eBaseNatureType );
	}

	return result;
}

std::string cSealMasterContents::GetSealCardObjectFile_Attribute( DWORD const & dwSealID )
{
	std::string result;
	SealInfoMapItr it = m_SealInfoMap.find( dwSealID );
	if( it == m_SealInfoMap.end() )
		return result;

	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( it->second.sModelID );
	if( pDigimon )
	{
		CsDigimon::sINFO* pTInfo = pDigimon->GetInfo();
		if( pTInfo && nsCsFileTable::g_pMaster_CardMng )
			result = nsCsFileTable::g_pMaster_CardMng->GetSealAttributeImgPath( pTInfo->s_eAttributeType );
	}

	return result;
}

std::string cSealMasterContents::GetSealCardObjectFile_Digimon( DWORD const & dwSealID )
{
	std::string result;
	SealInfoMapItr it = m_SealInfoMap.find( dwSealID );
	if( it == m_SealInfoMap.end() )
		return result;

	if( nsCsFileTable::g_pMaster_CardMng )
		result = nsCsFileTable::g_pMaster_CardMng->GetSealDigimonImgPath( it->second.sModelID );
	return result;
}
