#include "StdAfx.h"
#include "RecommendEvent_Contents.h"
#include "ContentsSystem/ContentsSystemDef.h"
#include "common_vs2019/pEvent.h"

CRecommendEvent_Contents::CRecommendEvent_Contents(void) :
	m_nRecommendState( false ),
	m_bRecommendEvent( false )
{
	m_RecommendInfoList.clear();
	m_mapRecommendTableData.clear();
	m_strSelectedTamerName.clear();

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_RECOMMEND_EVENT_INFO,		this, &CRecommendEvent_Contents::RecvRecommedEventInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_FRIENDLY_CHANGE_NAME,		this, &CRecommendEvent_Contents::RecvRecommedChangeTamerName );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_FRIENDLY_ADD_INFO,			this, &CRecommendEvent_Contents::RecvFriendlyAddInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_FRIENDLY_MARK_INFO,		this, &CRecommendEvent_Contents::RecvFriendlyMarkInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_FRIENDLY_CHANGESTATE_INFO, this, &CRecommendEvent_Contents::RecvFriendlyChangeStateInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_FRIENDLY_ADD_ANSWER_INFO, this, &CRecommendEvent_Contents::RecvFriendlyAddAnswerInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_RECOMMEND_ADD_ANSWER_INFO,	this, &CRecommendEvent_Contents::SendRecommendAddAnswerInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_FRIENDLY_MARKITEM_INFO, this, &CRecommendEvent_Contents::RecvFriendlyMarkItemInfo );
}

CRecommendEvent_Contents::~CRecommendEvent_Contents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const CRecommendEvent_Contents::IsContentsIdentity( void )
{
	return E_CT_FRIENDRECOMMEND_EVENT;
}

int const CRecommendEvent_Contents::GetContentsIdentity( void ) const
{
	return	IsContentsIdentity();
}

bool CRecommendEvent_Contents::Initialize( void )
{
	return true;
}

void CRecommendEvent_Contents::UnInitialize( void )
{

}

bool CRecommendEvent_Contents::IntraConnection( ContentsSystem& System )
{
	return true;
}

void CRecommendEvent_Contents::Update( float const& fElapsedTime )
{

}

void CRecommendEvent_Contents::NotifyEvent( int const& iNotifiedEvt )
{

}

void CRecommendEvent_Contents::NotifyEventStream( int const& iNotifiedEvt, ContentsStream const& kStream )
{

}

void CRecommendEvent_Contents::MakeMainActorData( void )
{

}

void CRecommendEvent_Contents::ClearMainActorData( void )
{
	cCreateName::DeleteInstance();	// 이름 생성창이 오픈되어 있다면 제거

}

void CRecommendEvent_Contents::MakeWorldData( void )
{
	SAFE_POINTER_RET( net::game )
	net::game->SendFriendInfo(); // 이벤트 정보를 요청한다.
}

void CRecommendEvent_Contents::ClearWorldData( void )
{
	ClearAllData(); // 데이터 초기화
}

void CRecommendEvent_Contents::ClearAllData()
{
	cCreateName::DeleteInstance();

	m_nRecommendState = 0;
	m_bRecommendEvent = false;
	m_strSelectedTamerName.clear();

	m_RecommendInfoList.clear();
	m_mapRecommendTableData.clear();
}

void CRecommendEvent_Contents::Init()
{

}

void CRecommendEvent_Contents::AddRecommendInfo( const TCHAR* const szName, int nType, u1 nMark[nLimit::MAX_FRIENDLY_MARK] )
{
	ST_RecommendInfo RecommendInfo;
	RecommendInfo.m_strTamerName = szName;
	RecommendInfo.m_nRecommendType = nType;
	memcpy(RecommendInfo.AchieveState, nMark, sizeof( u1 ) * nLimit::MAX_FRIENDLY_MARK );

	m_RecommendInfoList.push_back( RecommendInfo );
}

void CRecommendEvent_Contents::RecvRecommedEventInfo( void* pData ) /* RECV_RECOMMEND_EVENT_INFO */
{
	SAFE_POINTER_RET( pData );
	ST_RECV_RecommendEvent stRecommendEvent = *static_cast< ST_RECV_RecommendEvent* >( pData );

	if( stRecommendEvent.m_nResult == 1 ) // 추천 이벤트 진행중이다
	{
		std::list< ST_RECV_RecommendEvent::sInfo >::iterator iter = stRecommendEvent.m_EventInfoList.begin();
		for( ; iter != stRecommendEvent.m_EventInfoList.end(); ++iter )
			AddRecommendInfo( iter->m_szTamerName.data(), iter->m_nFriendlyType, iter->m_nFriendlyMark );

		InitRecommendTableData();			// 데이터 테이블 설정
		SetRecommendEventState( true );		// 이벤트 기간인지 아닌지 상태값 설정
		SetMyRecommendState( stRecommendEvent.m_nMyRecommendState ); // 내가 추천할 수 있는 상태인지
		CreateRecommendEventButton();		// 이벤트 버튼 생성

	}
	else // 이벤트 기간이 아니다
		SetRecommendEventState( false );
}
// 누군가 나를 친구 추천 했을때
void CRecommendEvent_Contents::RecvFriendlyAddInfo( void* pData ) /* RECV_FRIENDLY_ADD_INFO */
{
	SAFE_POINTER_RET( pData );
	TCHAR* pTamerName = static_cast< TCHAR* >( pData );
	cPrintMsg::PrintMsg( cPrintMsg::RECOMMENT_ANSWER_CONFIRM, pTamerName );
}
// 내가 보낸 추천의 결과를 받는 이벤트
void CRecommendEvent_Contents::RecvFriendlyAddAnswerInfo( void* pData ) /* RECV_FRIENDLY_ADD_ANSWER_INFO */
{
	SAFE_POINTER_RET( pData );
	ST_RECV_RecommendResultInfo RecommendResultInfo = *static_cast<ST_RECV_RecommendResultInfo*>( pData );

	u1 nMark[ nLimit::MAX_FRIENDLY_MARK ];
	memset( nMark, 0, sizeof(u1) * nLimit::MAX_FRIENDLY_MARK );

	TCHAR* szTamerName = const_cast<TCHAR*>( RecommendResultInfo.m_strTamerName.data() );

	switch( RecommendResultInfo.m_nAnswer )
	{
	case nEvent::FRIENDLY_NOT_FOUND:			// 해당 테이머 없음
		cPrintMsg::PrintMsg( 30491, szTamerName );
		break;
	case nEvent::FRIENDLY_REJECT:				// 해당 테이머가 거부함
		cPrintMsg::PrintMsg( 30495, szTamerName );
		break;
	case nEvent::FRIENDLY_FULL:				// 해당 테이머는 더이상 추천을 받을 수 없음
		cPrintMsg::PrintMsg( 30493, szTamerName );
		break;
	case nEvent::FRIENDLY_ALREADY:				// 이미 다른 사람을 추천하였기 때문에 더이상 추천할 수 없음
		cPrintMsg::PrintMsg( 30068, szTamerName );
		break;
	case nEvent::FRIENDLY_DUPLICATE:			// 이미 추천한 대상임 (중복 추천할 수 없음)
		cPrintMsg::PrintMsg( 30496, szTamerName );
		break;
	case nEvent::FRIENDLY_OFFILE:				// 추천 대상이 오프라인 상태임
		cPrintMsg::PrintMsg( 30491, szTamerName );
		break;
	case nEvent::FRIENDLY_NOT_USE:			// 시스템상 이벤트를 사용할 수 없는 상태임
		cPrintMsg::PrintMsg( 30068, szTamerName );
		break;
	case nEvent::FRIENDLY_NOT_EVENT_DATE:		// 친구 초대 이벤트 기간이 아닙니다.
		cPrintMsg::PrintMsg( 30068, szTamerName );
		break;
	case nEvent::FRIENDLY_TARGET_LEVEL_LOW:	// 친구 초대 대상 레벨이 필요 레벨보다 낮습니다.
		cPrintMsg::PrintMsg( 30498, szTamerName );
		break;
	case nEvent::FRIENDLY_ANOTHER_ACTION:	// 친구 초대 대상이 다른 행동중이여서 응답을 할 수 없습니다.
		cPrintMsg::PrintMsg( 30495, szTamerName );
		break;
	case nEvent::FRIENDLY_NAME_DUPLICATE	:	// 이미 해당 이름으로 초대된 대상이 있습니다.
		cPrintMsg::PrintMsg( 30496, szTamerName );
		break;
		// 실패 관련 메시지 처리 필요
	case nEvent::FRIENDLY_SUCCESS:		// 추천 성공
		{
			if( RecommendResultInfo.m_nRecommendType == Recommend::Type::Sent ||
				RecommendResultInfo.m_nRecommendType == Recommend::Type::Received )
			{	
				cPrintMsg::PrintMsg( 30494, szTamerName );
				AddRecommendInfo( szTamerName, RecommendResultInfo.m_nRecommendType, nMark);
				CreateRecommendEventWindow();
			}
			else
			{	// 이곳으로 들어온 경우 에러로 개발 완료 전까지는 확인이 필요 합니다.
				// asser 나 popup같은 창을 띄워 주시면 감사하겠습니다.
				assert_cs( false );
				return;
			}
			SetMyRecommendState( Recommend::MyState::Impossible );
		}
		break;
	default :
		xstop1(false, "친구 초대 관련 오류 nAnswer:%d", RecommendResultInfo.m_nAnswer);
	}
}

// 친구가 목표 달성했을때 정보
void CRecommendEvent_Contents::RecvFriendlyMarkInfo( void* pData ) /* RECV_FRIENDLY_MARK_INFO */
{
	SAFE_POINTER_RET( pData );
	ST_RECV_PurposeCompleteInfo PurposeCompleteInfo = *static_cast<ST_RECV_PurposeCompleteInfo*>( pData );

	//80001
	if(PurposeCompleteInfo.m_nRecommendType == 1)//내가 레벨을 달성하고 보상을 받을때
	{
		cPrintMsg::PrintMsg( 30806 );
	}
	else
	{
		//m_nMarkIndex 는 1 부터 시작합니다.
		if(PurposeCompleteInfo.m_nMarkIndex > 0)
		{
			TCHAR szOldVal[256] = {0, };
			_stprintf_s( szOldVal, 256, _T( "%s" ), PurposeCompleteInfo.m_strTamerName.c_str() );
			cPrintMsg::PrintMsg( 30806 + PurposeCompleteInfo.m_nMarkIndex, szOldVal );
		}
	}

	ChangePurposeInfo( Recommend::MarkValue::Completed, PurposeCompleteInfo.m_nRecommendType, PurposeCompleteInfo.m_nMarkIndex, PurposeCompleteInfo.m_strTamerName );
}

// 보상아이템 획득 요청 결과 정보
void CRecommendEvent_Contents::RecvFriendlyMarkItemInfo( void* pData ) /* RECV_FRIENDLY_MARKITEM_INFO */
{
	SAFE_POINTER_RET( pData );
	ST_RECV_RewardItemInfo RewardItemInfo = *static_cast<ST_RECV_RewardItemInfo*>( pData );

	// 현재까진 성공 외에 리턴값 없음
	switch( RewardItemInfo.m_nResult )
	{
	case nEvent::FRIENDLY_GET_SUCCESS:	// 성공
		{
			ChangePurposeInfo( Recommend::MarkValue::ReceivedReward, RewardItemInfo.m_nRecommendType, RewardItemInfo.m_nMarkIndex, RewardItemInfo.m_strTamerName );
		}
		break;
	case nEvent::FRIENDLY_NOT_EVENT_DATE:	// 이벤트 기간이 아닙니다.]
		cPrintMsg::PrintMsg( 30497, const_cast< TCHAR* >(RewardItemInfo.m_strTamerName.data()) );
		return;
	default :	// 이곳으로 들어오는 경우가 있는지 popup & assert 띄워 주시면 감사하겠습니다. 
		{
			cPrintMsg::PrintMsg( 30491, const_cast< TCHAR* >(RewardItemInfo.m_strTamerName.data()) );
			assert_cs( false );
		}
		return;
	}
}

// 친구 초대 이벤트 정보가 바꼈을때
void CRecommendEvent_Contents::RecvFriendlyChangeStateInfo( void* pData ) /* RECV_FRIENDLY_CHANGESTATE_INFO */
{
	u1 nState = *static_cast< u1* >( pData );

	switch( nState )
	{
	case nEvent::Friendly_NONE:
		SetMyRecommendState( nState );
		ClearAllData();
		DeleteRecommendEventButton();
		break;
	default:
		// 이곳에 들어오는 경우가 있는지 assert 띄워주세요.
		return;
	}
}

// 이벤트 윈도우를 생성한다. 추천 가능한 상태이면 추천 테이머명을 입력할수 있는 창을 생성한다
void CRecommendEvent_Contents::OpenRecommendEventWindow()
{
	if( IsPossibleRecommend() )
	{
		CreateRecommendNameInputWindow();
	}

	CreateRecommendEventWindow();
}
// 보상 받을 아이템 정보를 서버에 보낸다
void CRecommendEvent_Contents::SendMarkItemInfo( int nPurposeIndex )
{
	if( nPurposeIndex <= 0 || nPurposeIndex >= nLimit::MAX_FRIENDLY_MARK )
		return;

	ST_RecommendInfo RecommendInfo;
	if( GetRecommendInfo( RecommendInfo, m_strSelectedTamerName ) == false )
		return;

	SAFE_POINTER_RET( net::game );
	net::game->SendMarkItem( RecommendInfo.m_nRecommendType, nPurposeIndex, RecommendInfo.m_strTamerName.c_str() );
}

// 타인이 나를 추천했을때 나의 응답을 서버에 보낸다
void CRecommendEvent_Contents::SendRecommendAddAnswerInfo( void* pData )	/* SEND_RECOMMEND_ADD_ANSWER_INFO */
{
	cMessageBox* pBox = cMessageBox::GetMessageBox( cPrintMsg::RECOMMENT_ANSWER_CONFIRM );
	SAFE_POINTER_RET( pBox );
	cMessageBox::eSTATE eState = pBox->GetState();
	SAFE_POINTER_RET( net::game );

	std::wstring szRecommendName( net::game->m_szTamerName );
	if( szRecommendName.empty() )
		return;

	switch( eState )
	{
	case cMessageBox::MBOX_NONE: // 생성시
		{
			net::game->SendAddFriendlyAnswer( nEvent::ANSWER_REJECT, szRecommendName.c_str() );
		}
		break;
	case cMessageBox::MBOX_CALL_OK: // ok 버튼 눌렀을때
		{
			SetMyRecommendState( Recommend::MyState::Impossible );
			net::game->SendAddFriendlyAnswer( nEvent::ANSWER_OK, szRecommendName.c_str() );
		}
		break;
	case cMessageBox::MBOX_CALL_CANCEL: // 취소 버튼
		{
			SetMyRecommendState( Recommend::MyState::Impossible );
			net::game->SendAddFriendlyAnswer( nEvent::ANSWER_REJECT, szRecommendName.c_str() );
		}
		break;
	}
}

void CRecommendEvent_Contents::RecvRecommedChangeTamerName( void* pData )
{
	SAFE_POINTER_RET( pData );
	ContentsStream kStream = *static_cast< ContentsStream * >( pData );

	std::wstring strBefoleName;
	std::wstring strNewName;
	n1	nType;
	kStream >> nType >> strBefoleName >> strNewName;

	ChangeRecommendTamerName( nType, strBefoleName, strNewName );
}

bool CRecommendEvent_Contents::ChangeRecommendTamerName( n1 nType, const std::wstring& BefoleName, const std::wstring& NewName )
{
	std::list< ST_RecommendInfo >::iterator iter	 = m_RecommendInfoList.begin();
	for( ; iter != m_RecommendInfoList.end(); ++iter)
	{
		ST_RecommendInfo& RecommendInfo = (*iter);
		if(	(RecommendInfo.m_nRecommendType == nType) && (RecommendInfo.m_strTamerName == BefoleName) )
		{
			RecommendInfo.SetTamerName( NewName );
			ContentsStream kStream;
			kStream << BefoleName << NewName;
			Notify( eChange_TamerName, kStream );

			if( m_strSelectedTamerName.compare( BefoleName ) == 0 )
				SetSelectedTamerName( NewName );

			return true;
		}
	}
	return false;
}

void CRecommendEvent_Contents::SetMyRecommendState( const int nState )
{
	m_nRecommendState = nState;
}

const bool CRecommendEvent_Contents::IsPossibleRecommend() const
{
	if( m_nRecommendState == Recommend::MyState::FirstProduce || m_nRecommendState == Recommend::MyState::Possilble )
		return true;

	return false;
}

void CRecommendEvent_Contents::SetRecommendEventState( const bool bState )
{
	m_bRecommendEvent = bState;
}

const bool CRecommendEvent_Contents::IsRecommendEvent() const
{
	return m_bRecommendEvent;
}
// 현재 선택된 테이머명
void CRecommendEvent_Contents::SetSelectedTamerName( const std::wstring TamerName )
{
	m_strSelectedTamerName = TamerName;
}

void CRecommendEvent_Contents::ChangePurposeInfo( const int nMarkValue, const int nFriendlyType, const int nMarkIndex, const std::wstring& szTamerName )
{
	std::list< ST_RecommendInfo >::iterator iter = m_RecommendInfoList.begin();
	for( ; iter != m_RecommendInfoList.end(); ++iter)
	{
		ST_RecommendInfo& info = (*iter);
		if( info.m_nRecommendType == nFriendlyType && info.m_strTamerName.compare( szTamerName ) == 0 )
		{
			info.SetMarkValue( nMarkIndex, nMarkValue );

			ContentsStream kStream;
			kStream << nMarkIndex << nMarkValue << info.m_nRecommendType << info.m_strTamerName;
			Notify( eChange_PurposeState, kStream );
			return;
		}
	}
}

const std::list< CRecommendEvent_Contents::ST_RecommendInfo >& CRecommendEvent_Contents::GetRecommendInfoList() const
{
	return m_RecommendInfoList;
}

bool CRecommendEvent_Contents::GetRecommendInfo( CRecommendEvent_Contents::ST_RecommendInfo& rInfo, std::wstring strTamerName )
{
	std::list< ST_RecommendInfo >::const_iterator iter = m_RecommendInfoList.begin();
	for( ; iter != m_RecommendInfoList.end(); ++iter)
	{
		ST_RecommendInfo RecommendInfo = (*iter);
		if(	RecommendInfo.m_strTamerName.compare( strTamerName ) == 0 )
		{
			rInfo = RecommendInfo;
			return true;
		}
	}
	return false;
}
// 추천 이벤트 관련 테이블 데이터 설정
void CRecommendEvent_Contents::InitRecommendTableData()
{
	m_mapRecommendTableData.clear();

	std::list< ST_RecommendTableData > MyRecommendTableData;
	std::list< ST_RecommendTableData > OthersRecommendTableData;

	for( int i = 0; i < nsCsFileTable::g_pEventMng->GetRecommendTableCount(); ++i )
	{
		CsEventTable::sRECOMMENDE* pTableData  = nsCsFileTable::g_pEventMng->GetMap(i);
		SAFE_POINTER_RET( pTableData );

		ST_RecommendTableData TableData;
		TableData.m_nIndex			=	pTableData->s_nIndex;
		TableData.m_nType			=	pTableData->s_nType;
		TableData.m_nSuccessType	=	pTableData->s_nSuccessType;
		TableData.m_nSuccessValue	=	pTableData->s_nSuccessValue;

		if( i % 2 == 0)
			MyRecommendTableData.push_back( TableData );
		else
			OthersRecommendTableData.push_back( TableData );
	}

	m_mapRecommendTableData.insert( make_pair( Recommend::Type::Sent, MyRecommendTableData ) );
	m_mapRecommendTableData.insert( make_pair( Recommend::Type::Received, OthersRecommendTableData ) );
}

bool CRecommendEvent_Contents::GetRecommendTableData( CRecommendEvent_Contents::ST_RecommendTableData& rTableData, int nRecommendType, int nMarkIndex )
{
	std::map< int, std::list< ST_RecommendTableData > >::const_iterator iter = m_mapRecommendTableData.find( nRecommendType );
	if( iter != m_mapRecommendTableData.end() )
	{
		std::list< ST_RecommendTableData >::const_iterator Tableiter = iter->second.begin();
		for( ; Tableiter != iter->second.end(); ++Tableiter )
		{
			if( Tableiter->m_nIndex == (nMarkIndex + 1) )
			{
				rTableData = (*Tableiter);
				return true;
			}
		}
	}
	return false;
}

bool CRecommendEvent_Contents::GetTableDataList( std::list< CRecommendEvent_Contents::ST_RecommendTableData >& rTableList, int nRecommendType )
{
	std::map< int, std::list< ST_RecommendTableData > >::const_iterator iter = m_mapRecommendTableData.find( nRecommendType );
	if( iter != m_mapRecommendTableData.end() )
	{
		rTableList = iter->second;
		return true;
	}
	return false;
}

// 추천 이벤트 버튼 생성
void CRecommendEvent_Contents::CreateRecommendEventButton()
{
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_RECOMMENDEVENT_BUTTON );
}

void CRecommendEvent_Contents::CreateRecommendEventWindow()
{
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_RECOMMENDEVENT );
}

void CRecommendEvent_Contents::CreateRecommendNameInputWindow()
{
	cCreateName* pCName = cCreateName::NewInstance( cBaseWindow::WT_RECOMMEND_NAME, 1, CsPoint( -1, -1) );

	if( pCName )
		pCName->SetValue1( 1 );
}

void CRecommendEvent_Contents::DeleteRecommendEventButton()
{
	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_RECOMMENDEVENT_BUTTON );
}

void CRecommendEvent_Contents::ST_RecommendInfo::SetTamerName( const std::wstring& strName )
{ 
	m_strTamerName = strName;
}

void CRecommendEvent_Contents::ST_RecommendInfo::SetMarkValue( int nIndex, int nState )
{
	if( nIndex < 0 || nIndex >= nLimit::MAX_FRIENDLY_MARK)
		return;

	AchieveState[ nIndex ] = nState;
}
