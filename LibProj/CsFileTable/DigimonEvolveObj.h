#pragma once

#define MAX_EVOLUTION			9	// 진화 8, 퇴화 1

#define MAX_EVO_EFFECT_LEN		32
#define MAX_JOINT_TACTICS_LEN		3

enum NEED_QUALITICATION
{
	_NO_NEED,
	_PARTNERMON,
	_ROYAL_KNIGHT,
#ifdef SDM_TAMER_XGUAGE_20180628
	_XAI_SYSTEM,
#endif
};

class CDigimonEvolveObj
{
public:
public:
	typedef	std::map< DWORD, CDigimonEvolveObj* >				MAP;
	typedef	std::map< DWORD, CDigimonEvolveObj* >::iterator		MAP_IT;

public:
	CDigimonEvolveObj(void);
 
public:
	// 진화체 정보
	struct SEvolutionInfo
	{
		SEvolutionInfo(){ nSlot = INVAIDE; nType = INVAIDE; dwDigimonID =INVAIDE; }

		enum{ INVAIDE	= 0, };
		USHORT		nSlot;		// 진화 슬롯 번호
		USHORT		nType;		// 진화시 hp,ds변화 타입
		DWORD		dwDigimonID;
	};


	DWORD			m_dwID;						// 디지몬 아이디
	USHORT			m_nEvoSlot;					// 진화 슬롯
	SEvolutionInfo	m_nEvolutionList[MAX_EVOLUTION];	// 진화체 리스트	

	CsPoint			m_IconPos;					// 아이콘 위치,크기

	USHORT			m_nEnableSlot;				// 사용여부
	USHORT			m_nOpenQualification;		// 오픈 자격
	USHORT			m_nOpenLevel;				// 오픈 레벨
	USHORT			m_nOpenQuest;				// 오픈 퀘스트

	USHORT			m_nOpenItemTypeS;			// 오픈 아이템
	USHORT			m_nOpenItemNum;				// 오픈 아이템 개수	

	USHORT			m_nUseItem;					// 소모 아이템
	USHORT			m_nUseItemNum;				// 소모 아이템 개수
	USHORT			m_nIntimacy;				// 진화시 필요 친밀도

	USHORT			m_nOpenCrest;				// 오픈 문장

	USHORT			m_EvoCard1;					// 진화 카드1
	USHORT			m_EvoCard2;					// 진화 카드2
	USHORT			m_EvoCard3;					// 진화 카드3	
	USHORT			m_nEvoDigimental;			// 진화 캡슐
	USHORT			m_nEvoTamerDS;				// 테이머 소모 ds
	USHORT			m_nDummy;					// 나중에 필요하면쓰자

	int				m_bRender;
	CsPoint			m_TargetStartPos;			// 부화화면 시작좌표
	int				m_nStartHegiht;				// 시작 높이
	int				m_nStartRot;				// 시작 회전값
	CsPoint			m_TargetEndPos;				// 부화화면 종료좌표
	int				m_nEndHegiht;				// 종료 높이
	int				m_nEndRot;					// 종료 회전값	
	int				m_nSpeed;					// 애니메이션 이동속도
	DWORD			m_dwAni;					// 부화화면 애니
	double			m_fStartTime;				// 부화화면 플레이시간
	double			m_fEndTime;					// 부화화면 플레이시간
	int				m_nR;						// 배경 RGB값
	int				m_nG;
	int				m_nB;

	char			m_szLevel[ MAX_EVO_EFFECT_LEN ];			// 레벨 이펙트 부위
	char			m_szEnchant[ MAX_EVO_EFFECT_LEN ];			// 강화 이펙트 부위
	char			m_szSize[ MAX_EVO_EFFECT_LEN ];			// 크기 이펙트 부위

	int				m_nEvolutionTree;					// 진화 트리 완료

	DWORD			m_nJoGressQuestCheck;					// 조그레스 퀘스트검사

	USHORT			m_nChipsetType;					// 조그래스 칩셋
	USHORT			m_nChipsetTypeC;			// 조그래스 칩셋 필요아이템 소모
	USHORT			m_nChipsetNum;				// 조그래스 칩셋 수량
	USHORT			m_nChipsetTypeP;			// 조그래스 칩셋 필요아이템 기간


	USHORT			m_nJoGressesNum;									// 조그래스 필요 용병수
	int				m_nJoGress_Tacticses [ MAX_JOINT_TACTICS_LEN ];			// 조그래스 상대 디지몬 베이스
};
