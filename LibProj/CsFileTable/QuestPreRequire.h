

#pragma once 

class CsQuestPreRequire
{
public:
	enum eTYPE{	QUEST = 0, T_LEVEL, ITEM, D_LEVEL, D_TYPE, REGION,
		D_EVO, D_EVO_DISABLE, 
		D_BASE_TABLE_IDX, // 디지몬의 기본체 ID 조건
		TAMER_TABLE_IDX,		// 테이머 테이블 ID 조건
		QUEST_ID_CANNOT_PROGRESS,			// 진행 중인 퀘스트가 있을시 진행 불가
		QUEST_ID_CANNOT_COMPLETED,		// 완료된 퀘스트가 있을시 진행 불가
	};
	typedef	std::list< CsQuestPreRequire* >				LIST;
	typedef	std::list< CsQuestPreRequire* >::iterator	LIST_IT;
public:
	CsQuestPreRequire() {}
	~CsQuestPreRequire(){ Delete(); }

protected:
	eTYPE			m_eType;
	UINT			m_nTypeID;
	int				m_nTypeCount;

public:
	void			Delete(){}
	void			Init( eTYPE type, UINT nTypeID, int nTypeCount );
	void			Reset(){ m_eType = QUEST; m_nTypeID = 0; m_nTypeCount = 0; }

	void			SetType( eTYPE type ){ m_eType = type;}
	void			SetTypeID( UINT nTypeID ){ m_nTypeID = nTypeID; }
	void			SetTypeCount( int nTypeCount ){ m_nTypeCount = nTypeCount; }

	eTYPE			GetType(){ return m_eType; }
	UINT			GetTypeID(){ return m_nTypeID; }	
	int				GetTypeCount(){ return m_nTypeCount; }

	CsQuestPreRequire*	Clone();
};

