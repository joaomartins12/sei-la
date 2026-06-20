#pragma once


#include "nScope.h"

namespace pBattle
{
	enum
	{
		Begin = nScope::Battle,
		//서버<->서버
		ReqCoreServerChange,				//코어간 이동 요청 , Battle->Core  , Core->Battle
		ResCoreServerChange,				//코어간 이동 요청 응답
		UserDisconnected,					//BattleCore -> Core , 데이터 저장유지 해제

		//
		EnterBattle,						

		//카드 덱
		Deck_List,							//카드 리스트
		Deck_ItemToCard,					//카드아이템을 사용하여 카드로 변환
		Deck_DigimonToCard,					//Digimon->영웅카드로 변환
		Deck_MoveInvenToDeck,				//카드보관함 -> 덱
		Deck_MoveDeckToInven,				//덱->카드보관함
		Deck_AddCardSlot,					//카드보관함 확장
		Deck_RemoveCard,					//보유 카드 삭제

		//로비화면
		Lobby_Init,							//로비서버 초기화	
		Lobby_List,							//로비서버 대기실 목록
		Lobby_Update,						//특정 로비 상태 변경
		Lobby_UserRecord,					//자신의 랭킹정보 변경
		Lobby_OtherRecord,					//특정유저의 랭킹정보

		Lobby_Create,						//로비방 생성
		Lobby_Join,							//로비방 참여
		Lobby_Quit,							//로비방 대기중 나가기
		Lobby_DirectJoin,					//바로참여 버튼

		//매치메이킹 수락,거절관련
		Matching_Enqueue,					//자동매칭 큐 진입
		Matching_Leave,						//자동매칭 취소
		Matching_Matched,					//매치잡힘 , 수락,거절 화면으로 전환
		Matching_Accept,					//매치 수락
		Matching_Reject,					//매치 거절
		Matching_Canceled,					//해당 매치가 취소됨
		Matching_Start,						//해당 매치가 시작됨
		Matching_ServerInfo,				//다음 게임 서버 정보

		//PVP
		Game_Init,							//게임 초기화
		Game_UserConnect,					//다른 플레이어가 접속됨
		Game_UserLoadComplate,				//특정플레이어가 로딩완료됨
		Game_AllLoadComplate,				//모든유저가 로딩완료됨 , 해당시점에서부터 시작준비가능

		

		End		
	};
};