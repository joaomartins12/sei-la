#pragma once

namespace	NS_CHAT
{
	enum TYPE{
		//	: ChatContents == ChatMainWindow	
		NORMAL,			// 일반말. 유저들간의 일반 대화. 말풍선 효과 추가.
		PARTY,			// 파티말.
		GUILD,			// 길드말.
		WHISPER,		// 귓속말.
		SHOUT,			// 외치기.
#ifdef MEGAPHONE_BAN
		MEGAPHONE,		// 메가폰.(확성기)
#endif
		MAXCOUNT,		//
		CASH_NOR=nItem::Cash_Shouter_N,		// 확성기 일반
		CASH_SPE=nItem::Cash_Shouter_S,		// 확성기 고급
		CASH_NOR_TIME=nItem::Cash_Shouter_NT,	// 확성기 일반 기간제
		CASH_SPE_TIME=nItem::Cash_Shouter_ST,	// 확성기 고급 기간제
		GM_MEGAPHONE=nItem::GM_Shouter_T,		// GM 전용 확성기
		NOTICE,			// 공지 : 별개 처리
#ifdef GUILD_RENEWAL
		GUILD_SHOUT,	// 길드 확성기 스킬
#endif

		STORE_SERCH = 3333,	// 3333 위탁 상점 검색 문자열 색을 표시하기 위한 값
		
		NORMAL_TEXT,		// 일반말. 알림등의 일반 글. 말풍선 효과 없음.
		DIGIMON_TIP,		// 일반말. 디지몬이 게임팁에 대해서 설명.

		GUILD_RANK,			// 길드 랭킹.
		GUILD_NOTICE,		// 길드 공지.
		
		WHISPER_REPLY,		// 귓속말 응답하기
		DEBUG_TEXT,			// 디버깅용 문구

		SET_BANTIME,		// 채팅금지 시간
		RESET_SHOUTTIME,

		ADDSTRING_UPDATE,	// 채팅창에 실제 채팅 데이터 입력. 갱신
		FOCUS_UPDATE,		// 채팅 입력창의 포커싱을 확인.업데이트

		ADD_EMOTION,		// 채팅 동작 명령어 입력
		SET_SHOWINPUT,		// 채팅 입력창 키고, 끄기
		
		SET_ENABLE,				// 채팅 타입 보기, 안보기
		SET_ENABLE_RESET_SCROLL,// SET_ENABLE시 채팅스크롤 정보 리셋.

		CHAT_CLEAR,			// 채팅창 내용 전부 삭제(청소)
		SET_FROMWHISPER,	// 귓속말 입력

		CHAT_NEWLIST,		// 채팅창 최신글 보기.

		//	SYSTEM			: ChatSystemWindow
		SYSTEM_TEXT,
		SYSTEM_TEXT_ITEMCROP,	// [4/14/2016 hyun] 아이템 획득 

		//	BATTLE		: ChatBattleLogWindow
		BATTLE_TEXT,
	};
};