#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class CharacterCreateContents
	: public IUIContentSubject< CharacterCreateContents >
{
public:
	enum eEventType
	{
		eTamer_Selected,
		eDigimon_Selected,
	};

public:
	static int const	IsContentsIdentity(void);

	struct sSkillSimpleInfo
	{
		sSkillSimpleInfo():m_dwIconIndex(0), m_nAttType(0),m_dwSkillCode(0) {}
		~sSkillSimpleInfo() {}

		DWORD			m_dwIconIndex;	// 아이콘 번호
		DWORD			m_dwSkillCode;
		std::wstring	m_wsName;	// 스킬 이름
		std::wstring	m_wsDesc;	// 스킬 설명
		int				m_nAttType;	// 액티브/패시브 구분
	};

	typedef std::list<sSkillSimpleInfo>					LIST_SKILL_INFO;
	typedef std::list<sSkillSimpleInfo>::iterator		LIST_SKILL_INFO_IT;
	typedef std::list<sSkillSimpleInfo>::const_iterator	LIST_SKILL_INFO_CIT;

	struct sTamerCreatedInfo// 캐릭터 생성 할 수 있는 정보
	{
	public:
		sTamerCreatedInfo() : m_nFileTableID(0),m_nSeasonType(0), m_nEnableCreated(false),m_nIconIdx(0)
		{}
		~sTamerCreatedInfo() {}

		void						SetEtcInfo();
		void						SetTamerBaseState();

	public:
		uint						m_nFileTableID;		//	오브젝트 인덱스
		int							m_nSeasonType;		// 세이버즈/어드벤처		
		bool						m_nEnableCreated;	//	
		std::string					m_soundFileName;	//	사운드 파일명
		std::wstring				m_szName;			// 테이머/디지몬 이름
		std::wstring				m_szComment;		// 테이머/디지몬 설명
		std::wstring				m_szSeasonName;
		LIST_SKILL_INFO				m_skilList;
		int							m_nIconIdx;			// 테이머 UI 인덱스 번호

		std::map<int,int>			m_mapTamerState;
		std::list<DWORD>			m_listCostume;		// 테이머 코스튬 아이템 인덱스 번호
	};

	typedef std::list<sTamerCreatedInfo>					LIST_TAMER_INFO;
	typedef std::list<sTamerCreatedInfo>::iterator			LIST_TAMER_INFO_IT;
	typedef std::list<sTamerCreatedInfo>::const_iterator	LIST_TAMER_INFO_CIT;

	//////////////////////////////////////////////////////////////////////////
	struct sDigimonEvolutionInfo
	{
	public:
		sDigimonEvolutionInfo():m_nFileTableID(0),m_nDigimonAttributeType(nsCsDigimonTable::AT_NONE),m_nDigimonEvolType(nsCsDigimonTable::ET_DIGITAMA)
		{};
		~sDigimonEvolutionInfo() {};

		void	SetDigimonEvolType( nsCsDigimonTable::eEVOLUTION_TYPE const& eType );
		void	SetDigimonAttributeType( nsCsDigimonTable::eATTRIBUTE_TYPE const& eType );

		std::wstring						m_szName;			// 디지몬 이름
		std::string							m_szDigimonIcon;	// 디지몬 아이콘 이름
		uint								m_nFileTableID;		//	오브젝트 인덱스

		nsCsDigimonTable::eEVOLUTION_TYPE	m_nDigimonEvolType;	// 디지몬 성장 단계
		std::wstring						m_strEvolType;
		nsCsDigimonTable::eATTRIBUTE_TYPE	m_nDigimonAttributeType;	// 디지몬 속성
		std::wstring						m_strAttributeType;

		LIST_SKILL_INFO						m_skilList;
	};

	typedef std::list<sDigimonEvolutionInfo>					LIST_DIGIMON_EVO_INFO;
	typedef std::list<sDigimonEvolutionInfo>::iterator			LIST_DIGIMON_EVO_INFO_IT;
	typedef std::list<sDigimonEvolutionInfo>::const_iterator	LIST_DIGIMON_EVO_INFO_CIT;

	struct sDigimonCreatedInfo// 디지몬 생성 할 수 있는 정보
	{
	public:
		sDigimonCreatedInfo():m_nFileTableID(0),m_nEnableCreated(false)
		{}
		~sDigimonCreatedInfo() 
		{}

		void				SetEtcInfo();
		void				SetEvolutionInfo();

	public:
		uint				m_nFileTableID;		//	오브젝트 인덱스
		bool				m_nEnableCreated;	//	
		std::string			m_soundFileName;	//	사운드 파일명
		std::string			m_sDigimonIconName;
		std::wstring		m_szComment;		// 테이머/디지몬 설명

		LIST_DIGIMON_EVO_INFO	m_EvolutionInfo;	// 디지몬 진화 정보
	};

	typedef std::list<sDigimonCreatedInfo>					LIST_DIGIMON_INFO;
	typedef std::list<sDigimonCreatedInfo>::iterator		LIST_DIGIMON_INFO_IT;
	typedef std::list<sDigimonCreatedInfo>::const_iterator	LIST_DIGIMON_INFO_CIT;

public:
	CharacterCreateContents(void);
	virtual ~CharacterCreateContents(void);

	virtual int const	GetContentsIdentity(void) const;

	virtual bool		Initialize(void);
	virtual void		UnInitialize(void);
	virtual bool		IntraConnection(ContentsSystem& System);
	virtual void		Update(float const& fElapsedTime);
	virtual void		NotifyEvent(int const& iNotifiedEvt);
	virtual void		MakeMainActorData(void);
	virtual void		ClearMainActorData(void);

	void				GotoBack();
	bool				SendCheckTamerName( std::wstring const& pCheckName );
	bool				SendCreate( std::wstring const& pCheckName );

	void				SetSelectTamerIdx(unsigned int const& nSelIdx);
	void				SetSelectDigimonIdx(unsigned int const& nSelBaseIdx, unsigned int const& nSelEvolIdx);
	void				SetSelectDigimonEvolution( unsigned int const& nSelIdx, unsigned int const& nEvolutionIdx );

	LIST_TAMER_INFO const& GetMakeTamerList() const;
	LIST_DIGIMON_INFO const& GetMakeDigimonList() const;

	void				ChangeTamerCostume( DWORD const& dwItemID );

	void				Update_DigimonModel(float fDeltaTime);
	void				Update_TamerModel(float fDeltaTime);

	void				Render_DigimonModel();
	void				Render_TamerModel();

	void				MakeCreatedTamerList();
	void				MakeCreatedDigimonList();

	sTamerCreatedInfo const* GetTamerInfo( unsigned int const& nTIdx ) const;
	sDigimonCreatedInfo const* GetDigimonInfo( unsigned int const& nTIdx ) const;
	sDigimonEvolutionInfo const* GetDigimonEvolutionInfo( unsigned int const& nBaseTIdx, unsigned int const& nEvolTIdx ) const;

	std::wstring		GetItemName(DWORD const& dwItemCode);

private:
	void				ResetData();
	void				RecvCheckDoubleName(void* pData);
	void				RecvCreateResult(void* pData);

	std::string			GetSoundFileName( uint const& mTbIdx );

private:
	TCHAR				m_szTamerName[ MAX_FILENAME ];
	TCHAR				m_szDigimonName[ MAX_FILENAME ];

	unsigned int		m_nSelectedTamerTID;	// 선택한 테이머 테이블 인덱스 번호
	unsigned int		m_nSelectedDigimonTID;	// 선택한 디지몬 테이블 인덱스 번호

	CTamer*				m_pTamerObject;			// 3D 오브젝트
	CDigimon*			m_pDigimonObject;		// 3D 오브젝트

	LIST_TAMER_INFO		m_TamerListInfo;
	LIST_DIGIMON_INFO	m_DigimonListInfo;
};
