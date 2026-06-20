
#include "Community_Define.h"
#include <list>

//////////////////////////////////////////////////////////////////////////
// Mail 
//////////////////////////////////////////////////////////////////////////
class GS2C_GUILDINFO
{
public:
	GS2C_GUILDINFO(): nIncMaxMember(0)
	{}

public:	
	n4 nGuildIDX;
	n4 nGuildExp;

	u4 nIncMaxMember;	// 아이템에 의해 증가된 최대 멤버 인원수
	u4 nGSP;

	std::wstring wsGuildName;			//길드명

	std::wstring wsGuildNotice;
	//길드 등급 명칭/설명 정보
	std::list<std::pair<std::wstring, std::wstring>> m_GuildGradeInfoList;
	//길드원 정보
	std::list<GuildInfo::sGuild_PersonInfo> m_GuildPersonInfoList;


	n1 nGuildLevel;
#ifdef GUILD_RENEWAL
	//길드 스킬 정보
	std::list<std::pair<unsigned int, unsigned int>> m_GuildSkilList;
#endif
};

class GS2C_GUILD_HISTORY
{
public:
	GS2C_GUILD_HISTORY(){}

	std::list<GuildInfo::sGuild_History> m_GuildHistoryList;
};

class GS2C_GUILD_LEVELUP
{
public:
	GS2C_GUILD_LEVELUP():nTamerUID(0), nLevel(2), nSkillCode(0)
	{}

	u2 nTamerUID;	// 길드 레벨이 오른 테이머 UID
	n4 nLevel;		// 길드 레벨
	u4 nSkillCode;		// 0: 색 미변경, !0: 색 변경 베이스스킬 코드
};
class GS2C_GUILD_MEMBERLEVELUP
{
public:
	GS2C_GUILD_MEMBERLEVELUP(){}

	std::wstring wsTamerName;
	u1 nTamerLevel;
};
class GS2C_GUILD_ADD
{
public:
	GS2C_GUILD_ADD(){}

	u1 nGuildClass;
	u4 nSkillCode;						// 0: 색 미변경, !0: 색 변경 베이스스킬 코드
	u4 nTamerType;
	std::wstring szTamerName;
	std::wstring szGuildName;

	u1 nTamerLevel;
	u2 nMapNo;
	u1 nChannelNo;

};
class GS2C_GUILD_REQADDFAILURE
{
public:
	GS2C_GUILD_REQADDFAILURE(){}

	int nCode;
	std::wstring wsTamerName;
};
class GS2C_GUILD_ON
{
public:
	GS2C_GUILD_ON(){}

	std::wstring szTamerName;
	u2 nMapNo;
	u1 nChannelNo;
	u1 nTamerLevel;
};

class GS2C_GUILD_CHANGE_CLASSNAME_SEND
{
public:
	GS2C_GUILD_CHANGE_CLASSNAME_SEND(){}

	int nClass;
	std::wstring wsName;
	u4 baseWindowType;
};
class GS2C_GUILD_CHANGE_CLASSNAME_RECV
{
public:
	GS2C_GUILD_CHANGE_CLASSNAME_RECV(){}

	n1 nClass;
	std::wstring szName;
	std::wstring szDesc;
};
class GS2C_GUILD_SKILL
{
public:
	GS2C_GUILD_SKILL(): nSkillCode(2600001), nEndTime(0), nUseTime(0), bUsing(false)
	{}

	bool bUsing;
	u4 nSkillCode;
	u4 nEndTime;
	u4 nUseTime;
};
class GS2C_GUILD_CHAGE_MEMO
{
public:
	GS2C_GUILD_CHAGE_MEMO()
	{}

	std::wstring wsTamerName;
	std::wstring wsMemo;
};