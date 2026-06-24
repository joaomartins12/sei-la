
#include "stdafx.h"

#include "cNetwork.h"
#include "cCliGame.h"

#include "../Global.h"
#include "../Engine.h"

#include "common_vs2019/pGame.h"
#include "common_vs2019/pTamer.h"
#include "common_vs2019/pDigimon.h"
#include "common_vs2019/pSkill.h"

#include "common_vs2019/pLimit.h"
#include "common_vs2019/pQuest.h"

#include "common_vs2019/pItem.h"
#include "common_vs2019/pInven.h"
#include "common_vs2019/pDigivice.h"

#include "common_vs2019/cItemData.h"
#include "common_vs2019/cEvoUnit.h"

#include "common_vs2019/cMapRegion.h"

#include "common_vs2019/pScanner.h"
#include "common_vs2019/pPass2.h"

#include "common_vs2019/cBattleInfo.h"

#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"

#include "common_vs2019/pEvent.h"

#include "../nProtect/Client_nProtect.h"

//bool g_bTempRevPortal = false; // 안쓰는 변수이다.

#ifdef SDM_DEF_XIGNCODE3_20181107
#include "../xigncode3/Client_Xigncode3.h"
#endif

// Receive Channel info
void cCliGame::RecvChannelInfo(void)
{
	GS2C_RECV_CHANNEL_LISTINFO recv;

	//DBG("\nChannel: %d\n", recv.channel);
	//DBG("Channel_Result: %d\n", recv.m_nResult);

	do
	{
		u1 cChannelIDX;
		pop(cChannelIDX);

		//DBG("Channel_ID: %d\n", recv.channel[cChannelIDX]);

		if (cChannelIDX == 0xFF)
			break;

		pop(recv.channel[cChannelIDX]);

	} while (1);

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_CHANNEL_LIST_INFO, &recv);
}

// Reset skill points
void cCliGame::RecvInitDigimonSkillPoint(void)
{
	//	cCliGame::RecvInitGameData(void)

	cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();
	cData_PostLoad::sDATA* pDigimonData = pPostLoad->GetDigimonData();

	pop(pDigimonData->s_nMaxEvoUnit);
	pop(&pDigimonData->s_EvoUnit[1], sizeof(cEvoUnit) * pDigimonData->s_nMaxEvoUnit);

	g_pCharMng->GetDigimonUser(0)->SetAttributeEvoUnit(pDigimonData->s_nMaxEvoUnit, pDigimonData->s_EvoUnit);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_SKILL_DIGIMONSTATUS, NULL);
}

// Inicialize Game
void cCliGame::RecvInitGameData(void)
{
	OutputDebugStringA("[RECV_INIT] RecvInitGameData begin\n");

	try
	{
		DBG("\n--------- Init Game Data ---------\n\n");

		OutputDebugStringA("[RECV_INIT] Init tactics\n");
		g_pDataMng->GetTactics()->Init();

		cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();

		cData_PostLoad::sDATA* pTamerData = pPostLoad->GetTamerData();
		cData_PostLoad::sDATA* pDigimonData = pPostLoad->GetDigimonData();

		cData_Inven* pInven = g_pDataMng->GetInven();
		cData_TEquip* pTEquip = g_pDataMng->GetTEquip();
		cData_Digivice* pDigivice = g_pDataMng->GetDigivice();

		OutputDebugStringA("[RECV_INIT] before LibType\n");

		nLIB::eType LibType = nLIB::SVR_GAME;
		pop(LibType);

		nsCsGBTerrain::g_nSvrLibType = LibType;

		OutputDebugStringA("[RECV_INIT] before position\n");

		nSync::Pos pos;
		pop(pos);
		pPostLoad->SetPos(pos);

#ifdef SDM_TAMER_SERVERRELOCATE_20170911
		u4 nServerRelocateCompleteTime = 0;
		pop(nServerRelocateCompleteTime);
		pPostLoad->SetServerRelocateCompletedTime(nServerRelocateCompleteTime);
#endif

		OutputDebugStringA("[RECV_INIT] before Tamer type/name\n");

		pop(pTamerData->s_Type);

		char szName[Language::pLength::name + 1] = "";
		pop(szName);

		_tcscpy_s(pTamerData->s_szName, Language::pLength::name + 1, LanConvertT(szName));

		memset(szName, 0, sizeof(szName));

#ifndef UI_INVENTORY_RENEWAL
		OutputDebugStringA("[RECV_INIT] before money/inventory size\n");

		n8 nMoney;
		pop(nMoney);
		pInven->SetMoney(nMoney, false);

		u2 nInvenSlotCount;
		u2 nWarehouseSlotCount;

#ifdef NEW_SHARESTASH
		u2 nShareSlotSlotCount;
#endif

		pop(nInvenSlotCount);
		pop(nWarehouseSlotCount);

#ifdef NEW_SHARESTASH
		pop(nShareSlotSlotCount);
#endif

		pInven->SetInvenSlotCount(nInvenSlotCount);

		GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_NORMAL_SLOTCOUNT, &nWarehouseSlotCount);

#ifdef NEW_SHARESTASH
		GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_SHARE_SLOTCOUNT, &nShareSlotSlotCount);
#endif
#endif

		OutputDebugStringA("[RECV_INIT] before Tamer stats\n");

		pop(pTamerData->s_nExp);
		pTamerData->s_nExp = pTamerData->s_nExp / 100;

		pop(pTamerData->s_nLevel);

		pop(pTamerData->s_Attribute[MHP]);
		pop(pTamerData->s_Attribute[MDS]);
		pop(pTamerData->s_Attribute[HP]);
		pop(pTamerData->s_Attribute[DS]);
		pop(pTamerData->s_Attribute[FP]);
		pop(pTamerData->s_Attribute[AP]);
		pop(pTamerData->s_Attribute[DP]);
		pop(pTamerData->s_Attribute[MS]);

		cItemInfo* pItemInfo = NULL;

		OutputDebugStringA("[RECV_INIT] before Tamer Equipment\n");

		for (int i = 0; i < nLimit::Equip; ++i)
		{
			pItemInfo = pTEquip->GetData(i);
			pop(*pItemInfo);

			if (pItemInfo->IsEnable())
			{
				if (nsCsFileTable::g_pItemMng->IsItem(pItemInfo->GetType()) == false)
				{
					char log[256];
					sprintf_s(log, "[RECV_INIT][WARN] Invalid Tamer Equipment skipped. EquipIndex=%d ID=%d\n", i, pItemInfo->GetType());
					OutputDebugStringA(log);

					memset(pItemInfo, 0, sizeof(cItemInfo));
				}
			}
		}

		OutputDebugStringA("[RECV_INIT] after Tamer Equipment\n");
		OutputDebugStringA("[RECV_INIT] before Tamer Chipset\n");

		for (int i = 0; i < nLimit::Chipset; ++i)
		{
			pItemInfo = pDigivice->GetChipset(i);
			pop(*pItemInfo);

			if (pItemInfo->IsEnable())
			{
				if (nsCsFileTable::g_pItemMng->IsItem(pItemInfo->GetType()) == false)
				{
					char log[256];
					sprintf_s(log, "[RECV_INIT][WARN] Invalid Tamer Chipset skipped. ChipsetIndex=%d ID=%d\n", i, pItemInfo->GetType());
					OutputDebugStringA(log);

					memset(pItemInfo, 0, sizeof(cItemInfo));
				}
			}
		}

		OutputDebugStringA("[RECV_INIT] after Tamer Chipset\n");
		OutputDebugStringA("[RECV_INIT] before Digivice\n");

		assert(nLimit::Digivice == 1);
		cItemData digiviceItem;
		pop(digiviceItem);
		pTEquip->SetDigiviceItem(&digiviceItem);

		OutputDebugStringA("[RECV_INIT] after Digivice\n");
		OutputDebugStringA("[RECV_INIT] before TamerSkill\n");

		for (int i = 0; i < nLimit::SkillSlot; ++i)
			pop(pDigivice->GetTamerSkill(i), sizeof(cItemData));

		OutputDebugStringA("[RECV_INIT] after TamerSkill\n");

#ifdef INVEN_WRITE_FILE
		FILE* fp;
		fp = fopen("인벤토리정보.txt", "wt");
		for (int i = 0; i < nLimit::Inven; ++i)
		{
			pItemInfo = pInven->GetData(i);
			pop(*pItemInfo);

			char msg[256];
			_stprintf_s(msg, 256, _T("\nIndex = %d, ID = %d, count = %d"), i, pItemInfo->GetType(), pItemInfo->GetCount());
			fwrite(msg, sizeof(char), _tcslen(msg), fp);
		}
		fclose(fp);
#else

#ifndef UI_INVENTORY_RENEWAL
		OutputDebugStringA("[RECV_INIT] before Inventory\n");

		for (int i = 0; i < nLimit::Inven; ++i)
		{
			pItemInfo = pInven->GetData(i);
			pop(*pItemInfo);

			if (pItemInfo->IsEnable())
			{
				if (nsCsFileTable::g_pItemMng->IsItem(pItemInfo->GetType()) == false)
				{
					char log[256];
					sprintf_s(log, "[RECV_INIT][WARN] Invalid Inventory item skipped. InvenIndex=%d ID=%d\n", i, pItemInfo->GetType());
					OutputDebugStringA(log);

					memset(pItemInfo, 0, sizeof(cItemInfo));
				}
			}
		}

		OutputDebugStringA("[RECV_INIT] after Inventory\n");
#endif

#endif

#ifndef UI_INVENTORY_RENEWAL
		OutputDebugStringA("[RECV_INIT] before Warehouse\n");

		int nMaxWareHouse = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse;

		map<int, cItemInfo> mapNormalWareHouse;

		for (int i = 0; i < nMaxWareHouse; ++i)
		{
			cItemInfo pwItemInfo;
			pop(pwItemInfo);

			mapNormalWareHouse.insert(make_pair(i, pwItemInfo));

			if (pwItemInfo.IsEnable())
			{
				if (nsCsFileTable::g_pItemMng->IsItem(pwItemInfo.m_nType) == false)
				{
					char log[256];
					sprintf_s(log, "[RECV_INIT][WARN] Invalid Warehouse item skipped. WareHouseIndex=%d ID=%d\n", i, pwItemInfo.m_nType);
					OutputDebugStringA(log);
				}
			}
		}

		GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_SET_NORMAL, &mapNormalWareHouse);

#ifdef NEW_SHARESTASH
		int nMaxShareStash = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxShareStash;

		map<int, cItemInfo> mapShareWareHouse;

		for (int i = 0; i < nMaxShareStash; ++i)
		{
			cItemInfo pwItemInfo;
			pop(pwItemInfo);

			mapShareWareHouse.insert(make_pair(i, pwItemInfo));

			if (pwItemInfo.IsEnable())
			{
				if (nsCsFileTable::g_pItemMng->IsItem(pwItemInfo.m_nType) == false)
				{
					char log[256];
					sprintf_s(log, "[RECV_INIT][WARN] Invalid ShareWarehouse item skipped. ShareStashIndex=%d ID=%d\n", i, pwItemInfo.m_nType);
					OutputDebugStringA(log);
				}
			}
		}

		GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_SET_SHARE, &mapShareWareHouse);
#endif

		OutputDebugStringA("[RECV_INIT] after Warehouse\n");
#endif

		OutputDebugStringA("[RECV_INIT] before QuestInfo\n");

		pop(*g_pDataMng->GetQuest()->GetInfo());

		DBG("Quests em progresso: %d\n", g_pDataMng->GetQuest()->GetInfo()->GetExecuteCount());
		DBG("Quests completadas: %d\n", g_pDataMng->GetQuest()->GetInfo()->GetQuestCompletionInfo());

		for (int i = 0; i < g_pDataMng->GetQuest()->GetInfo()->GetExecuteCount(); i++)
		{
			DBG("Quest ativada com index: %d\n", g_pDataMng->GetQuest()->GetInfo()->GetExeInfo(i).m_nIDX);

			for (int j = 0; j < 5; j++)
			{
				DBG("Condicao %d da quest: ", g_pDataMng->GetQuest()->GetInfo()->GetExeInfo(i).m_nCondition[j]);
			}
		}

		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_QUEST_INFO_INIT);

		OutputDebugStringA("[RECV_INIT] after QuestInfo\n");
		OutputDebugStringA("[RECV_INIT] before Incubator\n");

		GS2C_RECV_MAKE_DIGITAMA recv;
		pop(recv.m_nEggType);
		pop(recv.m_nEggLevel);
		pop(recv.m_nEggTradeLimitTime);

		pop(recv.m_nBackupDisk);
		pop(recv.m_nBackupdiskTradeLimitTime);

		GAME_EVENT_ST.OnEvent(EVENT_CODE::MAKETACTICS_DIGITAMA_DATA, &recv);

		OutputDebugStringA("[RECV_INIT] after Incubator\n");
		OutputDebugStringA("[RECV_INIT] before Tamer Buffs\n");

		u2 nBuffCount = 0;
		cData_PostLoad::sPostBuff pInfo;

		pop(nBuffCount);

		{
			char log[256];
			sprintf_s(log, "[RECV_INIT] Tamer BuffCount=%d\n", nBuffCount);
			OutputDebugStringA(log);
		}

		if (nBuffCount > 1000)
		{
			OutputDebugStringA("[RECV_INIT][WARN] Tamer BuffCount > 1000, forcing 0\n");
			nBuffCount = 0;
		}

		if (nBuffCount != 0)
		{
			for (int i = 0; i < nBuffCount; i++)
			{
				pop(pInfo.s_nBuffCode);
				pop(pInfo.s_nBuffClassLevel);
				pop(pInfo.s_nBuffEndTS);
				pop(pInfo.s_dwSkillCode);
				g_pDataMng->GetPostLoad()->GetTBuffList()->push_back(pInfo);
			}
		}

		OutputDebugStringA("[RECV_INIT] after Tamer Buffs\n");

#ifdef BATTLE_MATCH
		u4 nPoint = 0;
		u4 nWin = 0;
		u4 nLose = 0;
		u4 nDraw = 0;
		u4 nTotal = 0;

		pop(nPoint);
		pop(nWin);
		pop(nLose);
		pop(nDraw);
		pop(nTotal);
#endif

		OutputDebugStringA("[RECV_INIT] before TacticsOpenSlot\n");

		n1 nTacticsOpenSlot = 2;
		pop(nTacticsOpenSlot);
		g_pDataMng->GetTactics()->_SetOpenSlot(nTacticsOpenSlot - 1);

		if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		{
			u4 nIDX;
			pop(pDigimonData->s_nBattleTeam);
			pop(nIDX);
		}

		OutputDebugStringA("[RECV_INIT] before Partner Digimon\n");

		pop(pDigimonData->s_Type);

		pop(szName);
		_tcscpy_s(pDigimonData->s_szName, Language::pLength::name + 1, LanConvertT(szName));

		u1 nPartnerHatchLevel;
		pop(nPartnerHatchLevel);
		pDigimonData->s_HatchLevel = nPartnerHatchLevel;

		u2 nScale;
		pop(nScale);
		pDigimonData->s_fScale = nScale * 0.0001f;

		pop(pDigimonData->s_nExp);

#ifdef COMPAT_487
		u8 ExpPt2 = 0;
		pop(ExpPt2);
#endif

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		pop(pDigimonData->s_nTranscendenceExp);
#endif

		pDigimonData->s_nExp = pDigimonData->s_nExp / 100;
		pop(pDigimonData->s_nLevel);

		pop(pDigimonData->s_Attribute, sizeof(pDigimonData->s_Attribute));

		pop(pDigimonData->s_dwBaseDigimonID);

		pop(pDigimonData->s_nMaxEvoUnit);

		{
			char log[256];
			sprintf_s(log, "[RECV_INIT] Partner EvoUnitCount=%d\n", pDigimonData->s_nMaxEvoUnit);
			OutputDebugStringA(log);
		}

		{
			const int nRecvEvoUnitCount = static_cast<int>(pDigimonData->s_nMaxEvoUnit);
			const int nMaxSafeEvoUnitCount = nLimit::EvoUnit - 1;
			const int nSafeEvoUnitCount =
				(nRecvEvoUnitCount > nMaxSafeEvoUnitCount) ? nMaxSafeEvoUnitCount : nRecvEvoUnitCount;

			if (nSafeEvoUnitCount > 0)
			{
				pop(&pDigimonData->s_EvoUnit[1], sizeof(cEvoUnit) * nSafeEvoUnitCount);
			}

			for (int i = nSafeEvoUnitCount; i < nRecvEvoUnitCount; ++i)
			{
				cEvoUnit kDummyEvoUnit;
				memset(&kDummyEvoUnit, 0, sizeof(cEvoUnit));

				pop(&kDummyEvoUnit, sizeof(cEvoUnit));
			}

			pDigimonData->s_nMaxEvoUnit = nSafeEvoUnitCount;
		}

		pop(pDigimonData->s_nEnchantLevel);

		pop(pDigimonData->s_ExtendAttribute, sizeof(pDigimonData->s_ExtendAttribute));
		pop(pDigimonData->s_ExtendAttributeLV, sizeof(pDigimonData->s_ExtendAttributeLV));

		OutputDebugStringA("[RECV_INIT] before Partner Buffs\n");

		pop(nBuffCount);

		{
			char log[256];
			sprintf_s(log, "[RECV_INIT] Partner BuffCount=%d\n", nBuffCount);
			OutputDebugStringA(log);
		}

		if (nBuffCount > 1000)
		{
			OutputDebugStringA("[RECV_INIT][WARN] Partner BuffCount > 1000, forcing 0\n");
			nBuffCount = 0;
		}

		if (nBuffCount != 0)
		{
			for (int i = 0; i < nBuffCount; i++)
			{
				pop(pInfo.s_nBuffCode);

#ifndef COMPAT_487
				pop(pInfo.s_nBuffClassLevel);
#endif
				pop(pInfo.s_nBuffEndTS);
				pop(pInfo.s_dwSkillCode);

				g_pDataMng->GetPostLoad()->GetDBuffList()->push_back(pInfo);
			}
		}

		OutputDebugStringA("[RECV_INIT] after Partner Buffs\n");

#ifdef BATTLE_MATCH
		u1 nBattleGrade;
		u4 nBattlePoint;
		u4 nBattleScore[eBattleMatchScore::eEnd] = { 0, };

		pop(nBattleGrade);
		pop(nBattlePoint);
		pop(nBattleScore[eBattleMatchScore::MatchWin]);
		pop(nBattleScore[eBattleMatchScore::MatchLose]);
		pop(nBattleScore[eBattleMatchScore::MatchDraw]);
		pop(nBattleScore[eBattleMatchScore::MatchTotal]);
#endif

		OutputDebugStringA("[RECV_INIT] before Partner AttributeExp\n");

		for (int i = 0; i < NewAttribute::MaxDigitalType; i++)
		{
			n2 AttributeExp;
			pop(AttributeExp);
			pDigimonData->s_AttributeExp[i] = AttributeExp;
		}

		for (int i = 0; i < NewAttribute::MaxNatualType; i++)
		{
			n2 NatureExp;
			pop(NatureExp);
			pDigimonData->s_NatureExp[i] = NatureExp;
		}

		OutputDebugStringA("[RECV_INIT] after Partner AttributeExp\n");
		OutputDebugStringA("[RECV_INIT] before Partner CashSkill\n");

		for (int i = 0; i < nLimit::EvoUnit; ++i)
		{
			pDigimonData->s_DCashSkill[i].s_nDigimonEvoStatus = 0;

			for (int j = 0; j < nLimit::MAX_ItemSkillDigimon; ++j)
			{
				pDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] = 0;
				pDigimonData->s_DCashSkill[i].s_nSkillCoolTime[j] = 0;
			}
		}

		u1 nDSkillCnt;

		pop(pDigimonData->s_nUID);
		pop(nDSkillCnt);

		{
			char log[256];
			sprintf_s(log, "[RECV_INIT] Partner DSkillCnt=%d\n", nDSkillCnt);
			OutputDebugStringA(log);
		}

		{
			const int nRecvDSkillCount = static_cast<int>(nDSkillCnt);
			const int nSafeDSkillCount =
				(nRecvDSkillCount > nLimit::EvoUnit) ? nLimit::EvoUnit : nRecvDSkillCount;

			for (int i = 0; i < nSafeDSkillCount; ++i)
			{
				pop(pDigimonData->s_DCashSkill[i].s_nDigimonEvoStatus);
				pop(pDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
				pop(pDigimonData->s_DCashSkill[i].s_nSkillCoolTime, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
			}

			for (int i = nSafeDSkillCount; i < nRecvDSkillCount; ++i)
			{
				u1 nDummyEvoStatus = 0;
				u4 nDummyCashSkillCode[nLimit::MAX_ItemSkillDigimon] = { 0, };
				u4 nDummySkillCoolTime[nLimit::MAX_ItemSkillDigimon] = { 0, };

				pop(nDummyEvoStatus);
				pop(nDummyCashSkillCode, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
				pop(nDummySkillCoolTime, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
			}
		}

		OutputDebugStringA("[RECV_INIT] after Partner CashSkill\n");
		OutputDebugStringA("[RECV_INIT] before Active Digimons\n");

		u1 slot;
		pop(slot);

		{
			char log[256];
			sprintf_s(log, "[RECV_INIT] First active digimon slot marker=%d\n", slot);
			OutputDebugStringA(log);
		}

		cData_Tactics* pTacticsData = g_pDataMng->GetTactics();

		int nActiveDigimonSafeCounter = 0;

		while (slot != 99)
		{
			if (++nActiveDigimonSafeCounter > 20)
			{
				OutputDebugStringA("[RECV_INIT][ERROR] Active digimon loop exceeded 20 entries. Breaking to avoid bad_alloc.\n");
				break;
			}

			if (slot == 0)
			{
				OutputDebugStringA("[RECV_INIT][WARN] Active digimon slot 0 received. This may be invalid. Continuing with slot-1 protection.\n");
			}

			cData_PostLoad::sDATA* pTactics = pTacticsData->GetTactics(slot - 1);

			pop(pTactics->s_Type);

			pop(szName);
			_tcscpy_s(pTactics->s_szName, Language::pLength::name + 1, LanConvertT(szName));

			u1 nTacticsHatchLevel;
			pop(nTacticsHatchLevel);
			pTactics->s_HatchLevel = nTacticsHatchLevel;

#ifndef _GIVE
			TCHAR msg[222];
			swprintf_s(msg, _T("%s : %d"), pTactics->s_szName, nTacticsHatchLevel);
			ContentsStream kStream;
			wstring str = msg;
			bool bParcing = true;
			bool bCheckSameString = false;
			kStream << str << bParcing << bCheckSameString;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream);
#endif

			pop(nScale);
			pTactics->s_fScale = nScale * 0.0001f;

			pop(pTactics->s_nExp);

#ifdef COMPAT_487
			ExpPt2 = 0;
			pop(ExpPt2);
#endif

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
			pop(pTactics->s_nTranscendenceExp);
#endif

			pTactics->s_nExp = pTactics->s_nExp / 100;

			pop(pTactics->s_nLevel);
			pop(pTactics->s_Attribute, sizeof(pTactics->s_Attribute));

			pop(pTactics->s_dwBaseDigimonID);

			pop(pTactics->s_nMaxEvoUnit);

			{
				const int nRecvEvoUnitCount = static_cast<int>(pTactics->s_nMaxEvoUnit);
				const int nMaxSafeEvoUnitCount = nLimit::EvoUnit - 1;
				const int nSafeEvoUnitCount =
					(nRecvEvoUnitCount > nMaxSafeEvoUnitCount) ? nMaxSafeEvoUnitCount : nRecvEvoUnitCount;

				if (nSafeEvoUnitCount > 0)
				{
					pop(&pTactics->s_EvoUnit[1], sizeof(cEvoUnit) * nSafeEvoUnitCount);
				}

				for (int i = nSafeEvoUnitCount; i < nRecvEvoUnitCount; ++i)
				{
					cEvoUnit kDummyEvoUnit;
					memset(&kDummyEvoUnit, 0, sizeof(cEvoUnit));

					pop(&kDummyEvoUnit, sizeof(cEvoUnit));
				}

				pTactics->s_nMaxEvoUnit = nSafeEvoUnitCount;
			}

			pop(pTactics->s_nEnchantLevel);
			pop(pTactics->s_ExtendAttribute, sizeof(pTactics->s_ExtendAttribute));
			pop(pTactics->s_ExtendAttributeLV, sizeof(pTactics->s_ExtendAttributeLV));

#ifdef BATTLE_MATCH
			u1 nBattleGrade;
			u4 nBattlePoint;
			u4 nBattleScore[eBattleMatchScore::eEnd] = { 0, };

			pop(nBattleGrade);
			pop(nBattlePoint);
			pop(nBattleScore[eBattleMatchScore::MatchWin]);
			pop(nBattleScore[eBattleMatchScore::MatchLose]);
			pop(nBattleScore[eBattleMatchScore::MatchDraw]);
			pop(nBattleScore[eBattleMatchScore::MatchTotal]);
#endif

			for (int i = 0; i < NewAttribute::MaxDigitalType; i++)
			{
				n2 AttributeExp;
				pop(AttributeExp);
				pTactics->s_AttributeExp[i] = AttributeExp;
			}

			for (int i = 0; i < NewAttribute::MaxNatualType; i++)
			{
				n2 NatureExp;
				pop(NatureExp);
				pTactics->s_NatureExp[i] = NatureExp;
			}

			for (int i = 0; i < nLimit::EvoUnit; ++i)
			{
				pTactics->s_DCashSkill[i].s_nDigimonEvoStatus = 0;

				for (int j = 0; j < nLimit::MAX_ItemSkillDigimon; ++j)
				{
					pTactics->s_DCashSkill[i].s_nDigimonCashSkillCode[j] = 0;
					pTactics->s_DCashSkill[i].s_nSkillCoolTime[j] = 0;
				}
			}

			u1 nTacticsDSkillCnt;

			pop(pTactics->s_nUID);
			pop(nTacticsDSkillCnt);

			{
				const int nRecvDSkillCount = static_cast<int>(nTacticsDSkillCnt);
				const int nSafeDSkillCount =
					(nRecvDSkillCount > nLimit::EvoUnit) ? nLimit::EvoUnit : nRecvDSkillCount;

				for (int i = 0; i < nSafeDSkillCount; ++i)
				{
					pop(pTactics->s_DCashSkill[i].s_nDigimonEvoStatus);
					pop(pTactics->s_DCashSkill[i].s_nDigimonCashSkillCode, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
					pop(pTactics->s_DCashSkill[i].s_nSkillCoolTime, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
				}

				for (int i = nSafeDSkillCount; i < nRecvDSkillCount; ++i)
				{
					u1 nDummyEvoStatus = 0;
					u4 nDummyCashSkillCode[nLimit::MAX_ItemSkillDigimon] = { 0, };
					u4 nDummySkillCoolTime[nLimit::MAX_ItemSkillDigimon] = { 0, };

					pop(nDummyEvoStatus);
					pop(nDummyCashSkillCode, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
					pop(nDummySkillCoolTime, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
				}
			}

			pop(slot);

			{
				char log[256];
				sprintf_s(log, "[RECV_INIT] Next active digimon slot marker=%d\n", slot);
				OutputDebugStringA(log);
			}
		}

		OutputDebugStringA("[RECV_INIT] after Active Digimons\n");

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
		u4 nPartsEffectType = 0;
		pop(nPartsEffectType);
		pPostLoad->AddPartsInfo(pDigimonData->s_Type.m_nType, nPartsEffectType);
#endif

#ifdef CROSSWARS_SYSTEM
		OutputDebugStringA("[RECV_INIT] before CrossWars block\n");

		cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();

		u1 slot2;
		pop(slot2);

		int nCrossSafeCounter = 0;

		while (slot2 != 99)
		{
			if (++nCrossSafeCounter > 20)
			{
				OutputDebugStringA("[RECV_INIT][ERROR] CrossWars loop exceeded 20 entries. Breaking.\n");
				break;
			}

			cData_CrossTactics::sCross* pTactics = pCrossTactics->GetTactics(slot2);

			pop(pTactics->s_nEnable);
			pop(pTactics->s_Type);

			pop(szName);
			_tcscpy_s(pTactics->s_szName, Language::pLength::name + 1, LanConvertT(szName));

			u1 nTacticsHatchLevel;
			pop(nTacticsHatchLevel);
			pTactics->s_HatchLevel = nTacticsHatchLevel;

#ifndef _GIVE
			TCHAR msg[222];
			swprintf_s(msg, _T("%s : %d"), pTactics->s_szName, nTacticsHatchLevel);
			ContentsStream kStream;
			wstring str = msg;
			bool bParcing = true;
			bool bCheckSameString = false;
			kStream << str << bParcing << bCheckSameString;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream);
#endif

			pop(nScale);
			pTactics->s_fScale = nScale * 0.0001f;

			pop(pTactics->s_nExp);

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
			pop(pTactics->s_nTranscendenceExp);
#endif

			pTactics->s_nExp = pTactics->s_nExp / 100;

			pop(pTactics->s_nLevel);
			pop(pTactics->s_Attribute, sizeof(pTactics->s_Attribute));

			pop(pTactics->s_dwBaseDigimonID);

			pop(pTactics->s_nMaxEvoUnit);
			pop(&pTactics->s_EvoUnit[1], sizeof(cEvoUnit) * pTactics->s_nMaxEvoUnit);

			pop(pTactics->s_nEnchantLevel);
			pop(pTactics->s_ExtendAttribute, sizeof(pTactics->s_ExtendAttribute));
			pop(pTactics->s_ExtendAttributeLV, sizeof(pTactics->s_ExtendAttributeLV));

#ifdef BATTLE_MATCH
			u1 nBattleGrade;
			u4 nBattlePoint;
			u4 nBattleScore[eBattleMatchScore::eEnd] = { 0, };

			pop(nBattleGrade);
			pop(nBattlePoint);
			pop(nBattleScore[eBattleMatchScore::MatchWin]);
			pop(nBattleScore[eBattleMatchScore::MatchLose]);
			pop(nBattleScore[eBattleMatchScore::MatchDraw]);
			pop(nBattleScore[eBattleMatchScore::MatchTotal]);
#endif

			for (int i = 0; i < NewAttribute::MaxDigitalType; i++)
			{
				n2 AttributeExp;
				pop(AttributeExp);
				pTactics->s_AttributeExp[i] = AttributeExp;
			}

			for (int i = 0; i < NewAttribute::MaxNatualType; i++)
			{
				n2 NatureExp;
				pop(NatureExp);
				pTactics->s_NatureExp[i] = NatureExp;
			}

			pop(slot2);
		}

		OutputDebugStringA("[RECV_INIT] after CrossWars block\n");
#endif

#ifdef COMPAT_487
		int IntBeforeChannel = 0;
		pop(IntBeforeChannel);
#endif

		OutputDebugStringA("[RECV_INIT] before CurrentChannel\n");

		GS2C_RECV_CURRENT_CHANNEL CurrentChannel;
		pop(CurrentChannel.channel_idx);
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_CURRENT_CHANNELINDEX, &CurrentChannel.channel_idx);

		OutputDebugStringA("[RECV_INIT] after CurrentChannel\n");
		OutputDebugStringA("[RECV_INIT] before MapRegion\n");

		g_pDataMng->GetMapRegion()->ToBackup();
		pop(*g_pDataMng->GetMapRegion()->GetMapRegion());

		if (g_pDataMng->GetMapRegion()->IsFirstLoad() == true)
		{
			g_pDataMng->GetMapRegion()->ToBackup();
			g_pDataMng->GetMapRegion()->FirstLoadComp();
		}

		OutputDebugStringA("[RECV_INIT] after MapRegion\n");
		OutputDebugStringA("[RECV_INIT] before Archive\n");

		GS2C_RECV_EXTEND_ARCHIVE kRecvArchiveData;
		int iOpenedSlotCnt;
		pop(iOpenedSlotCnt);

		kRecvArchiveData.SetExtendArchiveCnt(iOpenedSlotCnt);
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ARCHIVE_OPENCNT, &kRecvArchiveData);

		OutputDebugStringA("[RECV_INIT] after Archive\n");
		OutputDebugStringA("[RECV_INIT] before PartyInfo\n");

		GS2C_RECV_PARTY_INFO recvPartyInfo;
		pop(recvPartyInfo.m_nPartyIDX);

		u4 nPartyIDX = recvPartyInfo.m_nPartyIDX;

		u4 nCropType;
		pop(nCropType);
		recvPartyInfo.m_lootingInfo.m_nCropType = nCropType;
		pop(recvPartyInfo.m_lootingInfo.m_nRareRate);
		pop(recvPartyInfo.m_lootingInfo.m_nDispRareGrade);

		u1 nMasterSlotNo;
		pop(nMasterSlotNo);
		recvPartyInfo.m_nMasterSlotNo = nMasterSlotNo;

#ifdef COMPAT_487
		u2 shortBeforeSlot = 0;
		pop(shortBeforeSlot);
#endif

		u1 nSlotNo;
		pop(nSlotNo);

		int nPartySafeCounter = 0;

		while (nSlotNo < 99)
		{
			if (++nPartySafeCounter > 8)
			{
				OutputDebugStringA("[RECV_INIT][ERROR] Party loop exceeded 8 entries. Breaking.\n");
				break;
			}

			TPartyMember addPartyMember;
			addPartyMember.m_nSlotNo = nSlotNo;
			pop(addPartyMember.m_TamerInfo.m_nUID);
			pop(addPartyMember.m_DigimonInfo.m_nUID);

			pop(addPartyMember.m_TamerInfo.m_nType);
			pop(addPartyMember.m_TamerInfo.m_nLevel);
			pop(addPartyMember.m_TamerInfo.m_Name);

			if (0 == addPartyMember.m_TamerInfo.m_Name.compare(pTamerData->s_szName))
				recvPartyInfo.m_nMySlotNo = nSlotNo;

			pop(addPartyMember.m_DigimonInfo.m_nType);
			pop(addPartyMember.m_DigimonInfo.m_nLevel);
			pop(addPartyMember.m_DigimonInfo.m_Name);

			pop(addPartyMember.m_nMapNo);
			pop(addPartyMember.m_nChannelNo);

			recvPartyInfo.m_listMemberInfo.push_back(addPartyMember);
			pop(nSlotNo);
		}

		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_PARTY_INFO_INIT, &recvPartyInfo);

		OutputDebugStringA("[RECV_INIT] after PartyInfo\n");
		OutputDebugStringA("[RECV_INIT] before Achievement/Cooldown\n");

		u2 nAchievement;
		pop(nAchievement);

		GS2C_RECV_DISPTITLE recvTitle;
		recvTitle.dispTitle = nAchievement;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_DISPTITLE, &recvTitle);

		u4 nCoolDownEndTimeTS[nLimit::CoolDown];
		pop(nCoolDownEndTimeTS);

		for (int i = 0; i < nLimit::CoolDown; ++i)
		{
			if (nsCsFileTable::g_pItemMng->IsCoolTime(i + 1) == false)
				continue;

			CsCoolTimeSeq* pSeq = nsCsFileTable::g_pItemMng->GetCoolTime(i + 1)->GetSeq();

			if (nCoolDownEndTimeTS[i] < _TIME_TS + 1)
			{
				pSeq->Reset();
				continue;
			}

			double fOffset = pSeq->GetEndTime() - (nCoolDownEndTimeTS[i] - _TIME_TS) - 1;

			if (fOffset < 0)
				fOffset = 0;

			pSeq->Start(fOffset);
		}

		u4 nOption;
		pop(nOption);
		pPostLoad->SetSyncOption(nOption);

		n4 nWorkDayHistory;
		pop(nWorkDayHistory);

		u4 nTodayAttendanceTimeTS;
		pop(nTodayAttendanceTimeTS);

#ifndef REWARD_SYSTEM_UI
		g_pDataMng->GetAttendance()->SetWorkDayHistory(nWorkDayHistory);
		g_pDataMng->GetAttendance()->SetTodayAttendanceTimeTS(nTodayAttendanceTimeTS);
#endif

		OutputDebugStringA("[RECV_INIT] before BossGenInfo\n");

		RecvBossGenInfo();

		OutputDebugStringA("[RECV_INIT] after BossGenInfo\n");

		bool bIsPCbang = false;
		uint nCostumeNo = 0;
		pop(bIsPCbang);

		g_pDataMng->GetPCbang()->SetPcBang(bIsPCbang);
		g_pDataMng->GetPCbang()->SetCostume(0);

		if (bIsPCbang == true)
		{
			pop(nCostumeNo);
			g_pDataMng->GetPCbang()->SetCostume(nCostumeNo);
		}

		if (nPartyIDX > 0)
		{
			uint nPCBangExpBuffRate = 0;
			uint nPCBangMemberCount = 0;
			pop(nPCBangMemberCount);
			pop(nPCBangExpBuffRate);
		}

		OutputDebugStringA("[RECV_INIT] before ShopData\n");

		uint nShopData[5] = { 0, };

		pop(nShopData[0]);

		if (nShopData[0] != 0)
		{
			pop(nShopData[1]);
			pop(nShopData[2]);
			pop(nShopData[3]);
			pop(nShopData[4]);

			GS2C_RECV_SHOP_DATA pRecv;
			pRecv.iCharMapID = nShopData[0];
			pRecv.iCharChannel = nShopData[1];
			pRecv.iCharModelID = nShopData[4];
			pRecv.iCharPos = NiPoint2((float)nShopData[2], (float)nShopData[3]);
			GAME_EVENT_ST.OnEvent(EVENT_CODE::UNION_SET_SHOPDATA, &pRecv);
		}
		else
		{
			GAME_EVENT_ST.OnEvent(EVENT_CODE::UNION_RELEASE_DATA, NULL);
		}

		OutputDebugStringA("[RECV_INIT] after ShopData\n");

		u4 nClientOption = 0;
		pop(nClientOption);
		pPostLoad->SetServerOption(nClientOption);

		u4 nRank = 0;
		pop(nRank);

		GS2C_RECV_RANKACHIEVE recvRank;
		recvRank.rankAchieve = nRank;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_RANKACHIEVE, &recvRank);

#ifdef PLAY_PANELTY
		u2 nPaneltyType = 0;
		u4 CashTime = 0;
		u4 NormalTime = 0;
		u4 nBasePenaltyTime = 0;
		u4 nBaseFullTime = 0;

		pop(nBaseFullTime);
		pop(nBasePenaltyTime);
		pop(nPaneltyType);
		pop(CashTime);
		pop(NormalTime);

		g_pDataMng->PlayTime_Set_PaneltyType(nPaneltyType);
		g_pDataMng->PlayTime_Set_BaseFullTime(nBaseFullTime);
		g_pDataMng->PlayTime_Set_BasePaneltyTime(nBasePenaltyTime);
		g_pDataMng->PlayTime_Set_CashTime(CashTime);
		g_pDataMng->PlayTime_Set_NormalTime(NormalTime);
#endif

#ifdef BATTLE_MATCH
		u4 nBattleCoin = 0;
		u4 nBattleTamerScore[eBattleMatchScore::eEnd] = { 0, };
		pop(nBattleCoin);
		pop(nBattleTamerScore[eBattleMatchScore::MatchWin]);
		pop(nBattleTamerScore[eBattleMatchScore::MatchLose]);
		pop(nBattleTamerScore[eBattleMatchScore::MatchDraw]);
		pop(nBattleTamerScore[eBattleMatchScore::MatchTotal]);
#endif

#ifdef MINIGAME
		u1 nIsgame;
		u2 nSuccessCnt;
		pop(nIsgame);

		bool nResult = false;

		if (nIsgame == 1)
			nResult = true;
		else
			nResult = false;

		pop(nSuccessCnt);

		GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT kRecv;
		kRecv.nResult = nResult;
		kRecv.nSuccessCnt = nSuccessCnt;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_MINIGAME_INIT, &kRecv);
#endif

		OutputDebugStringA("[RECV_INIT] before Tamer Active Skills\n");

		u1 nSkill_Count;
		pop(nSkill_Count);

		{
			char log[256];
			sprintf_s(log, "[RECV_INIT] Tamer NormalSkillCount=%d\n", nSkill_Count);
			OutputDebugStringA(log);
		}

		if (nSkill_Count > 5)
		{
			OutputDebugStringA("[RECV_INIT][WARN] Tamer NormalSkillCount > 5. Forcing 5.\n");
			nSkill_Count = 5;
		}

		for (int i = 0; i < nSkill_Count; ++i)
		{
			pop(pTamerData->s_nTamerSkillCode[i]);
			pop(pTamerData->s_nTamerSkillCoolTime[i]);
		}

		u1 nCashCount;
		pop(nCashCount);

		{
			char log[256];
			sprintf_s(log, "[RECV_INIT] Tamer CashSkillCount=%d\n", nCashCount);
			OutputDebugStringA(log);
		}

		if (nCashCount > 2)
		{
			OutputDebugStringA("[RECV_INIT][WARN] Tamer CashSkillCount > 2. Forcing 2.\n");
			nCashCount = 2;
		}

		for (int i = 0; i < nCashCount; ++i)
		{
			pop(pTamerData->s_nTamerCashSkill_Index[i]);
			pop(pTamerData->s_nTamerCashSkill_LifeTime[i]);
			pop(pTamerData->s_nTamerCashSkill_CoolTime[i]);
		}

		OutputDebugStringA("[RECV_INIT] after Tamer Active Skills\n");

#ifdef CHAT_BAN
		bool bIsChatBan = true;

		pop(bIsChatBan);

		if (bIsChatBan)
		{
			u4 nBanSecond = 0;
			pop(nBanSecond);

			std::wstring banMsg = UISTRING_TEXT("CHAT_LIMIT_MESSAGE");
			std::wstring wsHour;
			DmCS::StringFn::From(wsHour, nBanSecond / 3600);
			DmCS::StringFn::Replace(banMsg, L"#Hour#", wsHour);

			std::wstring wsMin;
			DmCS::StringFn::From(wsMin, (nBanSecond / 60) % 60);
			DmCS::StringFn::Replace(banMsg, L"#Min#", wsMin);

			std::wstring wsSec;
			DmCS::StringFn::From(wsSec, nBanSecond % 60);
			DmCS::StringFn::Replace(banMsg, L"#Sec#", wsSec);

			CsMessageBox(MB_OK, banMsg.c_str());

			ST_CHAT_PROTOCOL CProtocol;
			CProtocol.m_Type = NS_CHAT::SET_BANTIME;
			CProtocol.m_value = nBanSecond;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		}
#endif

#ifdef MASTERS_MATCHING
		u1 nMyTeam = 0;
		pop(nMyTeam);
		g_pGameIF->m_sMyInfo.s_eMyTeam = (eTEAM)nMyTeam;
#endif

#ifndef UI_INVENTORY_RENEWAL
		g_pDataMng->GetInven()->LoadApply();
#endif

		if (true == g_pGameIF->IsActiveWindow(cBaseWindow::WT_CARDINVENTORY))
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_CARDINVENTORY);

		OutputDebugStringA("[RECV_INIT] before DeckBuff\n");

		u4 nDeck = 0;
		pop(nDeck);

		GS2C_RECV_ENCYCLOPEDIA_USEDECK ency_recv;

		if (nDeck != 0)
		{
			ency_recv.iDeckIdx = nDeck;
			ency_recv.bUse = true;
		}
		else
		{
			ency_recv.iDeckIdx = 0;
			ency_recv.bUse = false;
		}

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_USE_DECK, &ency_recv);

#ifdef MEGAPHONE_BAN
		u1 nMegaphone = 0;
		pop(nMegaphone);

		pTamerData->s_bMegaPhoneBan = (nMegaphone == 0) ? false : true;
#endif

		OutputDebugStringA("[RECV_INIT] before RecvActorObjectFactor_Init\n");

		try
		{
			RecvActorObjectFactor_Init();
		}
		catch (...)
		{
			OutputDebugStringA("[RECV_INIT][WARN] RecvActorObjectFactor_Init exception ignored\n");
		}

		OutputDebugStringA("[RECV_INIT] after RecvActorObjectFactor_Init\n");

		/*
			IMPORTANTE:
			Não trocar para InGame diretamente aqui.

			RecvInitGameData corre no contexto do network/packet. A criação do mundo,
			PostLoad, recursos/render e a troca de Flow têm de acontecer no main loop.

			Por isso só marcamos a flag. O LoadingFlow::UpdateFrame() vai consumir esta
			flag e disparar EVENT_CODE::RECV_PLAYER_DATA_LOAD_COMPLETE no main thread.
			O LoadingContents já está registado nesse evento e faz a transição correta
			para FLW_MAINGAME.
		*/
		net::bInitGameDataReady = true;

		OutputDebugStringA("[RECV_INIT] InitGameData ready flag set. Waiting LoadingFlow main-thread event dispatch.\n");
		OutputDebugStringA("[RECV_INIT] RecvInitGameData end OK\n");
	}
	catch (const std::bad_alloc&)
	{
		OutputDebugStringA("[RECV_INIT][ERROR] std::bad_alloc inside RecvInitGameData\n");
		CsMessageBox(MB_OK, _ONLY_ENG("RecvInitGameData failed: std::bad_alloc. Packet may be misaligned."));
		return;
	}
	catch (...)
	{
		OutputDebugStringA("[RECV_INIT][ERROR] unknown exception inside RecvInitGameData\n");
		CsMessageBox(MB_OK, _ONLY_ENG("RecvInitGameData failed: unknown exception. Packet may be misaligned."));
		return;
	}
}

void cCliGame::RecvSetMoney(void)
{
	n8 nMoney;
	pop(nMoney);

	cData_Inven* pInven = g_pDataMng->GetInven();
	pInven->SetMoney(nMoney, false);
}

void cCliGame::RecvUserMessage(void)
{
	GS2C_NTF_USER_MESSAGE recv;
	pop(recv.m_szSenderName);
	pop(recv.m_nSendTimeTS);
	pop(recv.m_szMessage);
	recv.m_nMailType = mail::eAccount;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_USER_MESSAGE, &recv);

	// 	cData_Mail::sMAILINFO m_pInfo;
	// 	m_pInfo.s_nSendTimeTS = nSendTimeTS;
	// 	_tcscpy_s( m_pInfo.s_szSenderName, szSenderName.c_str() );
	// 	_tcscpy_s( m_pInfo.s_szMessage, szMessage.c_str() );
	// 	g_pDataMng->GetMail()->SetMailInfo( &m_pInfo, cData_Mail::MAIL_ACCOUNT );
	// 
	// 	g_pGameIF->GetAlime()->SetAlime( 310, cAlime::MAIL, 0 );
	// 	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIL_BUTTON ) == false )
	// 	{
	// 		g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAIL_BUTTON );
	// 	}
}

void cCliGame::RecvTamerMessage(void)
{
	GS2C_NTF_USER_MESSAGE recv;
	pop(recv.m_szSenderName);
	pop(recv.m_nSendTimeTS);
	pop(recv.m_szMessage);
	recv.m_nMailType = mail::eTamer;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_USER_MESSAGE, &recv);

	// 	std::wstring szSenderName;
	// 	std::wstring szMessage;
	// 	uint nSendTimeTS;
	// 	pop(szSenderName);
	// 	pop(nSendTimeTS);
	// 	pop(szMessage);
	// 
	// 	if( szMessage[0] == '`' && szMessage[1] == '1' )
	// 	{
	// 		if( net::game )
	// 			net::game->SendTamerMessageConfirm();
	// 		g_pGameIF->GetAlime()->SetAlime( 310, cAlime::EVENT, 0 );
	// 		return;
	// 	}
	// 
	// 	cData_Mail::sMAILINFO m_pInfo;
	// 	m_pInfo.s_nSendTimeTS = nSendTimeTS;
	// 	_tcscpy_s( m_pInfo.s_szSenderName, szSenderName.c_str() );
	// 	_tcscpy_s( m_pInfo.s_szMessage, szMessage.c_str() );
	// 	g_pDataMng->GetMail()->SetMailInfo( &m_pInfo, cData_Mail::MAIL_TAMER );
	// 
	// 	g_pGameIF->GetAlime()->SetAlime( 310, cAlime::MAIL, 0 );
	// 	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAIL_BUTTON ) == false )
	// 		g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAIL_BUTTON );

}

void cCliGame::RecvBossGenInfo(void)
{
	OutputDebugStringA("[BOSS_GEN] RecvBossGenInfo begin\n");

	cBossScene* pBossScene = NULL;

	if (g_pGameIF)
	{
		pBossScene = g_pGameIF->GetBossScene();

		if (pBossScene)
			pBossScene->PreResetMap();
	}

	n4 nBossMonsterType = 0;
	n1 nBossMonsterCount = 0;
	n4 nSeedMonsterType = 0;
	n1 nSeedMonsterCount = 0;

	pop(nBossMonsterType);

	{
		char log[256];
		sprintf_s(log, "[BOSS_GEN] first BossMonsterType=%d\n", nBossMonsterType);
		OutputDebugStringA(log);
	}

	int nSafeCounter = 0;

	while (nBossMonsterType != 0)
	{
		if (++nSafeCounter > 32)
		{
			OutputDebugStringA("[BOSS_GEN][ERROR] BossGenInfo loop exceeded 32 entries. Breaking to avoid loading lock.\n");
			break;
		}

		pop(nBossMonsterCount);

		{
			char log[256];
			sprintf_s(
				log,
				"[BOSS_GEN] BossMonsterType=%d BossMonsterCount=%d\n",
				nBossMonsterType,
				nBossMonsterCount
			);
			OutputDebugStringA(log);
		}

		if (nBossMonsterCount != 0)
		{
			if (pBossScene)
			{
				pBossScene->SetBossInfo(
					nBossMonsterType,
					nBossMonsterCount,
					0,
					0
				);
			}
		}
		else
		{
			pop(nSeedMonsterType);

			{
				char log[256];
				sprintf_s(
					log,
					"[BOSS_GEN] SeedMonsterType=%d\n",
					nSeedMonsterType
				);
				OutputDebugStringA(log);
			}

			if (nSeedMonsterType != 0)
			{
				pop(nSeedMonsterCount);

				{
					char log[256];
					sprintf_s(
						log,
						"[BOSS_GEN] SeedMonsterCount=%d\n",
						nSeedMonsterCount
					);
					OutputDebugStringA(log);
				}

				if (pBossScene)
				{
					pBossScene->SetBossInfo(
						nBossMonsterType,
						0,
						nSeedMonsterType,
						nSeedMonsterCount
					);
				}
			}
		}

		pop(nBossMonsterType);

		{
			char log[256];
			sprintf_s(log, "[BOSS_GEN] next BossMonsterType=%d\n", nBossMonsterType);
			OutputDebugStringA(log);
		}
	}

	OutputDebugStringA("[BOSS_GEN] RecvBossGenInfo end\n");
}

void cCliGame::RecvTamerAchievement(void)
{
	u4 nTamerIDX = 0;
	u2 nAchievement = 0;

	pop(nTamerIDX);
	pop(nAchievement);

	CTamer* pTamer = (CTamer*)g_pCharMng->GetTamer(nTamerIDX);
	if (pTamer == NULL)
		return;
	assert_cs(pTamer->IsKindOf(RTTI_TAMER));

	pTamer->GetCharOption()->SetAchieve(nAchievement);
	if (pTamer->GetLeafRTTI() == RTTI_TAMER_USER)
	{
		GS2C_RECV_DISPTITLE recv;
		recv.dispTitle = nAchievement;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_DISPTITLE, &recv);

	}
}

void cCliGame::RecvTamerOption(void)
{
	u4 nTamerUID = 0;
	u4 nOption = 0;

	pop(nTamerUID);
	pop(nOption);

	CsC_AvObject* pTamer = g_pMngCollector->GetObject(nTamerUID);
	if (pTamer)
	{
		assert(pTamer->IsKindOf(RTTI_TAMER));
		((CTamer*)pTamer)->ApplySyncOption(nOption);
	}
}

void cCliGame::RecvItemAnalysis(void)
{
	u1 nRate;	// 아이템 능력치 적용 비율
	n8 nMoney;	// 분석후 남은 금액

	pop(nRate);
	pop(nMoney);

	SAFE_POINTER_RET(g_pDataMng);
	cData_ServerSync* pSrvSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RET(pSrvSync);
	pSrvSync->RecvServer(cData_ServerSync::ELEMENT_ITEM_CHECK, 0, &nRate, &nMoney);
}

void cCliGame::RecvItemSellSuccess(void)
{
	ST_RECV_ItemSellSuccessInfo SellInfo;

	pop(SellInfo.m_CurrentMoney);

	u1 ChangedInvenCount(0);
	pop(ChangedInvenCount);
	for (int i = 0; i < ChangedInvenCount; ++i)
	{
		ST_RECV_ItemSellSuccessInfo::sInvenInfo InvenInfo;
		pop(InvenInfo.m_nInvenSlotNumber);
		pop(InvenInfo.m_nItemTypeID);
		pop(InvenInfo.m_nInvenItemCount);

		SellInfo.m_InvenInfoList.push_back(InvenInfo);
	}

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_STORE_SELL_SUCCESS, &SellInfo);
}

void cCliGame::RecvItemBuySuccess(void)
{
	ST_RECV_ItemBuySuccessInfo BuyInfo;

	pop(BuyInfo.m_CurrentMoney);

	u1 ChangedInvenCount(0);
	pop(ChangedInvenCount);
	for (int i = 0; i < ChangedInvenCount; ++i)
	{
		ST_RECV_ItemBuySuccessInfo::sInvenInfo InvenInfo;
		pop(InvenInfo.m_nInvenSlotNumber);
		pop(InvenInfo.m_nItemTypeID);
		pop(InvenInfo.m_nInvenItemCount);
		pop(InvenInfo.m_nRate);
		pop(InvenInfo.m_nLimited);

		BuyInfo.m_InvenInfoList.push_back(InvenInfo);
	}

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_STORE_BUY_SUCCESS, &BuyInfo);
}

void cCliGame::RecvItemMake(void)
{
	GS2C_RECV_ITEMPRODUCTION recv;

	pop(recv.m_dwResult);			// 만들어진 아이템 테이블 IDX	

	pop(recv.m_sDwItemIdx);			// 만들어진 아이템 테이블 IDX	
	pop(recv.m_nItemCount);			// 실제 아이템 개수
	pop(recv.m_nTradeLimitTime);		// 아이템 거래 제한 시간
	pop(recv.m_MoneyResult);			// 실제 사용후 보유할 금액
	pop(recv.m_TotalMakedCount);		// 제작 총 갯수.
	pop(recv.m_GetCount);				// 실제 성공한 개수

	n4 nDeleteCount = 0;		// 삭제되는 재료 아이템 개수
	pop(nDeleteCount);

	for (int n = 0; n < nDeleteCount; ++n)
	{
		sItemInfo sDelItemInfo;
		pop(sDelItemInfo.sDwItemIdx);	// 지워질 아이템 테이블 IDX
		pop(sDelItemInfo.nItemCount);	// 실제 삭제된 개수

		recv.m_DelItemInfo.push_back(sDelItemInfo);
	}

	pop(recv.m_nIsItemProtect);		// 보호용 아이템 코드 번호
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ITEM_PRODUCTION_RESULT, &recv);
}

void cCliGame::RecvEmoticon(void)
{
	u4 nUID = 0;
	int nEmoticonType = 0;

	pop(nUID);	// 이모티콘 송신 테이머
	pop(nEmoticonType);
#ifdef TRICK_ORTREAT
	int nEffectNum;
	pop(nEffectNum);
#endif

	CsC_AvObject* pObject = g_pMngCollector->GetObject(nUID);
	if (pObject == NULL)
		return;

	if (nsCsFileTable::g_pTamerMng->IsEmotion(nEmoticonType) == false)
	{
		g_CriticalLog.Log(_T("존재 하지 않는 이모션 타입이 들어왔다. 이모션 타입 = %d"), nEmoticonType);
		return;
	}

	assert(pObject->IsKindOf(RTTI_TAMER));
	DWORD nAni = nsCsFileTable::g_pTamerMng->GetEmotion(nEmoticonType)->GetInfo()->s_nAniID;
#ifdef TRICK_ORTREAT
	if (1 == nEffectNum)
	{
		pObject->GetProp_Effect()->AddEffect(EFFECT_BOOM, pObject->GetCenter(), 1.0f);
	}
#endif
	pObject->SetAnimation(nAni);
}

void cCliGame::RecvDigitamaSellResult(void)
{
	GS2C_RECV_DIGITAMA_TO_MONEY_SUCCESS recv;
	pop(recv.nTotalPrice);
	pop(recv.nTamerMoney);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_SELL_SUCCESS, &recv);
}

void cCliGame::RecvDiedAlready(void)
{
	u2 nProtocol;
	pop(nProtocol);

	g_pDataMng->DiedAlready();
	g_pGameIF->RecvDiedAlready();

#ifndef _GIVE
	LOG("이미 죽어서 처리 불가능입니다. (protocol:%d)", nProtocol);
#endif //_GIVE

}

void cCliGame::RecvVersionError(void)
{
	uint nSvrVersion;
	char msg[255];

	pop(nSvrVersion);
	pop(msg);

	/*char *r = strstr(msg, "\\n");
	while(r)
	{
		r[0] = '\n';
		for(int i=1; i<255; i++)
		{
			if(!r[i])
			{
				break;
			}
			r[i] = r[i+1];
		}

		r = strstr(msg, "\\n");
	}*/

	// 	ReceiveVersionError();

		//assert( false );
#ifdef VERSION_USA
// 	cPrintMsg::PrintMsg( cPrintMsg::SERVER_VERSION_ERROR );
	TCHAR szBody[128];
	M2W(szBody, msg, 128);
	cMessageBox::InsertList(UISTRING_TEXT("MESSAGEBOX_ERROR_MESSAGE").c_str(), szBody, cPrintMsg::USE_MANUAL_MESSAGE, CsTalk_Message::MT_OK);

#else
	cPrintMsg::PrintMsg(cPrintMsg::SERVER_READYING, (void*)nBase::m2w(msg).c_str());
#endif
}

void cCliGame::RecvInsMap_DungeonFail(void)
{
	GS2C_RECV_DUNGEON_FAIL recv;
	pop(recv.nResult);
	cPrintMsg::PrintMsg(recv.nResult);

	// 메시지
	//cPrintMsg::PrintMsg( 30097);
	if (net::game)
		net::game->SetInsMap_DungeonRestoreFlag();
}

void cCliGame::RecvLocalPortalSuccess(void) // 존내 포탈이용시
{
	g_pResist->SetMovePortal(false);	//2017-04-12-nova

	u4 uTamerUID = 0;
	u4 uDigimonUID = 0;
	pop(uTamerUID);
	pop(uDigimonUID);

	nSync::Pos pos;
	pop(pos);

	bool bSendBlock = cClient::IsSendBlock();
	if (bSendBlock)
		cClient::SetSendBlock(false);

	m_bPortalRequesting = false;
	m_bPortalProcessing = false;

	g_pResist->SetEnablePortal(true);
	//CHAT_TEXT_DEGUG(_T("RcvLocalSucs-포탈 상태 %d"), (int)g_pResist->IsEnablePortal());

	//net::receive_active = false;

	// 인터페이스 초기화
	//assert_cs( g_pGameIF );
	//g_pGameIF->PreResetMap();

	net::bChangeChannel = false;
	if (net::IsMoveMapByCommand() == false)
	{
		net::prev_map_no = net::next_map_no;
		net::nIsMoveMapByCommand = true;
		net::bPortalMove = true;
	}
	else
	{

		net::bPortalMove = false;
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAP_NO, &net::next_map_no);

	CsC_AvObject* pTamer = g_pMngCollector->GetObject(uTamerUID);
	CsC_AvObject* pDigimon = g_pMngCollector->GetObject(uDigimonUID);

	if (NULL == pTamer)
		return;
	if (NULL == pDigimon)
		return;

	//CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	//CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser(0);		

	pTamer->SetPauseAll(true);
	pDigimon->SetPauseAll(true);

	pTamer->SetPos(NiPoint2((float)pos.m_nX, (float)pos.m_nY));
	pTamer->GetProp_Path()->ResetAgent();

	pDigimon->SetPos(NiPoint2((float)pos.m_nX, (float)pos.m_nY));
	pDigimon->GetProp_Path()->ResetAgent();

	pTamer->SetPauseAll(false);
	pTamer->SetAnimation(ANI::IDLE_NORMAL);

	pDigimon->SetPauseAll(false);
	pDigimon->SetAnimation(ANI::IDLE_NORMAL);


	pTamer->GetProp_Effect()->AddEffect_FT(EFFECT_CLOSED_SCENE);
	pDigimon->GetProp_Effect()->AddEffect_FT(EFFECT_CLOSED_SCENE);

	// 포탈타는게 자신이면. 패이드 씬.
	if (pTamer->GetIDX() == g_pCharMng->GetTamerUser()->GetIDX())
	{
		// 		SAFE_DELETE( g_pFade );
		// 		g_pFade = NiNew CFade( FADE_IN, 0.8f );	
	}
}

void cCliGame::RecvSelectPortalFailure(void)
{
	GS2C_RECV_SELECT_PORTAL_FAILURE recv;
	pop(recv.nResult);

	// 채널 변경시 실패 가 아니라면
	if (net::bChangeChannel == false)
	{
		// 입장시간 아닌데 들어왔다
		cPrintMsg::PrintMsg(recv.nResult);

		// 		ST_CHAT_PROTOCOL	CProtocol;
		// 		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		// 		CProtocol.m_wStr = _T( "portal failure" );
		// 		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	}
	// 채널 변경 실패 라면
	else
	{
		assert_cs(net::bChangeChannel == true);
		cPrintMsg::PrintMsg(10016);

		GAME_EVENT_ST.OnEvent(EVENT_CODE::CHANGE_CHANNEL_FAIL, null);

		assert_cs(g_pGameIF);
		g_pGameIF->GetPopup()->PreResetMap();
	}

	g_pResist->SetMovePortal(false);	//2017-04-12-nova
	g_pResist->SetEnablePortal(true); // 포탈 사용 다시 활성화
	net::bChangeChannel = false;
	m_bPortalRequesting = false;
	m_bPortalProcessing = false;
	SetSendBlock(false);
}

void cCliGame::RecvSelectPortalSuccess(void)//인맵포탈, 일반포탈이용시
{
	net::receive_active = false;

	// 인터페이스 초기화
	assert_cs(g_pGameIF);
	//g_pGameIF->PreResetMap();

	net::bChangeChannel = false;
	if (net::IsMoveMapByCommand() == false)
	{
		net::prev_map_no = net::next_map_no;
		net::nIsMoveMapByCommand = true;
		net::bPortalMove = true;
	}
	else
	{
		// 제거 이펙트
		//if( g_pLoading == NULL )
		//{
		g_pCharMng->GetTamerUser()->GetProp_Effect()->AddEffect_FT(EFFECT_CLOSED_SCENE);
		for (int i = 0; i < MAX_DIGIMONUSER_COUNT; ++i)
		{
			CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser(i);
			if (pDigimon)
				pDigimon->GetProp_Effect()->AddEffect_FT(EFFECT_CLOSED_SCENE);
		}
		//}		
		net::bPortalMove = false;
	}

	pop(net::ip);
	pop(net::port);
	pop(net::next_map_no);

	nSync::Pos pos;
	pop(pos);
	g_pDataMng->GetPostLoad()->SetPos(pos);
	m_bPortalProcessing = true;

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAP_NO, &net::next_map_no);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::LOADING_START);
}

void cCliGame::RecvChangeServer(void)
{
	net::cmd = Cmd::ConnectGameServer;
	//	net::start(net::type::game, net::ip, net::port);
}

void cCliGame::RecvSocketInSuccess(void)
{
	// 	assert_cs( g_pGameIF );
	// 	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_ATTACH ) );
	// 	g_pGameIF->GetEI_Attach()->RecvServerAttach();
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ELEMENT_ITEM_ATTACH, NULL);
}

void cCliGame::RecvSocketOutSuccess(void)
{
	// 	assert_cs( g_pGameIF );
	// 	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_ATTACH ) );
	// 	g_pGameIF->GetEI_Attach()->RecvServerDetach();
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ELEMENT_ITEM_DETACH, NULL);
}

void cCliGame::RecvSocketClearSuccess(void)
{
	// 	assert_cs( g_pGameIF );
	// 	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_ATTACH ) );
	// 	g_pGameIF->GetEI_Attach()->RecvServerDelete();
}

void cCliGame::RecvCropMoney(void)	// 87번 아이템 습득으로, 즉시 입금된 경우
{
	u4 nCropTamerUID = 0;
	u4 nMoney = 0;
	u4 nGuildAddMoney = 0;
	u4 nBuffAddMoney = 0;

	pop(nCropTamerUID);	// 아이템 실 습득자
	pop(nMoney);	// 해당 머니(bit) 추가 필요 , 기본 획득하는 돈
	pop(nGuildAddMoney);// 길드 버프로 인해 추가된 금액
	pop(nBuffAddMoney);// 일반 버프로 인해 추가된 금액

	// 다른 사람이 먼저 습득한 경우엔
	// pItem::CropItem 과 같이 pItem::CropFailure - pItem::CropF_Late 로 처리된다.	
	if (g_pCharMng->GetTamerUser())
		g_pCharMng->GetTamerUser()->MoneyCrop(nMoney, nCropTamerUID, nGuildAddMoney, nBuffAddMoney);

#ifndef _GIVE
	TCHAR msg[333] = { 0, };
	u4 nTotal = nMoney + nGuildAddMoney + nBuffAddMoney;
	swprintf(msg, _T("Crop Total : %d Bit, Org : %d Bit, Guild : %d Bit, Buff : %d Bit "), nTotal, nMoney, nGuildAddMoney, nBuffAddMoney);
	CHAT(msg);
#endif //_GIVE
}

void cCliGame::RecvCropFailure(void)
{
	uint nErrCode;
	pop(nErrCode);

	switch (nErrCode)
	{
	case pItem::CropF_Late:
		//cPrintMsg::PrintMsg( 30012 );
		break;
	case pItem::CropF_Far:
		cPrintMsg::PrintMsg(30013);
		cDigimonTalk::Print(30013);
		break;
	case pItem::CropF_Right:
		cPrintMsg::PrintMsg(18001);
		cDigimonTalk::Print(18001);
		break;
	case pItem::CropF_Full:
		cPrintMsg::PrintMsg(cPrintMsg::GAME_INVEN_LIMIT_COUNT);
		break;
	default:
		xassert(false, "확인 필요");
	}

	g_pCharMng->GetTamerUser()->ItemCropFailed();
}

void cCliGame::RecvAttack(bool bDie)
{
	u4 nHitterUID = 0;
	u4 nTargetUID = 0;
	n4 nDamage = 0;
	u4 nBattleOption = 0;

	pop(nHitterUID);	// 때린넘
	pop(nTargetUID);	// 맞는넘
	pop(nDamage);
	pop(nBattleOption);

	n4 hp = 0;
	n4 Mhp = 0;

	if (bDie == false)
	{
		pop(hp);
		pop(Mhp);
	}

	AttackProcesses(nHitterUID, nTargetUID, nDamage, nBattleOption, hp, Mhp, bDie);
}

void cCliGame::RecvAttackMiss(void)
{
	u4 nHitterUID = 0;
	u4 nTargetUID = 0;

	pop(nHitterUID);
	pop(nTargetUID);

	sRECV_HITTER_INFO hitter;
	hitter.s_nUID = nHitterUID;
	hitter.s_HitType = CsC_AttackProp::HT_Miss;

	sRECV_TARGET_INFO target;
	target.s_nUID = nTargetUID;
	target.s_DamageType = CsC_AttackProp::DT_Miss;
	target.s_nNumEffectType = NUMTYPE::ET_NORMAL;

	SetAttackInfo(&hitter, &target, false);
}

void cCliGame::RecvTargetInfo(void)
{
	u4 nUID = 0;
	u1 nHPRate = 0;
	u4 nRightUID = 0;		// 소유권자
	u4 nExpireTimeTS = 0;	// 0이면 소멸과 무관, 수치가 있다면.. 소멸될 시간임
	u4 nCreatedTimeTS = 0;	// 생성된 시간

	pop(nUID);
	pop(nHPRate);
	pop(nRightUID);
	pop(nExpireTimeTS);
	pop(nCreatedTimeTS);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(nUID);
	if (pObject == NULL)
		return;

	switch (pObject->GetLeafRTTI())
	{
	case RTTI_DIGIMON_USER:
	case RTTI_TAMER_USER:
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_TUTORIAL_TAMER:
		return;
	}

	pObject->GetProp_Attack()->ClearDamageValue();
	pObject->GetBaseStat()->SetHP(nHPRate);

	if (pObject->GetLeafRTTI() == RTTI_MONSTER)
	{
		CMonster* pMonster = (CMonster*)pObject;

		pMonster->SetOwnerID(nRightUID);
		if (nExpireTimeTS)
		{
			pMonster->SetExpireTimeTS(nExpireTimeTS);
			pMonster->SetTotalExpireTime(nExpireTimeTS - nCreatedTimeTS);
		}
	}
}

void cCliGame::RecvSkillDie(void)
{
	u4 nHitterUID;
	u4 nTargetUID; // 죽는넘

	u4 nSkillArrIDX; // nHitter's skill array idx
	n4 nDamage;

	pop(nHitterUID);
	pop(nTargetUID);
	pop(nSkillArrIDX);
	pop(nDamage);

	CsC_AvObject* pTarget = g_pMngCollector->GetObject(nTargetUID);
	if (pTarget == NULL)
		return;

	pTarget->SetAniPause(false);

	sRECV_HITTER_INFO hi;
	hi.s_nUID = nHitterUID;
	hi.s_nSkillIndex = nSkillArrIDX;

	CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill(nSkillArrIDX);
	if (pSkillInfo != NULL)
	{
		switch (pSkillInfo->s_nSkillType)
		{
		case CsMonsterSkill::ATTACH_SEED:	// 씨앗깔기		( 대상 디버프, 시간 경과 후 대상 위치 바닥 생성 )
		case CsMonsterSkill::BERSERK:		// 광폭화		( 공략 시간 초과 )
			hi.s_HitType = CsC_AttackProp::HT_Skill_NoAni;
			break;
		default:
			hi.s_HitType = CsC_AttackProp::HT_Skill;
			break;
		}
	}
	else
		hi.s_HitType = CsC_AttackProp::HT_Skill;

	sRECV_TARGET_INFO ti;
	ti.s_nUID = nTargetUID;
	ti.s_DamageType = CsC_AttackProp::DT_Normal;
	ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
	ti.s_nValue[0] = -nDamage;
	ti.s_nValue[1] = -pTarget->GetBaseStat()->GetBackupHP();

	SetSkillInfo(&hi, &ti, true);

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	//CProtocol.m_wStr = GetVAString( _T("RecvSkillDie SetDieInfo UID : %d"), pTarget->GetUniqID() );
	DmCS::StringFn::Format(CProtocol.m_wStr, _T("RecvSkillDie SetDieInfo UID : %d"), pTarget->GetUniqID());

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	SetDieInfo(pTarget);
}

void cCliGame::RecvDotDie()
{
	// 서버에서 안 보낸다고 함
// 	u4 HitterUID = 0;
// 	u4 TargetUID = 0;
// 	n2 nDamage = 0;	
// 	u2 nBuffCode = 0;
// 
// 	pop( HitterUID );
// 	pop( TargetUID );
// 	pop( nDamage );
// 	pop( nBuffCode );
// 
// 	CsC_AvObject* pTarget = g_pMngCollector->GetObject( TargetUID );
// 	if( pTarget == NULL )
// 		return;	
// 
// 	sRECV_TARGET_INFO ti;
// 	ti.s_nUID = TargetUID;
// 	ti.s_DamageType = CsC_AttackProp::DT_Damage_Dot;
// 	ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
// 	ti.s_nValue[ 0 ] = nDamage;
// 	ti.s_nValue[ 1 ] = -pTarget->GetBaseStat()->GetBackupHP();
// 
// 	sRECV_HITTER_INFO hi;
// 	hi.s_nUID = HitterUID;
// 	hi.s_nSkillIndex = nBuffCode;
// 	hi.s_HitType = CsC_AttackProp::HT_Normal;
// 
// 	SetDotInfo( &hi, &ti, nBuffCode, true );
// 
// 	ST_CHAT_PROTOCOL	CProtocol;
// 	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
// 	//CProtocol.m_wStr = GetVAString( _T("RecvDotDie SetDieInfo UID : %d"), pTarget->GetUniqID() );
// 	DmCS::StringFn::Format(CProtocol.m_wStr, _T("RecvDotDie SetDieInfo UID : %d"), pTarget->GetUniqID());
// 	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
// 	SetDieInfo( pTarget );
}

void cCliGame::RecvBattleOn(void)
{
	u4 uid = 0;
	pop(uid);

	//	net::chat("전투 상태 ON %d", uid);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(uid);
	if (pObject == NULL)
		return;

	pObject->SetBattle();
}

void cCliGame::RecvBattleOff(void)
{
	u4 uid = 0;
	pop(uid);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(uid);
	if (pObject == NULL)
		return;

	pObject->GetProp_Attack()->InsertEvent(CsC_AttackProp::EVENT_END_BATTLE, 0, 0, 0, 0);


	// GSP의 꼉우 battle 시간을 수신
#ifdef PLAY_PANELTY
	if (pObject->GetLeafRTTI() == RTTI_DIGIMON_USER)
	{
		u4 CashTime = 0;
		u4 NormalTime = 0;
		pop(NormalTime);
		pop(CashTime);

		g_pDataMng->PlayTime_Set_CashTime(CashTime);
		g_pDataMng->PlayTime_Set_NormalTime(NormalTime);
	}
#endif

	if (pObject->GetLeafRTTI() == RTTI_MONSTER)
	{
		// [11/15/2016 hyun] 몬스터 제어권자 초기화
		CMonster* pMonster = dynamic_cast<CMonster*>(pObject);
		SAFE_POINTER_RET(pMonster);
		pMonster->SetOwnerID(0);
	}
}

void cCliGame::RecvGetExp(void)
{
	n8 nTamerGainExp;	// 이번에 획득한 exp
	n8 nTamerBonusExp;	//
	n8 nTamerExp; // 최종 exp
	u4 nDigimonUID;
	n8 nDigimonGainExp;
	n8 nDigimonBonusExp;
	n8 nDigimonExp; // 최종 exp
	u4 nSkillExp;

	pop(nTamerGainExp);
	pop(nTamerBonusExp);
	pop(nTamerExp);		// 

	pop(nDigimonUID);	// 소유 디지몬이 여러 마리일 경우 처리를 위함
	pop(nDigimonGainExp);
	pop(nDigimonBonusExp);
	pop(nDigimonExp);	// 

	pop(nSkillExp);

	nTamerGainExp = nTamerGainExp / 100;
	nTamerBonusExp = nTamerBonusExp / 100;
	nTamerExp = nTamerExp / 100;
	nDigimonGainExp = nDigimonGainExp / 100;
	nDigimonBonusExp = nDigimonBonusExp / 100;
	nDigimonExp = nDigimonExp / 100;

#ifdef PLAY_PANELTY
	if ((nTamerGainExp == 0) && (nDigimonGainExp == 0) && (g_pDataMng->PlayTime_Get_PaneltyType() == nPlayTime::MAX_PENALTY))
	{
		cPrintMsg::PrintMsg(30419);
	}
	else if (nDigimonBonusExp == 0)
	{
		n8 TGain = nTamerGainExp + nTamerBonusExp;
		n8 DGain = nDigimonGainExp + nDigimonBonusExp;
		cPrintMsg::PrintMsg(30007, &TGain, &DGain);
	}
	else
	{
		n8 TGain = nTamerGainExp + nTamerBonusExp;
		n8 DGain = nDigimonGainExp + nDigimonBonusExp;
		n8 TBonusGain = nTamerBonusExp;
		n8 DBonusGain = nDigimonBonusExp;
		cPrintMsg::PrintMsg(30008, &TGain, &nTamerBonusExp, &DGain, &nDigimonBonusExp);
	}
#else
	if (nDigimonBonusExp == 0)
	{
		n8 TGain = nTamerGainExp + nTamerBonusExp;
		n8 DGain = nDigimonGainExp + nDigimonBonusExp;
		cPrintMsg::PrintMsg(30007, &TGain, &DGain);
	}
	else
	{
		n8 TGain = nTamerGainExp + nTamerBonusExp;
		n8 DGain = nDigimonGainExp + nDigimonBonusExp;
		n8 TBonusGain = nTamerBonusExp;
		n8 DBonusGain = nDigimonBonusExp;
		cPrintMsg::PrintMsg(30008, &TGain, &nTamerBonusExp, &DGain, &nDigimonBonusExp);
	}
#endif	

	g_pCharMng->GetTamerUser()->GetBaseStat()->SetExp(nTamerExp);

	uint nDigimonIDX = GetIDX(nDigimonUID);
	CDigimon* pDigimon = g_pCharMng->GetDigimon(nDigimonIDX);
	if (pDigimon)
	{
		pDigimon->GetBaseStat()->SetExp(nDigimonExp);
	}


	// begin 스킬 경험치+
	if (nSkillExp > 0)
	{
		assert_cs(g_pCharMng->GetDigimon(GetIDX(nDigimonUID))->GetLeafRTTI() == RTTI_DIGIMON_USER);

		if (pDigimon)
		{
			CDigimonUser* pDigimonUser = (CDigimonUser*)pDigimon;
			cEvoUnit* pEvoUnit = pDigimonUser->GetAttributeEvoUnit(pDigimonUser->GetFTEvolCurObj()->m_nEvoSlot);
			pEvoUnit->m_nSkillExp = CsFloat2Int(nSkillExp);
		}
	}
	// end 스킬 경험치+

}

void cCliGame::RecvLevelUp(void)
{
	u4 nUID = 0;
	char cLevel = 0;

	pop(nUID);
	pop(cLevel);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(nUID);
	if (pObject == NULL)
		return;

	assert_cs((pObject->IsKindOf(RTTI_DIGIMON)) || (pObject->IsKindOf(RTTI_TAMER)));
	pObject->GetProp_Attack()->InsertEvent(CsC_AttackProp::EVENT_LEVEL_UP, (int)cLevel, 0, 0, 0);
	if (pObject->IsKindOf(RTTI_DIGIMON_USER))//디지몬이 레벨업 한거면
	{
		GS2C_RECV_ENCYCLOPEDIA_TACTICSHOUSE recv;
		recv.nDigimonID = ((CDigimon*)pObject)->GetBaseDigimonFTID();
		recv.nLevel = (u1)cLevel;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_LV_CHECK, &recv);
	}
	else if (pObject->IsKindOf(RTTI_TAMER_USER))//테이머가 레벨업 한거면
	{
		GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);
	}
}

void cCliGame::RecvHealUp(void)
{
	u4 nUID = 0;
	u2 nHP = 0; // 최종 HP
	u2 nDS = 0; // 최종 DS
	u2 nFP = 0; // 최종 FP(fatigue point)

	pop(nUID);
	pop(nHP);
	pop(nDS);
	pop(nFP);	// 테이머인 경우만 의미가 있음, 디지몬인 경우 0

	CsC_AvObject* pObject = g_pMngCollector->GetObject(nUID);
	if (pObject == NULL)
		return;

	if (pObject->GetLeafRTTI() != RTTI_DIGIMON_USER &&
		pObject->GetLeafRTTI() != RTTI_TAMER_USER)
		return;

	pObject->GetBaseStat()->SetHP(nHP);
	pObject->GetBaseStat()->SetDS(nDS);
	pObject->GetBaseStat()->SetFP(nFP);


#ifndef _GIVE
	if (pObject->GetLeafRTTI() == RTTI_TAMER_USER)
	{
		TCHAR sz[260];
		_stprintf_s(sz, 260, _T("현재 피로도 %d"), nFP);
		ContentsStream kStream;
		wstring str = sz;
		bool bParcing = true;	bool bCheckSameString = false;
		kStream << str << bParcing << bCheckSameString;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream);
	}
#endif
}

// 자신의 정보는 자신에게는 오지 아니함
void cCliGame::RecvHpRate(void)
{
	u4 nUID = 0;
	u1 nHpRate = 0;

	pop(nUID);
	pop(nHpRate);

	SetHpRate(nUID, nHpRate);
}

void cCliGame::SetHpRate(u4 nUID, u1 nHpRate)
{
	CsC_AvObject* pObject = g_pMngCollector->GetObject(nUID);
	if (pObject == NULL)
		return;

	assert_cs(pObject->GetLeafRTTI() != RTTI_TAMER_USER);
	assert_cs(pObject->GetLeafRTTI() != RTTI_DIGIMON_USER);

	pObject->GetBaseStat()->SetHP(nHpRate);
}

void cCliGame::RecvMsgAllByItemFailure(void)	// 외치기 사용 실패
{
	int nErrCode;
	pop(nErrCode);

	switch (nErrCode)
	{
	case 0:	// 아이템 부족
		return;
	case 1:		// 아이템 만료
		return;

	default:
		return;
	}


}

void cCliGame::RecvMsgShoutFailure(void)
{
	int nErrCode;
	pop(nErrCode);

	switch (nErrCode)
	{
	case 1:
	{
		cPrintMsg::PrintMsg(20027);

		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::RESET_SHOUTTIME;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
	break;

	default:
		LOG("unknown shout failure code(%d)", nErrCode);
	}
}

void cCliGame::RecvCreateMonsterFailure(void)
{
	// 	CHAT_TEXT_DEGUG( _T( "cCliGame::RecvCreateMonsterFailure...maybe type error" ) );
}

void cCliGame::RecvSetMoveSpeed(void)
{
	u2 nMoveSpeedT = 0;
	u2 nMoveSpeedD = 0;

	u4 nTamerUID = 0;
	pop(nTamerUID);
	u4 nDigimonUID0 = 0;
	pop(nDigimonUID0);

#ifdef KEYBOARD_MOVE
	if (g_pResist->m_Global.s_bMoveDigimon && nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_BATTLE)
	{
		pop(nMoveSpeedD);
		pop(nMoveSpeedT);
	}
	else
	{
		pop(nMoveSpeedT);
		pop(nMoveSpeedD);
	}
#else
	pop(nMoveSpeedT);
	pop(nMoveSpeedD);
#endif
	u4 nTamerCondition = 0;
	pop(nTamerCondition);
	u4 nDigimonCondition = 0;
	pop(nDigimonCondition);

	CsC_AvObject* pTamer = g_pMngCollector->GetObject(nTamerUID);
	if (pTamer)
	{
		assert_cs(pTamer->IsKindOf(RTTI_TAMER));
		pTamer->GetBaseStat()->SetMoveSpeed((float)nMoveSpeedT);
		((CTamer*)pTamer)->SetCondition(nTamerCondition);
	}
	CsC_AvObject* pDigimon = g_pMngCollector->GetObject(nDigimonUID0);
	if (pDigimon)
	{
		assert_cs(pDigimon->IsKindOf(RTTI_DIGIMON));
		pDigimon->GetBaseStat()->SetMoveSpeed((float)nMoveSpeedD);
		((CDigimon*)pDigimon)->SetCondition(nDigimonCondition);
	}

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	//CProtocol.m_wStr = GetVAString( _T( "MoveSpeed %d %d => %d %d" ), nTamerUID, nDigimonUID0, nMoveSpeedT, nMoveSpeedD );
	DmCS::StringFn::Format(CProtocol.m_wStr, _T("MoveSpeed %d %d => %d %d"), nTamerUID, nDigimonUID0, nMoveSpeedT, nMoveSpeedD);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);

}

void cCliGame::RecvEvoOpen(void)
{
	// 사용 안됨
	assert_cs(false);

	u2 nUID;
	u1 nEvoSlotNo;
	pop(nUID);	// digimon's uid
	pop(nEvoSlotNo);

}

void cCliGame::RecvEvoFailure(void)
{
	if (g_pDataMng)
	{
		int nSuccess = 0;
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CHANGE_KFM, 0, &nSuccess);
	}
}

// 자신 포함 모든 유저들의 진화를 받는다
void cCliGame::RecvEvolution(void)
{
	u4 nUID = 0;
	pop(nUID);	// digimon's uid

	u4 nTamerUID = 0;
	pop(nTamerUID);

	u4 nEvoType = 0;
	pop(nEvoType);

	u1 nEvoSlotNo = 0;
	pop(nEvoSlotNo);

	u1 nHPRate = 0;
	pop(nHPRate);

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	u4 nPartsType = 0;
	pop(nPartsType);	// 디지몬 파츠 아이템 정보
#endif

	cType t(nUID);
	CDigimon* pDigimon = g_pCharMng->GetDigimon(t.m_nIDX);	// 자신이면 DigimonUser
	if (pDigimon != NULL)
	{
		if (nEvoSlotNo == nEvo::BurstMode)
		{
			pDigimon->Scene_Evol(nEvoType, true);
			// 진화 중일때만 테이머 애니메이션 보여주자
			t.m_nUID = nTamerUID;
			CTamer* pTamer = g_pCharMng->GetTamer(t.m_nIDX);
			if (pTamer)
			{
				pTamer->Scene_Evol(pDigimon, nEvoType);
			}
		}
		else if (nEvoSlotNo < nLimit::EvoStep - 1)
		{
			pDigimon->Scene_Evol(nEvoType, false);

			// 진화 중일때만 테이머 애니메이션 보여주자
			t.m_nUID = nTamerUID;
			CTamer* pTamer = g_pCharMng->GetTamer(t.m_nIDX);
			if (pTamer)
			{
				pTamer->Scene_Evol(pDigimon, nEvoType);
			}
		}
		else
		{
			pDigimon->Scene_Degenerate(nEvoType);
		}
		if (pDigimon->GetLeafRTTI() == RTTI_DIGIMON)
		{
			pDigimon->GetBaseStat()->SetHP(nHPRate);
		}
		// 위에 Scene_Evol()함수다음에 반드시 호출해야한다. Scene_Evol()함수안에 구현하거나..
		if (pDigimon->GetLeafRTTI() == RTTI_DIGIMON_USER)
		{
			GS2C_RECV_CHECKTYPE recv;
			recv.nType = AchieveContents::CA_Partner_Evo_JointProgress;
			recv.nValue1 = nEvoType;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);
		}

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
		// 이전 이펙트 파츠 정보 제거
		pDigimon->ClearAllPostDParts();
		// 이펙트 파츠 정보 추가
		pDigimon->SetPostDParts(nPartsType);
#endif
	}

#ifdef _DEBUG
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	//CProtocol.m_wStr = GetVAString( _T( "EVO nUID(%d) nEvoSlotNo(%d)" ), nUID, nEvoSlotNo );
	DmCS::StringFn::Format(CProtocol.m_wStr, _T("EVO nUID(%d) nEvoSlotNo(%d)"), nUID, nEvoSlotNo);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
#endif //_DEBUG
}

void cCliGame::RecvChangeStatValue(void)
{
	// 안쓰임
	assert_cs(false);

	// 자기 자신 ( 테이머 + 디지몬 )

// 	u2 nUID;
// 	u1 nStat;
// 	n4 nValue;
// 
// 	pop(nUID);
// 	pop(nStat);
// 	pop(nValue);
// 
// 	CsC_AvObject* pObject = g_pMngCollector->GetObject( nUID );
// 	if( pObject == NULL )
// 		return;

	/*switch( nStat )
	{
	case HP:	pObject->GetBaseStat()->SetHP( nValue );		break;
	case DS:	pObject->GetBaseStat()->SetDS( nValue );		break;
	case DP:	pAttObject->m_nDefence = nValue;			break;
	case AP:	pAttObject->m_nAttPower = nValue;			break;
	case EV:	pAttObject->m_nEvade = nValue;				break;
	case CA:	pAttObject->m_nCriticalRate = nValue;		break;
	case MS:	pAttObject->m_fSpeed = (float)nValue;		break;
	case AS:	pAttObject->m_fAttSpeed = nValue*0.001f;	break;
	case AR:	pAttObject->m_fAttRange = (float)nValue;	break;

	case MHP:	if(nValue < pAttObject->m_nHP)
				{
					pAttObject->m_nHP = nValue;
				}
				pAttObject->m_nMaxHP = nValue;				break;

	case MDS:	if(nValue < pAttObject->m_nDS)
				{
					pAttObject->m_nDS = nValue;
				}
				pAttObject->m_nMaxDS = nValue;				break;

	default :	assert_cs( false );							break;
	}*/
}


//===============================================================================================
//
// 어택정보 설정
//
//===============================================================================================
void cCliGame::SetAttackInfo(sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack)
{
	CsC_AvObject* pHitter = g_pMngCollector->GetObject(pHitterInfo->s_nUID);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(pTargetInfo->s_nUID);

	if (pHitter && (pHitter->IsLoad() == false))
		pHitter = NULL;

	if (pTarget && (pTarget->IsLoad() == false))
		pTarget = NULL;


	//==============================================================================
	//	둘다 안보일때
	//==============================================================================
	if ((pHitter == NULL) || (pTarget == NULL))
		return;

	assert_cs(pHitter->GetProp_Animation()->GetSequenceMng());
	assert_cs(pTarget->GetProp_Animation()->GetSequenceMng());

	// 정보구함 - 원거리근거리인지, 애니메이션 머해야하는지, 애니메이션 플레이 가속도
	pHitter->DeletePath();
	pHitterInfo->CalInfo(pHitter);


	// 일반 공격 이벤트구함
	CsC_AniProp* pHitterAniProp = pHitter->GetProp_Animation();
	int nEventCount = 1;
	CsVectorPB< int >* pEventArray;
	pHitterAniProp->GetNAttEventIndex(pHitterInfo->s_AniID, &pEventArray, nEventCount);
	// 	assert_csm1( pEventArray->Size() > 0, _T( "공격이벤트가 안들어가 있어요 ID = %d" ), pHitterInfo->s_AniID );
	if (pEventArray->Size() <= 0)
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T("공격이벤트가 안들어가 있어요 ID = %d"), pHitterInfo->s_AniID );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("공격이벤트가 안들어가 있어요 ID = %d"), pHitterInfo->s_AniID);
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
	//==============================================================================
	// 떄리는넘의 애니메이션 셋팅값을 구하자
	//==============================================================================	
	CsVectorPB< float > vpEventTime(10);
	for (int i = 0; i < nEventCount; ++i)
	{
		vpEventTime.PushBack(pHitter->GetProp_Animation()->CalEventTime(pHitterInfo->s_AniID, pEventArray->GetData(i), pHitterInfo->s_fAniAccelRate));
	}
	// 레인지 타입이라면
	if (pHitterInfo->s_bRangeAttack)
	{
		float fDist = (pHitter->GetPos2D() - pTarget->GetPos2D()).Length();

		float fHitTimeVelocity = CsC_AttackProp::GetNormalRangeVelocity();
		float fHitTimeAccelVelocity = CsC_AttackProp::GetNormalRangeAccelVelocity();

		if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		{
			fHitTimeVelocity *= BATTLE_SPEED_RANGE;
			fHitTimeAccelVelocity *= BATTLE_SPEED_RANGE;
		}

		// 충돌 시간
		float fRangeHitTime = CsC_AttackProp::CalRangeHitTime(fHitTimeVelocity, fHitTimeAccelVelocity, fDist);
		for (int i = 0; i < nEventCount; ++i)
		{
			vpEventTime[i] += fRangeHitTime;
			if (pTargetInfo->s_DamageType == CsC_AttackProp::DT_Miss)
				vpEventTime[i] = CsMax(0.0f, vpEventTime[i] - 0.3f);
		}
	}

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sDAMAGE_INFO* pDamageInfo = NULL;
	// 데미지 계산
	float fMin = 45.0f;
	float fMax = 50.0f;

	// 데미지 날라가는 방향 계산
	NiPoint3 v = pTarget->GetPos() - pHitter->GetPos();
	v.z = 0.0f;
	v.Unitize();
	float fRot = NiACos(NiPoint3(0.0f, -1.0f, 0.0f).Dot(v));
	if (NiPoint3(0.0f, -1.0f, 0.0f).Cross(v).z > 0)
		fRot = -fRot;

	CsC_AttackProp::sDAMAGE_INFO DInfo;
	DInfo.s_pHitter = pHitter;
	DInfo.s_eDamageType = pTargetInfo->s_DamageType;
	DInfo.s_nNumType = (pHitter->GetLeafRTTI() == RTTI_MONSTER || pHitter->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER) ? NUMTYPE::MONSTER_ATTACK : NUMTYPE::DIGIMON_ATTACK;
	DInfo.s_eActive = CsC_AttackProp::AT_NONE_ACTIVE;
	DInfo.s_nNumEffect = pTargetInfo->s_nNumEffectType;
	DInfo.s_eMarbleType = CsC_AttackProp::MB_NONE;
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		DInfo.s_eMarbleType |= CsC_AttackProp::MB_EVOL;
	}

	int nTotalValue[5];
	int nTotalDemage = 0;
	memcpy(nTotalValue, pTargetInfo->s_nValue, sizeof(int) * 5);
	for (int i = nEventCount - 1; i > -1; --i)
	{
		if (i == 0)
		{
			memcpy(DInfo.s_nValue, nTotalValue, sizeof(int) * 5);
		}
		else
		{
			// 랜덤 값으로 데미지 타격횟수로 나누어 준다. - 결과 값은 같게 만들자
			float fCalDamageRate = 0.0f;
			int nRandRate = static_cast<int>(fMax - fMin);
			if (0 == nRandRate)
				fCalDamageRate = fMin * 0.01f;
			else
				fCalDamageRate = (rand() % nRandRate + fMin) * 0.01f;

			// 마지막값이 조금 크게 만들자
			if (i == nEventCount - 1)
			{
				fMin = 50.0f / (nEventCount - 1);
				fMax = 70.0f / (nEventCount - 1);
			}

			for (int t = 0; t < 5; ++t)
			{
				DInfo.s_nValue[t] = (int)(pTargetInfo->s_nValue[t] * fCalDamageRate);

				if (DInfo.s_nValue[t] < 0)
				{
					if (DInfo.s_nValue[t] < nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				else
				{
					if (DInfo.s_nValue[t] > nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				nTotalValue[t] -= DInfo.s_nValue[t];
			}
		}

		//데미지 계산
		nTotalDemage += DInfo.s_nValue[0];

		// 랜덤 적용
		switch (DInfo.s_nNumEffect)
		{
		case NUMTYPE::ET_NORMAL:
			DInfo.s_fDamageRot = fRot + CsD2R(rand() % 70 - 35);
			break;
		case NUMTYPE::ET_CRITICAL:
			DInfo.s_fDamageRot = 0.0f;
			break;
		default:
			assert_cs(false);
		}

		DInfo.s_fHitEventTime = vpEventTime[i];
		DInfo.s_dwResistTime = GetTickCount() + (DWORD)(DInfo.s_fHitEventTime * 2000.0f);		// 틱이므로 1000 을 더 곱하자
		DInfo.s_pChildDamageInfo = pDamageInfo;

		CsC_AttackProp* pAtProp = pTarget->GetProp_Attack();
		if (pAtProp)
		{
			// 			if( bLastAttack )
			// 				pAtProp->DeleteDemageEvent();

			pDamageInfo = pAtProp->InsertDamage(&DInfo);
		}

		DInfo.s_nNumEffect = NUMTYPE::ET_NORMAL;
	}

	// 타겟 찍어 놓은게 없을때
	if (pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER)
	{
		if (g_pCharResMng->GetTargetMark() == NULL)
			g_pCharResMng->SetTargetMark(pHitter);
	}

	// 데미지 전투로그
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_BATTLELOG_WINDOW, 0))
	{
		ContentsStream kStream;
		float nRemainTime = vpEventTime[nEventCount - 1];
		int nTotalDem = abs(nTotalDemage);
		kStream << pHitterInfo << pTargetInfo << nRemainTime << nTotalDem << bLastAttack;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_BATTLE_PROCESS, &kStream);
	}

	//==============================================================================
	// 때린넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sHIT_INFO TInfo;
	TInfo.s_eHitType = pHitterInfo->s_HitType;
	TInfo.s_dwAniID = pHitterInfo->s_AniID;
	TInfo.s_fAniAccelRate = pHitterInfo->s_fAniAccelRate;
	TInfo.s_pPartnerDamageInfo = pDamageInfo;
	TInfo.s_pHitTarget = pTarget;
	TInfo.s_fRangeVelocity = CsC_AttackProp::GetNormalRangeVelocity();
	TInfo.s_fRangeAccelVelocity = CsC_AttackProp::GetNormalRangeAccelVelocity();
	TInfo.s_bRangeAttack = pHitterInfo->s_bRangeAttack;
	// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
	TInfo.s_nReflectType = pHitterInfo->s_nReflectType;

	CsMapMonster* tmpMon = nsCsMapTable::g_pMapMonsterMng->GetGroup(nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID)->GetMonster_ByMonsterID(pHitter->GetFTID());
	if (tmpMon != NULL && tmpMon->GetInfo()->s_nMoveType == 4)//오브젝트형 체크. 오브젝트형은 안돌고 안따라가야되
		TInfo.s_bRotation = false;//돌아볼꺼냐
	else
		TInfo.s_bRotation = true;

	TInfo.s_vDamagePos = pTarget->GetPos();

	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		TInfo.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
		TInfo.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
	}


	switch (g_pResist->m_Global.s_eAttackEffectViewMode)
	{
	case cResist::sGLOBAL::AEM_ALL_OFF:			// 랜더링 자체를 안하겠다
		TInfo.s_nAttackEffectIndex = -1;
		break;
	case cResist::sGLOBAL::AEM_ONLY_NORATT_ON:	// 일반 타격이펙트만 랜더링
		TInfo.s_nAttackEffectIndex = 0;
		break;
	case cResist::sGLOBAL::AEM_ALL_ON:
		TInfo.s_nAttackEffectIndex = pHitter->IsStrongNature2(pTarget) ? pHitter->GetNatureType2() : 0;
		break;
	default:
		assert_cs(false);
	}

	CsC_AttackProp* pAtProp = pHitter->GetProp_Attack();
	if (pAtProp)
	{
		if (bLastAttack)
			pAtProp->DeleteHitEvent();

		pAtProp->InsertHit(&TInfo);
	}
}


//===============================================================================================
//
// 스킬정보 설정 광역 스킬할때
//
//===============================================================================================
void cCliGame::SetDamage(sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo)
{
	CsC_AvObject* pHitter = g_pMngCollector->GetObject(pHitterInfo->s_nUID);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(pTargetInfo->s_nUID);

	if (pHitter && (pHitter->IsLoad() == false))
		pHitter = NULL;

	if (pTarget && (pTarget->IsLoad() == false))
		pTarget = NULL;

	//==============================================================================
	//	타겟은 반드시 존재해야함
	//==============================================================================
	if (pTarget == NULL)
		return;

	assert_cs(pTarget->GetProp_Animation()->GetSequenceMng());

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================	
	CsC_AttackProp::sDAMAGE_INFO DInfo;
	DInfo.s_pHitter = pHitter;
	DInfo.s_eDamageType = pTargetInfo->s_DamageType;
	DInfo.s_nNumType = (pTarget->GetLeafRTTI() == RTTI_MONSTER || pTarget->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER) ? NUMTYPE::DIGIMON_ATTACK : NUMTYPE::MONSTER_ATTACK;
	DInfo.s_eActive = CsC_AttackProp::AT_ACTIVE;
	DInfo.s_nNumEffect = pTargetInfo->s_nNumEffectType;
	DInfo.s_eMarbleType = CsC_AttackProp::MB_NONE;
	memcpy(DInfo.s_nValue, pTargetInfo->s_nValue, sizeof(int) * 5);

	// 랜덤 적용
	DInfo.s_fDamageRot = 0.0f;
	DInfo.s_fHitEventTime = 0.0f;
	DInfo.s_pChildDamageInfo = NULL;

	pTarget->GetProp_Attack()->InsertDamage(&DInfo);

	//토탈데미지 계산	
	int nTotalDemage = 0;
	nTotalDemage += DInfo.s_nValue[0];

	// 데미지 전투 로그
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_BATTLELOG_WINDOW, 0))
	{
		ContentsStream kStream;
		float nRemainTime = 0.0f;
		int nTotalDem = abs(nTotalDemage);

		kStream << pHitterInfo << pTargetInfo << nRemainTime << nTotalDem;

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_BATTLE_PROCESS, &kStream);
	}

#ifdef DAMAGE_METER
	sDamageMeterInfo damageInfo(pHitterInfo->s_nUID, pTargetInfo->s_nUID, nTotalDemage);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_DAMAGEMETER_DATA, &damageInfo);
#endif
}


void cCliGame::SetSkillInfo2(sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack, u4 nTargetUID, int nSkillSpeed, int nSkillAceel)
{
	if (bLastAttack)
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		CProtocol.m_wStr = _T("SetSkillInfo2 bLastAttack");
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
	// 때린놈과 타겟을 얻는다.
	CsC_AvObject* pHitter = g_pMngCollector->GetObject(pHitterInfo->s_nUID);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(pTargetInfo->s_nUID);

	if (pHitter)
	{
		// 때리는놈 로드 되이않았으면?
		if (pHitter->IsLoad() == false)
		{
			// 때리는 놈이 내디지몬이면.
			if (pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER)
			{
				((CDigimonUser*)pHitter)->RecvSkill(false, pHitterInfo->s_nSkillIndex);
			}
			pHitter = NULL;
		}
	}

	// 타겟도 로드되지 않았으면...
	if (pTarget && (pTarget->IsLoad() == false))
		pTarget = NULL;

	//==============================================================================
	//	둘중에 하나라도 안보이면 패스 ??안보이면??
	//==============================================================================
	if ((pHitter == NULL) || (pTarget == NULL))
		return;

	assert_cs(pHitter->GetProp_Animation()->GetSequenceMng());
	assert_cs(pTarget->GetProp_Animation()->GetSequenceMng());


	//==============================================================================
	// 떄리는넘의 애니메이션 셋팅값을 구하자
	//==============================================================================	

	CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill(pHitterInfo->s_nSkillIndex);
	if (pSkillInfo == NULL)
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T( "SetSkillInfo2 :: 존재하지 않는 몬스터스킬 - SkillIdx : %d" ), pHitterInfo->s_nSkillIndex );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("SetSkillInfo2 :: 존재하지 않는 몬스터스킬 - SkillIdx : %d"), pHitterInfo->s_nSkillIndex);
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}
	// 데미지 계산 시 스킬 사용 애니 쓰지 않는 스킬들 예외 처리
	bool _bHitterAni;
	switch (pSkillInfo->s_nSkillType)
	{
	case CsMonsterSkill::ATTACH_SEED:	// 씨앗깔기		( 대상 디버프, 시간 경과 후 대상 위치 바닥 생성 )
	case CsMonsterSkill::BERSERK:		// 광폭화		( 공략 시간 초과 )
		_bHitterAni = false;
		break;
	default:	_bHitterAni = true;		break;
	}

	DWORD dwAniID = pSkillInfo->s_dwSequenceID;	// 스킬인덱스 계산
	int nEventCount = 1;
	CsVectorPB< int >* pEventArray;
	bool bHitterAni = true;
	bHitterAni = _bHitterAni;

	// 스킬 정보를 얻고 이것이 밀리냐 래인지냐? 검사.
	pHitterInfo->s_bRangeAttack = pHitter->IsHaveRangeEvent(dwAniID);

	if (bHitterAni)
	{
		// 때리는 넘의 때리는 애니메이션 정보 얻는다.dwAniID 로 아웃 파라미터 2개 얻는다.
		if (pHitter->GetProp_Animation()->GetNAttEventIndex(dwAniID, &pEventArray, nEventCount) == false)
		{
			g_CriticalLog.Log(_T("SetSKillInfo : No Have Ani_1 ( model = %d, ani = %d, SkillIndex = %d )"), pHitter->GetModelID(), dwAniID, pHitterInfo->s_nSkillIndex);
			assert_csm2(false, L"ModelID = %d, LoadSeqModelID = %d", pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID());
			pHitter->RecvSkill(false, pHitterInfo->s_nSkillIndex);
			return;
		}

		assert_csm3(nEventCount > 0, L"AniID = %d, ModelID = %d, LoadSeqModelID = %d", dwAniID, pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID());
	}

	CsVectorPB< float > vpEventTime(10);
	float fAccelRate = 1.0f;
	// 배틀 모드시 래이팅값설정
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		fAccelRate = BATTLE_SPEED_SKILL;

	// 이밴트 카운트 만큼만 
	if (bHitterAni)
	{
		for (int i = 0; i < nEventCount; ++i)
		{
			vpEventTime.PushBack(pHitter->GetProp_Animation()->CalEventTime(dwAniID, pEventArray->GetData(i), fAccelRate));
		}
	}

	// 레인지 타입이라면
	if (pHitterInfo->s_bRangeAttack == true)
	{
		float fDist = (pHitter->GetPos2D() - pTarget->GetPos2D()).Length();

		float fHitTimeVelocity = (float)nSkillSpeed;
		float fHitTimeAccelVelocity = (float)nSkillAceel;
		if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		{
			fHitTimeVelocity *= BATTLE_SPEED_RANGE;
			fHitTimeAccelVelocity *= BATTLE_SPEED_RANGE;
		}

		float fRangeHitTime = CsC_AttackProp::CalRangeHitTime(fHitTimeVelocity, fHitTimeAccelVelocity, fDist);
		for (int i = 0; i < nEventCount; ++i)
		{
			vpEventTime[i] += fRangeHitTime;
		}
	}

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sDAMAGE_INFO* pDamageInfo = NULL;
	// 데미지 계산
	float fMin = 90.0f / (float)nEventCount;
	float fMax = 110.0f / (float)nEventCount;

	// 데미지 날라가는 방향 계산
	NiPoint3 v = pTarget->GetPos() - pHitter->GetPos();	// 타겟과 어택커와의 방향 계산
	v.z = 0.0f;
	v.Unitize();
	float fRot = NiACos(NiPoint3(0.0f, -1.0f, 0.0f).Dot(v));
	if (NiPoint3(0.0f, -1.0f, 0.0f).Cross(v).z > 0)
		fRot = -fRot;

	// 맞는넘의 어택 프로퍼티 등록
	CsC_AttackProp::sDAMAGE_INFO DInfo;
	DInfo.s_pHitter = pHitter;
	DInfo.s_eDamageType = pTargetInfo->s_DamageType;
	DInfo.s_nNumType = (pHitter->GetLeafRTTI() == RTTI_MONSTER || pHitter->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER) ? NUMTYPE::MONSTER_ATTACK : NUMTYPE::DIGIMON_ATTACK;
	DInfo.s_eActive = CsC_AttackProp::AT_NONE_ACTIVE;
	DInfo.s_nNumEffect = pTargetInfo->s_nNumEffectType;
	DInfo.s_vHitterPos = pHitter->GetPos2D();

	// 넉백이라면 가장 초기에 한번만 셋팅해주겠다
	if (pTargetInfo->s_DamageType == CsC_AttackProp::DT_KnockBack)
	{
		DInfo.s_eDamageType = CsC_AttackProp::DT_Normal;
	}
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		DInfo.s_eMarbleType |= CsC_AttackProp::MB_EVOL;
	}



	///////////////////////////////////////////////////////////////////////////////////////////////
	// 스킬 타격 횟수만큼 돌려서 하나씩 프로퍼티에 등록 예)짤싹때리기== 횟수 2번
	int nTotalValue[5];
	int nTotalDemage = 0;
	memcpy(nTotalValue, pTargetInfo->s_nValue, sizeof(int) * 5);
	for (int i = nEventCount - 1; i > -1; --i)
	{
		if (i == 0)
		{
			memcpy(DInfo.s_nValue, nTotalValue, sizeof(int) * 5);
			if (pTargetInfo->s_DamageType == CsC_AttackProp::DT_KnockBack)
			{
				DInfo.s_eDamageType = CsC_AttackProp::DT_KnockBack;
			}
		}
		else
		{
			// 랜덤 값으로 데미지 타격횟수로 나누어 준다. - 결과 값은 같게 만들자
			float fCalDamageRate = 0.0f;
			int nRandRate = static_cast<int>(fMax - fMin);
			if (0 == nRandRate)
				fCalDamageRate = fMin * 0.01f;
			else
				fCalDamageRate = (rand() % nRandRate + fMin) * 0.01f;

			for (int t = 0; t < 5; ++t)
			{
				DInfo.s_nValue[t] = (int)(pTargetInfo->s_nValue[t] * fCalDamageRate);

				if (DInfo.s_nValue[t] < 0)
				{
					if (DInfo.s_nValue[t] < nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				else
				{
					if (DInfo.s_nValue[t] > nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				nTotalValue[t] -= DInfo.s_nValue[t];
			}
		}

		//토탈데미지 계산
		nTotalDemage += DInfo.s_nValue[0];

		// 랜덤 적용
		DInfo.s_fDamageRot = fRot + CsD2R(rand() % 70 - 35);

		DInfo.s_fHitEventTime = vpEventTime[i];
		DInfo.s_dwResistTime = GetTickCount();// + (DWORD)( DInfo.s_fHitEventTime*2000.0f );		// 틱이므로 1000 을 더 곱하자
		DInfo.s_pChildDamageInfo = pDamageInfo;


		// 프로퍼티 등록
		pDamageInfo = pTarget->GetProp_Attack()->InsertDamage(&DInfo);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////




	if (pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER)
	{
		if (g_pCharResMng->GetTargetMark() == NULL)
			g_pCharResMng->SetTargetMark(pHitter);
	}

	// 데미지 전투 로그
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_BATTLELOG_WINDOW, 0))
	{
		ContentsStream kStream;
		float nRemainTime = vpEventTime[nEventCount - 1];
		int nTotalDem = abs(nTotalDemage);

		kStream << pHitterInfo << pTargetInfo << nRemainTime << nTotalDem << bLastAttack;

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_BATTLE_PROCESS, &kStream);
	}

	if (pTargetInfo->s_nUID == nTargetUID/* && ( bHitterAni || bLastAttack )*/)
	{

		//==============================================================================
		// 때린넘의 정보 만들자
		//==============================================================================
		CsC_AttackProp::sHIT_INFO info;
		switch (pSkillInfo->s_nSkillType)
		{
		case CsMonsterSkill::ATTACH_SEED:	// 씨앗깔기		( 대상 디버프, 시간 경과 후 대상 위치 바닥 생성 )
		case CsMonsterSkill::BERSERK:		// 광폭화		( 공략 시간 초과 )
			info.s_eHitType = CsC_AttackProp::HT_Skill_NoAni;
			break;
		default:
			info.s_eHitType = CsC_AttackProp::HT_Skill;
			break;
		}
		info.s_dwAniID = dwAniID;
		info.s_fAniAccelRate = fAccelRate;
		info.s_pPartnerDamageInfo = pDamageInfo;
		info.s_pHitTarget = pTarget;
		info.s_fRangeVelocity = (float)nSkillSpeed;
		info.s_fRangeAccelVelocity = (float)nSkillAceel;
		info.s_bRangeAttack = pHitterInfo->s_bRangeAttack;
		info.s_bRotation = true;
		info.s_nAttackEffectIndex = 0;

		switch (g_pResist->m_Global.s_eAttackEffectViewMode)
		{
		case cResist::sGLOBAL::AEM_ALL_OFF:			// 랜더링 자체를 안하겠다
			info.s_nAttackEffectIndex = -1;
			break;
		case cResist::sGLOBAL::AEM_ONLY_NORATT_ON:	// 일반 타격이펙트만 랜더링
			info.s_nAttackEffectIndex = 0;
			break;
		case cResist::sGLOBAL::AEM_ALL_ON:
			info.s_nAttackEffectIndex = pHitter->IsStrongNature2(pTarget) ? pHitter->GetNatureType2() : 0;
			break;
		}

		info.s_vDamagePos = pTarget->GetPos();

		if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		{
			info.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
			info.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		// 프로퍼티 등록
		pHitter->GetProp_Attack()->InsertHit(&info);

		///////////////////////////////////////////////////////////////////////////////////////////////
		//pHitter->RecvSkill( bHitterAni, pHitterInfo->s_nSkillIndex );
		if (bHitterAni)
			pHitter->RecvSkill(true, pHitterInfo->s_nSkillIndex);
	}

#ifdef DAMAGE_METER
	sDamageMeterInfo damageInfo(pHitterInfo->s_nUID, pTargetInfo->s_nUID, nTotalDemage);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_DAMAGEMETER_DATA, &damageInfo);
#endif
}

void cCliGame::RecvRaidRankingList()
{
	// 순위 개수 Count
	// 순위
	// 테이머
	// 디지몬
	// 데미지

	/*
	카운트 u4

	순위   u4
	테이머이름 char[22]
	디지몬이름 char[22]
	데미지 u4

	*/
	u4 Count;

	u4 Rank;
	char szTamer[Language::pLength::id + 1];
	char szDigimon[Language::pLength::id + 1];
	u4 Damage;

	TCHAR	tszTamer[Language::pLength::id + 1];
	TCHAR	tszDigimon[Language::pLength::id + 1];

	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_RAIDRANK) == false)
	{
		g_pGameIF->GetDynamicIF(cBaseWindow::WT_RAIDRANK);
	}
	g_pGameIF->GetRaidRank()->ResetRankList();

	pop(Count);
	for (u4 i = 0; i < Count; i++)
	{
		pop(Rank);
		pop(szTamer);
		pop(szDigimon);
		pop(Damage);


		_tcscpy_s(tszTamer, Language::pLength::id + 1, LanConvertT(szTamer));
		_tcscpy_s(tszDigimon, Language::pLength::id + 1, LanConvertT(szDigimon));

		if (1 == Rank)
			g_pGameIF->GetRaidRank()->SetRanker(i, Rank, tszTamer, tszDigimon, Damage, NiColor(CsB2F(180), CsB2F(175), CsB2F(94)));
		else if (_tcscmp(g_pCharMng->GetTamerUser()->GetName(), tszTamer) == 0)
			g_pGameIF->GetRaidRank()->SetRanker(i, Rank, tszTamer, tszDigimon, Damage, NiColor(CsB2F(252), CsB2F(255), CsB2F(25)));
		else
			g_pGameIF->GetRaidRank()->SetRanker(i, Rank, tszTamer, tszDigimon, Damage);

	}
}

//===============================================================================================
//
// 스킬정보 설정
//
//===============================================================================================
void cCliGame::SetSkillInfo(sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack)
{
	// 때린놈과 타겟을 얻는다.
	CsC_AvObject* pHitter = g_pMngCollector->GetObject(pHitterInfo->s_nUID);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(pTargetInfo->s_nUID);

	if (pHitter)
	{
		// 때리는놈 로드 되이않았으면?
		if (pHitter->IsLoad() == false)
		{
			// 때리는 놈이 내디지몬이면.
			if (pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER)
			{
				((CDigimonUser*)pHitter)->RecvSkill(false, pHitterInfo->s_nSkillIndex);
			}
			pHitter = NULL;
		}
	}

	// 타겟도 로드되지 않았으면...
	if (pTarget && (pTarget->IsLoad() == false))
		pTarget = NULL;

	//==============================================================================
	//	둘중에 하나라도 안보이면 패스 ??안보이면??
	//==============================================================================
	if ((pHitter == NULL) || (pTarget == NULL))
		return;

	assert_cs(pHitter->GetProp_Animation()->GetSequenceMng());
	assert_cs(pTarget->GetProp_Animation()->GetSequenceMng());

	// 때린놈에 대한 스킬정보를 얻는다.
	CsSkill::sINFO* pFTSkillInfo = NULL;
	CsTamerSkill* pTamerSkill = NULL;
	switch (pHitter->GetLeafRTTI())
	{
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
	{
		if (((CDigimon*)pHitter)->GetSkillMng()->IsSkill(pHitterInfo->s_nSkillIndex) == false)
		{
			g_CriticalLog.Log(_T("SetSKillInfo_1 : 스킬테이블에 존재하지도 않는 스킬 ( model = %d, SkillIndex = %d )"), pHitter->GetModelID(), pHitterInfo->s_nSkillIndex);
			return;
		}
		pFTSkillInfo = ((CDigimon*)pHitter)->GetSkillMng()->GetFTSkill(pHitterInfo->s_nSkillIndex)->GetInfo();
	}
	break;
	case RTTI_TUTORIAL_TAMER:
	case RTTI_TAMER:
	case RTTI_TAMER_USER:
	{
		pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill(pHitterInfo->s_nSkillIndex, -1);
		assert_csm(nsCsFileTable::g_pSkillMng->IsTamer_Skill(pTamerSkill->Get_TSkill_Info()->s_dwSkillCode) == true, _T("올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!"));
		pFTSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill(pTamerSkill->Get_TSkill_Info()->s_dwSkillCode)->GetInfo();
	}
	break;
	default:
		assert_cs(false);
	}

	// 스킬 정보를 얻고 이것이 밀리냐 래인지냐? 검사.
	pHitterInfo->s_bRangeAttack = (pFTSkillInfo->s_nAttType == 2);

	//==============================================================================
	// 떄리는넘의 애니메이션 셋팅값을 구하자
	//==============================================================================
	DWORD dwAniID = 0;
	if (pHitter->GetLeafRTTI() == RTTI_TAMER || pHitter->GetLeafRTTI() == RTTI_TAMER_USER)	// 때리는넘이 테이머라면
		dwAniID = pTamerSkill->Get_TSkill_Info()->s_dwTamer_SeqID;
	else
		dwAniID = ANI::ATTACK_SKILL1 + pHitterInfo->s_nSkillIndex;	// 스킬인덱스 계산

	int nEventCount = 1;
	CsVectorPB< int >* pEventArray;
	// 때리는 넘의 때리는 애니메이션 정보 얻는다.dwAniID 로 아웃 파라미터 2개 얻는다.
	if (pHitter->GetProp_Animation()->GetNAttEventIndex(dwAniID, &pEventArray, nEventCount) == false)
	{
		g_CriticalLog.Log(_T("SetSKillInfo : No Have Ani_2 ( model = %d, ani = %d, SkillIndex = %d )"), pHitter->GetModelID(), dwAniID, pHitterInfo->s_nSkillIndex);
		assert_csm2(false, L"ModelID = %d, LoadSeqModelID = %d", pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID());
		pHitter->RecvSkill(false, pHitterInfo->s_nSkillIndex);
		return;
	}

	assert_csm3(nEventCount > 0, L"AniID = %d, ModelID = %d, LoadSeqModelID = %d", dwAniID, pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID());

	CsVectorPB< float > vpEventTime(10);
	float fAccelRate = 1.0f;
	// 배틀 모드시 래이팅값설정
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		fAccelRate = BATTLE_SPEED_SKILL;

	// 이밴트 카운트 만큼만 
	for (int i = 0; i < nEventCount; ++i)
	{
		vpEventTime.PushBack(pHitter->GetProp_Animation()->CalEventTime(dwAniID, pEventArray->GetData(i), fAccelRate));
	}

	// 레인지 타입이라면
	if (pHitterInfo->s_bRangeAttack == true)
	{
		float fDist = (pHitter->GetPos2D() - pTarget->GetPos2D()).Length();

		float fHitTimeVelocity = pFTSkillInfo->s_fSkill_Velocity;
		float fHitTimeAccelVelocity = pFTSkillInfo->s_fSkill_Accel;
		if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		{
			fHitTimeVelocity *= BATTLE_SPEED_RANGE;
			fHitTimeAccelVelocity *= BATTLE_SPEED_RANGE;
		}

		float fRangeHitTime = CsC_AttackProp::CalRangeHitTime(fHitTimeVelocity, fHitTimeAccelVelocity, fDist);
		for (int i = 0; i < nEventCount; ++i)
		{
			vpEventTime[i] += fRangeHitTime;
		}
	}

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sDAMAGE_INFO* pDamageInfo = NULL;
	// 데미지 계산
	float fMin = 90.0f / (float)nEventCount;
	float fMax = 110.0f / (float)nEventCount;

	// 데미지 날라가는 방향 계산
	NiPoint3 v = pTarget->GetPos() - pHitter->GetPos();	// 타겟과 어택커와의 방향 계산
	v.z = 0.0f;
	v.Unitize();
	float fRot = NiACos(NiPoint3(0.0f, -1.0f, 0.0f).Dot(v));
	if (NiPoint3(0.0f, -1.0f, 0.0f).Cross(v).z > 0)
		fRot = -fRot;

	// 맞는넘의 어택 프로퍼티 등록
	CsC_AttackProp::sDAMAGE_INFO DInfo;
	DInfo.s_pHitter = pHitter;
	DInfo.s_eDamageType = pTargetInfo->s_DamageType;
	DInfo.s_nNumType = (pHitter->GetLeafRTTI() == RTTI_MONSTER || pHitter->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER) ? NUMTYPE::MONSTER_ATTACK : NUMTYPE::DIGIMON_ATTACK;
	DInfo.s_eActive = CsC_AttackProp::AT_NONE_ACTIVE;
	DInfo.s_nNumEffect = pTargetInfo->s_nNumEffectType;
	DInfo.s_vHitterPos = pHitter->GetPos2D();

	// 넉백이라면 가장 초기에 한번만 셋팅해주겠다
	if (pTargetInfo->s_DamageType == CsC_AttackProp::DT_KnockBack)
	{
		DInfo.s_eDamageType = CsC_AttackProp::DT_Normal;
	}
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		DInfo.s_eMarbleType |= CsC_AttackProp::MB_EVOL;
	}



	///////////////////////////////////////////////////////////////////////////////////////////////
	// 스킬 타격 횟수만큼 돌려서 하나씩 프로퍼티에 등록 예)짤싹때리기== 횟수 2번
	int nTotalValue[5];
	int nTotalDemage = 0;
	memcpy(nTotalValue, pTargetInfo->s_nValue, sizeof(int) * 5);
	for (int i = nEventCount - 1; i > -1; --i)
	{
		if (i == 0)
		{
			memcpy(DInfo.s_nValue, nTotalValue, sizeof(int) * 5);
			if (pTargetInfo->s_DamageType == CsC_AttackProp::DT_KnockBack)
			{
				DInfo.s_eDamageType = CsC_AttackProp::DT_KnockBack;
			}
		}
		else
		{
			// 랜덤 값으로 데미지 타격횟수로 나누어 준다. - 결과 값은 같게 만들자		
			float fCalDamageRate = 1.0f;
			(1.0f > (fMax - fMin)) ? fCalDamageRate = fMin * 0.01f : fCalDamageRate = (rand() % (int)(fMax - fMin) + fMin) * 0.01f;

			for (int t = 0; t < 5; ++t)
			{
				DInfo.s_nValue[t] = (int)(pTargetInfo->s_nValue[t] * fCalDamageRate);

				if (DInfo.s_nValue[t] < 0)
				{
					if (DInfo.s_nValue[t] < nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				else
				{
					if (DInfo.s_nValue[t] > nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				nTotalValue[t] -= DInfo.s_nValue[t];
			}
		}

		//토탈데미지 계산
		nTotalDemage += DInfo.s_nValue[0];

		// 랜덤 적용
		DInfo.s_fDamageRot = fRot + CsD2R(rand() % 70 - 35);

		DInfo.s_fHitEventTime = vpEventTime[i];
		DInfo.s_dwResistTime = GetTickCount() + (DWORD)(DInfo.s_fHitEventTime * 2000.0f);		// 틱이므로 1000 을 더 곱하자
		DInfo.s_pChildDamageInfo = pDamageInfo;


		// 프로퍼티 등록
		pDamageInfo = pTarget->GetProp_Attack()->InsertDamage(&DInfo);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////




	if (pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER)
	{
		if (g_pCharResMng->GetTargetMark() == NULL)
			g_pCharResMng->SetTargetMark(pHitter);
	}

	// 데미지 전투 로그
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_BATTLELOG_WINDOW, 0))
	{
		ContentsStream kStream;
		float nRemainTime = vpEventTime[nEventCount - 1];
		int nTotalDem = abs(nTotalDemage);

		kStream << pHitterInfo << pTargetInfo << nRemainTime << nTotalDem << bLastAttack;

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_BATTLE_PROCESS, &kStream);
	}

	//==============================================================================
	// 때린넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sHIT_INFO info;
	info.s_eHitType = pHitterInfo->s_HitType;
	info.s_dwAniID = dwAniID;
	info.s_fAniAccelRate = fAccelRate;
	info.s_pPartnerDamageInfo = pDamageInfo;
	info.s_pHitTarget = pTarget;
	info.s_fRangeVelocity = pFTSkillInfo->s_fSkill_Velocity;
	info.s_fRangeAccelVelocity = pFTSkillInfo->s_fSkill_Accel;
	info.s_bRangeAttack = pHitterInfo->s_bRangeAttack;
	info.s_bRotation = true;
	info.s_nAttackEffectIndex = 0;

	switch (g_pResist->m_Global.s_eAttackEffectViewMode)
	{
	case cResist::sGLOBAL::AEM_ALL_OFF:			// 랜더링 자체를 안하겠다
		info.s_nAttackEffectIndex = -1;
		break;
	case cResist::sGLOBAL::AEM_ONLY_NORATT_ON:	// 일반 타격이펙트만 랜더링
		info.s_nAttackEffectIndex = 0;
		break;
	case cResist::sGLOBAL::AEM_ALL_ON:
		info.s_nAttackEffectIndex = pHitter->IsStrongNature2(pTarget) ? pHitter->GetNatureType2() : 0;
		break;
	}

	info.s_vDamagePos = pTarget->GetPos();

	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		info.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
		info.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// 프로퍼티 등록
	pHitter->GetProp_Attack()->InsertHit(&info);

	///////////////////////////////////////////////////////////////////////////////////////////////

	pHitter->RecvSkill(true, pHitterInfo->s_nSkillIndex);

#ifdef DAMAGE_METER
	sDamageMeterInfo damageInfo(pHitterInfo->s_nUID, pTargetInfo->s_nUID, nTotalDemage);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_DAMAGEMETER_DATA, &damageInfo);
	// 	if( g_pDataMng->GetParty()->IsJoinParty() == true )
	// 	{
	// 		if( g_pGameIF->GetMainDigimonWindow()->IsBattle() || pHitterInfo->s_nUID == g_pCharMng->GetDigimonUser(0)->GetUniqID() )
	// 		{
	// 			cData_DamageMeter* pDamageMeter = g_pDataMng->GetDamageMeter();
	// 			if( pDamageMeter == NULL )
	// 				return;
	// 			pDamageMeter->_SetDamageInfo( pHitterInfo->s_nUID, pTargetInfo->s_nUID, abs(nTotalDemage), _TIME_TS );
	// 		}
	// 	}
#endif
}

void cCliGame::SetSkillInfo(sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, CsC_AvObject* pTargetMark, bool bLastAttack, bool& bMaxCount, bool bRecvComp)
{
	// 때린놈과 타겟을 얻는다.
	CsC_AvObject* pHitter = g_pMngCollector->GetObject(pHitterInfo->s_nUID);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(pTargetInfo->s_nUID);

	if (pHitter)
	{
		// 때리는놈 로드 되이않았으면?
		if (pHitter->IsLoad() == false)
		{
			// 때리는 놈이 내디지몬이면.
			if (pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER)
			{
				((CDigimonUser*)pHitter)->RecvSkill(false, pHitterInfo->s_nSkillIndex);
			}
			pHitter = NULL;
		}
	}

	// 타겟도 로드되지 않았으면...
	if (pTarget && (pTarget->IsLoad() == false))
		pTarget = NULL;

	//==============================================================================
	//	둘중에 하나라도 안보이면 패스 ??안보이면??
	//==============================================================================
	if ((pHitter == NULL) || (pTarget == NULL))
		return;

	assert_cs(pHitter->GetProp_Animation()->GetSequenceMng());
	assert_cs(pTarget->GetProp_Animation()->GetSequenceMng());

	// 때린놈에 대한 스킬정보를 얻는다.
	CsSkill::sINFO* pFTSkillInfo = NULL;

	switch (pHitter->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		if (((CDigimon*)pHitter)->GetSkillMng()->IsSkill(pHitterInfo->s_nSkillIndex) == false)
		{
			g_CriticalLog.Log(_T("SetSKillInfo_3 : 스킬테이블에 존재하지도 않는 스킬 ( model = %d, SkillIndex = %d )"), pHitter->GetModelID(), pHitterInfo->s_nSkillIndex);
			return;
		}
		if (pHitterInfo->s_nSkillIndex == -1)
			pFTSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill(pHitterInfo->s_nSkillCode)->GetInfo();
		else
			pFTSkillInfo = ((CDigimon*)pHitter)->GetSkillMng()->GetFTSkill(pHitterInfo->s_nSkillIndex)->GetInfo();
		break;
	default:
		assert_cs(false);
	}

	// 스킬 정보를 얻고 이것이 밀리냐 래인지냐? 검사.
	pHitterInfo->s_bRangeAttack = (pFTSkillInfo->s_nAttType == 2);

	//==============================================================================
	// 떄리는넘의 애니메이션 셋팅값을 구하자
	//==============================================================================

	DWORD dwAniID;
	if (pHitterInfo->s_AniID != 0)
	{
		dwAniID = pHitterInfo->s_AniID;

		if (pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER)
		{
			cSkill::sINFO* pSkill_Info = g_pCharMng->GetDigimonUser(0)->GetSkillMng()->GetSkill(pHitterInfo->s_nSkillIndex);	// 사용한 메모리 스킬 정보 얻어옴
			// 쿨타임 시작
			pSkill_Info->s_CoolTime.Start();
			pSkill_Info->s_CoolTime.SetEndTime(pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f);
		}
	}
	else
		dwAniID = ANI::ATTACK_SKILL1 + pHitterInfo->s_nSkillIndex;	// 스킬인덱스 계산

	int nEventCount = 1;
	CsVectorPB< int >* pEventArray;
	// 때리는 넘의 때리는 애니메이션 정보 얻는다.dwAniID 로 아웃 파라미터 2개 얻는다.
	if (pHitter->GetProp_Animation()->GetNAttEventIndex(dwAniID, &pEventArray, nEventCount) == false)
	{
		g_CriticalLog.Log(_T("SetSKillInfo : No Have Ani_3 ( model = %d, ani = %d, SkillIndex = %d )"), pHitter->GetModelID(), dwAniID, pHitterInfo->s_nSkillIndex);
		assert_csm2(false, L"ModelID = %d, LoadSeqModelID = %d", pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID());
		pHitter->RecvSkill(false, pHitterInfo->s_nSkillIndex);
		return;
	}

	assert_csm3(nEventCount > 0, L"AniID = %d, ModelID = %d, LoadSeqModelID = %d", dwAniID, pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID());

	CsVectorPB< float > vpEventTime(10);
	float fAccelRate = 1.0f;
	// 배틀 모드시 래이팅값설정
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		fAccelRate = BATTLE_SPEED_SKILL;

	// 이밴트 카운트 만큼만 
	for (int i = 0; i < nEventCount; ++i)
	{
		vpEventTime.PushBack(pHitter->GetProp_Animation()->CalEventTime(dwAniID, pEventArray->GetData(i), fAccelRate));
	}

	// 레인지 타입이라면
	if (pHitterInfo->s_bRangeAttack == true)
	{
		float fDist = (pHitter->GetPos2D() - pTarget->GetPos2D()).Length();

		float fHitTimeVelocity = pFTSkillInfo->s_fSkill_Velocity;
		float fHitTimeAccelVelocity = pFTSkillInfo->s_fSkill_Accel;
		if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		{
			fHitTimeVelocity *= BATTLE_SPEED_RANGE;
			fHitTimeAccelVelocity *= BATTLE_SPEED_RANGE;
		}

		float fRangeHitTime = CsC_AttackProp::CalRangeHitTime(fHitTimeVelocity, fHitTimeAccelVelocity, fDist);
		for (int i = 0; i < nEventCount; ++i)
		{
			vpEventTime[i] += fRangeHitTime;
		}
	}

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sDAMAGE_INFO* pDamageInfo = NULL;
	// 데미지 계산
	float fMin = 90.0f / (float)nEventCount;
	float fMax = 110.0f / (float)nEventCount;

	// 데미지 날라가는 방향 계산
	NiPoint3 v = pTarget->GetPos() - pHitter->GetPos();	// 타겟과 어택커와의 방향 계산
	v.z = 0.0f;
	v.Unitize();
	float fRot = NiACos(NiPoint3(0.0f, -1.0f, 0.0f).Dot(v));
	if (NiPoint3(0.0f, -1.0f, 0.0f).Cross(v).z > 0)
		fRot = -fRot;

	// 맞는넘의 어택 프로퍼티 등록
	CsC_AttackProp::sDAMAGE_INFO DInfo;
	DInfo.s_pHitter = pHitter;
	DInfo.s_eDamageType = pTargetInfo->s_DamageType;
	DInfo.s_nNumType = (pHitter->GetLeafRTTI() == RTTI_MONSTER || pHitter->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER) ? NUMTYPE::MONSTER_ATTACK : NUMTYPE::DIGIMON_ATTACK;
	DInfo.s_eActive = CsC_AttackProp::AT_NONE_ACTIVE;
	DInfo.s_nNumEffect = pTargetInfo->s_nNumEffectType;
	DInfo.s_vHitterPos = pHitter->GetPos2D();

	// 넉백이라면 가장 초기에 한번만 셋팅해주겠다
	if (pTargetInfo->s_DamageType == CsC_AttackProp::DT_KnockBack)
	{
		DInfo.s_eDamageType = CsC_AttackProp::DT_Normal;
	}
	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		DInfo.s_eMarbleType |= CsC_AttackProp::MB_EVOL;
	}



	///////////////////////////////////////////////////////////////////////////////////////////////
	// 스킬 타격 횟수만큼 돌려서 하나씩 프로퍼티에 등록 예)짤싹때리기== 횟수 2번
	int nTotalValue[5];
	int nTotalDemage = 0;
	memcpy(nTotalValue, pTargetInfo->s_nValue, sizeof(int) * 5);
	for (int i = nEventCount - 1; i > -1; --i)
	{
		if (i == 0)
		{
			memcpy(DInfo.s_nValue, nTotalValue, sizeof(int) * 5);
			if (pTargetInfo->s_DamageType == CsC_AttackProp::DT_KnockBack)
			{
				DInfo.s_eDamageType = CsC_AttackProp::DT_KnockBack;
			}
		}
		else
		{
			// 랜덤 값으로 데미지 타격횟수로 나누어 준다. - 결과 값은 같게 만들자		
			float fCalDamageRate = 1.0f;
			(1.0f > (fMax - fMin)) ? fCalDamageRate = fMin * 0.01f : fCalDamageRate = (rand() % (int)(fMax - fMin) + fMin) * 0.01f;

			for (int t = 0; t < 5; ++t)
			{
				DInfo.s_nValue[t] = (int)(pTargetInfo->s_nValue[t] * fCalDamageRate);

				if (DInfo.s_nValue[t] < 0)
				{
					if (DInfo.s_nValue[t] < nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				else
				{
					if (DInfo.s_nValue[t] > nTotalValue[t])
						DInfo.s_nValue[t] = nTotalValue[t];
				}
				nTotalValue[t] -= DInfo.s_nValue[t];
			}
		}

		//토탈데미지 계산
		nTotalDemage += DInfo.s_nValue[0];

		// 랜덤 적용
		DInfo.s_fDamageRot = fRot + CsD2R(rand() % 70 - 35);

		DInfo.s_fHitEventTime = vpEventTime[i];
		DInfo.s_dwResistTime = GetTickCount();// + (DWORD)( DInfo.s_fHitEventTime*2000.0f );		// 틱이므로 1000 을 더 곱하자
		DInfo.s_pChildDamageInfo = pDamageInfo;


		// 프로퍼티 등록
		pDamageInfo = pTarget->GetProp_Attack()->InsertDamage(&DInfo);

	}
	///////////////////////////////////////////////////////////////////////////////////////////////




	if (pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER)
	{
		if (g_pCharResMng->GetTargetMark() == NULL)
			g_pCharResMng->SetTargetMark(pHitter);
	}

	// 데미지 전투 로그
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_BATTLELOG_WINDOW, 0))
	{
		ContentsStream kStream;
		float nRemainTime = vpEventTime[nEventCount - 1];
		int nTotalDem = abs(nTotalDemage);

		kStream << pHitterInfo << pTargetInfo << nRemainTime << nTotalDem << bLastAttack;

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_BATTLE_PROCESS, &kStream);
	}

	//==============================================================================
	// 때린넘의 정보 만들자
	// hitter는 한명이기 때문에 hitter가 선택한 타겟과 damage를 받는 대상이 동일할때만 처리해야 한다.
	//==============================================================================

	if (pTargetMark == NULL)
		return;

	if (pTargetMark->GetUniqID() == pTarget->GetUniqID() && bMaxCount == false || bRecvComp == true)
	{
		CsC_AttackProp::sHIT_INFO info;

		//info.s_eHitType = CsC_AttackProp::HT_Skill;
		info.s_dwAniID = dwAniID;
		info.s_fAniAccelRate = fAccelRate;
		info.s_pPartnerDamageInfo = pDamageInfo;
		if (bRecvComp == true)	// 타겟 카운트 마지막까지 실제 스킬을 사용한 대상을 찾을 수 없을때(스킬을 사용한 대상이 범위에서 벗어났을때)
		{
			info.s_eHitType = CsC_AttackProp::HT_Miss;
			info.s_pHitTarget = pTargetMark;	// 디지몬 방향은 스킬을 사용한 대상을 바라본 상태로 스킬 애니를 구현하기 위함
		}
		else
		{
			info.s_eHitType = CsC_AttackProp::HT_Skill;
			info.s_pHitTarget = pTarget;
		}
		info.s_fRangeVelocity = pFTSkillInfo->s_fSkill_Velocity;
		info.s_fRangeAccelVelocity = pFTSkillInfo->s_fSkill_Accel;
		info.s_bRangeAttack = pHitterInfo->s_bRangeAttack;
		info.s_bRotation = true;
		info.s_nAttackEffectIndex = 0;

		switch (g_pResist->m_Global.s_eAttackEffectViewMode)
		{
		case cResist::sGLOBAL::AEM_ALL_OFF:			// 랜더링 자체를 안하겠다
			info.s_nAttackEffectIndex = -1;
			break;
		case cResist::sGLOBAL::AEM_ONLY_NORATT_ON:	// 일반 타격이펙트만 랜더링
			info.s_nAttackEffectIndex = 0;
			break;
		case cResist::sGLOBAL::AEM_ALL_ON:
			info.s_nAttackEffectIndex = pHitter->IsStrongNature2(pTarget) ? pHitter->GetNatureType2() : 0;
			break;
		}
		// 		if( pTargetMark == NULL )
		// 			info.s_vDamagePos = pTarget->GetPos();
		// 		else
		info.s_vDamagePos = pTargetMark->GetPos();

		if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
		{
			info.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
			info.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
		}

		bMaxCount = true;

		// 메모리 스킬이라면 광역 이펙트를 한번 붙여준다.
		if (pFTSkillInfo->s_nMemorySkill)
			pHitter->GetProp_Effect()->AddEffect_FT("system\\Buff\\Memory_Buff_Attack_Skill_2.nif"/*, 1.0f, nsEFFECT::POS_CHARPOS*/);

		///////////////////////////////////////////////////////////////////////////////////////////////
		// 프로퍼티 등록
		pHitter->GetProp_Attack()->InsertHit(&info);

		///////////////////////////////////////////////////////////////////////////////////////////////

		/*if( bRecvComplete == false )*/
		pHitter->RecvSkill(true, pHitterInfo->s_nSkillIndex);
	}

#ifdef DAMAGE_METER
	sDamageMeterInfo damageInfo(pHitterInfo->s_nUID, pTargetInfo->s_nUID, nTotalDemage);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_DAMAGEMETER_DATA, &damageInfo);
#endif
}

//===============================================================================================
//
// 도트 스킬정보 설정
//
//===============================================================================================
void cCliGame::SetDotInfo(sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, int nBuffCode, bool bLastAttack)
{
	CsC_AvObject* pHitter = g_pMngCollector->GetObject(pHitterInfo->s_nUID);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(pTargetInfo->s_nUID);

	if (pHitter && (pHitter->IsLoad() == false))
		pHitter = NULL;

	if (pTarget && (pTarget->IsLoad() == false))
		pTarget = NULL;

	//==============================================================================
	//	타겟은 반드시 존재해야함
	//==============================================================================
	if (pTarget == NULL)
		return;

	assert_cs(pTarget->GetProp_Animation()->GetSequenceMng());

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================	
	CsC_AttackProp::sDAMAGE_INFO DInfo;
	DInfo.s_pHitter = pHitter;
	DInfo.s_eDamageType = pTargetInfo->s_DamageType;
	DInfo.s_nNumType = (pTarget->GetLeafRTTI() == RTTI_MONSTER || pTarget->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER) ? NUMTYPE::DIGIMON_ATTACK : NUMTYPE::MONSTER_ATTACK;
	DInfo.s_eActive = CsC_AttackProp::AT_ACTIVE;
	DInfo.s_nNumEffect = pTargetInfo->s_nNumEffectType;
	DInfo.s_eMarbleType = CsC_AttackProp::MB_NONE;
	memcpy(DInfo.s_nValue, pTargetInfo->s_nValue, sizeof(int) * 5);

	// 랜덤 적용
	DInfo.s_fDamageRot = CsD2R(rand() % 700 - 350);
	DInfo.s_fHitEventTime = 0.0f;
	DInfo.s_dwResistTime = nBuffCode;	// 도트류는 시간없이 바로 나오므로 이값을 버프 번호로쓴다!!
	DInfo.s_pChildDamageInfo = NULL;

	pTarget->GetProp_Attack()->InsertDamage(&DInfo);

	//토탈데미지 계산	
	int nTotalDemage = 0;
	nTotalDemage += DInfo.s_nValue[0];

	// 데미지 전투 로그
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_BATTLELOG_WINDOW, 0))
	{
		ContentsStream kStream;
		float nRemainTime = 0.0f;
		int nTotalDem = abs(nTotalDemage);

		kStream << pHitterInfo << pTargetInfo << nRemainTime << nTotalDem << bLastAttack;

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_BATTLE_PROCESS, &kStream);
	}

#ifdef DAMAGE_METER
	sDamageMeterInfo damageInfo(pHitterInfo->s_nUID, pTargetInfo->s_nUID, nTotalDemage);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_DAMAGEMETER_DATA, &damageInfo);
	// 	if( g_pDataMng->GetParty()->IsJoinParty() == true )
	// 	{
	// 		if( g_pGameIF->GetMainDigimonWindow()->IsBattle() ||  pHitterInfo->s_nUID == g_pCharMng->GetDigimonUser(0)->GetUniqID() )
	// 		{
	// 			cData_DamageMeter* pDamageMeter = g_pDataMng->GetDamageMeter();
	// 			if( pDamageMeter == NULL )
	// 				return;
	// 			pDamageMeter->_SetDamageInfo( pHitterInfo->s_nUID, pTargetInfo->s_nUID, abs(nTotalDemage), _TIME_TS );
	// 		}
	// 	}
#endif
}


//===============================================================================================
//
// 죽이는 정보 설정
//
//===============================================================================================
void cCliGame::SetDieInfo(CsC_AvObject* pTarget)
{
	// 	CHAT_TEXT_DEGUG( _T("SetDieInfo UID : %d"), pTarget->GetUniqID() );
	pTarget->GetProp_Attack()->InsertDamageEnd();
	pTarget->SetServerDie(true);
	BHPRT("Recv SetDieInfo ID : %d, UIDX : %d", pTarget->GetIDX(), pTarget->GetUniqID());
}

void cCliGame::RecvStatChange()
{
	u4 nUID = 0;
	pop(nUID);
	CsC_AvObject* pObject = g_pMngCollector->GetObject(nUID);
	if (pObject == NULL)
		return;

	u2 nChangeCount = 0;
	pop(nChangeCount);

	CsC_AvObject::sBASE_STAT* pBaseStat = pObject->GetBaseStat();
	if (pBaseStat)
	{
		for (int i = 0; i < nChangeCount; i++)
		{
			u2 nType = 0;
			u4 nValue = 0;
			pop(nType);		// 변경된 파라미터 타입 Skill_Code 의 항목 A 값
			pop(nValue);	// 변경된 값

			switch (nType)
			{
			case APPLY_MS:		pBaseStat->SetMoveSpeed((float)nValue);	break;
			case APPLY_HP:		pBaseStat->SetHP(nValue);					break;
			case APPLY_DS:		pBaseStat->SetDS(nValue);					break;
			case APPLY_MAXHP:	pBaseStat->SetMaxHP(nValue);				break;
			case APPLY_MAXDS:	pBaseStat->SetMaxDS(nValue);				break;
			case APPLY_AP:		pBaseStat->SetAtt(nValue);				break;
			case APPLY_DP:		pBaseStat->SetDef(nValue);				break;
			case APPLY_AS:		pBaseStat->SetAttackSpeed(nValue);		break;
			case APPLY_CA:		pBaseStat->SetCritical(nValue);			break;
			case APPLY_EV:		pBaseStat->SetEvade(nValue);				break;
			case APPLY_HT:		pBaseStat->SetHitRate(nValue);			break;
			case APPLY_AR:
			case APPLY_FP:
			case APPLY_FS:
			case APPLY_EXP:
			case APPLY_POWERAPPLYRATE:
			case APPLY_BL:
			case APPLY_DA:
			case APPLY_ER:
			case APPLY_AllParam:
			case APPLY_SER:
			case APPLY_SDR:
			case APPLY_SRR:
			case APPLY_SCD:
			case APPLY_SCR:
			case APPLY_HRR:
			case APPLY_DRR:
			case APPLY_MDA:
			case APPLY_HR:
				break;
			default:
				assert_csm(false, L"정의 되지 않은 스탯값임 pParameter참조!!")
					break;
			}
		}
		if (RTTI_TAMER_USER == pObject->GetLeafRTTI())
			GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);
		if (RTTI_DIGIMON_USER == pObject->GetLeafRTTI())
			GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL);
	}
}

void cCliGame::RecvAllStat(void)
{
	SAFE_POINTER_RET(g_pCharMng);
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET(pDUser);

	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pTUser);

	// 자기 자신 ( 테이머 + 디지몬 )
	n4 nT_MaxHP = 0;
	n4 nT_MaxDS = 0;
	u2 nT_AP = 0;
	u2 nT_DP = 0;
	n4 nT_HP = 0;
	n4 nT_DS = 0;
	u2 nT_MS = 0;

	pop(nT_MaxHP);
	pop(nT_MaxDS);
	pop(nT_HP);
	pop(nT_DS);
	pop(nT_AP);
	pop(nT_DP);
	pop(nT_MS);

	CTamerUser::sUSER_STAT* pTBase = dynamic_cast<CTamerUser::sUSER_STAT*>(pTUser->GetBaseStat());
	if (pTBase && pTUser->GetLeafRTTI() == RTTI_TAMER_USER)
	{
		pTBase->SetMaxHP(nT_MaxHP);
		pTBase->SetMaxDS(nT_MaxDS);
		pTBase->SetHP(nT_HP);
		pTBase->SetDS(nT_DS);
		pTBase->SetAtt(nT_AP);
		pTBase->SetDef(nT_DP);
	}

	n4 nD_MaxHP = 0;
	n4 nD_MaxDS = 0;
	n4 nD_HP = 0;
	n4 nD_DS = 0;
	u2 nD_FS = 0;	// friend fs

	u2 nD_AP = 0;
	u2 nD_DP = 0;
	u2 nD_CA = 0;
	u2 nD_AS = 0;
	u2 nD_EV = 0;
	u2 nD_HT = 0;
	u2 nD_AR = 0;
	u2 nD_BL = 0;

	pop(nD_MaxHP);
	pop(nD_MaxDS);
	pop(nD_HP);
	pop(nD_DS);
	pop(nD_FS);	// 친밀도

	pop(nD_AP);
	pop(nD_DP);
	pop(nD_CA);

	pop(nD_AS);
	pop(nD_EV);
	pop(nD_HT);

	pop(nD_AR);
	pop(nD_BL);

	u2 nInchantLevel = 0;
	pop(nInchantLevel);

	u2 nExtentionParameter[nExtendState::MaxExtendStat] = { 0, };
	pop(nExtentionParameter);

	u2 nExtentionParameterLV[nExtendState::MaxExtendStat] = { 0, };
	pop(nExtentionParameterLV[nExtendState::ET_AT]);
	pop(nExtentionParameterLV[nExtendState::ET_BL]);
	pop(nExtentionParameterLV[nExtendState::ET_CR]);
	pop(nExtentionParameterLV[nExtendState::ET_AS]);
	pop(nExtentionParameterLV[nExtendState::ET_EV]);
	pop(nExtentionParameterLV[nExtendState::ET_HT]);
	pop(nExtentionParameterLV[nExtendState::ET_HP]);

	CDigimonUser::sUSER_STAT* pDBase = dynamic_cast<CDigimonUser::sUSER_STAT*>(pDUser->GetBaseStat());
	if (pDBase && pDUser->GetLeafRTTI() == RTTI_DIGIMON_USER)
	{
		pDBase->SetMaxHP(nD_MaxHP);
		pDBase->SetMaxDS(nD_MaxDS);
		pDBase->SetHP(nD_HP);
		pDBase->SetDS(nD_DS);
		pDBase->SetFriendShip(nD_FS);
		pDBase->SetAtt(nD_AP);
		pDBase->SetDef(nD_DP);
		pDBase->SetCritical(nD_CA);
		pDBase->SetAttackSpeed(nD_AS);
		pDBase->SetEvade(nD_EV);
		pDBase->SetHitRate(nD_HT);
		pDBase->SetAttRange(nD_AR);
		pDBase->SetBL(nD_BL);

		CDigimon::sENCHENT_STAT* pEStat = ((CDigimon*)g_pCharMng->GetDigimonUser(0))->GetEnchantStat();
		pEStat->SetEnchantLevel(nInchantLevel);

		for (int i = nExtendState::ET_AT; i < nExtendState::MaxExtendStat; ++i)
		{
			pEStat->SetEnchantInfo(i, nExtentionParameterLV[i], nExtentionParameter[i]);
		}
		GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);
		GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL);
	}
}


// 게임 시작시 처음 메세지(인사말, 공지등)
void cCliGame::RecvStartMessage(void)
{
	u1 nMsgCnt = 0;
	char msg[256];

	pop(nMsgCnt);

	for (uint i = 0; i < nMsgCnt; i++)
	{
		pop(msg);
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = nBase::m2w(msg);
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
}


void cCliGame::RecvItemMoveFailure(void)
{
	u2 nSrcPos;
	u2 nDstPos;

	pop(nSrcPos);
	pop(nDstPos);

	g_pDataMng->ServerItemMoveFailed(nSrcPos, nDstPos);
}

void cCliGame::RecvChangeItemLimitedAttribute(void)
{
	u2 nItemPos;
	u1 nLimited;

	pop(nItemPos);
	pop(nLimited);

	g_pDataMng->SetChangeItemLimited(nItemPos, nLimited);
}


void cCliGame::RecvItemMoveSuccess(void)
{
	RecvItemMove();
}


void cCliGame::RecvItemUseSuccess(void)
{
	u4 nTargetUID = 0;
	n2 nInvenPos = 0;

	pop(nTargetUID);
	pop(nInvenPos);

	g_pDataMng->ServerItemUseSuccess(nTargetUID, nInvenPos);
}
#ifdef CROSSWARS_SYSTEM
void cCliGame::RecvCrossItemUseSuccess(void)
{
	u4 nTargetUID = 0;
	n2 nInvenPos = 0;

	pop(nTargetUID);
	pop(nInvenPos);

	g_pDataMng->ServerCrossItemUseSuccess(nTargetUID, TO_ID(nInvenPos));
	GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);
}
void cCliGame::RecvCrossItemUseFailure(void)
{
	u2 nInvenPos;
	u4 nItemType;
	u4 nErrCode;

	pop(nInvenPos);
	pop(nItemType);
	pop(nErrCode);

	switch (nErrCode)
	{
	case nItem::USE_FAIL_NOT_USE_AREA:	// 사용 불가능 위치입니다.
		cPrintMsg::PrintMsg(30336);
		break;
	case nItem::USE_FAIL_BATTLE:	// 전투 상태로 인해 사용 불가능
		cPrintMsg::PrintMsg(16002);
		break;
	case nItem::USE_FAIL_COOLDOWN:	// 재사용 시간이 아직 남았습니다.
		// 쿨타임 중입니다.
		cPrintMsg::PrintMsg(13002);
		break;

	case nItem::USE_FAIL_NOT_USE_TERMS:
		cPrintMsg::PrintMsg(13002);
		break;
	default:	// error
		break;
	}

	g_pDataMng->ItemUnlock(nInvenPos);
}
void cCliGame::RecvCrossInvenSlotSize(void)
{

	u2 nSlotSize;
	pop(nSlotSize);

	g_pDataMng->GetInvenCross()->SetInvenSlotCount(nSlotSize);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================


}
#endif
void cCliGame::RecvItemUseFailure(void)
{
	u2 nInvenPos;
	u4 nItemType;
	u4 nErrCode;

	pop(nInvenPos);
	pop(nItemType);
	pop(nErrCode);

	switch (nErrCode)
	{
	case nItem::USE_FAIL_EXPIRE:	// 시간 제한 종료로 아이템 사용 불가능
		cPrintMsg::PrintMsg(20029, nsCsFileTable::g_pItemMng->GetItem(nItemType)->GetInfo()->s_szName);
		break;
	case nItem::USE_FAIL_BATTLE:	// 전투 상태로 인해 사용 불가능
		cPrintMsg::PrintMsg(16002);
		break;
	case nItem::USE_FAIL_COOLDOWN:	// 재사용 시간이 아직 남았습니다.
		// 쿨타임 중입니다.
		cPrintMsg::PrintMsg(13002);
		break;
	case nItem::USE_FAIL_NOT_USE_AREA:	// 사용 불가능 위치입니다.
		cPrintMsg::PrintMsg(30336);
		break;
	case nItem::USE_FAIL_NON_TARGET:	// 대상이 선택되지 아니하였습니다.
		cPrintMsg::PrintMsg(16001);
		break;
	case nItem::USE_FAIL_NOT_MONSTER:	// 대상이 몬스터가 아닙니다.
		cPrintMsg::PrintMsg(17004);
		break;
	case nItem::USE_FAIL_NOT_RANGE:	// 대상이 너무 멀리있습니다.
		cPrintMsg::PrintMsg(17005);
		break;
	case nItem::USE_FAIL_NOT_USE_TERMS:
		cPrintMsg::PrintMsg(11015);
		break;
	case nItem::USE_FAIL_LIMIT:
		cPrintMsg::PrintMsg(30505);	// 한번만(디지몬) 사용 가능한 아이템입니다.
		break;
	case nItem::USE_FAIL_NOT_USE_TARGET: // 사용할 수 없는 대상 입니다.
		break;
#ifdef ADVENTURE_TAMER_CHANGE
	case nItem::USE_FAIL_NOT_INVEN:		//테이머 체인지 아이템 사용시 인벤이 모자랄때
		cPrintMsg::PrintMsg(11015);
		break;
#endif
	case nItem::USE_FAIL_NOT_SAME:		// 동일한 아이템 적용 불가
		cPrintMsg::PrintMsg(30585);
		break;
	case nItem::USE_FAIL_NOT_USE_SCAN:
	{
		// 강제 캔슬
		UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_CLOSE, &nScanWT);
		cPrintMsg::PrintMsg(11002);	//아이템 스캔을 하였는데 Item_Rank에 스캔 결과 아이템 값이 없어서 아무일도 일어나지 않는 경우
	}
	break;
	case nItem::USE_FAIL_SPRIT_EXP:
		cPrintMsg::PrintMsg(30705);	//스피릿 부스터(경험치 획득 아이템임)를 사용 시 하이브리드체 디지몬(타입 10번)을 소환한 상태가 아닐 경우 아이템 사용 실패 메시지
		break;

	case nItem::USE_FAIL_MAX_LIMIT:
		cPrintMsg::PrintMsg(30338);
		break;

	case nItem::USE_FAIL_NO_SKILL:		// 스킬이 없다
		cPrintMsg::PrintMsg(11010);	// 아이템 기능이 연결되어 있지 않습니다.
		break;

	case nItem::USE_FAIL_OTHERS:		// 실패
		cPrintMsg::PrintMsg(11009);	// 아이템을 사용할 수 없습니다.
		break;

	default:	// error
		break;
	}

	g_pDataMng->ServerItemUseFailed(nInvenPos);

}

void cCliGame::RecvChangeTamer(void)
{
#ifdef ADVENTURE_TAMER_CHANGE

	cPrintMsg::PrintMsg(30181);

	g_pCharMng->DelTamer();

	cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();
#ifdef CROSSWARS_SYSTEM
	cData_PostLoad::sTAMERDATA* pTamerData = pPostLoad->GetTamerData();
#else
	cData_PostLoad::sDATA* pTamerData = pPostLoad->GetTamerData();
#endif

	pop(pTamerData->s_Type);

	nSync::Pos pos;
	pop(pos);
	pPostLoad->SetPos(pos);

#ifdef SDM_TAMER_SERVERRELOCATE_20170911
	u4 nServerRelocateCompleteTime = 0;
	pop(nServerRelocateCompleteTime);
	pPostLoad->SetServerRelocateCompletedTime(nServerRelocateCompleteTime);
#endif

	TCHAR szName[Language::pLength::name + 1];
	pop(szName);
	_tcscpy_s(pTamerData->s_szName, Language::pLength::name + 1, szName);

	pop(pTamerData->s_nExp);
	pTamerData->s_nExp = pTamerData->s_nExp / 100;
	pop(pTamerData->s_nLevel);

	pop(pTamerData->s_Attribute[MHP]);
	pop(pTamerData->s_Attribute[MDS]);
	pop(pTamerData->s_Attribute[HP]);
	pop(pTamerData->s_Attribute[DS]);
	pop(pTamerData->s_Attribute[FP]);
	pop(pTamerData->s_Attribute[AP]);
	pop(pTamerData->s_Attribute[DP]);
	pop(pTamerData->s_Attribute[MS]);
#ifdef CROSSWARS_SYSTEM
	pop(pTamerData->s_MCR);
	pop(pTamerData->s_CR);
#endif

	u4 nClientOption = 0;
	pop(nClientOption);

	u1 nCashCount;					// 테이머 캐쉬 스킬 개수
	pop(nCashCount);
	assert_cs(nCashCount < 3);	// 현재 캐쉬 스킬 개수는 2를 넘을 수 없다

	for (int i = 0; i < nCashCount; ++i)
	{
		pop(pTamerData->s_nTamerCashSkill_Index[i]);		// 캐쉬 스킬 인덱스 (Tamer_Skill.xls 의 인덱스 값)
		pop(pTamerData->s_nTamerCashSkill_LifeTime[i]);	// 기간 만료까지 시간
		pop(pTamerData->s_nTamerCashSkill_CoolTime[i]);	// 쿨 타임
	}

	g_pDataMng->GetPostLoad()->PostLoadT();

	u2 cnt;
	pop(cnt);

	nSync::Pos SyncPos;
	cType type;
	type = pTamerData->s_Type;
	while (cnt)
	{
		pop(SyncPos);		//위치를 받고
		pop(type);		//타입을 받아서

		switch (type.m_nClass)			//어떤 녀석인지 구분하자
		{
		case nClass::Tamer: SyncInTamer(SyncPos, type);			break;
		case nClass::Digimon: SyncInDigimon(SyncPos, type);			break;
		}
		--cnt;
	}
#else
	cPrintMsg::PrintMsg(30181);
#endif
	// 추가 퀵슬롯에 스킬 들어가있으면 빼줘야함
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_QUICKSLOT, cQuickSlot::Q_ADDSLOT))
	{
		g_pGameIF->GetQuickSlot(cQuickSlot::Q_ADDSLOT)->_DeleteSkill_AddSlot(RTTI_TAMER_USER);
	}
}
#ifdef ADVENTURE_TAMER_CHANGE
void cCliGame::RecvChangeTamerAround(void)		//주변에 있는 녀석들용
{
	u4 nUID = 0;
	nSync::Pos SyncPos;
	cType type;

	for (int i = 0; i < 2; ++i)
	{
		pop(nUID);

		type.m_nUID = nUID;

		switch (GetClass(nUID))
		{
		case nClass::Digimon:	g_pCharMng->DeleteDigimon(type.m_nIDX);								break;
		case nClass::Tamer:	g_pCharMng->DeleteTamer(type.m_nIDX);								break;
		default:						xassert1(false, "unknown uid(%d)", nUID);
		}
	}

	for (int j = 0; j < 2; ++j)
	{
		pop(SyncPos);		//위치를 받고
		pop(type);		//타입을 받아서

		switch (type.m_nClass)			//어떤 녀석인지 구분하자
		{
		case nClass::Tamer: SyncInTamer(SyncPos, type);			break;
		case nClass::Digimon: SyncInDigimon(SyncPos, type);			break;
		}
	}

}
#endif
void cCliGame::RecvCapsuleToItem(void)
{
	u4 nItemType;
	u1 nItemRate;
	u2 nOverlapItemCount;
	u4 nRemainTradeLimitTime = 0;

	pop(nItemType);
	while (nItemType)
	{
		pop(nItemRate);
		pop(nOverlapItemCount);
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		pop(nRemainTradeLimitTime);
#endif

		assert_csm(nOverlapItemCount == 1, _T("한번에 하나의 캡슐만 얻어야 한다."));

		cItemData data;
		data.m_nType = nItemType;
		data.m_nRate = nItemRate;
		data.m_nCount = nOverlapItemCount;
		data.m_nLevel = 1;
		data.m_nRemainTradeLimitTime = nRemainTradeLimitTime;

		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_CAPSULE_COIN_SERVER, 0, &data);
		pop(nItemType);
		assert_csm(nItemType == 0, _T("코인 하나에 캡슐은 하나여야한다."));
	}

#ifdef TAMER_IDLEANIMATION
	// 테이머가 자리 비움 상태이면 일어나야 한다.
	switch (g_pCharMng->GetTamerUser()->GetProp_Animation()->GetAnimationID())
	{
	case ANI::IDLE_SIT:
	case ANI::IDLE_SIT_DOWN:
		g_pCharMng->GetTamerUser()->SetPause(CsC_AvObject::PAUSE_ANIMATION, false);
		g_pCharMng->GetTamerUser()->SetAnimation(ANI::IDLE_SIT_UP);
		break;
	}
#endif
}

#ifdef UI_ITEMSCAN_RENEWAL
void cCliGame::RecvDigitamaScanResult(void)
{
	GS2C_RECV_ITEMSCAN recv;

	pop(recv.nResult);
	pop(recv.n8ScanMoney);	// 스캔 총 비용
	pop(recv.n8NowMoney);		// 스캔 후 돈
	pop(recv.uBeforeScanPos);
	pop(recv.uBeforeScanType);
	pop(recv.u2BeforeScanCount);	// 실패 시 0

	// 획득한 아이템 종류 개수 - 실패 시 0
	pop(recv.uItemCount);
	for (int i = 0; i < recv.uItemCount; ++i)
	{
		nsItemScan::sScanInfo sInfo;
		pop(sInfo.uInvenIdx);
		pop(sInfo.itemData);
		recv.lScanList.push_back(sInfo);
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_SCAN_ADD_DATA, &recv);
}
#else
void cCliGame::RecvDigitamaScanResult(void)
{
	GS2C_RECV_DIGITAMA_SCANITEM recv;

	while (1)
	{
		u4 nItemType = 0;
		pop(nItemType);

		if (0 == nItemType)
			break;
		u1 nItemRate = 0;
		pop(nItemRate);
		u2 nItemCount = 0;
		pop(nItemCount);
		u4 nRemainTradeLimitTime = 0;
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		pop(nRemainTradeLimitTime);
#endif
		cItemInfo data;
		data.Set(nItemType, nItemCount, nItemRate, 1);
		data.m_nRemainTradeLimitTime = nRemainTradeLimitTime;
		recv.m_ItemList.push_back(data);
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_SCAN_ADD_DATA, &recv);
}
#endif

void cCliGame::RecvHatchUpSuccess(void)
{
	u4 nTamerUID = 0;
	pop(nTamerUID);

	// 용병 부화용 데이타 입력 결과 - 성공
	u1 nHatchLevel = 0;
	pop(nHatchLevel); // 데이타 입력 성공후 레벨
	assert(nHatchLevel <= 5);
#pragma todo("용병 부화 단수 MAX 수치 조절")	

	CsC_AvObject* pTamer = g_pMngCollector->GetObject(nTamerUID);
	if (pTamer == NULL)
		return;

	switch (nHatchLevel)
	{
	case 3:
	case 4:
	case 5:
		if (pTamer->GetLeafRTTI() != RTTI_TAMER_USER)
		{
			pTamer->SetAnimation(ANI::NPC_YES);
			pTamer->GetProp_Effect()->AddEffect("System\\Tactics_success.nif", 1.0f, nsEFFECT::POS_CHARPOS);
		}
		break;
	}

	if (pTamer->GetLeafRTTI() != RTTI_TAMER_USER)
		return;

	// 사운드는 나만
	switch (nHatchLevel)
	{
	case 3:
	case 4:
	case 5:
		pTamer->PlaySound("system\\Tactics_yes.wav");
		break;
	}

	GS2C_RECV_MAKE_DIGITAMA_SCLV recv;
	recv.m_nSuccessLevel = nHatchLevel;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::MAKETACTICS_DIGITAMA_SC_LV, &recv);

	if (g_pGameIF && g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAKE_TACTICS))
	{
		g_pGameIF->GetMakeTactics()->RecvResult(cMakeTactics::SUCCESS);
		g_pGameIF->GetMakeTactics()->RecvServer();
	}
	else
	{
		recv.m_nSuccessLevel = -1;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::MAKETACTICS_DIGITAMA_SC_LV, &recv);
	}
}


void cCliGame::RecvHatchUpFailure(void)
{
	u4 nTamerUID = 0;
	pop(nTamerUID);

	u1 nType = 0;
	pop(nType);

	CsC_AvObject* pTamer = g_pMngCollector->GetObject(nTamerUID);
	if (pTamer == NULL)
		return;

	if (nType == 0)
	{
		if (pTamer->GetLeafRTTI() != RTTI_TAMER_USER)
		{
			pTamer->SetAnimation(ANI::NPC_NO);
			pTamer->GetProp_Effect()->AddEffect("System\\Tactics_fail.nif", 1.0f, nsEFFECT::POS_CHARPOS);
		}
	}

	if (pTamer->GetLeafRTTI() != RTTI_TAMER_USER)
		return;

	switch (nType)
	{
	case 0:
		if (g_pGameIF && g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAKE_TACTICS))
		{
			g_pGameIF->GetMakeTactics()->RecvResult(cMakeTactics::DEL_DIGITAMA);
			g_pGameIF->GetMakeTactics()->RecvServer();
		}
		else
		{
			cPrintMsg::PrintMsg(20015);

			GS2C_RECV_MAKE_DIGITAMA recv;
			recv.m_nEggType = 0;
			recv.m_nEggTradeLimitTime = 0;
			recv.m_nEggLevel = 0;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::MAKETACTICS_DIGITAMA_DATA, &recv);
		}
		break;
	case 1:
		if (g_pGameIF && g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAKE_TACTICS))
		{
			g_pGameIF->GetMakeTactics()->RecvResult(cMakeTactics::DEL_DATA);
			g_pGameIF->GetMakeTactics()->RecvServer();
		}
		else
		{
			cPrintMsg::PrintMsg(20016);
		}
		break;
	case 2:
		if (g_pGameIF && g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAKE_TACTICS))
		{
			g_pGameIF->GetMakeTactics()->RecvResult(cMakeTactics::DEL_BACKUP);
			g_pGameIF->GetMakeTactics()->RecvServer();
		}
		else
		{
			cPrintMsg::PrintMsg(30503);
#ifdef MINIGAME
			GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT kRecv;
			kRecv.nResult = false;
			kRecv.nSuccessCnt = 0;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_MINIGAME_INIT, &kRecv);
#endif
			GS2C_RECV_MAKE_DIGITAMA_LV recv;
			recv.m_nEggLevel = 0;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::MAKETACTICS_DIGITAMA_LV, &recv);
		}
		break;
	}
}

void cCliGame::RecvHatchBackupDisk(void)
{

}

void cCliGame::RecvHatchBackupDiskCancel(void)
{

}

void cCliGame::RecvHatchIn(void)	// 부화 디지타마 등록 결과
{
	// 실패시 연결 종료,
	// 성공시 그냥 진행, 결과가 없어도 무관할듯...
}

#ifdef CROSSWARS_SYSTEM
void cCliGame::RecvCrossWarsHatch(void)	// 부화 확인 결과
{
	u1 nArrIDX;
	pop(nArrIDX);

	cData_CrossTactics::sCross Tactics;

	pop(Tactics.s_nEnable);
	pop(Tactics.s_Type);
	std::wstring digimonName;
	pop(digimonName);

	wmemcpy_s(Tactics.s_szName, sizeof(Tactics.s_szName), digimonName.c_str(), digimonName.length());

	u2 nScale;
	pop(nScale);
	Tactics.s_fScale = nScale * 0.0001f;
	pop(Tactics.s_nExp);
	Tactics.s_nExp = Tactics.s_nExp / 100;

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
	pop(Tactics.s_nTranscendenceExp);
#endif

	pop(Tactics.s_nLevel);
	pop(Tactics.s_Attribute, sizeof(Tactics.s_Attribute));

	pop(Tactics.s_dwBaseDigimonID);
	DBG("nBaseEvoUnitIDX : %d", Tactics.s_dwBaseDigimonID);

	pop(Tactics.s_nMaxEvoUnit);
	pop(&Tactics.s_EvoUnit[1], sizeof(cEvoUnit) * Tactics.s_nMaxEvoUnit);

	memset(&Tactics.s_AttributeExp, NULL, sizeof(n2) * NewAttribute::MaxDigitalType);
	memset(&Tactics.s_NatureExp, NULL, sizeof(n2) * NewAttribute::MaxNatualType);

	u2 nInchantLevel = 0;
	u2 nExtentionParameter[6] = { 0, };
	pop(Tactics.s_nEnchantLevel);
	pop(Tactics.s_ExtendAttribute, sizeof(Tactics.s_ExtendAttribute));
	pop(Tactics.s_ExtendAttributeLV, sizeof(Tactics.s_ExtendAttributeLV));

	g_pDataMng->GetCrossTactics()->AddTactics(&Tactics, nArrIDX);

	g_pTacticsAni->Set_bAniProcess(true);	// 부화 애니 시작위치 설정
	g_pTacticsAni->Start(Tactics.s_Type.m_nType, g_pCharMng->GetTamerUser());	// 부화 애니메이션 활성화	

	GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);
	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	//g_pDataMng->GetAchieve()->CheckType( cData_Achieve::CA_GetTactics, Tactics.s_Type.m_nType );

}

void cCliGame::RecvCrossWarsEnergize(void)
{
	// 크로스용병이 이미 있는경우. 로더에 활성화
	u1 nLoaderIdx;
	u1 nEnable;

	pop(nLoaderIdx);
	pop(nEnable);

	g_pDataMng->GetCrossTactics()->GetTactics(nLoaderIdx)->s_nEnable = nEnable;
}

void cCliGame::RecvCrossNotReg(void)
{
	u4 nTargetUID = 0;
	n2 nInvenPos = 0;

	pop(nTargetUID);
	pop(nInvenPos);

	//g_pDataMng->ServerCrossItemUseSuccess( nTargetUID, TO_ID(nInvenPos) );

	CsPoint pos(-1, -1);
	pos = CURSOR_ST.GetPos();
	cCreateName::DeleteInstance(cBaseWindow::WT_CHANGE_T_NAME, 0);
	cCreateName::DeleteInstance(cBaseWindow::WT_CHANGE_D_NAME, 0);
	cCreateName::DeleteInstance(cBaseWindow::WT_MAKE_TACTICS, 0);
	cCreateName::DeleteInstance(cBaseWindow::WT_MAKE_CROSSTACTICS, 0);
	cCreateName* pCName = cCreateName::NewInstance(cBaseWindow::WT_MAKE_CROSSTACTICS, 0, pos);
	if (pCName)
		pCName->SetValue1(nInvenPos);

	//g_pDataMng->ItemUnlock(  nInvenPos  );	


}
#endif

void cCliGame::RecvHatchOut(void)	// 부화 확인 결과
{
	u4 nArrIDX;
	pop(nArrIDX);

	cData_PostLoad::sDATA Tactics;

	pop(Tactics.s_Type);

	std::wstring szTempName;
	pop(szTempName);

	_tcscpy_s(Tactics.s_szName, szTempName.c_str());

	u2 nScale;
	pop(nScale);
	Tactics.s_fScale = nScale * 0.0001f;
	pop(Tactics.s_nExp);
	Tactics.s_nExp = Tactics.s_nExp / 100;

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
	pop(Tactics.s_nTranscendenceExp);
#endif
	pop(Tactics.s_nLevel);
	pop(Tactics.s_Attribute, sizeof(Tactics.s_Attribute));

	pop(Tactics.s_HatchLevel);

	pop(Tactics.s_dwBaseDigimonID);
	DBG("nBaseEvoUnitIDX : %d", Tactics.s_dwBaseDigimonID);

	pop(Tactics.s_nMaxEvoUnit);
	pop(&Tactics.s_EvoUnit[1], sizeof(cEvoUnit) * Tactics.s_nMaxEvoUnit);

	memset(&Tactics.s_AttributeExp, NULL, sizeof(n2) * NewAttribute::MaxDigitalType);
	memset(&Tactics.s_NatureExp, NULL, sizeof(n2) * NewAttribute::MaxNatualType);

	u2 nInchantLevel = 0;
	u2 nExtentionParameter[6] = { 0, };
	pop(Tactics.s_nEnchantLevel);
	pop(Tactics.s_ExtendAttribute, sizeof(Tactics.s_ExtendAttribute));
	pop(Tactics.s_ExtendAttributeLV, sizeof(Tactics.s_ExtendAttributeLV));

	g_pDataMng->GetTactics()->AddTactics(&Tactics, nArrIDX - 1);

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	GS2C_RECV_CHECKTYPE recvAchieve;
	recvAchieve.nType = AchieveContents::CA_GetTactics;
	recvAchieve.nValue1 = Tactics.s_Type.m_nType;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recvAchieve);

	GS2C_RECV_ENCYCLOPEDIA_EVOLUTION recv;
	recv.nDigimonID = Tactics.s_dwBaseDigimonID;
	recv.nScale = nScale;
	recv.nType = 0;
	for (int i = 1; i < nLimit::EvoUnit; i++)
	{
		recv.nEvoUnit[i] = Tactics.s_EvoUnit[i];
	}
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_EVOL_UPDATE, &recv);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_HATCHOUT_COMPLETE, NULL);
}



void cCliGame::RecvBattleTagSuccess(void)	// 전투중 용병 변경 성공
{
	// 아이템 감소
	int nSlot = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS(nItem::BattleTag, 0);
	assert_cs(nSlot != cData_Inven::INVALIDE_INVEN_INDEX);

	if (nSlot != cData_Inven::INVALIDE_INVEN_INDEX)
	{
		g_pDataMng->GetInven()->GetData(nSlot)->DecreaseCount(1);
	}

	DWORD dwMask = nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CAMERA_AXIS_Z;
	g_pCharMng->GetTamerUser()->GetProp_Effect()->AddEffect("system\\BattleTag.nif", 1.0f, dwMask);

	switch (g_pCharMng->GetTamerUser()->GetModelID())
	{
	case CODE_MASARU:
		g_pCharMng->GetTamerUser()->PlaySound("Tamer\\masaru\\masaru_hit03.wav");
		break;
	case CODE_TOHMA:
		g_pCharMng->GetTamerUser()->PlaySound("Tamer\\tohma\\yoshino_hit03.wav");
		break;
	case CODE_YOSHINO:
		g_pCharMng->GetTamerUser()->PlaySound("Tamer\\yoshino\\tohma_hit03.wav");
		break;
	case CODE_IKUTO:
		g_pCharMng->GetTamerUser()->PlaySound("Tamer\\ikuto\\ikuto_hit03.wav");
		break;
	}

	g_pCharMng->GetDigimonUser(0)->GetProp_Effect()->AddEffect("system\\BattleTag_Effect.nif", 1.0f, dwMask);
}


void cCliGame::RecvBattleTagFailure(void)	// 전투중 용병 변경 실패
{
	g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CHANGE_PARTNER, 0, NULL);
#ifdef CROSSWARS_SYSTEM
	g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CROSSCHANGE_PARTNER, 0, NULL);
	g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CROSSRECOVER_PARTNER, 0, NULL);
#endif
}

#ifdef CROSSWARS_SYSTEM

void cCliGame::RecvCrossChangePartner(void) // 크로스워즈와 파트너 변경
{
	n4 nPrevType;
	cData_ServerSync::sCHANGE_PARTNER sync;

	pop(sync.nUID);
	if (sync.nUID == 0)
	{
		// 자신에게만 전송됨
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT("CHAT_NOT_POSSIBLE_CHANGE_PARTNER");
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	pop(nPrevType);

	pop(sync.nNextArrIDX); // 다음으로 선택된 용병의 ArrIDX
	pop(sync.nNextType);

	pop(sync.nNextLevel);

#ifdef THAI_WORDTRANSFORM	// 파트너 디지몬 변경 시 서버에서 char 타입으로 이름을 보내준것을 tchar 로 변환하여 저장. lks007	12.08.10.
	CHAR _tempName[Language::pLength::name + 1];
	pop(_tempName);
	M2W(sync.szNextDigimonName, _tempName, Language::pLength::name + 1);
#else
	pop(sync.szNextDigimonName);
#endif
	pop(sync.nNextScale);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(sync.nUID);
	if (pObject == NULL)
		return;

	CDigimon* pDigimon = (CDigimon*)pObject;

	// 디지몬 확장 능력치 수신
	// 수신 순서 1: AP(AT) 공격 2: BL 블록 3: CR 크리티컬 확률 4: AS 공격 스피드 5: EV 회피 6: HT 공격 성공률 7: HP 체력

	u2 nEnchantTotalLevel;
	pop(nEnchantTotalLevel);
	pDigimon->GetEnchantStat()->SetEnchantLevel(nEnchantTotalLevel);

	u2 nEnchantLevel[nExtendState::MaxExtendStat] = { 0, };
	pop(nEnchantLevel[nExtendState::ET_AT]);
	pop(nEnchantLevel[nExtendState::ET_BL]);
	pop(nEnchantLevel[nExtendState::ET_CR]);
	pop(nEnchantLevel[nExtendState::ET_AS]);
	pop(nEnchantLevel[nExtendState::ET_EV]);
	pop(nEnchantLevel[nExtendState::ET_HT]);
	pop(nEnchantLevel[nExtendState::ET_HP]);

	pDigimon->GetBuff()->GetBuff()->clear();
	pDigimon->GetBuff()->GetDeBuff()->clear();

	u2 nBuffCount = 0;
	pop(nBuffCount);					// 버프
	if (nBuffCount != 0)
	{
		u2 BuffCode = 0;
		u2 BuffClass = 0;
		u4 nEndTS = 0;
		u4 dwSkillCode = 0;

		for (int i = 0; i < nBuffCount; i++)
		{
			pop(BuffCode);			// 버프 코드
			pop(BuffClass);			// 버프 클래스 레벨
			pop(nEndTS);			// 버프 만료 시간
			pop(dwSkillCode);		// 버프코드가 같은 것이 있어 구분을 위해 스킬코드 저장
			pDigimon->GetBuff()->SetBuff(BuffCode, 0, nEndTS, BuffClass, dwSkillCode);
		}
	}

	// 캐스팅 타입별로 구현
	switch (pObject->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	{
		pDigimon->Scene_CrossChangePartner(sync.nNextType, sync.nNextArrIDX, sync.szNextDigimonName, sync.nNextScale * 0.0001f, sync.nNextLevel);

		CTamer* pTamer = g_pCharMng->GetTamer(pDigimon->GetTamerLink());
		if (pTamer)
		{
			pTamer->Scene_Evol(pDigimon, 0);
		}
	}
	break;
	case RTTI_DIGIMON_USER:
	{
		// 본인 것만 받는다
		cData_CrossTactics::sCross* pNextData = g_pDataMng->GetCrossTatics()->GetTactics(sync.nNextArrIDX);
		pop(pNextData->s_Attribute);					// 스텟
		//pop(pNextData->s_nEnable);			

		g_pCrossEvent->EventStart(pNextData->s_dwBaseDigimonID);

		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CROSSCHANGE_PARTNER, 0, &sync);

	}
	break;
	default:
		assert_cs(false);
	}
}


void cCliGame::RecvCrossChangePartner2(void)
{
	n4 nPrevType;
	cData_ServerSync::sCHANGE_PARTNER sync;

	pop(sync.nUID);
	if (sync.nUID == 0)
	{
		// 자신에게만 전송됨
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT("CHAT_NOT_POSSIBLE_CHANGE_PARTNER");
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	pop(nPrevType);

	pop(sync.nNextArrIDX); // 다음으로 선택된 용병의 ArrIDX
	pop(sync.nNextType);

	pop(sync.nNextLevel);

#ifdef THAI_WORDTRANSFORM	// 파트너 디지몬 변경 시 서버에서 char 타입으로 이름을 보내준것을 tchar 로 변환하여 저장. lks007	12.08.10.
	CHAR _tempName[Language::pLength::name + 1];
	pop(_tempName);
	M2W(sync.szNextDigimonName, _tempName, Language::pLength::name + 1);
#else
	pop(sync.szNextDigimonName);
#endif
	pop(sync.nNextScale);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(sync.nUID);
	if (pObject == NULL)
		return;

	CDigimon* pDigimon = (CDigimon*)pObject;

	// 디지몬 확장 능력치 수신
	// 수신 순서 1: AP(AT) 공격 2: BL 블록 3: CR 크리티컬 확률 4: AS 공격 스피드 5: EV 회피 6: HT 공격 성공률 7: HP 체력

	u2 nEnchantTotalLevel;
	pop(nEnchantTotalLevel);
	pDigimon->GetEnchantStat()->SetEnchantLevel(nEnchantTotalLevel);

	u2 nEnchantLevel[nExtendState::MaxExtendStat] = { 0, };
	pop(nEnchantLevel[nExtendState::ET_AT]);
	pop(nEnchantLevel[nExtendState::ET_BL]);
	pop(nEnchantLevel[nExtendState::ET_CR]);
	pop(nEnchantLevel[nExtendState::ET_AS]);
	pop(nEnchantLevel[nExtendState::ET_EV]);
	pop(nEnchantLevel[nExtendState::ET_HT]);
	pop(nEnchantLevel[nExtendState::ET_HP]);

	pDigimon->GetBuff()->GetBuff()->clear();
	pDigimon->GetBuff()->GetDeBuff()->clear();

	u2 nBuffCount = 0;
	pop(nBuffCount);					// 버프
	if (nBuffCount != 0)
	{
		u2 BuffCode = 0;
		u2 BuffClass = 0;
		u4 nEndTS = 0;
		u4 dwSkillCode = 0;

		for (int i = 0; i < nBuffCount; i++)
		{
			pop(BuffCode);			// 버프 코드
			pop(BuffClass);			// 버프 클래스 레벨
			pop(nEndTS);			// 버프 만료 시간
			pop(dwSkillCode);		// 버프코드가 같은 것이 있어 구분을 위해 스킬코드 저장
			pDigimon->GetBuff()->SetBuff(BuffCode, 0, nEndTS, BuffClass, dwSkillCode);
		}
	}

	// 캐스팅 타입별로 구현
	switch (pObject->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	{
		pDigimon->Scene_CrossChangePartner2(sync.nNextType, sync.nNextArrIDX, sync.szNextDigimonName, sync.nNextScale * 0.0001f, sync.nNextLevel);

		CTamer* pTamer = g_pCharMng->GetTamer(pDigimon->GetTamerLink());
		if (pTamer)
		{
			pTamer->Scene_Evol(pDigimon, 0);
		}
	}
	break;
	case RTTI_DIGIMON_USER:
	{
		cData_PostLoad::sDATA* pNextData = g_pDataMng->GetCrossTactics()->GetTempTactics();

		pNextData->s_fScale = sync.nNextScale * 0.0001f;//
		pNextData->s_nLevel = sync.nNextLevel;
		pNextData->s_dwBaseDigimonID = sync.nNextType;
		_tcscpy_s(pNextData->s_szName, Language::pLength::name + 1, sync.szNextDigimonName);

		pop(pNextData->s_Attribute);

		pop(pNextData->s_Type);

		pop(pNextData->s_nExp);
		pNextData->s_nExp = pNextData->s_nExp / 100;
#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		pop(pNextData->s_nTranscendenceExp);
#endif
		pop(pNextData->s_nMaxEvoUnit);
		pop(&pNextData->s_EvoUnit[1], sizeof(cEvoUnit) * pNextData->s_nMaxEvoUnit);
		pop(pNextData->s_ExtendAttribute, sizeof(pNextData->s_ExtendAttribute));

		// 기본 속성 경험치
		for (int i = 0; i < NewAttribute::MaxDigitalType; i++)
		{
			n2 AttributeExp;
			pop(AttributeExp);
			pNextData->s_AttributeExp[i] = AttributeExp;
		}

		// 디지몬 자연속성 경험치
		for (int i = 0; i < NewAttribute::MaxNatualType; i++)
		{
			n2 NatureExp;
			pop(NatureExp);
			pNextData->s_NatureExp[i] = NatureExp;
		}

		g_pCrossEvent->EventStart(pNextData->s_dwBaseDigimonID);

		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CROSSCHANGE_PARTNER, 0, &sync);	// 조그래스 플래그가 아니면 

	}
	break;
	default:
		assert_cs(false);
	}
}


void cCliGame::RecvCrossWarsCrownHeal(void)
{
	n4 nHeal;
	pop(nHeal);

	CTamerUser::sUSER_STAT* pBaseStat = (CTamerUser::sUSER_STAT*)g_pCharMng->GetTamerUser()->GetBaseStat();
	pBaseStat->SetCR(pBaseStat->GetCR() + (nHeal));

	// 맥스치보다 넘으면 최대치 고정
	if (pBaseStat->GetMaxCR() < pBaseStat->GetCR())
		pBaseStat->SetCR(pBaseStat->GetMaxCR());
	// 현재치가 0보다 작으면 0고정
	else if (0 > pBaseStat->GetCR())
		pBaseStat->SetCR(0);
}
void cCliGame::RecvCrossWarsLoader(void)
{
	n4 nMcr;
	n4 nCr;

	pop(nMcr);
	pop(nCr);


	CTamerUser::sUSER_STAT* pBaseStat = (CTamerUser::sUSER_STAT*)g_pCharMng->GetTamerUser()->GetBaseStat();
	pBaseStat->SetMaxCR(nMcr);
	pBaseStat->SetCR(nCr);

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	if (pTamerUser == NULL)
		return;
	pTamerUser->PlaySound(SOUND_ENABLE_CROSSLOADER);


	cData_Inven_Cross* pInvenCross = g_pDataMng->GetInvenCross();

	pInvenCross->SetCrossLoader(1); // 활성화

	g_pDataMng->GetPostLoad()->GetTamerData()->s_nCrossLoader = 1;


	if (false == g_pGameIF->IsActiveWindow(cBaseWindow::WT_BGSPRITE, cBGSprite::POINTTER_CROSSBUTTON))
		g_pGameIF->GetDynamicIF(cBaseWindow::WT_BGSPRITE, cBGSprite::POINTTER_CROSSBUTTON, cBGSprite::CROSSBUTTON);

	// 크로스 UI 리로드.
	GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_CROSSLOADER, NULL);
}
void cCliGame::RecvCrossWarsRelease(void)
{
	n4 nPrevType;
	cData_ServerSync::sCHANGE_PARTNER sync;

	pop(sync.nUID);
	if (sync.nUID == 0)
	{
		// 자신에게만 전송됨
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT("CHAT_NOT_POSSIBLE_CHANGE_PARTNER");
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	pop(nPrevType);

	pop(sync.nNextArrIDX); // 다음으로 선택된 용병의 ArrIDX
	pop(sync.nNextType);

	pop(sync.nNextLevel);

#ifdef THAI_WORDTRANSFORM	// 파트너 디지몬 변경 시 서버에서 char 타입으로 이름을 보내준것을 tchar 로 변환하여 저장. lks007	12.08.10.
	CHAR _tempName[Language::pLength::name + 1];
	pop(_tempName);
	M2W(sync.szNextDigimonName, _tempName, Language::pLength::name + 1);
#else
	pop(sync.szNextDigimonName);
#endif
	pop(sync.nNextScale);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(sync.nUID);
	if (pObject == NULL)
		return;

	CDigimon* pDigimon = (CDigimon*)pObject;

	// 디지몬 확장 능력치 수신
	// 수신 순서 1: AP(AT) 공격 2: BL 블록 3: CR 크리티컬 확률 4: AS 공격 스피드 5: EV 회피 6: HT 공격 성공률 7: 체력

	u2 nEnchantTotalLevel;
	pop(nEnchantTotalLevel);
	pDigimon->GetEnchantStat()->SetEnchantLevel(nEnchantTotalLevel);

	u2 nEnchantLevel[nExtendState::MaxExtendStat] = { 0, };
	pop(nEnchantLevel[nExtendState::ET_AT]);
	pop(nEnchantLevel[nExtendState::ET_BL]);
	pop(nEnchantLevel[nExtendState::ET_CR]);
	pop(nEnchantLevel[nExtendState::ET_AS]);
	pop(nEnchantLevel[nExtendState::ET_EV]);
	pop(nEnchantLevel[nExtendState::ET_HT]);
	pop(nEnchantLevel[nExtendState::ET_HP]);

	pDigimon->GetBuff()->GetBuff()->clear();
	pDigimon->GetBuff()->GetDeBuff()->clear();

	u2 nBuffCount = 0;
	pop(nBuffCount);					// 버프
	if (nBuffCount != 0)
	{
		u2 BuffCode = 0;
		u2 BuffClass = 0;
		u4 nEndTS = 0;
		u4 dwSkillCode = 0;

		for (int i = 0; i < nBuffCount; i++)
		{
			pop(BuffCode);			// 버프 코드
			pop(BuffClass);			// 버프 클래스 레벨
			pop(nEndTS);			// 버프 만료 시간
			pop(dwSkillCode);		// 버프코드가 같은 것이 있어 구분을 위해 스킬코드 저장
			pDigimon->GetBuff()->SetBuff(BuffCode, 0, nEndTS, BuffClass, dwSkillCode);
		}
	}

	// 캐스팅 타입별로 구현
	switch (pObject->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	{
		pDigimon->Scene_ChangePartner(sync.nNextType, sync.nNextArrIDX, sync.szNextDigimonName, sync.nNextScale * 0.0001f, sync.nNextLevel);

		CTamer* pTamer = g_pCharMng->GetTamer(pDigimon->GetTamerLink());
		if (pTamer)
		{
			pTamer->Scene_Evol(pDigimon, 0);
		}
	}
	break;
	case RTTI_DIGIMON_USER:
	{
		//임시 용병 데이타를 만들어서 넣고 cData_Tactics::ChangeCrossTactics 에서 용병 데이터 교체하자 		

		cData_PostLoad::sDATA* pNextData = g_pDataMng->GetTactics()->GetTempTactics();

		pNextData->s_fScale = sync.nNextScale * 0.0001f;//
		pNextData->s_nLevel = sync.nNextLevel;//
		pNextData->s_dwBaseDigimonID = sync.nNextType;//
		_tcscpy_s(pNextData->s_szName, Language::pLength::name + 1, sync.szNextDigimonName);//

		pop(pNextData->s_Attribute);//

		pop(pNextData->s_Type);//

		pop(pNextData->s_nExp);//
		pNextData->s_nExp = pNextData->s_nExp / 100;

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		pop(pNextData->s_nTranscendenceExp);
#endif
		pop(pNextData->s_nMaxEvoUnit);//
		pop(&pNextData->s_EvoUnit[1], sizeof(cEvoUnit) * pNextData->s_nMaxEvoUnit);//
		pop(pNextData->s_ExtendAttribute, sizeof(pNextData->s_ExtendAttribute));//

		// 기본 속성 경험치
		for (int i = 0; i < NewAttribute::MaxDigitalType; i++)
		{
			n2 AttributeExp;
			pop(AttributeExp);//
			pNextData->s_AttributeExp[i] = AttributeExp;
		}

		// 디지몬 자연속성 경험치
		for (int i = 0; i < NewAttribute::MaxNatualType; i++)
		{
			n2 NatureExp;
			pop(NatureExp);	//	
			pNextData->s_NatureExp[i] = NatureExp;
		}
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CROSSRECOVER_PARTNER, 0, &sync);	// 조그래스 플래그가 아니면 

	}
	break;
	default:
		assert_cs(false);
	}

}
void cCliGame::RecvCrossWarsCRRelease(void)
{
	n4 nPrevType;
	cData_ServerSync::sCHANGE_PARTNER sync;

	pop(sync.nUID);
	if (sync.nUID == 0)
	{
		// 자신에게만 전송됨
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT("CHAT_NOT_POSSIBLE_CHANGE_PARTNER");
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	pop(nPrevType);

	pop(sync.nNextArrIDX); // 다음으로 선택된 용병의 ArrIDX
	pop(sync.nNextType);

	pop(sync.nNextLevel);

#ifdef THAI_WORDTRANSFORM	// 파트너 디지몬 변경 시 서버에서 char 타입으로 이름을 보내준것을 tchar 로 변환하여 저장. lks007	12.08.10.
	CHAR _tempName[Language::pLength::name + 1];
	pop(_tempName);
	M2W(sync.szNextDigimonName, _tempName, Language::pLength::name + 1);
#else
	pop(sync.szNextDigimonName);
#endif
	pop(sync.nNextScale);

	CsC_AvObject* pObject = g_pMngCollector->GetObject(sync.nUID);
	if (pObject == NULL)
		return;

	CDigimon* pDigimon = (CDigimon*)pObject;

	// 디지몬 확장 능력치 수신
	// 수신 순서 1: AP(AT) 공격 2: BL 블록 3: CR 크리티컬 확률 4: AS 공격 스피드 5: EV 회피 6: HT 공격 성공률 7: HP 체력

	u2 nEnchantTotalLevel;
	pop(nEnchantTotalLevel);
	pDigimon->GetEnchantStat()->SetEnchantLevel(nEnchantTotalLevel);

	u2 nEnchantLevel[nExtendState::MaxExtendStat] = { 0, };
	pop(nEnchantLevel[nExtendState::ET_AT]);
	pop(nEnchantLevel[nExtendState::ET_BL]);
	pop(nEnchantLevel[nExtendState::ET_CR]);
	pop(nEnchantLevel[nExtendState::ET_AS]);
	pop(nEnchantLevel[nExtendState::ET_EV]);
	pop(nEnchantLevel[nExtendState::ET_HT]);
	pop(nEnchantLevel[nExtendState::ET_HP]);

	pDigimon->GetBuff()->GetBuff()->clear();
	pDigimon->GetBuff()->GetDeBuff()->clear();

	u2 nBuffCount = 0;
	pop(nBuffCount);					// 버프
	if (nBuffCount != 0)
	{
		u2 BuffCode = 0;
		u2 BuffClass = 0;
		u4 nEndTS = 0;
		u4 dwSkillCode = 0;

		for (int i = 0; i < nBuffCount; i++)
		{
			pop(BuffCode);			// 버프 코드
			pop(BuffClass);			// 버프 클래스 레벨
			pop(nEndTS);			// 버프 만료 시간
			pop(dwSkillCode);		// 버프코드가 같은 것이 있어 구분을 위해 스킬코드 저장
			pDigimon->GetBuff()->SetBuff(BuffCode, 0, nEndTS, BuffClass, dwSkillCode);
		}
	}

	// 캐스팅 타입별로 구현
	switch (pObject->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	{
		pDigimon->Scene_ChangePartner(sync.nNextType, sync.nNextArrIDX, sync.szNextDigimonName, sync.nNextScale * 0.0001f, sync.nNextLevel);

		CTamer* pTamer = g_pCharMng->GetTamer(pDigimon->GetTamerLink());
		if (pTamer)
		{
			pTamer->Scene_Evol(pDigimon, 0);
		}
	}
	break;
	case RTTI_DIGIMON_USER:
	{
		//임시 용병 데이타를 만들어서 넣고 cData_Tactics::ChangeCrossTactics 에서 용병 데이터 교체하자 		

		cData_PostLoad::sDATA* pNextData = g_pDataMng->GetTactics()->GetTempTactics();

		pNextData->s_fScale = sync.nNextScale * 0.0001f;//
		pNextData->s_nLevel = sync.nNextLevel;//
		pNextData->s_dwBaseDigimonID = sync.nNextType;//
		_tcscpy_s(pNextData->s_szName, Language::pLength::name + 1, sync.szNextDigimonName);//

		pop(pNextData->s_Attribute);//

		pop(pNextData->s_Type);//

		pop(pNextData->s_nExp);//
		pNextData->s_nExp = pNextData->s_nExp / 100;

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		pop(pNextData->s_nTranscendenceExp);
#endif
		pop(pNextData->s_nMaxEvoUnit);//
		pop(&pNextData->s_EvoUnit[1], sizeof(cEvoUnit) * pNextData->s_nMaxEvoUnit);//
		pop(pNextData->s_ExtendAttribute, sizeof(pNextData->s_ExtendAttribute));//

		// 기본 속성 경험치
		for (int i = 0; i < NewAttribute::MaxDigitalType; i++)
		{
			n2 AttributeExp;
			pop(AttributeExp);//
			pNextData->s_AttributeExp[i] = AttributeExp;
		}

		// 디지몬 자연속성 경험치
		for (int i = 0; i < NewAttribute::MaxNatualType; i++)
		{
			n2 NatureExp;
			pop(NatureExp);	//	
			pNextData->s_NatureExp[i] = NatureExp;
		}

		//g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CROSSRECOVER_PARTNER, 0, &sync );			

		// 교체 성공		
		{
			CsC_AvObject* pDigimon = g_pMngCollector->GetObject(sync.nUID);
			if (pDigimon == NULL)
			{
				assert_cs(false);
				return;
			}

			assert_cs(pDigimon->GetLeafRTTI() == RTTI_DIGIMON_USER);

			((CDigimon*)pDigimon)->GetBuff()->ReleaseBuff(nsBuff::BK_CANDY_SIZE_CHANGE);

			((CDigimon*)pDigimon)->Scene_CrossRecoverPartner(sync.nNextType, sync.nNextArrIDX, sync.szNextDigimonName, sync.nNextScale * 0.0001f, sync.nNextLevel);
			CTamer* pTamer = g_pCharMng->GetTamerUser();
			assert_cs(pTamer);
			pTamer->Scene_Evol(pDigimon, 0);
			g_pDataMng->GetServerSync()->SendServer(cData_ServerSync::CROSSRECOVER_PARTNER_ING, 0);

			// 인터페이스 닫아주자
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_TALK);
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_QUEST_REV);
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_QUEST_COMP);
		}

	}
	break;
	default:
		assert_cs(false);
	}
}
void cCliGame::RecvCrossWarsRecover(void)
{
	n1 nType;
	pop(nType);

	if (0 == nType)
	{
		RecvCrossWarsRelease(); // 해제 요청시
		//RecvCrossWarsCRRelease();
	}
	else if (1 == nType)
	{
		RecvCrossWarsCRRelease(); // CR 떨어져서
	}
}
#endif


void cCliGame::RecvChangePartner(void) // 파트너 변경
{
	n4 nPrevType = 0;
	cData_ServerSync::sCHANGE_PARTNER sync;

	pop(sync.nUID);
	if (sync.nUID == 0)
	{
		// 자신에게만 전송됨
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT("CHAT_NOT_POSSIBLE_CHANGE_PARTNER");
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	pop(nPrevType);

	pop(sync.nNextArrIDX); // 다음으로 선택된 용병의 ArrIDX
	pop(sync.nNextType);

	pop(sync.nNextLevel);

#ifdef THAI_WORDTRANSFORM	// 파트너 디지몬 변경 시 서버에서 char 타입으로 이름을 보내준것을 tchar 로 변환하여 저장. lks007	12.08.10.
	CHAR _tempName[Language::pLength::name + 1];
	pop(_tempName);
	M2W(sync.szNextDigimonName, _tempName, Language::pLength::name + 1);
#else
	pop(sync.szNextDigimonName);
#endif
	pop(sync.nNextScale);
	u4 nEndTick = 0;		// ScaleEvent 남은시간
	pop(nEndTick);			// 0이 아니면 ScaleEvent 버프 툴팁 세팅

	CsC_AvObject* pObject = g_pMngCollector->GetObject(sync.nUID);
	if (pObject == NULL)
		return;

	CDigimon* pDigimon = (CDigimon*)pObject;

	// 디지몬 확장 능력치 수신
	// 수신 순서 1: AP(AT) 공격 2: BL 블록 3: CR 크리티컬 확률 4: AS 공격 스피드 5: EV 회피 6: HT 공격 성공률 7: HP 체력

	u2 nEnchantTotalLevel;
	pop(nEnchantTotalLevel);
	pDigimon->GetEnchantStat()->SetEnchantLevel(nEnchantTotalLevel);

	u2 nEnchantLevel[nExtendState::MaxExtendStat] = { 0, };
	pop(nEnchantLevel[nExtendState::ET_AT]);
	pop(nEnchantLevel[nExtendState::ET_BL]);
	pop(nEnchantLevel[nExtendState::ET_CR]);
	pop(nEnchantLevel[nExtendState::ET_AS]);
	pop(nEnchantLevel[nExtendState::ET_EV]);
	pop(nEnchantLevel[nExtendState::ET_HT]);
	pop(nEnchantLevel[nExtendState::ET_HP]);

	pDigimon->GetBuff()->ClearBuffLoopEffect();
	pDigimon->GetBuff()->GetBuff()->clear();
	pDigimon->GetBuff()->GetDeBuff()->clear();

	u2 nBuffCount = 0;
	pop(nBuffCount);					// 버프
	if (nBuffCount != 0)
	{
		u2 BuffCode = 0;
		u2 BuffClass = 0;
		u4 nEndTS = 0;
		u4 dwSkillCode = 0;

		for (int i = 0; i < nBuffCount; i++)
		{
			pop(BuffCode);			// 버프 코드
			pop(BuffClass);			// 버프 클래스 레벨
			pop(nEndTS);			// 버프 만료 시간
			pop(dwSkillCode);		// 버프코드가 같은 것이 있어 구분을 위해 스킬코드 저장
			pDigimon->GetBuff()->SetBuff(BuffCode, sync.nUID, nEndTS, BuffClass, dwSkillCode);
		}
	}

	// 캐스팅 타입별로 구현
	switch (pObject->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	{
		pDigimon->Scene_ChangePartner(sync.nNextType, sync.nNextArrIDX - 1, sync.szNextDigimonName, sync.nNextScale * 0.0001f, sync.nNextLevel);

		CTamer* pTamer = g_pCharMng->GetTamer(pDigimon->GetTamerLink());
		if (pTamer)
		{
			pTamer->Scene_Evol(pDigimon, 0);
		}
	}
	break;
	case RTTI_DIGIMON_USER:
	{
		// 본인 것만 받는다
		cData_PostLoad::sDATA* pNextData = g_pDataMng->GetTactics()->GetTactics(sync.nNextArrIDX - 1);

		pNextData->s_fScale = sync.nNextScale * 0.0001f;//
		pNextData->s_nLevel = sync.nNextLevel;//
		pNextData->s_dwBaseDigimonID = sync.nNextType;//
		_tcscpy_s(pNextData->s_szName, Language::pLength::name + 1, sync.szNextDigimonName);//

		pop(pNextData->s_Attribute);					// 스텟

		// 파트너 채인지 시작
		CDigimonUser* pDUser = g_pCharMng->GetDigimonUser(0);
		if (0 != pDUser->GetJointProgressFlag())
			g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CHANGE_PARTNER_WITHOUTSCENE, 0, &sync);
		else
			g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CHANGE_PARTNER, 0, &sync);	// 조그래스 플래그가 아니면 

		if (nEndTick != 0)// 이벤트 실행
		{
			uint nTime = _TIME_TS + nEndTick / 1000;
			pDigimon->GetBuff()->SetBuff(nsBuff::BK_CANDY_SIZE_CHANGE, 0, nTime, 0);
		}
	}
	break;
	default:
		assert_cs(false);
	}
}

void cCliGame::RecvWarehouseSlotSize(void)
{
	u2 nSlotSize;
	pop(nSlotSize);

	//일반창고
	//	g_pDataMng->GetWarehouse()->SetSlotCount( nSlotSize );
	GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_NORMAL_SLOTCOUNT, &nSlotSize);

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_WareHouse_AllOpen;
	recv.nValue1 = nSlotSize;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);
}

// 인벤토리 크기 변경시
void cCliGame::RecvInvenSlotSize(void)
{
	u2 nInvenSlotSize;
	pop(nInvenSlotSize);

	SAFE_POINTER_RET(g_pDataMng);
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET(pInven);
	pInven->SetInvenSlotCount(nInvenSlotSize);
	pInven->LoadApply();

	// 인벤토리 추가가방 첫 확장시~ 가방 인터페이스 오픈시켜주자
	bool bSound = true;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_INVENTORY, &bSound);

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_InvenSlot_AllOpen;
	recv.nValue1 = nInvenSlotSize;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);
}


// void cCliGame::RecvMsgAllCropEventItem(void)
// {
// 	std::wstring szTamerName;
// 	u4 nItemType;
// 	u4 nItemNo;	// 해당 아이템의 dm_item_list-item-ItemNo
// 
// 	u2 nProtocol;
// 	u4 nSrcItemNo;	// 해당 아이템에 의해 nItemNo가 생산됨
// 
// 	pop(szTamerName);
// 	pop(nItemType);	// maybe nItem::EventItem - 이벤트 아이템(문화 상품권, mp3 등등)
// 	pop(nItemNo);	//
// 
// 	pop(nProtocol);
// 	pop(nSrcItemNo);
// 
// 	cPublicity::eITEM_HOW it;
// 	switch(nProtocol)
// 	{
// 	case pItem::Crop			:
// 		it = cPublicity::CROP;
// 		break;
// 	case pItem::CapsuleToItem	:
// 	case pItem::DigitamaToItem	:
// 		it = cPublicity::DIGITAMA_SCAN;
// 		break;
// 	default:
// 		assert_cs( false );
// 		return;
// 	}
// 
// 	( (cPublicity*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_PUBLICITY ) )->SetText_Item( it, szTamerName.c_str(), nSrcItemNo, nItemNo );	
// }


// void cCliGame::RecvMsgAllHatchOut(void)
// {
// 	std::wstring szTamerName;
// 	std::wstring szNewPartnerName;
// 	n4 nNewPartnerType;
// 	n4 nNewPartnerScale;
// 	n4 nEggLevel; // 부화 단계
// 
// 	pop(szTamerName);
// 	pop(szNewPartnerName);
// 	pop(nNewPartnerType);
// 	pop(nNewPartnerScale);
// 
// 	pop(nEggLevel);
// #ifdef VERSION_USA
// 	if(nEggLevel >=4)
// 		( (cPublicity*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_PUBLICITY ) )->SetText_Tactics( szTamerName.c_str(), szNewPartnerName.c_str(), nNewPartnerType, nNewPartnerScale*0.0001f, nEggLevel );
// #else
// 	( (cPublicity*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_PUBLICITY ) )->SetText_Tactics( szTamerName.c_str(), szNewPartnerName.c_str(), nNewPartnerType, nNewPartnerScale*0.0001f, nEggLevel );
// #endif
// }


void cCliGame::RecvDigimonCareSlotList(void)
{
	CDigimonArchiveContents::ArchiveInitInfo kRecvData;
	int nType = 0;
	pop(nType);
	kRecvData.SetType(nType);

	if (nType == 0)
	{
		int nOpenSlotSize = 0;
		pop(nOpenSlotSize);	// 열려있는 용병 보관소 슬롯 갯수
		kRecvData.SetOpenedArchiveSlotCount(nOpenSlotSize);

		u4 nSlot = 0;	// 인큐베이터 안에 들어있는 디지몬의 디보 슬롯 번호 ( 1000~ )
		for (int i = 0; i < 3; i++)
		{
			pop(nSlot);
			kRecvData.AddIncuInfo(i, nSlot);
		}
	}


	u2 nScale = 0;
	int nSlotNo = 0;

	int nInchantLevel = 0;
	u2 nExtentionParameter[6] = { 0, };

	int nTHouseCnt = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse;
	kRecvData.SetMaxArchiveCnt(nTHouseCnt);
	bool isEndPacket = false;
	while (nSlotNo < nTHouseCnt)
	{
		pop(nSlotNo);		// 용병이 들어가 있는 슬롯의 번호
		if (nSlotNo == 1888) {//마지막 패킷 입니다.
			g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::TACTICSHOUSE_LIST, 0);
			isEndPacket = true;
			break;
		}
		else if (nSlotNo == 1999)//중간 패킷 입니다.
		{
			break;
		}
		cData_PostLoad::sDATA* pTactics = new cData_PostLoad::sDATA();// pDataTH->GetTactics( nSlotNo );
		pTactics->s_SlotIdx = nSlotNo;

		u2 nLevel = 0;
		u2 nScale = 0;

		pop(pTactics->s_Type);

		std::wstring name;
		pop(name);
		_tcscpy_s(pTactics->s_szName, name.c_str());

		pop(nScale);
		pTactics->s_fScale = nScale * 0.0001f;

		pop(pTactics->s_nExp);

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		pop(pTactics->s_nTranscendenceExp);
#endif 

		pTactics->s_nExp = pTactics->s_nExp / 100;
		pop(pTactics->s_nLevel);
		pop(pTactics->s_Attribute, sizeof(pTactics->s_Attribute));

		pop(pTactics->s_HatchLevel);

		pop(pTactics->s_dwBaseDigimonID);
		DBG("nBaseEvoUnitIDX : %d", pTactics->s_dwBaseDigimonID);
		pop(pTactics->s_nMaxEvoUnit);
		pop(&pTactics->s_EvoUnit[1], sizeof(cEvoUnit) * pTactics->s_nMaxEvoUnit);

		pop(pTactics->s_nEnchantLevel);
		pop(pTactics->s_ExtendAttribute, sizeof(pTactics->s_ExtendAttribute));
		pop(pTactics->s_ExtendAttributeLV);

		// 기본 속성 경험치
		for (int i = 0; i < NewAttribute::MaxDigitalType; i++)
		{
			n2 AttributeExp = 0;
			pop(AttributeExp);
			pTactics->s_AttributeExp[i] = AttributeExp;
		}

		// 디지몬 자연속성 경험치
		for (int i = 0; i < NewAttribute::MaxNatualType; i++)
		{
			n2 NatureExp = 0;
			pop(NatureExp);
			pTactics->s_NatureExp[i] = NatureExp;
		}

		// 용병 디지몬의 캐쉬 스킬 정보를 받자
		u1 nDSkillCnt = 0;

		pop(pTactics->s_nUID);
		pop(nDSkillCnt);

		assert_cs(nDSkillCnt <= (nLimit::EvoUnit * nLimit::MAX_ItemSkillDigimon));

		for (int i = 0; i < nDSkillCnt; ++i)
		{
			pop(pTactics->s_DCashSkill[i].s_nDigimonEvoStatus);
			pop(pTactics->s_DCashSkill[i].s_nDigimonCashSkillCode, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
			pop(pTactics->s_DCashSkill[i].s_nSkillCoolTime, sizeof(u4) * nLimit::MAX_ItemSkillDigimon);
		}

		kRecvData.AddData(nSlotNo, pTactics);
	}
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ARCHIEVE_DIGIMONS, &kRecvData);
	if (isEndPacket == true)
	{
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_END_ARCHIEVE_DIGIMONS, &kRecvData);
	}
}

void cCliGame::RecvDigimonCareSlotSize(void)
{
	int nOpenSlotSize;
	pop(nOpenSlotSize);	// 열려있는 용병 보관소 슬롯 갯수

	GS2C_RECV_EXTEND_ARCHIVE kRecvData;
	kRecvData.SetExtendArchiveCnt(nOpenSlotSize);

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ON_EXTENT_ARCHIVE, &kRecvData);

	cWindow::PlaySound(cWindow::SD_Qu2);
}

void cCliGame::RecvChangeTamerName(void)
{
	cData_QuickSlot::CheckItemCount_AllUser();

	cMessageBox::DelMsg(14010, false);

	n4 nResCode;
	pop(nResCode);

	if (nResCode == 100)
	{
		cPrintMsg::PrintMsg(30500);
		g_IME.ReleaseText();

		bool bSuccess = false;
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_CHANGE_TNAME, 0, &bSuccess);
		return;
	}

	if (nResCode == 1)
	{
		cPrintMsg::PrintMsg(10022);
		g_IME.ReleaseText();

		bool bSuccess = false;
		g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_CHANGE_TNAME, 0, &bSuccess);
		return;
	}

	if (nResCode == 3)
	{
		GS2C_RECV_NAME_CHANGE_NEARBY recvChangeName;
		pop(recvChangeName.s_szPreName);
		pop(recvChangeName.s_szNextName);

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_CHANGE_NAME_NEARBY, &recvChangeName);
		return;
	}

	n4 nInvenSlotNo;
	pop(nInvenSlotNo);

	std::wstring szName;
	pop(szName);

	bool bSuccess = true;
	g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::CASH_CHANGE_TNAME, 0, &bSuccess);

	CTamerUser* pUser = g_pCharMng->GetTamerUser();

	// 케릭터 이름
	pUser->GetCharOption()->SetName(szName.c_str());
#ifdef MASTERS_MATCHING // 테이머 이름 변경 시 아이콘 셋팅
	DWORD dwTeam = pUser->GetCharOption()->GetMatchTeam();
	pUser->GetCharOption()->SetMatchIcon(dwTeam);
#endif
	// 타겟창
	g_pGameIF->GetTamerWindow()->SetTargetObject(pUser);
	// 스테이터스창
	GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);

	pUser->SetAnimation(ANI::IDLE_CHAR);
	pUser->GetProp_Effect()->AddEffect("system\\QuestEnd.nif", 1.0f, 0);
	cWindow::PlaySound(cWindow::SD_Qu1);

	// 이름 짓는 윈도우 닫아준다.
	cCreateName::DeleteInstance();
}


void cCliGame::RecvScannerError(void)
{
	u4 nErrCode;
	pop(nErrCode);

	switch (nErrCode)
	{
	case 1:	// 스캐너 인식되지 않은 상태에서 카드 리딩
		assert_cs(false);
		break;
	}

}


void cCliGame::RecvScannerEquip(void)
{
	u4 nUID = 0;
	pop(nUID);	// nUID 테이머가 장착함

	CsC_AvObject* pObj = g_pMngCollector->GetObject(nUID);
	if (pObj == NULL)
		return;

	if (pObj->GetLeafRTTI() != RTTI_TAMER)
	{
		assert_csm(false, _T("테이머만 스캐너 장착 가능"));
		return;
	}

	((CTamer*)pObj)->Scene_Barcode_Connect();
}


void cCliGame::RecvScannerOn(void)
{
	int nTamerCondition;
	int nIsRegistered;

	pop(nTamerCondition);
	pop(nIsRegistered);		// 이미 등록되어 있는지

	g_pCharMng->GetTamerUser()->SetCondition(nTamerCondition);

	UpdateScannerState(nTamerCondition);
}


void cCliGame::RecvScannerOff(void)
{
	int nTamerCondition = 0;
	u4 nUID = 0;	// nUID 테이머 ID

	pop(nTamerCondition);
	pop(nUID);

	CsC_AvObject* pObj = g_pMngCollector->GetObject(nUID);
	if (pObj == NULL)
		return;

	if (pObj->GetLeafRTTI() == RTTI_TAMER_USER)	// 자기 자신 일 때,
		g_pCharMng->GetTamerUser()->SetCondition(nTamerCondition);

	else if (pObj->GetLeafRTTI() == RTTI_TAMER)			// 다른 유저 일 때,
		((CTamer*)pObj)->SetCondition(nTamerCondition);
}


#define d_nDummyCardType 30001

void cCliGame::RecvCardScan(void)
{
	u4 nTamerIDX = 0;
	int nCardType = 0;

	pop(nTamerIDX);
	pop(nCardType);

	CTamer* pTamer = g_pCharMng->GetTamer(nTamerIDX);
	if (pTamer == NULL)
		return;

	if (nCardType == 0)
	{
		pTamer->Scene_Barcode_CardScratch_OverCount();
		if (pTamer->GetLeafRTTI() == RTTI_TAMER_USER)
			cPrintMsg::PrintMsg(20045);
		return;
	}

	// 더미 카드 사용 횟수 초과
	if (nCardType == -1)
	{
		pTamer->Scene_Barcode_CardScratch_OverCount();
		if (pTamer->GetLeafRTTI() == RTTI_TAMER_USER)
			cPrintMsg::PrintMsg(20046);
		return;
	}

	int nResCode;
	pop(nResCode);	// 0이면 아이템 생성 실패, 1이면 아이템 생성 성공

	if (nCardType == d_nDummyCardType)
	{
		// 더미 카드 사용 성공
		int nDummyCardCount;
		pop(nDummyCardCount);	// 남아있는 횟수
		if (pTamer->GetLeafRTTI() == RTTI_TAMER_USER)
			cPrintMsg::PrintMsg(20047, &nDummyCardCount);
	}

	if (nResCode == 0)
	{
		pTamer->Scene_Barcode_CardScratch_Faile();
	}
	else
	{
		pTamer->Scene_Barcode_CardScratch_Success();
		//if(nCardType == d_nDummyCardType)
		//{
			// 더미 카드 사용 성공
		//}
		//else
		//{

		//	// 일반 카드 사용 성공
		//}
	}
}


void cCliGame::UpdateScannerState(n4 nScannerState)
{
#ifdef USE_BARCODE_REDER
	if (nScannerState & nSync::Scanner0)
	{
		// 스캐너 장착중
	}
	else
	{
		// 스캐너 장착하고 있지 아니함
		return;
	}


	if (nScannerState & nSync::Scanner1)
	{
		// 자신 소유의 스캐너
	}
	else
	{
		// 미등록 또는 타인의 스캐너
	}

	if (nScannerState & nSync::Scanner2)
	{
		// 한정판 스캐너
	}
	else
	{
		// 일반판 스캐너
	}

	if (nsBARCODE::g_bFirstEquipBarcode == true)
	{
		g_pCharMng->GetTamerUser()->Scene_Barcode_Connect();
		nsBARCODE::g_bFirstEquipBarcode = false;
		bool bUseUpdate = true;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::USE_GAMESCANNER, &bUseUpdate);
	}
#endif
}

void cCliGame::RecvHandshake487(void) {
	short HS;
	pop(HS);
	pop(HS);
	pop(HS);
	pop(HS);
	pop(HS);
	pop(HS);
}

void cCliGame::RecvScannerRegister(void)
{
#ifdef USE_BARCODE_REDER
	u4 nResCode1;
	n4 nTamerCondition;

	pop(nResCode1);
	pop(nTamerCondition);

	if (nResCode1 == 0)
	{
		// 등록 성공 하였습니다.
		g_pCharMng->GetTamerUser()->SetCondition(nTamerCondition);

		UpdateScannerState(nTamerCondition);
		cPrintMsg::PrintMsg(20034);

		bool bUseUpdate = true;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::USE_GAMESCANNER, &bUseUpdate);
	}
	else if (nResCode1 == -1)
	{
		// 다른 사용자가 사용중입니다.
		cPrintMsg::PrintMsg(20037);
	}
	else
	{
		// 등록 실패 - 아직 해제 시간이 남아있습니다.
		// nResCode 등록 가능시까지 남은 시간
		cPrintMsg::PrintMsg(20043, &nResCode1);
	}
#endif
}

void cCliGame::RecvScannerRelease(void)
{
	n4 nResCode;
	u4 nTamerCondition;

	pop(nResCode);
	pop(nTamerCondition);

	if (nResCode)
	{
		// 해제 성공
		cPrintMsg::PrintMsg(20041);
		g_pCharMng->GetTamerUser()->SetCondition(nTamerCondition);
		bool bUseUpdate = true;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::USE_GAMESCANNER, &bUseUpdate);
	}
	else
	{
		assert_cs(false);
	}
}


// void cCliGame::RecvAdvertBurstMode(void)
// {
// 	std::wstring szTamerName;
// 	std::wstring szDigimonName;
// 	int nBurstModeDigimonType;
// 	pop(szTamerName);
// 	pop(szDigimonName);
// 	pop(nBurstModeDigimonType);
// 
// 	TCHAR sz[ IF_PUBLICITY_LEN ] = {0, };
// 
// 	switch( nBurstModeDigimonType )
// 	{
// 	case 71037:
// 	case 71019:
// 		{
// 			_stprintf_s( sz, IF_PUBLICITY_LEN, _LAN( "< %s > 테이머 님께서 < %s > 디지몬 을 < %s >로 진화시켰습니다." ), szTamerName.c_str(), szDigimonName.c_str(),
// 				nsCsFileTable::g_pDigimonMng->GetDigimon( nBurstModeDigimonType )->GetInfo()->s_szName );
// 			( (cPublicity*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_PUBLICITY ) )->SetText( sz );
// 		}
// 		break;	
// 	default:
// 		{
// 			_stprintf_s( sz, IF_PUBLICITY_LEN, _LAN( "< %s > 테이머 님께서 < %s > 디지몬을 각성모드로 진화시켰습니다." ), szTamerName.c_str(), szDigimonName.c_str() );
// 			( (cPublicity*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_PUBLICITY ) )->SetText( sz );
// 		}
// 		break;
// 	}
// }

void cCliGame::RecvDailyEventInfo(void)
{
	GS2C_RECV_DAILY_EVENT_DATA recv;
	pop(recv.m_nEventNo);
	pop(recv.m_nRemainedTimeTS);
	pop(recv.m_nTotalTime);
	pop(recv.m_nWeek);

	if (recv.m_nWeek > 6)
		recv.m_nWeek = 0xFF;

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_DAILY_EVENT_DATA, &recv);
}

//2017-02-16-nova	
void cCliGame::RecvMonthlyEventInfo(void)
{
	GS2C_RECV_MONTHLY_EVENT_DATA recv;
	pop(recv.m_nEventNo);	//이벤트 번호
	pop(recv.m_nAttendCnt);	//출석일수
	pop(recv.m_bNotify);	//최초알림

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MONTHLY_EVENT_DATA, &recv);
}

void cCliGame::RecvAdvertBossDie(void)
{
	n4 nBossMonsterType;
	n4 nRemainedCount;

	pop(nBossMonsterType);
	pop(nRemainedCount);

	if (nRemainedCount == 0)
	{
		g_pGameIF->GetBossScene()->SetBossDie(nBossMonsterType);
	}

}

void cCliGame::RecvAdvertBossCount(void)
{
	u4 nRemainedCount;		// 앞으로 잡아야할 Seed Monster의 개체수
	u4 nBossMonsterType;	// seed monster를 모두 잡은 경우, 태어나는 보스 몬스터
	u4 nSeedMonsterType;	// 잡아야할 seed monster

	pop(nRemainedCount);
	pop(nBossMonsterType);
	pop(nSeedMonsterType);

	g_pGameIF->GetBossScene()->SetWarning(nBossMonsterType, nSeedMonsterType, nRemainedCount);
}

void cCliGame::RecvAdvertBossCreate(void)
{
	u4 nMapIDX;
	u4 nChannelIDX;

	u4 nBossMonsterType;	// 생성된 보스 몬스터의 타입
	u4 nCreatedCount;		// 생성된 보스 몬스터의 개체수

	pop(nMapIDX);
	pop(nChannelIDX);

	pop(nBossMonsterType);
	pop(nCreatedCount);

	CsMonster::sINFO* pMon = nsCsFileTable::g_pMonsterMng->GetMonster(nBossMonsterType)->GetInfo();
	assert_cs(pMon != NULL);

	std::wstring wMapDiscript = nsCsMapTable::g_pMapListMng->GetList(nMapIDX)->GetInfo()->s_szMapDiscript;
#ifdef VERSION_USA
	cPrintMsg::PrintMsg(30171, const_cast<TCHAR*>(wMapDiscript.c_str()), pMon->s_szName, &nChannelIDX);
#else
	cPrintMsg::PrintMsg(30171, const_cast<TCHAR*>(wMapDiscript.c_str()), &nChannelIDX, pMon->s_szName);
#endif
	ChannelContents::CHANNELINFO ChannelData;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::GET_CHANNELINFO, &ChannelData);

	if (ChannelData.channel_idx != nChannelIDX)
		return;
	if (g_pTerrainMng->GetBackupMapID() != nMapIDX)
		return;

	g_pGameIF->GetBossScene()->SetEmergency(nBossMonsterType, nCreatedCount);
}

void cCliGame::RecvHatchOptionItemAddResult()
{
	u1 nType;
	pop(nType);

	if (g_pGameIF && g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAKE_TACTICS))
	{
		g_pGameIF->GetMakeTactics()->RecvResult(cMakeTactics::DEL_BACKUP);
		g_pGameIF->GetMakeTactics()->RecvServer();

		int nLevel = g_pDataMng->GetTactics()->GetTacticsCount();

		GS2C_RECV_MAKE_DIGITAMA_LV recv;
		recv.m_nEggLevel = nLevel;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::MAKETACTICS_DIGITAMA_LV, &recv);

		cPrintMsg::PrintMsg(30503);
#ifdef MINIGAME
		GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT kRecv;
		kRecv.nResult = false;
		kRecv.nSuccessCnt = 0;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_MINIGAME_INIT, &kRecv);
#endif
	}
}

void cCliGame::RecvPortableItemFail(void)
{
	// 오직 사용기간 만료 일때만 받겠다.
	n4 nSubType = 0;
	pop(nSubType);

	switch (nSubType)
	{
	case nItem::Portable_Hatch:// 휴대용 부화기
	{
		if (g_pGameIF && g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAKE_TACTICS))
		{
			cBaseWindow* pWin = g_pGameIF->GetDynamicIF(cBaseWindow::WT_MAKE_TACTICS);
			if (pWin)
			{
				cMakeTactics* pMakeTactics = static_cast<cMakeTactics*>(pWin);
				pMakeTactics->RecvServer();
			}
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_MAKE_TACTICS, 0, true);
		}
	}break;
	case nItem::Portable_Scanner:	// 휴대용 스캔너 이용 아이템
	{
		UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_CLOSE, &nScanWT);
	}break;
	case nItem::Portable_WareHouse:// 휴대용 창고 이용 아이템
	{
		if (g_pGameIF)
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_WAREHOUSE, 0, true);
	}break;
	case nItem::Portable_DigimonCareHouse: // 휴대용 디지몬 보관소 이용 아이템
	{
		if (g_pGameIF)
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_TACTICSHOUSE, 0, true);
	}break;
	case nItem::Portable_DigitamaCollect:	// 휴대용 아이템 회수 이용 아이템
	{
		UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::DIGITAMA_CLOSE, &nSellWT);
	}break;
	case nItem::Portable_Analysis:// 휴대용 속성 분석기
	case nItem::Portable_Socket:// 휴대용 장비 합성기
#ifdef LJW_EQUIPSYSTEM_190724
		GAME_EVENT_ST.OnEvent(EVENT_CODE::CLOSE_EQUIPSYSTEM, NULL);
#else
		GAME_EVENT_ST.OnEvent(EVENT_CODE::CLOSE_ELEMENT_ITEM, NULL);
#endif
		break;
	case nItem::Portable_JumpGate:// 점프부스터 아이템 기간 만료
	{
		if (g_pGameIF)
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_JUMPBUSTER, 0, true);
	}break;
	default:
		assert_cs(false);
		break;
	}

	// 사용기간 종료
	cPrintMsg::PrintMsg(30362);
}

void cCliGame::RecvDigimonMoveResult()
{
	//int Src = 0;//int Dst = 0;//u4 Price = 0;
	GS2C_RECV_ARCHIVE_CHANGE RecvData;
	pop(RecvData.Src);
	pop(RecvData.Dst);
	pop(RecvData.Price);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_CHANGE_DIGIMONS_ARCHIVE, &RecvData);
}

void cCliGame::RecvDigimonMoveResultInArchive(void)
{
	GS2C_RECV_Archive_Internal_Move RecvData;
	pop(RecvData.nSlot1);
	pop(RecvData.nSlot2);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MOVE_DIGIMONS_IN_ARCHIVE, &RecvData);
}

void cCliGame::RecvPlayTimePaneltyChange()
{
#ifdef PLAY_PANELTY

	u2 nPaneltyType = 0;		// 패널티 타입
	u4 CashTime = 0;			// 유료 플레이 시간
	u4 NormalTime = 0;			// 일반 플레이 시간

	u4 oldCashTime = g_pDataMng->PlayTime_Get_CashTime();

	pop(nPaneltyType);
	pop(CashTime);
	pop(NormalTime); // 금일 플레이 가능한 남은 시간(단위-초)

	g_pDataMng->PlayTime_Set_PaneltyType(nPaneltyType);
	g_pDataMng->PlayTime_Set_CashTime(CashTime);
	g_pDataMng->PlayTime_Set_NormalTime(NormalTime);


	switch (nPaneltyType)
	{
	case nPlayTime::LEVEL1_PANELTY:
	{
		int nRTime = g_pDataMng->PlayTime_Get_BaseFullTime() - g_pDataMng->PlayTime_Get_NormalTime();
		int nH = nRTime / 3600;
		nRTime = nRTime % 3600;
		int nM = nRTime / 60;

		TCHAR sz[64];
		_stprintf_s(sz, 64, L"%.2d : %.2d", nH, nM);

		cPrintMsg::PrintMsg(30415, sz);
	}
	break;
	case nPlayTime::LEVEL2_PANELTY:
	{
		g_pDataMng->PlayTime_GameResetTime_PrintMsg();
	}
	break;
	case nPlayTime::USE_CASH:
	{
		int nAddCashTime = g_pDataMng->PlayTime_Get_CashTime() - oldCashTime;
		int nH = nAddCashTime / 3600;
		nAddCashTime = nAddCashTime % 3600;
		int nM = nAddCashTime / 60;

		TCHAR sz[64];
		_stprintf_s(sz, 64, L"%.2d : %.2d", nH, nM);

		cPrintMsg::PrintMsg(30418, sz);
	}
	break;
	}

	g_pGameIF->GetPlayTime()->CalServerDay();

#endif
}

void cCliGame::RecvCropGiftInvenFail()
{
	u1 nResult = 0;

	pop(nResult);

	switch (nResult)
	{
	case nItem::GiftF_ITEMTYPE:	// 아이템 타입 에러
	case nItem::GiftF_INVEN_NOT_EMPTY:	// 인벤 공간 부족
	case nItem::GiftF_CROP_FAIL:	// 아이템 획득 처리 실패 ( 내부 실패 )
	case nItem::GiftF_COUNT_OVER:	// 아이템의 중첩개수 에러로 획득할 수없는 경우
		break;
	default: break;
	}
	if (cClient::IsSendBlock())
		cClient::SetSendBlock(false);
}

void cCliGame::RecvGiftBoxUseSuccess()
{
	u4 nTargetUID = 0;
	u2 nInvenPos = 0;

	u4 nUsedItemType = 0;
	pop(nUsedItemType);

	pop(nTargetUID);
	pop(nInvenPos);

	g_pDataMng->ServerItemUseSuccess(nTargetUID, nInvenPos);

	u4 nCount = 0;
	pop(nCount);

	for (u4 i = 0; i < nCount; ++i)
	{
		cItemData data;

		u4 nType = 0;
		pop(nType);
		data.m_nType = nType;

		u4 nItemCount = 0;
		pop(nItemCount);
		data.m_nCount = nItemCount;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		u4 nRemainTradeLimitTime = 0;
		pop(nRemainTradeLimitTime);

		data.m_nRemainTradeLimitTime = nRemainTradeLimitTime;
#endif

		// rate 값이 존재하는 아이템을 바로 위탁상점에 올릴 경우
		// 서버와 동기화가 맞지 않아서 끊기는 문제 있음
		// 서버에서 아이템을 받아오도록 수정해야 함
		data.m_nRate = 0;	// 아이템 능력치 적용 비율 - 0 == 클라에서 임의로 세팅한 값
		data.m_nLevel = 1;
		nsInventory::sGiftBox giftBox;
		giftBox.nUsedItemType = nUsedItemType;
		giftBox.itemData = data;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_GIFTBOX_WINDOW, &giftBox);
	}
}

void cCliGame::RecvCardEventStart()	// 카드 이벤트 시작 시 서버에서 데이터를 받음
{
	u8 nMoney = 0;
	u4 nItemID = 0;
	int nItemCount = 0;

	pop(nMoney);
	pop(nItemID);
	pop(nItemCount);

	if (g_pDataMng)
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if (pInven)
			pInven->SetMoney(nMoney, false);	// 돈 설정
	}

	std::pair<DWORD, int> result;
	result.first = nItemID;
	result.second = nItemCount;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_CARD_EVENT_RESULT, &result);
}

void cCliGame::RecvCardEventEnd()
{
	int nResult = 0;
	pop(nResult);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_CARD_EVENT_END, &nResult);
}

void cCliGame::RecvNatureExp()
{
	//namespace NewAttribute
	u1 uMainType;	// 메인 타입, 0~1,	실패시 255
	u1 uSubType;	// 서브 타입, 버튼 클릭후 실패시 255 
	n2 nExp;	// 0

	/*
	Da,	// 2 Data
	Va,	// 3 Vaccine
	Vi,	// 4 Virus
	*/


	cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();
	cData_PostLoad::sDATA* pDigimonData = pPostLoad->GetDigimonData();

	pop(uMainType);
	pop(uSubType);
	pop(nExp);


	// 기본속성
	if (NewAttribute::Digital == uMainType)
	{
		pDigimonData->s_AttributeExp[uSubType] += nExp;
		g_pCharMng->GetDigimonUser(0)->SetAttributeExp(pDigimonData->s_AttributeExp);

		if (-10000 == nExp)
		{
			CsElementItem::sINFO* pItem = nsCsFileTable::g_pItemMng->GetElementItem1(uSubType)->GetInfo();

			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			if (pTamerUser == NULL)
				return;


			cItemData item;
			item.m_nType = pItem->s_dwItemID;
			item.m_nCount = 1;
			item.m_nRate = 0;
			item.m_nLevel = 0;
			item.m_nLimited = 0;

			pTamerUser->RaidItemCropSuccess(&item, 30519);
		}
		else if (10000 == pDigimonData->s_AttributeExp[uSubType])
		{
			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			if (pTamerUser == NULL)
				return;
			pTamerUser->PlaySound(SOUND_ELEMENTUP);
			cPrintMsg::PrintMsg(30521 + uSubType);
		}



	}
	// 자연속성이면
	else if (NewAttribute::Natual == uMainType)
	{
		pDigimonData->s_NatureExp[uSubType] += nExp;
		g_pCharMng->GetDigimonUser(0)->SetNatureExp(pDigimonData->s_NatureExp);
		if (-10000 == nExp)
		{
			CsElementItem::sINFO* pItem = nsCsFileTable::g_pItemMng->GetElementItem2(uSubType)->GetInfo();

			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			if (pTamerUser == NULL)
				return;


			cItemData item;
			item.m_nType = pItem->s_dwItemID;
			item.m_nCount = 1;
			item.m_nRate = 0;
			item.m_nLevel = 0;
			item.m_nLimited = 0;

			pTamerUser->RaidItemCropSuccess(&item, 30519);
		}
		else if (10000 == pDigimonData->s_NatureExp[uSubType])
		{
			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			if (pTamerUser == NULL)
				return;
			pTamerUser->PlaySound(SOUND_ELEMENTUP);
			cPrintMsg::PrintMsg(30524 + uSubType);
		}

	}
	else if (255 == uMainType)
	{
		if (g_pDataMng->GetInven()->GetEmptySlotCount() == 0)
		{
			cPrintMsg::PrintMsg(11015);

		}
	}
}

// 오른쪽 버튼으로 일부 알(이벤트) 부화되는 기능
void cCliGame::RecvRClickDigimonHatch(void)
{
	u1 nSuccess;
	pop(nSuccess);	// 부화 성공 실패 여부 (1 : 성공, 0: 실패)

	if (nSuccess != 0)
	{
		int nInvenPos;
		pop(nInvenPos);	// 사용 Item Inven Position

		u4 nArrIDX;
		pop(nArrIDX);

		cData_PostLoad::sDATA Tactics;

		pop(Tactics.s_Type);
		pop(Tactics.s_szName);

		u2 nScale;
		pop(nScale);
		Tactics.s_fScale = nScale * 0.0001f;
		pop(Tactics.s_nExp);

		Tactics.s_nExp = Tactics.s_nExp / 100;// 서버에서 경험치가 백분률로 오기 때문임

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		pop(Tactics.s_nTranscendenceExp);
#endif
		pop(Tactics.s_nLevel);
		pop(Tactics.s_Attribute, sizeof(Tactics.s_Attribute));

		pop(Tactics.s_HatchLevel);

		pop(Tactics.s_dwBaseDigimonID);
		DBG("nBaseEvoUnitIDX : %d", Tactics.s_dwBaseDigimonID);

		pop(Tactics.s_nMaxEvoUnit);
		pop(&Tactics.s_EvoUnit[1], sizeof(cEvoUnit) * Tactics.s_nMaxEvoUnit);

		memset(&Tactics.s_AttributeExp, NULL, sizeof(n2) * NewAttribute::MaxDigitalType);
		memset(&Tactics.s_NatureExp, NULL, sizeof(n2) * NewAttribute::MaxNatualType);

		u2 nInchantLevel = 0;
		u2 nExtentionParameter[6] = { 0, };
		pop(Tactics.s_nEnchantLevel);
		pop(Tactics.s_ExtendAttribute, sizeof(Tactics.s_ExtendAttribute));
		pop(Tactics.s_ExtendAttributeLV, sizeof(Tactics.s_ExtendAttributeLV));

		g_pDataMng->GetTactics()->AddTactics(&Tactics, nArrIDX - 1);

		//=============================================================================
		//	인벤 내 아이템 감소 및 부화 애니메이션 활성화
		//=============================================================================
		g_pTacticsAni->Set_bAniProcess(true);	// 부화 애니 시작위치 설정
		g_pTacticsAni->Start(g_pDataMng->GetInven()->GetData(nInvenPos)->GetType(), g_pCharMng->GetTamerUser());	// 부화 애니메이션 활성화
		g_pDataMng->GetInven()->GetData(nInvenPos)->DecreaseCount(1);	// 사용한 Item 인벤 내 감소		

		//==========================================================================================================
		// 업적 체크
		//==========================================================================================================
		GS2C_RECV_CHECKTYPE recvAchieve;
		recvAchieve.nType = AchieveContents::CA_GetTactics;
		recvAchieve.nValue1 = Tactics.s_Type.m_nType;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recvAchieve);

		GS2C_RECV_ENCYCLOPEDIA_EVOLUTION recv;
		recv.nDigimonID = Tactics.s_dwBaseDigimonID;
		recv.nScale = nScale;
		recv.nType = 0;
		for (int i = 1; i < nLimit::EvoUnit; i++)
		{
			recv.nEvoUnit[i] = Tactics.s_EvoUnit[i];
		}
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_EVOL_UPDATE, &recv);
		GS2C_RECV_EXTEND_ACCOMPANY_SLOT kRecvdata;
		kRecvdata.OpenedCnt = 0;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ON_EXTENT_ACCOMPANY_SLOT, &kRecvdata);

	}
	else
	{
		cPrintMsg::PrintMsg(30535);	// 디지바이스에 디지몬이 꽉 차서 부활할 수 없습니다. (메세지 박스)
	}

}

#ifdef MINIGAME
void cCliGame::RecvMinigameStart()
{
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_START kRecv;
	pop(kRecv.nStage);
	pop(kRecv.nBarTime);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_START_MINIGAME, &kRecv);
}

void cCliGame::RecvMinigameClickBtn()
{
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_CLICKRESULT kRecv;

	pop(kRecv.nResult);
	pop(kRecv.nBarIndex);
	pop(kRecv.nBarTime);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_CLICK_RESULT, &kRecv);
}

void cCliGame::RecvMinigameTimeOut()
{
	//시간 지난거니까 실패로 처리해줘야함
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_TIMEOUT kRecv;
	pop(kRecv.nBarTime);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_TIME_OUT, &kRecv);

}

void cCliGame::RecvMinigameLimit()
{
	//미니게임 실행 하면 안되는 상황에서 실행했을 때 예외 처리
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_ERRORMESSAGE kRecv;
	pop(kRecv.nErrorCode);
	pop(kRecv.nSuccessCnt);	//이전 시도에 성공한 횟수

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_ERROR_MESSAGE, &kRecv);

}

void cCliGame::RecvMinigameEnd()
{
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_END kRecv;
	pop(kRecv.nSuccess);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_END_MINIGAME, &kRecv);

}
#endif

void cCliGame::RecvGotchaOpen()
{
#if 0 // our common lib
	GS2C_RECV_OPEN_GOTCHAMACHINE recv;
	pop(recv.nRareItemCount);
	for (int i = 0; i < recv.nRareItemCount; ++i)
	{
		u4 nItemID = 0;
		pop(nItemID);
		u4 nItemCount = 0;
		pop(nItemCount);
		cItemData itemData;
		itemData.Set(nItemID, nItemCount);
		recv.lRareItems.push_back(itemData);
	}
	pop(recv.nNormalItemCount);
	pop(recv.nResetCount);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_OPEN_GOTCHAMACHINE, &recv);
#endif
}

void cCliGame::RecvGotchaGet()
{
#if 0 // our common lib
	GS2C_RECV_GET_ITEM_GOTCHAMACHINE recv;
	pop(recv.nKind);
	pop(recv.nItemID);
	pop(recv.nItemCount);
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	pop(recv.nRemainTradeLimitTime);
#endif

	// refresh list
	pop(recv.nRareItemCount);
	for (int i = 0; i < recv.nRareItemCount; ++i)
	{
		u4 nItemID = 0;
		pop(nItemID);
		u4 nItemCount = 0;
		pop(nItemCount);
		cItemData itemData;
		itemData.Set(nItemID, nItemCount);
		recv.lRareItems.push_back(itemData);
	}
	pop(recv.nNormalItemCount);
	pop(recv.nResetCount);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_GET_ITEM_GOTCHAMACHINE, &recv);
#endif
}

void cCliGame::RecvGotchaError()
{
	u4 nErrCode;
	pop(nErrCode);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ERROR_GOTCHAMACHINE, &nErrCode);
}

void cCliGame::RecvGotchaRefresh()
{

}

void cCliGame::RecvMysteryGet()
{
#if 0 // our common lib
	GS2C_RECV_GET_ITEM_MYSTERYMACHINE recv;
	pop(recv.nItemID);
	pop(recv.nItemCount);
	pop(recv.nItemGroupID);
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	pop(recv.nRemainTradeLimitTime);
#endif
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_GET_ITEM_MYSTERYMACHINE, &recv);
#endif
}

#ifdef ADVENTURE_TAMER_CHANGE
void cCliGame::RecvMoveClear()
{
	u2 nEquipCount;
	n2 nDestID, nSrcID;

	pop(nEquipCount);

	for (u2 i = 0; i < nEquipCount; ++i)
	{
		pop(nDestID);
		pop(nSrcID);
		g_pDataMng->GetTEquip()->ToInven(TO_ID(nDestID), TO_ID(nSrcID));
	}
}
#endif
//#ifdef FIREWORK
void cCliGame::RecvFireWork()
{
	u4	uIDX = 0;
	u4	uItemID = 0;

	pop(uIDX);
	pop(uItemID);

	uint nUIDX = GetIDX(uIDX);

	CTamer* pTamer = NULL;
	pTamer = (CTamer*)g_pCharMng->GetTamer(nUIDX);

	if (pTamer)
	{
		pTamer->SetEvent(uItemID);
	}
}
//#endif

void cCliGame::RecvGlobalMsg()
{
	n2	 nCase = 0;
	pop(nCase);
	assert_cs(nCase >= 1);

	cGlobalMessage* pMsg = g_pGameIF->GetGlobalMsg();
	switch (nCase)
	{
	case 1:	// 레벨업 아이템 사용 시 서버의 모든 유저에게 메세지 보여줌
	{
		std::wstring wsTamerName;
		pop(wsTamerName/*, sizeof( char) * Language::pLength::name*/);

		u4 nItemNo = 0;
		pop(nItemNo);

		u4 nLevel = 0;
		pop(nLevel);

		std::wstring wsItemName;
		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(nItemNo);
		if (pFTItem)
		{
			wsItemName = pFTItem->GetName();
			if (wsItemName.empty())
				wsItemName = L"Unknown Item";
		}

		std::wstring wsLevel;
		DmCS::StringFn::Format(wsLevel, L"%d", nLevel);

		pMsg->SetMessage(nCase, wsTamerName.c_str(), wsItemName.c_str(), wsLevel.c_str());
	}
	break;

	case pMonster::RandomMonsterCreate:	// 랜덤리젠 몬스터 생성 메세지
	{
		char szMonName[MONSTER_NAME_LEN] = { 0, };

		u4 MonIDX = 0;
		pop(MonIDX);
		u4 MapID = 0;
		pop(MapID);

		std::wstring wsMonsterName;
		CsMonster* pMonster = nsCsFileTable::g_pMonsterMng->GetMonster(MonIDX);
		if (pMonster)
		{
			CsMonster::sINFO* pMonsterInfo = pMonster->GetInfo();
			if (pMonsterInfo)
			{
				wsMonsterName = pMonsterInfo->s_szName;
				if (wsMonsterName.empty())
					wsMonsterName = L"Unknown Monster";
			}
			pMsg->SetMessage(cGlobalMessage::RANDOMREGENBEGIN, wsMonsterName.c_str());
		}

		//g_pAchieveInfo->GetRandomRegen()->SetRandomRegenStart( pMonster->GetInfo()->s_szName );
	}
	break;

	case pMonster::RandomMonsterEnd:	// 랜덤리젠 몬스터 파괴 메세지
	{
		u4 MonIDX = 0;
		pop(MonIDX);
		u4 MapID = 0;
		pop(MapID);

		std::wstring wsMonsterName;
		CsMonster* pMonster = nsCsFileTable::g_pMonsterMng->GetMonster(MonIDX);
		if (pMonster)
		{
			CsMonster::sINFO* pMonsterInfo = pMonster->GetInfo();
			if (pMonsterInfo)
			{
				wsMonsterName = pMonsterInfo->s_szName;
				if (wsMonsterName.empty())
					wsMonsterName = L"Unknown Monster";
			}
			pMsg->SetMessage(cGlobalMessage::RANDOMREGENEND, wsMonsterName.c_str());
		}
	}
	break;

	case pMonster::MsgRaidMonsterDie:
	{
		u4 nMonsterIdx = 0;
		pop(nMonsterIdx);

		std::wstring wsTamerName;
		pop(wsTamerName);

		u4 nDigimon = 0;
		pop(nDigimon);

		u4 nItemIDX = 0;
		pop(nItemIDX);

		std::wstring wsMonsterName;
		CsMonster* pFTMonster = nsCsFileTable::g_pMonsterMng->GetMonster(nMonsterIdx);
		if (pFTMonster)
		{
			CsMonster::sINFO* pMonsterInfo = pFTMonster->GetInfo();
			if (pMonsterInfo)
				wsMonsterName = pMonsterInfo->s_szName;
			if (wsMonsterName.empty())
				wsMonsterName = L"Unknown Monster";
		}

		// 			CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimon );
		// 			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemIDX );
		// 
		// 			W2M( szDigimonName, pDigimon->GetInfo()->s_szName, 64 );
		// 			W2M( szItemName, pFTItem->GetInfo()->s_szName, 64 );

		std::wstring wsDigimonName;
		CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(nDigimon);
		if (pDigimon)
		{
			CsDigimon::sINFO* pDigimonInfo = pDigimon->GetInfo();
			if (pDigimonInfo)
				wsDigimonName = pDigimonInfo->s_szName;
			if (wsDigimonName.empty())
				wsDigimonName = L"Unknown Digimon";
		}

		// 하드 코딩임 나중에 서버와 협의 후 수정 필요함.
		std::wstring wsItemName;
		if (0 != nItemIDX)
		{
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(nItemIDX);
			if (pFTItem)
			{
				wsItemName = pFTItem->GetName();
				if (wsItemName.empty())
					wsItemName = L"Unknown Item";
			}
		}
		else
		{
			wsItemName = UISTRING_TEXT("GLOBAL_MESSAGE_VERDANDI_PEACE");
		}

		pMsg->SetMessage(cGlobalMessage::FILEBLESS, wsMonsterName.c_str(), wsTamerName.c_str(), wsDigimonName.c_str(), wsItemName.c_str());
	}
	break;
	default:
		assert_cs(false);
	}
}

// 이미 받았던 리스트를 받는다.
void cCliGame::RecvRequestRewardList()
{
	u4 count = 0;
	u1 idx = 0;

	g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::REWARD_REFRESH, 0);

	pop(count);

	//2017-03-15-nova
	GS2C_RECV_REWARD_DATA recv;
	for (u4 i = 0; i < count; ++i)
	{
		pop(idx);
		recv.AddData(idx);
	}
	GAME_EVENT_ST.OnEvent(EVENT_CODE::REWARD_RECEIVE_REWARD, &recv);
}
// 버튼 누른후 아이템 받기 처리
void cCliGame::RecvRequestRewards()
{
	u1 count = 0;
	u1 idx = 0;

	CsReward::MAP* pMapReward = nsCsFileTable::g_pRewardMng->GetReward(g_pResist->m_AccountGlobal.s_nLastConnetServer);
	// 테이블 블러오기 실패, UI 닫아준다.
	if (NULL == pMapReward)
	{
		if (true == g_pGameIF->IsActiveWindow(cBaseWindow::WT_REWARD))
		{
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_REWARD);
		}
	}

	pop(count);

	for (int i = 0; i < count; i++)
	{
		pop(idx);

		if (pMapReward)
		{
			CsReward* preward = (*pMapReward)[idx];
			if (NULL != preward)
			{
				int nItemIndex = preward->GetInfo()->s_nItemCode1;
				int nItemCount = preward->GetInfo()->s_nItemCount1;

				// [4/14/2016 hyun] 수정. 아이템 획득시 컬러값으로 출력을 위해
				// 보상 받았다면 메세지
				CsItem::sINFO* pItem = nsCsFileTable::g_pItemMng->GetItem(nItemIndex)->GetInfo();
				if (pItem)
				{
					if (nItemCount == 1)
						cPrintMsg::PrintMsg(cPrintMsg::GAME_ITEM_CROP, pItem->s_szName, &g_pDataMng->GetItemColor(pItem));
					else if (nItemCount > 1)
						cPrintMsg::PrintMsg(cPrintMsg::GAME_ITEM_MANYCROP, pItem->s_szName, &nItemCount, &g_pDataMng->GetItemColor(pItem));
				}
				// @@ 여기까지

				g_pDataMng->GetInven()->ItemCrop(nItemIndex, nItemCount);

				// 리스트에 있는 아이템 지워준다.
//				g_pDataMng->GetCash()->DeleteRewardItem( idx );

				GS2C_RECV_REWARD_SEND_TYPE recv;
				recv.iType = 4;
				recv.iIndex = idx;
				GAME_EVENT_ST.OnEvent(EVENT_CODE::REWARD_SEND_ACCPET_DATA, &recv);

			}
		}
	}

	g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::GET_REWARD, 0);

}
void cCliGame::RecvRequestRewardFail()
{
	u1 type = 0;
	pop(type);
}

void cCliGame::RecvMastersMatchOpen()
{
	//열어보자
	g_pGameIF->GetDynamicIF(cBaseWindow::WT_MASTERS_MATCHING);

	//타겟 가져와서
	CsC_AvObject* pTarget = g_pGameIF->GetMastersMatchTarget();
	//타겟 셋팅
	g_pGameIF->GetMastersMatching()->SetTarget(pTarget);

	//다음 리셋 시간
	u4 nResetTime = 0;
	char	szResetTime[32];

	pop(szResetTime); // 00:00:00

	strcpy(g_pGameIF->m_sMyInfo.s_szNextResetTime, szResetTime);

	pop(nResetTime); // 틱시간
	g_pGameIF->m_sMyInfo.s_nNextResetTime = nResetTime;

	//각 팀 기부 갯수
	u4 nTeamCnt = 2345;
	for (int i = 0; i < B_TEAM; i++)
	{
		pop(nTeamCnt);

		g_pGameIF->m_sRankInfo.s_nCnt[i] = nTeamCnt;
		g_pGameIF->GetMastersMatching()->_SetTeamCnt(i + 1, nTeamCnt);
	}

	//////////////////////////////본인 정보/////////////////////////////////////
	u4 nMyCnt = 52;
	u2 nMyRank = 62;
	u1 nMyTeam = 1;
	pop(nMyCnt);
	pop(nMyRank);
	pop(nMyTeam);

	g_pGameIF->m_sMyInfo.s_nMyCnt = nMyCnt;
	g_pGameIF->m_sMyInfo.s_nMyRank = nMyRank;
	//팀 결정 안되있으면 아이콘 덤으로 셋팅
	if (g_pCharMng->GetTamerUser()->GetMatchTeam() == NONE)
		g_pCharMng->GetTamerUser()->GetCharOption()->SetMatchIcon(nMyTeam);
	g_pGameIF->m_sMyInfo.s_eMyTeam = (eTEAM)nMyTeam;


	///////////////////////////////////////////////////////////////////////////////

	////////////////////////////////랭커 정보/////////////////////////////////////
	u2 nRank = 111;							//랭커 랭킹
	//char szTamer[ Language::pLength::id + 5 ];		//랭커 이름
	u4 nCount = 112;						//랭커 기부갯수

	//TCHAR	tszTamer[ Language::pLength::id + 5 ];
// 	_tcscpy_s( tszTamer, Language::pLength::id + 5, _T("테이머10등까지순위") );

	cStringList* pRankList_A = NiNew cStringList;	//A팀 리스트
	cStringList* pRankList_B = NiNew cStringList;	//B팀 리스트
	// 리스트 순서 :  A팀 ( 랭킹 - 이름 - 기부갯수) * 10 
	//					   B팀 ( 랭킹 - 이름 - 기부갯수) * 10 

	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init(&g_pEngine->m_FontSystem);

	// A팀
	for (u4 i = 0; i < 10; i++)//10등
	{
		pop(nRank);
		if (nRank == 0) // 0이면 해당 등수부터 10등까지는 미등록상태 셋팅
		{
			for (int j = i; j < 10; j++)
			{
				pString = NiNew cString;
				//랭킹
				ti.s_eTextAlign = DT_CENTER;
				ti.SetText(j + 1);
				ti.AddText(L".");
				pString->AddText(&ti);
				pString->TailAddSizeX(20 - pString->GetMaxSize().x);
				//이름
				ti.s_eTextAlign = DT_LEFT;
				ti.SetText(UISTRING_TEXT("COMMON_TXT_NOT_REGIST").c_str());
				pString->AddText(&ti);
				pString->TailAddSizeX(170 - pString->GetMaxSize().x);
				//기부갯수
				ti.s_eTextAlign = DT_RIGHT;
				ti.SetText(_T("0"));
				pString->AddText(&ti);

				pRankList_A->AddTail(pString);
			}
			break;
		}
		std::wstring szTamer;	//랭커 이름
		pop(szTamer);
		pop(nCount);

		pString = NiNew cString;
		//랭킹
		ti.SetText(nRank);
		ti.AddText(L".");
		ti.s_eTextAlign = DT_CENTER;
		pString->AddText(&ti);
		pString->TailAddSizeX(20 - pString->GetMaxSize().x);
		//이름 + 삭제된 아이디 검사
		int nOrg = szTamer.size();
		std::wstring wsToken = L"_";
		std::wstring wsTempstr = DmCS::StringFn::tokenize(szTamer, wsToken);
		if (nOrg != wsTempstr.size())
		{
			//'_' 앞부분이니까 아이디 얻으려면 한번더 잘라줘야함
			wsTempstr = DmCS::StringFn::tokenize(szTamer, wsToken);
		}
		ti.SetText(wsTempstr.c_str());
		ti.s_eTextAlign = DT_LEFT;
		pString->AddText(&ti);
		pString->TailAddSizeX(170 - pString->GetMaxSize().x);
		//기부갯수
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText(nCount);
		pString->AddText(&ti);

		//A팀 리스트 Add
		pRankList_A->AddTail(pString);
	}
	g_pGameIF->m_sRankInfo.s_pRankList_A = pRankList_A;

	// B팀
	for (int i = 0; i < 10; i++)
	{
		pop(nRank);
		if (nRank == 0) // 0이면 해당 등수부터 10등까지는 미등록상태 셋팅
		{
			for (int j = i; j < 10; j++)
			{
				pString = NiNew cString;
				//랭킹
				ti.s_eTextAlign = DT_CENTER;
				ti.SetText(j + 1);
				ti.AddText(L".");
				pString->AddText(&ti);
				pString->TailAddSizeX(20 - pString->GetMaxSize().x);
				//이름
				ti.s_eTextAlign = DT_LEFT;
				ti.SetText(UISTRING_TEXT("COMMON_TXT_NOT_REGIST").c_str());
				pString->AddText(&ti);
				pString->TailAddSizeX(170 - pString->GetMaxSize().x);
				//기부갯수
				ti.s_eTextAlign = DT_RIGHT;
				ti.SetText(_T("0"));
				pString->AddText(&ti);

				pRankList_B->AddTail(pString);
			}
			break;
		}
		std::wstring szTamer;	//랭커 이름
		pop(szTamer);
		pop(nCount);

		pString = NiNew cString;
		//랭킹
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(nRank);
		ti.AddText(L".");
		pString->AddText(&ti);
		pString->TailAddSizeX(20 - pString->GetMaxSize().x);

		//이름 + 삭제된 아이디 검사
		std::wstring wsToken = L"_";
		int nOrg = szTamer.size();
		std::wstring wsTempstr = DmCS::StringFn::tokenize(szTamer, wsToken);
		if (nOrg != wsTempstr.size())
		{
			//'_' 앞부분이니까 아이디 얻으려면 한번더 잘라줘야함
			wsTempstr = DmCS::StringFn::tokenize(szTamer, wsToken);
		}

		ti.s_eTextAlign = DT_LEFT;
		ti.SetText(wsTempstr.c_str());
		pString->AddText(&ti);
		pString->TailAddSizeX(170 - pString->GetMaxSize().x);
		//기부갯수
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText(nCount);
		pString->AddText(&ti);

		//B팀 리스트 Add
		pRankList_B->AddTail(pString);
	}
	g_pGameIF->m_sRankInfo.s_pRankList_B = pRankList_B;

	//리스트 업데이트
	g_pGameIF->GetMastersMatching()->RefreshList();
	///////////////////////////////////////////////////////////////////////////////
		//오픈 중 메세지 삭제
	cMessageBox::DelMsg(10019, false);
}

void cCliGame::RecvMastersMatchInsert()
{
	//전체 락풀고
	g_pDataMng->ReleaseItemLock();

	u4 nSlot = g_pGameIF->GetMastersMatching()->m_nItemSlot;			// 기부할 아이템 있는 인벤 번호
	u4 nItemCnt = g_pGameIF->GetMastersMatching()->m_nItemCnt;		// 기부 갯수

	//업적 확인
	g_pGameIF->GetMastersMatching()->_CheckAchieve(nItemCnt);

	//자신이 기부한 총 갯수(화면에 보여지는) ++
	g_pGameIF->m_sMyInfo.s_nMyCnt += nItemCnt;

	//기부아이템 감소
	g_pDataMng->GetInven()->GetData(nSlot)->DecreaseCount(nItemCnt);

	//에디트박스 0으로
	g_pGameIF->GetMastersMatching()->_SetCount(0, true);

	//리스트 업데이트
	g_pGameIF->GetMastersMatching()->RefreshList();

	//오픈 중 메세지 지워
	cMessageBox::DelMsg(10019, false);
	//아이템 기부 완료 메세지
	cPrintMsg::PrintMsg(30622, &nItemCnt);
}

#if !COMMON_LIB_FIXED
struct GS2C_RECV_ENCHANT_ITEM_IDENTIFY
{
	u2 nInvenPos;
	u1 nRate;
	u1 nLevel;
	u2 nAccOption[nLimit::MAX_ACCESSORY_OPTION];
	u2 nAccValues[nLimit::MAX_ACCESSORY_OPTION];
};

struct GS2C_RECV_ENCHANT_ITEM_ENCHANT
{
	u1 nResult;
	u2 nInvenPos;
	u1 nRate;
	u1 nLevel;
	u2 nAccOption[nLimit::MAX_ACCESSORY_OPTION];
	u2 nAccValues[nLimit::MAX_ACCESSORY_OPTION];
};
#endif

void cCliGame::RecvEnchantItemIdentify()
{
	GS2C_RECV_ENCHANT_ITEM_IDENTIFY recv;
	pop(recv.nInvenPos);		// 악세 인벤 위치
	pop(recv.nRate);			// 디지터리 수치( 95~200% )
	pop(recv.nLevel);			// 디지어블 파워 리뉴얼( 횟수 )
	pop(recv.nAccOption);		// 옵션 종류
	pop(recv.nAccValues);		// 옵션 능력치
	DBG("RecvEnchantItemIdentify: slot=%u rate=%u level=%u opt0=%u val0=%u",
		recv.nInvenPos, recv.nRate, recv.nLevel, recv.nAccOption[0], recv.nAccValues[0]);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ENCHANT_OPTION_IDENTIFY_RESULT, &recv);
}

void cCliGame::RecvEnchantItemEnchant()
{
	GS2C_RECV_ENCHANT_ITEM_ENCHANT recv;
	// 인첸 성공 여부
	// 1 : 성공, 2 : 실패, 3 : 보존
	pop(recv.nResult);
	pop(recv.nInvenPos);
	pop(recv.nRate);
	pop(recv.nLevel);
	pop(recv.nAccOption);
	pop(recv.nAccValues);
	DBG("RecvEnchantItemEnchant: result=%u slot=%u rate=%u level=%u opt0=%u val0=%u",
		recv.nResult, recv.nInvenPos, recv.nRate, recv.nLevel, recv.nAccOption[0], recv.nAccValues[0]);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ENCHANT_OPTION_ENCHANT_RESULT, &recv);
}

void cCliGame::RecvIncubaterList()		//	인큐베이터 오픈
{
	//	g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIGIMON_INCUBATOR );

	u4 nSlot = 0;				// 인큐베이터에 들어있는 디지몬의 디보 슬롯 번호 ( 1000 ~ )
	u4 nItemID = 0;				//	인큐베이터에 밖혀있는 아이템번호
	u4 nRemainTime = 0;		//	아이템 만료 시간


	const int MaxIncubatorCnt = 3;
	GS2C_RECV_INCUBATOR_INFO kIncuRecvInfo;

	for (int i = 0; i < 3; i++)
	{
		pop(nSlot);
		pop(nItemID);
		pop(nRemainTime);

		kIncuRecvInfo.AddIncuInfo(i, nSlot, nItemID, nRemainTime);
	}
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_INCUBATOR_INFO, &kIncuRecvInfo);

	//데이터 수신중 메세지
	cMessageBox::DelMsg(10019, false);

}

void cCliGame::RecvIncubatorFacilitation()		//	촉진
{
	u1 nSlot = 0;			//인큐베이터 슬롯
	u4 nRemainTime = 0;	//만료시간

	pop(nSlot);
	pop(nRemainTime);

	GS2C_RECV_START_INCUBATING_INFO kRecvData;
	kRecvData.Incu_slotIdx = nSlot;
	kRecvData.RemainTime = nRemainTime;

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_START_INCUBATING_INFO, &kRecvData);

	// 등록한 디지몬, 아이템을 Serv 데이터로 셋팅
	//g_pGameIF->GetTacticsHouse()->OnRecvFacilitation( nSlot, nRemainTime );
}

void cCliGame::RecvIncubatorExtraction()			//	추출 ( 꺼내기 )
{
	// 없어짐
	u1 nSlot = 0;		// 인큐베이터 슬롯번호
	pop(nSlot);


	GS2C_RECV_STOP_INCUBATING_INFO kRecvData;
	kRecvData.Incu_slotIdx = nSlot;

	// 등록되있는 디지몬, 아이템 제거
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_STOP_INCUBATING_INFO, &kRecvData);

	//g_pGameIF->GetIncubator()->_RecvExtraction( nSlot );
}

void cCliGame::RecvIncubaterSlotChange()
{
	n4 nIncuSlot = 0;				// 인큐베이터에 들어있는 디지몬의 디보 슬롯 번호 ( 1000 ~ )
	n4 nHouseSlot = 1000;				//	디보 슬롯(1000~)

	pop(nIncuSlot);
	if (nIncuSlot == -1)//실패
	{
		cPrintMsg::PrintMsg(11014);	// 해당 슬롯에 등록할 수 없습니다	
		//데이터 수신중 메세지
		cMessageBox::DelMsg(10019, false);
		return;
	}
	pop(nHouseSlot);

	GS2C_RECV_INCUBATOR_CHANGE kRecvData;
	kRecvData.Incu_slotIdx = nIncuSlot;
	kRecvData.Archive_slotIdx = nHouseSlot;
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_CHANGE_INCUBATOR, &kRecvData);
	//  g_pGameIF->GetIncubator()->_RecvDigimonChange( nIncuSlot, nHouseSlot );
}

void cCliGame::RecvIncubatorError()			//	예외상황 처리
{
	u4 nErrorCode;		// 에러코드
	pop(nErrorCode);

	enum eERR_CODE {
		ERR_FACILITATION = 1,
		ERR_2 = 2,
		ERR_3 = 3,
	};
	switch (nErrorCode)
	{
	case ERR_FACILITATION:	//해당 인큐베이터 슬롯에 디지몬 있을 때 디지몬 넣기 시도
	{
		//메세지 띄우고
		//인큐베이터창 끄기
	}
	break;
	case ERR_2:	// 
		break;
	}
}

void cCliGame::RecvSetMasterCardLeader()
{
	u4 nTamerID = 0;
	GS2C_RECV_SEALMASTER_LEADER  Leader;

	pop(nTamerID);
	pop(Leader.m_nLeader);

	CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject(nTamerID);
	if (pTamer != NULL)
	{
		CsMaster_CardLeader::sINFO* sInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCardLeader(Leader.m_nLeader)->GetInfo();


		CsMaster_Card::MAP_IT	it = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->begin();
		CsMaster_Card::MAP_IT	it_end = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->end();
		float fscale = 0.1f;

		for (;it != it_end; ++it)
		{
			if (it->second->GetInfo()->s_nDigimonID == sInfo->s_nDigimonID)
			{
				fscale = it->second->GetInfo()->s_nScale * 0.01f;
			}
		}

		pTamer->DeletePat();
		pTamer->AddPat(sInfo->s_nDigimonID, fscale);

		// 내 자신의 캐릭터일 때만 데이터를 수정하고 메시지를 보여줍니다.
		if (g_pCharMng->GetTamerUser()->GetUniqID() == nTamerID)
		{
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_SEALMASTER_LEADER, &Leader);
			cPrintMsg::PrintMsg(30654);	// 리더가 설정되었습니다.
		}
	}
}

void cCliGame::RecvUnSetMasterCardLeader()
{
	u4 nTamerID = 0;
	u2 nCardID = 0;

	pop(nTamerID);
	pop(nCardID);

	CTamer* pTamer = (CTamer*)g_pMngCollector->GetObject(nTamerID);
	if (pTamer != NULL)
	{
		pTamer->DeletePat();

		// 내 자신의 캐릭터일 때만 데이터를 수정합니다.
		if (g_pCharMng->GetTamerUser()->GetUniqID() == nTamerID)
		{
			GS2C_RECV_SEALMASTER_LEADER  Leader;	// 리더 없음 -1로 초기화 되어 있음
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_SEALMASTER_LEADER, &Leader);
		}
	}
}

void cCliGame::RecvCardInfoAll(void)
{

	GS2C_RECV_SEALMASTER_LEADER  Leader;
	pop(Leader.m_nLeader);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_SEALMASTER_LEADER, &Leader);

	u2 nCardSlotCount = 0;	// 카드 종류 개수
	pop(nCardSlotCount);

	GS2C_RECV_SEALMASTER_SEALINFO  RecvSealInfo;
	for (n4 i = 0; i < nCardSlotCount; i++)
	{
		RecvCardInfoAll::SealInfo sealInfo;
		pop(sealInfo.m_nCardCode);
		pop(sealInfo.m_nSealID);
		pop(sealInfo.m_nCardCount);
		RecvSealInfo.m_SealInfoList.push_back(sealInfo);
	}

	u2 nCardBookmarkCount = 0;	// 즐겨찾기 정보 개수
	pop(nCardBookmarkCount);
	for (n4 i = 0; i < nCardBookmarkCount; i++)
	{
		RecvCardInfoAll::FavoriteInfo sealFavorite;
		pop(sealFavorite.m_nCardCode);
		pop(sealFavorite.m_nBookmark);
		RecvSealInfo.m_FavoriteInfoList.push_back(sealFavorite);
	}
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_SEALMASTER_SEALINFO, &RecvSealInfo);
}

void cCliGame::RecvCardBookmark(void)
{
	GS2C_RECV_SEALMASTER_FAVORITE sealFavorite;

	pop(sealFavorite.m_nCardCode);
	pop(sealFavorite.m_nBookmark);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_SEALMASTER_FAVORITE, &sealFavorite);
}

void cCliGame::RecvTacticsOpen()
{
	// 	u2 nTargetUID = g_pCharMng->GetTamerUser()->GetUniqID();
	// 	n2 nInvenPos;
	// 
	// 	pop(nTargetUID);
	// 	pop(nInvenPos);
	// 
	// 	g_pDataMng->ServerItemUseSuccess( nTargetUID, nInvenPos );
}

void cCliGame::RecvEncyclopediaOpen()
{
	// 도감 정보( 데이터가 있는 놈들만)
	// 보유 계열체 수
	GS2C_RECV_ENCYCLOPEDIA_OPEN recv;

	n4 nCnt = 0;
	pop(nCnt);	// 데이터가 존재하는 ( 한번이라도 오픈 한 ) 디지몬 계열체 갯수
	for (int i = 0; i < nCnt; i++)
	{
		sEncyclopediaOpendedData sData;
		pop(sData.nDigimonID);	// 1. 계열체 디지몬 번호	// 계열체 번호
		pop(sData.nLevel);			// 2. 계열체 레벨
#ifdef SDM_DIGIMONBOOK_EXTEND_20181126
		pop(sData.nSlotOpened);	// 3. 계열체 내 진화 슬롯 오픈 정보 shift 연산 필요
#else
		for (int i = 1; i < 9; i++)	//8번 pop, 인덱스는 1부터 사용
			pop(sData.nIsOpen[i]);	// 3. 디지몬 오픈 여부( 8번 pop, 진화슬롯 1~8까지 ) 0 : Open, 2: Close 
#endif	
		pop(sData.nEnchant_AT);		// 계열체 - 강화::AT
		pop(sData.nEnchant_BL);		// 계열체 - 강화::BL
		pop(sData.nEnchant_CT);		// 계열체 - 강화::CT
		pop(sData.nEnchant_EV);		// 계열체 - 강화::EV
		pop(sData.nEnchant_HP);		// 계열체 - 강화::HP

		//계열체 정보	
		pop(sData.nSize);			// 8. 계열체 사이즈
		pop(sData.bIsReward);		// 9. 보상여부, 0: 아직 수령 안함, 1: 아이템 획득했음

		recv.m_listOpendedData.push_back(sData);
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::ENCYCLOPEDIA_OPEN_DATA, &recv);

	g_pGameIF->GetDynamicIF(cBaseWindow::WT_ENCYCLOPEDIA);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ENCYCLOPEDIA_RECV_SERVER, NULL);
	//데이터 수신중 메세지
	cMessageBox::DelMsg(10019, false);
}

void cCliGame::RecvEncyclopediaGetItem()
{
	//획득 아이템 생성
	u4 nItemID(5001);		// 획득 아이템
	u2 nItemCnt(2);	// 획득 아이템 갯수

	pop(nItemID);
	pop(nItemCnt);
	g_pDataMng->GetInven()->ItemCrop(nItemID, nItemCnt);

	//계열체 보상 아이템 수령 여부 체크
	int nDigimonID = g_pGameIF->GetEncyclopedia()->_GetSelectDigimonID();		// 계열체 디지몬 번호

	GS2C_RECV_ENCYCLOPEDIA_BISREWARD recv;
	recv.nDigimonID = nDigimonID;
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_BISREWARD, &recv);

	//아이템 획득 표시
	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem(nItemID)->GetInfo();
	// [4/14/2016 hyun] 수정. 아이템 컬러값 추가
	if (pFTItem)
	{
		int nItemCount = nItemCnt;
		if (nItemCount == 1)
			cPrintMsg::PrintMsg(cPrintMsg::GAME_ITEM_CROP, pFTItem->s_szName, &g_pDataMng->GetItemColor(pFTItem));	// 시스템창
		else if (nItemCount > 1)
			cPrintMsg::PrintMsg(cPrintMsg::GAME_ITEM_MANYCROP, pFTItem->s_szName, &nItemCount, &g_pDataMng->GetItemColor(pFTItem));	// 시스템창

		cPrintMsg::PrintMsg(30671, pFTItem->s_szName);	// 화면중앙
	}
	// @@ 여기까지

	//데이터 수신중 메세지
	cMessageBox::DelMsg(10019, false);
}

void cCliGame::RecvEncyclopediaBuff()
{
	//일정 시간동안 적용되는 발동형 버프 정보
	//덱 효과의 인덱스(0,1,2) : 한 덱의 효과는 최대 3개	
	//쿨타임 초기화일 경우 100으로 옴
	u2 nOptIdx = 2;
	u4 nEndTime = 0;//= _TIME_TS + 10;	//발동된 효과 종료 틱시간
	pop(nOptIdx);
	if (nOptIdx == 100)		//쿨타임 초기화
	{
		g_pCharMng->GetDigimonUser(0)->ResetSkillCool();
		return;
	}
	if (nOptIdx >= ENCY_MAX_OPTION)
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T("Over Max OptIdx - OptIdx:%d"), nOptIdx);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("Over Max OptIdx - OptIdx:%d"), nOptIdx);
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	bool bGroupInfoNULL = false;
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::GET_ENCYCLOPEDIA_NULL_CHECK, &bGroupInfoNULL);

	if (bGroupInfoNULL)
	{
		return;
	}

	pop(nEndTime);

	GS2C_RECV_ENCYCLOPEDIA_OPTIDX recv;
	recv.nOptIdx = nOptIdx;
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_GROUPINFOCON, &recv);

	if (nEndTime < _TIME_TS)
		return;
	//버프 셋팅
	g_pCharMng->GetTamerUser()->GetBuff()->_SetEncyBuff(nOptIdx, nEndTime);

}
void cCliGame::RecvEncyclopediaGetBuffStat()
{
	n4 nMaxHP;		// 버프로 인해 변경된 HP
	u2 nAttSpeed;	// 버프로 인해 변경된 공격속도
	pop(nMaxHP);
	pop(nAttSpeed);

	/*
	u2 nMaxDS;		// 버프로 인해 변경된 DS
	u2 nAtt;		// 버프로 인해 변경된 공격력
	u2 nDef;		// 버프로 인해 변경된 방어력
	u2 nCritical;	// 버프로 인해 변경된 크리티컬 확률
	u2 nEv;			// 버프로 인해 변경된 회피
	pop( nMaxDS );
	pop( nAtt );
	pop( nDef );
	pop( nCritical );
	pop( nEv );
	*/

	if (g_pCharMng->GetDigimonUser(0))
	{
		CDigimonUser::sUSER_STAT* pDigimonStat = (CDigimonUser::sUSER_STAT*)g_pCharMng->GetDigimonUser(0)->GetBaseStat();
		pDigimonStat->SetMaxHP(nMaxHP);
		pDigimonStat->SetAttackSpeed(nAttSpeed);
		/*
		pDigimonStat->SetMaxDS( nMaxDS );
		pDigimonStat->SetAtt( nAtt );
		pDigimonStat->SetDef( nDef );
		pDigimonStat->SetCritical( nCritical );
		pDigimonStat->SetEvade( nEv );
		*/

		GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL);
	}
}

#ifdef SELECT_SERVER_ERROR
void cCliGame::RecvSelectServer_Er()
{
	LOG("RecvAccessCode_Fail_LOG_포탈");
	Sleep(2000);

	SendAccessCode();
}
#endif

void cCliGame::RecvCapsuleEvol_Result()
{
	u2 nResult = 0;
	bool bSuccess = false;
	pop(nResult);
	switch (nResult)
	{
	case 1:	// 성공
	{
		cPrintMsg::PrintMsg(30661);	// 캡슐진화 슬롯이 확장되었습니다.
		bSuccess = true;
	}
	break;
	case 2:	// 실패
	{
		cPrintMsg::PrintMsg(30660);	// 캡슐진화 슬롯 확장에 실패하였습니다.
		bSuccess = false;
	}
	break;
	default:
		break;
	}

	//성공여부에 따른 동작
	g_pGameIF->GetCapsuleEvol()->_RecvResult(bSuccess);
}

void cCliGame::RecvSpirit_SpritToDigimon()
{
	u4	nDigimonID;		// 부화된 디지몬 타입
	n8	nMoney;			// 남은 돈
	u1	nUseItemCnt;	// 사용한 아이템 개수
	u4	nItemID;		// 사용한 아이템 종류

	pop(nDigimonID);
	pop(nMoney);
	g_pDataMng->GetInven()->SetMoney(nMoney, false);

	pop(nUseItemCnt);
	while (nUseItemCnt)
	{
		pop(nItemID);
		g_pDataMng->GetInven()->DecreaseItem(nItemID, nUseItemCnt, false, false);

		pop(nUseItemCnt);
	}

	//=============================================================================
	//	인벤 내 아이템 감소 및 부화 애니메이션 활성화
	//=============================================================================
	g_pTacticsAni->Set_bAniProcess(true);	// 부화 애니 시작위치 설정
	g_pTacticsAni->Start(nDigimonID, g_pCharMng->GetTamerUser());	// 부화 애니메이션 활성화

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_ITEM_TO_DIGIMON);
}

void cCliGame::RecvSpirit_DigimonToSpirit()
{
	u1	nDelSlot;		// 삭제 디지몬 슬롯
	n8	nMoney;			// 남은 돈
	u1	nUseItemCnt;	// 사용한 아이템 개수
	u4	nItemID;		// 사용한 아이템 종류

	pop(nDelSlot);
	pop(nMoney);

	//cPrintMsg::PrintMsg( 20100, g_pDataMng->GetTactics()->GetTactics( nDelSlot-1 )->s_szName );
	g_pDataMng->GetTactics()->DeleteTactics(nDelSlot - 1);

	g_pDataMng->GetInven()->SetMoney(nMoney, false);

	// 소모된 아이템 
	pop(nUseItemCnt);
	while (nUseItemCnt)
	{
		pop(nItemID);
		g_pDataMng->GetInven()->DecreaseItem(nItemID, nUseItemCnt, false, false);

		pop(nUseItemCnt);
	}

	// 증가된 아이템
	pop(nUseItemCnt);
	while (nUseItemCnt)
	{
		pop(nItemID);
		g_pDataMng->GetInven()->ItemCrop(nItemID, nUseItemCnt);

		// [4/14/2016 hyun] 수정. 아이템 컬러값 추가
		CsItem::sINFO* pItem = nsCsFileTable::g_pItemMng->GetItem(nItemID)->GetInfo();
		if (pItem)
			cPrintMsg::PrintMsg(cPrintMsg::GAME_ITEM_CROP, pItem->s_szName, &g_pDataMng->GetItemColor(pItem));
		// @@ 여기까지

		pop(nUseItemCnt);
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_DIGIMON_TO_ITEM);
}

void cCliGame::RecvSpirit_AncientSpiritEvolution()
{
	u4	nDigimonID;		// 부화된 디지몬 타입
	u1	nDelSlot_1;		// 삭제 디지몬 슬롯 1
	u1	nDelSlot_2;		// 삭제 디지몬 슬롯 2
	n8	nMoney;			// 남은 돈
	u1	nUseItemCnt;	// 사용한 아이템 개수
	u4	nItemID;		// 사용한 아이템 종류

	pop(nDigimonID);
	pop(nDelSlot_1);
	pop(nDelSlot_2);
	pop(nMoney);

	//cPrintMsg::PrintMsg( 20100, g_pDataMng->GetTactics()->GetTactics( nDelSlot_1 - 1 )->s_szName );
	g_pDataMng->GetTactics()->DeleteTactics(nDelSlot_1 - 1);

	//cPrintMsg::PrintMsg( 20100, g_pDataMng->GetTactics()->GetTactics( nDelSlot_2 - 1 )->s_szName );
	g_pDataMng->GetTactics()->DeleteTactics(nDelSlot_2 - 1);

	g_pDataMng->GetInven()->SetMoney(nMoney, false);

	pop(nUseItemCnt);
	while (nUseItemCnt)
	{
		pop(nItemID);
		g_pDataMng->GetInven()->DecreaseItem(nItemID, nUseItemCnt, false, false);

		pop(nUseItemCnt);
	}

	//=============================================================================
	//	인벤 내 아이템 감소 및 부화 애니메이션 활성화
	//=============================================================================
	g_pTacticsAni->Set_bAniProcess(true);	// 부화 애니 시작위치 설정
	g_pTacticsAni->Start(nDigimonID, g_pCharMng->GetTamerUser());	// 부화 애니메이션 활성화
}

void cCliGame::RecvActorObjectFactor_Init()
{
	OutputDebugStringA("[ACTOR_FACTOR_INIT] begin\n");

	if (g_pDataMng == NULL)
	{
		OutputDebugStringA("[ACTOR_FACTOR_INIT][WARN] g_pDataMng NULL\n");

		u4 nDummyCnt = 0;
		pop(nDummyCnt);

		for (u4 i = 0; i < nDummyCnt && i < 1024; ++i)
		{
			u4 nDummyFactID = 0;
			pop(nDummyFactID);
		}

		OutputDebugStringA("[ACTOR_FACTOR_INIT] end skipped g_pDataMng NULL\n");
		return;
	}

	if (g_pDataMng->GetMapObject() == NULL)
	{
		OutputDebugStringA("[ACTOR_FACTOR_INIT][WARN] MapObject NULL\n");

		u4 nDummyCnt = 0;
		pop(nDummyCnt);

		for (u4 i = 0; i < nDummyCnt && i < 1024; ++i)
		{
			u4 nDummyFactID = 0;
			pop(nDummyFactID);
		}

		OutputDebugStringA("[ACTOR_FACTOR_INIT] end skipped MapObject NULL\n");
		return;
	}

	// 문 다 열려있는것 처럼 초기화
	g_pDataMng->GetMapObject()->Init(true);

	u4 nCnt = 0;
	pop(nCnt);

	{
		char log[256] = { 0 };
		sprintf_s(log, "[ACTOR_FACTOR_INIT] Count=%u\n", nCnt);
		OutputDebugStringA(log);
	}

	/*
		Proteção:
		Se o packet vier desalinhado, nCnt pode vir absurdo.
		Não deixamos isto bloquear/crashar o loading.
	*/
	if (nCnt > 1024)
	{
		OutputDebugStringA("[ACTOR_FACTOR_INIT][WARN] Count > 1024, forcing 0\n");
		nCnt = 0;
	}

	for (u4 i = 0; i < nCnt; ++i)
	{
		u4 nFactID = 0;
		pop(nFactID);

		{
			char log[256] = { 0 };
			sprintf_s(log, "[ACTOR_FACTOR_INIT] FactID=%u\n", nFactID);
			OutputDebugStringA(log);
		}

		if (nFactID == 0)
			continue;

		try
		{
			g_pDataMng->GetMapObject()->OpenAndCloseObject(nFactID, false);
		}
		catch (...)
		{
			OutputDebugStringA("[ACTOR_FACTOR_INIT][WARN] OpenAndCloseObject exception ignored\n");
		}
	}

	OutputDebugStringA("[ACTOR_FACTOR_INIT] end\n");
}


void cCliGame::RecvActorObjectFactor()
{
	// 문과 관련된 몬스터 죽을 시 // 20150914 nFactID = MonsterID -> ObjectID로 변경
	u4 nFactID = 0;
	bool bOpend = false;
	pop(nFactID);
	pop(bOpend);

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	//CProtocol.m_wStr = GetVAString(_T("ActorObject - MonsterID : %d"), nFactID);
	DmCS::StringFn::Format(CProtocol.m_wStr, _T("ActorObject - MonsterID : %d, Opend : %d"), nFactID, bOpend);
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);

	// 몬스터 아이디로 오브젝트 플래그 열림 설정
	g_pDataMng->GetMapObject()->OpenAndCloseObject(nFactID, bOpend);
	//g_pDataMng->GetMapObject()->SetActorFlag( nFactID, true );
	//g_pDataMng->GetMapObject()->CheckActorFlag();
}

void cCliGame::RecvPinokimonInfoReq()
{
	u4 nResetTime;	// 다음 갱신시간
	u4 nAllVote;	// 각 카드의 전체 투표수
	u4 nMyVote;		// 각 카드에 자신이 투표한 수

	pop(nResetTime);

	g_pGameIF->m_sEventInfo.s_nNextResetTime = nResetTime;

	for (int i = 0; i < MAX_CARD_COUNT; i++)
	{
		pop(nAllVote);
		g_pGameIF->m_sEventInfo.s_nAllCnt[i] = nAllVote;
	}

	for (int i = 0; i < MAX_CARD_COUNT; i++)
	{
		pop(nMyVote);
		g_pGameIF->m_sEventInfo.s_nMyCnt[i] = nMyVote;
	}

	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_EVENT_PINOKIMON) == false)
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T( "피노키몬 이벤트 윈도우 생성 실패") );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("피노키몬 이벤트 윈도우 생성 실패"));
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	g_pGameIF->GetEvent_Pinokimon()->RefreshList();
}

void cCliGame::RecvPinokimonVote()
{
	u2 nInvenSlotCnt;	// 인벤 슬롯 카운트
	pop(nInvenSlotCnt);

	u2 nIdx;			// 인벤 슬롯 인덱스
	u4 nItemCnt;		// 아이템 감소 량
	for (int i = 0; i < nInvenSlotCnt; i++)
	{
		pop(nIdx);
		pop(nItemCnt);

		// 인벤에서 아이템 삭제
		g_pDataMng->GetInven()->GetData(nIdx)->DecreaseCount(nItemCnt);
	}

	// 서버응답 대기 해제
	cMessageBox::DelMsg(10019, false);
}

void cCliGame::RecvRemoveVoteItem()
{
	// 투표 창 띄워놓은 상태면 닫아주자
	if (g_pSeparate)
	{
		if (g_pSeparate->GetTargetWindowType() == cBaseWindow::WT_EVENT_PINOKIMON)
		{
			g_pSeparate->OnCancel();
		}
	}

	u4 nNpcID;
	pop(nNpcID);

	// 인벤 & 창고 & 지급창고 아이템 삭제
	u2 nCnt;		// 슬롯 카운트
	u2 nIdx;		// 슬롯 인덱스
	u4 nDecrease;	// 지워야 할 아이템 갯수

	// 1. 인벤
	pop(nCnt);
	for (int i = 0; i < nCnt; i++)
	{
		pop(nIdx);
		pop(nDecrease);
		if (g_pDataMng->GetInven() && nIdx < nLimit::Inven)
		{
			g_pDataMng->GetInven()->GetData(nIdx)->DecreaseCount(nDecrease);
		}
	}

	// 2. 창고
	pop(nCnt);

	list<WareHouseContents::sWAREHOUSE> listNormalWareHouse;

	WareHouseContents::sWAREHOUSE stTemp;

	for (int i = 0; i < nCnt; i++)
	{
		pop(nIdx);
		pop(nDecrease);
		if (nIdx < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse)
		{
			stTemp.Reset();

			stTemp.nIdx = nIdx;
			stTemp.nDecreaseCount = nDecrease;

			listNormalWareHouse.push_back(stTemp);

			//			g_pDataMng->GetWarehouse()->GetData( nIdx )->DecreaseCount( nDecrease );
		}
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_NORMAL_DATA_DECREASE, &listNormalWareHouse);

#ifdef NEW_SHARESTASH	// 계정 공유 창고는 국내에만 적용된 컨텐츠임.
	// 3. 공유창고
	list<WareHouseContents::sWAREHOUSE> listSharelWareHouse;

	pop(nCnt);
	for (int i = 0; i < nCnt; i++)
	{
		pop(nIdx);
		pop(nDecrease);
		if (nIdx < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxShareStash)
		{
			stTemp.Reset();

			stTemp.nIdx = nIdx;
			stTemp.nDecreaseCount = nDecrease;

			listSharelWareHouse.push_back(stTemp);
			/*
			int iData[2] = {0, 0};
			iData[0] = nIdx;
			iData[1] = nDecrease;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_NORMAL_DATA_DECREASE, iData);*/

			//			g_pDataMng->GetWarehouse()->GetShareData( nIdx )->DecreaseCount( nDecrease );
		}
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_SHARE_DATA_DECREASE, &listSharelWareHouse);

#endif

	// 4. 지급창고 ( 클라처리 )
	CsNpc* pNpc = nsCsFileTable::g_pNpcMng->GetNpc(nNpcID);
	if (pNpc == NULL)
		return;

	sNPC_TYPE_SPECIAL_EVENT* pNpcType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
	for (int i = 0; i < pNpcType->GetItemCount(); i++)
	{
		if (pNpcType->GetItem(i) == NULL)
			continue;

		// 투표 아이템
		int nVoteItemID = pNpcType->GetItem(i)->GetInfo()->s_dwItemID;

		// 아이템 삭제 알림 메세지
		cPrintMsg::PrintMsg(20029, nsCsFileTable::g_pItemMng->GetItem(nVoteItemID)->GetInfo()->s_szName);
		cMessageBox::GetFirstMessageBox()->SetValue1(1);					// 타입 : 아이템 제거
		cMessageBox::GetFirstMessageBox()->SetValue2(nItem::TabGiftShop);	// 삭제 위치
		cMessageBox::GetFirstMessageBox()->SetValue3(nVoteItemID);		// 삭제 아이템
	}

	// 인벤 락 해제
	for (int j = 0; j < pNpcType->GetItemCount(); j++)
	{
		int nItemID = pNpcType->GetItemID(j);
		g_pDataMng->GetInven()->ItemUnlock_ItemID(nItemID);
	}
}

void cCliGame::RecvDigimonDataChange()
{
	n4 nDataChangeType = nDigimon::eDataChangeType_None;
	n4 nResult = 0;
	u1 nLDigimonArrIDX = 0;
	u1 nRDigimonArrIDX = 0;

	pop(nDataChangeType);
	pop(nLDigimonArrIDX);
	pop(nRDigimonArrIDX);
	pop(nResult);
	if (nResult == MESSAGE_COMPLETE)		// 교환 성공
	{
		switch (nDataChangeType)
		{
		case nDigimon::eDataChangeType_Size:
		{
			u1 nLHatchLevel = 0;
			u2 nLNewScale = 0;

			pop(nLHatchLevel);
			pop(nLNewScale);

			u1 nRHatchLevel = 0;
			u2 nRNewScale = 0;

			pop(nRHatchLevel);
			pop(nRNewScale);

			// 부화 단수 레벨 값도 얻어 오지만 클라에서는 미사용.
			ContentsStream kStream;
			kStream << nLDigimonArrIDX << nRDigimonArrIDX << nLNewScale << nRNewScale << nLHatchLevel << nRHatchLevel;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_TRADE_DATA_SCALE, &kStream);
		}
		break;
		case nDigimon::eDataChangeType_Inchant:
		{
			u2 nLEnchantLevel = 0;
			u2 nLExtendAttribute[MaxExtendStat] = { 0, };
			u2 nLExtendAttributeLV[MaxExtendStat] = { 0, };

			pop(nLEnchantLevel);
			pop(nLExtendAttribute, sizeof(nLExtendAttribute));
			pop(nLExtendAttributeLV, sizeof(nLExtendAttributeLV));

			u2 nREnchantLevel = 0;
			u2 nRExtendAttribute[MaxExtendStat] = { 0, };
			u2 nRExtendAttributeLV[MaxExtendStat] = { 0, };

			pop(nREnchantLevel);
			pop(nRExtendAttribute, sizeof(nRExtendAttribute));
			pop(nRExtendAttributeLV, sizeof(nRExtendAttributeLV));

			ContentsStream kStream;
			kStream << nLDigimonArrIDX << nRDigimonArrIDX << nLEnchantLevel << nLExtendAttribute << nLExtendAttributeLV
				<< nREnchantLevel << nRExtendAttribute << nRExtendAttributeLV;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_TRADE_DATA_UPGRADE, &kStream);
		}
		break;
		case nDigimon::eDataChangeType_EvoSlot:
		{
			u1 nLMaxEvoUnit = 0;
			cEvoUnit kLEvoUnit[nLimit::EvoUnit];

			pop(nLMaxEvoUnit);
			pop(&kLEvoUnit[1], sizeof(cEvoUnit) * nLMaxEvoUnit);

			u1 nRMaxEvoUnit = 0;
			cEvoUnit kREvoUnit[nLimit::EvoUnit];

			pop(nRMaxEvoUnit);
			pop(&kREvoUnit[1], sizeof(cEvoUnit) * nRMaxEvoUnit);

			ContentsStream kStream;
			kStream << nLDigimonArrIDX << nRDigimonArrIDX << nLMaxEvoUnit << kLEvoUnit << nRMaxEvoUnit << kREvoUnit;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_TRADE_DATA_EVOLUTION, &kStream);
		}
		break;
		}
	}
	else
	{
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_TRADE_DATA_FAILURE, &nResult);
	}
}

void cCliGame::RecvErrorMessage()
{
	n4 nResult = -1;

	pop(nResult);

	switch (nResult)
	{
	case nErrorMessage::eErrorMessage_Make_CountOver:		// 제작 요청 개수 오류
		break;
	case nErrorMessage::eErrorMessage_Make_ItemCountOver:	// 제작 아이템 요청 개수 오류
		break;
	case nErrorMessage::eErrorMessage_Make_NoneResource:	// 제작 테이블 리소스 없음
		break;
	case nErrorMessage::eErrorMessage_Item_NoneResource:	// 아이템 테이블 리소스 없음
		break;
	case nErrorMessage::eErrorMessage_Make_NotEnoughInvenSlot:	// 제작 인벤 공간 부족
		cPrintMsg::PrintMsg(11015);
		break;
	case nErrorMessage::eErrorMessage_Make_NotEnoughMoney:		// 제작 금액 부족
		break;
	case nErrorMessage::eErrorMessage_Make_NotEnoughMaterialItem:	// 제작 재료 부족
		break;
	case nErrorMessage::eErrorMessage_Repurchase_NotEnoughInvenSlot:	// 재구매 인벤 부족
		break;
	}
}


void cCliGame::AttackProcesses(u4 nHitterUID, u4 nTargetUID, n4 nDamage, u4 nBattleOption, n4 hp, n4 Mhp, bool bDie)
{
	CsC_AvObject* pTarget = g_pMngCollector->GetObject(nTargetUID);
	if (pTarget == NULL)
		return;

	sRECV_HITTER_INFO hitter;
	hitter.s_nUID = nHitterUID;
	hitter.s_HitType = nBattleOption & pBattle::BO_CRITICAL ? CsC_AttackProp::HT_Critical : CsC_AttackProp::HT_Normal;

	// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
	//타겟에게 데미지반사 버프가 있는지 확인
	std::list< cBuffData::sBUFF_DATA >* pBuffList = NULL;
	if (pTarget->GetLeafRTTI() == RTTI_DIGIMON)
		pBuffList = ((CDigimon*)pTarget)->GetBuff()->GetBuff();
	else if (pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER)
		pBuffList = g_pCharMng->GetDigimonUser(0)->GetBuff()->GetBuff();

	if (pBuffList != NULL)
	{
		std::list< cBuffData::sBUFF_DATA >::iterator it = pBuffList->begin();
		std::list< cBuffData::sBUFF_DATA >::iterator itEnd = pBuffList->end();
		for (;it != itEnd;it++)
		{
			u4 nSkillCode = (*it).s_pBuffTable->GetInfo()->s_dwSkillCode;

			if (nSkillCode == 0)
				continue;

			//스킬코드로 스킬 종류 확인
			CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill(nSkillCode);
			if (pSkill == NULL)
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
				//CProtocol.m_wStr = GetVAString( _T("스킬 못찾음( SkillCode - %d )"), nSkillCode );
				DmCS::StringFn::Format(CProtocol.m_wStr, _T("스킬 못찾음( SkillCode - %d )"), nSkillCode);
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
				continue;
			}
			for (int i = 0; i < SKILL_APPLY_MAX_COUNT; i++)
			{
				//스킬 효과를 보고 반사 스킬인지 확인
				USHORT nApplyType = pSkill->GetInfo()->s_Apply[i].s_nA;

				switch (nApplyType)
				{
				case APPLY_DR:	hitter.s_nReflectType = (*it).s_pBuffTable->GetInfo()->s_nBuffClass;	break;	// 데미지 반사 버프
				case APPLY_AB:	// 데비지 흡수 버프
					if (nDamage == 0)
					{
						hitter.s_nSkillIndex = (*it).s_pBuffTable->GetInfo()->s_dwID;//pSkill->GetInfo()->s_Apply[ i ].s_nBuffCode;
						hitter.s_HitType = CsC_AttackProp::HT_Absorb;
					}
					break;
				}
			}

		}
	}

	sRECV_TARGET_INFO target;
	target.s_nUID = nTargetUID;

	target.s_DamageType = nBattleOption & pBattle::BO_BLOCK ? CsC_AttackProp::DT_Block : CsC_AttackProp::DT_Normal;
	target.s_nNumEffectType = nBattleOption & pBattle::BO_CRITICAL ? NUMTYPE::ET_CRITICAL : NUMTYPE::ET_NORMAL;
	target.s_nValue[0] = -nDamage;

	if (bDie == false)
	{
		switch (pTarget->GetLeafRTTI())
		{
		case RTTI_TAMER:
		case RTTI_TAMER_USER:
			assert_cs(false);
			break;
		case RTTI_TUTORIAL_MONSTER:
		case RTTI_DIGIMON:
		case RTTI_MONSTER:
		{
			__int64 changeHp = (__int64)hp * 255;
			int nHpRate = changeHp / Mhp;
			if (nHpRate < 1)
			{
				nHpRate = 1;
			}
			target.s_nValue[1] = nHpRate - pTarget->GetBaseStat()->GetBackupHP();
			pTarget->GetBaseStat()->SetBackupHP(nHpRate);
		}
		break;
		case RTTI_DIGIMON_USER:
		{
			pTarget->GetBaseStat()->SetHP(hp);
			pTarget->GetBaseStat()->SetBackupHP(hp);
		}
		break;
		default:
			assert_cs(false);
		}

		SetAttackInfo(&hitter, &target, false);
	}
	else
	{
		target.s_nValue[1] = -pTarget->GetBaseStat()->GetHP();

		pTarget->SetAniPause(false);
		SetAttackInfo(&hitter, &target, true);

		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T("RecvAttack SetDieInfo UID : %d"), pTarget->GetUniqID() );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("RecvAttack SetDieInfo UID : %d"), pTarget->GetUniqID());
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		SetDieInfo(pTarget);
	}

#ifdef DAMAGE_METER
	sDamageMeterInfo damageInfo(nHitterUID, nTargetUID, nDamage);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_DAMAGEMETER_DATA, &damageInfo);
#endif
}



//////////////////////////////////////////////////////////////////////////
// 매트로 프로텍트 관련 함수
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 내가 다른 테이머에게 매크로 프로텍트 아이템 사용 요청 패킷
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendMacroQuestion(std::wstring const& szTamerName)
{
	if (szTamerName.empty())
		return;

	C2GS_SEND_MACRO_QUESTION packet;
	packet.m_TargetName = szTamerName;
	newp(packet.GetPacketID());
	push(packet.m_TargetName);
	endp(packet.GetPacketID());
	send();
}

// 매크로 프로텍트 결과값 서버에 전송
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
void cCliGame::SendMacroAnswer(std::wstring const& wsAnswer)
{
	C2GS_SEND_MACRO_ANSWER packet;
	DmCS::StringFn::ToMB(wsAnswer, packet.m_szAnswer);
	newp(packet.GetPacketID());
	push(packet.m_szAnswer);
	endp(packet.GetPacketID());
	send();
}
#else
void cCliGame::SendMacroAnswer(n4 const& nRandNo, n4 const& nAnswer)
{
	C2GS_SEND_MACRO_ANSWER packet;
	packet.m_nRandNo = nRandNo;
	packet.m_nAnswer = nAnswer;

	newp(packet.GetPacketID());
	push(packet.m_nRandNo);
	push(packet.m_nAnswer);
	endp(packet.GetPacketID());
	send();
}
#endif
// 대상이 매크로인지 아닌지 결과값 받음.
void cCliGame::RecvMacroResult(void)
{
	GS2C_RECV_MACRO_PLAYER_RESULT recv;
	pop(recv.m_nResult);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MACRO_PLAYER_RESULT, &recv);
}

// 나한테 매크로 프로텍트 질문이 왔다.
void cCliGame::RecvMacroQuestion(void)
{
	GS2C_NTF_MACRO_QUESTION_REQUEST recv;
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	pop(recv.m_nTimeMax);
	pop(recv.m_nTimeLeft);
	pop(recv.m_nChanceLeft);
	pop(recv.m_nSize);
	if (recv.m_nSize > 0)
	{
		recv.m_pData = new char[recv.m_nSize];
		pop(recv.m_pData, recv.m_nSize);
	}

#ifdef SDM_MACRO_PROTECT_QUESTION_20190524
	pop(recv.m_nQuestionType);
	pop(recv.m_nQuestion);
#endif
#else
	pop(recv.m_nRandNum);
	pop(recv.m_nHint);
#endif

	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_MACROPROTECT_QUESTION_REQUEST, &recv);
}

void cCliGame::RecvMacroSuccess(void)
{
	C2GS_RECV_MACRO_QUESTION_RESULT recv;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MACROPROTECT_QUESTION_RESULT, &recv);
}

// 내가 매크로 프로텍트 요청한 것에 대한 결과값
void cCliGame::RecvMacroFailure(void)
{
	C2GS_RECV_MACRO_QUESTION_RESULT recv;
	pop(recv.m_nResult);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_MACROPROTECT_QUESTION_RESULT, &recv);
}

//////////////////////////////////////////////////////////////////////////
//	디지몬 아레나 관련 함수
//////////////////////////////////////////////////////////////////////////
// 아레나 랭킹 리스트 패킷
void cCliGame::Recv_ArenaRequestRank(void)
{
	GS2C_RECV_ARENA_REQUEST_RANK recv;
	pop(recv.nType);				//요청한 랭킹타입(eRankType)
	pop(recv.nResult);				//결과
	pop(recv.nRankStatus);			//랭킹 상황(eRankStatus)	
	pop(recv.nPoint);				//자신의 포인트 - 넣은정보가없으면 0
	pop(recv.nRank);				//자신의 절대 등수
	pop(recv.nModifier);			//순위 변동수치
	pop(recv.nRankType);			//자신의 랭크위치 분류(0 절대값 , 1상대값(%))
	pop(recv.nRankPercent);			//자신의 랭크위치
	pop(recv.nRankRenewSec);		//다음 랭킹 갱신 시간
	pop(recv.nRankEndSec);			//현 랭킹 마감 시간

	u1 nRankSize = 0;
	pop(nRankSize);			//Top100 랭킹 갯수(최대 100)
	for (int n = 0; n < nRankSize; ++n)
	{
		nArenaRank::st_TopRankInfo addRankData;
		pop(addRankData.nRank);//등수
		pop(addRankData.szName);//이름
		pop(addRankData.szGuild);//길드
		pop(addRankData.nPoint);//포인트
		pop(addRankData.nIsNew);//신규진입인가
		pop(addRankData.nModifier);//등수 변동 수치
		recv.lRankList.push_back(addRankData);
	}

	BHPRT("Recv - GS2C_RECV_ARENA_REQUEST_RANK : Size(%d)", nRankSize);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_RANKER_LIST, &recv);
}
// 아레나 이전 랭킹 리스트 패킷
void cCliGame::Recv_ArenaRequestOldRank(void)
{
	GS2C_RECV_ARENA_REQUEST_OLD_RANK recv;
	pop(recv.nType);				//요청한 랭킹타입(eRankType)
	pop(recv.nResult);				//결과
	pop(recv.nRank);				//자신의 절대 등수 , 자신이 참여안해서 정보가없으면 0

	u1 nRankSize = 0;
	pop(nRankSize);			//Top100 랭킹 갯수(최대 100)
	for (int n = 0; n < nRankSize; ++n)
	{
		nArenaRank::st_TopRankOldInfo addRankData;
		pop(addRankData.nRank);//등수
		pop(addRankData.szName);//이름
		pop(addRankData.szGuild);//길드
		pop(addRankData.nPoint);//포인트
		recv.lRankList.push_back(addRankData);
	}
	BHPRT("Recv - GS2C_RECV_ARENA_REQUEST_OLD_RANK : Size(%d)", nRankSize);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_PRE_RANKER_LIST, &recv);
}

// 아레나 명예의 전당 리스트 패킷
void cCliGame::Recv_ArenaRequestFame(void)
{
	GS2C_RECV_ARENA_REQUEST_FAME recv;
	pop(recv.nSeason);				//요청한 시즌 번호 , 0을넣었을경우 최신시즌
	pop(recv.nResult);				//결과

	u1 nRankSize = 0;
	pop(nRankSize);			//Top50 랭킹 갯수(최대 50)
	for (int n = 0; n < nRankSize; ++n)
	{
		nArenaRank::st_TopRankFameInfo addRankData;
		pop(addRankData.nRank);//등수
		pop(addRankData.szName);//이름
		pop(addRankData.szGuild);//길드
		pop(addRankData.nPoint);//포인트
		recv.lRankList.push_back(addRankData);
	}
	BHPRT("Recv - GS2C_RECV_ARENA_REQUEST_FAME : Size(%d)", nRankSize);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_FAME_LIST, &recv);
}


void cCliGame::Recv_ArenaRequestFameSeasonList()
{
	u2 nSeasonSize = 0;
	pop(nSeasonSize);//

	GS2C_RECV_ARENA_REQUEST_FAME_LIST recv;
	for (int n = 0; n < nSeasonSize; ++n)
	{
		u2 nSeasonIdx = 0;
		pop(nSeasonIdx);
		recv.lSeasonList.push_back(nSeasonIdx);
	}
	BHPRT("Recv - GS2C_RECV_ARENA_REQUEST_FAME_LIST : Size(%d)", nSeasonSize);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_INFINITEWAR_SEASON_FAME_LIST, &recv);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cCliGame::Recv_XGuageUpdate()
{
	GS2C_RECV_XGuage_Update recv;
	pop(recv.nXGuage);
	pop(recv.nCrystal);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::Recv_Ntf_XGuage_Update, &recv);
}

void cCliGame::Recv_XGuageMaxUpdate()
{
	GS2C_RECV_XGuage_Max_Update recv;
	pop(recv.nXGuageMax);
	pop(recv.nCrystalMax);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::Recv_Ntf_XGuage_Max_Update, &recv);

}

void cCliGame::Recv_XGToCrystal()
{
	GS2C_RECV_XGuage_to_Crystal recv;
	pop(recv.nResult);// 성공 실패만 받고 실제 게이지 변경은 Recv_XGuageUpdated에서 함.
	GAME_EVENT_ST.OnEvent(EVENT_CODE::Recv_XGToCrystal_Result, &recv);
}

void cCliGame::Recv_CrystalToXG()
{
	GS2C_RECV_Crystal_to_XGuage recv;
	pop(recv.nResult);// 성공 실패만 받고 실제 게이지 변경은 Recv_XGuageUpdated에서 함.
	GAME_EVENT_ST.OnEvent(EVENT_CODE::Recv_CrystalToXG_Result, &recv);

}
void cCliGame::Send_XGToCrystal()
{
	C2GS_SEND_XGuage_to_Crystal packet;

	newp(packet.GetProtocol());
	endp(packet.GetProtocol());
	send();
}

void cCliGame::Send_CrystalToXG()
{
	C2GS_SEND_Crystal_to_XGuage packet;

	newp(packet.GetProtocol());
	endp(packet.GetProtocol());
	send();

}

//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvVipMemberShipData()
{
#ifdef SDM_VIP_SYSTEM_20181105
	GS2C_RECV_VIP_SYSTEM_DATA recv;
	pop(recv.bIsVip);
	pop(recv.unVipEndTime);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_VIP_SYSTEM_DATA, &recv);
#endif
}

//////////////////////////////////////////////////////////////////////////
void cCliGame::NTF_NeonDisplay()
{
	GS2C_NOTIFY_Neon_Display recv;
	pop(recv.nNeonType);
	pop(recv.strTamerName);
	pop(recv.nType1);
	pop(recv.nType2);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_NEON_DISPLAY, &recv);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cCliGame::Recv_NProtectCheck()
{
#ifdef DEF_CORE_NPROTECT
	BYTE bPacket[4096] = { 0, };
	DWORD dwPacketSize = 0;
	UINT unwUniqueIdx = 0;

	pop(unwUniqueIdx);
	pop(bPacket);
	pop(dwPacketSize);

	CSAuth3(unwUniqueIdx, (PBYTE)bPacket, dwPacketSize, 0);
#endif
}

void cCliGame::Recv_XignCodeCheck(void)
{
#ifdef SDM_DEF_XIGNCODE3_20181107
	GS2C_RECV_XignCodePacket recv;
	pop(recv.szPacketData, XIGNCODE_PACKET_SIZE);
	pop(recv.nSize);
	XignCode_ProbeEx(recv.szPacketData, recv.nSize, NULL);
#endif
}

void cCliGame::RecvItemSort()
{
	GS2C_RECV_ITEMSORT recv;
	pop(recv.m_u1SortFlag);
	pop(recv.m_nResult);
	pop(recv.m_u2ItemCount);

	// count가 0이면 정렬할 아이템이 없어서 정렬 실패
	for (int i = 0; i < recv.m_u2ItemCount; ++i)
	{
		cItemData itemData;
		pop(itemData);
		recv.m_lItems.push_back(itemData);
	}

	if (nItemSort::eINVEN == recv.m_u1SortFlag)
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_INVEN_SORT_END, &recv);
	else if (nItemSort::eWAREHOUSE == recv.m_u1SortFlag || nItemSort::eShareStash == recv.m_u1SortFlag)
		GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_RECV_ITEMSORT_END, &recv);
}

void cCliGame::RecvInvenResult()
{
	GS2C_RECV_INVENTORY recv;
	pop(recv.m_nResult);
	pop(recv.m_n8Money);
	pop(recv.m_u1InventoryFlag);
	pop(recv.m_u2InventorySlot);

	for (int i = 0; i < recv.m_u2InventorySlot; ++i)
	{
		cItemData itemData;
		pop(itemData);
		recv.m_lItems.push_back(itemData);
	}

	if (nInventoryInfo::eINVEN == recv.m_u1InventoryFlag)
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_INVEN_REQ_RESULT, &recv);
	else if (nInventoryInfo::eWAREHOUSE == recv.m_u1InventoryFlag || nInventoryInfo::eSHARESTASH == recv.m_u1InventoryFlag)
		GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_WAREHOUSE_REQ_RESULT, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 초월 관련 코드 
void cCliGame::RecvDigimonTranscendenceChargeExp()
{
	GS2C_RECV_DigimonTranscendenceChargeEXP recv;
	pop(recv.m_nResult);

	if (nsDigimonTranscendenceResult::Success == recv.m_nResult)
	{
		pop(recv.m_u1Flag);	// 선택 (일반 충전, 고급 충전)
		pop(recv.m_u1DigimonTranscendencePos);	// 경험치 획득한 디지몬 슬롯 번호

		u2   u2DigimonCount;
		pop(u2DigimonCount);
		for (int n = 0; n < u2DigimonCount; ++n)
		{
			u2 delDigimonArchiveSlotIdx = 0;
			pop(delDigimonArchiveSlotIdx);
			recv.m_listDigimonPos.push_back(delDigimonArchiveSlotIdx);
		}

		u2   u2DelItemCount;
		pop(u2DelItemCount);
		for (int n = 0; n < u2DelItemCount; ++n)
		{
			nsItemDel::sItemInfo delItem;
			pop(delItem);
			recv.m_lItemList.push_back(delItem);
		}

		pop(recv.m_u2SuccessRate);// 성공 배율
		pop(recv.m_u8ChargeEXP);// 획득 경험치
		pop(recv.m_u8TotalEXP);// 총 경험치
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_DIGIMON_TRANSCENDENCE_CHARGE_EXP, &recv);
}

void cCliGame::RecvDigimonTranscendence()
{
	GS2C_RECV_DigimonTranscendence recv;
	pop(recv.m_nResult);

	if (nsDigimonTranscendenceResult::Success == recv.m_nResult)
	{
		pop(recv.m_u1DigimonTranscendencePos);
		pop(recv.m_u1HatchLevel);

		pop(recv.m_n8DigimonTranscendenceMoney); // 초월 요청한 비용
		pop(recv.m_n8Money);					    // 현재 소유 돈
		pop(recv.m_u8Exp);						// 현재 경험치
	}
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_DIGIMON_TRANSCENDENCE_HATCH_UP, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 디지몬 이팩트 변경
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvChangeDigimonEffect()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_DIGIMON_EFFECT_CHANGE recv;
	pop(recv.nResult);	//성공아닐시 아래는 모두 0
	pop(recv.nItemPos);	//아이템 위치
	pop(recv.nItemType);	//아이템 타입
	pop(recv.nRemainItemCount);			//사용후 아이템 갯수 , 다쓰면 0
	pop(recv.nEffectedDigimonEvoSlot);	//아이템이 적용된 진화 단계
	pop(recv.nEffectType);				//적용된 이펙트 아이템 IDX , 이펙트제거시 0일수있음

	if (nsDIGIMON_EFFECT_CHANGE_RES::SUCCESS != recv.nResult)
	{
		cPrintMsg::PrintMsg(recv.nResult);
		return;
	}

	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET(pDUser);
	SAFE_POINTER_RET(g_pDataMng);

	// 아이템 사용 적용
	g_pDataMng->ServerItemUseSuccess(pDUser->GetUniqID(), recv.nItemPos);

	CDigimonEvolveObj* pCurEvolObj = pDUser->GetFTEvolCurObj();
	SAFE_POINTER_RET(pCurEvolObj);

	if (pCurEvolObj->m_nEvoSlot == recv.nEffectedDigimonEvoSlot)
	{
		if (0 != recv.nEffectType)
		{
			nsDPART::sDPART_INFO partInfo;
			partInfo.s_nFileTableID = recv.nEffectType;
			if (nsCsFileTable::g_pItemMng->GetDPartsInfo(recv.nEffectType, pDUser->GetFTID(), partInfo.s_nPartIndex, partInfo.s_File))
				pDUser->ChangeParts(partInfo);
		}
		else
			pDUser->DetachParts(nsDPART::Skin);

		GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL);
	}
#endif
}

// 다른 유저가 아이템 사용 시 날아옴
void cCliGame::NTF_DigimonEffectChange()
{
#if COMMON_LIB_FIXED
	GS2C_NTF_DIGIMON_EFFECT_SYNC recv;
	pop(recv.nUID);			//해당 디지몬 UID
	pop(recv.nEffectType);	//새로 적용된 이펙트 아이템 , 이펙트제거시 0일수있음

	SAFE_POINTER_RET(g_pCharMng);
	CDigimon* pDigimon = g_pCharMng->GetDigimonFromUIDX(recv.nUID);
	SAFE_POINTER_RET(pDigimon);

	if (0 == recv.nEffectType)
	{
		pDigimon->DetachParts(nsDPART::Skin);
		return;
	}

	// 디지몬 파츠 추가
	nsDPART::sDPART_INFO partsInfo;
	partsInfo.s_nFileTableID = recv.nEffectType;
	if (nsCsFileTable::g_pItemMng->GetDPartsInfo(recv.nEffectType, pDigimon->GetFTID(), partsInfo.s_nPartIndex, partsInfo.s_File))
		pDigimon->ChangeParts(partsInfo);

	// 내 디지몬인 경우
	if (g_pCharMng->IsDigimonUserFromUIDX(recv.nUID))
		GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL);
#endif
}
