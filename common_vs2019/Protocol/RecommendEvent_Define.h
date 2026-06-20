#pragma once

#include "../pEvent.h"

namespace Recommend
{
	namespace MyState
	{
		enum
		{
			Impossible		= nEvent::Friendly_NONE,
			Possilble		= nEvent::Friendly_Save,	// 대상을 추천할 수 있는 상태 임
			FirstProduce	= nEvent::Friendly_First,	// 최초 생성된 캐릭터
		};
	}
	namespace Type
	{
		enum
		{
			Sent			= nEvent::Friendle_Type1,	// 내가 추천
			Received		= nEvent::Friendle_Type2,	// 나를 추천
		};
	}

	namespace MarkValue
	{
		enum
		{
			Uncompleted		= nEvent::MARK_NONE,		// 미달성
			Completed		= nEvent::MARK_ARCHIVE,		// 달성
			ReceivedReward	= nEvent::MARK_GET,			// 달성 보상 획득
		};

	}

	namespace SuccessType
	{
		enum
		{
			None,
			LevelUp,		// 레벨업
			NewArea,		// 새로운 지역 진입
			Achieve,		// 업적 달성
		};
	}

	namespace ItemType // 트리 아이템 타입
	{
		enum
		{
			None,
			TamerNameCategory, // 추천 테이머 상위 
			TamerName,
			PurposeCategory,
			Max
		};
	}
}
