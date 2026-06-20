#pragma once
#include "../pTamer.h"
#include "../pLimit.h"
#include "../cItemData.h"
#include "../pLogin.h"
#include "../pPass2.h"
#include "../ccertify.h"

struct sServerInfo
{
public:
	enum eSTATE{
		RUNNING = 0,	// 서버 가동중
		REPAIR,			// 서버 점검
		READY,			// 서버 준비
	};

	enum eCongestion
	{
		eComfortalbe,	// 쾌적
		eNormal,		// 보통
		eCrowded,		// 혼잡		
		eFull,			// 꽉참
	};

	sServerInfo():m_nIDX(0), m_nDisable(0), m_nClusterState(0), 
				  m_nTamerNumber(0),m_bNewServer(false),
				  m_nMaxTamerSlot(0),m_nOpenTamerSlot(0)
	{};

	~sServerInfo()
	{};

public:
	n4				m_nIDX;					// 서버 인덱스 번호
	u1				m_nDisable;				// 서버 사용 가능 여부, 0 : 가동중, 1: 점검중, 2: 대기중	eSTATE
	u1				m_nClusterState;		// 0 => 쾌적, 1 => 보통, 2 => 혼잡, 3 => 꽉참				eCongestion
	u1				m_nTamerNumber;			// 보유 캐릭서 갯수
	bool			m_bNewServer;			// The server is new ?
	std::wstring	m_szClusterName;		// Server Name
	u1				m_nMaxTamerSlot;		// Max Tamer Slot
	u1				m_nOpenTamerSlot;		// Tamer Slot Opened
};

struct sSimpleCharacterInfo
{
	sSimpleCharacterInfo():m_nMapID(0), m_nTamerObjectTBIdx(0), m_nTamerLevel(1)
		,m_nDigimonObjectTBIdx(0),m_nDigimonLevel(1),m_nDigimonScale(0),m_nPat_Leader(0),m_nSlotNumber(-1)
		,m_nRelocateState( 0 )
	{};

	~sSimpleCharacterInfo()
	{};

public:
	n1				m_nSlotNumber;			// Tamer slot number
	n2				m_nMapID;				// Map ID
	n4				m_nTamerObjectTBIdx;	// 테이머 오브젝트 테이블 인덱스
	u1				m_nTamerLevel;			// Tamer Level
	u4				m_nDigimonObjectTBIdx;	// 디지몬 오브젝트 테이블 인덱스 
	u1				m_nDigimonLevel;		// Digimon Level
	n2				m_nDigimonScale;		// Digimon Size
	u2				m_nPat_Leader;			// 팻 인덱스
	cItemData		m_TamerEquip[nLimit::Equip];	// 테이머 장비 정보
	std::wstring	m_szTamerName;			// Tamer Name
	std::wstring	m_szDigimonName;		// Digimon Name
	n4				m_nRelocateState;		// 테이머 이전 상태
};


namespace nsLogin
{
	enum eLOGINSUBTYPE{
		NONE = 0,
		LOGIN = 1,
		CHECK2NDPASS = 2,
		SET2NDPASS = 3,

		MAX = 255
	};
}
