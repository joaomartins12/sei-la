
#pragma once

#include "nScope.h"


// 카드스캐너 관련 프로토콜 처리
namespace pScanner
{
	enum
	{
		Begin = nScope::Scanner,

		Equip,		// 스캐너 장착시 - 주변에 동기화 처리됨
		On,			// 카드 스캐터를 장착 - 맵이동간 사용
		Off,		// 카드 스캐너를 장착 해제 했음을 알림

		Read,		// 카드 스캐너에서 카드를 리딩

		Register,	// 장치 등록
		Release,	// 장치 해제

		Error,		// 스캐너 인식되지 않은 상태에서 카드 리딩
	};


};


#define d_szDummyCard L"KMAAAGPRNZ8K4Q0"

namespace nScanner
{
	enum
	{
		NoLength	= 12,
		CodeLength	= 13,
		CardLength	= 15,
	};

	enum
	{
		None,			// 스캐너 착용중 아님
	};

	enum
	{
		RegisterSuccess = 1,	// 등록 성공
		RegisterFailure,		// 이미 사용중으로 등록 실패

		NoCodeNumber = None,	// 존재하지 아니한 코드 번호,  등록이나 장착 불가능
	};
};










