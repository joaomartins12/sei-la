#include "StdAfx.h"
#include "../../Flow/Flow.h"
#include "TutorialContents.h"

#include "../11.Tutorial/TutorialQuest.h"
#include "../11.Tutorial/TutorialIndicator.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int const CTutorialContents::IsContentsIdentity(void)
{
	return E_CT_TUTORIAL_CONTENTS;
}

CTutorialContents::CTutorialContents(void)
:m_bTutorialQuestCompleted(true),m_pQuestIndicatorInfo(NULL),m_pTutorialQuest(NULL),m_nUpdateLoadingStop(true)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_SKIP_ACCEPT, this, &CTutorialContents::_TutorialQuest_Skip_Accept );	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_IS_TUTORIAL_PLAYING, this, &CTutorialContents::_IsTutorialPlaying );

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PLAYER_DATA_LOAD_COMPLETE, this, &CTutorialContents::_TutorialEvent_Initialize );
	GAME_EVENT_ST.AddEvent(EVENT_CODE::RECV_QUEST_ALL_INFO, this, &CTutorialContents::_CheckAndOpenDoor);
#endif
}

CTutorialContents::~CTutorialContents(void)
{
	m_pQuestIndicatorInfo = 0;
	m_pTutorialQuest = 0;
	if( GAME_EVENT_STPTR )
		GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const CTutorialContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}


bool CTutorialContents::Initialize(void)
{
	return true;
}

void CTutorialContents::UnInitialize(void)
{
}

bool CTutorialContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void CTutorialContents::_AddTutorialQuestEvent()
{
	SAFE_POINTER_RET( GAME_EVENT_STPTR );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_ACCEPT, this, &CTutorialContents::_TutorialQuest_Accept );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_CANCEL, this, &CTutorialContents::_TutorialQuest_Cancel );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_COMPLETE, this, &CTutorialContents::_TutorialQuest_Complete );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_PROCESS_UPDATE, this, &CTutorialContents::_TutorialQuest_ProcessUpdate );	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_PROCESS_ALL_COMPLETE, this, &CTutorialContents::_TutorialQuest_AllComplete );	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_DIGIVICE_CHECK, this, &CTutorialContents::_TutorialQuest_CheckNewDigivice );	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TUTORIALQUEST_ASK_CANCEL, this, &CTutorialContents::_TutorialQuest_AskCancel );		
}

void CTutorialContents::_RemoveTutorialQuestEvet()
{
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::TUTORIALQUEST_ACCEPT, this );
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::TUTORIALQUEST_CANCEL, this );
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::TUTORIALQUEST_COMPLETE, this );
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::TUTORIALQUEST_PROCESS_UPDATE, this );	
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::TUTORIALQUEST_PROCESS_ALL_COMPLETE, this );	
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::TUTORIALQUEST_DIGIVICE_CHECK, this );	
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::TUTORIALQUEST_ASK_CANCEL, this );
}

void CTutorialContents::Update(float const& fElapsedTime)
{
	if( m_bTutorialQuestCompleted || m_nUpdateLoadingStop )
		return;

	if( m_pTutorialQuest )
		m_pTutorialQuest->Update( fElapsedTime );

	if( m_pQuestIndicatorInfo )
		m_pQuestIndicatorInfo->Update( fElapsedTime );
}


void CTutorialContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CTutorialContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void CTutorialContents::MakeMainActorData(void)
{
	m_nUpdateLoadingStop = false;
	// 맵 이동시마다 수행 가능한 퀘스트 검사
	if( m_pTutorialQuest )
	{
		m_pTutorialQuest->ReloadMap();
		m_pTutorialQuest->RecheckTutorial();
	}

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	if( net::next_map_no != 4 )
	{
		GAME_EVENT_ST.DeleteEvent( EVENT_CODE::RECV_PLAYER_DATA_LOAD_COMPLETE, this );
		GAME_EVENT_ST.DeleteEvent( EVENT_CODE::SKIP_TUTORIAL_EVENT, this );

		_TutorialEvent_ClearData();
	}
#endif
}

void CTutorialContents::ClearMainActorData(void)
{
	m_nUpdateLoadingStop = true;
#ifdef LJW_TUTORIAL_RENEWAL_20191128
	_TutorialEvent_ClearData();
#endif
}

void CTutorialContents::OnOverride(int const& nNextFlowID)
{
	if( nNextFlowID == Flow::CFlow::FLW_LOADING )
		return;

	m_nUpdateLoadingStop = false;
}

// void CTutorialContents::OnResume(int const& nBeforFlowID)
// {
// 	if( nBeforFlowID != Flow::CFlow::FLW_LOADING )
// 		return;
// 
// 	m_nUpdateLoadingStop = true;
// }

bool CTutorialContents::IsTutorialPlaying() const
{
	if( m_bTutorialQuestCompleted )
		return false;

	SAFE_POINTER_RETVAL( m_pQuestIndicatorInfo, false );
	SAFE_POINTER_RETVAL( m_pTutorialQuest, false );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 최초 게임 진입시
// 완료하지 않은 튜토리얼 퀘스트가 있는지 검사
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::MakeWorldData(void)
{
#ifdef LJW_TUTORIAL_RENEWAL_20191128
	SAFE_POINTER_RET(g_pCharMng);
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pTamer);

	// 현재 튜토리얼 퀘스트 정보 알고 있어야 하므로 일단 로드
	_LoadTutorialQuest();

	// 이미 예전 튜토리얼 퀘스트를 완료처리했거나 스킵했다면
	if (!pTamer->IsServerOption(CTamerUser::SO_DO_TUTORIAL))
	{
		// 현재 튜토리얼 퀘스트 완료 안했으면
		if (!_CheckCurTutorialQuestComplete())
		{
			// 완료 처리를 위해 서버에 요청
			if (net::game)
				net::game->SendClientOptionSave(pTamer->GetServerOption());
		}
		_ClearTutorialQuestData();
		return;
	}

	// 예전 튜토리얼 퀘스트를 스킵하지 않고 완료했으면
	// -> 완료 처리가 되지 않은 경우
	m_bTutorialQuestCompleted = _CheckPreTutorialQuestComplete();
	if (m_bTutorialQuestCompleted)
	{
		// 튜토리얼 퀘스트 완료 처리
		TutorialQuestCompleteAll();
		_ClearTutorialQuestData();
		return;
	}

	// 포탈 이용시 문제가 발생해서 튜토리얼 진행중에 디지몬 이동을 못하게 함.
	if (g_pResist && g_pResist->m_Global.s_bMoveDigimon)
	{
		g_pResist->m_Global.s_bMoveDigimon = FALSE;
		if (g_pServerMoveOwner)
			g_pServerMoveOwner->SetMoveOption(g_pResist->m_Global.s_bMoveDigimon);
	}

	_AddTutorialQuestEvent();
#else
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamer );
	if( !pTamer->IsServerOption(CTamerUser::SO_DO_TUTORIAL) )
		return;

	m_bTutorialQuestCompleted = _CheckTutorialQuestAllComplete();
	if( m_bTutorialQuestCompleted )// 튜토리얼 퀘스트를 모두 완료 한 경우
	{	
// 		// 서버에서는 튜토리얼 진행 중이라고 하는데
// 		// 튜토리얼 퀘스트가 없는 경우
// 		// 서버에 튜토리얼 퀘스트 진행이 아니라고 알려 줌.
// 		pTamer->RemoveServerOption( CTamerUser::SO_DO_TUTORIAL );
// 		// 서버에 패킷을 보내면 서버에서 
// 		// 맵 이동 시킴
// 		if( net::game )
// 			net::game->SendClientOptionSave( pTamer->GetServerOption() );
		return;
	}
	
	// 포탈 이용시 문제가 발생해서 튜토리얼 진행중에 디지몬 이동을 못하게 함.
	if( g_pResist && g_pResist->m_Global.s_bMoveDigimon )
	{
		g_pResist->m_Global.s_bMoveDigimon = FALSE;
		if( g_pServerMoveOwner )
			g_pServerMoveOwner->SetMoveOption( g_pResist->m_Global.s_bMoveDigimon );
	}

	_AddTutorialQuestEvent();

	m_pQuestIndicatorInfo = new cIndicatorInfos;
	if( m_pQuestIndicatorInfo )
	{
		m_pQuestIndicatorInfo->Construct();
		m_pQuestIndicatorInfo->LoadTutorialScript();
	}

	m_pTutorialQuest = new cTutorialQuest;
	if( m_pTutorialQuest )
	{	// 진행중인 튜토리얼 퀘스트가 있는지 체크 및 수행 가능한 퀘스트 체크
		m_pTutorialQuest->Construct();
		m_pTutorialQuest->LoadTutorialQuest();
	}
#endif
}

void CTutorialContents::ClearWorldData(void)
{
	m_pTutorialQuest = 0;
	m_pQuestIndicatorInfo = 0;

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PLAYER_DATA_LOAD_COMPLETE, this, &CTutorialContents::_TutorialEvent_Initialize );
#endif
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트를 모두 완료 했는지 체크 함수
//////////////////////////////////////////////////////////////////////////
bool CTutorialContents::_CheckTutorialQuestAllComplete() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, true );
	cData_Quest* pQuestInfo = g_pDataMng->GetQuest();
	SAFE_POINTER_RETVAL( pQuestInfo, true );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pQuestMng, true );	

	std::list<DWORD> tutorialQuest;
	nsCsFileTable::g_pQuestMng->GetQuestListFormType( CsQuest::QT_TUTORIAL, tutorialQuest );

	std::list<DWORD>::const_iterator it = tutorialQuest.begin();
	for( ; it != tutorialQuest.end(); ++it)
	{
		if( !pQuestInfo->IsCompleate( *it ) )
			return false;
	}

	return true;
}

void CTutorialContents::AddProcessTutorialQuest( DWORD const& dwQuestID )
{
	ContentsStream kTmp;
	kTmp << dwQuestID;
	Notify( eSetProcessTutorialQuest, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 수락시 호출됨
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_Accept( void* data )
{
	SAFE_POINTER_RET( data );	
	DWORD dwQuestID = *static_cast<DWORD*>(data);

	ContentsStream kTmpe;
	kTmpe << dwQuestID;
	Notify( eTutorialQuestAccpet, kTmpe );
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 포기 전 확인 메시지 띄울때 사용
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_AskCancel(void* pData)
{
	SAFE_POINTER_RET( pData );	
	DWORD dwQuestID = *static_cast<DWORD*>(pData);

	//20062 튜토리얼 퀘스트 포기 못한다는 메시지 번호
	cPrintMsg::PrintMsg( 20062 );

	//20062 튜토리얼 퀘스트 포기 못한다는 메시지 번호
	//cPrintMsg::PrintMsg( 30410 );
}

void CTutorialContents::TutorialQuest_Skip_Ask()
{
	if( cMessageBox::IsMessageBox(30410) )
		return;
	cPrintMsg::PrintMsg( 30410 );
}

void CTutorialContents::TutorialQuestCompleteAll()
{
	SAFE_POINTER_RET(g_pCharMng);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pUser);
	if (!pUser->IsServerOption(CTamerUser::SO_DO_TUTORIAL))
		return;

	pUser->RemoveServerOption(CTamerUser::SO_DO_TUTORIAL);
	// 서버에 패킷을 보내면 서버에서 
	// 맵 이동 시킴
	if (net::game)
		net::game->SendClientOptionSave(pUser->GetServerOption());

	// 전체 퀘스트 완료 처리할 때 한번만
	g_pResist->m_Global.s_bTipBalloone = true;
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 포기 확인 후 퀘스트 포기 패킷 날림.
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_Skip_Accept(void* pData)
{
	_ClearTutorialQuestData();
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_REV );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_COMP );

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	if( !pUser->IsServerOption( CTamerUser::SO_DO_TUTORIAL ) )
		return;

	pUser->RemoveServerOption( CTamerUser::SO_DO_TUTORIAL );
	// 서버에 패킷을 보내면 서버에서 
	// 맵 이동 시킴
	if( net::game )
		net::game->SendClientOptionSave( pUser->GetServerOption() );
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 포기 호출됨
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_Cancel(void* pData)
{
	SAFE_POINTER_RET( pData );	
	DWORD dwQuestID = *static_cast<DWORD*>(pData);

	ContentsStream kTmpe;
	kTmpe << dwQuestID;
	Notify( eTutorialQuestCancel, kTmpe );
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 완료시 호출됨
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_Complete( void* data )
{
	SAFE_POINTER_RET( data );
	DWORD dwQuestID = *static_cast<DWORD*>(data);

	ContentsStream kTmp;
	kTmp << dwQuestID;
	Notify( eTutorialQuestCompleted, kTmp );

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	m_bTutorialQuestCompleted = _CheckCurTutorialQuestComplete();
#else
	m_bTutorialQuestCompleted = _CheckTutorialQuestAllComplete();
#endif
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 완료 조건이 업데이트 됨
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_ProcessUpdate(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream kTmp = *static_cast<ContentsStream*>(pData);
	Notify( eProccesQuestConditionUpdate, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 완료 조건을 모두 완료 됬을 호출되는 함수
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_AllComplete(void* pData)
{
	SAFE_POINTER_RET( pData );
	DWORD dwQuestID = *static_cast<DWORD*>(pData);
	ContentsStream kTmp;
	kTmp << dwQuestID;
	Notify( eProccesQuestAllComplete, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 디지바이스로 받은 튜토리얼 퀘스트 체크
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_TutorialQuest_CheckNewDigivice(void* pData)
{
	SAFE_POINTER_RET( pData );
	DWORD dwQuestID = *static_cast<DWORD*>(pData);
	ContentsStream kTmp;
	kTmp << dwQuestID;
	Notify( eTutorialQuestDigiviceCheck, kTmp );	
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 중인가?
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_IsTutorialPlaying(void* pData)
{
	SAFE_POINTER_RET( pData );
	bool * pIsTutorialPlaying = static_cast<bool*>(pData);
	*pIsTutorialPlaying = IsTutorialPlaying();
}

//////////////////////////////////////////////////////////////////////////
// 수행 할 수 있는 튜토리얼 퀘스트가 있다고 이벤트 호출
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::HaveAcceptableQuest(DWORD const& dwQuestID)
{
	ContentsStream kTmpe;
	kTmpe << dwQuestID;
	Notify( eTutorialQuestAccpetAble, kTmpe );
}

//////////////////////////////////////////////////////////////////////////
// 현재 진행 중인 튜토리얼 퀘스트의 완료 조건이 모두 완료 됬을 때 이벤트 호출
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::ProcessConditionAllComplete( DWORD const& dwQuestID )
{
	ContentsStream kTmp;
	kTmp << dwQuestID;
	Notify( eProccesQuestAllComplete, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트의 완료 조건 상태 갱신
//////////////////////////////////////////////////////////////////////////
bool CTutorialContents::TutorialQuestConditionUpdate( DWORD const& dwQuestID, int const& nConditionIdx, int const& nValue )
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Quest* pQuestInfo = g_pDataMng->GetQuest();
	SAFE_POINTER_RETVAL( pQuestInfo, false );
	
	pQuestInfo->UpdateQuestCondition( dwQuestID, nConditionIdx, nValue );

	// 서버에 통신
	if( net::game )
		net::game->SendQuestChangeInfo( dwQuestID, nConditionIdx, nValue );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 가이드 윈도우를 띄우는 함수
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::ShowTooltipImage( std::string const& fileName )
{
	if( fileName.empty() )
	{
		if( g_pGameIF )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL );
		return;
	}

	if( g_pGameIF )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_TUTORIAL );

	ContentsStream kTmp;
	kTmp << fileName;
	Notify( eChangeShowImage, kTmp );
}

void CTutorialContents::SetUITraceData(sTraceData const* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( NULL == pData )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL_TRACE );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL );
		return;
	}

	sTraceData newData = *pData;

	if( newData.m_TooltipImgFile.empty() )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL );		
	else
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_TUTORIAL );
		
		ContentsStream kTmp;
		kTmp << newData.m_TooltipImgFile;
		Notify( eChangeShowImage, kTmp );
	}

	LIST_EFFECT_DATA_CIT it = newData.m_EffectData.begin();
	for( ; it != newData.m_EffectData.end(); ++it )
		SetObjectHighlight( (*it).cTargetType, (*it).dwTargetIDX, (*it).bShow );

	cBaseWindow* pWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_TUTORIAL_TRACE );
	if( pWin )
	{
		pWin->SetAlwaysTop( true );
		pWin->SetHaveNotFocus(true);
	}

	LIST_MONSTER_DATA_IT itMon = newData.m_MonsterData.begin();
	for( ; itMon != newData.m_MonsterData.end(); ++itMon )
		SetTutorialMonster( itMon->dwMonsterID, itMon->ptMonsterPos );

	// Set talk balloon data
	if( !newData.m_wsTalkMsg.empty() )
	{
		switch( newData.m_nTalkTarget )
		{
		case sTraceData::eDigimon:
			{
				CDigimonUser* pDUser = g_pCharMng->GetDigimonUser(0);
				if( pDUser && g_pTalkBallone )
					g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_3, pDUser->GetUniqID(), newData.m_wsTalkMsg.c_str() );
			}
			break;
		case sTraceData::eTamer:
			{
				CTamerUser* pTUser = g_pCharMng->GetTamerUser();
				if( pTUser && g_pTalkBallone )
					g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_3, pTUser->GetUniqID(), newData.m_wsTalkMsg.c_str() );
			}
			break;
		}
	}

	// Play video - 키가 눌릴 때까지 대기함
	if( !newData.m_VideoFile.empty() )
	{
		std::wstring wsFile;
		wsFile.assign( newData.m_VideoFile.begin(), newData.m_VideoFile.end() );
		_TutorialEvent_PlayVideo( wsFile );
	}

	// Set portal data
	if( 0 != newData.m_dwPortalID )
	{
		CsGBObject* pPortal = nsCsGBTerrain::g_pCurRoot->GetPortal( newData.m_dwPortalID );
		if( pPortal )
			pPortal->SetEnableUse( newData.m_bPortalEnable );
		if( newData.m_bPortalEnable )
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PORTAL_MOVE, &newData.m_dwPortalID );
	}

	ContentsStream kTmp;
	kTmp << newData.m_wsTargetWindowName << newData.m_nType << newData.m_wsTargetName << newData.m_dwTargetValue << newData.m_dwTargetValueSub;
	Notify( eUpdateUITrace, kTmp );

	// Move to next indicator
	ContentsStream kTmp2;
	kTmp2 << newData.m_dwNextQuestID << newData.m_nNextStep << newData.m_nNextTraceIdx;
	Notify( eUpdateQuestIndicatorInfo, kTmp2 );
}
//////////////////////////////////////////////////////////////////////////
// 튜토리얼 퀘스트 가이드 윈도우를 닫는 함수
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::CloseTooltipImage()
{
	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL );
}

void CTutorialContents::CloseUITrace()
{
	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL_TRACE );
}

void CTutorialContents::SetObjectHighlight( int const& nTargetType, DWORD const& dwTargetIDX, bool bShow )
{
	switch( nTargetType )
	{
	case sObjectEffectData::eNpc:		
		if( g_pNpcMng )
			g_pNpcMng->SetNpcSE_Highlight(dwTargetIDX, bShow);		
		break;
	case sObjectEffectData::eMonster:	
		if( g_pCharMng )
			g_pCharMng->SetMonsterSE_Highlight_TableID( dwTargetIDX, bShow );	
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CTutorialContents::_ClearTutorialQuestData()
{
	m_bTutorialQuestCompleted = true;
	_RemoveTutorialQuestEvet();
	m_pQuestIndicatorInfo = 0;
	m_pTutorialQuest = 0;
}

//////////////////////////////////////////////////////////////////////////
// 튜토리얼 이벤트 데이터 초기화
void CTutorialContents::_TutorialEvent_Initialize(void* pData)
{
	// 튜토리얼 맵이 아니면
	if( net::next_map_no != 4 )
		return;

	if( !m_sPlayerData.LoadPlayerData() )
		return;

	SAFE_POINTER_RET( g_pMngCollector );
	g_pMngCollector->SetSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT );

	SAFE_POINTER_RET( g_pDataMng );
	cData_PostLoad* pPostData = g_pDataMng->GetPostLoad();
	SAFE_POINTER_RET( pPostData );
	pPostData->SetDigimonData( m_sPlayerData.s_nDigimonFTID, m_sPlayerData.s_nBaseDigimonID, m_sPlayerData.s_wsDigimonName );
	pPostData->SetTamerData( m_sPlayerData.s_nTamerCostumeID );

	// disable digimon tip
	g_pResist->m_Global.s_bTipBalloone = false;

	// GameIF - Hide UI
	// 전부 hide 하고 필요한 ui 만 show ?
	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->HideWindow();
	g_pGameIF->HideWindowRestore( cBaseWindow::WT_TAMER_WINDOW );
	g_pGameIF->HideWindowRestore( cBaseWindow::WT_MAIN_DIGIMON_WINDOW );
	g_pGameIF->HideWindowRestore( cBaseWindow::WT_TARGET_WINDOW );
	for( int i = 0 ; i < IF_MAX_QUICKSLOT_LINE_COUNT; ++i )
		g_pGameIF->HideWindowRestore( cBaseWindow::WT_QUICKSLOT, i );
	g_pGameIF->HideWindowRestore( cBaseWindow::WT_QUICKEVOL );

#if !defined( _GIVE ) || defined( VERSION_QA )
	g_pGameIF->HideWindowRestore( cBaseWindow::WT_CHAT_MAIN_WINDOW );
#endif

	cMiniMap* pMiniMap = g_pGameIF->GetMiniMap();
	if (pMiniMap)
		pMiniMap->LoadMiniMap();

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SKIP_TUTORIAL_EVENT, this, &CTutorialContents::_TutorialEvent_Skip );
}

void CTutorialContents::_TutorialEvent_ClearData()
{
	if( net::next_map_no != 4 )
	{
		if( g_pMngCollector )
			g_pMngCollector->SetSceneState( CMngCollector::eSCENE_NONE );
	}

	m_sPlayerData.Reset();

	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->HideWindowRestore();

	for( int i = 0; i < IF_MAX_QUICKSLOT_LINE_COUNT; ++i )
	{
		cQuickSlot* pQuickSlot = g_pGameIF->GetQuickSlot( i );
		SAFE_POINTER_CON( pQuickSlot );
		pQuickSlot->UnlockSlotAll();
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TUTORIAL_HELP_UI ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL_HELP_UI );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TUTORIAL_SKIP_UI ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL_SKIP_UI );
}

void CTutorialContents::_TutorialEvent_PlayVideo(std::wstring const& wsPath)
{
	SAFE_POINTER_RET(g_pRenderer);

	g_pRenderer->SetBackgroundColor( NiColorA( 16.0f/255.0f, 16.0f/255.0f, 16.0f/255.0f, 1 ) );
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();

	if( nsDIRECTSHOW::InitDShowWindow( const_cast<TCHAR*>(wsPath.c_str()), CsPoint::ZERO, CsPoint::ZERO, true ) == true )
		nsDIRECTSHOW::WaitForKey_ForWindow( NiInputKeyboard::KEY_RETURN, NiInputKeyboard::KMOD_NONE );
		//nsDIRECTSHOW::WaitForKey_ForWindow_AniKey();
	g_pRenderer->SetBackgroundColor( NiColor::BLACK );
}

void CTutorialContents::_TutorialEvent_Skip(void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TUTORIAL_SKIP_UI );
}

void CTutorialContents::_CheckAndOpenDoor(void* pData)
{
	if (net::next_map_no != 105)
		return;

	// 튜토리얼을 전부 완료했으면
	if (!m_bTutorialQuestCompleted)
		return;

	SAFE_POINTER_RET(g_pDataMng);
	cData_MapObject* pMapObj = g_pDataMng->GetMapObject();
	SAFE_POINTER_RET(pMapObj);
	// 맵 내에 닫힌 문 모두 연다 - 마을 진입하는 문 열어줘야 함
	pMapObj->Init(true);
}

void CTutorialContents::_LoadTutorialQuest()
{
	m_pQuestIndicatorInfo = new cIndicatorInfos;
	SAFE_POINTER_RET(m_pQuestIndicatorInfo);
	m_pQuestIndicatorInfo->Construct();
	m_pQuestIndicatorInfo->LoadTutorialScript();

	std::list< DWORD > lQuestList;
	m_pQuestIndicatorInfo->GetTutorialQuestList(lQuestList);

	m_pTutorialQuest = new cTutorialQuest;
	SAFE_POINTER_RET(m_pTutorialQuest);
	// 진행중인 튜토리얼 퀘스트가 있는지 체크 및 수행 가능한 퀘스트 체크
	m_pTutorialQuest->Construct();
	// 현재 사용 중인 튜토리얼 퀘스트만 로드
	m_pTutorialQuest->LoadTutorialQuest(lQuestList);
}

bool CTutorialContents::_CheckCurTutorialQuestComplete()
{
	SAFE_POINTER_RETVAL(g_pDataMng, true);
	cData_Quest* pQuestInfo = g_pDataMng->GetQuest();
	SAFE_POINTER_RETVAL(pQuestInfo, true);
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pQuestMng, true);
	SAFE_POINTER_RETVAL(m_pQuestIndicatorInfo, true);

	std::list<DWORD> curTutorialQuest;
	m_pQuestIndicatorInfo->GetTutorialQuestList(curTutorialQuest);

	std::list<DWORD>::iterator it = curTutorialQuest.begin();
	for (; it != curTutorialQuest.end(); ++it)
	{
		if (!pQuestInfo->IsCompleate(*it))
			return false;
	}

	return true;
}

bool CTutorialContents::_CheckPreTutorialQuestComplete()
{
	SAFE_POINTER_RETVAL(g_pDataMng, true);
	cData_Quest* pQuestInfo = g_pDataMng->GetQuest();
	SAFE_POINTER_RETVAL(pQuestInfo, true);
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pQuestMng, true);
	SAFE_POINTER_RETVAL(m_pQuestIndicatorInfo, true);

	std::list<DWORD> tutorialQuest;
	nsCsFileTable::g_pQuestMng->GetQuestListFormType(CsQuest::QT_TUTORIAL, tutorialQuest);

	std::list<DWORD> curTutorialQuest;
	m_pQuestIndicatorInfo->GetTutorialQuestList(curTutorialQuest);

	// 현재 튜토리얼 퀘스트를 제거 - 예전 튜토리얼 퀘스트만 남긴다
	std::list<DWORD>::iterator it = curTutorialQuest.begin();
	for (; it != curTutorialQuest.end(); ++it)
		tutorialQuest.remove(*it);

	// 예전 튜토리얼 퀘스트 완료했는지 확인
	std::list<DWORD>::iterator itCheck = tutorialQuest.begin();
	for (; itCheck != tutorialQuest.end(); ++itCheck)
	{
		if (!pQuestInfo->IsCompleate(*itCheck))
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CTutorialContents::IsTutorialPlaying_ClientMode() const
{
	if( m_bTutorialQuestCompleted )
		return false;

	if( net::next_map_no == 4 )
		return true;

	return false;
}

void CTutorialContents::SetTutorialMonster(DWORD const& dwID, CsPoint const& ptPos)
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pTUser);

	int nMonsterIDX = g_pCharMng->GetMonsterTempID();

	// 테이머 바라보는 방향으로 회전
	NiPoint3 vPos(ptPos.x, ptPos.y, 0.0f);
	NiPoint3 vDstPos(pTUser->GetPos().x, pTUser->GetPos().y, 0.0f);
	NiPoint3 vDstDir = vDstPos - vPos;
	vDstDir.Unitize();
	float fRot = NiACos(NiPoint3(0.0f, -1.0f, 0.0f).Dot(vDstDir));
	if (NiPoint3(0.0f, -1.0f, 0.0f).Cross(vDstDir).z > 0)
		fRot = -fRot;

	CMonster* pTutoMon = g_pCharMng->AddMonster_Tutorial(
		nMonsterIDX,
		dwID,
		nSync::Pos( ptPos.x, ptPos.y ),
		fRot,
		CMonster::MONSTER_NORMAL );
}

void CTutorialContents::SkipTutorialEvent()
{
	// Tutorial Skip Message
	cPrintMsg::PrintMsg( cPrintMsg::TUTORIAL_EVENT_SKIP );
}

//////////////////////////////////////////////////////////////////////////
bool CTutorialContents::sTutorialPlayerData::LoadPlayerData()
{
	Json::Value root;
	Json::Reader reader;

	if( CsFPS::CsFPSystem::IsExistOnlyPack( 0, "Data\\Interface\\Tutorial\\TutorialPlayerData.Json" ) )
	{
		char* buffer = NULL;
		size_t size = CsFPS::CsFPSystem::Allocate_GetFileData( 0, &buffer, "Data\\Interface\\Tutorial\\TutorialPlayerData.Json" );
		if( 0 == size )
			return false;

		bool bSuccess = reader.parse( std::string( buffer ), root );
		if( !bSuccess )
			return false;
	}
	else
	{
		std::ifstream file( "Data\\Interface\\Tutorial\\TutorialPlayerData.Json" );
		if( !reader.parse( file, root ) )
			return false;
	}

	if( !root.isMember( "PlayerData" ) )
		return false;

	Json::Value const& jData = root.get( "PlayerData", "" );
	std::string strName = jData.get( "DigimonName", "OmegamonX" ).asString();
	DmCS::StringFn::FromUTF8( s_wsDigimonName, strName.c_str() );

	s_nDigimonFTID = jData.get( "DigimonFTID", 75902 ).asUInt();
	s_nBaseDigimonID = jData.get( "BaseDigimonID", 31007 ).asUInt();
	s_nTamerCostumeID = jData.get( "TamerCostumeID", 17001 ).asUInt();

	return true;
}

void CTutorialContents::sTutorialPlayerData::Reset()
{
	s_wsDigimonName.clear();
	s_nDigimonFTID = 0;
	s_nBaseDigimonID = 0;
	s_nTamerCostumeID = 0;
}