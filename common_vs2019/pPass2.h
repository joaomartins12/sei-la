


#pragma once


#include "nScope.h"

namespace pPass2
{
	enum
	{
		Begin = nScope::Pass2,

		Register,	// 2차 비번 등록

		On,			// 2차 비번 사용		==> 2차 비번이 등록된 이후에 설정 가능
		Off,		// 2차 비번 사용 안함	==> 2차 비번이 등록된 이후에 설정 가능

		Check,		// 2차 비번 체크
		Change,		// 2차 비번 변경
		ChangePass,	// 2차 비밀번호 변경(위의 패킷이 어느정도 되어있어서 새로 만듬)

		End
	};

};


 
namespace nPass2
{
	enum
	{
		NewSet		= 0,
		Registered	= 1,	//2차 패스워드 설정됨		
		Certified	= 2,	//2차 패스워드 확인 완료된 상태
		Skiped		= 3,	//2차 패스워드 스킵 된 상태
	};

	enum
	{
		SecondPassMaxTry = 5,
	};


};







