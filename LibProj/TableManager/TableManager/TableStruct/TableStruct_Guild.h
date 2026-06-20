#pragma once

BEGIN_NAME_TABLE

struct sGuildLv
{
	sGuildLv():s_nNeedFame(0), s_nItemNo1(0), s_nItemCount1(0), s_nItemNo2(0), s_nItemCount2(0)
			, s_nMaxGuildPerson(0), s_nIncMember(0), s_nMaxGuild2Master(0), s_nMasterLevel(0), s_nNeedPerson(0)
	{}
	
	DWORD		s_nNeedFame;	// 레벨업에 필요한 명성
	DWORD		s_nItemNo1;		// 필요 아이템
	int			s_nItemCount1;	// 갯수
	DWORD		s_nItemNo2;		// 필요 아이템
	int			s_nItemCount2;	// 갯수

	int			s_nMaxGuildPerson;	// 기본 길드원 수
	int			s_nIncMember;		// 최대 증가 길드원 수
	int			s_nMaxGuild2Master;	// 부마스터 수
	int			s_nMasterLevel;		// 마스터의 필요 레벨
	int			s_nNeedPerson;		// 레벨업에 필요한 최소 길드원 수

};

enum eGuildPointType 
{ 
	eNONE,
	eRAIDMONSTER,	// 레이드몬스터 1마리
	eQUEST,			// 퀘스트 완료 1회
	eHATCH,			// 알 부화 1회
	eKILL_100		// 몬스터 100마리(레이드몬스터 포함)
};

struct sGuildLimit 
{
	sGuildLimit():s_nLimitLv(0), s_nChangeRate(0)
	{}
	~sGuildLimit()
	{
		s_mapLevelInfo.clear();
		s_mapPointInfo.clear();
	}

	USHORT		s_nLimitLv;		// 길드 생성 레벨 제한(테이머)
	UINT		s_nChangeRate;	// GSP 전환 비율

	std::map< USHORT/*레벨*/, UINT /*필요경험치*/>	s_mapLevelInfo;
	std::map< USHORT/*획득 조건*/, USHORT /*조건 만족 시 획득 포인트*/>	s_mapPointInfo;
};

struct sGuildSkill
{
	sGuildSkill():s_nSkillType(0),s_nConsGsp(0), s_nReqLevel(0), s_dwSkillCode(0)
	{}

	USHORT		s_nSkillType;		// 0 : 기본스킬, 1 : 기간제스킬	
	UINT		s_nConsGsp;			// 소모 Gsp( guild skill point )
	USHORT		s_nReqLevel;		// 필요 길드 레벨
	DWORD		s_dwSkillCode;		// 스킬 코드
	std::wstring s_szName;		// 스킬 이름
	std::wstring s_szComment;	// 스킬 설명
};
END_NAME_TABLE