#pragma once 

	// 본섭 로그인 IP주소
	//#define		ACCOUNT_IP			"61.91.124.121"
//	#define		ACCOUNT_IP			"94.237.64.220"
	#define		ACCOUNT_PORT		7029


	#define	NAME_MIN_LEN				3
	#define NAME_MAX_LEN				12
	#define NAME_GUILD_MAX_LEN			14
	#define NAME_FRIEND_COMMENT_LEN		23		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
	#define NAME_BLOCK_COMMENT_LEN		23		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
	#define NAME_GUILDRANK_COMMENT_LEN	21		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
	#define VERSION_FILE				"LauncherLib\\vTDMO.ini"

	#define CHAT_LIMITE_SHOUTING_LV		1	// 외치기 제한 레벨

	#include "_Lan_Thailand.h"
	//#define PLAY_PANELTY						//	봇 2차 - 시간에 의한 패널티	
	#define CHEAT_MASTER						//	마스터 치트 사용 여부
	#define VERSION_ERROR_ROOLBACK				//	버젼 에러시 롤백
	#define ADD_ACHIEVE_COSTUME					//	추가 코스튬 업적

	#define GM_COSTUME							//	GM 코스츔 - 이펙트 효과	
	#define	BATTLE_MATCH						//  배틀 정보 받기
								


	#define FREETYPE_LENGTH_OVER				// 서부마을동쪽 보안관 지역 및 동쪽에서 다른곳으로 포탈 이동시 client 끊기는 현상 수정	lks007 12.07.25
	#define THAI_CHAR_CHECK						// 태국 문자 로직(입력된 문자가 제대로 된 문자인지 확인하는 알고리즘) 체크	lks007 12.08.07
	#define THAI_INPUT_PASTE					// 태국어 문자 붙여넣기 시 기존 input이 ANSI형식에서 인식이 되지 않아 unicode 형식 추가함	lks007	12.08.20
	#define THAI_WORDTRANSFORM					// 태국어로 생성된 Tamer 및 Digimon, 기타 String 문자 변환 lks007	12.10.15 
	#define THAI_PASSWORDLENGTH					// 태국 비밀번호 15자리까지 입력 가능하도록 변경(캐릭터 및 디지몬 삭제 시 입력 자리 수 변경)

	//#define THAI_TUTORIAL						// 튜토리얼 진행 사항 여부 lks007	// 신규 닷츠센터 추가로 주석 처리
	#define THAI_LEVELEFFECT					// 태국 레벨 이펙트 수정 lks007
//	#define THAI_VERSIONCHECK					// 태국 게임 버전 체크 lks007
	#define THAI_SPECIALWORD					// ALT키 로 특수문자 입력 안되도록 변경 lks007	12.11.01
	#define THAI_CHANNELDOWN					// 태국 채널 다운 시 메세지 출력 관련 코드 수정 lks007	12.11.01




	#define CASH_ITEM_TOOLTIP					// 일부 캐쉬템이 툴팁에서 이벤트 아이템으로 나오는것 관련 수정	lks007  13.07.03

	#define MINIGAME								//미니게임 chu8820 (13.2.20)

/// 13.09.12 파일섬 패치
	//#define EXP_UNLIMITED						// 퀘스트 수행 레벨과 디지몬 레벨이 15렙 이상 차이나면 경험치 못받는것 98렙 차이로 수정  13.01.15. lks007

/// 13.11.07 패치(테이머 레벨 99 이펙트, 카드 이벤트, 호박 폭탄
//	#define TAMERLEVEL_EFFECT					// 테이머 레벨 99가 되면 생기는 이펙트 13.10.29  lks007

/// 13.11.28 패치( 호박 폭탄 )
	#define TRICK_ORTREAT						// 할로윈 이밴트 추가 2012/10/15	vf00



// 14.05.15 패치
	#define REPURCHASE								//2014.01.08	상점 판매 아이템 재구매 기능 추가				chu8820
	
// 14.05.29 패치
	#define TUTORIAL_DISABLE						//2013.11.05	튜토리얼 비활성							vf00


// 14.06.27 패치
	#define BUFFINFO_FIX							//2014.04.30	버프 정보(버프아이콘) UI 수정					chu8820
	#define LEVELUP_EFFECT_LOOP						//2014.01.23	경험치 폭탄(?) 아이템 사용 시 레벨업 이펙트를 상승된 레벨만큼 보여주기		lks007

// 14.07.17 패치
 	#define	USE_CENTER_BAR							// 센터바
	#define CONSIGNMENT_BALLOON_CASH				//2013.12.11	위탁 상점 말풍선 캐시 아이템 타입 추가	lks007
	#define CONSIGNMENT_SALE						//2012.09.25	위탁 상점 찾기 기능						lks007

// 14.07.31 패치
	#define MONSTER_SKILL_GROWTH					//2013.12.18	몬스터 스킬::성장 추가					chu8820


// 14.12.11 패치
	#define EMPLOYMENT_TAMERNAME					//2013.06.11(chu8820)	위탁상점 오픈 시 테이머 이름 출력

// 14.12.24 패치
	#define	KEYBOARD_MOVE							//2014.09.17(chu8820)	키보드 이동(인던 추가 관련)

// 15.01.08 패치
	#define SIMPLE_TOOLTIP							//2013.11.13(vf00)		UI 툴팁
	#define MAKE_TACTICS_UI_UPGRADE					//2014.05.22(kimhoshok)	디지타마 부화창 중급 데이터 사용 UI 수정
#ifdef MAKE_TACTICS_UI_UPGRADE
	#define TACTICS_USE_DATA_LV2					//(kimhoshok)			중급 데이터 사용
#endif


// 15.05.19
	#define GUILD_RENEWAL						//2014.06.25(chu8820)	길드 시스템 리뉴얼(길드스킬)

// 16.01.28
#define NEW_SHARESTASH						//2012.08.06(vf00)		공용 창고, 정리기능 신규추가				
#define NEW_WAREHOUSESORT					//2012.08.03(vf00)		창고 정리 리뉴얼
#ifdef NEW_WAREHOUSESORT
	#define	WAREHOUSESORT					//2012.08.22(vf00)		창고 정리 소스 보호
#endif



#define DAMAGE_METER						//2014.11.05(chu8820)	데미지 미터기 (전투통계)

#define COSTUME_GENDER						//2014.02.11(kimhoshok)	남성 전용, 여성 전용 코스튬 2604, 2605
#define COSTUME_TAMER_ONLY					//2014.04.29(kimhoshok)	테이머 전용 코스튬 추가

#define SDM_CASHITEM_TRADE_LIMITED_20170214	// 캐쉬 아이템  거래 제한 기능



#define EGG_PREFERMENT						//2014.07.09(lks007)	강용알 스캔하여 나온 아이템으로 한단계 높은 강용알로 교환 (미스터리 머신 사용)


#define SDM_CASHSHOP_CASH_INTEGRATION_20181121	// 홍콩/대만/태국의 경우 보너스 캐쉬가 없기 때문에 통합된 UI로 표시되게 함.

//#define SDM_CASHSHOP_GIFT_SYSTEM_HIDE_20181207	// 선물하기 기능 막기

#define SDM_DIGIMON_SHOW_SKILLEXP_TABLE_20190212	// 디지몬 스킬 경험치표시를 테이블에서 참조 하도록 수정


#define MAP_REGION_SHOW						// 맵 - 지역명 표시

#define LJW_TACTICS_SLOT_EXTEND_190529		// 19.09.26 용병 슬롯 확장
#define MASTERS_MATCHING					//2013.11.20(chu8820)	마스터즈매칭 해당 ifdef들어간 곳 pop( nMyTeam ); 주석 풀어줄 것

	//========================================================================================
	//
	//  배포별 분리
	//
	//========================================================================================

#ifdef _GIVE

//#define	DEF_CORE_NPROTECT				// 2015.06.08	게임 가드 적용
//#define SDM_RESOURCE_CHECKER_ENABLE		// 리소스 변조 검사 기능 활성화

//#ifdef VERSION_QA
//	#define DEF_CORE_NPROTECT_TEST	// 2015.06.08	게임 가드 적용// 알파 빌드일 때만 활성
//#endif

#elif NDEBUG
	#define USE_DMO_INI						//	ini파일에서 초기 설정값 읽어옴
	#define SKIP_LOGO						// 로고 스킵

#else

	#define USE_DMO_INI					//	ini파일에서 초기 설정값 읽어옴
	#define SKIP_LOGO						// 로고 스킵
#endif