#pragma once

namespace eRelocateTamerResult
{
	const int SUCCESS						= 10100;	// 이전신청 성공
	const int DUPLICATE_TAMER_NAME			= 10101;	// 이전할 서버에 같은 이름의 테이머가 있음
	const int DENY_TAMER_NAME				= 10102;	// 허용되지 않은 테이머 이름
	const int NOT_ALLOW_TAMER_LEVEL			= 10103;	// 테이머의 레벨이 낮음
	const int DISAGREE_TERM					= 10104;	// 약관에 미동의
	const int JOINED_GUILD					= 10105;	// 길드에 가입중
	const int OPENED_COMMISSION_SHOP		= 10106;	// 위탁 상점이 오픈되어 있음
	const int COOL_DOWN						= 10107;	// 재사용 대기시간에 걸림
	const int NOT_ENOUGH_TAMER_SLOT			= 10108;	// 테이머 슬롯이 부족
	const int NOT_CONNECT_MANAGER_SERVER	= 10109;	// 매니저 서버가 실행되어있지 않음
	const int NOT_FOUND_ACCOUNT				= 10110;	// 요청한 사용자를 찾을 수 없음
	const int NOT_FOUND_CORE_SERVER			= 10111;	// 코어서버를 찾을 수 없음
	const int UNKNOWN						= 10112;	// 알수 없는 오류
	const int NOT_MATCH_ITEM				= 10113;	// 테이머 서버이전 이용권 아이템이 존재하지 않습니다.
	const int NO_SELECTED_SERVER			= 10114;	// 이전할 서버를 선택하지 않았습니다.
	const int BLANK_TAMER_NAME				= 10115;	// 이전할 서버에서 사용할 테이머 이름을 입력하지 않았습니다.
	const int JOINED_PARTY					= 10116;	// 파티에 참가중
	const int DENY_RELOCATE					= 10117;	// 서버 이전 내역이 존재합니다.
	const int FAIL_RELOCATE					= 10118;	// 테이머 서버 이전 스케줄 등록 실패. 이전 기록이 있음
	const int DENY_USE_MAP					= 30336;	// 사용할수 없는 지역
}
