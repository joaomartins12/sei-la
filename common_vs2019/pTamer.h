#pragma once

#include "nlib/base.h"
#include "pType.h"
#include "pLength.h"
#include "pParameter.h"
#include "nScope.h"

namespace pTamer
{
	enum
	{
		Start = nScope::Tamer,

		List,				// tamer list

		CheckDoubleName,	// Check for name already used

		Create, // 테이머 생성(생성시 digimon, digivice 같이 생성)
		Remove, // 제거시 digimon, digivice 같이 제거
		Select, // 해당 map server로 접속을 유도해준다.

		CreateSuccess,
		CreateFailure,

		SelectSuccess,
		SelectFailure,

		EquipItem,

		ChangeName,		// 테이머 이름 변경

		AddShopItem,

		Option,			// 옵션 변경시 송수신

		Change,			// 외형을 영구히 변경

		CreateWaitTime,	// 생성 대기 시간

		Message,		// 테이머에게 보내는 메세지
		MessageConfirm,	// 메세지 확인

		BuffSave,		// 버프 신규 저장
		BuffUpdate,		// 기존 버프 슬롯 수치 변경

		BuffStart,		// 버프 사용 시작, 본인에게만 송신됨
		BuffEnd,		// 버프 종료, 본인에게만 송신됨
		BuffInfo,		// 현재 적용중인 모든 버프 정보, 본인에게만 송신됨

		BuffActive,		// 버프 활성화
		BuffInactive,	// 버프 비활성화

		RideOn,			// 디지몬을 타다.
		RideOff,		// 디지몬에서 내리다.

		Skill,			// 테이머 스킬
		SkillSuccess,	// 테이머 스킬 성공
		SkillEnd,		// 테이머 스킬 종료

		ItemSkillApply,	// 아이템 적용 테이머 스킬
		ItemSkillExpire,// 아이템 적용 테이머 스킬 만료
		ItemSkillRemove,// 아이템 적용 테이머 스킬 삭제

		CardInfoAll,
		CardBookmark,

		BattleTicketPointSave, // 배틀코인 변환 한 값 저장
		BattleTicketPointNoti, // 배틀코인 값 통보 저장(변환값이 아님 절대값)

		Relocate,				// 테이머 이전 요청
		RelocateAccept,
		RelocateReject,

		CheckDoubleNameResult,
		CheckDoubleNameFailure,

		End
	};

}; // namespace pTamer


namespace nErrCode
{
	enum eErrorCode
	{
		ConflictTamerName = 9,
		ConflictDigimonName
	};
};


namespace nTamer
{
	bool IsValidType(uint nType);
//	inline bool CheckName(char *szName, int nSize = 16) { return chkstr(szName, 2, nSize); }
//	inline bool CheckName(wchar *szName, int nSize) 
//	{ 
//#ifdef VERSION_TH	// 태국문자로 캐릭터 및 길드 생성 관련 문자 check 코드 추가  lks007 12.08.09
//		return chkstr( szName, 2, nSize );
//#elif defined VERSION_TW
//		return chkcodeTaiwan( szName, nSize ); 
//#elif defined VERSION_HK
//		return chkcodeHongKong( szName, nSize ); 
//#else
//		szName;
//		nSize;
//		return false;
//#endif
//	}

	enum eTamerType
	{
		Masaru			= 80001,
		Tohma			= 80002,
		Yoshino			= 80003,
		Ikuto			= 80004,
		Tai				= 80005,
		Mina			= 80006,
		Matt			= 80007,
		Takeru			= 80008,
		Hikari			= 80009,
		Sora			= 80010,
		Izumi			= 80014,
		Joe				= 80015,
		Takato			= 80011,
		Rika			= 80012,
		Henry			= 80013,
		KatoJeri		= 80016,
		AkiyamaRyo		= 80017,
		Hiro			= 80018,
		Ruli			= 80019,
		Kiyoshirou		= 80020
	};

	enum eType // tamer constraint
	{
		Fighter,			// Masaru,
		Master,				// Tohma,
		Volunteer,			// Yoshino,
		tIkuto,				// Ikuto,
		Attacker,			// Tai
		VolunteerMina,		// Mina
		Friendship,			// Matt
		Hope,				// Takeru
		Light,				// Hikari
		Love,				// Sora
		Knowledge = 13,		// Izumi
		Honesty = 14,		// Joe
		MaxType,
		Common = 99	// 공통
	};

	enum eParts
	{
		Head,			// cabeca
		Coat,			// casaco
		Glove,			// luva
		Pants,			// calca
		Shoes,			// sapato
		Costume,		// ropa
		Glass,			// oculos
		Necklace,		// colar
		Ring,			// anel
		Earring,		// brinco
		EquipAura,		// DigiAura
		XAI,			// XAI
		Bracelet,		// Bracelete

		MaxParts
	};

	enum eSkillType
	{
		ActiveSkill			=	1,	// 공격형 스킬
		BuffSkill			=	2,	// 버프형 스킬
		ScriptSkill			=	3,	// 스크립트 -> 비규칙적인 구현이 필요한 스킬들
		PassiveSkill		=	4,	// 패시브 스킬

		MaxSkillType
	};

	enum eScriptType
	{
		ScriptNone		=	0,
		ScriptItemUse,	// 스크립트 타입 1번
		ScriptPeace,
		ScriptSummon,	// 테이머 소환
		Script4,
		Script5,

		MaxScriptType,
	};

	enum eScriptValid
	{
		SVDigimonCareHouse	= 0x00000001,	// 디지몬 보관소
		SVJumpBuster		= 0x00000002,	// 점프 부스터
	};

	namespace Option
	{
		enum eOption
		{
			CostumeOff	= 0x00000001,	// 코스츔 보이기, 보이지 않기
		};
	};

	enum eState
	{
		None,				// 이상무
		RelocateWaitTime,	// 테이머 서버 이전 중
		RelocateComplete	// 테이머 서버 이전 완료
							// 스케줄 또는 승인이 필요하므로 complete 사용
	};
	
	enum eErrorCode
	{
		NotAllowTamerLevel,			// 테이머의 레벨이 낮음
		DisagreeTerm,				// 약관에 미동의
		JoinedGuild,				// 길드에 가입중
		OpenedCommissionShop,		// 위탁 상점이 오픈되어 있음
		CoolDown,					// 재사용 대기시간에 걸림
		NotEnoughSlot,				// 슬롯이 부족
		Unknown
	};

}; // namespace nTamer

#ifdef JHB_DIGIMONBOOK_20140107
struct stDigimonBook
{
	//n8 idx ;
	u4 DigimonType;
	u2 DigimonLevel;
#ifdef SERVER_KSW_DIGIMONBOOK_EXTEND_181116
	u8 nSlotState;
#else if
	u1 SlotState[8];
#endif
	
	u2 InchantLevel[nExtendState::OpenETStat];
	n2 Scale;
	u1 ItemGive;	
#ifdef SERVER_KSW_DIGIMONBOOK_EXTEND_181116
public:
	stDigimonBook() { memset(InchantLevel, 0, sizeof(InchantLevel)); }

	inline void OpenSlotState(n4 nSlotIdx)
	{
		nSlotState |= (u8)1<<(nSlotIdx);
	}
	inline void CloseSlotState(n4 nSlotIdx)
	{
		nSlotState &= ~((u8)1<<(nSlotIdx));
	}
	inline bool IsSlotOpen(n4 nSlotIdx)
	{
		return (nSlotState & ((u8)1<<(nSlotIdx))) != 0;
	}
#endif
};
#endif
#ifdef JHB_ADD_DIGIMONBOOK_DECK_20140227
namespace nDigimonBookDeck
{
	enum eCondition
	{
		Passive		= 1,		//패시브 스킬로 계속 지속
		Perssive,				//패시브로 계속 지속되지만 일정확률로 발동
		Active					//발동시 n초간 유지
	};
	enum eType
	{
		None		= 0,		//발동조건이 none이면 발동형이 아닌 패시브 
		Basic,					//일반공격시 랜덤으로 발동
		Skill					//스킬공격시 랜덤하게 발동
	};
	enum eOption
	{
		AT			= 1,		//공격력 X% 증가
		SP,						//스킬피해 X% 증가
		CT,						//크리티컬 피해량 X% 증가
		CoolReset,				//쿨타임 초기화
		HP,						//최대HP X% 증가
		AS,						//공격속도 증가
		MaxOption				//최대옵션개수
	};

};
struct stBookDeck
{
	u4 nGroupIdx;
	u2 nCondition[3];
	u2 nATK_Type[3];
	u2 nOption[3];
	u2 nValue[3];
	u2 nRandValue[3];
	u2 nTime[3];
	cList <u4*> DigimonList;
};
#endif
