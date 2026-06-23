#pragma once 

#define ACCOUNT_IP					"127.0.0.1"
#define	ACCOUNT_PORT				7029

#define	NAME_MIN_LEN				3
#define NAME_MAX_LEN				16
#define NAME_GUILD_MAX_LEN			14
#define NAME_FRIEND_COMMENT_LEN		23		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define NAME_BLOCK_COMMENT_LEN		23		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define NAME_GUILDRANK_COMMENT_LEN	21		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define VERSION_FILE				"LauncherLib\\vGDMO.ini"
//	#define NET_VERSION_ACCOUNT			52304112//VERSION.ini 파일로 체크하던 버전 코드상에서 확인하도록 수정 chu8820
//	#define NET_VERSION_GAME			20140911//VERSION.ini 파일로 체크하던 버전 코드상에서 확인하도록 수정 chu8820

#define CHAT_LIMITE_SHOUTING_LV		20	// 외치기 제한 레벨

#include "_Lan_English.h"

#define TARGET_WIN_MULTYLINE				// 타겟창 2라인	
//#define PLAY_PANELTY						//	봇 2차 - 시간에 의한 패널티	
#define CHEAT_MASTER						//	마스터 치트 사용 여부
#define VERSION_ERROR_ROOLBACK				//	버젼 에러시 롤백
#define ADD_ACHIEVE_COSTUME					//	추가 코스튬 업적

#define GM_COSTUME							//	GM 코스츔 - 이펙트 효과	
#define INVENSORT							//  인벤 정리		
#define GUILD_INVITATION					//	길드 초대 거부

#define ITEM_LIMIT_STRING_BUG				//2012.07.27(chu8820)	툴팁에서 제한 레벨 텍스트 "Tamer Limitation Lv" 에 레벨수치 가려지는 현상 수정


/********** 13.08.27 패치 (폭죽) ****************************************/
//#define FIREWORK							//2013.05.01(khc0729)	폭죽 리뉴얼

/********** 13.10.10 패치 (인던 추가) ***********************************/
#define TRICK_ORTREAT						//2012.10.15(vf00)		할로윈 이밴트 추가


/********** 13.12.24 패치 (국내 버그 수정 반영) *************************/
#define CHEAT_ITEMRANK						//2013.11.21(chu8820)	아이템랭크 치트키 추가
//닷본/요코하마 맵 리뉴얼 관련 추가
//#define TUTORIAL_DISABLE					//2013.11.05(vf00)		튜토리얼 비활성

/********** 14.01.28 패치 (악세서리, 간략화 추가) ***********************/
#define SIMPLEFICATION						//2013.02.27(khc0729)	간략화


/********** 14.03.25 패치 ***********************************************/
//씰마스터, 재구매, 퀵슬롯, 기적의 사파이어, 도감 추가
//#define CARD_MASTER							//2013.12.30(vf00)		카드마스터
#define REPURCHASE							//2014.01.08(chu8820)	상점 판매 아이템 재구매 기능 추가
#define	USE_CENTER_BAR						// 센터바
#define LEVELUP_EFFECT_LOOP					//2014.01.23(lks007)	경험치 폭탄(?) 아이템 사용 시 레벨업 이펙트를 상승된 레벨만큼 보여주기
//2014동계올림픽 복장(남녀구분) 추가
#define COSTUME_GENDER						//2014.02.11(kimhoshok)	남성 전용, 여성 전용 코스튬 2604, 2605
//버그 수정 - 사막 맵 레이드몬스터 AI 추가
#define MONSTER_SKILL_GROWTH				//2013.12.18(chu8820)	몬스터 스킬::성장 추가
/* 요청사항 - GM 투명 아이템 */
//#define GM_CLOCKING						//2013.02.20(chu8820)	투명 아이템


//월드컵 아바타 추가 관련
#define COSTUME_TAMER_ONLY					//2014.04.29(kimhoshok)	테이머 전용 코스튬 추가
//버그수정 - 버프아이콘 UI 수정
#define BUFFINFO_FIX						//2014.04.30(chu8820)	버프 정보(버프아이콘) UI 수정
//업적 아이콘_02 추가

/********** 14.06.24 패치 ***********************************************/
//데미지 자료형 수정
#define DEBUFF_ITEM							//2013.08.07(lks007)	디버프 아이템
//국내 수정사항 적용
#define TAMER_IDLEANIMATION					//2014.03.25(lks007)	테이머가 자리 비움 상태일때 서버와 데이터를 주고 받으면 일어서야 한다

/********** 14.07.22 패치 ***********************************************/
//중급데이터
#define MAKE_TACTICS_UI_UPGRADE				//2014.05.22(kimhoshok)	디지타마 부화창 중급 데이터 사용 UI 수정
#define TACTICS_USE_DATA_LV2				//(kimhoshok)			중급 데이터
//#define TACTICS_USE_DATA_LV3
//#define TACTICS_USE_DATA_LV4
//#define TACTICS_USE_DATA_LV5


/********** 14.11.25 패치 ***********************************************/
//키보드 이동, 핵쉴드 업데이트, 텍스트 겹침 현상 수정( 텍스트 폭 늘림 )
#define	KEYBOARD_MOVE						//2014.09.17(chu8820)	키보드 이동(인던 추가 관련)




/************************************************************************/
/*							    eiless84                                */
/************************************************************************/

/********** 15.10.01 패치 미적용 ****************************************/	
//#define PARTY_BUFF_SHOW						// 2015.10.01(eiless84)	

/********** 15.12.02 ****************************************/

#define MASTERS_MATCHING					//2013.11.20(chu8820)	마스터즈매칭 해당 ifdef들어간 곳 pop( nMyTeam ); 주석 풀어줄 것

// 2015.12.22
#define	MAP_REGION_SHOW							// 2015.11.26(eiless84) 맵에 지역명 표시,숨기기 추가.

#define NEW_SHARESTASH						//2012.08.06(vf00)		공용 창고, 정리기능 신규추가				
#define NEW_WAREHOUSESORT					//2012.08.03(vf00)		창고 정리 리뉴얼
#ifdef NEW_WAREHOUSESORT
#define	WAREHOUSESORT					//2012.08.22(vf00)		창고 정리 소스 보호
#endif

#define CONSIGNMENT_BALLOON_CASH			//2013.12.11(lks007)	위탁 상점 말풍선 캐시 아이템 타입 추가
#define CONSIGNMENT_SALE					//2012.09.25(lks007)	위탁 상점 찾기 기능
//#define EXP_UNLIMITED						//2013.01.15(lks007)	퀘스트 수행 레벨과 디지몬 레벨이 15렙 이상 차이나면 경험치 못받는것 98렙 차이로 수정

#define MEGAPHONE_BAN						//2014.05.28(lks007)	확성기 차단

#define EMPLOYMENT_TAMERNAME				// [2/25/2016 hyun]	위탁상점 오픈 시 테이머 이름 출력

// 16.08.16
#define LSJ_ALREADY_BUFF_CHECK				// 2016.06.07 중복된 버프효과 있는지 검사하는 구문 추가.	DM_Talk 테이블 30456 번호 추가.	//	BUFF_CHECK_MSG 거의 같은 기능 확인이 필요함...

// [8/23/2016 hyun] 현욱 추가.
#define MINIGAME							//2013.02.20(chu8820)	미니게임


#define SDM_CASHITEM_TRADE_LIMITED_20170214	// 캐쉬 아이템  거래 제한 기능

//#define SDM_MASTERS_MATCHING_ICON_HIDE_20170220			// 마스터즈 매칭 아이콘 숨기는 define 서버에서 마스터즈 매칭이 끝나도 팀 정보를 알려주기 때문에 임시로 막음

#define DAMAGE_METER						//2014.11.05(chu8820)	데미지 미터기 (전투통계)

#define SDM_SECONDPASSWORD_REINFORCE_20180330		// 2차 비밀번호 입력 창에 특수 문자 입력이 가능하도록 UI 변경

#define SDM_SERVER_STATE_SHOW_20181011				// 서버 포화 상태 표시 하도록 UI 개선

#define SDM_DIGIMON_SHOW_SKILLEXP_TABLE_20190212	// 디지몬 스킬 경험치표시를 테이블에서 참조 하도록 수정

#define CHAT_BAN							//	운영툴::채팅 금지


#define LJW_TACTICS_SLOT_EXTEND_190529	// 19.07.03 용병 슬롯 확장
#define LJW_TUTORIAL_RENEWAL_20191128	// 튜토리얼 개선

//========================================================================================
//
//  배포별 분리
//
//========================================================================================


#ifdef _GIVE
	//#define SDM_DEF_XIGNCODE3_20181107		// Turn on XignCode3
	//#define SDM_RESOURCE_CHECKER_ENABLE		// Hash Check ?

	#ifdef VERSION_QA
		#define CHEAT_KEY
	#endif

#elif NDEBUG
	#define USE_DMO_INI						//	ini파일에서 초기 설정값 읽어옴
	#define IGNORE_DISCONNECT_EXIT			// 서버로 부터 disconnect 패킷을 받아도 클라이언트가 종료되지 않도록 함
	#define SKIP_LOGO						// 로고 스킵
	#define CHEAT_KEY
#else //_DEBUG
	#define USE_DMO_INI						//	ini파일에서 초기 설정값 읽어옴
	#define IGNORE_DISCONNECT_EXIT			// 서버로 부터 disconnect 패킷을 받아도 클라이언트가 종료되지 않도록 함
	#define SKIP_LOGO						// 로고 스킵
#endif