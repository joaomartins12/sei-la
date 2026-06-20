#include "StdAfx.h"
#include "InfiniteWar_Contents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"


CInfiniteWarContents::sStageInfo::sStageInfo() : s_dwMonsterIDX(0)
{
	s_wsMonsterName = L"???";
	s_sMonsterImageFile = "Data\\Interface\\InfiniteMatch\\Infinite_Match_secret.tga";//EmptyIconImageFile
}

CInfiniteWarContents::sStageInfo::~sStageInfo()
{
}

bool CInfiniteWarContents::sStageInfo::IsClear() const
{
	if( 0 == s_dwMonsterIDX )
		return false;

	return true;
}

std::string const& CInfiniteWarContents::sStageInfo::GetMonsterIconFile() const
{
	return s_sMonsterImageFile;
}

std::wstring const& CInfiniteWarContents::sStageInfo::GetMonsterName() const
{
	return s_wsMonsterName;
}

void CInfiniteWarContents::sStageInfo::SetClearMonster(DWORD const & dwMonsterIDX)
{
	s_dwMonsterIDX = dwMonsterIDX;
	SAFE_POINTER_RET( nsCsFileTable::g_pMonsterMng );
	SAFE_POINTER_RET( g_pModelDataMng );

	CsMonster* pMonster = nsCsFileTable::g_pMonsterMng->GetMonster( dwMonsterIDX );
	SAFE_POINTER_RET( pMonster );

	CsMonster::sINFO* pTInfo = pMonster->GetInfo();
	SAFE_POINTER_RET( pTInfo );

	s_wsMonsterName = pTInfo->s_szName;
	s_sMonsterImageFile = g_pModelDataMng->GetLargeModelIconFile( pTInfo->s_dwModelID );
}
//////////////////////////////////////////////////////////////////////////


std::string const& CInfiniteWarContents::sInfiniteWarInfo::GetMonsterIconFile() const
{
	return s_ClearInfo.GetMonsterIconFile();
}

std::wstring const& CInfiniteWarContents::sInfiniteWarInfo::GetMonsterName() const
{
	return s_ClearInfo.GetMonsterName();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int const CInfiniteWarContents::IsContentsIdentity(void)
{
	return E_CT_INFINITEWAR_CONTENTS;
}

CInfiniteWarContents::CInfiniteWarContents(void):m_pTarget(NULL),m_dwTargetNpcIdx(0),m_dwTotalHavePoint(0),m_AdaptPartySystem(NULL),m_bBeChallenging(false)
,m_nLastClearStage(0),m_nNextChallengeStage(1),m_nOpenWindowWaitForMonsterToDie(0),m_dwWarNpcIdx(0)// 최초 도전 스테이지는 첫번째이다
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_INFINITEWAR_START_WINDOW, this, &CInfiniteWarContents::_OpenWarStartWindow );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_INFINITEWAR_GIVEUP_WINDOW, this, &CInfiniteWarContents::_OpenWarGiveUpWindow );

	// 무한 대전 스테이지 클리어
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_CHALLENGESTATECLEAR, this, &CInfiniteWarContents::_ReceChallengeStageClear );
	// 무한 대전 다음 스테이지
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_CHALLENGETRY_NEXTSTAGE, this, &CInfiniteWarContents::_ReceChallengeNextStage );
	// 무한 대전 포기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_CHALLENGE_GIVEUP, this, &CInfiniteWarContents::_RecvChallengeGiveup );
	// 무한 대전 맵 이동 실패
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_INFINITEWAR_MAPMOVE_FALSE, this, &CInfiniteWarContents::_RecvMapMoveFalse );
}

CInfiniteWarContents::~CInfiniteWarContents(void)
{
	m_AdaptPartySystem = NULL;
	GAME_EVENT_ST.DeleteEventAll( this );
	EventRouter()->UnRegisterAll( this );
}

int const CInfiniteWarContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CInfiniteWarContents::Initialize(void)
{

	return true;
}

void CInfiniteWarContents::UnInitialize(void)
{
}

bool CInfiniteWarContents::IntraConnection(ContentsSystem& System)
{ 
	m_AdaptPartySystem = new AdaptPartySystem;
	if( m_AdaptPartySystem )
		m_AdaptPartySystem->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_PARTYSYSTEM ) );

	return true; 
}

void CInfiniteWarContents::Update(float const& fElapsedTime)
{
	if( 0 == m_nOpenWindowWaitForMonsterToDie )
		return;

	if( !m_nWaitTimer.IsEnableTime() )
	{
		if( g_pCharMng )
		{
			std::map< DWORD, CMonster* >* pMonsters = g_pCharMng->GetMonsterVector();
			if( pMonsters && 0 == pMonsters->size())
			{
				m_nWaitTimer.SetEnableTime(true);
				m_nWaitTimer.SetDeltaTime( 3000 );
				m_nWaitTimer.OffReset();
			}
		}
	}
	else if( m_nWaitTimer.IsEnable() )
	{
		switch( m_nOpenWindowWaitForMonsterToDie )
		{
		case cBaseWindow::WT_INFINITEWAR_NEXTSTAGE_WINDOW:	OpenCloseNextStageTryWindow(true);			break;
		case cBaseWindow::WT_INFINITEWAR_ALLCLEAR_WINDOW:	OpenCloseAllClearMessageWindow(true);		break;
		}
		m_nOpenWindowWaitForMonsterToDie = 0;
		m_nWaitTimer.SetEnableTime(false);
		m_nWaitTimer.Reset();
	}	
}

void CInfiniteWarContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CInfiniteWarContents::MakeMainActorData(void)
{
	_InitData();
}

void CInfiniteWarContents::ClearMainActorData(void)
{
	_InitData();
	ClearAllRegistered();
}

void CInfiniteWarContents::MakeWorldData(void)
{

}

void CInfiniteWarContents::ClearWorldData(void)
{
}

//////////////////////////////////////////////////////////////////////////


void CInfiniteWarContents::_InitData()
{
	m_pTarget = NULL;
	m_dwTargetNpcIdx = 0;
	m_dwTotalHavePoint = 0;
	m_dwWarNpcIdx = 0;
	m_WarStageInfo.clear();
	m_nNextChallengeStage = 1;
	m_nLastClearStage = 0;
	m_bBeChallenging = false;
	m_nOpenWindowWaitForMonsterToDie = 0;
	m_nWaitTimer.SetEnableTime(false);
	m_nWaitTimer.Reset();
	OpenCloseStartWindow(false);
	OpenCloseNextStageTryWindow(false);
	OpenCloseGiveupWindow(false);
	OpenCloseAllClearMessageWindow(false);
}

void CInfiniteWarContents::_ClearData()
{
	m_pTarget = NULL;
	m_dwTargetNpcIdx = 0;
}


DWORD const& CInfiniteWarContents::GetTotalHavePoint() const{	return m_dwTotalHavePoint;}// 현재 보유한 포인트
int CInfiniteWarContents::GetLastClearStageIdx() const		{	return m_nLastClearStage;}// 마지막으로 클리어한 스테이지 인덱스
int CInfiniteWarContents::GetNextChallengeStageIdx() const	{	return m_nNextChallengeStage;}// 도전해야할 스테이지
bool CInfiniteWarContents::IsStageAllClear()
{
	MAP_WARINFO const* pCurrentWarInfo = GetInfiniteWarTatalInfo();
	SAFE_POINTER_RETVAL( pCurrentWarInfo, true );

	MAP_WARINFO_CIT it = pCurrentWarInfo->begin();
	for( ; it != pCurrentWarInfo->end(); ++it )
	{
		if( !it->second.s_ClearInfo.IsClear() )
			return false;
	}

	return true;
}

// 다음 스테이지를 클리어 했을 경우 획득하는 포인트
int CInfiniteWarContents::GetNextStageWinPoint()
{
	sInfiniteWarInfo const* pStageInfo = GetWarInfo(m_nNextChallengeStage);
	SAFE_POINTER_RETVAL( pStageInfo, 0 );
	return pStageInfo->s_nWinPoint;
}

// 다음 스테이지를 실패 했을 경우 차감되는 포인트
int CInfiniteWarContents::GetNextStageLosePoint()
{
	sInfiniteWarInfo const* pStageInfo = GetWarInfo(m_nNextChallengeStage);
	SAFE_POINTER_RETVAL( pStageInfo, 0 );
	return pStageInfo->s_nLosePoint;
}

// 현재 타겟의 포인터 - NPC와의 거리가 멀어질 경우 UI를 닫기 용도
CsC_AvObject* CInfiniteWarContents::GetTargetNpc() const
{
	return m_pTarget;
}

// 
CInfiniteWarContents::MAP_WARINFO const* CInfiniteWarContents::GetInfiniteWarTatalInfo()
{
	DWORD dwloadNpcIdx = m_dwTargetNpcIdx == 0 ? m_dwWarNpcIdx : m_dwTargetNpcIdx;
	if( !_LoadInfiniteWarTableData( dwloadNpcIdx ) )
	{
		//BHPRT( "Infinite War Table Load False = m_dwTargetNpcIdx : %d, m_dwWarNpcIdx : %d", m_dwTargetNpcIdx,m_dwWarNpcIdx);
		return NULL;
	}

	std::map<DWORD, MAP_WARINFO >::const_iterator it = m_WarStageInfo.find( dwloadNpcIdx );
	if( it == m_WarStageInfo.end() )
		return NULL;

	return &it->second;
}

CInfiniteWarContents::sInfiniteWarInfo const* CInfiniteWarContents::GetWarInfo( int const& nStageIdx )
{
	MAP_WARINFO const* stageInfo = GetInfiniteWarTatalInfo();
	SAFE_POINTER_RETVAL( stageInfo, NULL );

	MAP_WARINFO_CIT it = stageInfo->find( nStageIdx );
	if( it == stageInfo->end() )
		return NULL;

	return &it->second;
}

CInfiniteWarContents::sStageInfo const* CInfiniteWarContents::GetStageInfo( int const& nStageIdx )
{
	CInfiniteWarContents::sInfiniteWarInfo const* pWarInfo = GetWarInfo( nStageIdx ); 
	SAFE_POINTER_RETVAL(pWarInfo, NULL);
	return &pWarInfo->s_ClearInfo;
}

// 무한대전 시작 및 종료 할 수 있는 권한은 파티장이거나 혼자 들어 왔을 때
bool CInfiniteWarContents::IsPartyMaster() const
{
	SAFE_POINTER_RETVAL( m_AdaptPartySystem, false );
	if( m_AdaptPartySystem->IsJoinParty() )
		return m_AdaptPartySystem->IsMePartyMaster();
	return true;
}

void CInfiniteWarContents::_OpenWarStartWindow(void* pData)
{
	SAFE_POINTER_RET( pData );

	CsC_AvObject* pTargetObject = static_cast<CsC_AvObject*>(pData);
	assert_cs( pTargetObject->GetLeafRTTI() == RTTI_NPC );

	if( m_bBeChallenging )
	{
		cPrintMsg::PrintMsg( 30900 ); //도전 중에는 창을 열수 없다.
		return;
	}

	DWORD dwTargetObjIdx = pTargetObject->GetFTID();
	if( m_dwWarNpcIdx > 0 && dwTargetObjIdx != m_dwWarNpcIdx )
	{
		// 진행 중인 NPC가 아닌경우 에러 메시지 출력
		cPrintMsg::PrintMsg( 30902 );
		_ClearData();
		return;
	}

	m_pTarget = pTargetObject;
	m_dwTargetNpcIdx = dwTargetObjIdx;

	if( _LoadInfiniteWarTableData( m_dwTargetNpcIdx ) )
		OpenCloseStartWindow(true);
	else
		_ClearData();
}

void CInfiniteWarContents::_OpenWarGiveUpWindow(void* pData)
{
	if( m_bBeChallenging )
	{
		cPrintMsg::PrintMsg( 30900 ); //도전 중에는 포기 할 수 없다.
		return;
	}

	CsC_AvObject* pTargetObject = static_cast<CsC_AvObject*>(pData);
	assert_cs( pTargetObject->GetLeafRTTI() == RTTI_NPC );

	DWORD dwTargetObjIdx = pTargetObject->GetFTID();
	m_pTarget = pTargetObject;
	m_dwTargetNpcIdx = dwTargetObjIdx;

	OpenCloseGiveupWindow(true);
}

//////////////////////////////////////////////////////////////////////////
// Server Recv Function
//////////////////////////////////////////////////////////////////////////
// 현재 스테이지 클리어
void CInfiniteWarContents::_ReceChallengeStageClear(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_CHALLENGE_STAGE_CLEAR * recv = static_cast<GS2C_RECV_CHALLENGE_STAGE_CLEAR*>(pData);

// 	BHPRT( "Recv - GS2C_RECV_CHALLENGE_STAGE_CLEAR");
// 	BHPRT( "	nCurrentPlayedStage : %d", recv->nCurrentPlayedStage );
// 	BHPRT( "	bIsLastStage : %d", recv->bIsLastStage );
// 	BHPRT( "	nMonsterIDX : %u", recv->nMonsterIDX );
// 	BHPRT( "	nTotalPoint : %d", recv->nTotalPoint );
// 	BHPRT( "	nCurrentStagePoint : %d", recv->nCurrentStagePoint );
// 	BHPRT( "	nNpcIDX : %d", recv->nNpcIDX );

	m_bBeChallenging = false;

	// 클리어한 스테이지 정보 갱신
	_SetCurrentWarNpcIdx( recv->nNpcIDX );
	_UpdateClearStageInfo( recv->nCurrentPlayedStage, recv->nMonsterIDX );

	// 스테이지 클리어시 획득 한 포인트 갱신
	_Update_Point( recv->nTotalPoint, recv->nCurrentStagePoint );	

	// 모두 클리어 하지 않았을 경우
	bool bServerAllClear = recv->bIsLastStage;
	bool bClientAllClear = IsStageAllClear();
// 	if( bClientAllClear != bServerAllClear )
// 		BHPRT( "GS2C_RECV_CHALLENGE_STAGE_CLEAR Not Same Stage All Clear = Client : %d, Server : %d", bClientAllClear,bServerAllClear);
		
	if( !recv->bIsLastStage )
		m_nOpenWindowWaitForMonsterToDie = cBaseWindow::WT_INFINITEWAR_NEXTSTAGE_WINDOW;
	else 
		m_nOpenWindowWaitForMonsterToDie = cBaseWindow::WT_INFINITEWAR_ALLCLEAR_WINDOW;
}

//////////////////////////////////////////////////////////////////////////
// 현재 스테이지 클리어한 데이터 셋팅
bool CInfiniteWarContents::_UpdateClearStageInfo( int const& nStageNumber, DWORD const& dwClearMonsterID)
{
	DWORD dwloadNpcIdx = m_dwTargetNpcIdx == 0 ? m_dwWarNpcIdx : m_dwTargetNpcIdx;
	if( !_LoadInfiniteWarTableData( dwloadNpcIdx ) )
	{
		//BHPRT( "_SetClearStageInfo Infinite War Table Load False = m_dwTargetNpcIdx : %d, m_dwWarNpcIdx : %d", m_dwTargetNpcIdx,m_dwWarNpcIdx);
		return false;
	}

	std::map<DWORD, MAP_WARINFO >::iterator it = m_WarStageInfo.find( dwloadNpcIdx );
	if( it == m_WarStageInfo.end() )
		return false;

	std::map<int,sInfiniteWarInfo>::iterator subIT = it->second.find( nStageNumber );
	if( subIT == it->second.end() )
		return false;

	subIT->second.s_ClearInfo.SetClearMonster( dwClearMonsterID );
	m_nLastClearStage = nStageNumber;
	m_nNextChallengeStage = m_nLastClearStage + 1;
	if( m_nNextChallengeStage > it->second.size() )
		m_nNextChallengeStage = -1;
	
	return true;
}

// 다음 스테이지 진행 시작
void CInfiniteWarContents::_ReceChallengeNextStage(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_CHALLENGE_RES_NEXT_STAGE* pRecv = static_cast<GS2C_RECV_CHALLENGE_RES_NEXT_STAGE*>(pData);

	//BHPRT( "Recv - GS2C_RECV_CHALLENGE_RES_NEXT_STAGE = eRes : %d, nNpcIDX : %d, nStartCountDown : %d", pRecv->eRes, pRecv->nNpcIDX, pRecv->nStartCountDown );

	m_bBeChallenging = true;

	_SetCurrentWarNpcIdx( pRecv->nNpcIDX );
	// 전투가 시작되면 화면 UI 제거
	OpenCloseStartWindow(false);
	OpenCloseNextStageTryWindow(false);
	OpenCloseGiveupWindow(false);

	// 전투가 시작瑛?때 메시지 출력
	OpenCloseTimerWindow( true );
	float fTimer = pRecv->nStartCountDown * 0.001f;
	ContentsStream kTmp;
	kTmp << fTimer;
	Notify(eSet_Start_Timer, kTmp);
}

// 현재 진행 중인 대전의 NPC 인덱스 번호 저장
void CInfiniteWarContents::_SetCurrentWarNpcIdx( DWORD const& nNpcIDX )
{
	if( 0 == m_dwWarNpcIdx )
	{
		m_dwWarNpcIdx = nNpcIDX;
		return;
	}
	else if( m_dwWarNpcIdx == nNpcIDX )
		return;

	m_dwWarNpcIdx = nNpcIDX;
	//BHPRT( "Not Match Npc IDX = Client NpcIDX : %d, Server NpcIDX : %d", m_dwWarNpcIdx, nNpcIDX );
}

// 다음 스테이지 포기에 대한 결과값
void CInfiniteWarContents::_RecvChallengeGiveup(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHALLENGE_RES_GIVE_UP *recv = static_cast<GS2C_RECV_CHALLENGE_RES_GIVE_UP*>(pData);
	//BHPRT( "Recv - GS2C_RECV_CHALLENGE_RES_GIVE_UP = eRes : %d, nNpcIDX : %d", recv->eRes, recv->nNpcIDX );

	// 전투를 포기 했을 경우 메시지 출력
	//cPrintMsg::PrintMsg(  );
}

// 무한대전 입장시 에러 메시지 처리
void CInfiniteWarContents::_RecvMapMoveFalse(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHALLENGE_RES_PARTY_ENTER_FAIL *recv = static_cast<GS2C_RECV_CHALLENGE_RES_PARTY_ENTER_FAIL*>(pData);

	//BHPRT( "Recv - GS2C_RECV_CHALLENGE_RES_PARTY_ENTER_FAIL" );
	// 파티를 맺은 플레이어가 무한대전에 진입할 때 이미 전투가 시작瑛?때 에러 메시지
	cPrintMsg::PrintMsg( 11080 );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CInfiniteWarContents::_LoadInfiniteWarTableData( DWORD const& dwNpcIDX )
{
	std::map<DWORD, MAP_WARINFO >::iterator it = m_WarStageInfo.find( dwNpcIDX );
	if( it != m_WarStageInfo.end() )
		return true;

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pInfiniteWarTBMng, false );
	CsInfiniteWarMng::StageInfos const* pWarInfo = nsCsFileTable::g_pInfiniteWarTBMng->GetStageInfo( dwNpcIDX );
	SAFE_POINTER_RETVAL( pWarInfo, false );

	auto rib = m_WarStageInfo.insert( std::make_pair( dwNpcIDX, MAP_WARINFO() ) );
	it = rib.first;

	CsInfiniteWarMng::StageInfos_CIT itWar = pWarInfo->begin();
	for( ; itWar != pWarInfo->end(); ++itWar )
		_SetStageInfo( it->second, itWar->first, itWar->second );

	return true;
}

void CInfiniteWarContents::_SetStageInfo( MAP_WARINFO& Continer, int const& nStageNumber, TB::sStageInfos const& tbStage )
{
	sInfiniteWarInfo addStageInfo;
	addStageInfo.s_nLosePoint = tbStage.s_nLosePoint;
	addStageInfo.s_nWinPoint = tbStage.s_nWinPoint;
	Continer.insert( std::make_pair( nStageNumber, addStageInfo) );
}

//////////////////////////////////////////////////////////////////////////
void CInfiniteWarContents::_Update_Point( DWORD const& dwTotalPoint, int const& nCurrentGetPoint )
{
	if( dwTotalPoint != (m_dwTotalHavePoint + nCurrentGetPoint) )
		assert_csm( false, "서버와 클라이언트가 보유한 포인트가 다르다." );
	m_dwTotalHavePoint = dwTotalPoint;
	ContentsStream kTmp;
	kTmp << m_dwTotalHavePoint;
	Notify( eUpdate_Total_Point, kTmp );
}

//////////////////////////////////////////////////////////////////////////

void CInfiniteWarContents::OpenCloseStartWindow(bool bVisible)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( bVisible )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_START_WINDOW );
	else
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_START_WINDOW );
		_ClearData();
	}
}

void CInfiniteWarContents::OpenCloseGiveupWindow(bool bVisible)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( bVisible )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_GIVEUP_WINDOW );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_GIVEUP_WINDOW );
}

void CInfiniteWarContents::OpenCloseNextStageTryWindow(bool bVisible)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( bVisible )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_NEXTSTAGE_WINDOW );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_NEXTSTAGE_WINDOW );
}

void CInfiniteWarContents::OpenCloseAllClearMessageWindow(bool bVisible)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( bVisible )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_ALLCLEAR_WINDOW );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_ALLCLEAR_WINDOW );
}

void CInfiniteWarContents::OpenCloseTimerWindow(bool bVisible)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( bVisible )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INFINITEWAR_START_TIMER_WINDOW );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INFINITEWAR_START_TIMER_WINDOW );
}

void CInfiniteWarContents::AskGiveup()
{
	OpenCloseStartWindow(false);

	if( m_dwTargetNpcIdx == 0 )
		return;

	if( m_dwWarNpcIdx > 0 && m_dwWarNpcIdx != m_dwTargetNpcIdx ) // 현재 
	{
		cPrintMsg::PrintMsg( 30902 );
		return;
	}

	OpenCloseGiveupWindow(true);
}

//////////////////////////////////////////////////////////////////////////
// 서버에 도전 포기 패킷 전송
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarContents::SendInfiniteWarGiveup()
{
	SAFE_POINTER_RET(net::game);
	DWORD dwNpcIdx = m_dwTargetNpcIdx > 0 ? m_dwTargetNpcIdx : m_dwWarNpcIdx;
	net::game->Send_InfiniteWar_ChallengeGiveup(dwNpcIdx);
	//BHPRT( "Send - C2GS_SEND_CHALLENGE_REQ_GIVE_UP : NpcIdx = %d", dwNpcIdx );
	OpenCloseGiveupWindow(false);
}

//////////////////////////////////////////////////////////////////////////
// 모든 대전이 종료 瑛?때 서버에 나가기 패킷 보내기
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarContents::SendInfiniteWarLeave()
{
	if( !IsStageAllClear() )
	{
		// 모든 스테이지를 클리어 하지 않았는데 패킷을 보내면 포기가 된다
		return;
	}

	SAFE_POINTER_RET(net::game);
	DWORD dwNpcIdx = m_dwTargetNpcIdx > 0 ? m_dwTargetNpcIdx : m_dwWarNpcIdx;
	net::game->Send_InfiniteWar_ChallengeGiveup(dwNpcIdx);
	//BHPRT( "Send - C2GS_SEND_CHALLENGE_REQ_GIVE_UP : NpcIdx = %d", dwNpcIdx );
	OpenCloseAllClearMessageWindow(false);
}

//////////////////////////////////////////////////////////////////////////
// 서버에 다음 도전 시작 패킷 전송
//////////////////////////////////////////////////////////////////////////
void CInfiniteWarContents::SendInfiniteWarTryNextStage_TargetNpc()
{
	SAFE_POINTER_RET(net::game);
	if( 0 == m_dwTargetNpcIdx )
		return;

	if( m_dwWarNpcIdx > 0 && m_dwWarNpcIdx != m_dwTargetNpcIdx )
	{
		//에러메시지
		cPrintMsg::PrintMsg( 30902 );
		return;
	}

	net::game->Send_InfiniteWar_ChallengeTryNextStage( m_dwTargetNpcIdx );
	//BHPRT( "Send - C2GS_SEND_CHALLENGE_REQ_NEXT_STAGE : NpcIdx = %d", m_dwTargetNpcIdx );
	OpenCloseStartWindow(false);
}
void CInfiniteWarContents::SendInfiniteWarTryNextStage()
{
	SAFE_POINTER_RET(net::game);
	if( 0 == m_dwWarNpcIdx )
		return;

	net::game->Send_InfiniteWar_ChallengeTryNextStage( m_dwWarNpcIdx );
	//BHPRT( "Send - C2GS_SEND_CHALLENGE_REQ_NEXT_STAGE : NpcIdx = %d", m_dwWarNpcIdx );
	OpenCloseStartWindow(false);
}

std::wstring CInfiniteWarContents::GetReturnMapName()
{
	SAFE_POINTER_RETVAL( nsCsMapTable::g_pMapListMng, L"" );
	SAFE_POINTER_RETVAL( nsCsGBTerrain::g_pCurRoot, L"" );

	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, L"" );

	CsMapList* pMapInfo = nsCsMapTable::g_pMapListMng->GetList(pInfo->s_dwMapID);	
	SAFE_POINTER_RETVAL( pMapInfo, L"" );
	CsMapList::sINFO* pCurrentMapInfo =pMapInfo->GetInfo();
	SAFE_POINTER_RETVAL( pCurrentMapInfo, L"" );

	CsMapList* pReturnMap = nsCsMapTable::g_pMapListMng->GetList(pCurrentMapInfo->s_dwResurrectionMapID);	
	SAFE_POINTER_RETVAL( pReturnMap, L"" );
	CsMapList::sINFO* pReturnMapInfo = pReturnMap->GetInfo();
	SAFE_POINTER_RETVAL( pReturnMapInfo, L"" );

	return pReturnMapInfo->s_szMapDiscript;
}
