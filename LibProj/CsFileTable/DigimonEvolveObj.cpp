#include "StdAfx.h"
#include "DigimonEvolveObj.h"

CDigimonEvolveObj::CDigimonEvolveObj(void)
{
	m_dwID = 0;						// 디지몬 아이디
	m_nEvoSlot = 0;					// 진화 실롯	
	m_IconPos.x = 0;					// 아이콘 위치,크기
	m_IconPos.y = 0;					// 아이콘 위치,크기

	m_nOpenQualification = 0;		// 오픈 자격
	m_nOpenLevel = 0;				// 오픈 레벨
	m_nOpenQuest = 0;				// 오픈 퀘스트
	m_nOpenItemTypeS = 0;			// 오픈 아이템
	m_nOpenCrest = 0;				// 오픈 문장	

	m_EvoCard1 = 0;					// 진화 카드1
	m_EvoCard2 = 0;					// 진화 카드2
	m_EvoCard3 = 0;					// 진화 카드3	
	m_nEvoDigimental = 0;			// 진화 캡슐

	m_nEvoTamerDS = 0;

	memset( m_szLevel, 0, sizeof( m_szLevel ) );
	memset( m_szEnchant, 0, sizeof( m_szEnchant ) );
	memset( m_szSize, 0, sizeof( m_szSize ) );

	m_nEvolutionTree	=	0;				// 진화 트리

	m_nJoGressQuestCheck	=	0;				// 진화 트리

	m_nChipsetType	=	0;				// 조그래스 칩셋1 
	m_nChipsetTypeC	=	0;			// 조그래스 칩셋1 필요아이템 소모
	m_nChipsetNum	=	0;			// 조그래스 칩셋1 소모 수량
	m_nChipsetTypeP	=	0;			// 조그래스 칩셋1 필요아이템 수량

	m_nJoGressesNum	=	0;												// 조그래스 필요 용병수
	memset( m_nJoGress_Tacticses, 0, sizeof( m_nJoGress_Tacticses ) );	// 조그래스 상대 디지몬 베이스
}


