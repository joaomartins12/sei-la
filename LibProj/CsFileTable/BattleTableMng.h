
#pragma once 

struct sBattleMode
{
	sBattleMode():m_bActive(false), m_VsType(0), m_dwMapID(0), m_nLimitDLv(0),
				  m_nLimitJoinCount(0), m_nNeedTicket(0),m_WinRankPoint(0), m_LoseRankPoint(0),
				  m_WinRewardTalbeIdx(0),m_LoseRewardTalbeIdx(0)
	{}
	~sBattleMode(){};

	bool		m_bActive;	// 배틀모드가 활성화 되어 있는지 여부. true : 배틀 신청 가능, false : 배틀 신청 불가
	int			m_VsType;	// 1 : 1 Vs 1, 2 : 2 Vs 2, 3 : 3 Vs 3, 4 : 4 Vs 4 , 5 : 8 Vs 8, 6 : 16 Vs 16
	DWORD		m_dwMapID;	// 배틀 맵 ID
	std::string	m_strMapImgFile;	// 배틀 맵 UI 파일 이미지

	unsigned int m_nLimitDLv;	// 디지몬 참가 최소 레벨
	unsigned int m_nNeedTicket;	// 배틀 모드에 참가 신청 할 때 필요한 티켓 갯수
	unsigned int m_nLimitJoinCount;	// 최대 입장 가능 횟수

	int		m_WinRankPoint;		// 이겼을 때 주어지는 포인트
	int		m_LoseRankPoint;	// 졌을 때 주어지는 포인트
	int		m_WinRewardTalbeIdx;
	int		m_LoseRewardTalbeIdx;
};

typedef std::list<sBattleMode>					TB_BATTLE_MODE_INFO;
typedef std::list<sBattleMode>::iterator		TB_BATTLE_MODE_INFO_IT;
typedef std::list<sBattleMode>::const_iterator	TB_BATTLE_MODE_INFO_CIT;

typedef std::map<int/*Battle Mode Type*/, TB_BATTLE_MODE_INFO>					TB_BATTLE_TALBE_INFO;
typedef std::map<int/*Battle Mode Type*/, TB_BATTLE_MODE_INFO>::iterator		TB_BATTLE_TALBE_INFO_IT;
typedef std::map<int/*Battle Mode Type*/, TB_BATTLE_MODE_INFO>::const_iterator	TB_BATTLE_TALBE_INFO_CIT;

// 시즌별 참여할 수 있는 디지몬 정보
typedef std::map<int, std::list<DWORD>>						TB_BATTLE_ENTERABLE_INFO;
typedef std::map<int, std::list<DWORD>>::iterator			TB_BATTLE_ENTERABLE_INFO_IT;
typedef std::map<int, std::list<DWORD>>::const_iterator		TB_BATTLE_ENTERABLE_INFO_CIT;

struct sTbBattleReward
{
	DWORD			m_dwItemCode;
	unsigned int	m_Count;
};

typedef std::list<sTbBattleReward>					TB_BATTLE_REWARD_ITEM;
typedef std::list<sTbBattleReward>::iterator			TB_BATTLE_REWARD_ITEM_IT;
typedef std::list<sTbBattleReward>::const_iterator	TB_BATTLE_REWARD_ITEM_CIT;

typedef std::map<int, TB_BATTLE_REWARD_ITEM>					TB_BATTLE_REWARD_INFO;
typedef std::map<int, TB_BATTLE_REWARD_ITEM>::iterator			TB_BATTLE_REWARD_INFO_IT;
typedef std::map<int, TB_BATTLE_REWARD_ITEM>::const_iterator	TB_BATTLE_REWARD_INFO_CIT;

class CsBattleTableMng
{
public:
	CsBattleTableMng();
	~CsBattleTableMng();

	void			Delete();
	bool			Init( char* cPath );

	TB_BATTLE_TALBE_INFO const&		GetBattleInfo() const;
	TB_BATTLE_ENTERABLE_INFO const& GetBattleEnterableDigimonInfo() const;
	TB_BATTLE_REWARD_INFO const&	GetBattleRewardInfo() const;

private:
	void			_LoadExcel(char const* pFileName, char const* pSheetName);
	bool			_LoadBin( char* cPath );
	bool			_LoadFilePack( char* cPath );

	void			_SaveBin_BattleInfo( FILE* fp );
	void			_SaveBin_BattleRewardInfo( FILE* fp );
	void			_SaveBin_BattleEnterableInfo( FILE* fp );

	void			_LoadExcel_BattleInfo();
	void			_LoadExcel_BattleRewardInfo();
	void			_LoadExcel_BattleEnterableInfo();

	void			_LoadBin_BattleInfo( FILE *fp );
	void			_LoadBin_BattleRewardInfo( FILE *fp );
	void			_LoadBin_BattleEnterableInfo( FILE *fp );


	void			_LoadPack_BattleInfo( int nHandle );
	void			_LoadPack_BattleRewardInfo( int nHandle );
	void			_LoadPack_BattleEnterableInfo( int nHandle );

public:
	void			SaveBin( char* cPath );

private:
	TB_BATTLE_TALBE_INFO		m_BattleTableInfos;
	TB_BATTLE_ENTERABLE_INFO	m_BattleEnterableDigimons;
	TB_BATTLE_REWARD_INFO		m_BattleRewardInfo;
};