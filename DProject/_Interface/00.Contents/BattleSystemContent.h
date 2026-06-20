#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptPartySystem;

// 배틀 신청 관련 데이터 모음.
namespace BRegist
{
	//////////////////////////////////////////////////////////////////////////
	// 배틀 등록 페이지 스텝
	//////////////////////////////////////////////////////////////////////////
	enum eRegistState			
	{
		eState_Type_Select,		// 배틀 타입 선택 페이지
		eState_Mode_Select,		// 배틀 모드 선택 페이지
	};
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 배틀 디지몬 3D 데이터
	//////////////////////////////////////////////////////////////////////////
	struct sDigimonData
	{
		sDigimonData();
		~sDigimonData();

		void MakeDigimon( unsigned int const& nFileTableIdx, // 디지몬 테이블 번호
						  NiPoint3 const& nPos,				 // 3D Map에 위치할 좌표
						  float const& fPosHeight );		 // 3D Map상에 위치할 높이값

		CsC_AvObject*	m_pObject;							// 디지몬 Object 
		std::string		m_DigimonImgFile;					// 디지몬 Icon 파일 이름
		std::wstring    m_DigimonTableName;					// 디지몬 테이블 이름
	};

	//* Key : 디지몬 테이블 번호, Value : 디지몬 데이터 */
	typedef std::map<unsigned int,sDigimonData>					MAP_DIGIMONINFO;
	typedef std::map<unsigned int,sDigimonData>::iterator		MAP_DIGIMONINFO_IT;
	typedef std::map<unsigned int,sDigimonData>::const_iterator	MAP_DIGIMONINFO_CIT;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 배틀 보상 데이터
	//////////////////////////////////////////////////////////////////////////
	struct sViewRewardItem
	{
		sViewRewardItem() : m_dwItemCode(0),m_dwItemCount(0)
		{};
		~sViewRewardItem() {};

		DWORD			m_dwItemCode;			// 보상 아이템 코드
		DWORD			m_dwItemCount;			// 지금 아이템 갯수
	};

	typedef std::list<sViewRewardItem>					REWARD_ITEM_CON;
	typedef std::list<sViewRewardItem>::iterator		REWARD_ITEM_CON_IT;
	typedef std::list<sViewRewardItem>::const_iterator	REWARD_ITEM_CON_CIT;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 배틀 등록 맵 데이터 
	//////////////////////////////////////////////////////////////////////////
	struct sMapInfos
	{
		friend struct sRegist;

	public:
		sMapInfos();
		~sMapInfos();

		unsigned int				GetNeedTicket() const;
		std::wstring				GetMapName() const;
		std::string					GetMapFileName() const;
		unsigned int				GetLimitDLv() const;
		unsigned int				GetLimitTicket() const;
		unsigned int				GetLimitPlayCount() const;
		DWORD						GetMapID() const;
		REWARD_ITEM_CON	const&		GetViewWinRewardItems() const;
		REWARD_ITEM_CON	const&		GetViewLoseRewardItems() const;
		void						SetPlayCount( unsigned int const& nTotalCount );
		unsigned int				GetPlayCount() const;
		bool						IsEnablePlayCount() const;
		void						SetRewardPoint(int const& winPoint, int const& losePoint);

	//protected:
		void						SetBattleMap( DWORD const& dwMapID, std::string const& imgFileName );
		void						SetLimitInfo( unsigned int const& nDlv, 
												  unsigned int const& nTicket, 
												  unsigned int const& nJoinCount );		

	protected:
		DWORD						m_dwMapID;			// 배틀 맵 ID
		std::wstring				m_wcMapName;		// 배틀 맵 이름
		std::string					m_cMapFileName;		// 맵 파일 이름.

		unsigned int				m_nLimitDLv;		// 디지몬 참가 최소 레벨
		unsigned int				m_nNeedTicket;		// 배틀 모드에 참가 신청 할 때 필요한 티켓 갯수
		unsigned int				m_nLimitPlayCount;	// 최대 참여 제한 수
		unsigned int				m_nPlayCount;		// 참여 횟수

		int							m_WinRankPoint;		// 이겼을 때 주어지는 포인트
		int							m_LoseRankPoint;	// 졌을 때 주어지는 포인트

		REWARD_ITEM_CON				m_ViewWinRewardItems;	// 이겼을 때 보상 아이템 목록
		REWARD_ITEM_CON				m_ViewLoseRewardItems;	// 졌을 때 보상 아이템 목록
	};

	typedef std::list<sMapInfos>									MAP_INFO_LIST;
	typedef std::list<sMapInfos>::iterator							MAP_INFO_LIST_IT;
	typedef std::list<sMapInfos>::const_iterator					MAP_INFO_LIST_CIT;

	/** Battle Mode : 1 = 1vs1, 2 = 2vs2, 3 = 3vs3, 4 = 4vs4, 5 = 8vs8, 6 16vs16 */
	typedef std::map<unsigned int/* Battle Mode */,MAP_INFO_LIST>	MAP_MODE_INFO;
	typedef std::map<unsigned int,MAP_INFO_LIST>::iterator			MAP_MODE_INFO_IT;
	typedef std::map<unsigned int,MAP_INFO_LIST>::const_iterator	MAP_MODE_INFO_CIT;

	/** Battle Type : 1 = 팀 대전, 2 = ... */
	typedef std::map<unsigned int/*Battle Type*/,MAP_MODE_INFO>		MAP_TYPE_INFO;
	typedef std::map<unsigned int,MAP_MODE_INFO>::iterator			MAP_TYPE_INFO_IT;
	typedef std::map<unsigned int,MAP_MODE_INFO>::const_iterator	MAP_TYPE_INFO_CIT;

	// 배틀 등록 관련 데이터 모음.
	struct sRegist
	{
	public:
		sRegist();
		~sRegist();

		bool							LoadRegistData();	// 배틀 신청 데이터 로드
		void							SetModePlayCount(std::map<char,int> const& playCountInfo);
		void							ClearData();		// 배틀 신청 데이터 모두 제거
		void							ResetSelectedInfo();

		void							SetTargetNpdID(DWORD const& dwNpcIDX);	// 배틀 신청 NPC IDX 저장, 배틀 등록시 서버에 보냄.

		bool							LoadRegistMap(std::string const& loadFileName);	// 배틀 신청 페이지 3D Map 로드
		void							LoadBattleInfoFromTable();			// 배틀 신청 테이블 데이터 로드
		void							LoadDigimon( NiPoint3 const& position, float const& fHeight, int const& nSeasonNumber );// 배틀 등록 가능 디지몬들의 3D Object 로드
		void							LoadRewardItems( int const& nRewardTableIdx, // 배틀 보상 아이템 목록을 배틀 테이블에서 읽어 컨테이너에 넣음.
														 REWARD_ITEM_CON& container );

		void							Add3D_Digimon( MAP_DIGIMONINFO & continer, NiPoint3 const& position, float const& fHeight, DWORD const& dwDigimonIDX );
		void							DeleteDigimonInfo( MAP_DIGIMONINFO & continer);// 컨테이너에 있는 디지몬 3D Object를 제거

		bool							SendBattleRegist();	// 서버에 배틀 신청 패킷을 보낸다

		void							GetTypeList( std::list<unsigned int> & listCon ) const;
		void							SetSelectType(unsigned int const& nType);

		void							RenderBackgroundMap();
		void							UpdateBackgroundMap(float const& fAccumTime);
		void							UpdateOjbect( float const& fElapsedTime );

		bool							ChangeNextStep();
		bool							ChangeBackStep();
		
		bool							InitModeSelectInfo(unsigned int const& nBattleType);
		void							_SelectedMyDigimon();
		void							_ResetDigimonRote( float const& fRot );

		eRegistState					GetCurrentStep() const;

		bool							IsEnoughTicket( unsigned int const& unHaveTicketCount) const;
		bool							IsEnoughPlayCount() const;
		bool							IsEnoughDigimonType( DWORD const& dwDigimonIDX ) const;
		bool							IsEnoughDigimonLevel( int const& nDigimonLv ) const;
		
		MAP_DIGIMONINFO const&			GetDigimonList() const;
		sMapInfos const*				GetCurrentBattleInfo() const;
		unsigned int					GetSelectType() const;
		unsigned int					GetSelectMode() const;
		DWORD							GetSelectMapIDX() const;

		bool							CurrentModeDown();
		bool							CurrentModeUp();

	private:
		eRegistState					m_nCurrentState;	// 현재 등록 페이지 상태
		MAP_TYPE_INFO					m_BattleInfos;		// 배틀 정보를 가지고 있는 컨테이너

		MAP_DIGIMONINFO					m_RegistedDigimons;		// 배틀 등록 가능한 디지몬 3D Object 
		MAP_DIGIMONINFO					m_BattlenMyDigimonInfo;	// 현재 내 디지몬의 3D Object
		CsC_AvObject*					m_pRenderDigimon;		// 배틀 등록 페이지에서 보여질 디지몬 3D Object 
		CsNodeObj						m_BattleRegiestMap;		// 배틀 등록 페이지 3D Map 데이터

		NiPoint3						m_vCharPos;				// 3D Map 상에 디지몬이 위치할 좌표값
		float							m_fBoxHeight;			// 3D Map 상에 디지몬이 위치의 디지몬 사이즈의 높이값

		DWORD							m_dwRegistNpcIDX;		/** 배틀 신청 NPC IDX */
		unsigned int					m_nSelectType;			/** Battle Type : 1 = 팀 대전, 2 = ... */
		unsigned int					m_nSelectMode;			/** Battle Mode  : 1 = 1vs1, 2 = 2vs2, 3 = 3vs3, 4 = 4vs4, 5 = 8vs8, 6 16vs16 */
		DWORD							m_dwSelectMap;			/** 선택한 맵 IDX */
		sMapInfos const*				m_pSelectedBattleInfo;	// 현재 선택된 배틀 정보
	};
}

namespace BWait
{
	// 배틀 준비 상태인 플레이어 정보
	struct sWait
	{
	public:
		sWait();
		~sWait();
		//////////////////////////////////////////////////////////////////////////
		// 배틀에 참여한 플레이어 정보
		//////////////////////////////////////////////////////////////////////////
		struct sPlayerInfo
		{
			sPlayerInfo():m_uTamerUIDX(0),m_nDigimonTableIdx(0),m_bIsMe(false),m_nbReady(false)
			{};

			~sPlayerInfo() {};

			bool			m_bIsMe;				// 내 캐릭터 인가?
			std::wstring	m_PlayerName;			// 테이머 이름
			std::wstring	m_DigimonName;			// 디지몬 이름

			int				m_uTamerUIDX;			// 테이머 UIDX
			int				m_nDigimonTableIdx;		// 디지몬 테이블 ID
			std::string		m_DigimonIconFileName;	// 디지몬 아이콘 파일
			bool			m_nbReady;				// 배틀 준비를 했는지 여부
		};

		typedef std::list<sPlayerInfo>					PLAYER_INFO;
		typedef std::list<sPlayerInfo>::iterator		PLAYER_INFO_IT;
		typedef std::list<sPlayerInfo>::const_iterator	PLAYER_INFO_CIT;

		typedef std::map<int/*팀 코드*/, PLAYER_INFO>					WAIT_PLAYER_LIST_CON;
		typedef std::map<int/*팀 코드*/, PLAYER_INFO>::iterator			WAIT_PLAYER_LIST_CON_IT;
		typedef std::map<int/*팀 코드*/, PLAYER_INFO>::const_iterator	WAIT_PLAYER_LIST_CON_CIT;

		void						ClearData();

		bool						IsBattleRegisted() const;// 배틀 신청한 상태인지 체크
		unsigned int				GetBattleUIDX() const;
		unsigned int				GetBattleType() const;
		unsigned int				GetBattleMode() const;
		char						GetMyTeamCode() const;

		bool						SendRegistReady();
		bool						SendRegistCancel();
		void						SetBattleWaitInfo(  u1 const& battleType, u1 const& battleMode, u4 const& mapID );
		void						RecvSetWaitInfo( u4 const& battleUIDX, u4 const& mapID, u1 const& battleType, u1 const& battleMode );
		void						SetWaitPlayerInfo( std::map<n1, std::list<sBattlePlayerInfo>> const& playerlist );
		bool						ChangeReadyState( DWORD const& dwPlayerIDX, bool bReady );
		
		sPlayerInfo*				_FindPlayerInfo( DWORD const& dwPlayerIDX );

		WAIT_PLAYER_LIST_CON const& GetWaitPlayerList() const;

	private:
		bool						m_bRegisted;			/** 배틀 신청 중*/
		unsigned int				m_nBattleUIDX;			// 배틀 고유 번호
		unsigned int				m_nBattleType;			
		unsigned int				m_nBattleMode;
		DWORD						m_ndwBattleMapId;
		char						m_nMyTeamCode;

		WAIT_PLAYER_LIST_CON		m_WaitPlayers;
	};
}

// 배틀 중일 때 필요한 데이터 모음.
namespace BPlaying
{
	//////////////////////////////////////////////////////////////////////////
	// 배틀 시작 및 진행 시간을 서버로 부터 받아 저장하는 구조체
	//////////////////////////////////////////////////////////////////////////
	struct sTimerInfo
	{
		sTimerInfo():m_nStart(0),m_nEnd(0),m_nRound(0)
		{}
		~sTimerInfo() {}

		u4 GetStartTime() const
		{
			return m_nEnd - m_nRound - m_nStart;
		}
		void Reset()
		{
			m_nStart = 0;
			m_nRound = 0;
			m_nEnd = 0;
		}

		u4	m_nStart;	// 경기 시작 시간
		u4	m_nEnd;		// 경기 종료 시간
		u4	m_nRound;	// 경기 진행 시간		
	};

	struct sBattlePlaying
	{
		sBattlePlaying() :m_nBattleUIDX(0),m_cTeamCode(0)
		{};
		~sBattlePlaying() {};

		bool			IsMyTeam( unsigned int const& nTeamCode ) const;
		void			ResetData();
		void			CopyWaitInfo( BWait::sWait const& data );
		void			SetBattleTimerInfo( unsigned int const& start, unsigned int const& end, unsigned int const& round );
		unsigned int	GetStartTime() const;
		unsigned int	GetBattleRoundTime() const;
		char			GetMyTeamCode() const;
		
	private:
		sTimerInfo					m_TimerInfo;
		char						m_cTeamCode;
		unsigned	int				m_nBattleUIDX;			// 배틀 고유 번호		
	};
}

// 배틀 결과 관련 데이터 모음.
namespace BResult
{
	//////////////////////////////////////////////////////////////////////////
	// 배틀 종료 후 받게될 보상 데이터
	//////////////////////////////////////////////////////////////////////////
	enum eResultType
	{
		LOSE = -1,
		DRAW = 0,
		WIN = 1,
	};

	struct sResultInfo
	{
	public:
		sResultInfo();
		~sResultInfo();

		struct sReward
		{
			sReward() : m_dwItemCode(0),m_dwItemCount(0)
			{};
			~sReward() {};

			DWORD			m_dwItemCode;			// 보상 아이템 코드
			DWORD			m_dwItemCount;			// 지금 아이템 갯수
			std::wstring	m_wsItemName;			// 아이템 이름
		};

		typedef std::list<sReward>					RESULT_REWARD_ITEM_CON;
		typedef std::list<sReward>::iterator		RESULT_REWARD_ITEM_CON_IT;
		typedef std::list<sReward>::const_iterator	RESULT_REWARD_ITEM_CON_CIT;
		//////////////////////////////////////////////////////////////////////////


		bool						LoadResultData();
		void						ClearData();
		bool						LoadResultMap(std::string const& loadFileName);	// 배틀 결과 페이지 3D Map 로드
		void						RenderBackgroundMap();
		void						UpdateBackgroundMap(float const& fAccumTime);
		void						UpdateResultObject(float const& fElapsedTime);
		void						SetBattleResult(char const& cMyTeamCode, char const& cWinTeamCode);
		eResultType					GetMyResultValue() const;

		bool						IsRecvedRewardData() const;
		void						SetRewardItems( std::list<sRewardItemInfo> const& itemList, int const& nGetPoint );
		RESULT_REWARD_ITEM_CON const&	GetRewardItems() const;

	private:
		NiPoint3					m_vCharPos;				// 3D Map 상에 디지몬이 위치할 좌표값
		float						m_fBoxHeight;			// 3D Map 상에 디지몬이 위치의 디지몬 사이즈의 높이값
		CsNodeObj					m_BattleResultMap;		// 배틀 결과 페이지 3D Map 데이터
		CsC_AvObject*				m_pRenderDigimon;		// 배틀 등록 페이지에서 보여질 디지몬 3D Object 
		CsC_AvObject*				m_pRenderTamer;			// 배틀 등록 페이지에서 보여질 테이머 3D Object 
		char						m_cWinTeamCode;
		char						m_cMyTeamCode;
		RESULT_REWARD_ITEM_CON		m_RewardItems;			// 배틀 종료로 받게될 아이템 목록
		bool						m_bRecvedRewardData;	// 보상 패킷을 받았는지 체크
		int							m_bGetPoint;			// 보상 패킷을 받았는지 체크
	};
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CBattleSystemContents
	: public IUIContentSubject< CBattleSystemContents >
{
	struct sMyBattleInfo
	{
		struct sModeUserData	// 모드별 유저 데이터
		{
			sModeUserData():m_nTotalPlayCount(0),m_nTotalRankPoint(0)
			{}
			~sModeUserData() {}

			int			m_nTotalPlayCount;	// 해당 모드 플레이 횟수
			__int64		m_nTotalRankPoint;		// 해당 모드 Ranking Point
		};
		sMyBattleInfo();
		~sMyBattleInfo();

		void			ResetData();
		void			AddBattleTicketPoint( DWORD const& addPoint );
		void			SetRecved();
		void			SetBattleTicketPoint(__int64 const& nCount);
		void			UpdatePlayCount( char const& cMode, int const& nPlayCount );
		__int64			GetBattleTicketPoint() const;
		bool			IsRecvData() const;
		std::map<char, int> GetModePlayCount() const;
		void			SumRankPoint( char const& cMode, __int64 const& nSumRankPoint );
		void			SetBattleTypeUserData( std::map<char, sMainTypeData> const& mapUserData );

	private:
		sModeUserData *	GetUserData( char const& cMode );

	private:
		std::map<char, sModeUserData>	m_modeUserData;				// 모드별 유저 데이터
		__int64							m_nHaveBattleTicketPoint;	// 보유 하고 있는 배틀 티켓 수		
		bool							m_bRecvData;				// 서버로부터 데이터를 받았는가?
	};	// 배틀 신청에 관련된 데이터 저장

public:
	static int const IsContentsIdentity(void);

public:
	CBattleSystemContents(void);
	virtual ~CBattleSystemContents(void);

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

	//////////////////////////////////////////////////////////////////////////
	// 배틀 등록 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:
	enum eEvent
	{
		eChange_State,			// 배틀 등록 페이지 변경시 호출 이벤트 키
		eChange_Mode,			// 배틀 등록 페이지의 배틀 모드 변경시 호틀 이벤트 키
		eAsk_BattleRequest,		// 배틀 진입요청 
		eMy_Ready_Change,		// 내가 수락/거절 했을 때
		eOther_Ready_Change,	// 다른 플레이어가 수락/거절했을 때
		eRegistFales,			// 배틀 등록 실패
		eBattleMatchCancel,		// 다른 플레이어 및 내가 취소시 
		eBattleReadyTimerStart,
		eBattleRoundStart,			
		eBattleRoundEnd,
	};

	void						CloseRegisterPage();

	bool						Send_BattleMatch_Regist();			// 배틀 신청 패킷 보냄

	void						SetCurrentModeDown();				// 배틀 신청시 현재 모드(1Vs1, 2Vs2.. )타입을 다운 시킨다.
	void						SetCurrentModeUp();					// 배틀 신청시 현재 모드(1Vs1, 2Vs2.. )타입을 업 시킨다.
	unsigned	int				GetHaveBattleTicketPoint() const;	// 현재 내가 보유한 배틀 티켓 갯수

	void						StartRegistPage();
	void						EndRegistPage();

	void						RenderRegistBackMap();	// 배틀 등록 페이지의 3D Map 렌더링
	void						UpdateRegistBackMap(float const& fAccumTime);
	void						UpdateRegistObject(float const& fElapsedTime);


	void						GetBattleTypeList( std::list<unsigned int>& listType ) const;	// 배틀 타입 목록만 가져 온다.(1 : 팀전, 2 : ...)
	void						SetBattleTypeSelect(unsigned int const& nSelectType);			// 배틀 타입 저장
	void						RegisterNextStep();
	void						RegisterBackStep();
	void						GotoBack();

	BRegist::MAP_DIGIMONINFO const & GetRegistableDigimonList() const;
	BRegist::sMapInfos const*	GetCurrentReigistBattleInfo() const;
	unsigned int				GetSelectedType() const;
	unsigned int				GetSelectedMode() const;
	bool						IsEnoughDigimonLv() const;
	bool						IsEnoughDigimon() const;
	bool						IsJoinBattleHaveDigimon() const;		// 배틀 신청 가능한 디지몬을 가지고 있는지 여부

private:
	void						_OpenRegisterPage(void* pData);		// 배틀 신청 페이지 열기
	bool						_CheckRegistEnable() const;			// 배틀 신청 패킷 보내기전 신청 가능 상태인지 체크 함수
	void						_SetCameraInfo();						// 배틀 맵 및 결과 페이지 진입시 카메라 셋팅
	void						_ResetCameraInfo();						// 배틀 맵 및 결과 페이지 삭제시 카메라 셋팅

	//************************************************************************/
	// 현재 사용 안함.
	//************************************************************************/
// 	void						SelectedDigimon(unsigned int const& nDigimonIdx);	// 배틀 등록 페이지의 디지몬 선택 변경
// 	void						DigimonRote(float const& fRot);						// 디지몬 회전
// 	BRegist::MAP_DIGIMONINFO const&		GetDigimonInfo() const;						// 배틀 등록 페이지의 선택된 디지몬의 정보
	//************************************************************************/
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 배틀 공통 함수
	//////////////////////////////////////////////////////////////////////////
public:
	int							GetMyDigimonLv() const;		// 현재 내 디지몬의 레벨
	DWORD						GetMyDigimonBaseFTID() const;	// 현재 내 디지몬의 테이블 IDX
	std::wstring 				GetMyDigimonName() const;		// 현재 내 디지몬의 이름
	std::wstring				GetMyDigimonTableName() const;	// 현재 내 디지몬 테이블 이름
	bool						IsMyBattleTeam( int const& nCheckTeamCode )const;
	static std::wstring			GetDigimonExplainName( DWORD const& dwDigimonTbID );

private:
	bool						_SendMyBattleInfoRequest();	// 서버에 내 배틀 정보를 요청 함.
	void						_RecvMyBattleInfoRequest(void*pData);	// 서버에 내 배틀 정보를 요청 함.
	void						_Recv_AddBattlePoint(void* pData);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 배틀 등록 후 대기 상태 관련 함
	//////////////////////////////////////////////////////////////////////////
public:
	void						ToggleWaitingWindow();		// 배틀 대기 상태 UI 토글

	bool						Send_Regist_Ready();					// 배틀 준비 상태 수락
	bool						Send_Regist_Cancel();					// 배틀 등록 취소 패킷을 보냄
	bool						IsBattleMatchRegisted() const;			// 배틀 매칭 신청 중인가?
	void						Get_IsRegisted(void* pData);			// 배틀 매칭 신청 중인가?	
	void						Get_IsMyBattleTeam(void* pData);		// 배틀 팀이 나와 같은가?
	void						Get_IsBattlePlaying(void* pData);				// 배틀 중인가?
	bool						IsBattlePlaying() const;				// 배틀 중인가

	unsigned int				GetRegistedBattleType() const;
	unsigned int				GetRegistedBattleMode() const;
	BWait::sWait::WAIT_PLAYER_LIST_CON const& GetWaitPlayerInfo() const;
	bool						GetBattleLoadingInfo( BWait::sWait::WAIT_PLAYER_LIST_CON& info ) const;
	void						GetBattleRegistInfo( char & TeamCode, unsigned int & nBattleType, unsigned int & nBattleMode ) const;

private:
	void						Recv_BattleMatch_Regist(void* pData);	// 배틀 신청 결과값을 받는다.
	void						Recv_Regist_Cancel(void* pData);		// 배틀 신청 취소 결과값을 받음.
	void						Recv_RegistReadyState(void* pData);		// 내가 보낸 배틀 준비or취소 상태 패킷
	void						Recv_Match_Cancel(void* pData);			// 배틀 매칭이 취소 瑛?때 날라옴.
	void						NTF_RegistReadyState(void* pData);		// 내가 아닌 다른 플레이어들의 준비 상태 패킷
	void						NTF_BattleReadyRequest(void* pData);	// 배틀 매칭 완료시 플레이어들의 매칭 수락 요청 
	void						NTF_BattleMatchComplete(void* pData);	// 배틀 매칭이 완료 된 후 날라오는 패킷, 맵 로딩 준비 시작함

	//////////////////////////////////////////////////////////////////////////
	//	배틀 중일 때 사용되는 함수
	//////////////////////////////////////////////////////////////////////////
public:
	void						OnBattleStart();

private:	
	void						_BattleMapDoorOpen();

	//////////////////////////////////////////////////////////////////////////
	//	배틀 시작/종료 관련 함수
	//////////////////////////////////////////////////////////////////////////
private:
	void 						NTF_Battle_Start(void* pData);
	void 						NTF_Battle_End(void* pData);
	void 						NTF_Battle_Rewards(void* pData);

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 배틀 결과 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:
	void						CloseResultPage();		// 배틀 결과 페이지 닫기
	void						DeleteResultData();		// 배틀 결과 데이터 삭제

	void						RenderResultBackMap();	// 배틀 결과 페이지의 3D Map 렌더링
	void						UpdateResultBackMap(float const& fAccumTime);
	void						UpdateResultObject(float const& fElapsedTime);

	void						StartResultPage();
	void						EndResultPage();
	BResult::eResultType		GetMyResult() const;
	BResult::sResultInfo::RESULT_REWARD_ITEM_CON const&	GetRewardItems() const;

	void						OpenResultPage();		// 배틀 결과 페이지 열기


	//////////////////////////////////////////////////////////////////////////
private:
	BRegist::sRegist			m_sRegistInfo;		// 배틀 등록 관련 데이터
	BWait::sWait				m_sWaitInfo;		// 배틀 등록 후 대기 상태 정보
	BPlaying::sBattlePlaying	m_sPlayBattleInfo;	// 배틀 중 배틀 데이터
	BResult::sResultInfo		m_sResultInfo;		// 배틀 종료 관련 데이터

	sMyBattleInfo				m_sMyBattleInfo;	// 내 배틀 데이터 정보

	float						m_fBackupDistMin;			// 배틀 맵 진입전 카메라 거리 값 저장 변수
	float						m_fBackupDistMax;			// 배틀 맵 진입전 카메라 거리 값 저장 변수

	TOwnershipGuard< AdaptPartySystem >	m_AdaptPartySystem;			// 파티 컨텐츠 데이터
};
