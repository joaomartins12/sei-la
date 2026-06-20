#include "stdafx.h"
#include "cCliGame.h"

#include "nlib/map.h"
// #ifndef _GIVE
// #include "nlib/list.h"
// #endif
#include "common_vs2019/pSync.h"

#ifdef GM_CLOCKING
#define CLOCKING_ITEM_ID		30				//투명 아이템 아이템번호(장비아이템) chu8820
#endif

// #ifndef _GIVE
// 	cList<int> *g_lSync = null;
// #endif

void StartNetSync(void)
{
	// #ifndef _GIVE
	// 	g_lSync = xnew cList<int>();
	// #endif
}

void EndNetSync(void)
{
	// #ifndef _GIVE
	// 	xdelete(g_lSync);
	// #endif
}


void cCliGame::RecvSyncCondition(void)	// object 의 상태
{
	SAFE_POINTER_RET(g_pMngCollector);

	u4 nUID = 0;
	pop(nUID);
	u4 nCondition = 0;
	pop(nCondition);

	CsC_AvObject* pObj = g_pMngCollector->GetObject(nUID);
	SAFE_POINTER_RET(pObj);

	switch (pObj->GetLeafRTTI())
	{
	case RTTI_TAMER_USER:
	case RTTI_TAMER:
		((CTamer*)pObj)->SetCondition(nCondition);
		break;
	case RTTI_DIGIMON_USER:
	case RTTI_DIGIMON:
		((CDigimon*)pObj)->SetCondition(nCondition);
		break;
	case RTTI_MONSTER:
		((CMonster*)pObj)->SetCondition(nCondition);
		break;
		// 	default:
		// 		assert_cs( false );
	}

	// 컨디션에서 개인상점의 값이 변경 되었다	
	if (nCondition & nSync::Shop)
	{
		std::wstring szShopTitle;
		pop(szShopTitle);

		if (szShopTitle.empty())
			szShopTitle = L"???";

		ContentsStream kTmp;
		uint checkid = pObj->GetUniqID();
		int nType = cTalkBalloon::MAX_TYPE;
		kTmp << checkid << szShopTitle << nType;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_BALLOON_TITLE, &kTmp);
	}
}


void cCliGame::RecvChangePartnerScale(void)		// 용병의 크기 변경
{
	u4 nUID = 0;
	pop(nUID);
	u2 nScale = 0;
	pop(nScale);
	u4 nEndTick = 0;
	pop(nEndTick);

	SAFE_POINTER_RET(g_pMngCollector);
	CsC_AvObject* pObject = g_pMngCollector->GetObject(nUID);
	SAFE_POINTER_RET(pObject);

	float fScale = nScale * 0.0001f;

	pObject->PlaySound(SOUND_LEVEL_UP);

	switch (pObject->GetLeafRTTI())
	{
	case RTTI_DIGIMON_USER:
	{	// 내자신의 디지몬이라면
		int nBefore = CsFloat2Int(pObject->GetGameScale() * 100.0f);
		//int nBefore = CsFloat2Int(pObject->GetOrgGameScale()*100.0f);
		int nAfter = CsFloat2Int(fScale * 100.0f);
		cPrintMsg::PrintMsg(30163, &nBefore, &nAfter);
	}
	case RTTI_DIGIMON:
	{
		((CDigimon*)pObject)->CheckLevelEffect();
		((CDigimon*)pObject)->CheckEnchantEffect();

#ifdef  SDM_TAMER_XGUAGE_20180628
		((CDigimon*)pObject)->CheckingXAntiEffect();
#endif
		DWORD dwPlag = nsEFFECT::POS_CHARPOS;
		pObject->GetProp_Effect()->AddEffect(EFFECT_DIGIMON_LEVELUP, pObject->GetToolWidth() * 0.02f, dwPlag);

		//=================================================================================================================
		//	크기 변하는 열매
		//=================================================================================================================
		if (nEndTick != 0)
		{
			pObject->SetScale(fScale, false);
			((CDigimon*)pObject)->GetBuff()->ReleaseBuff(nsBuff::BK_CANDY_SIZE_CHANGE);

			// 스케일 크기가 틀리다면 버프셋팅
			//if( ( fScale != pObject->GetOrgGameScale() )&& nEndTick > 0 )
			{
				uint nTime = _TIME_TS + nEndTick / 1000;
				((CDigimon*)pObject)->GetBuff()->SetBuff(nsBuff::BK_CANDY_SIZE_CHANGE, 0, nTime, 0);
			}
		}
		else	// CheckSizeEffect , UI->ResetTall 은 SetSCale 이후에 호출 되어야 갱신된 값을 얻어올 수 있습니다.
		{

			pObject->SetScale(fScale, true);
			((CDigimon*)pObject)->CheckSizeEffect();
			((CDigimon*)pObject)->GetBuff()->ReleaseBuff(nsBuff::BK_CANDY_SIZE_CHANGE);
			((CDigimon*)pObject)->CheckSizeEffect();
		}

		GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS, NULL);

		if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_TACTICSHOUSE) == true)
		{
			int NoUseParam = 0;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ON_CHANGE_PARTNER_DIGIMON, &NoUseParam);
		}
		if (pObject->GetLeafRTTI() == RTTI_DIGIMON_USER && nEndTick == 0)//임시 아이템이 아니면서 본인 디지몬일 경우
		{
			GS2C_RECV_ENCYCLOPEDIA_SCALE recv;
			recv.nDigimonID = ((CDigimon*)pObject)->GetBaseDigimonFTID();
			recv.nScale = nScale;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::ENCYCLOPEDIA_SCALE, &recv);
		}
	}
	break;
	case RTTI_TAMER_USER:
	{	// 내자신의 디지몬이라면
		int nBefore = CsFloat2Int(pObject->GetGameScale() * 100.0f);
		//int nBefore = CsFloat2Int(pObject->GetOrgGameScale()*100.0f);
		int nAfter = CsFloat2Int(fScale * 100.0f);
		cPrintMsg::PrintMsg(30167, &nBefore, &nAfter);
	}
	case RTTI_TAMER:
	{
		DWORD dwPlag = nsEFFECT::POS_CHARPOS;
		pObject->GetProp_Effect()->AddEffect(EFFECT_TAMER_LEVELUP, pObject->GetToolWidth() * 0.02f, dwPlag);

		//=================================================================================================================
		//	크기 변하는 열매
		//=================================================================================================================
		if (nEndTick != 0)
		{
			pObject->SetScale(fScale, false);
			((CTamer*)pObject)->GetBuff()->ReleaseBuff(nsBuff::BK_CANDY_SIZE_CHANGE);

			// 스케일 크기가 틀리다면 버프셋팅
			//if( ( fScale != pObject->GetOrgGameScale() )&& nEndTick > 0 )
			{
				uint nTime = _TIME_TS + nEndTick / 1000;
				((CTamer*)pObject)->GetBuff()->SetBuff(nsBuff::BK_CANDY_SIZE_CHANGE, 0, nTime, 0);
			}
		}
		else	// CheckSizeEffect , UI->ResetTall 은 SetSCale 이후에 호출 되어야 갱신된 값을 얻어올 수 있습니다.
		{

			pObject->SetScale(fScale, true);
			((CTamer*)pObject)->GetBuff()->ReleaseBuff(nsBuff::BK_CANDY_SIZE_CHANGE);
		}
	}
	break;
	}
}

void cCliGame::RecvSyncData(void)
{
	if (m_bPortalProcessing)
	{
		// 처리하지 않는다.
		DBG("PORTAL PROCESS LOCKING");
		return;
	}

	static uint s_nMaxSize = 0;
	if (s_nMaxSize < iReceiver::GetPacket()->m_wSize)
	{
		s_nMaxSize = iReceiver::GetPacket()->m_wSize;
		DBG2("RecvSyncData(packet max size:%d)", s_nMaxSize);
	}

	u1 cSyncType;
	pop(cSyncType);

	while (cSyncType)
	{
		switch (cSyncType)
		{
		case pSync::Walk: SyncWalkObject();		break;
		case pSync::Move: SyncMoveObject();		break;

		case pSync::In: SyncInObject();		break;
		case pSync::Out: SyncOutObject();		break;
		case pSync::New: SyncNewObject();		break;
		case pSync::Delete: SyncDeleteObject();	break;

		case pSync::MsgChat: SyncMsgChat();		break;
		case pSync::MsgWhisper: SyncMsgWhisper();		break;
		case pSync::MsgAll: SyncMsgAll();			break;
		case pSync::MsgSystem: SyncMsgSystem();		break;
#ifdef GUILD_RENEWAL
		case pSync::MsgGuildShout: SyncMsgShout(true);	break;	// 길드 확성기 스킬
#endif
		case pSync::MsgShout: SyncMsgShout();		break;

		case pSync::MsgAllByItem: SyncMsgAllByItem();	break;

		case pSync::Condition: SyncCondition();		break;
		case pSync::BuffSync: SyncInBuff();			break;

		default: xassert1(false, "(SyncType:%d)", cSyncType);
		}

		pop(cSyncType);
	}
}


void cCliGame::SyncCondition(void)
{
	u2 cnt = 0;
	pop(cnt);

	u4 nUID = 0;
	u4 nCondition = 0;
	u4 nExtBuff = 0;

	for (int i = 0; i < cnt; i++)
	{
		pop(nUID);
		pop(nCondition);

		CsC_AvObject* pObj = g_pMngCollector->GetObject(nUID);
		if (pObj == NULL)
			continue;

		switch (pObj->GetLeafRTTI())
		{
		case RTTI_TAMER_USER:
		case RTTI_TAMER:
			pop(nExtBuff);
			((CTamer*)pObj)->SetCondition(nCondition);
			//( (CTamer*)pObj )->SetExpCondition( nExtBuff );
			break;
		case RTTI_DIGIMON_USER:
		case RTTI_DIGIMON:
			((CDigimon*)pObj)->SetCondition(nCondition);
			break;
		case RTTI_MONSTER:
			((CMonster*)pObj)->SetCondition(nCondition);
			break;
		default:
			assert_cs(false);
		}
	}
}


void cCliGame::SyncNewObject(void)
{
	u2 cnt = 0;
	pop(cnt);

	xassert1(cnt < 1000, "cnt(%d) is too big", cnt);
	//	DBG("SyncNew Cnt(%d)", cnt);

	nSync::Pos pos;
	cType type;


	while (cnt)
	{
		pop(pos);
		pop(type);

		xassert2(pos.m_nX > 0, "(%d, %d)", pos.m_nX, pos.m_nY);
		xassert2(pos.m_nY > 0, "(%d, %d)", pos.m_nX, pos.m_nY);

		switch (type.m_nClass)
		{
		case nClass::Tamer: SyncNewTamer(pos, type);			break;
		case nClass::Digimon: SyncNewDigimon(pos, type);		break;
		case nClass::Monster: SyncNewMonster(pos, type);		break;
		case nClass::Item: SyncNewItem(pos, type);			break;
		case nClass::CommissionShop: SyncNewCommissionShop(pos, type);	break;
		}
		--cnt;
	}
}


void cCliGame::SyncInObject(void)
{
	u2 cnt = 0;
	pop(cnt);

	xassert1(cnt < 1000, "cnt(%d) is too big", cnt);

	nSync::Pos pos;
	cType type;

	while (cnt)
	{
		pop(pos);
		pop(type);

		xassert2(pos.m_nX > 0, "(%d, %d)", pos.m_nX, pos.m_nY);
		xassert2(pos.m_nY > 0, "(%d, %d)", pos.m_nX, pos.m_nY);

		switch (type.m_nClass)
		{
		case nClass::Tamer: SyncInTamer(pos, type);			break;
		case nClass::Digimon: SyncInDigimon(pos, type);			break;
		case nClass::Monster: SyncInMonster(pos, type);			break;
		case nClass::Item: SyncInItem(pos, type);			break;
		case nClass::CommissionShop: SyncInCommissionShop(pos, type);	break;
		}
		--cnt;
	}
}


void cCliGame::SyncOutObject(void)
{
	u2 cnt = 0;
	pop(cnt);

	xassert1(cnt < 1000, "cnt(%d) is too big", cnt);

	u4 nUID = 0;
	nSync::Pos pos; // sync pos
	cSyncUnit* out = null;
	cType type;

	while (cnt)
	{
		pop(nUID);
		pop(pos);

		type.m_nUID = nUID;

		// Patched: was GetClass(nUID), which calls the legacy free helper
		// `__forceinline uint GetClass(u2 nUID) { return nUID >> 14; }`. With u4 nUID,
		// the implicit narrowing conversion to u2 drops bits 16-31, so for nClass >= 4
		// (Monster=4, Npc=5, Party=6, CommissionShop=7) the dispatch silently fell into
		// the default case (xassert no-op in release). Reading via the bitfield works
		// for all classes — type.m_nUID was just assigned from nUID above, so
		// type.m_nClass extracts bits 14-18 correctly.
		switch (type.m_nClass)
		{
		case nClass::Digimon:	g_pCharMng->DeleteDigimon(type.m_nIDX);								break;
		case nClass::Tamer:	g_pCharMng->DeleteTamer(type.m_nIDX);								break;
		case nClass::Monster:	g_pCharMng->DeleteMonster(type.m_nIDX, CsC_AvObject::DS_FADEOUT);	break;
		case nClass::Item:	g_pItemMng->DeleteItem(type.m_nIDX);								break;
		case nClass::CommissionShop:	g_pCharMng->DeleteEmployment(type.m_nIDX);							break;
		default:						xassert1(false, "unknown uid(%d)", nUID);
		}
		xassert2(pos.m_nX > 0, "(%d, %d)", pos.m_nX, pos.m_nY);
		xassert2(pos.m_nY > 0, "(%d, %d)", pos.m_nX, pos.m_nY);

		--cnt;
	}
}


void cCliGame::SyncDeleteObject(void)
{
	u2 cnt = 0;
	pop(cnt);

	xassert1(cnt < 1000, "cnt(%d) is too big", cnt);

	u4 nUID = 0;
	cSyncUnit* out = null;
	cType type;

	while (cnt)
	{
		pop(nUID);

		type.m_nUID = nUID;
		// Patched: was GetClass(nUID), which calls the legacy free helper
		// `__forceinline uint GetClass(u2 nUID) { return nUID >> 14; }`. With u4 nUID,
		// the implicit narrowing conversion to u2 drops bits 16-31, so for nClass >= 4
		// (Monster=4, Npc=5, Party=6, CommissionShop=7) the dispatch silently fell into
		// the default case (xassert no-op in release). Reading via the bitfield works
		// for all classes — type.m_nUID was just assigned from nUID above, so
		// type.m_nClass extracts bits 14-18 correctly.
		switch (type.m_nClass)
		{
		case nClass::Digimon:
		{
			CDigimon* pDigimon = g_pCharMng->GetDigimon(type.m_nIDX);
			if ((pDigimon) && (pDigimon->GetLeafRTTI() != RTTI_DIGIMON_USER))
				pDigimon->GetProp_Effect()->AddEffect_FT(EFFECT_CLOSED_SCENE);

		}
		g_pCharMng->DeleteDigimon(type.m_nIDX);
		break;
		case nClass::Tamer:
		{
			CTamer* pTamer = g_pCharMng->GetTamer(type.m_nIDX);
			if ((pTamer) && (pTamer->GetLeafRTTI() != RTTI_TAMER_USER))
				pTamer->GetProp_Effect()->AddEffect_FT(EFFECT_CLOSED_SCENE);

			if (pTamer)
				pTamer->PlaySound(SOUND_SKILL_UP);
		}
		g_pCharMng->DeleteTamer(type.m_nIDX);
		break;
		case nClass::Monster:
			g_pCharMng->DeleteMonster(type.m_nIDX, CsC_AvObject::DS_COSTOM_01);
			break;
		case nClass::Item:
			g_pItemMng->DeleteItem(type.m_nIDX);
			break;
		case nClass::CommissionShop:
		{
			if (g_pCharMng->IsEmployment(type.m_nIDX))
			{
				g_pCharMng->DeleteEmployment(type.m_nIDX);
			}
#ifdef SYNC_DATA_LIST
			else
			{
				CCharMng::sSYNC_DATA* pData = g_pCharMng->NewSync();
				pData->s_Type = CCharMng::sSYNC_DATA::DEL_EMPLOYMENT;
				pData->s_EmploymentData.s_Type = type;
				g_pCharMng->InsertSync_2(pData, 0);
			}
#endif
		}
		break;
		default:
			xassert1(false, "unknown uid(%d)", nUID);
		}
		--cnt;
	}
}


void cCliGame::SyncMoveObject(void)
{
	u2 cnt = 0;
	pop(cnt);

	xassert1(cnt < 1000, "cnt(%d) is too big", cnt);

	u4 nUID = 0;
	nSync::Pos pos;
	cType type;

	while (cnt)
	{
		pop(nUID);
		pop(pos);

		xassert2(pos.m_nX > 0, "(%d, %d)", pos.m_nX, pos.m_nY);
		xassert2(pos.m_nY > 0, "(%d, %d)", pos.m_nX, pos.m_nY);

		type.m_nUID = nUID;
		
		// Patched: was GetClass(nUID), which calls the legacy free helper
		// `__forceinline uint GetClass(u2 nUID) { return nUID >> 14; }`. With u4 nUID,
		// the implicit narrowing conversion to u2 drops bits 16-31, so for nClass >= 4
		// (Monster=4, Npc=5, Party=6, CommissionShop=7) the dispatch silently fell into
		// the default case (xassert no-op in release). Reading via the bitfield works
		// for all classes — type.m_nUID was just assigned from nUID above, so
		// type.m_nClass extracts bits 14-18 correctly.
		switch (type.m_nClass)
		{
		case nClass::Tamer: SyncMoveTamer(type.m_nIDX, pos);							break;
		case nClass::Digimon: SyncMoveDigimon(type.m_nIDX, pos);						break;
		case nClass::Monster: SyncMoveMonster(type.m_nIDX, pos, false);					break;
		case nClass::Npc: xassert(false, "nClass::NPC");								break;
		case nClass::Item: xassert(false, "nClass::Item");								break;
		case nClass::CommissionShop: xassert(false, "nClass::nClass::CommissionShop");	break;
		default: xassert(false, "GetClass(nUID) error");
		}

		--cnt;
	}
}


void cCliGame::SyncWalkObject(void)
{
	u2 cnt = 0;
	pop(cnt);

	xassert1(cnt < 1000, "cnt(%d) is too big", cnt);

	u4 nUID = 0;
	nSync::Pos pos;
	
	cType type;

	while (cnt)
	{
		pop(nUID);
		pop(pos);

		DBG("\nSyncWalkObject\n", nUID);
		DBG("nUID: %d\n", nUID);

		xassert2(pos.m_nX > 0, "(%d, %d)", pos.m_nX, pos.m_nY);
		xassert2(pos.m_nY > 0, "(%d, %d)", pos.m_nX, pos.m_nY);
		xassert2((nsCsGBTerrain::g_pCurRoot == NULL) || (pos.m_nX < nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth()), "(%d, %d)", pos.m_nX, pos.m_nY);
		xassert2((nsCsGBTerrain::g_pCurRoot == NULL) || (pos.m_nY < nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight()), "(%d, %d)", pos.m_nX, pos.m_nY);

		type.m_nUID = nUID;

		auto classType = GetClass(nUID);

		DBG("Class type: %d\n", classType);

		// Patched: was GetClass(nUID), which calls the legacy free helper
		// `__forceinline uint GetClass(u2 nUID) { return nUID >> 14; }`. With u4 nUID,
		// the implicit narrowing conversion to u2 drops bits 16-31, so for nClass >= 4
		// (Monster=4, Npc=5, Party=6, CommissionShop=7) the dispatch silently fell into
		// the default case (xassert no-op in release). Reading via the bitfield works
		// for all classes — type.m_nUID was just assigned from nUID above, so
		// type.m_nClass extracts bits 14-18 correctly.
		switch (type.m_nClass)
		{
		case nClass::Tamer: SyncMoveTamer(type.m_nIDX, pos);							break;
		case nClass::Digimon: SyncMoveDigimon(type.m_nIDX, pos);						break;
		case nClass::Monster: SyncMoveMonster(type.m_nIDX, pos, true);					break;
		case nClass::Npc: xassert(false, "nClass::NPC");								break;
		case nClass::Item: xassert(false, "nClass::Item");								break;
		case nClass::CommissionShop: xassert(false, "nClass::nClass::CommissionShop");	break;
		default: xassert(false, "GetClass(nUID) error");
		}

		--cnt;
	}
}


void cCliGame::SyncNewDigimon(nSync::Pos& pos, cType& type)
{
	SyncInDigimon(pos, type, true);
}


void cCliGame::SyncNewTamer(nSync::Pos& pos, cType& type)
{
	SyncInTamer(pos, type, true);
}


void cCliGame::SyncNewMonster(nSync::Pos& pos, cType& type)
{
	SyncInMonster(pos, type, true);
}


void cCliGame::SyncNewItem(nSync::Pos& pos, cType& type)
{
	//	DBG("type:%d", type.m_nType);

	SyncInItem(pos, type, true);
}

void cCliGame::SyncNewCommissionShop(nSync::Pos& pos, cType& type)
{
	SyncInCommissionShop(pos, type, true);
}

void cCliGame::SyncInItem(nSync::Pos& pos, cType& type, bool bNew)
{
	u4 nOwnerTamerUID;
	u1 nItemState;

	pop(nOwnerTamerUID);
	pop(nItemState);

	g_pItemMng->AddItem(type.m_nIDX, type.m_nType, pos, nOwnerTamerUID, nItemState);
}

void cCliGame::SyncInTamer(nSync::Pos& pos, cType& type, bool bNew)
{	// 내 캐릭터면 패스
	xstop(!g_pCharMng->IsTamerUser(type.m_nIDX), "내테이머는 받지 말자");

#ifdef COMPAT_487
	uint fDirect;
#else
	float fDirect;
#endif
	
	u2 nMoveSpeed;
	u1 nLevel;

	nSync::Pos DstPos;

	pop(DstPos);

	std::wstring szName;
	pop(szName);
	pop(nLevel);
	pop(fDirect);
	pop(nMoveSpeed);

	u1 nHpRate;
	pop(nHpRate);

	cItemData ItemData[nLimit::Equip];
	pop(ItemData, sizeof(ItemData));

	//assert_cs( nLimit::Equip == nsPART::MAX_TOTAL_COUNT );
	CsC_PartObject::sCHANGE_PART_INFO cp[nsPART::MAX_TOTAL_COUNT];
#ifdef GM_CLOCKING
	bool bIsClocking = false;
#endif
	for (int i = 0; i < nsPART::MAX_TOTAL_COUNT/*nLimit::Equip*/; ++i)
	{
		// 디지바이스
		if (i >= nTamer::MaxParts)
		{
			cp[i].s_nPartIndex = i;
			break;
		}
		cp[i].s_nFileTableID = ItemData[i].m_nType;
		cp[i].s_nRemainTime = ItemData[i].m_nEndTime;
		cp[i].s_nPartIndex = i;
#ifdef GM_CLOCKING
		if (ItemData[i].m_nType == CLOCKING_ITEM_ID)
		{
			bIsClocking = true;
		}
#endif
	}

#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
	// 디지바이스 정보 받아서 처리해줘야 디지바이스 이펙트 띄워줄 수 있음
	cItemData cDigiviceItem;
	pop(&cDigiviceItem, sizeof(cDigiviceItem));
	cp[nTamer::MaxParts].s_nFileTableID = cDigiviceItem.m_nType;
	cp[nTamer::MaxParts].s_nRemainTime = cDigiviceItem.GetEndTime();
#endif

	u4 nCondition;
	u4 nSync;
	u4 nPartnerUID;

	pop(nCondition);
	pop(nSync);
	pop(nPartnerUID);

	float fTamerScale = 1.0f;
	u2 u2TamerScale;
	pop(u2TamerScale);
	if (u2TamerScale > 0)
		fTamerScale = u2TamerScale * 0.0001f;

	CTamer* pTamer = NULL;

	// 현재의 맵 번호.
	int dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	// 튜토 리얼 중에만 닷트본부 지하수도에 있음. 이 경우에만 출력안함.
	if (dwMapID == 4)
	{
		pTamer = NULL;
	}
	else
	{
		pTamer = g_pCharMng->AddTamer(type.m_nIDX, type.m_nType, pos, fDirect, szName.c_str(), cp, nSync);
	}
	if (pTamer)
	{
		int DigimonIDX = GetIDX(nPartnerUID);
		pTamer->SetDigimonLink(DigimonIDX);
		pTamer->GetBaseStat()->SetLevel(nLevel);
		pTamer->GetBaseStat()->SetMoveSpeed((float)nMoveSpeed);
		if (bNew)
		{
			//DWORD dwPlag = nsEFFECT::POS_CHARPOS;
			pTamer->GetProp_Effect()->AddEffect_FT(EFFECT_CREATE_SCENE);
		}
		pTamer->SetScale(fTamerScale, false);
	}

#ifdef COMPAT_487
	u1 nGuildClass;
	u4 nGuildIDX;
#else
	u1 nGuildClass;
	u4 nGuildIDX;	// 길드 식별자
#endif
	

	std::wstring szGuildName;
	pop(nGuildClass);	// 길드 내에서의 레벨 nGuild::Member, nGuild::Master, nGuild::SubMaster
	if (nGuildClass)
	{
		pop(nGuildIDX);
		pop(szGuildName);
	}

	if (pTamer)
	{
		pTamer->GetCharOption()->DeleteGuildName();
		if (szGuildName.length() > 0)
		{
			pTamer->GetCharOption()->SetGuildName(szGuildName.c_str());
			pTamer->GetCharOption()->SetGuildUID(nGuildIDX);
		}
	}

	u2 nTamerAchievement;
	pop(nTamerAchievement);
	if (pTamer)
		pTamer->GetCharOption()->SetAchieve(nTamerAchievement);

#ifdef MASTERS_MATCHING
	u1 nTeam = 0;
	pop(nTeam);
	if (pTamer)
		pTamer->GetCharOption()->SetMatchIcon(nTeam);
#endif

	u2 nCard = 0;
	pop(nCard);

	if (0 != nCard) {

		CsMaster_CardLeader::sINFO* sInfo = nsCsFileTable::g_pMaster_CardMng->GetMasterCardLeader(nCard)->GetInfo();

		CsMaster_Card::MAP_IT	it = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->begin();
		CsMaster_Card::MAP_IT	it_end = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->end();
		float fscale = 0.1f;

		for (; it != it_end; ++it)
		{
			if (it->second->GetInfo()->s_nDigimonID == sInfo->s_nDigimonID)
			{
				fscale = it->second->GetInfo()->s_nScale * 0.01f;
			}
		}
		if (pTamer)
		{
			pTamer->DeletePat();
			pTamer->AddPat(sInfo->s_nDigimonID, fscale);
		}
	}

	if (nCondition & nSync::Shop)
	{
		std::wstring szShopTitle;
		pop(szShopTitle);

		if (szShopTitle.empty())
			szShopTitle = L"???";

		ContentsStream kTmp;
		uint checkid = pTamer->GetUniqID();
		int nType = cTalkBalloon::MAX_TYPE;
		kTmp << checkid << szShopTitle << nType;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_BALLOON_TITLE, &kTmp);

		// 		GS2C_RECV_STORE_NAME recv;
		// 		recv.szShopTitle = szShopTitle;
		// 		GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_SET_STORENAME, &recv );
	}

	u4 nChangeCostumeNo = 0;
	pop(nChangeCostumeNo);

	if (nChangeCostumeNo != 0)
	{
		CsC_PartObject::sCHANGE_PART_INFO cp;
		cp.s_nFileTableID = nChangeCostumeNo;
		cp.s_nPartIndex = nsPART::Costume;
		cp.s_nRemainTime = INT_MAX;
		if (pTamer)
		{
			pTamer->ChangePart(&cp);
			pTamer->SetPcbangCostume(nChangeCostumeNo);
		}
	}

	// 이동
	if (pos != DstPos)
	{
		SyncMoveTamer(type.m_nIDX, DstPos);
	}

	// hp
	if (pTamer)
	{
		pTamer->GetBaseStat()->SetHP(nHpRate);
		pTamer->SetCondition(nCondition);
#ifdef GM_CLOCKING
		pTamer->SetClocking(bIsClocking);
#endif
	}

#ifdef GUILD_RENEWAL
	int nColorLv = 0;	// 색 변경 우선순위. 색 변경 스킬 효과 추가 시 우선순위 체크
	u4 nSkillCode = 0;		// 0: 색 미변경, !0: 색 변경 베이스스킬 코드
	NiColor pColor;
	pop(nSkillCode);

	if (nSkillCode == 0)
	{
		pColor = DEFAULT_NAMECOLOR;
	}
	else
	{
		if (pTamer)
			pTamer->GetCharOption()->SetSkillCode(nSkillCode);

		CsSkill::sINFO* pInfo = nsCsFileTable::g_pSkillMng->GetSkill(nSkillCode)->GetInfo();

		for (int i = 0; i < SKILL_APPLY_MAX_COUNT; i++)
		{
			switch (pInfo->s_Apply[i].s_nA)
			{
			case GUILD_NAMECOLOR:	// 색 변경
				if (nColorLv < 1)
				{
					float fR = (float)(pInfo->s_Apply[i].s_nB / 100);
					float fG = (float)((pInfo->s_Apply[i].s_nB / 10) % 10);
					float fB = (float)(pInfo->s_Apply[i].s_nB % 10);
					pColor = NiColor(fR / 10.0f, fG / 10.0f, fB / 10.0f);
					break;
					nColorLv = 1;
				}
			default:
				break;
			}
		}
	}

	if (pTamer)
	{
		std::pair<int, bool> isPartyMember;
		isPartyMember.first = pTamer->GetUniqID();
		GAME_EVENT_ST.OnEvent(EVENT_CODE::IS_PARTYMEMBER_FROM_TAMERUIDX, &isPartyMember);
		if (isPartyMember.second)
			pColor = PARTY_NAMECOLOR;

		pTamer->GetCharOption()->SetNameColor(pColor);
	}

#endif
}

void cCliGame::SyncInDigimon(nSync::Pos& pos, cType& type, bool bNew)
{
	// 내 캐릭을 이동 시키려고 하면 막음
	//xstop(!g_pCharMng->IsDigimonUser(type.m_nIDX), "내테이머는 받지 말자");

	float fDirect = 0.0f;
	u2 nMoveSpeed = 0;
	u2 nAttackSpeed = 0;
	u1 nLevel = 0;
	u2 nScale = 0; // 7000~13000

	nSync::Pos DstPos;
	pop(DstPos);

	std::wstring szName;
	pop(szName);
	pop(nScale); // 용병 관련 스케일 적용	

	pop(nLevel);
	pop(fDirect);
	pop(nMoveSpeed);
	pop(nAttackSpeed);

	CDigimon* pDigimon = NULL;
	// 현재의 맵 번호.
	int dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	// 튜토리얼이 아닌 경우에만 세팅
	if (dwMapID != 4)
	{
		if (g_pCharMng->IsDigimonUser(type.m_nIDX))
			pDigimon = g_pCharMng->GetDigimonUser(0);
		else
			pDigimon = g_pCharMng->AddDigimon(type.m_nIDX, type.m_nType, NULL, pos, fDirect, szName.c_str());
	}

	if (pDigimon)
	{
		pDigimon->SetScale(nScale * 0.0001f);
		pDigimon->GetBaseStat()->SetLevel(nLevel);
		pDigimon->GetBaseStat()->SetMoveSpeed((float)nMoveSpeed);
		pDigimon->GetBaseStat()->SetAttackSpeed(nAttackSpeed);
		if (bNew)
		{
			DWORD dwPlag = nsEFFECT::POS_CHARPOS;
			pDigimon->GetProp_Effect()->AddEffect_FT(EFFECT_CREATE_SCENE);
		}
	}

	// 이동
	if (pos != DstPos)
	{
		SyncMoveDigimon(type.m_nIDX, DstPos);
	}

	// 테이머 링크
	u4 nTamerUID = 0;
	pop(nTamerUID);
	if (pDigimon)
		pDigimon->SetTamerLink(GetIDX(nTamerUID));

	// HP
	u1 nHpRate = 0;
	pop(nHpRate);
	if (pDigimon)
		pDigimon->GetBaseStat()->SetHP(nHpRate);

	u4 nCondition = 0;
	pop(nCondition);
	if (pDigimon)
		pDigimon->SetCondition(nCondition);

	if (nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
	{
		u1 nBattleTeam;
		pop(nBattleTeam);

		if (pDigimon)
		{
			pDigimon->SetBattleTeam(nBattleTeam);
			// 			if( g_pDataMng->GetBattle()->IsMyTeam( nBattleTeam ) == true )
			// 				pDigimon->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_B.nif" );
			// 			else
			// 				pDigimon->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BATTLE_TEAM, "system\\IFF\\IFF_R.nif" );
		}
	}

	u2 nInchantLevel = 0;
	pop(nInchantLevel);

	u2 nExtentionParameterLV[nExtendState::MaxExtendStat] = { 0, };
	pop(nExtentionParameterLV[nExtendState::ET_AT]);
	pop(nExtentionParameterLV[nExtendState::ET_BL]);
	pop(nExtentionParameterLV[nExtendState::ET_CR]);
	pop(nExtentionParameterLV[nExtendState::ET_AS]);
	pop(nExtentionParameterLV[nExtendState::ET_EV]);
	pop(nExtentionParameterLV[nExtendState::ET_HT]);
	pop(nExtentionParameterLV[nExtendState::ET_HP]);

#ifdef COMPAT_487
	int remainingBytes = 0;
	pop(remainingBytes);
#endif

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	u4 nPartsType = 0;
	pop(nPartsType);	// 디지몬 파츠 아이템 정보
#endif

	if (pDigimon)
	{
		CDigimon::sENCHENT_STAT* pDStat = pDigimon->GetEnchantStat();
		if (pDStat)
		{
			pDStat->SetEnchantLevel(nInchantLevel);

			for (int i = nExtendState::ET_AT; i < nExtendState::MaxExtendStat; ++i)
			{
				pDStat->SetEnchantInfo(i, nExtentionParameterLV[i], 0);
			}
		}

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
		// 디지몬 이펙트 추가
		nsDPART::sDPART_INFO partInfo;
		partInfo.s_nFileTableID = nPartsType;
		if (nsCsFileTable::g_pItemMng->GetDPartsInfo(nPartsType, pDigimon->GetFTID(), partInfo.s_nPartIndex, partInfo.s_File))
			pDigimon->ChangeParts(partInfo);
#endif
	}

	// 테이머 상태 체크
	CTamer* pTamer = g_pCharMng->GetTamer(GetIDX(nTamerUID));
	if (pTamer && pTamer->GetCondition()->IsCondition((nSync::eCondition)(nSync::Shop | nSync::ShopReady)))
	{
		if (pDigimon)
			pDigimon->SetEnableObject(false);
	}

}

void cCliGame::RecvSyncEnchantEffect()
{
	u4 nDigimonIDX = 0;

	pop(nDigimonIDX);
	CDigimon* pDigimon = g_pCharMng->GetDigimon(nDigimonIDX);

	u2 nInchantLevel = 0;
	pop(nInchantLevel);

	u2 nAPLevel = 0;	// 공격 단계
	u2 nDPLevel = 0;	// 방어 단계
	u2 nCALevel = 0;	// 크리티컬 단계
	u2 nEVLevel = 0;	// 회피 단계
	u2 nHPLevel = 0;	// 체력 단계
	pop(nAPLevel);
	pop(nDPLevel);
	pop(nCALevel);
	pop(nEVLevel);
	pop(nHPLevel);

	assert_cs(pDigimon != NULL);

	if (pDigimon == NULL)
	{
		//g_CriticalLog.Log( _T( "SyncEnchantEffect : idx = %d" ), nDigimonIDX );
		return;
	}

	CDigimon::sENCHENT_STAT* pDStat = pDigimon->GetEnchantStat();
	if (pDStat)
	{
		pDStat->SetEnchantLevel(nInchantLevel);

		pDStat->SetEnchantInfo(ET_AT, nAPLevel, 0);
		pDStat->SetEnchantInfo(ET_BL, nDPLevel, 0);
		pDStat->SetEnchantInfo(ET_CR, nCALevel, 0);
		pDStat->SetEnchantInfo(ET_EV, nEVLevel, 0);
		pDStat->SetEnchantInfo(ET_HP, nHPLevel, 0);
	}

	pDigimon->CheckEnchantEffect();
}

void cCliGame::SyncInMonster(nSync::Pos& pos, cType& type, bool bNew)
{
	CMonster::eMONSTER_STATE eMonsterState = bNew == true ? CMonster::MONSTER_CREATE : CMonster::MONSTER_NORMAL;

	CMonster* pMonster = NULL;
	CsMapMonster* tmpMon = nsCsMapTable::g_pMapMonsterMng->GetGroup(nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID)->GetMonster_ByMonsterID(type.m_nType);
	if (tmpMon != NULL && tmpMon->GetInfo()->s_nMoveType == 4)//오브젝트형 체크. 오브젝트형은 안돌고 안따라가야되
	{
		pMonster = g_pCharMng->AddMonster(type.m_nIDX, type.m_nType, pos, 0.0f, eMonsterState);
	}
	else
	{
		pMonster = g_pCharMng->AddMonster(type.m_nIDX, type.m_nType, pos, (rand() % 648 - 314) * 0.01f, eMonsterState);
	}

	nSync::Pos DstPos;
	pop(DstPos);

	if (tmpMon != NULL && tmpMon->GetInfo()->s_nMoveType == 4)//오브젝트형 체크. 오브젝트형은 안돌고 안따라가야되
		DstPos = pos;

	// hp
	u1 nHpRate;
	pop(nHpRate);
	if (pMonster)
		pMonster->GetBaseStat()->SetHP(nHpRate);

	u1 nLevel;
	pop(nLevel);
	if (pMonster)
		pMonster->GetBaseStat()->SetLevel(nLevel);

#ifdef MONSTER_SKILL_GROWTH
	u4 nMonSkill_Idx = 0;		// 몬스터스킬 인덱스
	u2 nHitterUID = 0;			// 나에게 버프를 건 대상 ( 자신일 수도 있음 )
	u4 nEndTS = UINT_MAX;				// 종료 시간
	u2 nBuffClassLevel = 0;		// 버프 레벨
	u4 nSkillCode = 0;			// 버프 스킬 코드
	int nStack = 0;				// 현재 버프 중첩 수

	// 몬스터스킬 테이블 내에 있는 스킬인지 확인
	int nSkill_MaxIdx = (int)nsCsFileTable::g_pMonsterMng->GetMonsterSkillMap()->size();
	pop(nMonSkill_Idx);
	pop(nStack);

	// 해당 스킬 정보
	CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill(nMonSkill_Idx);

	if (pSkillInfo == NULL || nStack == 0)	// 몬스터 스킬이 아니거나 중첩이 0인 경우
	{
#ifndef _GIVE
		// 		CHAT_TEXT2( _T( "몬스터 %d 현재 가지고 있는 버프 없음" ), type.m_nType );
#endif
	}
	else
	{
		switch (pSkillInfo->s_nSkillType)	// 스킬 효과별로 체크
		{
		case CsMonsterSkill::GROWTH:	// 몬스터 성장 스킬
		{
			//중첩 증가
			int nBuffCode = nsBuff::BK_MONSTER_GROWTH;

			//해당 버프 없으면 새로 셋팅
			if (pMonster && pMonster->GetBuff()->IsBuffData(nBuffCode) == false)
			{
				pMonster->GetBuff()->SetBuff(nBuffCode, nHitterUID, nEndTS, nBuffClassLevel/*, nSkillCode*/);
				// 이미 버프 받고 있는놈이면 중첩 증가
				pMonster->GetBuff()->GetBuffData(nBuffCode)->m_nStack = nStack;
			}
			//크기 키우기
			float fScale = nsCsFileTable::g_pMonsterMng->GetEffectVal_Scale(nMonSkill_Idx);

			if (fScale == 0.0f)
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
				//CProtocol.m_wStr = GetVAString(_T( "스케일 증가값 0\n\nSkill_Idx : %d" ), nMonSkill_Idx);
				DmCS::StringFn::Format(CProtocol.m_wStr, _T("스케일 증가값 0\n\nSkill_Idx : %d"), nMonSkill_Idx);
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
				break;
			}
			else
			{
				fScale = (fScale * (float)nStack) + 1.0f;	// 크기 증가값 * 중첩 수 + 1.0f 
				if (pMonster)
					pMonster->SetScale(fScale);
			}

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString(_T( "%s 스킬 ( %d )중첩 스케일 : %.2f" ), nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo()->s_szName, nStack, fScale);
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("%s 스킬 ( %d )중첩 스케일 : %.2f"), nsCsFileTable::g_pBuffMng->GetBuff(nBuffCode)->GetInfo()->s_szName, nStack, fScale);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		}
		break;
		default:
		{
			wstring str;
			//str = GetVAString( _T( "SyncInMonster :: 몬스터 스킬 효과(Effect)가 성장타입이 아님\n\n Idx : %d \n Effect : %d" ), nMonSkill_Idx, pSkillInfo->s_nSkillType );
			DmCS::StringFn::Format(str, _T("SyncInMonster :: 몬스터 스킬 효과(Effect)가 성장타입이 아님\n\n Idx : %d \n Effect : %d"), nMonSkill_Idx, pSkillInfo->s_nSkillType);

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			CProtocol.m_wStr = str;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
			break;
		}
		}
	}
#endif
	u4 nCondition;
	pop(nCondition);
	if (pMonster)
		pMonster->SetCondition(nCondition);

	// 이동
	// Walk
	u4 nIsWalk = nCondition & nSync::Run ? 0 : 1;
	if (pMonster && (bNew == false) && (pos != DstPos))
	{
		SyncMoveMonster(type.m_nIDX, DstPos, (nIsWalk != 0));
	}

	if (nCondition == 6/*nSync::Casting*/)	// 시전 중인 상태
	{
		// 스킬코드 -> 시전 
		u4 nSkillIdx = 0;
		pop(nSkillIdx);

		u2 nMonsterUID = type.m_nUID;
		if (nMonsterUID == 0 || nSkillIdx == 0)
			return;

		CsC_AvObject* pObject = g_pMngCollector->GetObject(type);
		if (pObject == NULL)
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString(_T( "SyncInMonster :: Sync 내에 존재하지 않는 몬스터 - UID : %d" ), nMonsterUID);
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("SyncInMonster :: Sync 내에 존재하지 않는 몬스터 - UID : %d"), nMonsterUID);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
			return;
		}

		CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill(nSkillIdx);
		if (pSkillInfo == NULL)
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString(_T( "SyncInMonster :: 존재하지 않는 몬스터스킬 - SkillIdx : %d" ), nSkillIdx);
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("SyncInMonster :: 존재하지 않는 몬스터스킬 - SkillIdx : %d"), nSkillIdx);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
			return;
		}

		// 		CHAT_TEXT_DEGUG( _T("SyncInMonster :: 스킬 시전 중 ( SkillType - %d ) - TimeTag%d"), pSkillInfo->s_nSkillType, _TIME_TS%100 );
				//=============================================================================
				// 스킬 사용하는 몬스터 애니 셋팅
				//=============================================================================
		if (pSkillInfo->s_nCastCheck == 1)
		{
			pObject->SetPause(CsC_AvObject::PAUSE_PATH, true);
			//시전 애니
			pObject->GetProp_Animation()->ResetAnimation();
			pObject->SetAnimation(pSkillInfo->s_dwSequenceID + 20);
			((CMonster*)pObject)->SetCasting_Monster(true);
		}
	}

	// 씨앗 Sync
	// 일단 있던 씨앗 다 지움
	g_pCharMng->DeleteTempObject(type.m_nUID);

	u4 nCnt;
	pop(nCnt);

	for (u4 i = 0; i < nCnt; i++)
	{
		u4 nSkillIdx = 0;
		pop(nSkillIdx);

		u2 nMonsterUID = type.m_nUID;
		if (nMonsterUID == 0 || nSkillIdx == 0)
			return;

		CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill(nSkillIdx);
		if (pSkillInfo == NULL)
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString( _T( "SyncInMonster :: 존재하지 않는 몬스터스킬 - SkillIdx : %d" ), nSkillIdx );
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("SyncInMonster :: 존재하지 않는 몬스터스킬 - SkillIdx : %d"), nSkillIdx);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
			return;
		}
#define CENTERFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER | nsEFFECT::LIVE_LOOP
#define FOOTFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::LIVE_LOOP
#define HEIGHTFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT | nsEFFECT::LIVE_LOOP

		switch (pSkillInfo->s_nSkillType)
		{
		case CsMonsterSkill::ATTACH_SEED:	// 씨앗 깔기
		{
			int nPosX, nPosY;
			n4 nEffectLiftTime = 0;
			pop(nPosX);
			pop(nPosY);
			pop(nEffectLiftTime);

			//바닥 이펙트용 더미 오브젝트 생성
			float fPosZ = nsCsGBTerrain::g_pCurRoot->GetHeight((float)nPosX, (float)nPosY);
			NiPoint3 vPos = NiPoint3((float)nPosX, (float)nPosY, fPosZ);
			CsC_AvObject* pDummyTarget = g_pCharMng->AddTempObject(nMonsterUID, vPos);
			SAFE_POINTER_CON(pDummyTarget);

			char szEff[OBJECT_PATH_LEN] = { 0, };
			if (strlen(pSkillInfo->s_szNoticeEff_Name) > 0)
				sprintf_s(szEff, OBJECT_PATH_LEN, pSkillInfo->s_szNoticeEff_Name);
			else
				sprintf_s(szEff, OBJECT_PATH_LEN, "system\\Raid_Skill\\Dot_Damage_Circle.nif");

			CsC_EffectProp* pDummyEffProp = pDummyTarget->GetProp_Effect();
			if (pDummyEffProp)
			{
				DWORD dwFlag = nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER | nsEFFECT::LIVE_LOOP;
				pDummyEffProp->AddSkillLoopEffect(szEff, nSkillIdx, dwFlag, false, 1.0f, nEffectLiftTime * 0.001f);
				CsC_AvObject* pLoopSkillEf = pDummyEffProp->_GetLoopSkillEffect(nSkillIdx);
				if (pLoopSkillEf)
				{
					int nRange = 0;
					CsMonsterSkillTerms::sINFO* pSkillTerms = nsCsFileTable::g_pMonsterMng->GetMonsterSkillTerms(pSkillInfo->s_nRangeIdx);
					if (pSkillTerms)
						nRange = pSkillTerms->s_nRange;

					// 기본적으로 광역 스킬 이펙트는 8M ( 범위 800 ) 기준으로 리소스 제작되므로
					// 리소스를 테이블에서 설정한 범위에 따라 크기 변경하기
					float fScale = 1.0f;
					if (nRange > 0)
						fScale = (float)nRange / 800.0f;
					pLoopSkillEf->SetScale(fScale);
				}
			}
		}
		break;
		default:
			break;
		}
	}
}

#ifdef SYNC_DATA_LIST

void cCliGame::SyncInCommissionShop(nSync::Pos& pos, cType& type, bool bNew/* =false */)
{
	CCharMng::sSYNC_DATA* pSync = g_pCharMng->NewSync();
	pSync->s_Type = CCharMng::sSYNC_DATA::ADD_EMPLOYMENT;
	CCharMng::sSYNC_DATA::sEMPLOYMENT* pData = &pSync->s_EmploymentData;

	pData->s_Pos = pos;
	pData->s_Type = type;
	pData->s_bNew = bNew;
	pop(pData->s_nFloat);			// 회전 값
	pop(pData->s_nItemType);		// 사용된 아이템 타입
	pop(pData->s_szShopName);		// 상점 이름

	if (pData->s_nItemType == 0)
	{
		g_CriticalLog.Log(_T("위탁상점 정보 이상 : UID = %d"), type.m_nUID);
		g_pCharMng->DelSync(pSync);
		return;
	}

	g_pCharMng->InsertSync_2(pSync, SYNC_DATA_LIST_FRAME_2);
}

#else

void cCliGame::SyncInCommissionShop(nSync::Pos& pos, cType& type, bool bNew/* =false */)
{
	float nFloat = 0;
	uint nItemType = 0;
	std::wstring szShopName;

	pop(nFloat);			// 회전 값
	pop(nItemType);		// 사용된 아이템 타입
	pop(szShopName);		// 상점 이름

	if (nItemType == 0)
	{
		g_CriticalLog.Log(_T("위탁상점 정보 이상 : UID = %d"), type.m_nUID);
		return;
	}

	NiPoint3 vpos((float)pos.m_nX, (float)pos.m_nY, 0.0f);
	vpos.z = nsCsGBTerrain::g_pCurRoot->GetHeight(vpos);
	CEmployment* pEmployment = g_pCharMng->AddEmployment(type.m_nIDX, nItemType, vpos, nFloat);


	ContentsStream kTmp;
	int nType = cTalkBalloon::MAX_TYPE;
	if (szShopName.empty())
		szShopName = L"???";

	kTmp << type.m_nUID << szShopName << nType;

	GAME_EVENT_ST.OnEvent(EVENT_CODE::SET_BALLOON_TITLE, &kTmp);

	// #ifdef CONSIGNMENT_BALLOON_CASH
	// 	CsC_AvObject* pTarget = g_pMngCollector->GetObject( type.m_nUID );
	// 	TCHAR* szFileName = nsCsFileTable::g_pBaseMng->IsBallonNewType( pTarget->GetFTID() );
	// 	if( szFileName != NULL )
	// 	{
	// 		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_6, type.m_nUID, szShopName.c_str(), true,
	// 			cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
	// 	}
	// 	else
	// 	{
	// 		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, type.m_nUID, szShopName.c_str(), true,
	// 			cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
	// 	}
	// #else
	// 	g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, type.m_nUID, szShopName.c_str(), true,
	// 								 cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
	// #endif
}

#endif

void cCliGame::SyncMoveDigimon(u4 nIDX, nSync::Pos& pos)
{
	CsC_AvObject* pObject = g_pCharMng->GetDigimon(nIDX);

	if (pObject == NULL)
		return;

	// 내 캐릭을 이동 시키려고 하면 막음
	if (pObject->GetLeafRTTI() == RTTI_DIGIMON_USER)
		return;

	if (pObject->GetProp_Path()->PathTest(NiPoint2((float)pos.m_nX, (float)pos.m_nY)))
	{
		pObject->GetProp_Path()->SetPath((float)pos.m_nX, (float)pos.m_nY);
	}
}


void cCliGame::SyncMoveMonster(u4 nIDX, nSync::Pos& pos, bool bWalk)
{
	/*
		if(!bWalk)
		{
			LOG("위치 : %d %d", pos.m_nX, pos.m_nY);
		}
	//*/

	CMonster* pMonster = g_pCharMng->GetMonster(nIDX);


	if (pMonster)
	{
		u4 nMonID = pMonster->GetFTID();
		CsMapMonster* tmpMon = nsCsMapTable::g_pMapMonsterMng->GetGroup(nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID)->GetMonster_ByMonsterID(nMonID);

		if (tmpMon != NULL && tmpMon->GetInfo()->s_nMoveType == 4)//오브젝트형 체크. 오브젝트형은 안돌고 안따라가야되
		{
			return;
		}

		if (pMonster == g_pCharResMng->GetTargetMark())
		{
			int b = 0;
			NiPoint2 pp = pMonster->GetPos2D();
			int d = 0;
		}

		if (pMonster->GetProp_Path()->PathTest(NiPoint2((float)pos.m_nX, (float)pos.m_nY)))
		{
			pMonster->GetProp_Path()->SetMoveState(bWalk == true ? CsC_PathProp::MS_WALK : CsC_PathProp::MS_RUN);
			pMonster->GetProp_Path()->SetPath((float)pos.m_nX, (float)pos.m_nY);
		}
	}
}



void cCliGame::SyncMoveTamer(u4 nIDX, nSync::Pos& pos)
{
	// 내 캐릭터면 패스
	assert_cs(g_pCharMng->GetTamerUser()->GetIDX() != nIDX);

	CsC_AvObject* pObject = g_pCharMng->GetTamer(nIDX);
	if (pObject) {
		if (pObject->GetProp_Path()->PathTest(NiPoint2((float)pos.m_nX, (float)pos.m_nY)))
		{
			pObject->GetProp_Path()->SetPath((float)pos.m_nX, (float)pos.m_nY);
		}
	}
}


void cCliGame::SyncMsgChat(void)
{
	u1 nCnt = 0;
	pop(nCnt);

	for (int i = 0; i < nCnt; i++)
	{
		u4 nTamerUID = 0;
		pop(nTamerUID);
		std::wstring msg;
		pop(msg);

		assert_cs(g_pGameIF);
		ST_CHAT_PROTOCOL		CProtocol;
		CProtocol.m_Index = nTamerUID;
		CProtocol.m_Type = NS_CHAT::NORMAL;
		CProtocol.m_wStr = msg;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
}


void cCliGame::SyncMsgWhisper(void)
{
	uchar cnt = 0;
	pop(cnt);

	for (int i = 0; i < cnt; ++i)
	{
		char nRetCode = 0; // 0:미접속, 1:맵이동중, 2:없는 ID, 3:성공
		pop(nRetCode);
		std::wstring src;
		pop(src);
		std::wstring dst;
		pop(dst);
		std::wstring msg;
		pop(msg);

		char outstr[256] = { 0 };
		std::wstring totalMsg;

		switch (nRetCode)
		{
		case 0: // 접속중이지 않음
		{
			totalMsg = UISTRING_TEXT("CHAT_MESSAGE_FAIL_WHISPER_NOT_CONNECT");
			DmCS::StringFn::Replace(totalMsg, L"#Target#", dst.c_str());
		}
		break;
		case 1: // 맵이동중
		{
			totalMsg = UISTRING_TEXT("CHAT_MESSAGE_FAIL_WHISPER_CURRENT_MOVING");
			DmCS::StringFn::Replace(totalMsg, L"#Target#", dst.c_str());
		}
		break;
		case 2: // 존재하지 아니한 유저
		{
			totalMsg = UISTRING_TEXT("CHAT_MESSAGE_FAIL_WHISPER_NOT_EXIST");
			DmCS::StringFn::Replace(totalMsg, L"#Target#", dst.c_str());
		}
		break;
		case 3: // 성공
		{
			if (0 == src.compare(g_pCharMng->GetTamerUser()->GetName()))// 내가 보낸 귓말
				DmCS::StringFn::Format(totalMsg, _T("To.%s : %s"), dst.c_str(), msg.c_str());
			else// 내가 받은 귓말
			{
				DmCS::StringFn::Format(totalMsg, _T("From.%s : %s"), src.c_str(), msg.c_str());

				ST_CHAT_PROTOCOL CProtocol;
				CProtocol.m_Type = NS_CHAT::SET_FROMWHISPER;
				CProtocol.m_wStr = src;
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
			}
		}
		break;
		}

		ST_CHAT_PROTOCOL		CProtocol;
		CProtocol.m_Type = NS_CHAT::WHISPER;
		CProtocol.m_wStr = totalMsg;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
}


void cCliGame::SyncMsgAll(void)
{
	uchar cnt;
	pop(cnt);

	for (int i = 0; i < cnt; i++)
	{
		std::wstring src;
		pop(src);

		std::wstring msg;
		pop(msg);

		std::wstring totalMsg;
		DmCS::StringFn::Format(totalMsg, _T("[%s] %s : %s"), UISTRING_TEXT("CHAT_SYNC_MESSAGE_SHOUT").c_str(), src.c_str(), msg.c_str());

		assert_cs(g_pGameIF);
		ST_CHAT_PROTOCOL		CProtocol;
		CProtocol.m_Type = NS_CHAT::SHOUT;
		CProtocol.m_wStr = totalMsg;
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
}


void cCliGame::SyncMsgAllByItem(void)
{
	uchar cnt = 0;
	pop(cnt);
	for (int i = 0; i < cnt; i++)
	{
		std::wstring src;
		pop(src);
		std::wstring msg;
		pop(msg);

		n4 nItemType = 0;
		pop(nItemType);
		u1 nTamerLevel = 0;
		pop(nTamerLevel);

		assert_cs(g_pGameIF);

		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem(nItemType);
		SAFE_POINTER_CON(pFTItem);
		CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
		SAFE_POINTER_CON(pFTItemInfo);
		int nType = pFTItemInfo->s_nType_L;

		switch (nType)
		{
		case nItem::Cash_Shouter_N: // 일반형 확성기
		case nItem::Cash_Shouter_NT:// 일반형 기간제 확성기
		case nItem::Cash_Shouter_S: // 고급형 확성기
		case nItem::Cash_Shouter_ST:// 고급형 기간제 확성기
		case nItem::GM_Shouter_T:// GM 전용 확성기 
		{
			if (nType == nItem::Cash_Shouter_N || nType == nItem::Cash_Shouter_NT)
				nTamerLevel = 0;

			std::wstring totalMsg;
			DmCS::StringFn::Format(totalMsg, _T("%s     : %s"), src.c_str(), msg.c_str());

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = (NS_CHAT::TYPE)nType;
			CProtocol.m_value = nTamerLevel;
			CProtocol.m_wStr = totalMsg;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		}
		break;
		}
	}
}


void cCliGame::SyncMsgShout(bool bGuild /*= false*/)
{
	uchar cnt = 0;
	pop(cnt);

	for (int i = 0; i < cnt; i++)
	{
		std::wstring src;
		pop(src);
		std::wstring msg;
		pop(msg);

		std::wstring totalMsg;
		DmCS::StringFn::Format(totalMsg, _T("%s : %s"), src.c_str(), msg.c_str());

		assert_cs(g_pGameIF);
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::SHOUT;

#ifdef GUILD_RENEWAL
		if (bGuild)
			CProtocol.m_Type = NS_CHAT::GUILD_SHOUT;
#endif

		CProtocol.m_wStr = totalMsg;

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	}
}

void cCliGame::SyncMsgSystem(void)
{
	uchar cnt;
	char msg[Language::pLength::str + 1];

	pop(cnt);
	// #ifdef CRASHDUMP_0305
	// 	DUMPLOG("02.MsgReceive001 ] cnt : %d", cnt);
	// #endif

	for (int i = 0; i < cnt; i++)
	{
		pop(msg);

#ifdef THAI_CHANNELDOWN		// 태국 채널 다운 시 메세지 출력 관련 코드 수정 lks007	12.11.01
		char TempMsg[Language::pLength::str + 1] = { 0, };

		int nLen = strlen(msg);
		int nNumber = 0;

		for (int i = 0; i < nLen; ++i)
		{
			if (msg[i] >= 0x30 && msg[i] <= 0x39)
			{
				nNumber = i;
				break;
			}
			TempMsg[i] = msg[i];
		}

		if (strncmp(TempMsg, "CHANNEL DOWN", 12) == 0)
		{
			TCHAR TempString[Language::pLength::str + 1] = { 0, };

			_stprintf(TempString, _T("แชนแนลนี้กำลังจะปิดตัวลงในอีก %d วินาที กรุณาย้ายไปที่แชนแนลอื่น มิฉะนั้นตัวละครของคุณจะถูกส่งกลับหมู่บ้าน"), nNumber);

			assert_cs(g_pGameIF);
			cPrintMsg::PrintMsg(12000, TempString);
		}
		else
		{
			assert_cs(g_pGameIF);
			cPrintMsg::PrintMsg(12000, (TCHAR*)LanConvertT(msg));
		}
#else
		assert_cs(g_pGameIF);
		
		cPrintMsg::PrintMsg(12000, (TCHAR*)LanConvertT(msg));
#endif
	}
}



void cCliGame::RecvEquipItem(void)
{
	// 	GS2C_RECV_TAMER_ITEM_EQUIP recv;
	// 	pop(recv.m_nTamerIDX);
	// 	pop(recv.m_nEquipSlot);
	// 	pop(recv.m_nItemType);
	// 	pop(recv.m_nEndTime);
	// 	pop(recv.m_nChangeCostumeNo);
	// 
	// #ifndef VERSION_TH
	// #ifndef VERSION_TW
	// #ifndef VERSION_HK
	// 	u1 nLimited;
	// 	pop(nLimited);//귀속여부 확인용.. 나중에 쓸 일이 있을지 모름 chu8820
	// #endif
	// #endif
	// #endif
	// 
	// 	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_EQUIP, &recv);
	u4 nTamerIDX = 0;
	u1 nEquipSlot = 0;
	u4 nItemType = 0;
	u4 nEndTime = 0;
	u4 nChangeCostumeNo = 0;

	pop(nTamerIDX);
	pop(nEquipSlot);
	pop(nItemType);
	pop(nEndTime);
	pop(nChangeCostumeNo);

#ifndef VERSION_TH
#ifndef VERSION_TW
#ifndef VERSION_HK
	u1 nLimited;
	pop(nLimited);//귀속여부 확인용.. 나중에 쓸 일이 있을지 모름 chu8820
#endif
#endif
#endif

	//assert_cs( nEquipSlot < nTamer::MaxParts );
	CTamer* pTamer = g_pCharMng->GetTamer(nTamerIDX);
	if (pTamer == NULL)
		return;

	CsC_PartObject::sCHANGE_PART_INFO cp;
	cp.s_nFileTableID = nItemType;
	cp.s_nPartIndex = nEquipSlot;
	cp.s_nRemainTime = nEndTime;


	if (nEquipSlot == nsPART::Costume)
	{
		pTamer->SetPcbangCostume(0);
		if (nItemType == PCBANG_COSTUME_ITEM_ID) // pc방 아이템을 입은경우
		{
			cp.s_nFileTableID = nChangeCostumeNo;
			pTamer->SetPcbangCostume(nChangeCostumeNo);
		}
#ifdef GM_CLOCKING
		if (nItemType == CLOCKING_ITEM_ID)
			pTamer->SetClocking(true);
		else
			pTamer->SetClocking(false);
#endif
	}
	else if (nEquipSlot == nsPART::EquipAura)
	{
		if (g_pCharMng->IsTamerUser(nTamerIDX)) // 내 캐릭터 일때만 동작
		{
			SAFE_POINTER_RET(g_pDataMng);
			cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
			SAFE_POINTER_RET(pServerSync);

			if (pServerSync->IsEmptyRefCount(cData_ServerSync::RIDE_OFF))	// 한번만 라이딩 해제기능 보내도록 검사.
			{
				if (pTamer->IsRide() == true) //라이딩 중일 땐 내려야 한다
				{
					pTamer->SetRideEnd();
					g_pDataMng->GetServerSync()->SendServer(cData_ServerSync::RIDE_OFF, 0);

				}
				g_pCharMng->GetTamerUser()->DeletePath();//가던길 멈주자
			}
		}

		pTamer->Scene_Aura(&cp);
	}

	pTamer->ChangePart(&cp);
}


void cCliGame::RecvTing(void)
{
	u4 nMonsterUID = 0;
	pop(nMonsterUID);

	CMonster* pMonster = (CMonster*)g_pMngCollector->GetObject(nMonsterUID);
	if (pMonster == NULL)
		return;

	assert_cs(pMonster->GetLeafRTTI() == RTTI_MONSTER);
	/*NiPoint3 vPos = pMonster->GetPos();
	vPos.z += pMonster->GetToolHeight();*/

	DWORD dwPlag = nsEFFECT::POS_CHARPOS | nsEFFECT::OFFSET_USE;
	NiPoint3 vOffset = NiPoint3(0, 0, pMonster->GetToolHeight() + 30.0f);
	pMonster->GetProp_Effect()->AddEffect("system\\Mark_Surprise.nif", sqrt(pMonster->GetToolWidth() * 0.01f), dwPlag)->SetOffset(vOffset);
	pMonster->PlaySound("System\\Enemy_warning.wav");
}


void cCliGame::SendRideOn(void)
{
	newp(pTamer::RideOn);
	endp(pTamer::RideOn);
	send();
}


void cCliGame::SendRideOff(void)
{
	newp(pTamer::RideOff);
	endp(pTamer::RideOff);
	send();
}


void cCliGame::RecvRideOn(void)
{
	u4 nTamerUID = 0;
	pop(nTamerUID);
	u4 nDigimonUID = 0;
	pop(nDigimonUID);

	CTamer* pTamer = g_pCharMng->GetTamer(cType::GetIDX(nTamerUID));
	CDigimon* pDigimon = g_pCharMng->GetDigimon(cType::GetIDX(nDigimonUID));
	if (pTamer != NULL)
	{
		pTamer->GetProp_Effect()->AddEffect_FT("system\\Riding_Change.nif");
		pTamer->GetCondition()->AddCondition(nSync::Ride);

		if (pTamer->GetLeafRTTI() == RTTI_TAMER_USER)
		{
			g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::RIDE_ON, 0);
		}
	}

	if (pDigimon != NULL)
	{
		pDigimon->GetProp_Effect()->AddEffect_FT("system\\Riding_Change.nif");
		pDigimon->GetCondition()->AddCondition(nSync::Ride);
	}
}


void cCliGame::RecvRideOff(void)
{
	u4 nTamerUID = 0;
	pop(nTamerUID);
	u4 nDigimonUID = 0;
	pop(nDigimonUID);

	CTamer* pTamer = g_pCharMng->GetTamer(cType::GetIDX(nTamerUID));
	CDigimon* pDigimon = g_pCharMng->GetDigimon(cType::GetIDX(nDigimonUID));

	if (pDigimon != NULL)
		pDigimon->GetCondition()->ReleaseCondition(nSync::Ride);

	if (pTamer != NULL)
	{
		pTamer->GetCondition()->ReleaseCondition(nSync::Ride);
		if (pTamer->IsKindOf(RTTI_TAMER_USER))
			g_pDataMng->GetServerSync()->RecvServer(cData_ServerSync::RIDE_OFF, 0);
	}
}

void cCliGame::_SyncInBuffObject(int nObjectCnt)
{
	while (nObjectCnt > 0)
	{
		u4 nObjectUID = 0;		// 버프 대상 타겟 UID - 테이머
		pop(nObjectUID);

		u1 nBuffCnt = 0;		// 버프 개수
		pop(nBuffCnt);

		while (nBuffCnt > 0)
		{
			u2 nBuffCode = 0;		// 버프 코드	
			pop(nBuffCode);

			u2 nBuffLv = 1;			// 버프 레벨
			pop(nBuffLv);

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString( _T("nBuffCode : %d"), nBuffCode );
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("nBuffCode : %d"), nBuffCode);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
			u4 nEndTS = 0;			// 종료 시간			
			pop(nEndTS);

			u4 dwSkillCode = 0;
			pop(dwSkillCode);

			CsC_AvObject* pObject = g_pMngCollector->GetObject(nObjectUID);
			if (!pObject)
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
				//CProtocol.m_wStr = GetVAString( _T("Not Find UID : %d"), nObjectUID );
				DmCS::StringFn::Format(CProtocol.m_wStr, _T("Not Find UID : %d"), nObjectUID);
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
			}

			else
			{
				switch (pObject->GetLeafRTTI())
				{
				case RTTI_TAMER:
				{
					CTamer* pValue = dynamic_cast<CTamer*>(pObject);
					if (pValue)
					{
						pValue->GetBuff()->ReleaseBuff(nBuffCode);	// 루프형 버프 이펙트 때문에
						pValue->GetBuff()->SetBuff(nBuffCode, nObjectUID, nEndTS, nBuffLv, dwSkillCode);
					}
				}
				break;
				case RTTI_DIGIMON:
				{
					CDigimon* pValue = dynamic_cast<CDigimon*>(pObject);
					if (pValue)
					{
						pValue->GetBuff()->ReleaseBuff(nBuffCode);	// 루프형 버프 이펙트 때문에
						pValue->GetBuff()->SetBuff(nBuffCode, nObjectUID, nEndTS, nBuffLv, dwSkillCode);
					}
				}
				break;
				case RTTI_MONSTER:
				{
					CMonster* pValue = dynamic_cast<CMonster*>(pObject);
					if (pValue)
					{
						if (dwSkillCode != 0)
						{
							CsSkill::sINFO* pSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill(dwSkillCode)->GetInfo();
							for (int i = 0; i < SKILL_APPLY_MAX_COUNT; ++i)
							{
								if (pSkillInfo->s_Apply[i].s_nA == APPLY_STUN)
								{
									pValue->SetAniPause(true);
									ST_CHAT_PROTOCOL	CProtocol;
									CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
									CProtocol.m_wStr = L"애니 정지";
									GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
								}
							}
						}
						pValue->GetBuff()->ReleaseBuff(nBuffCode);	// 루프형 버프 이펙트 때문에
						pValue->GetBuff()->SetBuff(nBuffCode, nObjectUID, nEndTS, nBuffLv, dwSkillCode);
					}
				}
				break;

				default:
					ST_CHAT_PROTOCOL	CProtocol;
					CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
					//CProtocol.m_wStr = GetVAString(_T("Not Find RTTI : %d"), pObject->GetLeafRTTI());
					DmCS::StringFn::Format(CProtocol.m_wStr, _T("Not Find RTTI : %d"), pObject->GetLeafRTTI());
					GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
					break;
				}
			}//else

			--nBuffCnt;
		}// buffCnt Loop

		--nObjectCnt;
	}// ObjectCnt Loop
}


void cCliGame::SyncInBuff(void)
{
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = _T("Recv BuffSync");
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
	{	// 테이머 버프 처리
		u2 nTamerCnt = 0;
		pop(nTamerCnt);

		xassert1(nTamerCnt < 1000, "nTamerCnt(%d) is too big", nTamerCnt);

		//CProtocol.m_wStr = GetVAString( _T("nTamerCnt : %d"), nTamerCnt );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("nTamerCnt : %d"), nTamerCnt);
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		_SyncInBuffObject(nTamerCnt);
	}

	{	// 디지몬 버프 처리
		u2 nDigimonCnt = 0;
		pop(nDigimonCnt);

		xassert1(nDigimonCnt < 1000, "nDigimonCnt(%d) is too big", nDigimonCnt);

		//CProtocol.m_wStr = GetVAString( _T("nDigimonCnt : %d"), nDigimonCnt );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("nDigimonCnt : %d"), nDigimonCnt);

		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		_SyncInBuffObject(nDigimonCnt);
	}

	{	// 몬스터 버프 처리
		u2 nMonsterCnt = 0;
		pop(nMonsterCnt);

		xassert1(nMonsterCnt < 1000, "nMonsterCnt(%d) is too big", nMonsterCnt);

		//CProtocol.m_wStr = GetVAString( _T("nMonsterCnt : %d"), nMonsterCnt );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("nMonsterCnt : %d"), nMonsterCnt);
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		_SyncInBuffObject(nMonsterCnt);
	}
}