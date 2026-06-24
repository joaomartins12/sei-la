
#include "stdafx.h"
#include "Data_Inven.h"

namespace
{
	void CopyItemInfoFields(cItemInfo& dest, const cItemInfo& src)
	{
		dest.m_nAll = src.m_nAll;
		dest.m_nRate = src.m_nRate;
		dest.m_nLevel = src.m_nLevel;
		dest.m_nLimited = src.m_nLimited;

		for (int i = 0; i < nLimit::SocketSlot; ++i)
		{
			dest.m_nSockItemType[i] = src.m_nSockItemType[i];
			dest.m_nSockAppRate[i] = src.m_nSockAppRate[i];
		}

		for (int i = 0; i < nLimit::MAX_ACCESSORY_OPTION; ++i)
		{
			dest.m_nAccOption[i] = src.m_nAccOption[i];
			dest.m_nAccValues[i] = src.m_nAccValues[i];
		}

		dest.m_nEndTime = src.m_nEndTime;
		dest.m_nRemainTradeLimitTime = src.m_nRemainTradeLimitTime;

#ifdef COMPAT_487
		dest.ExtraBytes = src.ExtraBytes;
#endif
	}

	CsItem::sINFO* GetItemInfoSafe(int nItemType)
	{
		if (nItemType <= 0)
			return NULL;

		if (nsCsFileTable::g_pItemMng == NULL)
			return NULL;

		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem(nItemType);
		if (pItem == NULL)
			return NULL;

		return pItem->GetInfo();
	}

	bool IsValidEquipSlot(int nEquipIndex)
	{
		return (nEquipIndex >= 0 && nEquipIndex < nLimit::Equip);
	}

	void SafeChangePart(int nPartIndex, int nFileTableID, int nRemainTime)
	{
		if (g_pCharMng == NULL)
			return;

		CTamerUser* pTamer = g_pCharMng->GetTamerUser();
		if (pTamer == NULL)
			return;

		CsC_PartObject::sCHANGE_PART_INFO cp;
		memset(&cp, 0, sizeof(cp));

		cp.s_nFileTableID = nFileTableID;
		cp.s_nPartIndex = nPartIndex;
		cp.s_nRemainTime = nRemainTime;

		pTamer->ChangePart(&cp);
	}
}

cData_TEquip::cData_TEquip()
{

}

void cData_TEquip::InitNetOff()
{
	/*m_pEquip[ nsPART::Glass ].m_nType = 11001;
	m_pEquip[ nsPART::Glass ].m_nCount = 1;
	m_pEquip[ nsPART::Glass ].m_nRate = 40;
	
	m_pEquip[ nsPART::Coat ].m_nType = 12008;
	m_pEquip[ nsPART::Coat ].m_nCount = 1;
	m_pEquip[ nsPART::Coat ].m_nRate = 40;
	m_pEquip[ nsPART::Coat ].m_nSockItemType[ 0 ] = 1;
	m_pEquip[ nsPART::Coat ].m_nSockAppRate[ 0 ] = 50;
	m_pEquip[ nsPART::Coat ].m_nSockItemType[ 1 ] = 1;
	m_pEquip[ nsPART::Coat ].m_nSockAppRate[ 1 ] = 30;

	m_pEquip[ nsPART::Glove ].m_nType = 13002;
	m_pEquip[ nsPART::Glove ].m_nCount = 1;
	m_pEquip[ nsPART::Glove ].m_nRate = 40;

	m_pEquip[ nsPART::Costume ].m_nType = 17042;
	m_pEquip[ nsPART::Costume ].m_nCount = 1;

	cItemData digi;
	digi.m_nType = 40001;
	digi.m_nCount = 1;
	SetDigiviceItem( &digi );*/
}

void cData_TEquip::Reset()
{
	memset( m_pEquip, 0, sizeof(cItemInfo)*nLimit::Equip);
	m_DigiviceItem.Reset();
}

void cData_TEquip::SetDigiviceItem( cItemData* pDigiviceItem )
{
	if( ( pDigiviceItem == NULL )||( pDigiviceItem->GetType() == 0 ) )
	{
		// 장비 해제
		m_DigiviceItem.Reset();
		cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
		if( pDigivice )
		{
			pDigivice->SetTamerSkillCount( 0 );
			pDigivice->SetChipsetCount( 0 );
		}
	}
	else
	{
		// 장비 장착
		m_DigiviceItem = *pDigiviceItem;
		assert_cs( m_DigiviceItem.IsEnable() == true );

		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_DigiviceItem.m_nType );
		if( pFTItem )
		{
			CsItem::sINFO* pInfo = pFTItem->GetInfo();
			if( pInfo && pInfo->s_nType_L == nItem::Digivice)
			{
				cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
				if( pDigivice )
				{
					pDigivice->SetTamerSkillCount( pInfo->s_nDigiviceSkillSlot );
					pDigivice->SetChipsetCount( pInfo->s_nDigiviceChipsetSlot );
				}
			}
		}
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

void cData_TEquip::SetData(int nIndex, cItemInfo* pItem, bool bCallChangePart)
{
	if (IsValidEquipSlot(nIndex) == false)
		return;

	cItemInfo* pEquip = GetData(nIndex);
	if (pEquip == NULL)
		return;

	if (pItem != NULL && pItem->IsEnable() == true && pItem->GetCount() == 1)
	{
		CopyItemInfoFields(*pEquip, *pItem);

		if (nIndex == nTamer::Costume)
		{
			GS2C_RECV_CHECKTYPE recv;
			recv.nValue1 = pItem->m_nType;

			recv.nType = AchieveContents::CA_Equip_Costume_1;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

			recv.nType = AchieveContents::CA_Equip_Costume_2;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

			recv.nType = AchieveContents::CA_Equip_Costume_3;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

			recv.nType = AchieveContents::CA_Equip_Costume_4;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

			recv.nType = AchieveContents::CA_Equip_Costume_5;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

			recv.nType = AchieveContents::CA_Equip_Costume_6;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);

			recv.nType = AchieveContents::CA_Equip_Costume_7;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv);
		}

		if (bCallChangePart || (net::game == NULL))
		{
			CsItem::sINFO* pInfo = GetItemInfoSafe(pItem->GetType());

			// Se o item não existir no FileTable do client, não chamar ChangePart.
			// Isto evita crash ao equipar itens que o client não conhece.
			if (pInfo != NULL)
				SafeChangePart(nIndex, pItem->GetType(), pItem->m_nEndTime);
		}
	}
	else
	{
		pEquip->Reset();

		if (bCallChangePart || (net::game == NULL))
			SafeChangePart(nIndex, 0, 0);
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::UPDATE_TAMERSTATUS, NULL);
}
cItemInfo* cData_TEquip::GetData( int nIndex )
{ 
	if( nIndex < nLimit::Equip )
		return &m_pEquip[ nIndex ]; 
	return NULL;
}

bool cData_TEquip::IsExistItem( int nIndex )
{ 
	if( nIndex < nLimit::Equip )
		return m_pEquip[ nIndex ].IsEnable(); 
	return false;
}

void cData_TEquip::ToInven(int nEquipIndex, int nInvenIndex)
{
	if (IsValidEquipSlot(nEquipIndex) == false)
		return;

	if (g_pDataMng == NULL || g_pDataMng->GetInven() == NULL)
		return;

	cData_Inven* pInven = g_pDataMng->GetInven();

	cItemInfo* pEquipItem = GetData(nEquipIndex);
	cItemInfo* pInvenItem = pInven->GetData(nInvenIndex);

	if (pEquipItem == NULL || pInvenItem == NULL)
		return;

	CsItem::sINFO* pFTItemInfo = NULL;

	if (pEquipItem->IsEnable() == true)
		pFTItemInfo = GetItemInfoSafe(pEquipItem->GetType());
	else if (pInvenItem->IsEnable() == true)
		pFTItemInfo = GetItemInfoSafe(pInvenItem->GetType());

	// Item desconhecido no client. Não continuar, senão crasha em GetInfo().
	if (pFTItemInfo == NULL)
		return;

	if (pFTItemInfo->s_nUseTimeGroup != 0 && pFTItemInfo->GetCoolTimeSeq() != NULL)
		pFTItemInfo->GetCoolTimeSeq()->Start();

	// Inventário vazio: tira do equipamento e mete no inventário.
	if (pInvenItem->IsEnable() == false)
	{
		if (pEquipItem->IsEnable() == false)
			return;

		CopyItemInfoFields(*pInvenItem, *pEquipItem);
		SetData(nEquipIndex, NULL, false);
	}
	else
	{
		CsItem::sINFO* pInvenFTInfo = GetItemInfoSafe(pInvenItem->GetType());
		if (pInvenFTInfo == NULL)
			return;

		// Validar se o item do inventário pode ir para este slot.
		bool bValidPart = false;

		if (pInvenFTInfo->s_nType_L != nItem::EquipAura)
			bValidPart = (pInvenFTInfo->s_nType_L - nItem::Head == nEquipIndex);
		else
			bValidPart = ((pInvenFTInfo->s_nType_L - nItem::Head) == nEquipIndex);

		if (bValidPart == false)
			return;

		cItemInfo Temp;
		Temp.Reset();

		CopyItemInfoFields(Temp, *pInvenItem);

		if (pEquipItem->IsEnable() == true)
			CopyItemInfoFields(*pInvenItem, *pEquipItem);
		else
			pInvenItem->Reset();

		SetImpute(&Temp);
		SetData(nEquipIndex, &Temp, false);
	}

#ifdef UI_INVENTORY_RENEWAL
	std::pair< int, bool > pairInfo(nInvenIndex, false);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo);
#endif
}


void cData_TEquip::ToInven_Digivice(int nInvenIndex)
{
	if (g_pDataMng == NULL || g_pDataMng->GetInven() == NULL)
		return;

	cData_Inven* pInven = g_pDataMng->GetInven();

	cItemInfo* pDigiviceItem = GetDigiviceItem();
	cItemInfo* pInvenItem = pInven->GetData(nInvenIndex);

	if (pDigiviceItem == NULL || pInvenItem == NULL)
		return;

	CsItem::sINFO* pFTItemInfo = NULL;

	if (pDigiviceItem->IsEnable() == true)
		pFTItemInfo = GetItemInfoSafe(pDigiviceItem->GetType());
	else if (pInvenItem->IsEnable() == true)
		pFTItemInfo = GetItemInfoSafe(pInvenItem->GetType());

	if (pFTItemInfo == NULL)
		return;

	if (pFTItemInfo->s_nUseTimeGroup != 0 && pFTItemInfo->GetCoolTimeSeq() != NULL)
		pFTItemInfo->GetCoolTimeSeq()->Start();

	if (pInvenItem->IsEnable() == false)
	{
		if (pDigiviceItem->IsEnable() == false)
			return;

		CopyItemInfoFields(*pInvenItem, *pDigiviceItem);
		SetDigiviceItem(NULL);
	}
	else
	{
		CsItem::sINFO* pInvenFTInfo = GetItemInfoSafe(pInvenItem->GetType());
		if (pInvenFTInfo == NULL)
			return;

		if (pInvenFTInfo->s_nType_L != nItem::Digivice)
			return;

		cItemInfo Temp;
		Temp.Reset();

		CopyItemInfoFields(Temp, *pInvenItem);

		if (pDigiviceItem->IsEnable() == true)
			CopyItemInfoFields(*pInvenItem, *pDigiviceItem);
		else
			pInvenItem->Reset();

		SetDigiviceItem(&Temp);
	}

#ifdef UI_INVENTORY_RENEWAL
	std::pair< int, bool > pairInfo(nInvenIndex, false);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo);
#endif
}

int cData_TEquip::GetCount_Item_ID( int nItemID )
{
	int nReturn = 0;
	for( uint i=0; i<nLimit::Equip; ++i )
	{
		if( m_pEquip[ i ].IsEnable() == false )
			continue;

		if( m_pEquip[ i ].m_nType == nItemID )
			nReturn += m_pEquip[ i ].GetCount();
	}
	return nReturn;
}

int cData_TEquip::GetEquipedCount()
{
	int nEquipedCount = 0;
	for( uint i=0; i<nLimit::Equip; ++i )
	{
		if( m_pEquip[ i ].IsEnable() )
			++nEquipedCount;
	}

	return nEquipedCount;
}





void cData_TEquip::SetImpute(cItemInfo* pItem)
{
	SAFE_POINTER_RET(pItem);
		//착귀템귀속여부변경chu8820
	if(pItem->m_nEndTime!= 0 && pItem->m_nLimited == 0)//기간제 아이템인데 귀속이 안되있으면
		pItem->m_nLimited = 1; //귀속되도록 바꿔주자
}
//void cData_TEquip::CheckBuff()
//{
//	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
//	cCondition* pCondition = pTamer->GetCondition();
//
//	pCondition->ReleaseBuff( cCondition::BK_FRIENDSHIP );
//	pCondition->ReleaseBuff( cCondition::BK_CASHEXP_EQUIP );
//	pCondition->ReleaseBuff( cCondition::BK_CASHEXP_BUSTER );
//
//	int nFriendShip = 0;
//	uint nFriendShipTime = UINT_MAX;
//	uint nFriendShipTotalTime = UINT_MAX;
//	int nExpEquip = 0;	
//	int nExpBuster = 0;
//	uint nExpBusterTime = UINT_MAX;
//	uint nExpBusterTotalTime = UINT_MAX;
//	for( int i=0; i<nLimit::Equip; ++i )
//	{
//		if( GetData( i )->IsEnable() == false )
//			continue;
//
//		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( GetData( i )->GetType() )->GetInfo();
//		if( pFTItem->s_dwSkill == 0 )
//			continue;
//
//		// 기간제 체크
//		uint nItemEndTime = GetData( i )->m_nEndTime;
//		if( pFTItem->s_nUseTime_Min )
//		{
//			// 기간제 끝났다면
//			if( ( nItemEndTime == ITEMINFO_INVALIDE_TIME )||( nItemEndTime <= _TIME_TS ) )
//				continue;
//		}
//
//		CsSkill::sINFO* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTItem->s_dwSkill )->GetInfo();
//		for( int skill=0; skill<SKILL_APPLY_MAX_COUNT; ++skill ) 
//		{
//			if( pFTSkill->s_Apply[ skill ].s_nID == 0 )
//				continue;
//
//			switch( pFTSkill->s_Apply[ skill ].s_nA )
//			{
//			case 14:
//				{	
//					if( pFTItem->IsPCBangType() )
//						continue; 
//
//					nFriendShip += pFTSkill->s_Apply[ skill ].s_nB;
//					if( nFriendShipTime > nItemEndTime )
//						nFriendShipTime = nItemEndTime;
//					if( nFriendShipTotalTime > pFTItem->s_nUseTime_Min )
//						nFriendShipTotalTime = pFTItem->s_nUseTime_Min;
//				}				
//				break;
//			case 15:
//				if( i == nTamer::Ring )
//				{
//					nExpBuster += pFTSkill->s_Apply[ skill ].s_nB;
//					if( nExpBusterTime > nItemEndTime )
//						nExpBusterTime = nItemEndTime;
//					if( nExpBusterTotalTime > pFTItem->s_nUseTime_Min )
//						nExpBusterTotalTime = pFTItem->s_nUseTime_Min;
//				}				
//				else
//				{
//					if( pFTItem->IsPCBangType() )
//						continue;
//
//					nExpEquip += pFTSkill->s_Apply[ skill ].s_nB;
//				}
//				break;
//			}
//		}
//	}	
//
//	if( nFriendShip )
//	{
//		pCondition->SetBuff( cCondition::BK_FRIENDSHIP, nFriendShipTime, nFriendShipTotalTime*60, nFriendShip, 0 );
//	}
//	if( nExpEquip )
//	{
//		pCondition->SetBuff( cCondition::BK_CASHEXP_EQUIP, 0, 0, nExpEquip, 0 );
//	}
//	if( nExpBuster )
//	{
//		pCondition->SetBuff( cCondition::BK_CASHEXP_BUSTER, nExpBusterTime, nExpBusterTotalTime*60, nExpBuster, 0 );
//	}
//}
