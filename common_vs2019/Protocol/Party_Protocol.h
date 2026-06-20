#pragma once

#include "Party_Define.h"

//////////////////////////////////////////////////////////////////////////
// 내가 파티생성
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_MAKE
{
public:
	GS2C_RECV_PARTY_MAKE():m_nPartyIDX(0)
	{}
	~GS2C_RECV_PARTY_MAKE() {}

	u4					m_nPartyIDX;	// 파티 인덱스
	u4					m_nCropType;	// 자신의 슬롯번호
};


//////////////////////////////////////////////////////////////////////////
// 파티정보 
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_INFO
{
public:
	GS2C_RECV_PARTY_INFO():m_nPartyIDX(0)
	{}
	~GS2C_RECV_PARTY_INFO() {}

	u4					m_nPartyIDX;	// 파티 인덱스
	n4					m_nMySlotNo;		// 자신의 슬롯번호
	n4					m_nMasterSlotNo;	// 파티장의 슬롯 번호

	TItemLootingInfo		m_lootingInfo;		// 아이템 루팅 정보	
	std::list<TPartyMember>	m_listMemberInfo;
};

//////////////////////////////////////////////////////////////////////////
// 파티원 아이템 획득
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_GET_ITEM
{
public:
	GS2C_NTF_PARTY_GET_ITEM():m_nRealCropTamerIDX(0)
	{}
	~GS2C_NTF_PARTY_GET_ITEM() {}

	u2						m_nRealCropTamerIDX;	// 실제로 아이템을 획득한 테이머 인덱스 번호
	std::wstring			m_szRealCropTamerName;	// 아이템을 획득한 사람
	cItemData				m_GetItem;
	std::list<TDiceResult>	m_DiceResult;
};

//////////////////////////////////////////////////////////////////////////
// 새로운 파티원 추가
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_ADD_NEW_PARTY_MEMBER
{
public:
	GS2C_RECV_ADD_NEW_PARTY_MEMBER() {}
	~GS2C_RECV_ADD_NEW_PARTY_MEMBER() {}

	TPartyMember	m_memberInfo;
};

//////////////////////////////////////////////////////////////////////////
// 파티원 강퇴
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_MEMBER_BANISH
{
public:
	GS2C_RECV_PARTY_MEMBER_BANISH():m_nResult(0),m_cBanishMemberSlotNum(0)
	{}
	~GS2C_RECV_PARTY_MEMBER_BANISH() {}

	int				m_nResult;
	n1				m_cBanishMemberSlotNum;
};

//////////////////////////////////////////////////////////////////////////
// 파티원 떠남
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_MEMBER_LEAVE
{
public:
	GS2C_RECV_PARTY_MEMBER_LEAVE():m_nResult(0),m_cLeaveMemberSlotNum(0)
	{}
	~GS2C_RECV_PARTY_MEMBER_LEAVE() {}

	int				m_nResult;
	n1				m_cLeaveMemberSlotNum;
};


//////////////////////////////////////////////////////////////////////////
// 파티 초대 패킷 받음.
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_MEMBER_INVITE_REQUEST
{
public:
	GS2C_RECV_PARTY_MEMBER_INVITE_REQUEST()
	{}
	~GS2C_RECV_PARTY_MEMBER_INVITE_REQUEST() {}

	std::wstring	m_wsInviterTamerName;// 요청자 테이머 이름
};


//////////////////////////////////////////////////////////////////////////
// 파티 요청에 대한 결과
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_MEMBER_INVITE_RESULT
{
public:
	GS2C_RECV_PARTY_MEMBER_INVITE_RESULT():m_nResult(0), m_nResultType(0)
	{}
	~GS2C_RECV_PARTY_MEMBER_INVITE_RESULT() {}

	int				m_nResult;
	n4				m_nResultType;			// enPartyInviteResult 참조
	std::wstring	m_wsTargetTamerName;
};

//////////////////////////////////////////////////////////////////////////
// 파티원 맵 이동
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_MEMBER_MAP_CHANGE
{
public:
	GS2C_NTF_PARTY_MEMBER_MAP_CHANGE():m_nSlotNo(0), m_nMapNo(0),m_nChannelNo(0),m_nTamerUID(0),m_nDigimonUID(0)
	{}
	~GS2C_NTF_PARTY_MEMBER_MAP_CHANGE() {}

	n1		m_nSlotNo;
	n4		m_nMapNo;
	n4		m_nChannelNo;

	u2		m_nTamerUID;
	u2		m_nDigimonUID;
};

//////////////////////////////////////////////////////////////////////////
// 파티원 정보 갱신
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_MEMBER_INFO_CHANGE
{
public:
	GS2C_NTF_PARTY_MEMBER_INFO_CHANGE():m_nSlotNo(0),m_nDigimonType(0),m_nTamerLevel(0),m_nDigimonLevel(0)
	{}
	~GS2C_NTF_PARTY_MEMBER_INFO_CHANGE() {}

	n1					m_nSlotNo;
	n4					m_nDigimonType;
	n2					m_nTamerLevel;
	n2					m_nDigimonLevel;
	TBaseStateInfo		m_TamerBaseInfo;
	TBaseStateInfo		m_DigimonBaseInfo;	
};

//////////////////////////////////////////////////////////////////////////
// 파티원 디지몬 교체
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_MEMBER_DIGIMON_CHANGE
{
public:
	GS2C_NTF_PARTY_MEMBER_DIGIMON_CHANGE():m_nSlotNo(0),m_nDigimonType(0)
	{}
	~GS2C_NTF_PARTY_MEMBER_DIGIMON_CHANGE() {}

	n1					m_nSlotNo;
	n4					m_nDigimonType;
	std::wstring		m_szDigimonName;	
	TBaseStateInfo		m_DigimonBaseInfo;	
};

//////////////////////////////////////////////////////////////////////////
// 파티원 On/Off Line 변경
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_MEMBER_ONOFFLINE
{
public:
	GS2C_NTF_PARTY_MEMBER_ONOFFLINE():m_nSlotNo(0),m_bOnline(true)
	{}
	~GS2C_NTF_PARTY_MEMBER_ONOFFLINE() {}

	n4				m_nSlotNo;
	bool			m_bOnline;
};

//////////////////////////////////////////////////////////////////////////
// 파티원 버프 정보 변경
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_MEMBER_BUFF_CHANGE
{
public:
	GS2C_NTF_PARTY_MEMBER_BUFF_CHANGE():m_nSlotNo(0)
	{}
	~GS2C_NTF_PARTY_MEMBER_BUFF_CHANGE() {}

	n1							m_nSlotNo;
	std::list<TPartyMemberBuffInfo>	m_listBuffInfo;
};

//////////////////////////////////////////////////////////////////////////
// 파티장 변경
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_MASTER_CHANGE
{
public:
	GS2C_RECV_PARTY_MASTER_CHANGE():m_nSlotNo(0)
	{}
	~GS2C_RECV_PARTY_MASTER_CHANGE() {}

	n4							m_nSlotNo;
};


//////////////////////////////////////////////////////////////////////////
// 파티원 위치 변경
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_MEMBER_POSITION_UPDATE
{
public:
	GS2C_NTF_PARTY_MEMBER_POSITION_UPDATE():m_nSlotNo(0)
	{}
	~GS2C_NTF_PARTY_MEMBER_POSITION_UPDATE() {}

	n1					m_nSlotNo;
	nSync::Pos			m_tPos;
	nSync::Pos			m_dPos;
};

//////////////////////////////////////////////////////////////////////////
// 파티 아이템 루팅 정보 변경
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_PARTY_LOOTING_INFO_CHANGE
{
public:
	GS2C_RECV_PARTY_LOOTING_INFO_CHANGE(){}
	~GS2C_RECV_PARTY_LOOTING_INFO_CHANGE(){}

	TItemLootingInfo m_lootingInfo;
};

//////////////////////////////////////////////////////////////////////////
// 파티 경험치 패킷 내 정보만 옴.
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_PARTY_EXP_UPDATE
{
public:
	GS2C_NTF_PARTY_EXP_UPDATE():m_nTamerGainExp(0),m_nTamerBonusExp(0),m_nTamerExp(0),
		m_nDigimonUID(0),m_nDigimonGainExp(0),m_nDigimonBonusExp(0),m_nDigimonExp(0),m_nSkillExp(0)
	{}
	~GS2C_NTF_PARTY_EXP_UPDATE(){}

	u8				m_nTamerGainExp;	// 이번에 획득한 exp
	u8				m_nTamerBonusExp;	//
	u8				m_nTamerExp; // 최종 exp
	u2				m_nDigimonUID;
	u8				m_nDigimonGainExp;
	u8				m_nDigimonBonusExp;
	u8				m_nDigimonExp;			// 최종 exp
	u4				m_nSkillExp;
	std::wstring	m_szOfferTamerName;		// 제공자 테이머 이름
};