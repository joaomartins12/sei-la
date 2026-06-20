#pragma once

#include "../../ContentsSystem/ContentsSystem.h"


class CInfiniteWarContents
	: public IUIContentSubject< CInfiniteWarContents >
{
public:
	struct sStageInfo
	{
		sStageInfo();
		~sStageInfo();

		bool IsClear() const;
		std::wstring const& GetMonsterName() const;
		std::string const& GetMonsterIconFile() const;

		void SetClearMonster(DWORD const & dwMonsterIDX);

		DWORD			s_dwMonsterIDX;	// 클리어한 디지몬 인덱스 번호
		std::string	s_sMonsterImageFile;
		std::wstring	s_wsMonsterName;		// 디지몬 이름
	};

	struct sInfiniteWarInfo
	{
		sInfiniteWarInfo() : s_nWinPoint(0),s_nLosePoint(0)
		{}
		~sInfiniteWarInfo() {};

		int				s_nWinPoint;
		int				s_nLosePoint;
		sStageInfo		s_ClearInfo;		// 

		std::wstring const&	GetMonsterName() const;
		std::string const&	GetMonsterIconFile() const;
	};

	typedef std::map<int,sInfiniteWarInfo>					MAP_WARINFO;
	typedef std::map<int,sInfiniteWarInfo>::iterator		MAP_WARINFO_IT;
	typedef std::map<int,sInfiniteWarInfo>::const_iterator	MAP_WARINFO_CIT;

public:
	static int const IsContentsIdentity(void);

	enum eEvent{
		eUpdate_Stage_Info,
		eUpdate_Total_Point,
		eSet_Start_Timer,
	};

public:
	CInfiniteWarContents(void);
	virtual ~CInfiniteWarContents(void);

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

	void		OpenCloseStartWindow(bool bVisible);
	void		OpenCloseGiveupWindow(bool bVisible);
	void		OpenCloseNextStageTryWindow(bool bVisible);
	void		OpenCloseAllClearMessageWindow(bool bVisible);
	void		OpenCloseTimerWindow(bool bVisible);
	void		AskGiveup();

		
	DWORD const&	GetTotalHavePoint() const;

	int				GetLastClearStageIdx() const;
	int				GetNextChallengeStageIdx() const;
	int				GetNextStageWinPoint();
	int 			GetNextStageLosePoint();
	bool			IsStageAllClear();

	CsC_AvObject*	GetTargetNpc() const;
	//////////////////////////////////////////////////////////////////////////
	// Sender Function
	void			SendInfiniteWarGiveup();	
	void			SendInfiniteWarLeave();
	void			SendInfiniteWarTryNextStage_TargetNpc();
	void			SendInfiniteWarTryNextStage();
	//////////////////////////////////////////////////////////////////////////

	MAP_WARINFO const*		GetInfiniteWarTatalInfo();
	sInfiniteWarInfo const*	GetWarInfo( int const& nStageIdx );
	sStageInfo const*		GetStageInfo( int const& nStageIdx );

	bool		IsPartyMaster() const;
	std::wstring	GetReturnMapName();

private:
	void		_OpenWarStartWindow(void* pData);
	void		_OpenWarGiveUpWindow(void* pData);
	//////////////////////////////////////////////////////////////////////////
	// Recv Function
	void		_ReceChallengeStageClear(void* pData);
	void		_ReceChallengeNextStage(void* pData);
	void		_RecvChallengeGiveup(void* pData);
	void		_RecvMapMoveFalse(void* pData);
	//////////////////////////////////////////////////////////////////////////

	void		_InitData();
	void		_ClearData();
	void		_SetCurrentWarNpcIdx( DWORD const& nNpcIDX );// 현재 진행 중인 대전의 NPC 인덱스 번호 저장
	bool		_LoadInfiniteWarTableData( DWORD const& dwNpcIDX );
	void		_SetStageInfo( MAP_WARINFO& Continer, int const& nStageNumber, TB::sStageInfos const& tbStage );
	bool		_UpdateClearStageInfo( int const& nStageNumber, DWORD const& dwClearMonsterID );
	void		_Update_Point( DWORD const& dwTotalPoint, int const& nCurrentGetPoint );

private:
	int								m_nOpenWindowWaitForMonsterToDie;	// 몬스터가 필드에 없을 때까지 기다렸다가 뛰울 창 인덱스 번호를 넣는다.
	CsTimeSeq						m_nWaitTimer;
	CsC_AvObject*					m_pTarget;
	DWORD							m_dwTargetNpcIdx;

	DWORD							m_dwWarNpcIdx;//서버로 부터 받은 NPC 인덱스
	/*NPC Idx, 무한대전정보*/
	std::map<DWORD, MAP_WARINFO >	m_WarStageInfo;// NPC별 무한대전 정보

	DWORD							m_dwTotalHavePoint;			// 현재 획득한 총 포인트
	int								m_nNextChallengeStage;		// 도전하려는 스테이지 인덱스
	int								m_nLastClearStage;			// 클리어한 스테이지 인덱스
	bool							m_bBeChallenging;			// 무한대전 시작 중인지 체크

	TOwnershipGuard< AdaptPartySystem >	m_AdaptPartySystem;		// 파티 컨텐츠 데이터	
};
