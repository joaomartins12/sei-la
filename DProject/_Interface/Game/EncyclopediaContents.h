#pragma once

class EncyclopediaContents : public IUIContentSubject< EncyclopediaContents >
{
public:
	struct sINFO
	{
		sINFO();

		int			s_nDigimonID;						// 디지몬 ID
		USHORT		s_nEvolSlot;						// 진화체 슬롯 번호( 해당 계열에 몇번째 진화체인지)

		enum eSTAT{ S_OPEN = 0, S_CLOSE = 2, S_NONE, };
		eSTAT		s_eImgState;						// 진화 이미지 상태 - S_OPEN : Open, S_CLOSE : Close, S_NONE : unuse
		CsRect		s_rcIcon;

		std::string	s_ImgFileName;
	};

	struct sEVOL_INFO
	{
		sEVOL_INFO();
		sINFO			s_sInfo[ nLimit::EvoUnit ];	// 디지몬 한 계열 정보
		UCHAR			s_nGroup;					// 디지몬 계열체 이름 정렬 그룹( 가~나 : 0, 다~라 : 1, 마~바 : 2, 사~아 : 3, 자~차 : 4, 카~타 : 5, 파~하 : 6 )
		UCHAR			s_nOrder;					// 디지몬 계열체 이름 정렬 번호
		USHORT			s_nSize;					// 도감용 사이즈
		UCHAR			s_nEnchant;					// 도감용 강화 ( 0 ~ 4 )
		UCHAR			s_nEnchant_AT;				// 도감용 강화 :: AT
		UCHAR			s_nEnchant_CT;				// 도감용 강화 :: CT
		UCHAR			s_nEnchant_BL;				// 도감용 강화 :: BL
		UCHAR			s_nEnchant_EV;				// 도감용 강화 :: EV
		UCHAR			s_nEnchant_HP;				// 도감용 강화 :: HP
		UCHAR			s_nLevel;					// 도감통계용 디지몬 레벨
		UCHAR			s_nCount;					// 해당 계열체에서 사용하고 있는 디지몬 수(비활성 디지몬 제외)
		bool			s_bIsAllOpen;				// 해당 계열체 모두 오픈 되었는지, 배경을 바꿔야하기때문에 필요함.. 클라 안에서 계산하자
		bool			s_bIsReward;				// 해당 계열체 완성 보상을 획득 하였는지 여부, true : 아이템 받음 false : 아직 수령 안함
		std::wstring	s_cDigimonName;
	};

	struct sDATA
	{
		sDATA();

		DWORD		s_dwBaseDigimonID;			//계열체 번호
		USHORT		s_nEvolslot;				//진화슬롯 번호
		DWORD		s_dwDestDigimonID;			//초상화 보여줄 디지몬
		bool		s_bIsOpen;					//해당 디지몬 오픈 여부
		bool		s_bIsRender;				//디지몬 랜더 여부( 중복 디지몬일 경우 랜더 안함)

		std::string	s_ImgFileName;
	};	

	struct sGROUP_INFO	// 덱별 옵션 정보
	{
		sGROUP_INFO(){  }

		USHORT		 s_nGroupIdx;				//그룹번호			
		bool		 s_bGroupOpen;				//덱 오픈 여부
		bool		 s_bUse;						//덱 효과 사용 여부
		UCHAR		 s_cRanderCnt;				//실제 랜더 디지몬 카운트

		USHORT		 s_nCondition[ ENCY_MAX_OPTION ];//옵션 발동 조건 ( 1:패시브, 2:확률발동, 3:확률발동+지속시간 )
		USHORT		 s_nAT_Type[ ENCY_MAX_OPTION ];	//발동 효과 체크 타입 ( 0:사용안함-패시브 1:일반공격시 , 2:스킬공격시 )
		USHORT		 s_nOption[ ENCY_MAX_OPTION ];	//옵션		## 옵션 : 수치 증가( 1:공격력 2:HP 3:스킬피해 4:크리티컬 )	##
		USHORT		 s_nVal[ ENCY_MAX_OPTION ];		//옵션값	## 옵션 : 확률발동, ( 5: 스킬 쿨타임 초기화 )				##
		UINT		 s_nProb[ ENCY_MAX_OPTION ];		//확률		## 옵션 : 확률발동, ( 6: 일정시간 공속증가 )				##
		UINT		 s_nTime[ ENCY_MAX_OPTION ];		//적용시간(초)
		std::wstring s_GroupName;

		std::list< sDATA* >		s_pListData;	//해당 덱에 속한 디지몬 정보
	};	

public:
	enum eEvent
	{
		ENCYCLOPEDIA_REFRESHLIST,
		ENCYCLOPEDIA_USEDECK
	};

	/************************************************************************/
	/*                              덱 정보                                 */
	/************************************************************************/
	enum eDECK_CONDITION	// 덱 효과 버프 타입
	{	
		COND_NONE			=	0,		// 사용 안함
		COND_PASSIVE		=	1,		// 패시브
		COND_ACTIVE			=	2,		// 확률 발동
		COND_ACTIVE_TIME	=	3,		// 확률 발동 - 일정시간 효과
		COND_MAX,
	};
	enum eDECK_CHECKTYPE	// 확률 발동 버프 - 발동 체크
	{	
		NONE				=	0,		// 사용 안함 - 패시브일 경우
		ATK_NORMAL			=	1,		// 일반 공격
		ATK_SKILL			=	2,		// 스킬 공격
		CHECKTYPE_MAX,
	};

	enum eDECK_OPTION		// 옵션 종류
	{	
		DECK_NONE			=	0,		// 사용 안함
		DECK_ATTACK_UP		=   1,		// 공격력 증가
		DECK_SKILL_UP		=	2,		// 스킬데미지 증가
		DECK_CRIT_UP		=	3,		// 크리티컬 증가
		DECK_SKILL_COOL		=	4,		// 쿨타임 초기화
		DECK_HP_UP			=	5,		// 체력 증가
		DECK_ATK_SPEED		=	6,		// 공속증가
		DECK_MAX,
	};

public:
	EncyclopediaContents(void);
	virtual ~EncyclopediaContents(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void)							{ return true; }
	virtual void UnInitialize(void)							{}
	virtual bool IntraConnection(ContentsSystem& System)	{ return true; }
	virtual void Update(float const& fElapsedTime)					{}
	virtual void NotifyEvent(int const& iNotifiedEvt)		{}
	virtual void MakeMainActorData(void)					{}	
	virtual void ClearMainActorData(void)					{}
	virtual void MakeWorldData(void)						{}
	virtual void ClearWorldData(void)						{}

	static int const IsContentsIdentity(void);

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream) {}	

private:
	int		mUseDeck;	// 선택한 덱( 사용 안할 땐 INT_MIN )
	int		iDeckIdx;	// 2017-05-29-nova 툴팁에 사용하는 idx
	bool	mbIsRecv;	// 도감 목록 최초 서버 수신 셋팅

	std::map<std::wstring,std::wstring>		mGroupStr;
	std::map< int, sEVOL_INFO* >			mEncyInfoMap;		// 디지몬 계열체 맵
	std::map< int, sGROUP_INFO* >			mGroupInfoMap;		// 그룹 완성 정보
	
	void Recv_OpenData(void* pData);
	void Recv_UseDeck(void* pData);
	void Recv_GetDeck(void* pData);
	void Recv_SetEvolInfo(void* pData);
	void Recv_Server_Encyclopedia(void* pData);
	void Recv_InchantResult(void* pData);
	void Recv_GroupInfoCondition(void* pData);
	void Recv_LevelCheckUpdate(void* pData);
	void Recv_EvolutionUpdate(void* pData);
	void Recv_BisReward(void* pData);
	void Recv_Scale(void* pData);
	void Recv_LogOut(void* pData);

	void Get_UseDeck(void* pData);
	void Get_BisRecv(void* pData);
	void Get_GroupInfoNullCheck(void* pData);
	void Get_GroupInfoCondition(void* pData);
	void Get_GroupInfoNatType(void* pData);
	void Get_GroupInfoProb(void* pData);
	void Get_GroupInfoOption(void* pData);
	void Get_GroupInfoVal(void* pData);
	void Get_GroupInfoTime(void* pData);

	void SetEncyInfoMap();
	void SetGroupInfoMap();	
	void SetUseDeck( int nDeckIdx, bool bUse );
	void MakeGroupString( std::map<std::wstring,std::wstring>& wsGroupStr );
	
	int	 GetGroup_DigimonName( std::map<std::wstring,std::wstring> const& wsPP, int nDigimonID );
	int  GetIdx_DigimonName( int nDigimonID );
	int  GetEnchant_Val( int nEnchant_AT, int nEnchant_CT, int nEnchant_EV, int nEnchant_BL, int nEnchant_HP );	 // 강화 수치 높은거 두개로 강화 인덱스 return
	
	bool CompEnchant_Val( int nDigimonID, int nEnchant_AT, int nEnchant_CT, int nEnchant_EV, int nEnchant_BL, int nEnchant_HP );	// 현재 강화수치와 새로 변화된 강화수치 비교

	EncyclopediaContents::sEVOL_INFO* GetEncyclopediaEvolInfo( DWORD const& dwBaseDigimonID );		
	
	typedef std::list< sDATA* >::iterator			MAP_DATA_IT;
	typedef	std::map< int, sGROUP_INFO* >::iterator	MAP_GROUP_IT;

public:
	int	GetUseDeck();
	int	GetScaleVal( int nScale );	
	int	GetEnchant_Order( int nOrder, int& nEnchant_AT, int& nEnchant_CT, int& nEnchant_EV, int& nEnchant_BL, int& nEnchant_HP );	// 해당번째 높은 강화 수치 종류 return ( 0 : AT, 1 : CT, 2 : EV, 3 : BL )

	sEVOL_INFO* GetEncyData(int num) const;

	std::map< int, sEVOL_INFO* > const&			GetEncyMapData()  const;
	std::map< int, sGROUP_INFO* > const&		GetGroupMapData() const; 
	std::map<std::wstring,std::wstring> const&	GetGroupString()  const;

	typedef	std::map< int, sEVOL_INFO* >::iterator					MAP_IT;
	typedef	std::map< int, sEVOL_INFO* >::const_iterator			MAP_CIT;	
	typedef std::list< sDATA* >::const_iterator						MAP_DATA_CIT;		
	typedef	std::map< int, sGROUP_INFO* >::const_iterator			MAP_GROUP_CIT;

	typedef	std::map< int, sEVOL_INFO* >::const_reverse_iterator	MAP_CIT_R;
	typedef	std::map< int, sGROUP_INFO* >::const_reverse_iterator	MAP_GROUP_CIT_R;
};