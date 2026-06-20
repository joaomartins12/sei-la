
#pragma once 

BEGIN_NAME_TABLE

struct sDBookDeck
{
	struct sCondition
	{
		USHORT		s_nCondition;//옵션 발동 조건		( 1:패시브, 2:확률발동, 3:확률발동+지속시간 )
		USHORT		s_nAT_Type;	//발동 효과 체크 타입	( 0:사용안함-패시브 1:일반공격시 , 2:스킬공격시 )
		USHORT		s_nOption;	//옵션
		//옵션 : 수치 증가 ( 1:공격력	2:HP	3:스킬피해	4:크리티컬 )
		//옵션 : 확률발동  ( 5: 스킬 쿨타임 초기화 )
		//옵션 : 확률발동  ( 6: 공속증가 )		
		USHORT		s_nVal;		//옵션값
		UINT		s_nProb;		//확률
		UINT		s_nTime;		//적용시간
	};

	struct sComposition
	{
		DWORD	s_dwBaseDIdx;
		USHORT	s_nEvolslot;
		DWORD	s_dwDestDIdx;
	};

	typedef std::list<sCondition>	Conditions;
	typedef std::list<sComposition> Compositions;

	std::wstring	s_wsGroupName;
	std::wstring	s_wsExplain;
	Conditions		s_listConditions;
	Compositions	s_listCompositions;
};

struct sDBookOption
{
	DWORD			s_dwIconNum;
	std::wstring	s_wsOptTitle;
	std::wstring	s_wsOptDesc;
};

END_NAME_TABLE