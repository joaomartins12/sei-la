#pragma once

#define EVENT_MSG_LEN		512
#define EVENT_ITEM_MAXCNT	6		
#define MONTHLY_ITEM_MAXCNT	28	//2017-03-07-nova 월간접속보상 

class CsEventTable
{
public:
	enum eEVENT_TYPE{
		ET_DAILY			=	10000,
		ET_ATTENDANCE		=	20000,
		ET_NOT_LOGIN		=	30000,
		ET_PCBANG			=	40000,
		ET_LEVELUP			=	50000,
		ET_MONTHLY			=	60000,	//2017-03-07-nova 월간접속보상 
		ET_DIGIMONPARCEL	=	70000,	//디지몬 분양 이벤트 지급 아이템
		ET_HOTTIME			=   80000,
	};

	struct sMonthlyEvent
	{
	public:
		sMonthlyEvent(){ s_nTableNo = 0; s_szMessage[ 0 ] = NULL; memset( s_nItemID, 0, sizeof( int )*MONTHLY_ITEM_MAXCNT ); memset( s_nItemCnt, 0, sizeof( USHORT )*MONTHLY_ITEM_MAXCNT ); }
	public:
		int		s_nTableNo;
		TCHAR	s_szMessage[ EVENT_MSG_LEN ];

		int		s_nItemID[ MONTHLY_ITEM_MAXCNT ];
		USHORT	s_nItemCnt[ MONTHLY_ITEM_MAXCNT ];
	};

	struct sEVENT
	{
	public:
		sEVENT();
		~sEVENT();

	public:
		int		s_nTableNo;

		int		s_nMinute;	//2017-03-07-nova 일간 이벤트 시간

		int		s_nItemID[ EVENT_ITEM_MAXCNT ];
		USHORT	s_nItemCnt[ EVENT_ITEM_MAXCNT ];

		TCHAR	s_szMessage[ EVENT_MSG_LEN ];
	};

	struct sRECOMMENDE
	{
	public:
		sRECOMMENDE(){ s_nTableNo = 0; s_nIndex = 0; s_nType = 0; s_nSuccessType = 0; s_nSuccessValue = 0; memset( s_nItemID, 0, sizeof( int )*EVENT_ITEM_MAXCNT ); 
						memset( s_nItemCnt, 0, sizeof( USHORT )*EVENT_ITEM_MAXCNT ); /*s_nSDate = 0; s_nSTime = 0; s_nEDate = 0; s_nETime = 0; s_nSYear = 0;*/
						/*s_nSMonth = 0; s_nSDay = 0; s_nSHour = 0; s_nSMin = 0; s_nSSec = 0; s_nEYear = 0; s_nEMonth = 0; s_nEDay = 0; s_nEHour = 0; s_nEMin = 0; s_nESec = 0;*/ s_nItemKind = 0;}
	public:
		int		s_nTableNo;
		int		s_nUse;

		int		s_nIndex;
		int		s_nType;
		int		s_nSuccessType;
		int		s_nSuccessValue;
// 		int		s_nSYear;
// 		int		s_nSMonth;
// 		int		s_nSDay;
// 		int		s_nSHour;
// 		int		s_nSMin;
// 		int		s_nSSec;
// 		int		s_nEYear;
// 		int		s_nEMonth;
// 		int		s_nEDay;
// 		int		s_nEHour;
// 		int		s_nEMin;
// 		int		s_nESec;

		int		s_nItemKind;
		int		s_nItemID[ EVENT_ITEM_MAXCNT ];
		USHORT	s_nItemCnt[ EVENT_ITEM_MAXCNT ];

		// 친구추천 기간 체크 수정 chu8820
		TCHAR	s_cStartTime[ 32 ];
		TCHAR	s_cEndTime[ 32 ];
	};

	struct sATTENDANCE_INFO
	{
		tm	s_StartTime;
		tm	s_EndTime;
	};

	struct sHotTimeEvent
	{
		sHotTimeEvent() : s_nEventNo( 0 ), s_nDay( 0 ), s_nItemNo( 0 ), s_nItemCnt( 0 ){};
		
		int			s_nEventNo;
		wstring		s_szStartDate;
		wstring		s_szEndDate;
		int			s_nDay;
		wstring		s_szStartTime;
		wstring		s_szEndTime;
		int			s_nItemNo;
		int			s_nItemCnt;
	};

protected:
	sATTENDANCE_INFO				m_AttendanceInfo;
	std::map< int, sEVENT* >		m_mapEvent;
	std::map< int, sRECOMMENDE* >	m_mapRecommendE;

	std::map< int, sMonthlyEvent* >	m_mapMonthlyEvent;	//2017-03-07-nova 월간접속보상
	std::map< int, sHotTimeEvent* > m_mapHotTimeEvent;	// 핫타임 이벤트

public:
	void			Delete();
	bool			Init( char* cPath );

	bool			IsEvent( int nType, int nNO ) const;
	sEVENT*			GetMap( int nType, int nNO );
	sMonthlyEvent*	GetMapMonthly( int nType, int nNO );
	void			SaveBin( char* cPath );

	int				GetRecommendTableCount() const;
	bool			IsRecommendE(int nNO ) const;
	sRECOMMENDE*	GetMap( int nNO );

	sATTENDANCE_INFO*	GetAttendanceInfo();
	sHotTimeEvent const* GetHotTime( int nEventNo ) const;

protected:
	void			_LoadExcel();	
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

	void			_LoadExcelDaily( DWORD nStartID );
	void			_LoadExcelAttendance( DWORD nStartID );
	void			_LoadExcelNotLogIn( DWORD nStartID );	
	void			_LoadExcelPCbang( DWORD nStartID );
	void			_LoadExcelLevelup( DWORD nStartID );
	void			_LoadExcelMonthly( DWORD nStartID );	//2017-03-07-nova 월간접속보상 엑셀파일
	void			_LoadExcelParcel( DWORD nStartID );
	void			_LoadExcelFEvent();
	void			_LoadExcelHotTime();	// 핫타임 이벤트

	//////////////////////////////////////////////////////////////////////////
	// 출석부 이벤트
public:
	struct sDailyCheckItem
	{
		sDailyCheckItem() : nRewardItemID(0), nRewardItemCnt(0) {}
		~sDailyCheckItem() {}

		std::wstring	wsCheckData;		// 출석일자
		UINT			nRewardItemID;		// 출석 보상 아이템 번호
		UINT			nRewardItemCnt;		// 출석 보상 아이템 개수
	};

	typedef std::list< sDailyCheckItem >		LIST_DCHECK_ITEM;
	typedef LIST_DCHECK_ITEM::iterator			LIST_DCHECK_ITEM_IT;
	typedef LIST_DCHECK_ITEM::const_iterator	LIST_DCHECK_ITEM_CIT;

	struct sDailyCheckTime
	{
		sDailyCheckTime() {}
		~sDailyCheckTime() {}

		std::wstring	wsEventTab;			// 이벤트 탭명
		std::wstring	wsEventTitle;		// 이벤트 타이틀명
		std::wstring	wsEventDescript;	// 이벤트 설명
		std::wstring	wsStartTime;		// 이벤트 시작 일
		std::wstring	wsEndTime;			// 이벤트 종료 일
		std::wstring	wsResetTime;		// 출석 보상 초기화 시간

		LIST_DCHECK_ITEM lDailyRewards;		// 출석부 보상 목록
	};

	typedef std::map< UINT, sDailyCheckTime >	MAP_DAILYCHECK;
	typedef MAP_DAILYCHECK::iterator			MAP_DAILYCHECK_IT;
	typedef MAP_DAILYCHECK::const_iterator		MAP_DAILYCHECK_CIT;

private:
	MAP_DAILYCHECK	m_mapDailyCheck;

private:
	void			_LoadExcelDailyCheckEvent();
	void			_LoadBinDailyCheckEvent( FILE* fp );
	void			_LoadFilePackDailyCheckEvent( int nHandle );
	void			_SaveBinDailyCheckEvent( FILE* fp );

public:
	sDailyCheckTime const*		GetDailyCheck( UINT const& nGroupNo ) const;
	LIST_DCHECK_ITEM const*		GetDailyCheckRewardList( UINT const& nGroupNo ) const;
};