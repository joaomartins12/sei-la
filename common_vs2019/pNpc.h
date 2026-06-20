
#pragma once

namespace nNpc
{
	enum eAttribute
	{
		Map,
		X,
		Y,
		Type,

		ItemCount,	// shop npc인 경우 

		MoveType,	// 고정, 선형이동, 영역이동등 처리

		MaxAttribute
	};

	enum eType
	{
		TypeNormal			= 0,	// General - Give or receive a quest
		TypeShop			= 1,	// Merchant
		TypeShopDT			= 2,	// Digitama Merchant
		TypePortal			= 3,	// Portal NPC
		TypeIncubator		= 4,	// Mercenary Incubator
		TypeSocket			= 5,	// Item analysis and socket processing
		TypeWarehouse		= 6,	// Warehouse manager
		TypeCareSlot		= 7,	// Digimon Archive
		TypeGuild			= 8,	// Merchant, guild creation, guild wrap-up
		TypeDigiCore		= 9,	// Merchant sells Digicore-related items ==> Only the number of Digicores is checked, other merchants are processed at the standard purchase price.
		TypeCapsuleMachine	= 10,	// Capsule Machine
		TypeSkill			= 11,	// Skill Advanced (Skill Pointer)
		TypeShopEvent		= 12,	// Merchant, Event Store
		TypeRichardShop		= 13,	// Richard Quests ??
		TypeShopBarter		= 14,	// Barter Shop
		TypeRareMachine		= 15,	// Rare machine
		TypeDonate			= 16,	// Masters Matching System Donate
		TypeMysteryMachine	= 17,	// Mystery Machine
		TypeSpiritChange	= 18,	// Spirit Exchange
		TypePinochiCard		= 19,	// Pinocchimon Card Game
		TypeMake			= 20,	// Item Making

		TypeArena			= 22,	// Arena/Coliseum
		TypeArenaBoard		= 23,	// Arena/Coliseum Board Guide
		TypeExtraEvo		= 24,	// Special evolution

		TypeChangeTName		= 29,	// NPC to change Name ??
		TypeTimeLimited		= 30,	// When an item disappears due to a time limit
	};

	enum eMoveType
	{
		None	= 0,		// Dont move
		Move	= 1,		// Move
	};
};




