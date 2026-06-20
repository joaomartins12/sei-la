#pragma once

// 확장 조건
struct sExpansionCondition
{
	sExpansionCondition():nExpansionRank(0)
	{}

	~sExpansionCondition() {};

	int nExpansionRank;			// 확장 단계
	std::list<int>	digimonEvoTypes;	// 확장 가능한 디지몬 진화 타입

	bool IsEnableEvoType(int const& nCheckEvoType) const
	{
		std::list<int>::const_iterator subIT = digimonEvoTypes.begin();
		for( ; subIT != digimonEvoTypes.end(); ++subIT )
		{
			if( *subIT == nCheckEvoType )
				return true;
		}
		return false;
	}
};

typedef std::map<int/*진화에 필요한 아이템 타입*/, sExpansionCondition >	MAP_DSKILL_OPEN_EXPANSION_DATA;
typedef std::map<int, sExpansionCondition >::iterator						MAP_DSKILL_OPEN_EXPANSION_DATA_IT;
typedef std::map<int, sExpansionCondition >::const_iterator					MAP_DSKILL_OPEN_EXPANSION_DATA_CIT;


// 디지몬의 진화 단계별 Max Skill Level
typedef std::list<int>						LIST_RANK_MAXSKILL;
typedef std::list<int>::iterator			LIST_RANK_MAXSKILL_IT;
typedef std::list<int>::const_iterator		LIST_RANK_MAXSKILL_CIT;

struct sEvoMaxkSkillLv
{
	sEvoMaxkSkillLv():s_SkillExpStartLv(0)
	{}
	~sEvoMaxkSkillLv() {};

	int					s_SkillExpStartLv;// 진화체 별로 디지몬의 스킬 경험시 시작 테이블 인덱스 번호
	LIST_RANK_MAXSKILL	s_SkillMaxLvs;
};

typedef std::map<int, sEvoMaxkSkillLv>					MAP_DIGIMON_EVOMAXLV;
typedef std::map<int, sEvoMaxkSkillLv>::iterator		MAP_DIGIMON_EVOMAXLV_IT;
typedef std::map<int, sEvoMaxkSkillLv>::const_iterator	MAP_DIGIMON_EVOMAXLV_CIT;
