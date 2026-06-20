#pragma once
BEGIN_NAME_TABLE

// 확장 조건
struct sExpansionCondition
{
	sExpansionCondition() :nExpansionRank(0)
	{}

	~sExpansionCondition() {};

	USHORT				nExpansionRank;			// 확장 단계
	std::list<USHORT>	digimonEvoTypes;	// 확장 가능한 디지몬 진화 타입

	bool IsEnableEvoType(int const& nCheckEvoType) const
	{
		std::list<USHORT>::const_iterator subIT = digimonEvoTypes.begin();
		for (; subIT != digimonEvoTypes.end(); ++subIT)
		{
			if (*subIT == nCheckEvoType)
				return true;
		}
		return false;
	}
};

struct sEvoMaxkSkillLv
{
	sEvoMaxkSkillLv() :s_SkillExpStartLv(0)
	{}
	~sEvoMaxkSkillLv() {};

	int					s_SkillExpStartLv;// 진화체 별로 디지몬의 스킬 경험시 시작 테이블 인덱스 번호
	std::list<USHORT>	s_SkillMaxLvs;
};

END_NAME_TABLE