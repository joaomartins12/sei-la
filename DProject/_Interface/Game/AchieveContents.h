//2017-04-20-nova
#pragma once
class AdaptDigimonArchiveSystem;

#define ACHIEVE_RANK		1500
#define ACHIEVE_TAMERRANK	1501
#define ACHIEVE_DIGIMONRANK	1502
#define ACHIEVE_DIGIMONSIZE	1503

class AchieveContents: public IUIContentSubject< AchieveContents >
{
public:
	enum eRank_Achieve
	{
		Rank_Size    = 0x01, // 크기랭커
		Rank_Tamer   = 0x02, // 테이머경험치랭커
		Rank_Digimon = 0x04, // 파트너몬경험치랭커
		Rank_Tactics = 0x08, // 용병디지몬경험치랭커
	};	

	enum eTYPE{
		CA_TLevel						=	100,
		CA_FriendCount					=	101,
		CA_GuildJoin					=	102,
		CA_InvenSlot_AllOpen			=	103,
		CA_WareHouse_AllOpen			=	104,
		CA_Money						=	105,

		CA_DLevel						=	200,
		CA_RideOpen						=	201,
		CA_GetTactics					=	202,
		CA_Tactics_Evo					=	205,
		CA_Partner_Evo_Mega				=	206,
		CA_Partner_Evo_Bust				=	207,
		CA_Partner_Evo_JointProgress	=	208,

#ifdef CROSSWARS_SYSTEM
		CA_Partner_Evo_CrossWars		=	209,
#endif

		CA_Equip_Costume_1				=	300,
		CA_Equip_Costume_2				=	301,
		CA_Equip_Costume_3				=	302,
		CA_Equip_Costume_4				=	303,
		CA_Equip_Costume_5				=	304,
		CA_Equip_Costume_6				=	305,
		CA_Equip_Costume_7				=	306,

		CA_MAP_1						=	400,
		CA_MAP_2						=	401,
		CA_MAP_3						=	402,
		CA_MAP_4						=	403,
		CA_MAP_5						=	404,
		CA_MAP_6						=	405,
		CA_MAP_ALL						=	406,

		CA_QuestCount					=	500,
		CA_QuestComp					=	501,

		CA_NpcTalk						=	600,	

		CA_ItemUse						=	700,	
		CA_Masters_Match				=	800,	
	};

public:
	AchieveContents(void);
	virtual ~AchieveContents(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void)							{ return true; }
	virtual void UnInitialize(void)							{}
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime)					{}
	virtual void NotifyEvent(int const& iNotifiedEvt)		{}
	virtual void MakeMainActorData(void)					{}	
	virtual void ClearMainActorData(void)					{}
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

	static int const IsContentsIdentity(void);

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream) {}	

private:	
	DWORD	m_nDispTitle;
	u4		m_nRankAchieve;
	int		m_nQuestCompCount;	

	std::map< DWORD, CsAchieve* >	m_mapComp;	//조건완료
	std::list< CsAchieve* >			m_listProc;	//조건미완료
	CsCoolTimeSeq					m_seqCheckAll;

	void	Set_DispTitle(void* pData);	
	void	Set_RankAchieve(void* pData);
	void	Set_CheckType( void* pData );

	void	Get_DispTitle(void* pData);

	void	DisplayInfo( CsAchieve::sINFO* pFTInfo );
	void	AchieveRank();
	bool	RemoveAchieveRank();
	void	CheckType( eTYPE nType, int nValue1 = 0, int nValue2 = 0 );
	void	CompAchieve( std::list< CsAchieve* >::iterator& it );

	bool	_IsOpenSlot( int nBaseDigimonID, int nSlot, int nLevel, cEvoUnit* pEvoUnit );	
	bool	_IsOpenMap( int* pArray, int nArrayCnt );
	bool	_IsOpenMap_All();
	int		_CalQuestCompCount();

public:	
	void	FirstPostLoad();	
	void	CheckAllAchieve();
	bool	IsEnableCheckAll();
	DWORD	GetDispTitle();

	std::map< DWORD, CsAchieve* >const*	GetCompMap()  const;
	std::list< CsAchieve* >const*		GetProcList() const;

	typedef	std::map< DWORD, CsAchieve* >::const_iterator	MAP_COMP_CIT;
	typedef	std::map< DWORD, CsAchieve* >const*				COMP_CIT;
	typedef	std::list< CsAchieve* >::const_iterator			MAP_LIST_CIT;
	typedef std::list< CsAchieve* >const*					LIST_CIT; 


	TOwnershipGuard< AdaptDigimonArchiveSystem >	m_AdaptDigimonAchive;
};
