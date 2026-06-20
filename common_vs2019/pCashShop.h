
#pragma once

#include "nScope.h"


namespace pCashShop
{
	const int MAX_CASHITEM_COUNT		= 12;

	enum
	{
		Begin = nScope::CashShop,

		BuyRequest,			// 아이템 구매전 요청(스팀)
		Buy,				// 아이템 구매 요청 및 결과
		Gift,				// 아이템 선물 요청 및 결과

		Balance,			// 현재 남아있는 잔액 요청 및 결과

		Cart,				// 장바구니 목록 요청 및 응답
		CartSave,			// 장바구니 저장, 창(캐시샵) 닫을 경우만 유효

		FailBalance,		// 조회 실패. hjlim. 20110817
		FailHttp,			// http 에러. hjlim. 20110817
		FailBuyRequest,		// 구매전 요청 실패(스팀)
		FailBuy,			// 구매 실패. 
		InvoiceID,			// 구매 요청 : invoice 발금 요청 20110818
		
		BuyHistory,

		MultiBuy,			// 아이템 다중구매 요청 및 결과
		
		VIPAutoPayment,     // VIP 멤버십 자동 결제

		End
	};

	// error code	
	enum
	{
		SUCCESS = 0,
		BILLING_FAIL_CLOSE_CASHSHOP		= 1,	//클라이언트 캐쉬창 강제로 닫음
		BILLING_FAIL_NOT_EXIST_TAMER	= 2,	//클라이언트 캐쉬창 강제로 닫음
		BILLING_FAIL_BUY_CASHITEM		= 3,	//아이템 구매 실패 메시지		
		BILLING_FAIL_GIFT_CASHITEM		= 4,	//아이템 선물 실패 메시지
		BILLING_FAIL_BAN_USER			= 99,	//유저블록
	};
};



