#pragma once 

/*#ifdef VERSION_QA
#define		ACCOUNT_IP			"18.163.96.237"
#else
#define		ACCOUNT_IP			"18.163.30.48"
#endif
*/

#define		ACCOUNT_PORT		7029


//#define VERSION_TW							// pLength 길이 관련 디파인		lks007	13.10.31

#define	NAME_MIN_LEN				2//3
#define NAME_MAX_LEN				8//12
#define NAME_GUILD_MAX_LEN			14
#define NAME_FRIEND_COMMENT_LEN		23		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define NAME_BLOCK_COMMENT_LEN		23		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define NAME_GUILDRANK_COMMENT_LEN	21		//cCreateName::Init 에서 m_nMaxLengthByte 도 같이 수정 되야 한다
#define VERSION_FILE				"LauncherLib\\vTDMO.ini"

#define CHAT_LIMITE_SHOUTING_LV		1	// 외치기 제한 레벨

#include "_Lan_Taiwan.h"
//#define PLAY_PANELTY						//	봇 2차 - 시간에 의한 패널티	
#define CHEAT_MASTER						//	마스터 치트 사용 여부
#define VERSION_ERROR_ROOLBACK				//	버젼 에러시 롤백
#define ADD_ACHIEVE_COSTUME					//	추가 코스튬 업적


#define GM_COSTUME							//	GM 코스츔 - 이펙트 효과	
//#define	BATTLE_MATCH						//  배틀 정보 받기



 #define THAI_INPUT_PASTE					// 태국어 문자 붙여넣기 시 기존 input이 ANSI형식에서 인식이 되지 않아 unicode 형식 추가함	lks007	12.08.20
 #define THAI_WORDTRANSFORM					// 태국어로 생성된 Tamer 및 Digimon, 기타 String 문자 변환 lks007	12.10.15 
// #define THAI_PASSWORDLENGTH					// 태국 비밀번호 15자리까지 입력 가능하도록 변경(캐릭터 및 디지몬 삭제 시 입력 자리 수 변경)


#define NOT_ENTER_CREATENAME				// 대만(중국)에서 글자 조합 및 선택 시 Enter 키로 함. 현재 Enter로 생성 버튼 누르게 되어 있어 해당 동작 막음. lks007 13.06.10
#define MACROKEY_NOTINPUT					// 단축키 입력이 ime 에 남아 채팅 입력 시 해당 키 값이 나오는 걸 막음. lks007 13.06.11
#define DEFAULT_IME							// IME 기본 상태로 동작하도록 함. lks007	13.06.12.

#define TAIWAN_DAYI_IME						// DaYi 입력볍
#define TAIWAN_NAME_LENGTH					// 중문 이름 생성 길이 제한
#define TAIWAN_WORDTRANSFORM				// 중문 번체로 생성된 Tamer 및 Digimon, 기타 String 문자 변환(멀티→유니, 유니→멀티)   lks007	13.08.08

#define QUEST_STRING_PARCING				// 폰트 변경으로 퀘스트 필요 숫자 표시부분 숫자 잘리는것 관련 수정	lks007	13.08.20
#define IME_WINDOW_POSITION					// Candidate Window Position 설정	lks007	 13.08.20

#define PASSWORD_ENCODE						// pw md5 암호화
#define INVENCNT_CHECK						// open된 인벤 개수보다 더 큰 인벤 번호 사용 (해당 인벤에 아이템 있을 시) 안되도록	lks007	13.11.05


#define LOGIN_SCREEN						// 캐릭터 select화면에서 cancel 버튼 눌렀을때 로그인 화면으로

#define SELECT_SERVER_ERROR					// 맵 이동 시 클라에서 기존 맵 disconnect 후 connect 시도 시 서버에서는 아직 기존맵 disconnect되지 않아 끊기는 현상 lks007	13.12.21



//=============================================================================
// 14. 2.19 패치
//=============================================================================
//#define CHAT_BAN							// 운영툴::채팅 금지								2013.12.04						chu8820







/************************************************************************/
/*							    eiless84                                */
/************************************************************************/

/********** 15.10.01 패치 미적용 ****************************************/	
//#define PARTY_BUFF_SHOW						// 2015.10.01(eiless84)	

/********** 15.10.07 패치 ************************************(eiless84)*/
#define NEW_WAREHOUSESORT					//2012.08.03(vf00)		창고 정리 리뉴얼
#ifdef NEW_WAREHOUSESORT
#define	WAREHOUSESORT						//2012.08.22(vf00)		창고 정리 소스 보호
#endif




// [4/5/2016 hyun] 추가
#define MAKE_TACTICS_UI_UPGRADE				//(kimhoshok)	디지타마 부화창 중급 데이터 사용 UI 수정
#ifdef MAKE_TACTICS_UI_UPGRADE
#define TACTICS_USE_DATA_LV2				//(kimhoshok)			중급 데이터 사용
//#define TACTICS_USE_DATA_LV3
//#define TACTICS_USE_DATA_LV4
//#define TACTICS_USE_DATA_LV5
#endif



// [6/2/2016 hyun] 추가
#define	KEYBOARD_MOVE						//2014.09.17(chu8820)	키보드 이동(인던 추가 관련)



// [9/20/2016 eiless84] 추가				//	[9/28/2016 eiless84] 업데이트 예정
#define GUILD_RENEWAL						//2014.06.25(chu8820)	길드 시스템 리뉴얼(길드스킬)

// [11/9/2016 hyun] 추가
#define MEGAPHONE_BAN						// 확성기 차단

#define SDM_CASHITEM_TRADE_LIMITED_20170214	// 캐쉬 아이템  거래 제한 기능




#define NEW_SHARESTASH						//2012.08.06(vf00)		공용 창고, 정리기능 신규추가



// 2018/12/05
#define	USE_CENTER_BAR						// 센터바
#define DAMAGE_METER						//2014.11.05(chu8820)	데미지 미터기 (전투통계)
#define MASTERS_MATCHING					//2013.11.20(chu8820)	마스터즈매칭 해당 ifdef들어간 곳 pop( nMyTeam ); 주석 풀어줄 것


//#define SDM_CASHSHOP_CASH_INTEGRATION_20181121	// 홍콩/대만/태국의 경우 보너스 캐쉬가 없기 때문에 통합된 UI로 표시되게 함.


#define SDM_SECONDPASSWORD_REINFORCE_20180330		// 2차 비밀번호 입력 창에 특수 문자 입력이 가능하도록 UI 변경

#define	MAP_REGION_SHOW							// 2015.11.26(eiless84) 맵에 지역명 표시,숨기기 추가.
#define BUFFINFO_FIX						//2014.04.30(chu8820)	버프 정보(버프아이콘) UI 수정
#define LJW_TACTICS_SLOT_EXTEND_190529			// 용병 슬롯 확장

//========================================================================================
//
//  배포별 분리
//
//========================================================================================

#ifdef _GIVE
//#define SDM_DEF_XIGNCODE3_20181107			// 대만 Xigncode3 사용 2019-10-07
//#define SDM_RESOURCE_CHECKER_ENABLE		// 리소스 변조 검사 기능 활성화

#elif NDEBUG
#define USE_DMO_INI						//	ini파일에서 초기 설정값 읽어옴
#define SKIP_LOGO						// 로고 스킵
#else

#define USE_DMO_INI						//	ini파일에서 초기 설정값 읽어옴
#define SKIP_LOGO						// 로고 스킵

// #pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
// 
// #define TEMPLOG	printf

#endif