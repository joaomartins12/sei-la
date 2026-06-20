#pragma once

#include "ContentsSystem/ContentsSystem.h"

class CRecommendEvent_Contents : public IUIContentSubject< CRecommendEvent_Contents >
{
public:
	struct ST_RecommendInfo
	{
		ST_RecommendInfo() :	m_nRecommendType( 0 )
		{ 
			m_strTamerName.clear();
			memset( AchieveState, 0, sizeof( AchieveState ));
		}

		void	SetTamerName( const std::wstring& strName );
		void	SetMarkValue( int nIndex, int nState );	

		std::wstring	m_strTamerName;
		int				m_nRecommendType;
		u1				AchieveState[ nLimit::MAX_FRIENDLY_MARK ];
	};

	struct ST_RecommendTableData
	{
		ST_RecommendTableData() :
			m_nIndex( 0 ),
			m_nType( 0 ),
			m_nSuccessType( 0 ),
			m_nSuccessValue( 0 )
		{ 
		}

		int		m_nIndex;
		int		m_nType;
		int		m_nSuccessType;
		int		m_nSuccessValue;
	};

public:
	static int const		IsContentsIdentity(void);

	enum eEventType
	{
		eChange_PurposeState,		// 목표의 상태 변경
		eChange_TamerName			// 테이머 이름 변경
	};

public:
	CRecommendEvent_Contents(void);
	virtual ~CRecommendEvent_Contents(void);

	virtual int const		GetContentsIdentity(void) const;
	virtual bool 			Initialize(void);
	virtual void 			UnInitialize(void);
	virtual bool 			IntraConnection(ContentsSystem& System);
	virtual void 			Update(float const& fElapsedTime);
	virtual void 			NotifyEvent(int const& iNotifiedEvt);
	virtual void 			NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void 			MakeMainActorData(void);
	virtual void 			ClearMainActorData(void);
	virtual void 			MakeWorldData(void);
	virtual void 			ClearWorldData(void);
	void					ClearAllData();
	void					Init();	

public:
	bool					GetRecommendInfo( CRecommendEvent_Contents::ST_RecommendInfo& rInfo, std::wstring strTamerName );

	const bool				IsPossibleRecommend() const;
	const bool				IsRecommendEvent() const;

	void					SendMarkItemInfo( int nPurposeIndex );

	void					OpenRecommendEventWindow();

	void					SetSelectedTamerName( const std::wstring TamerName );

	const std::list< ST_RecommendInfo >&	GetRecommendInfoList() const;
	
	bool					GetRecommendTableData( CRecommendEvent_Contents::ST_RecommendTableData& rTableData, int nRecommendType, int iTableIdx );
	bool					GetTableDataList( std::list< ST_RecommendTableData >& rTableList, int nRecommendType );

private:
	void					AddRecommendInfo( const TCHAR* const szName, int nType, u1 nMark[nLimit::MAX_FRIENDLY_MARK] );

	void					SetMyRecommendState( const int	nState );
	void					SetRecommendEventState( const bool bState );

	void					CreateRecommendEventButton();
	void					DeleteRecommendEventButton();
	void					CreateRecommendEventWindow();
	void					CreateRecommendNameInputWindow();

	void					SendRecommendAddAnswerInfo( void* pData );
	void					RecvRecommedEventInfo( void* pData );
	void					RecvRecommedChangeTamerName( void* pData );
	void					RecvFriendlyAddInfo( void* pData );
	void					RecvFriendlyAddAnswerInfo( void* pData );
	void					RecvFriendlyMarkInfo( void* pData );
	void					RecvFriendlyMarkItemInfo( void* pData );
	void					RecvFriendlyChangeStateInfo( void* pData );

	bool					ChangeRecommendTamerName( n1 nType, const std::wstring& BefoleName, const std::wstring& NewName );
	void					ChangePurposeInfo( const int nMarkValue, const int nFriendlyType,const int nMarkIndex, const std::wstring& szTamerName );

	void					InitRecommendTableData();


private:
	int						m_nRecommendState;		// 내가 추천 가능한 상태인지 판단하는 변수
	bool					m_bRecommendEvent;		// 이벤트 기간인지 아닌지
	std::wstring			m_strSelectedTamerName;	// 현재 선택되어있는 테이머명

private:
	std::list< ST_RecommendInfo >							m_RecommendInfoList;
	std::map< int, std::list< ST_RecommendTableData > >		m_mapRecommendTableData;
};
