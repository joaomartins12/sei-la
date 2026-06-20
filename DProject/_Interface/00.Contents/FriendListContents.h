#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

class cFriendListContents
	: public IUIContentSubject< cFriendListContents >
{
public:
	enum eEventType
	{
		RenewTab_Friend,			//친구 탭을 시작/갱신
		RenewTab_Shutout,			//차단 탭을 시작/갱신
		AddMemo_Friend,				//친구 메모 추가
		AddMemo_Shutout,			//차단 메모 추가
		Friend_OnOffline,			//친구 on/off
	};
	enum eONLINE_STATE{ OFFLINE, ONLINE, NO_EXIST, };
	struct FRIEND_INFO : public NiMemObject
	{
		FRIEND_INFO() {};

		eONLINE_STATE			s_eOnlineState;		
		std::wstring			s_szName;
		std::wstring			s_szMemo;
	};
	struct SHUTOUT_INFO : public NiMemObject
	{
		SHUTOUT_INFO() {};

		std::wstring		s_szName;
		std::wstring		s_szMemo;
	};

	typedef std::list<FRIEND_INFO>		FRIENDLIST;
	typedef FRIENDLIST::iterator		FITR;
	typedef FRIENDLIST::const_iterator	CFITR;
	typedef std::list<SHUTOUT_INFO>		SHUTOUTLIST;
	typedef SHUTOUTLIST::iterator		SITR;
	typedef SHUTOUTLIST::const_iterator	CSITR;
public:

	cFriendListContents(void);
	virtual ~cFriendListContents(void);
	// 상속 기본 선언
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);
	static int const IsContentsIdentity(void);

	//data get
	const FRIENDLIST&		 GetFriendList() const;
	const SHUTOUTLIST&		 GetShutoutList() const;

	//Friend
	void					AddFriend( std::wstring* szName, int nOnline );
	bool					IsMyFriend( std::wstring const& wsTamerName ) const;
	const FRIEND_INFO*		GetFriendInfo( const unsigned int nIndex );
	const FRIEND_INFO*		GetFriendInfo( const std::wstring* szName );

	void			FriendOnOff( std::wstring const* szName, eONLINE_STATE nOnline );
	void			SetOnOff( bool bCheck );
	const bool		GetOnOff();
	
	//Shutout
	void			AddShutout( std::wstring const* szName, bool bOnline );	
	const SHUTOUT_INFO*	GetShutInfo( const unsigned int nIndex );
	const SHUTOUT_INFO*	GetShutInfo( const std::wstring* szName );

	bool			IsShutout( std::wstring const* szName ) const;

	//CurSel DataIndex
	const int		GetCurSelect_DataIndex();
	void			SetCurSelect_DataIndex(int curSelect);

private:
	void			InitListNSendCheck();						//list와 m_bSendCheck를 초기화

	//add friend
	void			SendAddFriend(std::wstring tamername);		//친구 추가
	bool			PreCheckAddFriend(std::wstring tamername);	//친구 추가 전방 체크

	//add shutout
	void			SendAddShutout(std::wstring tamername);		//차단 추가
	bool			PreCheckAddShutout(std::wstring tamername);	//차단 추가 전방 체크
	
	//del firend/shutout
	void			DelFriend(int delIndex);					//친구 삭제
	void			DelShutout(int delIndex);					//차단 삭제

	//Send memo
	void			SendFriendMemo(std::wstring memoInfo);		//친구 메모 패킷 전송
	void			SendShutoutMemo(std::wstring memoInfo);		//친구 메모 패킷 전송

	void			ChangeTamerName( const std::wstring& BefoleName, const std::wstring& NewName );

private:
	//Event 함수
	//pair<bool, int> bool true = 친구, false = 차단
	void		ReceiveFriendList(void* pData);			// 친구 리스트 받음
	void		ReceiveShutoutList(void* pData);		// 차단 리스트 받음
	void		ReceiveFriendOnOff(void* pData);		// 친구 Login/LogOff
	void		ReceiveChangeNameNearby(void* pData);	// 이름 변경 주변
	void		ReceiveChangeName(void* pData);			// 이름 변경
	void		ReceiveAddFriendSucces(void* pData);	// 친구 추가 성공
	void		ReceiveAddShutoutSucces(void* pData);	// 차단 추가 성공
	void		ReceiveNotFoundTamer(void* pData);		// 테이머 찾지 못함( 친구/차단 추가 삭제 모두 

	void		SendFriendList(void* pData);			// 친구 리스트 요청
	void		SendFriendShutoutMemo(void* pData);		// 친구/차단 메모 작성
	void		SendAddFriendShutout(void* pData);		// 친구/차단 추가 패킷 보내기 및 전방체크

	void		GetFriendList(void* pData);				// 친구 리스트 가져오기
	void		GetShutoutList(void* pData);			// 차단 리스트 가져오기	
	void		GetCurSelect_DataIndex(void* pData);	// 현재 선택 중인 목록의 list index를 가져온다.

	void		CheckAchieveType(void* pData);			// 아카이브 타입 체크
	void		DelFriendShutout(void* pData);			// 친구/차단 삭제 pair<bool, int> bool true = 친구삭제, false = 차단삭제 int index
	void		ToggleUI(void* pData);					// UI 토글 0 차단 탭이면 친구 탭으로 변경 토글, 1 단순 토글

private:
	FRIENDLIST		m_FriendList;
	SHUTOUTLIST		m_ShutoutList;

	bool			m_bSendCheck;						// SendFriendList 여부 체크
	bool			m_bOnOffCheck;						// 오프라인 친구 보기

	int				m_nCurSelectDataIdx;				// 실제 정보에서 선택

	
};
