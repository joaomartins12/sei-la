#pragma once

namespace nsCashShopResult
{
	const int	BILLING_RESULT_CROPCASHITEM_SUCCESS				= 100;			// 캐쉬아이템 회수	성공

	const int	BILLING_RESULT_CASH_NOT_ENABLE_SLOT				= 20150;		// 이용 가능한 슬롯이 아닙니다.			
	const int	BILLING_RESULT_CASH_NOT_SERVER_DATA				= 20150;		// 서버에 요청한 슬롯 아이템 정보와 실제 아이템 정보가 다릅니다. 갱신 정보 수신

	const int	BILLING_RESULT_CASH_NOT_ENOUGH					= 30310;		// 잔액이 부족합니다.
	const int	BILLING_RESULT_CASHITEM_NOT_PURCHASE			= 30311;		// 아이템 구매 실패

	const int	BILLING_RESULT_CASHITEM_NOT_INVENTORY_LIMITE	= 30322;		// 캐쉬 아이템 창고에서 아이템 찾기시 제한 시간 메시지

	const int Success				= 0;
	const int CashError				= 31010;		//캐시 관련오류(부족등)
	const int ItemError				= 31011;		//아이템이 존재하지않음
	const int PartBuyError			= 31017;			//구매과정중 일부는 성공했지만 나머지가 실패한상태
	const int BuyError				= 31012;		//구매과정중 오류 발생

	const int GiftPeerNotBuddy		= 31013;		//선물 대상이 친구가 아님
	const int GiftpeerError			= 31014;		//선물 대상을 찾을수 없음
	const int GiftError				= 31015;		//선물과정중 오류 발생
	const int GiftCashError			= 31016;		//캐시 관련오류(부족등)
}
