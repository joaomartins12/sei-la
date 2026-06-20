

#pragma once


#include "nScope.h"


namespace pLogin
{

	enum
	{
		Begin = nScope::Login,

		Request,
		Success,
		Failure,

		OnTheAnvil,		// 준비중

		WaitTime,		// 로그인 처리 대기 시간

		FailureGSP,		// GSP : 인증실패
		RequestAeriaChannel,	// 아에리아 채널링
		AccountBan,		//계정 밴 정보를전송
		Birthday,		//한국 셧다운제 생년월일 전송

		End
	};

	enum
	{
		ErrID,
		ErrPass,

		Online,		// 사용중
		Waiting,	// 인증 처리중
		Blocking,	// 사용 정지중

		ErrAge,		// 12세 이하

		NotOpen,	// 서버가 아직 열리지 아니하였습니다.
		Ban,		// 접속 제한 중인 상태

		HttpError,		// GSP : 인증서버 오류
		AccoutDBError,	// GSP : UseAgreement DB 오류
		NotAgreement,	// GSP : Use Agreement Not
		IPBlock,			//GSP : IP 차단
		PlayTimeOut,
		PopErrMsg,	// ERR 메시지 팝해주세요

		SystemError,
		UNKNOWN_NICK,
		UNKNOWN_EMAIL,		

		ErrLoginPass,	// 북미 일때만 아이디/비번 틀릴경우 서버 에러 타입
		EMPTY_TOKEN,
		UNKNOWN_TOKEN,
		UNKNOWN_TOKENTIME,
	};

};




