#pragma once 

class cQuestFindWay : public CUSTOMDATA
{
public:
	cQuestFindWay();
	~cQuestFindWay();

	enum eFindWayDestType
	{
		FindWay_None	= -1,
		FindWay_Npc		= 0,
		FindWay_Monster = 1,
		FindWay_Region  = 2,
		FindWay_Map		= 3,
		FindWay_Employment = 4
	};

	bool				Initialize( int nID, CsQuestCompRequire::eTYPE eQuestCompleteType, int nTargetID = 0);
	bool				InitMonsterInfo( int nMonsterTableID );
	bool				InitNpcInfo( int nNpcTableID );

	std::wstring		m_strDestName;  // 찾는 대상의 이름
	int 				m_nDestTableID; // 찾는 대상의 테이블 번호
	eFindWayDestType	m_nDestType;	// 이 변수 타입별로 chelpArrow 클래스에서 길찾기를 수행한다.
	int 				m_nDestMapID;   // 찾는 대상이 존재하는 맵번호
};