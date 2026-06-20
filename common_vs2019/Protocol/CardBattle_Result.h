#pragma once

namespace BattleResult
{
	namespace Enter
	{
		const int Success = 0;
		const int Fail = 0;
	};

	namespace ItemToCard
	{
		const int Success = 0;
		const int InvalidItem = 0;		//아이템이이상함
		const int FullCardInven = 0;	//카드인벤 꽉참
		const int Pending = 0;			//진행중
	}

	namespace DigimonToCard
	{
		const int Success = 0;
		const int InvalidItem = 0;		//아이템이이상함
		const int FullCardInven = 0;	//카드인벤 꽉참
		const int Pending = 0;			//진행중
		const int InvalidDigimon = 0;	//디지몬 없음
	}

	namespace InvenToDeck
	{
		const int Success = 0;
		const int CostOver = 0;			//코스트 초과
		const int InvalidCard = 0;		//카드가 없음
	}

	namespace DeckToInven
	{
		const int Success = 0;
		const int InvalidCard = 0;		//카드가 없음
	}

	namespace CardRemove
	{
		const int Success = 0;
		const int InvalidCard = 0;		//카드가 없음
		const int CardInDeck = 0;		//덱안의 카드는 지울수 없음
	}

	namespace CardSlotAdd
	{
		const int Success = 0;
		const int InvalidItem = 0;		//확장 아이템 없음
		const int AllOpened = 0;			//이미 최대 확장상태
	}

}