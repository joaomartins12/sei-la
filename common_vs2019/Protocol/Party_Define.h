#pragma once

#define MAX_PARTY_COUNT 3		// 최대 파티인원 수

namespace enPartyInviteResult
{
	enum eResult
	{
		Impossible		= -3,	// 파티 요청을 응답할 수 없는 상태임
		OffLine			= -2,	// 접속중이지 아니하거나, 존재하지 아니함
		Reject			= -1,	// 파티 거부
		AlreadyParty	= 0,	// 다른 파티에 가입중
		Accept			= 1,	// 수락
	};
};

// 파티원 주사위 결과
struct TDiceResult
{
	TDiceResult():m_nDiceNumber(0)
	{}

	n1				m_nDiceNumber;			// 주사위 나온 숫자
	std::wstring	m_szDiceTamerName;		// 주사위 굴린 테이머 이름
};

// 파티원 버프 정보
struct TPartyMemberBuffInfo
{
	TPartyMemberBuffInfo():m_nStatus(0),m_nBuffCode(0)
	{}

	n1 m_nStatus;
	u2 m_nBuffCode;
};

// 파티원 루팅 정보
struct TItemLootingInfo
{
	TItemLootingInfo() : m_nCropType(0), m_nRareRate(0), m_nDispRareGrade(0)
	{}

	n1		m_nCropType;
	n1		m_nRareRate;
	n1		m_nDispRareGrade;
};

// 파티원 기본 정보
struct TBaseStateInfo
{
	TBaseStateInfo() :m_nHP(0),m_nMHP(0),m_nDS(0),m_nMDS(0)
	{}
	n4 m_nHP;
	n4 m_nMHP;
	n4 m_nDS;
	n4 m_nMDS;
};

// 파티원 테이머 정보
struct TPartyTamerInfo
{
	TPartyTamerInfo() :m_nType(0),m_nUID(0),m_nLevel(0)
	{}

	u2				m_nUID;		// 테이머 UIDX
	n4				m_nType;
	n2				m_nLevel;
	std::wstring	m_Name;
	TBaseStateInfo	m_BaseInfo;
};

// 파티원 디지몬 정보
struct TPartyDigimonInfo
{
	TPartyDigimonInfo() :m_nType(0),m_nUID(0),m_nLevel(0)
	{}
	u2					m_nUID;		// 디지몬 UIDX
	n4					m_nType;
	n2					m_nLevel;
	std::wstring		m_Name;
	TBaseStateInfo		m_BaseInfo;
};

// 파티원 정보
struct TPartyMember
{
	TPartyMember() : m_nSlotNo(0),m_nMapNo(0),m_nChannelNo(0)
	{}

	n1					m_nSlotNo;
	n4					m_nMapNo;
	n4					m_nChannelNo;
	TPartyTamerInfo		m_TamerInfo;
	TPartyDigimonInfo	m_DigimonInfo;
};


struct stPartyLastParam	//서버간 통신용
{
	TBaseStateInfo cStatTamer;
	TBaseStateInfo cStatDigimon;
	n2 nTamerLv;
	n2 nDigimonLv;
};