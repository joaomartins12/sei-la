#pragma once

#define LANGUAGE_SUPPORT

#define DEFAULT_COUNTRY_CODE			"ZZ"	// ZZ = Desconhecido

namespace Language
{
	namespace pCountry
	{
		#ifdef  VERSION_KOR

			const int CHARACTER_SET = 949;

			enum eSize
			{
				SizeID				= 21,
				SizePass			= 128,
				SizeAccessToken		= 255,
				SizeCharName		= 32,
				SizeGuildName		= 48,
				SizeGuildNotice		= 255,
				SizeGuildClassName	= 32,
				SizeGuildClassNameDesc = 72,
				SizeGuildMemo		= 64,
				SizeShopName		= 128,
				SizeBuddyMemo		= 255,
				#ifdef SERVER_KOREANDNS_UIRENEWAL_INVENTORYMODE_190325
					SizeInvenMax		= 150,
				#else
					SizeInvenMax		= 84,
				#endif
				SizeSecondPass		= 33,
				SizeFirstInven      = 30,
				SizeFirstWarehouse  = 21,
				SizeFirstShareStash = 14,
				SizeDigimonBaseSlot = 4,
			};

			enum eMinLen	// 글자 길이제한
			{
				MIN_LenID				= 3,
				MIN_LenPass				= 3,
				MIN_LenCharName			= 2,
				MIN_LenGuildName		= 2,
				MIN_LenGuildClassName	= 2,
				MIN_LenGuildClassNameDesc = 2,
				MIN_LenGuildNotice		= 0,
				MIN_LenGuildMemo		= 0,
				MIN_LenShopName			= 1,
				MIN_LenBuddyMemo		= 2,
				MIN_LenMsg				= 1,
			};

			enum eMaxLen	// 글자 길이제한
			{
				MAX_LenID				= 12,
				MAX_LenPass				= 16,
				MAX_LenCharName			= 8,
				MAX_LenGuildName		= 14,
				MAX_LenGuildClassName	= 8,
				MAX_LenGuildClassNameDesc = 21,
				MAX_LenGuildNotice		= 120,
				MAX_LenGuildMemo		= 31,
				MAX_LenShopName			= 17,
				MAX_LenBuddyMemo		= 23,
				MAX_LenMsg				= 250,
			};

			enum eData
			{
				TamerLevelMax		= 120,
				DigimonLevelMax		= 120,
				EvoStep				= 9, // 최대 9단계 진화(0~7), 8번째는 퇴화
				MaxAttrInchantLevel = 15,
				#ifdef SERVER_KOREANDNS_DIGIMON_TRANSCENDENCE_190412
					MaxInchantLevel =	75,
				#else
					MaxInchantLevel =	60,
				#endif
				MAX_CHANNELING		= 5,
				MAX_ItemSkillDigimon = 2,
				DropItem			= 35,
				MAX_DIGIMONSKILLLevel	= 30,
			};

		#else

			const int CHARACTER_SET = 949;

			enum eSize
			{
				SizeID					= 30,		// 21
				SizePass				= 120,		// 128
				SizeAccessToken			= 255,		// 255
				SizeCharName			= 32,		// 32
				SizeGuildName			= 48,		// 32
				SizeGuildNotice			= 255,		// 255
				SizeGuildClassName		= 32,		// 32
				SizeGuildClassNameDesc	= 72,		// 72
				SizeGuildMemo			= 64,		// 64
				SizeShopName			= 128,		// 128
				SizeBuddyMemo			= 255,		// 255
				SizeInvenMax			= 150,		// 150
				SizeSecondPass			= 33,		// 33
				SizeFirstInven			= 30,		// Inital Inventory Space
				SizeFirstWarehouse		= 21,		// Initial Warehouse Space
				SizeFirstShareStash		= 14,		// Account Warehouse Space
				SizeDigimonBaseSlot		= 9,		// Digimon Slot
			};

			enum eMinLen
			{
				MIN_LenID					= 3,	// 3
				MIN_LenPass					= 3,	// 3
				MIN_LenCharName				= 2,	// 2
				MIN_LenGuildName			= 2,	// 2
				MIN_LenGuildClassName		= 2,	// 2
				MIN_LenGuildClassNameDesc	= 2,	// 2
				MIN_LenGuildNotice			= 0,	// 0
				MIN_LenGuildMemo			= 0,	// 0
				MIN_LenShopName				= 1,	// 1
				MIN_LenBuddyMemo			= 2,	// 2
				MIN_LenMsg					= 1,	// 1
			};

			enum eMaxLen
			{
				MAX_LenID					= 12,
				MAX_LenPass					= 16,
				MAX_LenCharName				= 8,
				MAX_LenGuildName			= 14,
				MAX_LenGuildClassName		= 8,
				MAX_LenGuildClassNameDesc	= 21,
				MAX_LenGuildNotice			= 120,
				MAX_LenGuildMemo			= 31,
				MAX_LenShopName				= 17,
				MAX_LenBuddyMemo			= 23,
				MAX_LenMsg					= 250,	
			};

			enum eData
			{
				TamerLevelMax			= 120,		// Max Level tamer
				DigimonLevelMax			= 120,		// Max Digimon Level
				EvoStep					= 9,		// EvoStep
				MaxAttrInchantLevel		= 15,		// Max Clon Level
				MaxInchantLevel			= 75,		// Total Clon Level
				MAX_CHANNELING			= 5,		// 
				MAX_ItemSkillDigimon	= 2,		// 
				DropItem				= 35,		// 
				MAX_DIGIMONSKILLLevel	= 30,		// Max Skill Level
			};

		#endif
	}

};