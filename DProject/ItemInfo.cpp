

#include "stdafx.h"
#include "ItemInfo.h"

int cItemInfo::GetSkillRate() const
{
	return m_nRate;
}

bool cItemInfo::IsRemainSocket() const
{
	for( int i=0; i<nLimit::SocketSlot; ++i )
	{
		if( m_nSockItemType[ i ] == 0 )
			return true;
	}
	return false;
}

void cItemInfo::DecreaseCount( uint nCount )
{
	xassert2( m_nCount >= nCount, "m_nCount(%d)>=nCount(%d)", m_nCount, nCount );
	if( (m_nCount -= nCount ) == 0 )
	{
		Reset();
	}
	xassert1( m_nCount >= 0, "m_nCount : %d", m_nCount );
}

int cItemInfo::GetSocketRate( int nType )
{
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( nType );
	if( pItem == NULL )
		return 0;

	CsItem::sINFO* pFT = pItem->GetInfo();
	if( pFT == NULL )
		return 0;

	cItemInfo item;
	item.m_nType = nType;
	item.m_nRate = pFT->s_btApplyElement;
	item.m_nEndTime = _TIME_TS + pFT->s_nUseTime_Min*60;
	return item.GetSocketRate();
}

int cItemInfo::GetSocketRate()
{
	CsItem* pEquipItem = nsCsFileTable::g_pItemMng->GetItem( GetType() );
	if( pEquipItem == NULL )
		return m_nRate;

	CsItem::sINFO* pFTEquip = pEquipItem->GetInfo();
	if( pFTEquip == NULL )
		return m_nRate;

	int nReturn = pFTEquip->s_btApplyElement;
	if( IsCashEndTime( pFTEquip ) )
		return nReturn;

	if( pFTEquip->s_dwSkill != 0 )
	{
		if( pFTEquip->IsAccessory() ) //악세서리 일 때
		{
 			CsAccessory_Option* pAccessory = nsCsFileTable::g_pItemMng->GetAccessoryOption( pFTEquip->s_dwSkill );
			if( pAccessory )
				return m_nRate;//추가옵션 수 리턴
		}
		else if( pFTEquip->IsAccStone() ) //강화석 일 때
		{
			CsAccessory_Enchant* pAccessory = nsCsFileTable::g_pItemMng->GetAccessoryEnchant( pFTEquip->s_dwSkill );
			if( pAccessory )
			{
				CsAccessory_Enchant::sINFO* pAccInfo = pAccessory->GetInfo();
				if( pAccInfo )
					return pAccInfo->s_nOpt;//강화아이템 종류
			}
		}

		CsSkill::sINFO* pFTEquipSkill = NULL;
		if( pFTEquip->s_nType_L == 180 )
		{
			CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( pFTEquip->s_dwSkill, -1 );
			if( pTamerSkill )
			{
				CsTamerSkill::sTSkill_INFO* pSkillInfo = pTamerSkill->Get_TSkill_Info();
				if( pSkillInfo )
				{
					CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pSkillInfo->s_dwSkillCode );
					if( pSkill )
						pFTEquipSkill = pSkill->GetInfo();
				}
			}			
		}
		else
		{
			CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTEquip->s_dwSkill );
			if( pSkill )
				pFTEquipSkill = pSkill->GetInfo();
		}

		if( pFTEquipSkill )
		{
			for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
			{
				if( pFTEquipSkill->s_Apply[ i ].s_nID == 0 )
					continue;

				if( pFTEquipSkill->s_Apply[ i ].s_nA != 16 )
					continue;

				nReturn += pFTEquipSkill->s_Apply[ i ].s_nB;
			}
		}
	}
	return nReturn;
}

bool cItemInfo::IsCashEndTime( CsItem::sINFO const* pFTItem ) const
{
	// 기간제 아이템의 시간 적용 타입이 능력 효과만 삭제일 경우에만 기간 종료 적용
	switch( pFTItem->s_btUseTimeType )
	{
	case 0:
	case 1:
	case 4:
		return false;

	case 2:
	case 3:
		return ( m_nEndTime == ITEMINFO_INVALIDE_TIME )||( m_nEndTime <= _TIME_TS && m_nEndTime != 0 );
		//캐쉬샵 내의 아이템은 EndTime이 0이어서 툴팁에 속성적용 수치 비정상 출력 -> 0 예외 처리 ( 14.10.22_chu8820 )
	}

	assert_cs( false );
	return false;
}

bool cItemInfo::IsUseItemTime( CsItem::sINFO const* pFTItem )
{
	switch( pFTItem->s_btUseTimeType )
	{
	case 0:
	case 4:
		return false;
	case 1:
	case 2:
	case 3:
		return true;
	}

	assert_cs( false );
	return false;
}

void cItemInfo::SetTradeLimitedTimeEnd()
{
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	m_nRemainTradeLimitTime = ITEMINFO_INVALIDE_TIME;
#endif
}

bool cItemInfo::IsEnableTamerLv( int const& nTLv ) const
{
	if( !IsEnable() )
		return false;

	CsItem const* pItem = nsCsFileTable::g_pItemMng->GetItem( GetType() );
	SAFE_POINTER_RETVAL( pItem, false );

	if( !pItem->IsEnableTamerLv( nTLv ) )
		return false;

// 	for( int i=0; i<nLimit::SocketSlot; ++i )
// 	{
// 		if( 0 == m_nSockItemType[ i ] )
// 			continue;
// 
// 		CsItem const* pSockItem = nsCsFileTable::g_pItemMng->GetItem( m_nSockItemType[ i ] );
// 		SAFE_POINTER_RETVAL( pSockItem, false );
// 
// 		if( !pSockItem->IsEnableTamerLv( nTLv ) )
// 			return false;
// 	}

	return true;
}

bool cItemInfo::IsEnableDigimonLv( int const& nDLv ) const
{
	if( !IsEnable() )
		return false;

	CsItem const* pItem = nsCsFileTable::g_pItemMng->GetItem( GetType() );
	SAFE_POINTER_RETVAL( pItem, false );

	if( !pItem->IsEnableDigimonLv( nDLv ) )
		return false;

// 	for( int i=0; i<nLimit::SocketSlot; ++i )
// 	{
// 		if( 0 == m_nSockItemType[ i ] )
// 			continue;
// 
// 		CsItem const* pSockItem = nsCsFileTable::g_pItemMng->GetItem( m_nSockItemType[ i ] );
// 		SAFE_POINTER_RETVAL( pSockItem, false );
// 
// 		if( !pSockItem->IsEnableDigimonLv( nDLv ) )
// 			return false;
// 	}

	return true;
}

void cItemInfo::GetTamerReqLevel( int& nMinLv, int& nMaxLv ) const
{
	nMinLv = 0;	nMaxLv = 0;

	if( !IsEnable() )
		return;

	CsItem const* pItem = nsCsFileTable::g_pItemMng->GetItem( GetType() );
	SAFE_POINTER_RET( pItem );

	pItem->GetTamerReqLv( nMinLv, nMaxLv );

// 	for( int i=0; i<nLimit::SocketSlot; ++i )
// 	{
// 		if( 0 == m_nSockItemType[ i ] )
// 			continue;
// 
// 		CsItem const* pSockItem =nsCsFileTable::g_pItemMng->GetItem( m_nSockItemType[ i ] );
// 		SAFE_POINTER_CON( pSockItem );
// 
// 		int nSocketMinLv = 0;	int nSocketMaxLv = 0;
// 		pSockItem->GetTamerReqLv( nSocketMinLv, nSocketMaxLv );
// 	
// 		nMinLv = CsMin(nMinLv,nSocketMinLv);
// 		nMaxLv = CsMax(nMaxLv,nSocketMaxLv);
// 	}
}

void cItemInfo::GetDigimonReqLevel( int& nMinLv, int& nMaxLv ) const
{
	nMinLv = 0;	nMaxLv = 0;

	if( !IsEnable() )
		return;

	CsItem const* pItem = nsCsFileTable::g_pItemMng->GetItem( GetType() );
	SAFE_POINTER_RET( pItem );

	pItem->GetDigimonReqLv( nMinLv, nMaxLv );

// 	for( int i=0; i<nLimit::SocketSlot; ++i )
// 	{
// 		if( 0 == m_nSockItemType[ i ] )
// 			continue;
// 
// 		CsItem const* pSockItem =nsCsFileTable::g_pItemMng->GetItem( m_nSockItemType[ i ] );
// 		SAFE_POINTER_CON( pSockItem );
// 
// 		int nSocketMinLv = 0;	int nSocketMaxLv = 0;
// 		pSockItem->GetDigimonReqLv( nSocketMinLv, nSocketMaxLv );
// 
// 		nMinLv = CsMin(nMinLv,nSocketMinLv);
// 		nMaxLv = CsMax(nMaxLv,nSocketMaxLv);
// 	}
}
