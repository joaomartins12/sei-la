#pragma once

namespace eDuelErr
{
	const int NOT_MAP						= 40007;	// 맵이 없음
	const int FAILED_TO_CREATE_MAP			= 40008;	// 맵 생성 실패
	const int DURING_PARTY					= 40000;	// 파티 중
	const int DURING_PARTY_INVATE			= 40001;	// 파티 신청중이면 참가 안됨
	const int TAMER_LEVEL_LOW				= 40009;	// Tamer 레벨이 낮음
	const int DIGIMON_LEVEL_LOW				= 40006;	// Digimon 레벨이 낮음
	const int PLAY_COUNT_OVER				= 40004;	// 입장 가능 횟수 초과
	const int DIGIMON_IS_DEATH				= 40010;	// Digimon이 죽어 있음
	const int NPC_DISTANCE_FAR				= 40011;	// NPC와 거리가 멀어서 신청 할 수 없음
	const int NPC_NOT_EXIST					= 40012;	// 신청 NPC가 없음
	const int DUEL_MATCH_REGISTER_SUCCESS	= 9;		// 듀얼 신청 성공
	const int ALREADY_WAITING_ENTER			= 40002;	// 이미 참여 대기 중

	const int DUEL_ENTER_REPLY_WAITING		= 40021;	// 배틀 응답 대기 중
	const int PLAY_DUEL						= 40022;	// 배틀 중
	const int IN_DUNGEON					= 40023;	// 현재 위치가 던전이라서 신청 할 수 없음
	const int NOT_AVAILABLE_PARTNER_DIGIMON	= 40005;	// 사용 가능한 파트너몬이 아님
	const int DIGIMON_NOT_EXIST				= 40024;	// 파트너몬이 없음
	const int TICKET_LOW					= 40003;	// 참여 티켓이 부족
	const int NOT_TABLE_DATA				= 40025;	// 기본 베이스 테이블 데이타 없음
	const int AVAILABLE_TIME				= 40026;	// 이용 가능한 시간
	const int NOT_AVAILABLE_TIME			= 40015;	// 참여 가능한 시간이 아님
	const int NOT_WAITING_REGISTER			= 40027;	// 참여 대기자 리스트에 존재하지 않음

	const int NOT_DUEL_ROOM_EXIST			= 40028;	// 듀얼이 존재 하지 않음
	const int NOT_DUEL_AWAITING_RESPONSE	= 40029;	// 응답 대기중인 상태가 아님
	const int NOT_DUEL_IN					= 40030;	// 듀얼에 참여 중이지 않다.
	const int CANCEL_OF_TAMER				= 24;		// 테이머의 취소로 Duel 방이 사라짐.
	const int END_DUEL						= 25;		// 듀얼 종료
	const int NOT_DUEL_BASE_DATA			= 26;		// 기본 데이타가 존재하지 않음
}
