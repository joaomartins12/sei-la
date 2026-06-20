

#pragma once 

// 클라이언트에서 처리할 완료 타입
namespace clientCompType	
{
	enum emType
	{
		eAccept = 1,		//퀘스트 수락 완료
		eMovePosition,		//특정 위치로 이동
		eCamerControl,		// 카메라 줌/회전
		eTargetTrace,		// 타겟 추적
		eAttack,			// 공격
		eScanAndRetrieve,	// 스캔/회수
		eDigimonHatch,		// 디지몬 부화
		eDigimonEnchant,	// 디지몬 강화
		eDigimonChange,		// 디지몬 교체
	};
}

class CsQuestCompRequire
{
public:
	enum eTYPE // 완료 조건 타입
	{	
		// 테이블 설정값
		NONE = -1,
		MONSTER = 0,
		ITEM,
		CLIENT,
		REGION,
		NPCTALK,
		USE_ITEM,
		USE_ITEM_NPC,
		USE_ITEM_MONSTER,
		USE_ITEM_REGION,
		T_LEVEL,
		D_TYPE,

		// 클라이언트 임의값
		MAP = 100,
		EMPLOYMENT = 101,
	};

	typedef	std::list< CsQuestCompRequire* >			LIST;
	typedef	std::list< CsQuestCompRequire* >::iterator	LIST_IT;
public:
	CsQuestCompRequire();
	~CsQuestCompRequire(){ Delete(); }

protected:
	eTYPE			m_eType;
	UINT			m_nTypeID;
	int				m_nTypeCount;
	int				m_nCurTypeCount;
	UINT			m_nTargetID;

	int				m_nSubValue;
	int				m_nSubValue2;

public:
	void			Delete(){}
	void			Init( eTYPE type, UINT nTypeID, int nTypeCount, UINT nTargetID, int nSubValue, int nSubValue2 );
	void			Reset();

	void			SetType( eTYPE type ){ m_eType = type;}
	void			SetTypeID( UINT nTypeID ){ m_nTypeID = nTypeID; }
	void			SetTypeCount( int nTypeCount ){ m_nTypeCount = nTypeCount; }
	void			SetTargetID( UINT nTargetID ){ m_nTargetID = nTargetID; }

	eTYPE			GetType(){ return m_eType; }
	UINT			GetTypeID(){ return m_nTypeID; }	
	int				GetTypeCount(){ return m_nTypeCount; }
	UINT			GetTargetID(){ return m_nTargetID; }

	void			SetSubValue(int const& nValue)	{ m_nSubValue = nValue; }
	void			SetSubValue2(int const& nValue)	{ m_nSubValue2 = nValue; }

	int				GetSubValue() const { return m_nSubValue; }
	int				GetSubValue2() const { return m_nSubValue2; }


public:
	inline bool		SetCurCount( int nCurCount );
	inline bool		IncreaseCurCount();
	inline bool		IsCompleat();
	int				GetCurCount(){ return m_nCurTypeCount; }

	CsQuestCompRequire*	Clone();
};

#include "QuestCompRequire.inl"

