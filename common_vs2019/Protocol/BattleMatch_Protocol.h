#pragma once

#include "BattleMatch_Define.h"
//////////////////////////////////////////////////////////////////////////
// pDuel::Info
// 내 배틀 매칭 등록 요청 패킷
//////////////////////////////////////////////////////////////////////////
class C2GS_SEND_BATTLEMATCHING_MY_INFO
{
public:
	C2GS_SEND_BATTLEMATCHING_MY_INFO():m_nPacketId( pDuel::Info )
	{}

	~C2GS_SEND_BATTLEMATCHING_MY_INFO(){};

public:
	WORD		GetPacketID() const	{return m_nPacketId;}

private:
	WORD		m_nPacketId;	
};

//////////////////////////////////////////////////////////////////////////
// pDuel::Info
// 내 배틀 매칭 정보를 받는다.
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_BATTLEMATCHING_MY_INFO
{
public:
	GS2C_RECV_BATTLEMATCHING_MY_INFO():m_nBattleTicketPoint(0)
	{}

	~GS2C_RECV_BATTLEMATCHING_MY_INFO(){};

public:	
	n8							m_nBattleTicketPoint;		//	보유 배틀 티켓
	std::map<n1, sMainTypeData>	m_nBattleTypeUserData;		//	모드별 배틀 정보
};

//////////////////////////////////////////////////////////////////////////
// pDuel::MatchRegister
// 배틀 매칭 등록 요청 패킷
//////////////////////////////////////////////////////////////////////////
class C2GS_SEND_BATTLEMATCHING_REGISTER 
{
public:
	C2GS_SEND_BATTLEMATCHING_REGISTER():m_nPacketId( pDuel::MatchRegister ),m_nBattleMainMode(0), m_nBattleSubMode(0), m_nNpcIdx(0),m_nMapIdx(0)
	{}

	~C2GS_SEND_BATTLEMATCHING_REGISTER(){};
	
public:
	n1		m_nBattleMainMode;	// 1 : 팀 대전, 2 ...
	n1		m_nBattleSubMode;	// 0 : 없음. 1 : 1vs1, 2 : 2vs2 ...
	u4		m_nNpcIdx;			// 배틀 등록 NPC IDX
	u4		m_nMapIdx;			// 배틀 등록 맵 IDX

	WORD		GetPacketID() const	{return m_nPacketId;}

private:
	WORD		m_nPacketId;	
};

//////////////////////////////////////////////////////////////////////////
// pDuel::MatchRegister
// 배틀 등록 대기 요청에 대한 결과 값
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_BATTLEMATCHING_REGISTER
{
public:
	GS2C_RECV_BATTLEMATCHING_REGISTER() : m_nResult(0),m_nBattleMainMode(0),m_nBattleSubMode(0),m_nMapIdx(0)
	{}
	~GS2C_RECV_BATTLEMATCHING_REGISTER(){};

public:	
	n4 m_nResult;	// 결과 성공, 실패(DM_MESSAGE Table 인덱스 번호)
	u1 m_nBattleMainMode;	// 1 : 팀 대전, 2 ...
	u1 m_nBattleSubMode;	// 0 : 없음. 1 : 1vs1, 2 : 2vs2 ...
	u4 m_nMapIdx;			// 배틀 등록 맵 IDX
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// pDuel::MatchRegiCancel
// 배틀 등록 취소
//////////////////////////////////////////////////////////////////////////
class C2GS_SEND_BATTLE_REGISTER_REQUEST_CANCEL
{
public:
	C2GS_SEND_BATTLE_REGISTER_REQUEST_CANCEL():m_nPacketId( pDuel::MatchRegiCancel )
	{}
	~C2GS_SEND_BATTLE_REGISTER_REQUEST_CANCEL(){};

	WORD		GetPacketID() const{	return m_nPacketId; }

private:
	WORD		m_nPacketId;	
};

//////////////////////////////////////////////////////////////////////////
// pDuel::MatchRegiCancel
// 배틀 등록 대기 취소에 대한 결과
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_BATTLE_REGISTER_REQUEST_CANCEL
{
public:
	GS2C_RECV_BATTLE_REGISTER_REQUEST_CANCEL() : m_nResult(0)
	{}
	~GS2C_RECV_BATTLE_REGISTER_REQUEST_CANCEL(){};

public:	
	n4	m_nResult;	// 성공, 실패(DM_MESSAGE Table 인덱스 번호)
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// pDuel::TeamReadyInfo
// 배틀 매칭이 완료 瑛?때 플레이어 정보 패킷
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_BATTLE_READY_STATE_ASK
{
public:
	GS2C_NTF_BATTLE_READY_STATE_ASK() : m_nBattleIdx(0),m_nMapIdx(0),m_cBattleMod(0),m_cBattleType(0)
	{}
	~GS2C_NTF_BATTLE_READY_STATE_ASK(){};

public:		
	u4					m_nBattleIdx;
	u4					m_nMapIdx;
	u1					m_cBattleType;// 1 : 팀전, 2...
	u1					m_cBattleMod;// Vs Type(1 Vs 1, 2 Vs 2, ... )
	// int : TeamCode, Player Info
	std::map<n1, std::list<sBattlePlayerInfo>>	m_mapTeamInfo;
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// pDuel::Ready 
// 서버에 배틀 준비 상태 패킷 보냄
//////////////////////////////////////////////////////////////////////////
class C2GS_SEND_BATTLE_REGISTER_READY_STATE
{
public:
	C2GS_SEND_BATTLE_REGISTER_READY_STATE() : m_nPacketId( pDuel::Ready),m_nReadyState(0),m_nBattleIdx(0)
	{}
	~C2GS_SEND_BATTLE_REGISTER_READY_STATE(){};

public:
	WORD		GetPacketID() const	{return m_nPacketId;}

	n1			m_nReadyState;		// (0 : 취소, 1 : 준비)
	n4			m_nBattleIdx;

private:
	WORD		m_nPacketId;	
};

//////////////////////////////////////////////////////////////////////////
// pDuel::Ready
// 내가 보낸 배틀 준비 상태 패킷 결과 받음.
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_BATTLE_REGISTER_READY_STATE
{
public:
	GS2C_RECV_BATTLE_REGISTER_READY_STATE() : m_nResult(0),m_nReadyState(0)
	{}
	~GS2C_RECV_BATTLE_REGISTER_READY_STATE(){};

public:
	n4		m_nResult;	// 성공, 실패(DM_MESSAGE Table 인덱스 번호)
	n1		m_nReadyState;		// (0 : 취소, 1 : 준비)
};


//////////////////////////////////////////////////////////////////////////
// pDuel::ReadyResultNoti
// 다른 플레이어들의 배틀 준비 상태 패킷
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_BATTLE_REGISTER_READY_STATE
{
public:
	GS2C_NTF_BATTLE_REGISTER_READY_STATE():m_nReadyState(0),m_nTamerIDX(0)
	{}
	~GS2C_NTF_BATTLE_REGISTER_READY_STATE(){};

public:
	n1		m_nReadyState;	//	0 : 취소, 1 : 수락
	u4		m_nTamerIDX;	//	테이머 IDX
};

//////////////////////////////////////////////////////////////////////////
// pDuel::RemoveDuel
// 다른 플레이어들의 배틀 준비 취소 및 기타 배틀 매칭이 취소 瑛?때의 패킷
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_BATTLE_MATCH_CANCEL
{
public:
	GS2C_NTF_BATTLE_MATCH_CANCEL():m_nResult(0)
	{}
	~GS2C_NTF_BATTLE_MATCH_CANCEL(){};

public:
	n4		m_nResult;	// 배틀 매칭 취소 이유
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// pDuel::ReadyComplete
// 모든 유저가 수락 후 맵 이동 전에 날라오는 패킷
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_BATTLE_MATCH_COMPLETE
{
public:
	GS2C_NTF_BATTLE_MATCH_COMPLETE()
	{}
	~GS2C_NTF_BATTLE_MATCH_COMPLETE(){};
};
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// pDuel::Start
// 배틀 시작 패킷
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_BATTLE_START
{
public:
	GS2C_NTF_BATTLE_START() : m_nStart(0), m_nEnd(0), m_nRound(0),m_nBattleTicketPoint(0),m_nBattleMainMode(0),m_nPlayCount(0)
	{}
	~GS2C_NTF_BATTLE_START(){};

public:
	u4		m_nStart;				// 경기 시작 시간
	u4		m_nEnd;					// 경기 종료 시간
	u4		m_nRound;				// 경기 진행 시간
	n8		m_nBattleTicketPoint;		// 최종 보유 배틀 티켓
	u1		m_nBattleMainMode;		// 1 : 팀 대전, 2 ...
	n4		m_nPlayCount;			// 최종 플레이 카운트
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// pDuel::End
// 배틀 종료 패킷
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_BATTLE_END
{
public:
	GS2C_NTF_BATTLE_END() : m_nWinTeamCode(0)
	{}
	~GS2C_NTF_BATTLE_END(){};

public:	
	n1			m_nWinTeamCode;		// 승리 팀 (0:무승부)
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// pDuel::Result
// 배틀 종료 보상 패킷
//////////////////////////////////////////////////////////////////////////
class GS2C_NTF_BATTLE_REWARDS
{
public:
	GS2C_NTF_BATTLE_REWARDS(): m_nDuelPoint(0)
	{}
	~GS2C_NTF_BATTLE_REWARDS(){};

public:	
	n1							m_nBattleMainType;	// 획득한 모드
	n4							m_nDuelPoint;	// 획득 포인트
	std::list<sRewardItemInfo>	m_Rewardlist;	// 배틀 보상 아이템
};
//////////////////////////////////////////////////////////////////////////
