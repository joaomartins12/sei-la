
#pragma once 

BEGIN_NAME_TABLE

struct sDigiEvoApplyData
{
	sDigiEvoApplyData():nStatusPer(0)
	{}
	int nStatusPer;
	std::wstring name;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 한번에 진화 할 수 있는 디지몬의 정보
struct sDEvoTargetInfo
{
	sDEvoTargetInfo():nSlot(0), nType(0), dwDigimonID(0)
	{}

	USHORT		nSlot;		// 진화 슬롯 번호
	USHORT		nType;		// 진화시 hp,ds변화 타입
	DWORD		dwDigimonID;// 대상의 디지몬 IDX
};

// UI Icon 좌표
struct sDEvoUIIconPos
{
	int			m_PosX;					// 아이콘 위치,크기
	int			m_PosY;					// 아이콘 위치,크기
};

// 디지몬 부화시 연출되는 3D씬 정보
struct sDEvo3DRenderInfo
{
	bool		m_bRender;
	int			m_TargetStartPosX;			// 부화화면 시작좌표
	int			m_TargetStartPosY;			// 부화화면 시작좌표
	int			m_nStartHegiht;				// 시작 높이
	int			m_nStartRot;				// 시작 회전값
	int			m_TargetEndPosX;				// 부화화면 종료좌표
	int			m_TargetEndPosY;				// 부화화면 종료좌표
	int			m_nEndHegiht;				// 종료 높이
	int			m_nEndRot;					// 종료 회전값	
	int			m_nSpeed;					// 애니메이션 이동속도
	DWORD		m_dwAni;					// 부화화면 애니
	double		m_fStartTime;				// 부화화면 플레이시간
	double		m_fEndTime;					// 부화화면 플레이시간
	int			m_nR;						// 배경 RGB값
	int			m_nG;
	int			m_nB;
};

// 슬롯 오픈에 필요한 조건들
struct sDEvoSlotOpenLimit
{
	enum eLimitType
	{
		eLimit_None,
		eLimit_OnlyPartner,
		eLimit_RoyalKnights,
	};
	USHORT			m_nOpenQualification;		// 오픈 자격
	USHORT			m_nOpenLevel;				// 오픈 레벨
	DWORD			m_dwOpenQuest;				// 오픈 퀘스트

	USHORT			m_nOpenItemTypeS;			// 오픈 아이템
	USHORT			m_nOpenItemNum;				// 오픈 아이템 개수	

	DWORD			m_dwUseItem;					// 소모 아이템
	USHORT			m_nUseItemNum;				// 소모 아이템 개수

	USHORT			m_nIntimacy;				// 진화시 필요 친밀도
	USHORT			m_nOpenCrest;				// 오픈 문장
};

// 진화 카드 정보???
struct sDEvoCard
{
	USHORT			m_EvoCard1;					// 진화 카드1
	USHORT			m_EvoCard2;					// 진화 카드2
	USHORT			m_EvoCard3;					// 진화 카드3	
	USHORT			m_nEvoDigimental;			// 진화 캡슐
};

// 각 이펙트를 붙일 Bone Name
struct sDEvoEffectPos
{
	std::string m_sLevel;		// 레벨 이펙트 부위
	std::string m_sEnchant;		// 강화 이펙트 부위
	std::string m_sSize;		// 크기 이펙트 부위
};

// 조그레스 진화 시 필요한 정보들
struct sDEvoJogres
{
	DWORD				m_nJoGressQuestCheck;	// 조그레스 퀘스트검사
	USHORT				m_nChipsetType;			// 조그래스 칩셋
	USHORT				m_nChipsetTypeC;		// 조그래스 칩셋 필요아이템 소모
	USHORT				m_nChipsetNum;			// 조그래스 칩셋 수량
	USHORT				m_nChipsetTypeP;		// 조그래스 칩셋 필요아이템 기간
	std::list<DWORD>	m_NeedDigimons;			// 조그레스 진화시 필요한 디지몬 인덱스 번호
};

struct sDEvolutionInfo
{
	DWORD						m_dwID;				// 디지몬 아이디
	USHORT						m_nEvoSlot;			// 현재 디지몬의 진화 슬롯 번호
	std::list<sDEvoTargetInfo>	m_nEvolutionList;	// 한번에 진화 할 수 있는 디지몬의 정보 리스트	
	
	bool						m_bEnableSlot;		// 사용여부
	sDEvoSlotOpenLimit			m_SlotOpenLimit;	// 슬롯을 오픈할 때 조건
	USHORT						m_nEvoTamerDS;		// 테이머 소모 ds Type
	sDEvoCard					m_EvoCardInfo;		// 진화 카드 정보 
	sDEvo3DRenderInfo			m_3dRenderInfo;
	sDEvoEffectPos				m_EffectPos;	
	sDEvoUIIconPos				m_UIIconPos;
	int							m_nCapsuleEvo;					// 캡슐 진화시 트리값
	sDEvoJogres					n_JogresEvo;					// 조그레스 진화시 필요한 정보
};

END_NAME_TABLE