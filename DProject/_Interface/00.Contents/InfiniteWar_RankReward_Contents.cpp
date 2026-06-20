#include "StdAfx.h"
#include "InfiniteWar_RankReward_Contents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

void CInfiniteWarRankRewardContents::sPriviouslyData::InitData()
{
	bRecvData = false;
	listPreviouslyRanker.clear();
}

void CInfiniteWarRankRewardContents::sRankInfos::InitData()
{
	bRecvData = false;
	nMyRanking = 0;
	nUpDownPoint = 0;
	bAbsRanking = false;
	nMyRankPrecent = 0;
	nMyPoint = 0;
	i64RestRankRenewSec = 0;
	i64RestRankEndSec = 0;
	nPreviouslyMyRanking = 0;
	nRankState = 0;

	listRewardItems.clear();
	listRanker.clear();
	PriviouslyData.InitData();
}

bool CInfiniteWarRankRewardContents::sRankInfos::IsNeedUpdateData()	// 데이터 갱신이 필요한지 체크
{
	if( !bRecvData )
		return true;

	uint currentTime = net::net->GetTimeTS();
	if( i64RestRankRenewSec - currentTime <= 0 )
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarRankRewardContents::sDailyPointData::InitData()		// 데이터 삭제
{
	bRecvData = false;
	m_nCurrentDay = -1;
	m_dwTotalInjectedPoint = 0;
	m_RestDayTime = 0;
	m_mapDayPointItemList.clear();
}

bool CInfiniteWarRankRewardContents::sDailyPointData::IsNeedUpdateData()	// 데이터 갱신이 필요한지 체크
{
	if( !bRecvData )
		return true;

	uint currentTime = net::net->GetTimeTS();
	if( m_RestDayTime - currentTime <= 0 )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

void CInfiniteWarRankRewardContents::sHallOfFameData::InitData()		// 데이터 삭제
{
	bRecvData = false;
	i64UpdateRamainTime = 0;
	m_mapFameRankerList.clear();
}

bool CInfiniteWarRankRewardContents::sHallOfFameData::IsNeedUpdateData()	// 데이터 갱신이 필요한지 체크
{
	if( !bRecvData )
		return true;

	uint currentTime = net::net->GetTimeTS();
	if( i64UpdateRamainTime - currentTime <= 0 )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int const CInfiniteWarRankRewardContents::IsContentsIdentity(void)
{
	return E_CT_INFINITEWAR_RANKREWARD_CONTENTS;
}

CInfiniteWarRankRewardContents::CInfiniteWarRankRewardContents(void)
:m_pTarget(NULL),m_dwTargetNpcIdx(0),m_dwTargetNpcIdx_Temp(0)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_INFINITEWAR_NOTICEBOARD_WINDOW, this, &CInfiniteWarRankRewardContents::_OpenNoticeboardWindow );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_DATA, this, &CInfiniteWarRankRewardContents::_Recv_NoticeBoardData );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_POINT_PUT_RESULT, this, &CInfiniteWarRankRewardContents::_Recv_NoticeBoard_PointPut_Result );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_INFINITEWAR_NOTICEBOARD_DATA_UPDATE, this, &CInfiniteWarRankRewardContents::_Ntf_NoticeBoardData_Updata );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_RANKER_LIST, this, &CInfiniteWarRankRewardContents::_RecvRankerList );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_PRE_RANKER_LIST, this, &CInfiniteWarRankRewardContents::_RecvPreRankerList );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_FAME_LIST, this, &CInfiniteWarRankRewardContents::_RecvHallofFameList );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_SEASON_FAME_LIST, this, &CInfiniteWarRankRewardContents::_RecvHallofFameSeasonList );
}

CInfiniteWarRankRewardContents::~CInfiniteWarRankRewardContents(void)
{
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const CInfiniteWarRankRewardContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CInfiniteWarRankRewardContents::Initialize(void)
{

	return true;
}

void CInfiniteWarRankRewardContents::UnInitialize(void)
{
}

bool CInfiniteWarRankRewardContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void CInfiniteWarRankRewardContents::Update(float const& fElapsedTime)
{
}

void CInfiniteWarRankRewardContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CInfiniteWarRankRewardContents::MakeMainActorData(void)
{
	_InitData();
}

void CInfiniteWarRankRewardContents::ClearMainActorData(void)
{
	ClearAllRegistered();
	_InitData();
}


void CInfiniteWarRankRewardContents::MakeWorldData(void)
{
	_InitData();
}

void CInfiniteWarRankRewardContents::ClearWorldData(void)
{
	_InitData();
}
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarRankRewardContents::_InitData()
{
	_ClearData();
	m_mapRankInfoByType.clear();
	m_sDailyPointData.InitData();
	m_sHallOfFameData.InitData();
}

void CInfiniteWarRankRewardContents::_ClearData()
{
	m_pTarget = NULL;
	m_dwTargetNpcIdx = 0;	
	m_dwTargetNpcIdx_Temp = 0;
}


CsC_AvObject* CInfiniteWarRankRewardContents::GetTargetNpc() const
{
	return m_pTarget;
}

//////////////////////////////////////////////////////////////////////////
// Window Show/Hide 관련 함수
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarRankRewardContents::_OpenNoticeboardWindow(void* pData)
{
	SAFE_POINTER_RET( pData );

	CsC_AvObject* pTargetObject = static_cast<CsC_AvObject*>(pData);
	assert_cs( pTargetObject->GetLeafRTTI() == RTTI_NPC );

	DWORD dwTargetObjIdx = pTargetObject->GetFTID();
	if( dwTargetObjIdx == m_dwTargetNpcIdx )
		return;

	m_pTarget = pTargetObject;
	m_dwTargetNpcIdx = dwTargetObjIdx;
	OpenCloseNoticeWindow(true);

	// 윈도우를 열때 서버로부터 데이터를 받아야 하는지 체크 
	_CheckDailyDataUpdate();

#ifdef SDM_INFINITEWAR_RANKING_SYSTEM_20180312
	_CheckRankingDataUpdate();
	_CheckHallOfFameDataUpdate();
#endif
}

void CInfiniteWarRankRewardContents::OpenCloseNoticeWindow(bool bVisible)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( bVisible )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW ) )
			return;

		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW );
	}
	else
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW, 0, true );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_PRERANKLIST_WINDOW, 0, true );	
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_RANKREWARD_WINDOW, 0, true );	
		_ClearData();
	}
}

void CInfiniteWarRankRewardContents::_WindowsLock( cBaseWindow::eWINDOW_TYPE wt, bool bLock )
{
	if( !g_pGameIF->IsActiveWindow( wt ) )
		return;

	cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( wt );
	if( pWindow )
		pWindow->SetEnableWindow( !bLock );
}

void CInfiniteWarRankRewardContents::OpenPreRankingListWindow(int const& nType)
{
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW ) )
		return;

	//_WindowsLock( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW, true );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INFINITEWAR_PRERANKLIST_WINDOW ) )
		return;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INFINITEWAR_RANKREWARD_WINDOW ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_RANKREWARD_WINDOW, 0, true );

	g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_PRERANKLIST_WINDOW );	

	ContentsStream kTmp;
	kTmp << nType;
	Notify( eUpdate_Previously_RankerList, kTmp );

	if( IsNeedUpdatePreviouslyRankerData( nType ) )
		_SendReuqestPreviouslyRankerList( nType );
}

void CInfiniteWarRankRewardContents::ClosePreRankingListWindow()
{
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_PRERANKLIST_WINDOW, 0, true );
}

void CInfiniteWarRankRewardContents::OpenRankRewardItemListWindow(int const& nType)
{
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW ) )
		return;

	//_WindowsLock( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW, true );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INFINITEWAR_PRERANKLIST_WINDOW ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_PRERANKLIST_WINDOW, 0, true );	

	g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_RANKREWARD_WINDOW );

	ContentsStream kTmp;
	kTmp<<nType;
	Notify( eOpenRankRewardItemWindow, kTmp );
}

void CInfiniteWarRankRewardContents::CloseRankRewardItemListWindow()
{
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_RANKREWARD_WINDOW, 0, true );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 일일 포인트 데이터 갱신
void CInfiniteWarRankRewardContents::_CheckDailyDataUpdate()
{
	if( !m_sDailyPointData.IsNeedUpdateData() )
		return;
	
	SendRequestInfiniteWarData();

	_WindowsLock( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW, true );
}

void CInfiniteWarRankRewardContents::SendRequestInfiniteWarData()
{
	SAFE_POINTER_RET( net::game );
	net::game->Send_Request_ChallengeData();
}

// 랭크 타입 별 데이터 갱신
void CInfiniteWarRankRewardContents::_CheckRankingDataUpdate()
{
	if( IsNeedUpdateRankInfoDataByType( TB::eInfiniteWarRewardType::eWeek ) )
		_SendReuqestRankInfoByType(TB::eInfiniteWarRewardType::eWeek);

	if( IsNeedUpdateRankInfoDataByType( TB::eInfiniteWarRewardType::eMonth ) )
		_SendReuqestRankInfoByType(TB::eInfiniteWarRewardType::eMonth);

	if( IsNeedUpdateRankInfoDataByType( TB::eInfiniteWarRewardType::eSeason ) )
		_SendReuqestRankInfoByType(TB::eInfiniteWarRewardType::eSeason);
}

//////////////////////////////////////////////////////////////////////////
// 주간/월간/시즌 별 데이터 요청
//////////////////////////////////////////////////////////////////////////
// 업데이트 시간
void CInfiniteWarRankRewardContents::SendRankingDataUpdateByType(int const& nType)
{
	_SendReuqestRankInfoByType( nType );
}

void CInfiniteWarRankRewardContents::SendRankingDeadline(int const& nType)
{
	_SendReuqestRankInfoByType( nType );
	_ResetPriviouslyRankListData( nType );
}

void CInfiniteWarRankRewardContents::_ResetPriviouslyRankListData(int const& nType)
{
	std::map<int,sRankInfos>::iterator it = m_mapRankInfoByType.find( nType );
	if( it == m_mapRankInfoByType.end() )
		return;

	it->second.PriviouslyData.InitData();
}

bool CInfiniteWarRankRewardContents::IsNeedUpdateRankInfoDataByType( int const& nType )
{
	std::map<int,sRankInfos>::iterator it = m_mapRankInfoByType.find(nType);
	if( it == m_mapRankInfoByType.end() )
		return true;

	return it->second.IsNeedUpdateData();
}

void CInfiniteWarRankRewardContents::_SendReuqestRankInfoByType( int const& nType )
{
	SAFE_POINTER_RET( net::game );
	unsigned char cRankType = (unsigned char)nType;
	net::game->Send_ArenaRequestRank( cRankType );
}

//////////////////////////////////////////////////////////////////////////
//  주간/월간/시즌 별 지난 랭커 리스트 요청
//////////////////////////////////////////////////////////////////////////
bool CInfiniteWarRankRewardContents::IsNeedUpdatePreviouslyRankerData( int const& nType )
{
	std::map<int,sRankInfos>::const_iterator it = m_mapRankInfoByType.find(nType);
	if( it == m_mapRankInfoByType.end() )
		return true;

	if( !it->second.IsRecvData() )
		return true;

	if( !it->second.PriviouslyData.IsRecvData() )
		return true;
	
	return false;
}

void CInfiniteWarRankRewardContents::_SendReuqestPreviouslyRankerList( int const& nType )
{
	SAFE_POINTER_RET( net::game );
	unsigned char cRankType = (unsigned char)nType;
	net::game->Send_ArenaRequestOldRank( cRankType );
}

//////////////////////////////////////////////////////////////////////////
// 명예의 전당 데이터 갱신
//////////////////////////////////////////////////////////////////////////

// 명예의 전당 시즌 리스트 관련
void CInfiniteWarRankRewardContents::_CheckHallOfFameDataUpdate()
{
	if( m_sHallOfFameData.IsRecvData() )
		return;

	_SendReuqestFameSeasonListBySeason();
}

void CInfiniteWarRankRewardContents::_SendReuqestFameSeasonListBySeason()
{
	SAFE_POINTER_RET( net::game );
	net::game->Send_ArenaRequestFameSeasonList();
}

void CInfiniteWarRankRewardContents::SelectedHallofFameBySeason( int const& nType )
{
	if( IsNeedUpdateFameListData(nType) )
	{
		_SendReuqestFameListBySeason(nType);
		return;
	}

	ContentsStream kTmp;
	kTmp << nType;
	Notify( eUpdate_FameRankerList, kTmp );
}

// 명예의 전당 시즌별 랭커 리스트 요청
bool CInfiniteWarRankRewardContents::IsNeedUpdateFameListData( int const& nType )
{
	if( !m_sHallOfFameData.IsRecvData() )// 서버로 부터 시즌 정보를 받지 않았을 때
		return true;

	std::map<int,sFamData>::const_iterator it = m_sHallOfFameData.m_mapFameRankerList.find(nType);
	if( it == m_sHallOfFameData.m_mapFameRankerList.end() )
		return true;

	if( !it->second.IsRecvData() )
		return true;

	return false;
}

void CInfiniteWarRankRewardContents::_SendReuqestFameListBySeason( int const& nType )
{
	SAFE_POINTER_RET( net::game );
	unsigned char cRankType = (unsigned char)nType;
	net::game->Send_ArenaRequestFame( cRankType );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


DWORD CInfiniteWarRankRewardContents::GetTodayTotalInjectedPoint() const
{
	if( !m_sDailyPointData.IsRecvData() )// 서버로 부터 데이터를 받지 않았을 때
		return 0;

	return m_sDailyPointData.m_dwTotalInjectedPoint;
}

__int64 CInfiniteWarRankRewardContents::GetRestRemainTime() const
{
	SAFE_POINTER_RETVAL( net::net, 0 );

	if( !m_sDailyPointData.IsRecvData() )
		return 0;

	uint uServerTime = net::net->GetTimeTS();
	if( m_sDailyPointData.m_RestDayTime - uServerTime < 0 )
		return 0;

	return m_sDailyPointData.m_RestDayTime - uServerTime;
}

CsInfiniteWarMng::DailyRewardItems const * CInfiniteWarRankRewardContents::GetTodayRewardItemLIst() const
{
	if( !m_sDailyPointData.IsRecvData() )
		return NULL;

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pInfiniteWarTBMng, NULL );
	CsInfiniteWarMng::DailyRewardContiner const& pmapDaily = nsCsFileTable::g_pInfiniteWarTBMng->GetRewardData_Daily();
	CsInfiniteWarMng::DailyRewardContiner_CIT it = pmapDaily.find( m_sDailyPointData.m_nCurrentDay );
	if( it == pmapDaily.end() )
		return NULL;

	return &it->second;
}

CInfiniteWarRankRewardContents::sRankInfos const * CInfiniteWarRankRewardContents::GetRankInfoByType(int const& nType) const
{
	std::map<int,sRankInfos>::const_iterator it = m_mapRankInfoByType.find( nType );
	if( it == m_mapRankInfoByType.end() )
		return NULL;	

	return &it->second;
}

std::list<CInfiniteWarRankRewardContents::sRankerList> const * CInfiniteWarRankRewardContents::GetRankerListFormType(int const& nType) const
{
	sRankInfos const * pInfo = GetRankInfoByType( nType );
	SAFE_POINTER_RETVAL(pInfo, NULL);
	
	return &pInfo->listRanker;
}

DWORD CInfiniteWarRankRewardContents::GetCurrentNeedItemCode() const
{
	if( !m_sDailyPointData.IsRecvData() )
		return 0;

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pInfiniteWarTBMng, 0 );
	CsInfiniteWarMng::DailyRewardContiner const& pmapDaily = nsCsFileTable::g_pInfiniteWarTBMng->GetRewardData_Daily();
	CsInfiniteWarMng::DailyRewardContiner_CIT it = pmapDaily.find( m_sDailyPointData.m_nCurrentDay );
	if( it == pmapDaily.end() )
		return 0;

	if( it->second.empty() )
		return 0;

	CsInfiniteWarMng::DailyRewardItems_CIT subIt = it->second.begin();
	for( ; subIt != it->second.end(); ++subIt )
	{
		if( m_sDailyPointData.m_dwTotalInjectedPoint < (*subIt).s_nNeedPoint )
			return (*subIt).s_dwNeedItemCode;
	}

	CsInfiniteWarMng::DailyRewardItems::const_reverse_iterator secIT = it->second.rbegin();
	return (*secIT).s_dwNeedItemCode;
}

DWORD CInfiniteWarRankRewardContents::GetCurrentNeedItemCount() const
{
	SAFE_POINTER_RETVAL(g_pDataMng, 0);
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL(pInven, 0);

	DWORD dwItemCode = GetCurrentNeedItemCode();
	return pInven->GetCount_Item_ID( dwItemCode );
}

std::wstring CInfiniteWarRankRewardContents::GetItemName( DWORD const& dwItemCode )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, L"" );
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemCode );
	SAFE_POINTER_RETVAL( pItem, L"" );
	SAFE_POINTER_RETVAL( pItem->GetInfo(), L"" );
	return pItem->GetInfo()->s_szName;	
}

void CInfiniteWarRankRewardContents::SendPointInject( int const& nInjectCount )
{
	// 서버에서 데이터를 받지 못했는데 패킷을 보낼수 없다
	if( !m_sDailyPointData.IsRecvData() )
		return;

	SAFE_POINTER_RET(net::game);
	DWORD dwNeedItemCode = GetCurrentNeedItemCode();
	if( 0 == dwNeedItemCode )
		return;

	if( GetRestRemainTime() <= 60 )
	{
		cPrintMsg::PrintMsg( 30901 ); //초기화 1분 전에는 패킷을 보내지 않는다.
		return;
	}

	SAFE_POINTER_RET(g_pDataMng);
 	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET(pInven);

	std::map<int,int> items;
	pInven->GetItemDatas_ItemID( dwNeedItemCode, nInjectCount, items );

 	std::list<nChallengeSt::st_ItemInfo> lItemList;
	std::map<int,int>::iterator it = items.begin();
	for( ; it != items.end(); ++it )
	{
		nChallengeSt::st_ItemInfo addItem;
		addItem.nItemPos = it->first;
		addItem.nItemCode = dwNeedItemCode;
		addItem.nCount = it->second;
		lItemList.push_back( addItem );
	}

 	net::game->Send_Challenge_PointItem_Put(lItemList);
}

void CInfiniteWarRankRewardContents::GetHallofFameSeasonList( std::list<int>& lists )
{
	lists.clear();
	std::map<int,sFamData>::const_iterator it = m_sHallOfFameData.m_mapFameRankerList.begin();
	for( ; it != m_sHallOfFameData.m_mapFameRankerList.end(); ++it )
		lists.push_back( it->first );
}

std::list<CInfiniteWarRankRewardContents::sFameRankerInfo>	const* CInfiniteWarRankRewardContents::GetHallofFameRankerList(int const& nSeasonIdx) const
{
	std::map<int,sFamData>::const_iterator it = m_sHallOfFameData.m_mapFameRankerList.find(nSeasonIdx);
	if( it == m_sHallOfFameData.m_mapFameRankerList.end() )
		return NULL;
	return &it->second.RankerList;
}

std::list<TB::sRankRewardInfo>	const* CInfiniteWarRankRewardContents::GetRewardItemListFromType( int const& nType ) const
{
	CsInfiniteWarMng::RankRewardItemsContanier const& pmapDaily = nsCsFileTable::g_pInfiniteWarTBMng->GetRewardData_Rank();
	CsInfiniteWarMng::RankRewardItemsContanier_CIT it = pmapDaily.find( nType );
	if( it == pmapDaily.end() )
		return NULL;

	return &it->second;
}

//////////////////////////////////////////////////////////////////////////
// 서버로 부터 게시판 정보를 받음.
void CInfiniteWarRankRewardContents::_Recv_NoticeBoardData(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHALLENGE_DAILY_GET* recv = static_cast<GS2C_RECV_CHALLENGE_DAILY_GET*>(pData);

	m_sDailyPointData.InitData();
	m_sDailyPointData.bRecvData = true;
	m_sDailyPointData.m_nCurrentDay = recv->nDay;					//요일 , 0 - 일 ~ 6 - 토
	m_sDailyPointData.m_RestDayTime = recv->nResetTimeLeftSec;		//초기화까지 남은 시간(초)
	m_sDailyPointData.m_dwTotalInjectedPoint = recv->nDailyPoint;	//현재 자신의 일일 포인트

	Notify(eUpdate_NoticeBoard_Data);

	_WindowsLock( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW, false );
}

// 게시판을 통해 포인트 투입에 대한 결과
void CInfiniteWarRankRewardContents::_Recv_NoticeBoard_PointPut_Result(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHALLENGE_DAILY_PUT* recv = static_cast<GS2C_RECV_CHALLENGE_DAILY_PUT*>(pData);

	switch( recv->nResult )
	{
	case CHALLENGE_REWARD_INPUT_RESULT::OK:
		break;
	case CHALLENGE_REWARD_INPUT_RESULT::WRONG_ITEM:
		return;
	case CHALLENGE_REWARD_INPUT_RESULT::ITEM_EMPTY:
		return;
	case CHALLENGE_REWARD_INPUT_RESULT::BLOCKED_TIME:
		cPrintMsg::PrintMsg( 30901 ); //초기화 1분 전에는 패킷을 보내지 않는다.
		return;
	}

	if( !m_sDailyPointData.IsRecvData() ) // 서버로 부터 데이터를 받지 않았는데 포인트 투입 결과가 옴.
	{
	
	}

	if( m_sDailyPointData.m_dwTotalInjectedPoint != recv->nDailyPoint )
	{
		m_sDailyPointData.m_dwTotalInjectedPoint = recv->nDailyPoint;

		ContentsStream kTmp;
		kTmp << m_sDailyPointData.m_dwTotalInjectedPoint;
		Notify( eUpdate_Today_Injected_Point, kTmp );
	}

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	
	std::list<nChallengeSt::st_ItemInfo>::iterator it = recv->lUsedItemList.begin();	//실제 사용한 아이템들 목록					
	for( ; it != recv->lUsedItemList.end(); ++it )
		pInven->ItemRemove_NoServer( (*it).nItemPos, (*it).nItemCode, (*it).nCount, false );
	
	std::list<nChallengeSt::st_RewardItemInfo>::iterator secit = recv->lRewardItemList.begin();	//보상 아이템들 목록
	for( ; secit != recv->lRewardItemList.end(); ++secit )
		pInven->ItemCrop( (*secit).nItemCode, (*secit).nCount );
}

// 게시판 정보가 갱신 됐을 때 서버에서 받는 데이터
void CInfiniteWarRankRewardContents::_Ntf_NoticeBoardData_Updata(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHALLENGE_DAILY_UPDATE* recv = static_cast<GS2C_RECV_CHALLENGE_DAILY_UPDATE*>(pData);

	if( !m_sDailyPointData.IsRecvData() ) // 서버로 부터 데이터를 받지 않았는데 포인트 투입 결과가 옴.
	{

	}

	m_sDailyPointData.m_nCurrentDay = recv->nDay;				//요일 , 0 - 일 ~ 6 - 토
	m_sDailyPointData.m_RestDayTime = recv->nResetTimeLeftSec;	//초기화까지 남은 시간(초)
	m_sDailyPointData.m_dwTotalInjectedPoint = recv->nDailyPoint;			//현재 자신의 일일 포인트

	Notify( eUpdate_NoticeBoard_Data );
	_WindowsLock( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW, false );
}

CInfiniteWarRankRewardContents::sRankInfos&	CInfiniteWarRankRewardContents::_GetRankInfoByType(int const& nType)
{
	std::map<int,sRankInfos>::iterator it = m_mapRankInfoByType.find( nType );
	if( it == m_mapRankInfoByType.end() )
	{
		auto rib = m_mapRankInfoByType.insert( std::make_pair( nType, sRankInfos() ) );
		it = rib.first;
	}
	return it->second;
}

CInfiniteWarRankRewardContents::sRankInfos const* CInfiniteWarRankRewardContents::GetPreviouslyRankerInfoByType( int const& nType ) const
{
	std::map<int,sRankInfos>::const_iterator it = m_mapRankInfoByType.find( nType );
	if( it == m_mapRankInfoByType.end() )
		return NULL;

	return &it->second;
}

//////////////////////////////////////////////////////////////////////////
// 주간/월간/시즌 별 랭커 정보 갱신 
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarRankRewardContents::_RecvRankerList(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ARENA_REQUEST_RANK* recv = static_cast<GS2C_RECV_ARENA_REQUEST_RANK*>(pData);

	if(0 != recv->nResult )
		return;

	sRankInfos & RankInfo = _GetRankInfoByType( recv->nType );// 주/월/시즌 
	RankInfo.InitData();
	RankInfo.bRecvData = true;
	RankInfo.nMyPoint =	recv->nPoint;				//자신의 포인트 - 넣은정보가없으면 0
	RankInfo.nMyRanking = recv->nRank;				//자신의 절대 등수
	RankInfo.nRankState = recv->nRankStatus;		//랭킹 상황(eRankStatus)
	RankInfo.nUpDownPoint=	recv->nModifier;			//순위 변동수치
	RankInfo.nMyRankPrecent = recv->nRankPercent;			//자신의 랭크위치
	RankInfo.bAbsRanking = recv->nRankType;			//자신의 랭크위치 분류(0 절대값 , 1상대값(%))
	RankInfo.i64RestRankRenewSec =	recv->nRankRenewSec;		//다음 랭킹 갱신 시간
	RankInfo.i64RestRankEndSec = recv->nRankEndSec;			//현 랭킹 마감 시간
	RankInfo.listRanker.clear();
	_SetRewardItemFromMyRanking( recv->nType, RankInfo.nMyRanking, RankInfo.nMyRankPrecent, RankInfo.listRewardItems );

	std::list<nArenaRank::st_TopRankInfo>::const_iterator it = recv->lRankList.begin();
	for( ; it != recv->lRankList.end(); ++it )
	{
		sRankerList addItem;
		addItem.dwPoint = (*it).nPoint;
		addItem.nRank = (*it).nRank;
		addItem.bIsNew = (*it).nIsNew;			//신규진입인가
		addItem.nUpDownPoint = (*it).nModifier;	//등수 변동 수치
		DmCS::StringFn::From( addItem.wsGuildName, (*it).szGuild );
		DmCS::StringFn::From( addItem.wsTamerName, (*it).szName );
		RankInfo.listRanker.push_back( addItem );
	}

	int nUpdateRankType =  recv->nType;

	ContentsStream kTmp;
	kTmp << nUpdateRankType;
	Notify( eUpdate_RankInfoByType, kTmp);
}

//////////////////////////////////////////////////////////////////////////
// 랭킹 타입(주간/월간/시즌), 내 랭킹, 내 랭킹 퍼센트 로 획득할 수 있는 아이템 목록을 셋팅 한다.
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarRankRewardContents::_SetRewardItemFromMyRanking( int const& nRankType, 
																  int const& nMyRanking, 
																  int const& nRankPercent, 
																  std::list<TB::sRewardItemInfo>& itemlist  )
{
	itemlist.clear();

	if( 0 == nMyRanking )
		return;

	SAFE_POINTER_RET( nsCsFileTable::g_pInfiniteWarTBMng );
	CsInfiniteWarMng::RankRewardItemsContanier const& pMaps = nsCsFileTable::g_pInfiniteWarTBMng->GetRewardData_Rank();

	CsInfiniteWarMng::RankRewardItemsContanier_CIT itS = pMaps.find( nRankType );
	if( itS == pMaps.end() )
		return;
	
	CsInfiniteWarMng::RankRewardItems_CIT rankIt = itS->second.begin();
	for( ; rankIt != itS->second.end(); ++rankIt )
	{
		if( 0 == (*rankIt).s_nRankType )// 절대 수치 
		{
			if( (*rankIt).s_nRankMin > nMyRanking || (*rankIt).s_nRankMax < nMyRanking )
				continue;
		}
		else
		{
			if( (*rankIt).s_nRankMin > nRankPercent || (*rankIt).s_nRankMax < nRankPercent )
				continue;
		}

		itemlist = (*rankIt).s_listRewardItems;
		break;
	}
}

void CInfiniteWarRankRewardContents::_RecvPreRankerList(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ARENA_REQUEST_OLD_RANK* recv = static_cast<GS2C_RECV_ARENA_REQUEST_OLD_RANK*>(pData);

	if(0 != recv->nResult )
		return;

	sRankInfos & RankInfo = _GetRankInfoByType( recv->nType );// 주/월/시즌 
	RankInfo.nPreviouslyMyRanking = recv->nRank;				//자신의 절대 등수 , 자신이 참여안해서 정보가없으면 0
	RankInfo.PriviouslyData.bRecvData = true;
	RankInfo.PriviouslyData.listPreviouslyRanker.clear();
	std::list<nArenaRank::st_TopRankOldInfo>::const_iterator it = recv->lRankList.begin();
	for( ; it != recv->lRankList.end(); ++it )
	{
		sPreviouslyRankerInfo addInfo;
		addInfo.nRank = (*it).nRank;
		addInfo.dwPoint = (*it).nPoint;
		DmCS::StringFn::From( addInfo.wsGuildName, (*it).szGuild );
		DmCS::StringFn::From( addInfo.wsTamerName, (*it).szName );
		RankInfo.PriviouslyData.listPreviouslyRanker.push_back( addInfo );
	}

	int nUpdateRankType =  recv->nType;
	ContentsStream kTmp;
	kTmp << nUpdateRankType;
	Notify( eUpdate_Previously_RankerList, kTmp );
}

CInfiniteWarRankRewardContents::sFamData& CInfiniteWarRankRewardContents::_GetFameRankerList( const int & nSeason )
{	
	std::map<int,sFamData>::iterator it = m_sHallOfFameData.m_mapFameRankerList.find( nSeason );
	if( it == m_sHallOfFameData.m_mapFameRankerList.end() )
	{
		auto rib = m_sHallOfFameData.m_mapFameRankerList.insert( std::make_pair( nSeason, sFamData() ) );
		it = rib.first;
	}

	return it->second;
}

void CInfiniteWarRankRewardContents::_RecvHallofFameList(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ARENA_REQUEST_FAME* recv = static_cast<GS2C_RECV_ARENA_REQUEST_FAME*>(pData);

	if(0 != recv->nResult )
		return;

	sFamData& listInfo = _GetFameRankerList( recv->nSeason );
	listInfo.bRecvData = true;
	listInfo.RankerList.clear();

	std::list<nArenaRank::st_TopRankFameInfo>::const_iterator it = recv->lRankList.begin();
	for( ; it != recv->lRankList.end(); ++it )
	{
		sFameRankerInfo addInfo;
		addInfo.dwPoint = (*it).nPoint;
		addInfo.nRank = (*it).nRank;
		DmCS::StringFn::From( addInfo.wsTamerName, (*it).szName );
		DmCS::StringFn::From( addInfo.wsGuildName, (*it).szGuild );
		listInfo.RankerList.push_back( addInfo );
	}

	int nUpdateSeasonIdx =  recv->nSeason;
	ContentsStream kTmp;
	kTmp << nUpdateSeasonIdx;
	Notify( eUpdate_FameRankerList, kTmp );
}

void CInfiniteWarRankRewardContents::_RecvHallofFameSeasonList(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ARENA_REQUEST_FAME_LIST* recv = static_cast<GS2C_RECV_ARENA_REQUEST_FAME_LIST*>(pData);
	std::list<u2>::const_iterator it = recv->lSeasonList.begin();

	m_sHallOfFameData.InitData();
	m_sHallOfFameData.bRecvData = true;

	for( ; it != recv->lSeasonList.end(); ++it )
		m_sHallOfFameData.m_mapFameRankerList.insert( std::make_pair( (*it), sFamData() ) );
	Notify( eUpdate_FameSeasonList );
}
