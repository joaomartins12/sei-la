
#pragma once 

#define _ONLY_KOR(x)		__T(x)			// 현재 한국어 버젼에서만 사용
#define _ONLY_ENG(x)		__T(x)			// 현재 영문 버젼에서만 사용
#define _CUR_ENG(x)			__T(x)			// 이미 영문 사용중
#define _ERROR_MSG(x)       __T(x)			// 에러 메세지용 - 현재 한글로 표기
#define _DEBUG_MSG(x)       __T(x)			// 디버그 메세지용 - 영문으로 표기


#define FONT_WHITE		NiColor( 0.98f, 0.98f, 0.98f )
#define FONT_RED		NiColor( 0.98f, 0.1f, 0.1f )
#define FONT_GREEN		NiColor( 0, 0.98f, 0 )
#define FONT_YELLOW		NiColor( 0.98f, 0.98f, 0 )
#define FONT_GOLD		NiColor( 247/255.0f, 211/255.0f, 153/255.0f )
#define FONT_BLACK		NiColor( 0.02f, 0.02f, 0.02f )
#define FONT_GLAY		NiColor( 0.5f, 0.5f, 0.5f )
#define FONT_NEWGOLD	NiColor( 222/255.0f, 204/255.0f, 48/255.0f )
#define FONT_DARKBLUE	NiColor(0.5f, 0.8f, 1.0f)

#define DEFAULT_NAMECOLOR NiColor( 139.0f/255.0f, 248.0f/255.0f, 8.0f/255.0f )
#define PARTY_NAMECOLOR	NiColor( 28/255.0f, 102/255.0f, 193/255.0f )

#define BATTLE_SPEED_NORMAL				1.4f				// 평타
#define BATTLE_SPEED_SKILL				1.4f				// 스킬
#define BATTLE_SPEED_RANGE				1.1f				// 발사체

#define DIMA_OAUTH_KEY_VALUE		"DiMaOAuthKey.value"

#ifdef VERSION_USA
	#include "common_vs2019/Define/Define_USA.h"
	#include "EngDefine.h"

#elif VERSION_TH
	#include "common_vs2019/Define/Define_TH.h"
	#include "ThaiDefine.h"

#elif VERSION_TW
	#include "common_vs2019/Define/Define_TW.h"
	#include "TaiwanDefine.h"

#elif VERSION_HK
	#include "common_vs2019/Define/Define_HK.h"
	#include "HongKongDefine.h"

#else
	#include "common_vs2019/Define/Define_KOR.h"
	#include "KorDefine.h"

#endif

#ifdef NEW_ATTAND_EVENT
	#define REWARD_SYSTEM_UI							//2017-03-08-nova	접속보상 시스템
#endif

#ifdef ITEM_TIME_PASS_ACTIVE
	#define	ITEM_USE_TIME_PASS							//2017-04-04-nova	시간제아이템
#endif

#ifdef SERVER_KSW_DIGIMON_ARENA_RANK_20180312
	#define SDM_INFINITEWAR_RANKING_SYSTEM_20180312		// 디지몬 아레나 랭킹 시스템
#endif

//#define SDM_VIP_SYSTEM_20181105							// VIP SYSTEM

#ifdef SERVER_KSW_DIGIMONBOOK_EXTEND_181116
	#define SDM_DIGIMONBOOK_EXTEND_20181126				// 디지몬 도감의 계열체 갯수 증가
#endif

#ifdef SERVER_KSW_SKILL_LV_EXPEND_181205
	#define SDM_DIGIMONSKILL_LV_EXPEND_20181206			// 디지몬 스킬 레벨 확장
#endif

#ifdef SERVER_KSW_MACRO_RENEW_181214
	#define SDM_MACRO_PROTECT_RENEW_20181217			// 메크로 프로텍터 리뉴얼
	#ifdef SERVER_KSW_MACRO_QUESTION_190517
		#define SDM_MACRO_PROTECT_QUESTION_20190524
	#endif
#endif

#ifdef SERVER_KSW_XEVOLUTION_1_180615
	#define SDM_TAMER_XGUAGE_20180628					// X-항체 게이지 및 아이템 사용 관련 디파인
#endif

#ifdef SERVER_KSW_ADD_BRACELET_181106
	#define SDM_TAMER_EQUIP_ADD_BRACELET_20181031			// 테이머 신규 장비 팔찌 추가 활성화시 CsGBChar 라이브러리의 define 팔일에 추가해 줘야함.
#endif

#ifdef SERVER_KOREANDNS_DIGIMON_TRANSCENDENCE_190412
	#define SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507	// 디지몬 초월 관련 업데이트
#endif

#ifdef SERVER_KOREANDNS_UIRENEWAL_ITEMSCANMODE_190318
	#define UI_ITEMSCAN_RENEWAL							// 아이템 스캔 개선
#endif

#define UI_INVENTORY_RENEWAL							// Inventory
#define WAREHOUSE_SERVER_SORT							// Warehouse Sort


#ifdef SERVER_KOREANDNS_LINKEDACCESSORY_190822
	#define LJW_ENCHANT_OPTION_DIGIVICE_190904		// 인챈트 가능한 옵션 디지바이스 추가 - CsGBChar - define에 추가해야 함
#endif

#ifdef SERVER_KSW_ITEM_TIME_CHARGE_190716
	#define LJW_EQUIPSYSTEM_190724					// 장비, 속성 합성기 통합 + 장비 충전 시스템 추가
#endif

// 모든 국가 적용
#define SDM_CHET_ULIST_LOG_FILE_SAVE_20190110		// 치트키 ulist를 사용하였을 때 로그 파일로 저장 되도록 추가

#ifdef SERVER_KSW_DAILYCHECKEVENT_191014
	#define LJW_DAILYCHECKEVENT_191030				// 일일 출석부 이벤트
#endif

#define TOOLTIP_SKILL_ADD_BASEATT				// 스킬 데미지 개선 - 스킬 + 일반 공격력 = 전 국가 적용됨

#ifdef SERVER_KSW_DIGIMON_EFFECT_CHANGE_191230
	#define SDM_DIGIMON_PARTSSYSTEM_20200115	// 디지몬 스킨 변경 아이템 추가
#endif

#ifdef _DEBUG
#define USECONSOLE 1
#endif

#include "_LanConvert.h"
