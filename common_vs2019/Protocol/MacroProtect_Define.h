#pragma once

#ifdef VERSION_USA
#include "common_vs2019/Define/Define_USA.h"
#elif defined VERSION_TH
#include "common_vs2019/Define/Define_TH.h"
#elif defined VERSION_HK
#include "common_vs2019/Define/Define_HK.h"
#elif defined VERSION_TW
#include "common_vs2019/Define/Define_TW.h"
#elif defined VERSION_KOR
#include "common_vs2019/Define/Define_KOR.h"
#endif

#define MACROPROTECT_MAX_CHANCE_COUNT 3

//해당 ms보다 빠르게 응답했을경우 오답처리
#define dfMACROLIMITRESPONSETIME 1000

//최소 N글자이상 있어야 문자만이나 숫자만 가능
#define dfMINQUESTIONLEN 2  
#define dfMAXQUESTIONLEN 3

//서버용
enum eMacroActiveType
{
	SYSTEM = 0,
	USER
};
//서버용
enum  eMacroResult 
{
	CORRECT = 0,
	LOGOUT,			//로그아웃해버림
	INCORRECT,		//캡챠틀림
	TIMEOUT,		//시간초과
	FASTRES,		//너무빠르게 응답
};

namespace nsMacroProtector
{
	enum QuestionType
	{
		NONE,
		OnlyNumber,			//숫자만
		OnlyCharactor,		//문자만
		SpecificPosision,	//몇개만 순서대로 입력하기
		SumAllNumber,		//모든 숫자 더하기
		TYPEMAX,
	};

}


struct stMacroProtectorData
{
public:
	u8 nMacroReqTameridx;
	n1 nChanceLeft;
	n4 nMacroKey;
	n4 nMacroMapChangeLeftTime;
	nsMacroProtector::QuestionType eQuestionType;

	void Clear() 
	{
		nMacroReqTameridx = 0;
		nChanceLeft = 0;
		nMacroKey = 0;
		nMacroMapChangeLeftTime = 0;
		eQuestionType = nsMacroProtector::QuestionType::NONE;
	}

	
};