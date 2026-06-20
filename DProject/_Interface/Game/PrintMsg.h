
#pragma once 

class cPrintMsg
{
public:
	enum
	{
		NONE							= 0,
		USE_MANUAL_MESSAGE				= 1,
		USE_MANUAL_MESSAGE_URL			= 2,
		USE_MANUAL_MESSAGE_DISCONNECT	= 3,
	};
	enum eTYPE
	{
		LOGIN_ID_FAILE					= 10001,		// 아이디 공백
		LOGIN_ID_NONE_EXIST				= 10002,		// 존재하지 않는 아이디, 비번오류		

		LOGIN_HTTP_SERVER_ERROR			= 10007,		// 인증Http 서버 오류	

		LOGIN_CHECKING_SERVER			= 10011,		// 서버 점검

		LOADING_SERVER_INFO				= 10019,		// 서버정보를 불러오고 있습니다
		
		CHARCREATE_NAME_LEN_ERROR		= 10021,		// 이름 글자수 오류
		CHARCREATE_TAMER_NAME_EXIST		= 10022,		// 사용중인 테이머 이름
		CHARCREATE_DIGIMON_NAME_EXIST	= 10025,		// 사용중인 디지몬 이름
		CHARSELECT_MAX_CHAR_COUNT		= 10023,		// 캐릭터 5개 이상 생성 오류
		CHARCREATE_NAME_ERROR			= 10024,		// 이름에 사용할수 없는 문자 포함
		CHARCREATE_NAME_ERROR_TYPE2		= 10029,		// 이름에 사용할 수 없는 단어가 포함되어 있습니다
		CHARCREATE_SELECT_CHAR_ERROR	= 10031,		// 캐릭터를 선택하지 않았을시 오류

		CHARSELECT_DELETE_CHAR			= 10041,		// 캐릭터 삭제 유무		
		CHARSELECT_DELETE_COMP			= 10042,		// 캐릭터 삭제

		DIGITAMA_NO						= 11001,		// 디지타마가 아님
		DIGITAMA_EXIST					= 11002,		// 회수 안된 디지타마가 존재
		SHOP_MONTY_SHORT				= 11003,		// 소지금 부족	

		GAME_ITEM_DELETE				= 11011,		// 아이템 삭제
		GAME_INVEN_LIMIT_COUNT			= 11015,		// 가방 부족
		GAME_INVEN_LIMIT_SLOTCOUNT		= 11016,		// 인벤 비활성 슬롯
		GAME_ITEM_LIMIT_SALE			= 11017,		// 판매불가 아이템

		GAME_ITEM_BUY					= 11020,		// 아이템 구입시
		GAME_ITEM_DROP					= 11021,		// 아이템 드랍
		GAME_ITEM_SELL					= 11022,		// 아이템 판매

		GAME_ITEM_CROP					= 11023,		// 아이템 획득

		GAME_ITEM_STORE_SELL			= 11028,		// 아이템 상점 판매

		GAME_ITEM_SHORTAGE				= 11040,		// 아이템이 부족합니다
		GAME_ITEM_BUY_BYITEM			= 11045,		// 아이템으로 아이템 구입
		GAME_ITEM_SHORTAGE_DIGICORE		= 11046,		// 디지코어 아이템 부족
		GAME_ITEM_MANYCROP				= 11047,		// 아이템 1개이상 획득
		
		GAME_ITEM_MAKING				= 11073,		// 아이템 제작

		GAME_DEAD						= 12001,		// 파트너몬 죽음

		TAMER_DS_SHORT					= 13001,		// 테이머 DS부족

		// 퀘스트 관련
		QUEST_REQUITE					= 14001,		// 퀘스트 항목 보상받을수 있는 상태가 됨
		QUEST_LIMIT_MAX_COUNT			= 14002,		// 퀘스트 갯수 제한에 걸림
		SCREENSHOT						= 15001,		// 스크린샷
		NO_SELECT_TARGET				= 16001,		// 대상 선택 오류
		ONLY_DIGIMON					= 17001,		// 디지몬만 선택 가능
		ONLY_TAMER						= 17002,		// 테이머만 선택 가능
		TARGET_EYE_OUT					= 17003,		// 타겟이 시야에 없음		
		SERVER_DISCONNET				= 10003,		// 서버와의 접속 종료
		
		QUEST_VALUE						= 14002,		// 퀵슬롯 받을 수량
		EVO_NOT							= 13005,		// 진화 불가		
		TARGET_ERROR					= 11101,		// 대상이 잘못됨

		TAMER_SKILLUP					= 19005,		// 테이머스킬 업
		DIGIMON_SKILLUP					= 19006,		// 디지몬스킬 업
		CHIPSET_FULL					= 11019,		// 칩만땅	
		
		SECOND_PASS_DISACCORD			= 20052,		// 2차 비번 불일치
		SECOND_PASS_SET_COMPLETE		= 20053,		// 2차 비번 설정 완료
		SECOND_PASS_CONFIRM				= 20055,		// 2차 비번 확인 

		CASHSHOP_STEAM_OVERLAY_SYSTEM_DIABLE= 30320,		// 스팀용일 경우 캐쉬 아이템 구매시 스팀 오버레이 기능이 활성화 되어 있지 않다는 메시지

		QUEST_DROP_BTN					= 30001,		// 퀘스트 드랍 버튼 클릭
		CROP_MONEY						= 30170,		// 돈 획득

		USE_ITEM_CHANGE_ADVENTURE		= 30538,
		USE_ITEM_CHANGE_SAVERS			= 30539,

		// 장비 시스템 - 시간 충전
		EQUIPSYS_WITHDRAW_ITEM			= 11210,		// 충전 완료된 아이템 회수
		EQUIPSYS_CANCEL_CHARGE			= 11211,		// 충전 진행 중인 아이템 충전 취소
		EQUIPSYS_UNLOCK_SLOT			= 11212,		// 충전 슬롯 잠금 해제

		SYSTEM_MESSAGE					= 12000,		// 시스템 알림 메세지
		SERVER_VERSION_ERROR			= 19999,		// 서버와버젼 오류
		SERVER_READYING					= 19998,		// 서버 준비중
		SERVER_READYING_AERIA			= 19997,		// 서버 준비중 (Aeria용) chu8820
		SAME_MACRO_KEY					= 20000,		// 단축키 중복

		// 튜토리얼 이벤트 스킵
		TUTORIAL_EVENT_SKIP				= 10130,		// 튜토리얼 이벤트 스킵

		RECOMMENT_ANSWER_CONFIRM		= 30492,		// 누구님을 추천하였습니다. 수락하시겠습니까?
		LOG_OUT							= 99998,		// 로그아웃 - 임시		
		GAME_EXIT						= 99999,		// 게임종료 - 임시
	};

	static void		PrintMsg( int nFTID, void* pValue1 = NULL, void* pValue2 = NULL, void* pValue3 = NULL, void* pValue4 = NULL, void* pValue5 = NULL );

	static TCHAR*	_Parcing( TCHAR* szMsg, int nLen, void* pValue1, void* pValue2, void* pValue3, void* pValue4, void* pValue5 );
	static bool		_ParcingToString( TCHAR* szOut, TCHAR* szIn, void* pValue1, void* pValue2, void* pValue3, void* pValue4, void* pValue5, int& nParcingIndex );
};
