//---------------------------------------------------------------------------
//
// 파일명 : GameEventCode.h
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
namespace EVENT_CODE
{
	enum GAME_EVENT_CODE
	{
		RESET_LOGIN_TIME_SEQ = 0,
		START_RESOURCECHECKER,		// 20180504 리소스 무결성 검사 시작 이벤트
		TIMEOUT_RESOURCECHECKER,		// 20180504 리소스 무결성 검사 시작 이벤트
		//NOTMATCH_MAPNAME_RESOURCECHECKER,
		RECV_LOGIN_WAIT_TIME,
		RECV_USE_BARCODE_FALSE,		//2017-07-11-nova

		RECV_LOGIN_REQUEST,

		RECV_SERVERSELECT_CLUSTER,	// 서버 접속 요청에 대한 결과 값

		RECV_PLAYER_DATA_LOAD_COMPLETE,

		RECV_SHUTDOWNSYSTEM_DATA,	// 셧다운 데이터 받음.
		RECV_WEEK_SHUTDOWN_DATA,	// 선택적 셧다운 데이터 받음.

		RECV_SECONDPASS_CHECK,		// 2차 비번 확인에 대한 결과 값
		RECV_SECONDPASS_REGISTER,	// 2차 비번 등록에 대한 결과 값
		RECV_SECONDPASS_CHANGE,		// 2차 비번 변경에 대한 결과 값

		RECV_CHECK_DOUBLE_NAME,		// 사용 사능한 이름인지 서버에서 패킷을 받음
		RECV_SERVER_LIST,

		RECV_CHAR_SELETECT_LIST,	// 캐릭터 선택창의 캐릭터 정보 받음.

		GET_SELETECT_LIST_EMPTY_SLOT_NUMBER,	// 캐릭터 선택창의 빈 슬롯 번호를 얻어 온다.

		RECV_ADD_NEW_CHARACTER,		// 새로 생성된 캐릭터 정보를 받았다
		RECV_CHAR_CREATE_RESULT,	// 캐릭터 생성 결과
		RECV_CHAR_DELETE_RESULT,	// 캐릭터 삭제 결과

		RECV_CHAR_SELECT_RESULT,	// 캐릭터 선택 성공

		OPEN_CHARDEL_PASSWORD_WINDOW,

		SEND_PORTAL_MOVE,			// 서버에 포탈 이동한다고 패킷을 보냄
		LOADING_START,				// 로딩 시작
		LOADING_PROGRESS_VALUE,		// 로딩 게이지
		LOADING_COMPLETE,			// 로딩 완료
		TABLE_LOAD_SUCCESS,			// 게임 테이블 로드 끝

		//////////////////////////////////////////////////////////////////////////

		OPEN_ITEM_PRODUCTION_SHOP,	// 아이템 제작 상점 오픈
		CLOSE_ITEM_PRODUCTION_SHOP,	// 아이템 제작 상점 닫기

		SEND_ITEM_PRODUCTION_RUN,		// 아이템 제작. 실행
		RECV_ITEM_PRODUCTION_RESULT,	// 아이템 제작. 결과

		SET_ITEM_PRODUCTION_ASSIST_ITEM_REG,

		//////////////////////////////////////////////////////////////////////////

		CHANGE_ITEM_COUNT,						// 아이템 제작. 아이템 갯수 변경
		CHANGE_HAVE_MONEY,						// 아이템 제작. 금액 변경

		DIGITAMA_SELL_COMPLETE,					// 디지타마 회수 성공 이벤트
		RECV_HATCHOUT_COMPLETE,					// 디지타마 부화 성공 이벤트
		RECV_ENCHANT_COMPLETE,					// 디지몬 강화 성공 이벤트
			
		EVENT_CHAT_PROCESS,						// 채팅 이벤트를 처리한다.
		EVENT_SYSTEM_PROCESS,					// 시스템 로그 이벤트를 처리한다.
		EVENT_BATTLE_PROCESS,					// 배틀 로그 이벤트를 처리한다.
		EVENT_ITEMCROP_PROCESS,					// 아이템획득 로그 이벤트를 처리한다.

		CASHSHOP_TOGGLE,						// 캐시샵 오픈 
		CASH_TOGGLE,							// 2017-03-28-nova	캐시창고

		CASHWAREHOUSE_TOGGLE,					// 캐시창고 오픈
		CASHWAREHOUSE_ITEM_SET,					// 캐시창고 아이템 세팅
		CASHWAREHOUSE_ITEM_SET_REFRESH_DISABLE,	// 캐시창고 새로고침 막음

		CASHITEMPAY_TOGGLE,						// 아이템지급 윈도우 오픈 
		CASHITEMPAY_REFRESH,					//
		CASHITEMPAY_POSTITEM_SET,				//
		CASHITEMPAY_REWARDITEM_SET,				//
 
		MAIL_SET_DATA,							// Letter Content


		//////////////////////////////////////////////////////////////////////////
		// Exp Event
		WAREHOUSE_TOGGLE,					// 창고 열기,닫기
		WAREHOUSE_SET_NORMAL,
		WAREHOUSE_SET_SHARE,
		WAREHOUSE_ITEMREMOVE_NOSERVER,

		WAREHOUSE_NORMAL_DATA_DECREASE,
		WAREHOUSE_SHARE_DATA_DECREASE,
		WAREHOUSE_NORMAL_SLOTCOUNT,			// 일반창고 개수
		WAREHOUSE_SHARE_SLOTCOUNT,			// 공유창고 개수
		WAREHOUSE_GETDATA,
		WAREHOUSE_RESET,				
		WAREHOUSE_ENDTIME,
		WAREHOUSE_GET_SLOTCOUNT,
		WAREHOUSE_GET_EVENTTIME,
		WAREHOUSE_TRADELIMITED_ITEM_TIMEOUT,

		WAREHOUSE_RECV_ITEMSORT_END,
		//////////////////////////////////////////////////////////////////////////

		ACHIEVE_CHECKTYPE,
		ACHIEVE_FIRSTPOSTLOAD,
		ACHIEVE_SETDISPTITLE,
		ACHIEVE_SETRANKACHIEVE,

		OPEN_CARD_EVENT_UI,					// 카드 게임 창 열기
		RECV_CARD_EVENT_RESULT,				// 카드게임 결과 받음.
		RECV_CARD_EVENT_END,				// 카드 게임 종료

		OPEN_TRADE_SELECT,					// 데이터 교환 선택창을 연다. 또는 교환창에서 선택창으로 돌아간다.
		START_TRADE_DATA,					// 데이터 교환 실행.

		RECV_TRADE_DATA_SCALE,				// 크기 데이터를 교환정보를 받는다.
		RECV_TRADE_DATA_UPGRADE,			// 강화 데이터를 교환정보를 받는다.
		RECV_TRADE_DATA_EVOLUTION,			// 진화 데이터를 교환정보를 받는다.	
		RECV_TRADE_DATA_FAILURE,			// 데이터 교환 실패

		// [5/13/2016 hyun] 친구 추천 이벤트 관련
		RECV_RECOMMEND_EVENT_INFO,			// 친구 추천 이벤트 관련 정보를 받는다.

		SEND_RECOMMEND_ADD_ANSWER_INFO,		// 타인이 나를 추천했을때 나의 응답을 서버에 보낸다
		RECV_FRIENDLY_ADD_ANSWER_INFO,		// 내가 보낸 추천의 결과를 받는 이벤트
		RECV_FRIENDLY_MARKITEM_INFO,		// 보상 아이템을 눌렀을때 받는 정보

		RECV_FRIENDLY_CHANGE_NAME,			// 테이머 이름 변경
		RECV_FRIENDLY_ADD_INFO,				// 누군가 나를 추천 했을때
		RECV_FRIENDLY_MARK_INFO,			// 친구가 목표달성했을 때 관련 정보
		RECV_FRIENDLY_CHANGESTATE_INFO,		// 친구 추천 이벤트 정보가 바꼈을때
		// @@ 여기까지

		// [8/24/2016 hyun] 상점
		OPEN_STORE_WINDOW,					// 상점 열기
		SEND_STORE_ITEMBUY,					// 아이템 구매
		SEND_STORE_ITEMSELL,				// 아이템 판매

		STORE_APPLY_DISCOUNT,				// 할인률 적용

		RECV_STORE_BUY_SUCCESS,				// 아이템 구매 성공
		RECV_STORE_SELL_SUCCESS,			// 아이템 판매 성공


		REQUEST_STORE_REPURCHASE_OPEN,		// 재구매창 오픈 요청
		RECV_STORE_REPURCHASE_SET,			// 재구매창 설정
		SEND_STORE_REPURCHASE,				// 아이템 재구매 요청
		RECV_STORE_REPURCHASE_SUCCESS,		// 아이템 재구매 응답
		RECV_STORE_REPURCHASE_FAILED,		// 아이템 재구매 실패

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// Exp Event
		RECV_EXP_EVENT_DATA,				// 경험치 이벤트 데이터 받음.
		DISABLE_EXP_EVENT_UI,				// 경험치 이벤트 UI 삭제
		HIDE_EXP_EVENT_UI,					// 경험치 이벤트 UI 숨김
		SHOW_EXP_EVENT_UI,					// 경험치 이벤트 UI 보여줌
		GET_EXP_EVENT_NEXT_EXPRATE,			// 다음날 접속 경험치를 가져온다
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 거래 관련 이벤트
		SEND_TRADE_REQUEST_TAMERNAME,			// 거래 요청(테이머 이름으로 요청)
		SEND_TRADE_REQUEST_TAMEROBJECTIDX,		// 거래 요청(테이머 오브젝트 IDX로 요청)
		SEND_TRADE_REQUEST_CANCEL,				// 거래 요청 취소.
		SEND_TRADE_REQUEST_RESULT,				// 거래 요청의 결과값 보냄
		SEND_TRADE_MONEY_CHANGE,				// 거래 금액 변경.
		SEND_TRADE_ITEM_REGIST,					// 인벤토리에서 아이템을 등록 한다.

		RECV_TRADE_REQUEST_RESULT,				// 거래 요청의 결과값.
		RECV_TRADE_RECEIVE_REQUEST,				// 거래 요청 받음.
		RECV_TRADE_ITEM_REGIST,					// 거래 아이템 등록
		RECV_TRADE_ITEM_UNREGIST,				// 거래 아이템 제거
		RECV_TRADE_MONEY_CHANGE,				// 거래 돈 변경
		RECV_TRADE_INVEN_LOCK_UNLOCK,			// 거래 준비 상태 변경
		RECV_TRADE_STANDBY_RESULT,				// 거래 완료 대지중
		RECV_TRADE_FINAL_RESULT,				// 최종 거래 결과

		//////////////////////////////////////////////////////////////////////////
		//채널 변경 UI 관련
		RECV_CHANNEL_LIST_INFO,					//채널 정보 받음.
		RECV_CURRENT_CHANNELINDEX,				//현재 채널
		SEND_CHANGE_CHANNEL,					//채널 변경 패킷 전송 및 채팅 메세지 세트
		SEND_CHANGE_CHANNEL_BYMASTER,			//SEND_CHANGE_CHANNEL 동일 GM에 의한 호출

		GET_CHANNELINFO,						//채널 정보 가져오기
		OPEN_CHANGE_CHANNEL_POPUP,				//채널 변경 확인 팝업 열기
		DISPLAY_CHANGE_CHANNEL,					//채널 변경 표시
		CHANGE_CHANNEL_FAIL,					//채널 변경 실패
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Daily Event
		RECV_DAILY_EVENT_DATA,					// 접속 이벤트 데이터
		RECV_MONTHLY_EVENT_DATA,				//2017-03-07-nova 월간 접속 이벤트
		//////////////////////////////////////////////////////////////////////////
		//채널 변경 UI 관련
		RECV_FRIENDLIST,						// 친구 목록 받음
		RECV_SHUTOUTLIST,						// 차단 목록 받음
		RECV_FRIEND_ON_OFF,						// 친구 login
		RECV_CHANGE_NAME_NEARBY,				// 이름 변경 주변
		RECV_CHANGE_NAME_FRIEND_SHUTOUT,		// 친구/차단 이름 변경
		RECV_ADD_FRIEND,						// 친구 추가 성공
		RECV_ADD_SHUTOUT,						// 차단 추가 성공
		RECV_NOT_FOUND,							// 테이머 찾지 못함( 친구/차단 추가 삭제 모두 )

		SEND_FRIENDLIST,						// 친구 목록 요청
		SEND_FRIEND_SHUTOUT_MEMO,				// 친구/차단 메모 쓰기
		SEND_ADD_FRIEND_SHUTOUT,				// 친구/차단 추가 패킷 보내기 및 전방체크

		GET_FRIENDLIST,							// 친구 목록 가져오기
		GET_SHUTOUTLIST,						// 차단 목록 가져오기
		GET_CURRENT_LISTINDEX,					// 현재 선택 중인 목록의 list index를 가져온다.

		CHECK_ACHIEVE_TYPE,						// 아카이프 타입 체크
		DELETE_FRIEND_SHUTOUT,					// 친구/차단 목록 삭제
		TOGGLE_FRIEND_SHUTOUT_UI,				// UI 토글 0 차단 탭이면 친구 탭으로 변경 토글, 1 단순 토글

		REWARD_ITEM_TOGGLE,						// 2017-03-08-nova  아이템 지급창고	
		REWARD_UI_TOGGLE,						// 2017-03-08-nova  접속보상 시스템
		REWARD_RECEIVE_GIFT,					// 2017-03-09-nova	선물아이템 받기
		REWARD_RECEIVE_REWARD,					// 2017-03-09-nova	보상아이템 받기
		REWARD_SEND_ACCPET_DATA,				// 2017-03-15-nova	선물,보상 아이템 수령여부 서버로 보내기
		
		DIGITAMA_SCAN_ADD_DATA,					//2017-03-28-nova
		DIGITAMA_SCAN_SUCCESS,					//2017-03-28-nova
		DIGITAMA_SELL_SUCCESS,					//2017-03-28-nova

		DIGITAMA_OPEN_NPC,
		DIGITAMA_OPEN_PORTABLE,
		DIGITAMA_CLOSE,
		DIGITAMA_SCAN_START,
		DIGITAMA_SELL_START,
		DIGITAMA_SETITEM,
		DIGITAMA_ALLSCAN,

		ENCYCLOPEDIA_OPEN_DATA,					//2017-05-11-nova
		ENCYCLOPEDIA_USE_DECK,					//2017-05-15-nova
		ENCYCLOPEDIA_USE_GETDECK,				//2017-05-29-nova
		ENCYCLOPEDIA_SET_EVOLINFO,				//2017-05-15-nova
		ENCYCLOPEDIA_RECV_SERVER,				//2017-05-15-nova
		ENCYCLOPEDIA_INCHANTRESULT,				//2017-05-15-nova
		ENCYCLOPEDIA_GROUPINFOCON,				//2017-05-15-nova
		ENCYCLOPEDIA_LV_CHECK,					//2017-05-16-nova
		ENCYCLOPEDIA_EVOL_UPDATE,				//2017-05-16-nova
		ENCYCLOPEDIA_BISREWARD,					//2017-05-16-nova
		ENCYCLOPEDIA_SCALE,						//2017-05-16-nova
		ENCYCLOPEDIA_LOGOUT,					//2017-05-16-nova
		GET_ENCYCLOPEDIA_USE_DECK,				//2017-05-15-nova
		GET_ENCYCLOPEDIA_BISRECV,				//2017-05-15-nova
		GET_ENCYCLOPEDIA_NULL_CHECK,			//2017-05-15-nova
		GET_ENCYCLOPEDIA_CONDITION,				//2017-05-16-nova
		GET_ENCYCLOPEDIA_NATTYPE,				//2017-05-16-nova
		GET_ENCYCLOPEDIA_PROB,					//2017-05-16-nova
		GET_ENCYCLOPEDIA_OPTION,				//2017-05-16-nova
		GET_ENCYCLOPEDIA_VAL,					//2017-05-16-nova
		GET_ENCYCLOPEDIA_TIME,					//2017-05-16-nova
		
		// 파티 관련 이벤트
		RECV_PARTY_INFO_INIT,						// 맵 이동시 서버로 부터 파티 정보 갱신 테이터 받음.
		RECV_PARTY_INFO,							// 파티 정보 받음.
		RECV_PARTY_INVITE_RESULT,					// 파티 요청 결과
		SEND_PARTY_INVITE_FROM_TAMERNAME,
		SEND_PARTY_INVITE_FROM_UNIQUE_ID,			// 파티 초대 요청 보냄
		RECV_PARTY_INVITE_ASK,						// 파티 초대 요청 받음.
		SEND_PARTY_INVITE_ANSWER_ACCEPT,			// 파티 초대 요청 받음았을 때 초대 응답.
		SEND_PARTY_INVITE_ANSWER_REJECT,

		RECV_PARTY_MAKE_ME,							// 내가 파티 생성
		RECV_PARTY_ADD_NEW_MEMBER,					// 새로운 파티원 정보
		RECV_PARTY_MEMBER_LEAVE,					// 파티 멤버 떠남.
		SEND_PARTY_LEAVE,							// 파티 탈퇴 보냄
		RECV_PARTY_LEAVE,							// 파티 탈퇴 보냄
		RECV_PARTY_MEMBER_BANISH,					// 파티 멤버 강퇴.
		SEND_PARTY_MEMBER_BANISH,					// 파티 멤버 강퇴 보냄
		RECV_PARTY_CHANGE_MASTER,					// 파티장 변경.
		SEND_PARTY_CHANGE_MASTER,					// 파티장 변경 보냄
		RECV_PARTY_DESTROY,							// 파티 해체
		RECV_PARTY_ITEM_LOOTING_INFO_CHANGE,		// 파티 아이템 루팅 정보 변경

		NTF_PARTY_MEMBER_MAP_CHANGE,				// 파티원 맵 이동
		NTF_PARTY_MEMBER_INFO_CHANGE,				// 파티원 정보 변경
		NTF_PARTY_MEMBER_DIGIMON_CHANGE,			// 파티원 디지몬 변경
		NTF_PARTY_MEMBER_ONOFFLINE,					// 파티원 온오프 정보
		NTF_PARTY_MEMBER_BUFF_CHANGE,				// 파티원 버프 정보 변경
		NTF_PARTY_MEMBER_POSITION_UPDATE,			// 파티원 위치 정보 갱신
		NTF_PARTY_EXP_UPDATE,						// 파티 일때 획득하는 추가 경험치
		NTF_PARTY_MEMBER_LOOT_ITEM,					// 파티원 아이템 획득
		OPEN_PARTY_LOOTING_WINDOW,					// 파티 아이템 분배 방식 셋팅하는 윈도우 열기
		OPEN_PARTY_ITEMRATE_WINDOW,					// 파티 아이템 분배시 아이템 등급 셋팅하는 윈도우 열기

		GET_PARTY_UIDX,								// 파티 IDX를 얻는다.
		GET_PARTY_MEMBER_NAME,						// 파티원의 이름을 얻어온다
		GET_PARTY_LOOTING_INFO,						// 현재 파티의 루팅 정보를 얻어온다
		IS_PARTYMEMBER_FROM_TAMERUIDX,				// 파티원 인지 체크
		IS_PARTYMEMBER_FROM_DIGIMONUIDX,			// 파티원 인지 체크

		//////////////////////////////////////////////////////////////////////////
		// 결투와 관련된 이벤트
		SEND_DUAL_REQUEST_OBJIDX,					// 결투 신청 요청 오브젝트 번호로
		SEND_DUAL_REQUEST_TAMERNAME,				// 결투 신청 요청 테이머 이름으로
		SEND_DUAL_REQUEST_ANSWER,					// 결투 신청 요청을 받은 것에대한 응답.
		RECV_DUAL_REQUEST_RESULT,					// 결투 신청 결과
		RECV_DUAL_REQUEST_ASK,						// 결투 요청 받음.
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// 부화기 이벤트
		MAKETACTICS_DIGITAMA_DATA,				//2017-04-25-nova
		MAKETACTICS_DIGITAMA_LV,				//2017-04-25-nova
		MAKETACTICS_DIGITAMA_SC_LV,				//2017-04-25-nova

		GET_MAKETACTICS_DIGITAMA_COUNT,			//2017-04-26-nova
		GET_MAKETACTICS_DIGITAMA_ISENABLE,		//2017-04-26-nova
		GET_MAKETACTICS_DIGITAMA_TYPE,			//2017-04-26-nova
#ifdef MINIGAME
		// 미니 게임
		RECV_MAKETACTICS_DIGITAMA_START_MINIGAME,
		RECV_MAKETACTICS_DIGITAMA_CLICK_RESULT,
		RECV_MAKETACTICS_DIGITAMA_TIME_OUT,
		RECV_MAKETACTICS_DIGITAMA_ERROR_MESSAGE,
		RECV_MAKETACTICS_DIGITAMA_END_MINIGAME,
		RECV_MAKETACTICS_MINIGAME_INIT,

		SEND_MINIGAME_END,
		SEND_MINIGAME_TIMEOUT,
		SENT_MINIGAME_CLICKBTN,
#endif
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		SET_TARGET_MARK,
		RELEASE_TARGET_MARK,
		//////////////////////////////////////////////////////////////////////////
		SEALMASTER_UI_OPEN_CLOSE,					// UI 열기/닫기

		RECV_SEALMASTER_SEALINFO,					// 씰정보 받기
		RECV_SEALMASTER_FAVORITE,					// 즐겨찾기 정보 받기
		RECV_SEALMASTER_LEADER,						// 리더 설정 된 씰
		RECV_SEALOPEN,								// 씰 오픈 받기
		RECV_RESEALING,								// 씰 재봉인 받기
		SEND_SEALMASTER_LEADER,						// 리더 설정 검증 밑 보내기
		CHECK_SEALMASTER_UNSEAL,					// 언씰 전 언씰 가능 여부 체크
		

		RE_SEALING_UISTART,							// 다시 씰로 만들기 UI 시작

		GET_SEALMASTER_SELECTSEALID,				// 현재 선택 중인 SEALID 가져오기
		GET_SEALMASTER_PLUS_STATE,					// 추가된 능력치 합 가져오기

		//////////////////////////////////////////////////////////////////////////
		SEND_GUILD_LEAVE,							// 길드 떠나기(자기자신) 보내기
		SEND_GUILD_INVITE,							// 길드 초대 보내기
		SEND_GUILD_CREATE,							// 길드 개설 보내기
		SEND_GUILD_DESTROY,							// 길드 폐쇄 보내기
		SEND_GUILD_BAN,								// 길드 탈퇴 시키기(길드장만 가능)
		SEND_GUILD_CHANGECLASSNAME,					// 길드 등급명 변경 보내기
		SEND_GUILDRANK_SET,							// 길드 등급 임명 보내기
		SEND_GUILDMEMO,								// 길드 메모 보내기	

		RECV_GUILDINFO,								// 길드 정보 받음(맵이동/접속)
		RECV_GUILD_MEMBERDELETE,					// 길드원 강퇴 받음
		RECV_GUILDMEMBER_MAXINC,					// 최대 길드원수 받음
		RECV_MAP_NO,								// 맵번호 받음
		RECV_GUILD_LEAVE,							// 길드 떠나기(자기자신) 받음
		RECV_GUILD_CREATE,							// 길드 개설 받음
		RECV_GUILDNOTICE_CHANGE,					// 길드 공지 변경
		RECV_GUILD_HISTORY,							// 길드 활동 기록 받음
		RECV_GUILD_LEVELUP,							// 길드 레벨 업 받음
		RECV_GUILD_MEMBER_LEVELUP,					// 길드원 레벨 업 받음
		RECV_GUILD_DESTORY_BC,						// 길드 삭제 주변인 모두 받음(BC 브로드캐스트)
		RECV_GUILD_ADD,								// 길드 추가
		RECV_GUILD_INVITEFAIL,						// 길드 초대 실패 받음
		RECV_GUILD_REJECT,							// 길드 초대 거절
		RECV_GUILDMEMBER_DELETE,					// 길드 멤버 삭제
		RECV_GUILD_TAMERNAME_CHANGE,				// 이름 변경 길드에 적용 받음
		RECV_GUILDRANK_SET,							// 길드 등급 임명 받음
		RECV_GUILD_ON,								// 길드 ON 받음
		RECV_GUILD_OFF,								// 길드 OFF 받음
		RECV_GUILD_CHANGE_CLASSNAME,				// 길드랭크명 변경 받음
		RECV_GUILDSKILL_SET,						// 길드 스킬 사용여부 받음
		RECV_GUILD_SKILLPOINT,						// 길드 스킬포인트 받음
		RECV_GUILD_CHANGEMEMO,						// 길드원 메모 받음
		RECV_GUILD_SUMMON,							// 길마(부길마) 소환 받음
		RECV_GUILD_SKILL_RESET,						// 2017-06-29-nova

		CHECK_INCREASE_GUILDMEMBERMAX,				// 길드원 최대수 증가 아이템 사용전 검사
		CHECK_GUILD_DESTROY,						// 길드 삭제전 재질문
		SET_GUILD_PERSONLEVEL,						// 길드원 레벨 변경

		GET_GUILDINFO,								// 길드 정보 가져오기
		GET_GUILD_MYINFO,							// 내 길드 정보 가져오기
		GET_GUILD_PERSON_INFO,						// 길드원 정보 가져오기
		GET_USINGSKILL_MAP,							// 사용중인 스킬이 저장된 MAP가져 오기

		GET_SET_PASIVESKILL_COLOR,					// 길드 패시브 스킬의 색을 설정하고 가져오기.
		GET_USING_SKILL,							// 사용중인 스킬 가져오기
		GET_GUILDRANK_NAME,							// 길드 랭크 네임 가져오기
		GET_GUILDRANK_DECRIPT,						// 길드 랭크 설명 가져오기

		GET_ISACTIVESKILL,							// 액티브 스킬 소유 여부 가져오기
		GET_USINGSKILLVALUE,						// 적용 중인 스킬로 인한 변화값 가져오기


		//////////////////////////////////////////////////////////////////////////
		NTF_USER_MESSAGE,
		//////////////////////////////////////////////////////////////////////////

		TOGGLE_DAMAGEMETER_WINDOW,					// 데미지 미터 창 토글 
		ADD_DAMAGEMETER_DATA,						// 데미지 미터에 데이터 셋팅

		DIGIMON_BATTLE_ON,
		DIGIMON_BATTLE_OFF,
		//////////////////////////////////////////////////////////////////////////
		// @@ 여기까지

		PERSON_ITEM_SELL_ADD,						//2017-05-02-nova
		PERSON_ITEM_BUY,							//2017-05-02-nova
//		PERSON_SET_STORENAME,						//2017-05-08-nova
//		PERSON_GET_STORENAME,						//2017-05-08-nova
//		PERSON_GET_VAULETYPE,						//2017-05-23-nova
		REQ_PERSON_OPEN,							// 개인/위탁 상점을 열겠다고 서버에 요청함.
		
		OPEN_PERSONSTORE,							//2019-01-04-leejiu
		OPEN_COMMISSIONSTORE,						//2019-01-04-leejiu
		CLOSE_PERSONSTORE_MSGBOX,
		REGIST_PERSONSTORE_ITEM,
		
		RECV_STORE_READY_RESULT,					//2019-01-23-leejiu
		RECV_OPEN_PERSONSTORE,						//2019-01-23-leejiu
		RECV_OPEN_COMMISSIONSTORE,					//2019-01-23-leejiu
		RECV_BUY_ITEM_RESULT,						//2019-01-23-leejiu
		RECV_PERSONSTORE_ITEMLIST,
		RECV_COMMISSIONSTORE_ITEMLIST,
		RECV_CLOSE_PERSONSTORE,
		RECV_CLOSE_COMMISSIONSTORE,

		UNION_ITEM_ADD_DATA,						//2017-05-02-nova
		UNION_MONEY_SET_DATA,						//2017-05-02-nova
		UNION_SET_SHOPDATA,							//2017-05-08-nova
		UNION_SET_MODELID,							//2017-05-08-nova
		UNION_RELEASE_DATA,							//2017-05-08-nova
		UNION_INITNET_OFF,							//2017-05-08-nova
		UNION_DELETE_DATA,							//2017-05-08-nova
		UNION_GET_CHAR_MAPID,						//2017-05-08-nova
		UNION_GET_CHAR_POS,							//2017-05-08-nova
		UNION_GET_CHAR_MODELID,						//2017-05-08-nova
		RECV_UNION_SHOP_WITHDRAW,					

		ACHIEVE_SET_DISPTITLE,						//2017-06-08-nova
		ACHIEVE_GET_DISPTITLE,						//2017-06-08-nova
		ACHIEVE_SET_RANKACHIEVE,					//2017-06-12-nova
		ACHIEVE_SET_CHECKTYPE,						//2017-06-12-nova

		//////////////////////////////////////////////////////////////////////////

		USING_MACROPROTECT,							// 매크로 프로텍트를 타겟의 이름으로 검사
		USING_MACROPROTECT_TARGET_UIDX,				// 매크로 프로텍트를 타겟의 UIDX로 검사 
		SEND_MACROPROTECT_QUESTION,					// 서버에 특정 유저에게 매크로 프로텍트 사용 요청 보냄
		NTF_MACROPROTECT_QUESTION_REQUEST,			// 내가 매크로 프로텍트 요청 옴.
		RECV_MACROPROTECT_QUESTION_RESULT,			// 내가 매크로 프로텍트 요청한 플레이어에 대한 결과값.
		RECV_MACRO_PLAYER_RESULT,					// 매크로 플레이어인지에 대한 결과값
		//////////////////////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////////////////////
		// 배틀매칭 시스템 관련 이벤트
		OPEN_BATTLEMATCH_REGIST_WINDOW,				// 배틀 신청 창 열기
		RECV_BATTLEMATCH_MY_INFO,					// 배틀 매칭 정보는 서버로 부터 받았다.
		RECV_BATTLEMATCH_REGISTER_RESULT,			// 배틀 매칭 신청 결과를 서버로 부터 받는다.
		RECV_BATTLEMATCH_REGISTER_CANCEL,			// 배틀 매칭 신청 취소 결과를 서버에서 받는다.
		NTF_BATTLE_MATCH_READY_REQUEST,				// 배틀 매치에 참가할 수 있는지 상태인지 서버에서 요청 옴.
		RECV_BATTLEMATCH_READY_STATE,				// 내가 배틀 준비 상태에 대한 결과 값
		NTF_BATTLEMATCH_READY_STATE,				// 다른 플레이어들의 배틀 준비 상태에 대한 결과 값
		NTF_BATTLEMATCH_CANCEL,						// 다른 플레이어들의 배틀 준비 상태에서 취소 및 기타 매칭이 취소 瑛?때 날라옴.
		NTF_BATTLEMATCH_COMPLETE,					// 매칭 성공 패킷
		NTF_BATTLE_START,							// 배틀 시작
		NTF_BATTLE_END,								// 배틀 종료
		NTF_BATTLE_REWARDS,							// 배틀 보상
		IS_BATTLEMATCH_REGISTED,					// 배틀 매칭 신청 중인가
		IS_MY_BATTLE_TEAM,							// 나와 같은 배틀 팀인가?
		IS_BATTLE_PLAYING,							// 배틀중인가?
		RECV_ADD_BATTLE_POINT,						// 배틀 티켓 사용으로 획득한 배틀 포인트
		//////////////////////////////////////////////////////////////////////////
		OPEN_SERVER_RELOCATE_PAGE_UI,					// 서버 변경 UI 열기
		RECV_SERVER_RELOCATE_CHECK_TAMERNAME_RESULT,	// 이전 하려는 서버에 생성 가능한 이름인지에 대한 결과값
		RECV_SERVER_RELOCATE_ITEMUSE_RESULT,			// 이전하기 위해 서버에 요청 패킷에대한 결과값
		IS_SERVERR_RELOCATE_PAGE_SHOW,					// 서버 이전 페이지를 보고 있는지 체크
		//////////////////////////////////////////////////////////////////////////

		RECV_QUEST_INFO_INIT,							// 서버로 부터 퀘스트 정보를 받았다.
		TUTORIALQUEST_ASK_CANCEL,						// 튜토리얼 퀘스트 포기관련
		TUTORIALQUEST_ACCEPT,							// 튜토리얼 퀘스트 수락
		TUTORIALQUEST_CANCEL,							// 튜토리얼 퀘스트 포기
		TUTORIALQUEST_COMPLETE,							// 튜토리얼 퀘스트 완료
		TUTORIALQUEST_PROCESS_ALL_COMPLETE,				// 진행 중인 튜토리얼 퀘스트 완료조건 모두 완료 瑛?때 호출됨.
		TUTORIALQUEST_PROCESS_UPDATE,					// 진행 중인 튜토리얼 퀘스트 완료이 업데이트 됨
		TUTORIALQUEST_DIGIVICE_CHECK,					// 디지바이스 퀘스트를 받았을 때 호출됨.
		TUTORIALQUEST_SKIP_ACCEPT,					// 튜토리얼 퀘스트 포기 수락
		GET_IS_TUTORIAL_PLAYING,						// 튜토리얼 중인지 가져옴.
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		OPEN_INFINITEWAR_START_WINDOW,						// 무한 대전 시작 윈도우 열기
		OPEN_INFINITEWAR_GIVEUP_WINDOW,						// 무한 대전 종료 윈도우 열기
		OPEN_INFINITEWAR_NOTICEBOARD_WINDOW,				// 무한 대전 게시판 윈도우 열기

		RECV_INFINITEWAR_CHALLENGESTATECLEAR,				// 무한 대전 스테이지 클리어
		RECV_INFINITEWAR_CHALLENGETRY_NEXTSTAGE,			// 무한 대전 다음 스테이지
		RECV_INFINITEWAR_CHALLENGE_GIVEUP,					// 무한 대전 포기
		RECV_INFINITEWAR_MAPMOVE_FALSE,						// 무한 대전 맵 이동 실패

		RECV_INFINITEWAR_NOTICEBOARD_DATA,					// 게시판 정보 받음.
		RECV_INFINITEWAR_NOTICEBOARD_POINT_PUT_RESULT,		// 포인트 투입에 대한 결과
		NTF_INFINITEWAR_NOTICEBOARD_DATA_UPDATE,			// 게시판 정보 갱신

		RECV_INFINITEWAR_NOTICEBOARD_RANKER_LIST,			// 주간,월간,시즌 별 현 랭커 리스트 목록 
		RECV_INFINITEWAR_NOTICEBOARD_PRE_RANKER_LIST,		// 주간,월간,시즌 별 지난 랭커 리스트 목록
		RECV_INFINITEWAR_NOTICEBOARD_FAME_LIST,				// 시즌별 명예의 전당 랭커 리스트 목록
		RECV_INFINITEWAR_SEASON_FAME_LIST,					// 시즌별 명예의 전당 리스트 목록
		//////////////////////////////////////////////////////////////////////////

		FADE_IN_END,
		FADE_OUT_END,

		//////////////////////////////////////////////////////////////////////////
		ON_CANCEL_INCUBATING,

		REQ_START_INCUBATING,
		REQ_STOP_INCUBATING,
		REQ_UNREGISTER_INCUBATED_DIGIMON,
		REQ_REGISTER_DIGIMON_TO_ACTIVATE_INCUSLOT,
		REQ_CHANGE_DIGIMON_IN_ACTIVATE_INCUSLOT,
		REQ_INCUBATOR_ITEM_IN_INVENTORY,
		REQ_SHOWVIEWER_BY_NPC,
		REQ_SHOWVIEWER_BT_ITEM,
		REQ_CHANGE_DIGIMON,
		REQ_HIDE_VIEWER,

		RECV_ARCHIVE_OPENCNT,
		RECV_ARCHIEVE_DIGIMONS,
		RECV_END_ARCHIEVE_DIGIMONS,
		RECV_INIT_ARCHIVE_INFO,
		RECV_ON_EXTENT_ARCHIVE,
		RECV_INCUBATOR_INFO,
		RECV_CHANGE_DIGIMONS_ARCHIVE,
		RECV_MOVE_DIGIMONS_IN_ARCHIVE,
		RECV_CHANGE_INCUBATOR,
		RECV_START_INCUBATING_INFO,
		RECV_STOP_INCUBATING_INFO,
		RECV_ON_EXTENT_ACCOMPANY_SLOT,
		RECV_ON_CHANGE_PARTNER_DIGIMON,
		//////////////////////////////////////////////////////////////////////////
		CHANGE_EQUIP,
		Recv_Ntf_XGuage_Update,
		Recv_XGToCrystal_Result,
		Recv_CrystalToXG_Result,
		Recv_Ntf_XGuage_Max_Update,

		CHANGE_QUICK_EXPAND,

		SET_BALLOON_TITLE,

		//////////////////////////////////////////////////////////////////////////
		NTF_NEON_DISPLAY,	//  전광판 패킷 받음.

		//////////////////////////////////////////////////////////////////////////
		REQ_SHOW_NEWCASHSHOP,
		REQ_HIDE_NEWCASHSHOP,

		RECV_CASHSHOP_STORAGE,
		//RECV_CASHSHOP_CART,
		SEND_CASHSHOP_ITEM_CROP,
		RECV_CASHSHOP_ITEM_CROP,
		SEND_CASHSHOP_ITEM_PRESENT,
		RECV_CASHSHOP_ITEM_PRESENT,
		CLOSE_CASH_ITEMBUY_WINDOW,

		REQUEST_STEAM_CASHITEM_PURCHASE,
		RECV_REQUEST_STEAM_CASHITEM_PURCHASE,	// 스팀 캐쉬아이템 구매시 중간 결과값.

		SEND_CASHSHOP_ITEM_PURCHASE,			// 캐쉬 아이템 구매 이벤트	
		RECV_RESULT_CASHITEM_PURCHASE,			// 아이템 구매 결과값
		RECV_RESULT_HAVE_CASH_REFRESH,			// 가지고 있는 캐쉬 정보 요청 결과
		RECV_CASHSHOP_BUY_HISTORY,			
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		OPEN_EXTRA_EVOLUTION_STORE,
		CLOSE_EXTRA_EVOLUTION_STORE,
		ASK_DIGIMON_TO_ITEM,
		ASK_ITEM_TO_DIGIMON,
		//SEND_DIGIMON_TO_ITEM,
		SEND_ITEM_TO_DIGIMON,
		RECV_ITEM_TO_DIGIMON,
		RECV_DIGIMON_TO_ITEM,
		//////////////////////////////////////////////////////////////////////////
		RECV_VIP_SYSTEM_DATA,
		ON_VIP_SYSTEM_CONTENTS,

		//////////////////////////////////////////////////////////////////////////
		RECV_NTF_HOTTIME_EVENT_INFO,
		RECV_HOTTIME_EVENT_ITEM_GET,

		SET_ACTIONKEY_NPC,
		SET_GIFTBOX_WINDOW,
		UPDATE_GIFTBOX,

		//////////////////////////////////////////////////////////////////////////
		OPEN_INVENTORY,
		CLOSE_INVENTORY,
		SET_ENABLE_INVENTORY,
		GET_POS_DEPENDON_INVEN,
		ADD_NEWITEM_INVENTORY,
		RECV_INVEN_SORT_END,

		RECV_INVEN_REQ_RESULT,
		RECV_WAREHOUSE_REQ_RESULT,

		//////////////////////////////////////////////////////////////////////////
		OPEN_CLOSE_TAMERSTATUS,
		UPDATE_TAMERSTATUS,

		OPEN_CROSSLOADER,
		CHANGE_CROSSINVEN_SIZE,
		CHANGE_CROSSDIGIMON,

		REQUEST_DELETE_DIGIMON,

		USE_GAMESCANNER,
		//////////////////////////////////////////////////////////////////////////
		OPEN_CLOSE_DIGIMONSTATUS,
		UPDATE_STAT_DIGIMONSTATUS,
		UPDATE_SKILL_DIGIMONSTATUS,
		CHANGE_DIGIMON_DIGIMONSTATUS,
		RECV_SKILL_LEVELUP_DIGIMONSTATUS,
		//////////////////////////////////////////////////////////////////////////
		UPDATE_TARGET_HPRATE,
		//////////////////////////////////////////////////////////////////////////
		
		RECV_DIGIMON_TRANSCENDENCE_CHARGE_EXP,
		RECV_DIGIMON_TRANSCENDENCE_HATCH_UP,
		//////////////////////////////////////////////////////////////////////////
		SEND_OPEN_GOTCHAMACHINE,
		RECV_OPEN_GOTCHAMACHINE,
		SEND_GET_ITEM_MACHINE,
		RECV_GET_ITEM_GOTCHAMACHINE,
		RECV_GET_ITEM_MYSTERYMACHINE,
		RECV_ERROR_GOTCHAMACHINE,

		GET_ITEM_GOTCHAMACHINE,
		CLOSE_MACHINE,
		OPEN_MYSTERYMACHINE,
		//////////////////////////////////////////////////////////////////////////
		SEND_ELEMENT_ITEM_CHECK,
		RECV_ELEMENT_ITEM_CHECK,
		RECV_ELEMENT_ITEM_ATTACH,
		RECV_ELEMENT_ITEM_DETACH,

		OPEN_ELEMENT_ITEM_NPC,
		OPEN_ELEMENT_ITEM_PORTABLE,
		CLOSE_ELEMENT_ITEM,

		REGIST_ELEMENT_ITEM_CHECK,
		REGIST_ELEMENT_ITEM_EQUIP,

		//////////////////////////////////////////////////////////////////////////
		OPEN_EQUIPSYSTEM_NPC,
		OPEN_EQUIPSYSTEM_PORTABLE,
		CLOSE_EQUIPSYSTEM,

		RECV_CHARGE_ITEM_LIST,
		RECV_CHARGE_ITEM_NORMAL_RESULT,
		RECV_CHARGE_ITEM_QUICK_RESULT,

		SEND_CHARGE_ITEM_CANCEL,
		RECV_CHARGE_ITEM_CANCEL,

		SEND_CHARGE_ITEM_WITHDRAW,
		RECV_CHARGE_ITEM_WITHDRAW,

		SEND_CHARGE_ITEM_UNLOCK,
		RECV_CHARGE_ITEM_UNLOCK,

		//////////////////////////////////////////////////////////////////////////
		RECV_ENCHANT_OPTION_IDENTIFY_RESULT,
		RECV_ENCHANT_OPTION_ENCHANT_RESULT,
		START_ENCHANT_OPTION_ENCHANT,
		//////////////////////////////////////////////////////////////////////////

		REQ_CHANGETAMER_USINGITEM,
		OPEN_CHANGETAMER_WINDOW,
		SEND_CHANGETAMER_CHANGE,

		//////////////////////////////////////////////////////////////////////////
		UPDATE_DAILYCHECKEVENT_TIMER,
		RECV_DAILYCHEKEVENT_INFO_RESULT,
		RECV_DAILYCHECKEVENT_RESULT_ITEM,
		//////////////////////////////////////////////////////////////////////////
		// Tutorial Event
		SKIP_TUTORIAL_EVENT,
		//////////////////////////////////////////////////////////////////////////
		RECV_QUEST_ALL_INFO,		// 전체 퀘스트 정보 갱신

		// Event Notice
		REQ_OPEN_EVENT_NOTICE,
		REQ_CLOSE_EVENT_NOTICE,
		REQ_TOGGLE_EVENT_NOTICE,

		// Season Pass
		REQ_OPEN_SEASON_PASS,
		REQ_CLOSE_SEASON_PASS,
		REQ_TOGGLE_SEASON_PASS,

		MAX_EVENT
	};
}
