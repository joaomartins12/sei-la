#include "StdAfx.h"
#include "FriendListContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include <atlconv.h>

cFriendListContents::cFriendListContents(void) : m_bSendCheck(false), m_nCurSelectDataIdx(0), m_bOnOffCheck(false)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_FRIENDLIST, this, &cFriendListContents::ReceiveFriendList);			// 친구 목록 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SHUTOUTLIST, this, &cFriendListContents::ReceiveShutoutList);			// 차단 목록 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_FRIEND_ON_OFF, this, &cFriendListContents::ReceiveFriendOnOff);		// 친구 Login/LogOff
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHANGE_NAME_NEARBY, this, &cFriendListContents::ReceiveChangeNameNearby);	// 이름 변경 주변
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHANGE_NAME_FRIEND_SHUTOUT, this, &cFriendListContents::ReceiveChangeName);// 이름 변경
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ADD_FRIEND, this, &cFriendListContents::ReceiveAddFriendSucces);		// 친구 추가 성공
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ADD_SHUTOUT, this, &cFriendListContents::ReceiveAddShutoutSucces);		// 차단 추가 성공
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_NOT_FOUND, this, &cFriendListContents::ReceiveNotFoundTamer);			// 테이머 찾지 못함( 친구/차단 추가 삭제 모두 

	//GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_FRIENDLIST, this, &cFriendListContents::SendFriendList);				// 친구 목록 요청
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_FRIEND_SHUTOUT_MEMO, this, &cFriendListContents::SendFriendShutoutMemo);	// 친구/차단 메모 작성
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_ADD_FRIEND_SHUTOUT, this, &cFriendListContents::SendAddFriendShutout);	// 친구/차단 추가 패킷 보내기 및 전방체크

	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_FRIENDLIST, this, &cFriendListContents::GetFriendList);					// 친구 목록 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_SHUTOUTLIST, this, &cFriendListContents::GetShutoutList);				// 차단 목록 가져오기
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_CURRENT_LISTINDEX, this, &cFriendListContents::GetCurSelect_DataIndex);	// 현재 선택 중인 목록의 list index를 가져온다.

	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHECK_ACHIEVE_TYPE, this, &cFriendListContents::CheckAchieveType);			// 아카이브 타입 체크
	GAME_EVENT_ST.AddEvent( EVENT_CODE::DELETE_FRIEND_SHUTOUT, this, &cFriendListContents::DelFriendShutout);		// 친구/차단 삭제
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TOGGLE_FRIEND_SHUTOUT_UI, this, &cFriendListContents::ToggleUI);			// UI 토글 0 차단 탭이면 친구 탭으로 변경 토글, 1 단순 토글
}

cFriendListContents::~cFriendListContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}
int const cFriendListContents::IsContentsIdentity(void)
{
	return E_CT_FRIENDLIST;
}
int const cFriendListContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}
bool cFriendListContents::Initialize(void)
{
	return true;
}
void cFriendListContents::UnInitialize(void)
{
}
bool cFriendListContents::IntraConnection(ContentsSystem& System)
{
	return true; 
}
void cFriendListContents::Update(float const& fElapsedTime)
{
}
void cFriendListContents::NotifyEvent(int const& iNotifiedEvt)
{
	return;
}
void cFriendListContents::MakeMainActorData(void)
{
}
void cFriendListContents::ClearMainActorData(void)
{
}
void cFriendListContents::MakeWorldData(void)
{
	SendFriendList(NULL);
	return;
}
void cFriendListContents::ClearWorldData(void)
{
	InitListNSendCheck();
	return;
}
const cFriendListContents::FRIENDLIST& cFriendListContents::GetFriendList() const
{
	return m_FriendList;
}
const cFriendListContents::SHUTOUTLIST& cFriendListContents::GetShutoutList() const
{
	return m_ShutoutList;
}

void cFriendListContents::AddFriend( std::wstring* szName, int nOnline )
{
	FRIEND_INFO fi;
	fi.s_szName = *szName;
	fi.s_eOnlineState = (eONLINE_STATE)nOnline;	
	m_FriendList.push_back( fi );

	Notify(RenewTab_Friend);

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_FriendCount;
	recv.nValue1 = m_FriendList.size();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
}		

bool cFriendListContents::IsMyFriend( std::wstring const& wsTamerName ) const
{
	CFITR itrFriend = m_FriendList.begin();
	for( ; itrFriend != m_FriendList.end(); ++itrFriend )
	{
		if( (*itrFriend).s_szName.compare(wsTamerName) == 0 )
			return true;
	}	

	return false;
}

void cFriendListContents::FriendOnOff( std::wstring const* szName, eONLINE_STATE nOnline )
{
	// 이미 존재 한다면 리턴
	FITR itrFriend = m_FriendList.begin();
	for( int i=0; itrFriend != m_FriendList.end(); ++itrFriend, ++i )
	{
		if( (*itrFriend).s_szName.compare(*szName) == 0 )
		{			
			(*itrFriend).s_eOnlineState = nOnline;			
			ContentsStream kStream;
			kStream << i;
			Notify(Friend_OnOffline, kStream);
			if (nOnline == ONLINE)
			{
				cPrintMsg::PrintMsg( 30124, const_cast<TCHAR*>( (*itrFriend).s_szName.c_str() ) );
			} 
			else if (nOnline == OFFLINE)
			{
			}
			break;
		}
	}	
}	

const cFriendListContents::FRIEND_INFO*	cFriendListContents::GetFriendInfo( unsigned int nIndex )
{
	FITR fItr = m_FriendList.begin();
	for(int i = 0; fItr != m_FriendList.end(); ++fItr, ++i )
	{
		if( nIndex == i )
			return &(*fItr);
	}
	return NULL;
}
const cFriendListContents::FRIEND_INFO*	cFriendListContents::GetFriendInfo( const std::wstring* szName )
{
	for( FITR fItr = m_FriendList.begin(); fItr != m_FriendList.end(); ++fItr )
	{
		if( (*fItr).s_szName.compare(*szName) == 0 )
			return &(*fItr);
	}
	return null;
}
void cFriendListContents::SetOnOff( bool bCheck )
{
	m_bOnOffCheck = bCheck;
}
const bool cFriendListContents::GetOnOff()
{
	return m_bOnOffCheck;
}

//Shutout
void cFriendListContents::AddShutout( std::wstring const* szName, bool bOnline )
{
	SHUTOUT_INFO si;
	si.s_szName = *szName;
	m_ShutoutList.push_back( si );
	Notify(RenewTab_Shutout);
}	
bool cFriendListContents::IsShutout( std::wstring const* szName ) const
{
	CSITR itrShutout = m_ShutoutList.begin();
	for(; itrShutout != m_ShutoutList.end(); ++itrShutout )
	{
		if( (*itrShutout).s_szName.compare(*szName) == 0 )
		{	
			return true;
		}
	}
	return false;
}
const cFriendListContents::SHUTOUT_INFO*	cFriendListContents::GetShutInfo( unsigned int nIndex )
{
	CSITR itrShutout = m_ShutoutList.begin();
	for(int i = 0; itrShutout != m_ShutoutList.end(); ++itrShutout, ++i )
	{
		if( i == nIndex )
		{	
			return &(*itrShutout);
		}
	}
	return null;
}
const cFriendListContents::SHUTOUT_INFO*	cFriendListContents::GetShutInfo( const std::wstring* szName )
{

	SITR itrShutout = m_ShutoutList.begin();
	for(; itrShutout != m_ShutoutList.end(); ++itrShutout )
	{
		if( (*itrShutout).s_szName.compare(*szName) == 0 )
			return &(*itrShutout);
	}
	return null;
}
void cFriendListContents::ChangeTamerName( const std::wstring& BefoleName, const std::wstring& NewName )
{
	FRIEND_INFO*	pFriendInfo  = const_cast< FRIEND_INFO* >(GetFriendInfo( &BefoleName ));
	if( pFriendInfo != null )
	{
		pFriendInfo->s_szName = NewName;
		Notify(RenewTab_Friend);
	}

	SHUTOUT_INFO*	pShutOutInfo = const_cast< SHUTOUT_INFO* >( GetShutInfo( &BefoleName ) );
	if( pShutOutInfo != null )
	{
		pShutOutInfo->s_szName = NewName;
		Notify(RenewTab_Shutout);
	}
}

const int cFriendListContents::GetCurSelect_DataIndex()
{ 
	return m_nCurSelectDataIdx;
}
void cFriendListContents::SetCurSelect_DataIndex(int curSelect)
{
	m_nCurSelectDataIdx = curSelect;
}
//event 함수
void cFriendListContents::ReceiveFriendList(void* pData)
{
	SAFE_POINTER_RET(pData);

	GS2C_RECV_FRIENDLIST* pCastData = static_cast<GS2C_RECV_FRIENDLIST*>(pData);
	std::list<nsFriendInfo::sFriendInfo>::iterator fItr = pCastData->m_FriendList.begin();
	for (;fItr != pCastData->m_FriendList.end(); ++fItr)
	{
		FRIEND_INFO friendListElement;
		friendListElement.s_eOnlineState = static_cast<eONLINE_STATE>( (*fItr).s_eOnlineState );
		friendListElement.s_szName = (*fItr).s_szName;
		friendListElement.s_szMemo = (*fItr).s_szMemo;
		m_FriendList.push_back( friendListElement );
	}

	//cFriendListContents::FRIENDLIST::const_iterator fItrTest = m_FriendList.begin();
	//for(int i = 0; i < 20; ++fItrTest, ++i)
	//{
	//	if (fItrTest == m_FriendList.end())
	//	{
	//		fItrTest = m_FriendList.begin();
	//	}
	//	m_FriendList.push_back( *fItrTest );
	//}
}
void cFriendListContents::ReceiveShutoutList(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SHUTOUTLIST* pCastData = static_cast<GS2C_RECV_SHUTOUTLIST*>(pData);
	std::list<nsShutout::sShutoutInfo>::iterator sItr = pCastData->m_ShoutoutList.begin();
	for (;sItr != pCastData->m_ShoutoutList.end(); ++sItr)
	{
		SHUTOUT_INFO shutoutListElement;
		shutoutListElement.s_szName = (*sItr).s_szName;
		shutoutListElement.s_szMemo = (*sItr).s_szMemo;
		m_ShutoutList.push_back( shutoutListElement );
	}
}

//친구 리스트 가져오기
void cFriendListContents::GetFriendList(void* pData)			
{
	SAFE_POINTER_RET(pData);
	FRIENDLIST* pGetData = static_cast<FRIENDLIST*>(pData);

	*pGetData = m_FriendList;
}
////차단 리스트 가져오기
void cFriendListContents::GetShutoutList(void* pData)
{
	SAFE_POINTER_RET(pData);
	SHUTOUTLIST* pGetData = static_cast<SHUTOUTLIST*>(pData);

	*pGetData = m_ShutoutList;
}
//친구 리스트 요청
void cFriendListContents::SendFriendList(void* pData)
{
	if( m_bSendCheck == true )
		return;

	m_bSendCheck = true;

	if( net::game )
	{
		/*if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
		{
			PersonStoreContents::eVALUE_TYPE valueType;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_GET_VAULETYPE, &valueType);

			switch( valueType )
			{
			case PersonStoreContents::VT_RES_PERSON:
			case PersonStoreContents::VT_RES_PERSON_START:
			case PersonStoreContents::VT_RES_EMPLOYMENT:
			case PersonStoreContents::VT_RES_EMPLOYMENT_START:
				{
					cPrintMsg::PrintMsg( 30357 );
					g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );		
					return;
				}
			}
		}*/
		net::game->SendBuddyList();		
	}
}

//친구/차단 삭제 pair<bool, int> bool true = 친구삭제, false = 차단삭제 int index
void cFriendListContents::DelFriendShutout(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<bool, int> delInfo = *(static_cast< std::pair<bool, int>*>(pData));
	if (delInfo.first == true)
	{
		DelFriend(delInfo.second);
	}
	else
	{
		DelShutout(delInfo.second);
	}
	
}

//친구/차단 메모 작성
void cFriendListContents::SendFriendShutoutMemo(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<bool, std::wstring> memoInfo = *( static_cast< std::pair<bool, std::wstring>* >(pData) );
	if (memoInfo.first == true)
	{
		SendFriendMemo(memoInfo.second);
	}
	else
	{
		SendShutoutMemo(memoInfo.second);
	}
}

//아카이브 타입 체크
void cFriendListContents::CheckAchieveType(void* pData)
{
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_FriendCount;
	recv.nValue1 = m_FriendList.size();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
}
// 친구 추가 성공
void cFriendListContents::ReceiveAddFriendSucces(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ADDINFO* addFriendInfo = static_cast< GS2C_RECV_ADDINFO* >(pData);

	bool nIsOnline = addFriendInfo->s_bOnlineState;
	if( g_pDataMng->GetServerSync() == null )
		return;
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );
	AddFriend( &(addFriendInfo->s_szName), nIsOnline );

	if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::TEMP_FRIEND_LOCAL_TO_SERVER, 0 ) == false )
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::TEMP_FRIEND_LOCAL_TO_SERVER, 0 );
	}
	else
	{
		cPrintMsg::PrintMsg( 30037, const_cast<TCHAR*>( addFriendInfo->s_szName.c_str() ) );
	}
}
// 차단 추가 성공
void cFriendListContents::ReceiveAddShutoutSucces(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::wstring addShutoutInfo = *( static_cast<std::wstring*>(pData) );
	if( g_pDataMng->GetServerSync() == null )
		return;
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::SHUTOUT_LIST, 0 );
	AddShutout( &addShutoutInfo, true );

	if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::TEMP_FRIEND_LOCAL_TO_SERVER, 0 ) == false )
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::TEMP_FRIEND_LOCAL_TO_SERVER, 0 );
	}
	else
	{
		cPrintMsg::PrintMsg( 30039, const_cast<TCHAR*>( addShutoutInfo.c_str() ) );
	}

}
// 친구 Login
void cFriendListContents::ReceiveFriendOnOff(void* pData)
{
	SAFE_POINTER_RET(pData);
	pair<bool, std::wstring> friendOnOffInfo = *( static_cast< pair<bool, std::wstring>* >(pData) );
	FriendOnOff( &friendOnOffInfo.second, static_cast<eONLINE_STATE>(friendOnOffInfo.first) );
}
// 이름 변경 커뮤니티 내부의 이름만 변경
void cFriendListContents::ReceiveChangeName(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_NAME_CHANGE_FREIND_SHUTOUT* recvChangeName = static_cast<GS2C_RECV_NAME_CHANGE_FREIND_SHUTOUT*>(pData);
	ChangeTamerName( recvChangeName->s_szPreName, recvChangeName->s_szNextName );
}
// 이름 변경 주변(테이머 자체의 이름을 변경)
void cFriendListContents::ReceiveChangeNameNearby(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_NAME_CHANGE_NEARBY* recvChangeName = static_cast<GS2C_RECV_NAME_CHANGE_NEARBY*>(pData);
	CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject( RTTI_TAMER, recvChangeName->s_szPreName.c_str() );
	if( pTamer != NULL )
	{
		pTamer->GetCharOption()->SetName( recvChangeName->s_szNextName.c_str() );
#ifdef MASTERS_MATCHING// 테이머 이름 변경 시 아이콘 셋팅
		DWORD dwTeam = pTamer->GetCharOption()->GetMatchTeam();
		pTamer->GetCharOption()->SetMatchIcon( dwTeam );
#endif
	}

	// [5/20/2016 hyun] 커뮤니티 목록 이름 갱신
	//20170215 pBuddy::ChangeName의 동작이 되면 pTamer::ChangeName에서 UI 네임을 변경할 이유는 없다.
	//ChangeTamerName( recvChangeName->s_szPreName, recvChangeName->s_szNextName );

}
// UI 토글 0 차단 탭이면 친구 탭으로 변경 토글, 1 단순 토글
void cFriendListContents::ToggleUI(void* pData)
{
	SAFE_POINTER_RET(pData);
	int notifyType = *( static_cast<int*>(pData) );
	if( g_pGameIF->IsActiveWindow(cBaseWindow::WT_FRIEND_LIST) == false )
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_FRIEND_LIST );
		Notify(RenewTab_Friend);
		return;
	}
	else if (notifyType == 0)
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_FRIEND_LIST );
		if( g_pGameIF->GetFriendList()->_GetTab() != cFriendList::TAB_FRIEND )
		{
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_FRIEND_LIST );
			Notify(RenewTab_Friend);
		}
	}
	else if (notifyType == 1)
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_FRIEND_LIST );
	}
}
void cFriendListContents::GetCurSelect_DataIndex(void* pData)
{
	SAFE_POINTER_RET(pData);
	int* pGetData = static_cast<int*>(pData);

	*pGetData = m_nCurSelectDataIdx;

}
// 친구/차단 추가 패킷 보내기 및 전방체크
void cFriendListContents::SendAddFriendShutout(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<bool, std::wstring> addData = *( static_cast< std::pair<bool, std::wstring>* >(pData) );

	if (addData.first == true)
	{
		SendAddFriend(addData.second);
	} 
	else
	{
		SendAddShutout(addData.second);
	}
	
}
// 테이머 찾지 못함( 친구/차단 추가 삭제 모두 
void cFriendListContents::ReceiveNotFoundTamer(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_NOTFOUND* NotfoundTamer = static_cast< GS2C_RECV_NOTFOUND* >(pData);
	// 1. 친구, 차단 등록시 - 해당 테이머는 존재하지 않음
	// 2. 친구, 차단 목록 제거시 - 등록(친구,차단)되지 않은 테이머

	if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::TEMP_FRIEND_LOCAL_TO_SERVER, 0 ) == false )
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::TEMP_FRIEND_LOCAL_TO_SERVER, 0 );
	}
	else
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::SHUTOUT_LIST, 0 );
		cPrintMsg::PrintMsg( 11101 );
	}
}
//친구 추가
void cFriendListContents::SendAddFriend(std::wstring tamername)
{
	if( PreCheckAddFriend( tamername ) == false )
	{
		switch( Language::CheckTamerName( tamername ) )
		{
		case Language::STRING_EMPTY:
		case Language::STRING_SIZE_MIN:
		case Language::STRING_SIZE_MAX:
		case Language::STRING_UNKNOWN_TYPE:
			{
				cPrintMsg::PrintMsg( 10021 );
				g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );
			}
			break;
		}

		if( net::game )
		{
			/*if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
			{
				PersonStoreContents::eVALUE_TYPE valueType;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_GET_VAULETYPE, &valueType);

				switch( valueType )
				{
				case PersonStoreContents::VT_RES_PERSON:
				case PersonStoreContents::VT_RES_PERSON_START:
				case PersonStoreContents::VT_RES_EMPLOYMENT:
				case PersonStoreContents::VT_RES_EMPLOYMENT_START:
					{
						cPrintMsg::PrintMsg( 30357 );
						g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );		
						return;
					}
				}
			}*/
			net::game->SendBuddyAdd( tamername );
		}
	}	
	else
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );
}
bool cFriendListContents::PreCheckAddFriend(std::wstring tamername)
{
	if( m_FriendList.size() >= neFriend::nMaxFriendCount )
	{
		cPrintMsg::PrintMsg( 30035 );
		return true;
	}
	std::wstring targetName = std::wstring(g_pCharMng->GetTamerUser()->GetName());
	if( targetName.compare(tamername)== 0 )
	{
		cPrintMsg::PrintMsg( 11101 );
		return true;
	}

	for(cFriendListContents::FITR fItr = m_FriendList.begin(); fItr != m_FriendList.end(); ++fItr)
	{
		if( (*fItr).s_szName.compare(tamername) == 0 )
		{
			cPrintMsg::PrintMsg( 30036 );
			return true;
		}
	}

	for(SITR sItr = m_ShutoutList.begin(); sItr != m_ShutoutList.end(); ++sItr)
	{
		if( (*sItr).s_szName.compare(tamername) == 0 )
		{
			m_ShutoutList.erase(sItr);
			break;
		}
	}
	return false;
}
//차단 추가
void cFriendListContents::SendAddShutout(std::wstring tamername)
{
	if( PreCheckAddShutout(tamername) == false )
	{
		switch( Language::CheckTamerName( tamername ) )
		{
		case Language::STRING_EMPTY:
		case Language::STRING_SIZE_MIN:
		case Language::STRING_SIZE_MAX:
		case Language::STRING_UNKNOWN_TYPE:
			{
				cPrintMsg::PrintMsg( 10021 );
				g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::SHUTOUT_LIST, 0 );
			}
			break;
		}

		if( net::game )
			net::game->SendBuddyBan( tamername );
	}	
	else
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::SHUTOUT_LIST, 0 );
}
bool cFriendListContents::PreCheckAddShutout(std::wstring tamername)
{
	if( m_ShutoutList.size() >= neFriend::nMaxShutOutCount )
	{
		cPrintMsg::PrintMsg( 30038 );
		return true;
	}
	std::wstring targetName = std::wstring(g_pCharMng->GetTamerUser()->GetName());

	if( targetName.compare( tamername.c_str() ) == 0 )
	{
		cPrintMsg::PrintMsg( 11101 );
		return true;
	}

	for(SITR sItr = m_ShutoutList.begin(); sItr != m_ShutoutList.end(); ++sItr)
	{
		if( (*sItr).s_szName.compare(tamername.c_str()) == 0 )
		{
			cPrintMsg::PrintMsg( 30036 );
			return true;
		}
	}

	for(FITR fItr = m_FriendList.begin(); fItr != m_FriendList.end(); ++fItr)
	{
		if( (*fItr).s_szName.compare(tamername.c_str()) == 0 )
		{
			m_FriendList.erase( fItr );
			break;
		}
	}
	return false;
}
void cFriendListContents::InitListNSendCheck()
{
	m_bSendCheck = false;
	m_FriendList.clear();
	m_ShutoutList.clear();
}
//친구 삭제
void cFriendListContents::DelFriend(int delIndex)
{
	const FRIEND_INFO* pInfo = GetFriendInfo( delIndex );
	if( pInfo != null )
	{
		if( net::game )
		{
			/*if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
			{
				PersonStoreContents::eVALUE_TYPE valueType;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_GET_VAULETYPE, &valueType);

				switch( valueType )
				{
				case PersonStoreContents::VT_RES_PERSON:
				case PersonStoreContents::VT_RES_PERSON_START:
				case PersonStoreContents::VT_RES_EMPLOYMENT:
				case PersonStoreContents::VT_RES_EMPLOYMENT_START:
					{
						cPrintMsg::PrintMsg( 30357 );
						g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );		
						return;
					}
				}
			}*/
			net::game->SendBuddyDelete( pInfo->s_szName );
		}

		FITR fItr = m_FriendList.begin();
		for( int i = 0; fItr != m_FriendList.end(); ++fItr, ++i)
		{
			if(i == delIndex)
			{
				m_FriendList.erase( fItr );
				break;
			}
		}
		Notify(RenewTab_Friend);
	}

}
//차단 삭제
void cFriendListContents::DelShutout(int delIndex)
{
	const SHUTOUT_INFO* pInfo = GetShutInfo( delIndex );
	if( pInfo != null )
	{
		if( net::game )
		{
			/*if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
			{
				PersonStoreContents::eVALUE_TYPE valueType;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_GET_VAULETYPE, &valueType);

				switch( valueType )
				{
				case PersonStoreContents::VT_RES_PERSON:
				case PersonStoreContents::VT_RES_PERSON_START:
				case PersonStoreContents::VT_RES_EMPLOYMENT:
				case PersonStoreContents::VT_RES_EMPLOYMENT_START:
					{
						cPrintMsg::PrintMsg( 30357 );
						g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::FRIEND_LIST, 0 );		
						return;
					}
				}
			}*/
			net::game->SendBuddyDelete( pInfo->s_szName );
		}

		SITR sItr = m_ShutoutList.begin();
		for( int i = 0; sItr != m_ShutoutList.end(); ++sItr, ++i)
		{
			if(i == delIndex)
			{
				m_ShutoutList.erase( sItr );
				break;
			}
		}
		Notify(RenewTab_Shutout);
	}
}
//친구 메모 패킷 전송
void cFriendListContents::SendFriendMemo(std::wstring memoInfo)
{
	int nIdx = m_nCurSelectDataIdx;
	FRIEND_INFO* pInfo = const_cast< FRIEND_INFO* >( GetFriendInfo( nIdx ) );
	if( pInfo != null )
	{
		if( net::game )
		{
			if( Language::SUCCEEDED != Language::CheckBudyMemo( memoInfo ) )
				return;
			net::game->SendBuddyMemo( pInfo->s_szName, memoInfo);
		}
		pInfo->s_szMemo = memoInfo;
		ContentsStream kStream;
		kStream << nIdx;
		Notify(AddMemo_Friend, kStream);
	}
}

//친구 메모 패킷 전송
void cFriendListContents::SendShutoutMemo(std::wstring memoInfo)
{
	int nIdx = m_nCurSelectDataIdx;
	SHUTOUT_INFO* pInfo = const_cast< SHUTOUT_INFO* >( GetShutInfo( nIdx ) );

	if( pInfo != null )
	{
		if( net::game )
		{
			if( Language::SUCCEEDED != Language::CheckBudyMemo( memoInfo ) )
				return;
			net::game->SendBuddyMemo( pInfo->s_szName, memoInfo );
		}
		pInfo->s_szMemo = memoInfo;
		ContentsStream kStream;
		kStream << nIdx;
		Notify(AddMemo_Shutout, kStream);
	}
}
