

#pragma once

#include "nScope.h"

namespace pParty
{
	enum
	{
		Begin = nScope::Party,

		Invite,				// 초대 요청
		InviteResult,		// 초대응답 (초대수락,초대거부)
		InviteImpossible,	// 다른 작업중으로 인한 초대 요청 불가

		Chat,				// 파티 채팅 메세지
		Join,				// 파티에 들어옮
		Kick,				// 강퇴
		Leave,				// 떠남

		ChangeMaster,		// 마스터 변경
		ChangeCropType,		// 아이템 습득 방식 변경

		MemberList,			// 자신을 제외한 파티 맴버들의 정보
		Exp,				// 파티에 의한 추가 경험치 획득
		Disconnected,		// 연결 종료됨

		Info,				// hp, ds 변화시
		InfoFirst,			// 첫번째 동기화 데이타

		ChangeMap,			// 맵이나 채널 변경시
		ChangeDigimon,		// 디지몬 변경시

		Destroy,			// 파티가 파괴됨

		Die,				// 파티원 죽음
		Created,			// 파티 생성됨, 생성한넘은 파티장임

		Position,			// 위치 동기화

		ExpBuff,			// 파티 경험치 버프 사용
		PCBangExpBuff,		// PC방 파티 경험치 버프 적용

		SyncPartyBuff,		// 테이머 스킬로 버프가 적용瑛?때, 어떤 유저의 테이머에게 적용이 榮쩝?

		End
	};


};

namespace nParty
{
	enum
	{
		Free = 0,	// 가입해 있지 아니함
		Master,		// 파티 마스터
		Member,		// 파티 멤버로 가입되어 있음
	};
};


namespace nCrop
{
	namespace RareRate
	{
		enum
		{
			Lv1 = 1, // ItemClass ~2 , 모두 주사위 처리			
			Lv2 = 2, // ItemClass  3 , 0~2 자유분배, 3~ 주사위 분배 
			Lv3 = 3, // ItemClass  4 , 0~3 자유분배, 4~ 주사위 분배 
			Lv4 = 4, // ItemClass  5~, 0~4 자유분배, 5~ 주사위 분배
		};
	};

	enum	// 아이템 습득 방식
	{
		Base = 0,	// 선행 공격자 우선 습득

		Free,		// 자유 획득 - 우선 습득자 취득
					// 레어도 값이 0이면 모든 아이템 자유 분배
					// 레어도 값이 있다면 해당 레어도 보다 높은 아이템은 주사위 획득

		Order,		// 순서대로 습득

		End
	};

};



