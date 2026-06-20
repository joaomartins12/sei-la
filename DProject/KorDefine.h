#pragma once 


//#define		ACCOUNT_IP			"211.252.86.66"	// 국내 게임서버 이전으로 접속 IP 변경(KT 클라우드)
// #ifdef VERSION_QA// 국내 게임서버 이전으로 접속 IP 변경(네이버 클라우드)
// 	#define		ACCOUNT_IP			"1.255.51.120"	
// #else
// 	#define		ACCOUNT_IP			"1.255.55.185"
// #endif

//#ifdef VERSION_QA// 국내 게임서버 이전으로 접속 IP 변경(AWS)
//#define		ACCOUNT_IP			"13.124.249.85"	
//#else
//#define		ACCOUNT_IP			"76.223.32.7"
//#endif

#define ACCOUNT_IP "26.60.202.44"
#define		ACCOUNT_PORT		7029

#include "_Lan_Korea.h"

#define	NAME_MIN_LEN				2
#define NAME_MAX_LEN				8
#define NAME_GUILD_MAX_LEN			14
#define NAME_FRIEND_COMMENT_LEN		23	//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define NAME_BLOCK_COMMENT_LEN		23	//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define NAME_GUILDRANK_COMMENT_LEN	21	//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define VERSION_FILE				"LauncherLib\\vDMO.ini"
#define SPLASH_LOGO

#define CHAT_LIMITE_SHOUTING_LV		20	// 외치기 제한 레벨

#define LANGAUGE_SELECT					// 언어 선택
#define GUILD_MARK_DOWN					// 길드 마크 다운로드
#define USE_BARCODE_REDER				// 바코드 리더기 사용
#define USE_ID_SAVE						// 아이디 세이브
#define WEB_INFOMATION					// 웹정보 사용

#define USE_SHUTDOWN					// 셧다운 제도 시행
//#define USE_SHUTDOWN_NOTICE_10		// 저녁 10시에 인게임 공지		

#define SYNC_DATA_LIST					// 싱크 데이터 매프레임 처리 및 우선 순위
#define SYNC_DATA_LIST_FRAME_1	1		// 싱크 데이터 1의 프레임단위
#define SYNC_DATA_LIST_FRAME_2	1		// 싱크 데이터 2의 프레임단위
#define	USE_CENTER_BAR					// 센터바

#define	ENABLE_KEYBOARD_CAMERA			// 키보드 카메라 회전	
#define	MATCHING_SYSTEM					// 매칭 시스템 UI
#define	BATTLE_MATCH					// 배틀 정보 받기

//	#define GUILD_INVITATION				// 길드 초대 거부
//	#define STAND_ARRANGE_SERVER			// 서버 고정 배치

/************************************************************************/
/*								khc0729									*/
/************************************************************************/


#define SIMPLEFICATION						//2013.02.27(khc0729)	간략화

//#define FIREWORK							//2013.05.01(khc0729)	폭죽 리뉴얼
//#define ADVENTURE_TAMER_CHANGE			//2013.05.07(khc0729)	어드벤처 -> 세이버즈 테이머 교체

/************************************************************************/
/*								 vf00									*/
/************************************************************************/

#define NEW_WAREHOUSESORT					//2012.08.03(vf00)		창고 정리 리뉴얼
#ifdef NEW_WAREHOUSESORT
#define	WAREHOUSESORT					//2012.08.22(vf00)		창고 정리 소스 보호
#endif
#define NEW_SHARESTASH						//2012.08.06(vf00)		공용 창고, 정리기능 신규추가

//#define TRICK_ORTREAT						//2012.10.15(vf00)		할로윈 이밴트 추가

//#define SERVER_WAITING_LINE				//2013.01.15(vf00)		서버 대기열 추가

#define CROSSWARS_SYSTEM					//2013.07.29(vf00)		디지몬 크로스워즈 시스템


//#define TUTORIAL_DISABLE					//2013.11.05(vf00)		튜토리얼 비활성
#define SIMPLE_TOOLTIP						//2013.11.13(vf00)		UI 툴팁


/************************************************************************/
/*								lks007									*/
/************************************************************************/
#define CONSIGNMENT_SALE					//2012.09.25(lks007)	위탁 상점 찾기 기능

//#define EXP_UNLIMITED						//2013.01.15(lks007)	퀘스트 수행 레벨과 디지몬 레벨이 15렙 이상 차이나면 경험치 못받는것 98렙 차이로 수정

#define DEBUFF_ITEM							//2013.08.07(lks007)	디버프 아이템


#define CONSIGNMENT_BALLOON_CASH			//2013.12.11(lks007)	위탁 상점 말풍선 캐시 아이템 타입 추가

#define LEVELUP_EFFECT_LOOP					//2014.01.23(lks007)	경험치 폭탄(?) 아이템 사용 시 레벨업 이펙트를 상승된 레벨만큼 보여주기
#define SERVER_MONEY_SAVE_CHEAT				//2014.01.16(lks007)	서버내 모든 유저의 돈을 db에 저장하는 치트키


#define TAMER_IDLEANIMATION					//2014.03.25(lks007)	테이머가 자리 비움 상태일때 서버와 데이터를 주고 받으면 일어서야 한다




#define MEGAPHONE_BAN						//2014.05.28(lks007)	확성기 차단

#define EGG_PREFERMENT						//2014.07.09(lks007)	강용알 스캔하여 나온 아이템으로 한단계 높은 강용알로 교환 (미스터리 머신 사용)

//#define NECKLACE_ACCESSORY					//2014.08.13(lks007)	목걸이 악세서리 (디파인은 주석처리 작업한 부분 확인 시 디파인으로 검색 ㄱㄱ)


#define CONSIGNMENT_CREATE_RESTRICT			//2015.03.18(lks007)	위탁상점이 NPC 주변 및 포탈 주변의 가까운 위치에 생성되지 않도록... 

/************************************************************************/
/*								chu8820									*/
/************************************************************************/
#define MINIGAME							//2013.02.20(chu8820)	미니게임

// #define UI_RENEWAL						//2013.05.16(chu8820)	UI리뉴얼 05.16 ~ 10.15 (취소)
#define EMPLOYMENT_TAMERNAME				//2013.06.11(chu8820)	위탁상점 오픈 시 테이머 이름 출력

#define PC_BANG_SERVICE						//2013.08.29(chu8820)	웹젠 PC방 퍼블리싱
// #define PC_BANG_SERVICE_TEST				//2013.08.29(chu8820)	웹젠측 테스트 클라 -릴리즈모드

#define MASTERS_MATCHING					//2013.11.20(chu8820)	마스터즈매칭 해당 ifdef들어간 곳 pop( nMyTeam ); 주석 풀어줄 것

#define CHEAT_ITEMRANK						//2013.11.21(chu8820)	아이템랭크 치트키 추가
#define CHAT_BAN							//2013.12.04(chu8820)	운영툴::채팅 금지
#define MONSTER_SKILL_GROWTH				//2013.12.18(chu8820)	몬스터 스킬::성장 추가

#define CHANGE_CHANNEL_LIMIT				//2013.12.31(chu8820)	채널 이동시 포화 채널일 경우 이동 제한
#define REPURCHASE							//2014.01.08(chu8820)	상점 판매 아이템 재구매 기능 추가

#define BUFFINFO_FIX						//2014.04.30(chu8820)	버프 정보(버프아이콘) UI 수정

#define GUILD_RENEWAL						//2014.06.25(chu8820)	길드 시스템 리뉴얼(길드스킬)

#define	KEYBOARD_MOVE						//2014.09.17(chu8820)	키보드 이동(인던 추가 관련)


#define DAMAGE_METER						//2014.11.05(chu8820)	데미지 미터기 (전투통계)


/************************************************************************/
/*							    kimhoshok                               */
/************************************************************************/
#define COSTUME_GENDER						//2014.02.11(kimhoshok)	남성 전용, 여성 전용 코스튬 2604, 2605
#define COSTUME_TAMER_ONLY					//2014.04.29(kimhoshok)	테이머 전용 코스튬 추가

//#define TARGETUI_TIMEMASK_DELETE			//2014.05.07(kimhoshok)	타겟 UI에서 버프의 타임 마스크 렌더 제거
//#define ZONEMAP_CLICK_MOVE				//2014.05.21(kimhoshok)	존맵(KEY : m)에서 Ctrl + R버튼으로 이동하기 - 업데이트 미정


#define MAKE_TACTICS_UI_UPGRADE				//2014.05.22(kimhoshok)	디지타마 부화창 중급 데이터 사용 UI 수정
#ifdef MAKE_TACTICS_UI_UPGRADE
#define TACTICS_USE_DATA_LV2				//(kimhoshok)			중급 데이터 사용
//#define TACTICS_USE_DATA_LV3
//#define TACTICS_USE_DATA_LV4
//#define TACTICS_USE_DATA_LV5
#endif

/************************************************************************/
/*								nova									*/
/************************************************************************/


/************************************************************************/
/*								nova									*/
/************************************************************************/

/********** 15.10.01 패치 미적용 ****************************************/	
//#define PARTY_BUFF_SHOW						// 2015.10.01(eiless84)	


/********** 15.11.26 ****************************************/
#define	MAP_REGION_SHOW							// 2015.11.26(eiless84) 맵에 지역명 표시,숨기기 추가.






#define SDM_USER_UI_SKIN_CHANGE_20160331		// UI 스킨 적용 용이하게 수정

#define SDM_CASHITEM_TRADE_LIMITED_20170214		// 캐쉬 아이템  거래 제한 기능

//#define SDM_MASTERS_MATCHING_ICON_HIDE_20170220// 마스터즈 매칭 아이콘 숨기는 define 서버에서 마스터즈 매칭이 끝나도 팀 정보를 알려주기 때문에 임시로 막음

#define SDM_TAMER_SERVERRELOCATE_20170911		// 테이머 서버 이동 기능
#define SDM_TAMER_SERVERRELOCATE_NOCHECK_TAMERNAME_20170914	// 테이머 서버 이동시 테이머 이름 체크 하지 않도록 수정

#define SDM_TAMERSLOT_EXPANSION_20170911		// 테이머 슬롯 확장 기능


//============================================================
//							leejiu
//============================================================
/********** 19.05.29 *****************************************/
#define LJW_TACTICS_SLOT_EXTEND_190529			// 용병 슬롯 확장

//=============================================================================
//	덤프 수정 공통 디파인(작업 후 4주 지나면 문제없는것은 삭제)
//=============================================================================
//#define CRASHDUMP_0305						//2014.03.05 정리한 덤프
//#define CRASHDUMP_0326						//2014.03.26 정리한 덤프
//#define CRASHDUMP_0424						//2014.04.24 정리한 덤프

// 16.06.07
#define LSJ_ALREADY_BUFF_CHECK				// 2016.06.07 중복된 버프효과 있는지 검사하는 구문 추가.	DM_Talk 테이블 30456 번호 추가.	//	BUFF_CHECK_MSG 거의 같은 기능 확인이 필요함...



#define SDM_SECONDPASSWORD_REINFORCE_20180330		// 2차 비밀번호 입력 창에 특수 문자 입력이 가능하도록 UI 변경

#define MACROKEY_NOTINPUT							// 단축키 입력이 ime 에 남아 채팅 입력 시 해당 키 값이 나오는 걸 막음. lks007 13.06.11


#define SDM_DIGIMON_SHOW_SKILLEXP_TABLE_20190212	// 디지몬 스킬 경험치표시를 테이블에서 참조 하도록 수정

#define LJW_TUTORIAL_RENEWAL_20191128				// 튜토리얼 개선

//========================================================================================
//
//  배포별 분리
//  
//========================================================================================
//#define USECONSOLE

#ifdef _GIVE		// 배포판
//#define USE_DUMPER						//2014.02.18(kimhoshok)	한국 전용 덤프 전송
#define DEFAULT_IME_HANGEUL				//2014.02.13(lks007)	맵 이동 시 입력 언어(IME)가 영어로 바뀌는 현상 관련

//#define	DEF_CORE_NPROTECT				// 2019.07.12	게임 가드 적용
//#define	DEF_CORE_NO_GAMEGUARD

//#define SDM_RESOURCE_CHECKER_ENABLE			// 리소스 변조 검사 기능 활성화

//#define SDM_DEF_XIGNCODE3_20181107			// 국내 Xigncode3 사용

#ifdef VERSION_QA
//	#define DEF_CORE_NPROTECT_TEST	// 2015.06.08	게임 가드 적용// 알파 빌드일 때만 활성
	#define	CHEAT_KEY
#endif

#elif NDEBUG		// 릴리즈

//#define USECONSOLE

//#define DEBUG_SYNC_CHAR					//	디버그용 싱크 케릭터
#define USE_DMO_INI						//	ini파일에서 초기 설정값 읽어옴

//#define ENABLE_KNOCKBACK				//	넉백가능		

//#define GM_CLOCKING						//2013.02.20(chu8820)	투명 아이템

//#define USE_DUMPER						// 2014.02.18	한국 전용 덤프 전송		kimhoshok
//#define MOVIECAPTURE					// 2014.03.19	라이브 버전 절대 사용 금지! F12 내부/Not라이선스/반디캠 녹화 kimhoshok
#define IGNORE_DISCONNECT_EXIT			// 서버로 부터 disconnect 패킷을 받아도 클라이언트가 종료되지 않도록 함
//	#define ZONEMAP_CLICK_MOVE				// 2014.05.21 존맵(KEY : m)에서 Ctrl + R버튼으로 이동하기	kimhoshok - 업데이트 미정


//#define	DEF_CORE_NPROTECT				// 2015.06.08	게임 가드 적용
//#define DEF_CORE_NPROTECT_TEST	// 2015.06.08	게임 가드 적용// 알파 빌드일 때만 활성

#define SKIP_LOGO						// 로고 스킵

#else				// 디버그	
// 	#define MOVIECAPTURE					// 2014.03.19	라이브 버전 절대 사용 금지! F12 내부/Not라이선스/반디캠 녹화 kimhoshok

#define DEBUG_SYNC_CHAR					//	디버그용 싱크 케릭터

#define USE_DMO_INI						//	ini파일에서 초기 설정값 읽어옴

#define CLIENT_MONSTER					//	몬스터 테이블 보고 배치 및 기타

#define ENABLE_KNOCKBACK				//	넉백가능

#define IGNORE_DISCONNECT_EXIT			// 서버로 부터 disconnect 패킷을 받아도 클라이언트가 종료되지 않도록 함
#define ZONEMAP_CLICK_MOVE				// 2014.05.21 존맵(KEY : m)에서 Ctrl + R버튼으로 이동하기	kimhoshok - 업데이트 미정
#define SKIP_LOGO						// 로고 스킵

#endif