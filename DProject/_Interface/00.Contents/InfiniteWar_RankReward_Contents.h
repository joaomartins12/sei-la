#pragma once

#include "../../ContentsSystem/ContentsSystem.h"


class CInfiniteWarRankRewardContents
	: public IUIContentSubject< CInfiniteWarRankRewardContents >
{
public:
	static int const IsContentsIdentity(void);

	enum sEvent
	{
		eUpdate_Today_Injected_Point,	// 포인트 변경
		eUpdate_NoticeBoard_Data,		// 게시판 정보 갱신
		eChange_NeedItem_Count,			// 포인트 투입에 필요한 아이템의 갯수가 변경 瑛?때
		eOpenRankRewardItemWindow,
		eUpdate_RankInfoByType,			// 주간/월간/시즌 별 랭커 정보 갱신 
		eUpdate_FameRankerList,			// 명예의 전당 랭커 리스트 갱신
		eUpdate_FameSeasonList,			// 명예의 전당 시즌 리스트 갱신
		eUpdate_Previously_RankerList,	// 전주/전월/전시즌의 랭커 리스트 갱신.
	};

	struct sRecvedData
	{
		sRecvedData() : bRecvData(false) {};
		virtual ~sRecvedData() {};

		bool		IsRecvData() const { return bRecvData; } 

		bool		bRecvData;				// 서버로 부터 데이터를 받았는지 체크		
	};

	struct sRankerList	// 랭커리스트의 기본 정보
	{
		sRankerList():nRank(0),dwPoint(0),nUpDownPoint(0),bIsNew(false)
		{}
		~sRankerList() {};

		int				nRank;			// 랭킹 순위
		std::wstring	wsTamerName;	// 테이머 이름
		std::wstring	wsGuildName;	// 소속 길드 명
		DWORD			dwPoint;		// 포인트
		int				nUpDownPoint;	// 등록 포인트
		bool			bIsNew;			// 새로 랭커 리스트에 추가榮쩝?여부 
	};

	struct sPreviouslyRankerInfo// 명예의 전당 랭커 정보 
	{
		sPreviouslyRankerInfo():nRank(0),dwPoint(0)
		{}
		~sPreviouslyRankerInfo() {}

		int				nRank;			// 랭킹 순위
		std::wstring	wsTamerName;	// 테이머 이름
		std::wstring	wsGuildName;	// 소속 길드 명
		DWORD			dwPoint;		// 포인트
	};

	struct sPriviouslyData : public sRecvedData	// 랭킹 타입별 데이터
	{
		sPriviouslyData() {};
		~sPriviouslyData() {};

		void								InitData();
		std::list<sPreviouslyRankerInfo>	listPreviouslyRanker;	// 이전 랭커 리스트
	};

	struct sRankInfos : public sRecvedData	// 랭킹 타입별 데이터
	{
		sRankInfos(): nMyRanking(0),nUpDownPoint(0),bAbsRanking(false),nMyRankPrecent(0),
			nMyPoint(0),i64RestRankRenewSec(0),i64RestRankEndSec(0),nPreviouslyMyRanking(0),nRankState(0)
		{}

		~sRankInfos() {}
		
		void								InitData();
		bool								IsNeedUpdateData();	// 데이터 갱신이 필요한지 체크

		int									nMyRanking;			// 나의 랭킹
		int									nMyPoint;			// 나의 포인트
		int									nRankState;			// 현재 랭킹의 진행 상황
		int									nUpDownPoint;		// 랭킹 등락
		int									nMyRankPrecent;		// 절대 수치에 들지 않았을 경우 퍼센트 수치
		int									nPreviouslyMyRanking;	//  (전주/전월/전시즌)별 내 랭킹
		bool								bAbsRanking;		// false : 절대 수치, true : 상대 추시
		__int64								i64RestRankRenewSec;	//다음 랭킹 갱신 시간
		__int64								i64RestRankEndSec;		//현 랭킹 마감 시간

		std::list<TB::sRewardItemInfo>		listRewardItems;		// 보상 아이템 리스트
		std::list<sRankerList>				listRanker;				//랭커 리스트
		sPriviouslyData						PriviouslyData;
	};

	//////////////////////////////////////////////////////////////////////////
	// 일일 포인트 투입 데이터
	struct sDailyPointData : public sRecvedData
	{
		sDailyPointData() :m_nCurrentDay(-1),m_dwTotalInjectedPoint(0),m_RestDayTime(0)
		{};

		~sDailyPointData() {};

		void								InitData();		// 데이터 삭제
		bool								IsNeedUpdateData();	// 데이터 갱신이 필요한지 체크

		int									m_nCurrentDay;			// 현재 요일
		DWORD								m_dwTotalInjectedPoint;	// 현재 요일에 총 넣은 포인트
		__int64								m_RestDayTime;			// 요일 초기화 서버 시간
		std::map<int, std::list<DWORD>>		m_mapDayPointItemList;	// 현재 요일에 받을 수 있는 투입 포인트별 아이템 목록
	};

	//////////////////////////////////////////////////////////////////////////
	// 명예의 전당 관련 구조체
	struct sFameRankerInfo// 명예의 전당 랭커 정보 
	{
		sFameRankerInfo():nRank(0),dwPoint(0)		{};
		~sFameRankerInfo() {};

		int				nRank;			// 랭킹 순위
		std::wstring	wsTamerName;	// 테이머 이름
		std::wstring	wsGuildName;	// 소속 길드 명
		DWORD			dwPoint;		// 포인트
	};

	struct sFamData : public sRecvedData
	{
		sFamData() {}
		~sFamData() {}
		std::list<sFameRankerInfo>	RankerList;
	};

	// 명예의 전당 관련 데이터
	struct sHallOfFameData : public sRecvedData
	{
		sHallOfFameData():i64UpdateRamainTime(0)	{}
		~sHallOfFameData() {}

		void					InitData();		// 데이터 삭제
		bool					IsNeedUpdateData();	// 데이터 갱신이 필요한지 체크

		__int64					i64UpdateRamainTime;	// 시즌 종료 시간과 같음. 시즌이 종료 되야지 
		std::map<int,sFamData>	m_mapFameRankerList;	// 시즌별 명예의 전당 렝커 리스트
	};

public:
	CInfiniteWarRankRewardContents(void);
	virtual ~CInfiniteWarRankRewardContents(void);

	virtual int const			GetContentsIdentity(void) const;
	virtual bool				Initialize(void);
	virtual void				UnInitialize(void);
	virtual bool				IntraConnection(ContentsSystem& System);
	virtual void				Update(float const& fElapsedTime);
	virtual void				NotifyEvent(int const& iNotifiedEvt);
	virtual void				MakeMainActorData(void);
	virtual void				ClearMainActorData(void);
	virtual void				MakeWorldData(void);
	virtual void				ClearWorldData(void);

	void						OpenCloseNoticeWindow(bool bVisible);
	void						OpenPreRankingListWindow(int const& nType);
	void						OpenRankRewardItemListWindow(int const& nType);
	void						ClosePreRankingListWindow();
	void						CloseRankRewardItemListWindow();

	CsC_AvObject*				GetTargetNpc() const;

	DWORD						GetCurrentNeedItemCode() const;
	DWORD						GetCurrentNeedItemCount() const;

	DWORD						GetTodayTotalInjectedPoint() const;
	__int64						GetRestRemainTime() const;
	std::wstring				GetItemName( DWORD const& dwItemCode );
	void						SendPointInject( int const& nInjectCount );	
	void						SendRequestInfiniteWarData();

	sRankInfos const *			GetRankInfoByType(int const& nType) const;
	sRankInfos const*			GetPreviouslyRankerInfoByType( int const& nType ) const;
	std::list<sRankerList> const * GetRankerListFormType(int const& nType) const;
	CsInfiniteWarMng::DailyRewardItems const * GetTodayRewardItemLIst() const;

	void						GetHallofFameSeasonList( std::list<int>& lists );
	std::list<sFameRankerInfo> const* GetHallofFameRankerList(int const& nSeasonIdx) const;

	std::list<TB::sRankRewardInfo> const* GetRewardItemListFromType( int const& nType ) const;

	bool						IsNeedUpdateRankInfoDataByType( int const& nType );
	bool						IsNeedUpdatePreviouslyRankerData( int const& nType );

	void						SendRankingDataUpdateByType(int const& nType);
	void						SendRankingDeadline(int const& nType);

	void						SelectedHallofFameBySeason( int const& nType );
	bool						IsNeedUpdateFameListData( int const& nType );

private:
	void						_InitData();
	void						_ClearData();
	void						_WindowsLock( cBaseWindow::eWINDOW_TYPE wt, bool bLock );

	void						_CheckDailyDataUpdate();
	void						_CheckRankingDataUpdate();
	void						_CheckHallOfFameDataUpdate();
	

	void						_OpenNoticeboardWindow(void* pData);
	void						_Recv_NoticeBoardData(void* pData);
	void						_Recv_NoticeBoard_PointPut_Result(void* pData);
	void						_Ntf_NoticeBoardData_Updata(void* pData);
	void						_RecvRankerList(void* pData);
	void						_RecvPreRankerList(void* pData);
	void						_RecvHallofFameList(void* pData);
	void						_RecvHallofFameSeasonList(void* pData);

	sRankInfos&					_GetRankInfoByType(int const& nType);
	sFamData&					_GetFameRankerList( const int & nSeason );
	void						_SetRewardItemFromMyRanking( int const& nRankType, int const& nMyRanking, int const& nRankPercent, std::list<TB::sRewardItemInfo>& itemlist  );

	void						_SendReuqestRankInfoByType( int const& nType );
	void						_SendReuqestPreviouslyRankerList( int const& nType );
	void						_SendReuqestFameListBySeason( int const& nType );
	void						_SendReuqestFameSeasonListBySeason();
	void						_ResetPriviouslyRankListData(int const& nType);

private:
	CsC_AvObject*				m_pTarget;
	DWORD						m_dwTargetNpcIdx;
	DWORD						m_dwTargetNpcIdx_Temp;

	std::map<int,sRankInfos>	m_mapRankInfoByType;	// 랭킹 타입(주/월/시즌간) 별 랭크 정보( 랭커 리스트 포함 )
	sDailyPointData				m_sDailyPointData;		// 일일 포인트 투입 데이터
	sHallOfFameData				m_sHallOfFameData;		// 명예의 전당 관련 데이터
};
