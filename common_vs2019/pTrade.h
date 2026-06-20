

#pragma once


#include "pNetwork.h"

namespace pTrade
{
	enum
	{
		Begin = nScope::Trade,

		Request,			// 요청
		Approval,			// 거래 요청 승인

		Complete,			// 완료 선택
		CompleteFinal,		// 최종 거래 완료
		CompleteCancel,		// 거래 상태 취소

		Reject,				// 거절
		Impossible,			// 처리할 수 없는 상태

		AddItem,			// 아이템 추가
		AddMoney,			// 돈추가

		////////////////////////////////////////////////////////////////////////////
		// 테이머 개인 상점 처리
		TamerShopReady,		// 테이머 상점 개설 준비 단계
		TamerShopOpen,		// 테이머 개인 상점 개설
		TamerShopClose,		// 테이머 개인 상점 폐쇄

		TamerShopBuy,		// 테이머 개인 상점에서 물건 구매(아이템 이동 : shop->other tamer)
		TamerShopSell,		// 미사용, 테이머 개인 상점에서 물건 판매(아이템 이동 : other tamer->shop) ==> 테이머 개인 상점에서는 제공하지 아니함

		TamerShopList,		// 테이머 개인 상점에 있는 아이템 목록

		////////////////////////////////////////////////////////////////////////////
		// 위탁 상점 처리		
		ShopOpen,			// 위탁 상점 오픈
		ShopClose,			// 위탁 상점 폐쇄
		ShopBuy,			// 위탁 상점 구매(아이템 이동 : shop->other tamer)
		ShopSell,			// 위탁 상점에 물건 판매(아이템 이동 : other tamer->shop)
		ShopList,			// 위탁 상점 아이템 목록 요청
		ShopWithdraw,		// 조합 상인 npc에게서 아이템 회수 요청(단품)

		ShopItem,			// 위탁 상점 의 아이템 구매 & 판매 처리		
		ShopNPCItemList,	// 조합상인이 소지하고 있는 아이템 리스트 요청

		ItemAddToTamer,		// 테이머에게 아이템 지급(new)
		ItemRemoveToTamer,	// 테어머의 아이템 제거(new)

		CardRequest,		// 마스터카드 거래 요청			// 일반 거래 요청과 동일한 루틴을 탈뿐 코드는 똑같다. 카드 거래타입인 것만 다르다.
		CardApproval,		// 마스터카드 거래 요청 승인	//
		CardAddItem,		// 마스터카드 아이템 추가		// 일반 거래 아이템 추가와 다른 점은 거래창에서 보여지는 아이템의 타입이 다르다. 서버는 이부분 주의해야 한다.
		CardComplete,		// 마스터카드 거래 요청 승인	// 일반 거래 완료와 달리 카드 아이템은 카드 UI->인벤토리로 가게 된다.
		CardCompleteFinal,	// 마스터카드 거래 최종 완료	// 카드거래가 끝나면 기존 패킷과 다르게 써야 한다면 해당 패킷 사용

		CancelItem,			// 아이템 등록 취소

		InvenLock,			// 거래창 잠금

		End
	};

};


namespace nTrade
{
	struct Item
	{
		//n4 m_nType;		// 아이템 타입
		//n2 m_nNumber;	// 매매를 위해 등록된 아이템 개수
		cItemData m_sItem;		// 등록된 아이템 정보
		n8 m_nPrice;	// 아이템 개별 가격		
		n1 m_nIsSell;	// 1 : 판매용 아이템, 0 : 구매용 아이템		
	};

	enum
	{
		None,			// 거래중 아님
		Request,		// 거래 요청~승인 처리중
		Approval,		// 아이템 올려놓는 과정
		InvenLock,		// 거래창 잠금		
		Confirmed,		// 아이템 거래 최종 확인
	};
};


namespace nCommissionLog
{
	enum
	{
		ShopOpen,				// 위탁 상점 오픈					
		ShopExpire,				// 상점 기간 만료		
		ShopUseClosing,			// 유저가 직접 위탁 상점을 직접 닫음
		ShopAutoClosing,		// 아이템이 모두 다 판매되어 자동으로 상점이 클로즈됨		
		ShopClose,				// 조합 상인의 아이템이 모두 회수되어 폐쇠됨
	};
};