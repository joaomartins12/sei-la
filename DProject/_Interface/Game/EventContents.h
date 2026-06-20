#pragma once


class AdaptTutorialQuest;

class EventContents
	: public IUIContentSubject< EventContents >
{
public:
	struct sExpEventData
	{
		sExpEventData();
		~sExpEventData();

		void	ResetData();

		u4		m_nExpRate;			// 경험치 비율
		u4		m_nNextDayExpRate;	// 다음날 접속시 받게되는 경험치 비율
		u4		m_nExpTarget;		// 경험치 종류 1: 일반 2: 부스터
		u4		m_nSpecialExp;		// 해당 유저가 스폐셜 경험치 부여 대상이라면 0이 아닌 값이 들어 있습니다. 최대 500 이상은 되지 않습니다.
		bool	m_HideExpEventUI;	// UI 숨김
	};

	struct sDailyEventData
	{
		sDailyEventData();
		~sDailyEventData();

		void	ResetData();
		
		int		GetTimeLineCount() const;
		int		GetWeekStartIdx() const;
		int		GetCompleteCount() const;

		n4		m_nEventNo;				// DM_Event-매일 참조 - 이벤트 번호
		u4		m_nRemainedTimeTS;		// 해당 이벤트 아이템을 받기위해 남은 시간
		u4		m_nTotalTime;			// 해당 이벤트 아이템을 받기위한 총 시간
		byte	m_nWeek;				// 2017-02-16-nova 서버에서 보내주는 요일

		u4		m_nRecvServerTimeTS;		
		struct sItems
		{
			int	m_nItemCode;	// 아이템 코드
			int m_nItemCount;	// 아이템 갯수
		};

		std::list<sItems>	m_ItemLists;	// 받을 아이템 목록
	};

	//2017-02-16-nova 월간 이벤트 정보
	struct sMonthlyEventData
	{
		sMonthlyEventData();
		~sMonthlyEventData();

		void	ResetData();
		bool	SetUpdateData( byte const& nEventNo, byte const& attendCnt, bool const& bNotify );
		bool	IsHaveEvent() const;

		byte	m_nEventNo;		//이벤트번호		
		byte	m_nAttendCnt;	//출석일수		
		bool	m_bNotify;		//최초알림

		CsEventTable::sMonthlyEvent* monthInfo;
	};

	//////////////////////////////////////////////////////////////////////////
	struct sHotTimeEventData
	{
		sHotTimeEventData():nEventState(0),nCurrentTableIdx(0),nNextTableIdx(0),bIsGetItem(false),nStartTimeLeftTs(0),nEndTimeLeftTs(0),bWaitRecv(false),
			pHotTimeCur(NULL), pHotTimeNext(NULL) {};
		~sHotTimeEventData() {};

		void ResetData();
		bool IsHaveEvent() const;
		bool IsGetItemTime() const;// 아이템을 받을 수 있는 상태인지 확인
		bool IsGetItem() const;	// 아이템을 받았는지 검사 
		void SetGetItem(bool bValue);	// 아이템을 받았는지 검사 
		int  GetCurrentItemTIDX() const;
		int	 GetNextItemTIDX() const;
		
		int				nEventState;
		int				nCurrentTableIdx;
		int				nNextTableIdx;	
		bool			bIsGetItem;
		u4				nStartTimeLeftTs;
		u4				nEndTimeLeftTs;	

		wstring			szStartDate;
		wstring			szEndDate;

		bool			bWaitRecv;

		CsEventTable::sHotTimeEvent const* pHotTimeCur;
		CsEventTable::sHotTimeEvent const* pHotTimeNext;
	};

	//////////////////////////////////////////////////////////////////////////

	// 출석부 수령 아이템 정보
	struct sCheckItemInfo
	{
		sCheckItemInfo() : bChecked(false) {}
		~sCheckItemInfo() {}

		void			SetCheckItem( std::map< uint, uint >& mapItems, std::wstring const& wsDate, bool const& bCheck );
		void			SetCheckState( bool const& bCheck );

		std::map< uint, uint > const&	GetCheckItems() const;
		std::wstring	GetCheckDate() const;
		bool			GetCheckState() const;

		//		ItemID, ItemCount
		std::map< uint, uint >	mapCheckItems;
		std::wstring	wsCheckDate;
		bool			bChecked;
	};

	// 출석부 그룹 정보
	struct sCheckGroupInfo
	{
		sCheckGroupInfo() : nCheckGroupID(0), nCheckIndex(0), nCheckNextLeftTS(0) {}
		~sCheckGroupInfo() {}

#if COMMON_LIB_FIXED
		void			InitCheckInfo( nsDailyCheckEvent::sDailyCheckData& cDailyInfo );
#endif

		void			SetCheckNextTimer( uint const& nLeftTS );
		bool			SetCheckNextIndex();
		void			SetCheckInfo( uint const& nGroupID, uint const& nTodayIndex, uint const& nNextLeftTS );
		void			ResetCheckInfo();
		sCheckItemInfo const*	GetCheckItemInfo( uint const& nIndex ) const;
		sCheckItemInfo const*	GetCheckableItemInfo() const;
		uint			GetCheckItemListCount() const;
		uint			GetCheckIndex() const;
		bool			IsCurrentGroupID( uint const& nGroupID ) const;
		bool			IsCheckable() const;
		bool			IsNextCheckTime();

		uint			nCheckGroupID;		// 출석부 그룹 아이디(1~)
		uint			nCheckIndex;		// 수령일 인덱스(1~)
		uint			nCheckNextLeftTS;	// 다음 출석일까지 남은 시간
		CsTimeSeq		cLeftTimer;			// 다음 출석일 타이머
		std::map< uint, sCheckItemInfo >	mapCheckInfos;		// 수령 아이템 인덱스(1~), 수령 아이템 정보
	};

	// 출석부 이벤트 정보
	struct sDailyCheckEventData
	{
		sDailyCheckEventData() : bWaitRecv(false) {}
		~sDailyCheckEventData() {}

#if 0
		void			InitDailyCheckList( nsDailyCheckEvent::sDailyCheckData& cDailyInfo );
#endif

		void			SetDailyCheckList( uint const& nGroupID, uint const& nTodayIndex, uint const& nNextLeftTS );
		bool			SetNextDailyCheck( uint const& nGroupID );
		void			ResetData();
		void			ResetCheckListData( uint const& nGroupID );

		uint			GetGroupID( int const& nIndex ) const;
		uint			GetGroupCount() const;
		std::wstring	GetGroupName( int const& nIndex ) const;
		sCheckGroupInfo const*	GetCheckGroupInfo( int const& nIndex ) const;
		sCheckItemInfo const*	GetCheckableItemInfo( int const& nIndex ) const;
		bool			IsCheckable( int const& nIndex ) const;
		bool			IsHaveDailyCheckEvent() const;
		bool			IsWaitRecv() const;

		bool			bWaitRecv;
		std::list< sCheckGroupInfo >		lDailyCheck;
	};

public:
	enum eEvent
	{
		eUPDATE_EXP_EVENT_DATA,
		eDailyEvent_Update,
		eMonthlyEvent_Update,	
		eHotTimeEvent_Update,
		eHotTimeEvent_GetItem_PacketRecv,
		eDailyCheckEvent_UpdateInfo,
		eDailyCheckEvent_UpdateItem,
		eDailyCheckEvent_NotTime,
	};
public:
	static int const IsContentsIdentity(void);

public:
	EventContents(void);
	virtual ~EventContents(void);
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

	static void CALLBACK ExpEventCheckTimerProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime );

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	//////////////////////////////////////////////////////////////////////////
	// Exp Event Function
	//////////////////////////////////////////////////////////////////////////
	void		Recv_ExpEventData(void* pData);
	void		Hide_ExpEventData(void* pData);
	void		Show_ExpEventData(void* pData);
	void		Get_NextExpRate(void* pData);
	void		Recv_ExpEventDisable( void* pData );

	void		RewardUI_Toggle(void* pData);

	void		SetExpEventTimer();
	void		RemoveExpEventTimer();

public:
	unsigned int GetCurrentExpRate() const;
	unsigned int GetExpRateType() const;
	unsigned int GetNextExpRate() const;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Daily Event Function
	//////////////////////////////////////////////////////////////////////////
public:
	void			CloseDailyEventUI();	
	void			CloseRewardEventUI();				//2017-03-08-nova
	bool			AcceptButtonEvent(int sel, int selSub = 0);			//2017-03-08-nova

	unsigned int	GetDailyEventTotalTs() const;
	unsigned int	GetDailyEventCurrentTs() const;
	unsigned int	GetDailyEventRemainedTs() const;
	std::list<sDailyEventData::sItems> const& GetDailyEventItems() const;

	//////////////////////////////////////////////////////////////////////////
	byte			GetMonthlyEventNo() const;
	byte			GetMonthlyEventAttendCnt() const;
	bool			GetMonthlyEventNotify() const;
	//////////////////////////////////////////////////////////////////////////

	int				GetDailyEventNo() const;			//2017-02-16-nova	일간 이벤트 번호	
	byte			GetDailyEventWeek() const;			//2017-02-16-nova	일간 요일정보
	//unsigned int	GetDailyEventCurrentTime() const;	//2017-02-16-nova	일간 이벤트 진행시간
	int				GetDailyEventPlayTimeMinute() const;
	int				GetDailyEventTimeLineCount() const;
	int				GetDailyEventCompleteCount() const;
	int				GetDailyEventWeekStartIdx() const;
	
	//////////////////////////////////////////////////////////////////////////

	sMonthlyEventData const& GetMonthlyEventData() const;

	int				GetMonthItemID(int no);
	USHORT			GetMonthItemCnt(int no);
	void			SetMonthMessage(cText* text);

	// Hot Time Event 아이템을 받으려고 서버에 패킷을 보내는 함수
	bool			Send_HotTimeEvent_ItemGet();
	bool			IsHaveHotTimeEvent() const;
	sHotTimeEventData const& GetHotTimeEvent_Info() const;

	std::wstring	GetItemName( DWORD const& dwItemCode );
	NiColor			GetItemColor( DWORD const& dwItemCode );

	//////////////////////////////////////////////////////////////////////////
	// Daily Check Event Function
private:
	void			Send_DailyCheckEventInfo_Request();
	void			Recv_DailyCheckEventInfo_Request( void* pData );
	void			Send_DailyCheckEvent_ItemGet( int const& nGroupIndex );
	void			Recv_DailyCheckEvent_ItemGet( void* pData );
	void			Update_DailyCheckEvent_Timer( void* pData );

	bool			CheckInventory(EventContents::sCheckItemInfo const* pCheckItem);

#if COMMON_LIB_FIXED
	void			InsertItemToInventory(std::list< nsDailyCheckEvent::sDailyCheckedItem >& lCheckedItems);
#endif

public:
	bool			IsHaveDailyCheckEvent() const;
	uint			GetDailyCheckEventCount() const;
	void			GetDailyCheckEventInfo( int const& nGroupIndex, std::wstring& wsTitle, std::wstring& wsDesc,
											std::wstring& wsStart, std::wstring& wsEnd );
	sCheckGroupInfo const*	GetDailyCheckGroupInfo( int const& nGroupIndex ) const;

	//////////////////////////////////////////////////////////////////////////

	std::wstring	GetSubTitleName( int const& nMainIndex, int const& nSubIndex ) const;

private:
	void			Recv_DailyEventData(void* pData);
	void			Recv_MonthlyEventData(void* pData);	

	void			Recv_NTF_HotTimeEventInfo(void* pData);
	void			Recv_HotTimeEventItemGet(void* pData);

private:
	sExpEventData		m_ExpEventData;
	sDailyEventData		m_DailyEventData;
	sMonthlyEventData	m_MonthlyEventData;	
	sHotTimeEventData	m_HotTimeEventData;
	sDailyCheckEventData	m_DailyCheckEventData;

	TOwnershipGuard< AdaptTutorialQuest >		m_pAdaptTutorial;
};
