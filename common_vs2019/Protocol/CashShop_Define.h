#pragma once

#include "common_vs2019/plength.h"
#include "common_vs2019/pCashShop.h"

struct BILLING_RESULT_INVENTORY_LIMITE
{
	int		limitTS;
	char	limiteCountry[Language::pLength::COUNTRY + 1];
};


const BILLING_RESULT_INVENTORY_LIMITE	BILLING_RESULT_INVENTORY_LIMITE_TS[] = {
	{ 60 * 60 * 48, "ID"}			// 캐쉬 아이템 창고에서 아이템 찾기시 제한 시간(48시간, 인도네시아)
};

#define TRADE_LIMITE_TIME_DALAY		10	// 서버 시간에서 10초 정도 +시킨다. 서버와 시간 동기화 문제때문임.

