
#include "stdafx.h"
#include "Data_Digivice.h"

namespace
{
void CopyItemInfoFields( cItemInfo& dest, const cItemInfo& src )
{
	dest.m_nAll = src.m_nAll;
	dest.m_nRate = src.m_nRate;
	dest.m_nLevel = src.m_nLevel;
	dest.m_nLimited = src.m_nLimited;
	for( int i = 0; i < nLimit::SocketSlot; ++i )
	{
		dest.m_nSockItemType[i] = src.m_nSockItemType[i];
		dest.m_nSockAppRate[i] = src.m_nSockAppRate[i];
	}
	for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION; ++i )
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
}

cData_Digivice::cData_Digivice()
{

}

void cData_Digivice::InitNetOff()
{

}

void cData_Digivice::Reset()
{
	m_nChipsetCount = 0;
	m_nTamerSkillCount = 0;

	memset( m_Chipset, 0, sizeof(cItemInfo)*nLimit::Chipset );
	memset( m_TamerSkill, 0, sizeof(cItemInfo)*nLimit::SkillSlot );
}

void cData_Digivice::SetChipsetCount(int nCount)
{
	m_nChipsetCount = nCount;
}

cItemInfo*	cData_Digivice::GetChipset_TypeL( int nType_L )
{
	for( int i=0; i < nLimit::Chipset; i++)
	{	
		cItemInfo *pItem = &m_Chipset[ i ];
		if( pItem->IsEnable()){
			CsItem::sINFO* pInfo = NULL;
			pInfo = nsCsFileTable::g_pItemMng->GetItem( pItem->GetType() )->GetInfo();
			if( pInfo->s_nType_L == nType_L )
				return pItem;
		}
	}
	return NULL;
}


bool cData_Digivice::DoYouHaveJointProgressChipset( )
{
	CsItem::sINFO* pInfo = NULL;

	for( uint i=0; i<nLimit::Chipset; ++i )
	{
		if( m_Chipset[ i ].IsEnable() == false )
			continue;		 

		pInfo = nsCsFileTable::g_pItemMng->GetItem( m_Chipset[ i ].m_nType )->GetInfo();
		if( 0 != pInfo->s_nType_S )	// data테이블에 정의된 52"01"~"02" 값으로 비교..
			return true;
	}
	return false;
}

int cData_Digivice::GetCount_Item_TypeL( int nType_L )
{
	CsItem::sINFO* pInfo = NULL;
	int nReturn = 0;
	for( uint i=0; i<nLimit::Chipset; ++i )
	{
		if( m_Chipset[ i ].IsEnable() == false )
			continue;

		pInfo = nsCsFileTable::g_pItemMng->GetItem( m_Chipset[ i ].m_nType )->GetInfo();
		if( pInfo->s_nType_L == nType_L )
			nReturn += m_Chipset[ i ].GetCount();
	}
	return nReturn;
}

// 01, 02 값으로 디지바이스에 아이템있으면 인덱스를 리턴
int	cData_Digivice::GetChipsetIndex_TypeLT( int nType_L, int nType_T )
{
	CsItem::sINFO* pInfo = NULL;
	for( int i=0; i < nLimit::Chipset; i++)
	{	
		cItemInfo *pItem = &m_Chipset[ i ];
		if( pItem->IsEnable()){			

			pInfo = nsCsFileTable::g_pItemMng->GetItem( m_Chipset[ i ].m_nType )->GetInfo();
			DWORD t = pInfo->s_nType_L * 100 + pInfo->s_nType_S;
			if( t == nType_T)			
				return i;			
		}
	}
	return INVALIDE_DIGIVICE_INDEX;
}

bool cData_Digivice::ItemRemove_NoServer( int nIndex, bool bCalQuest )
{
	if(INVALIDE_DIGIVICE_INDEX == nIndex)
		return false;

	cItemInfo *pItem = &m_Chipset[ nIndex ];
	if(NULL == pItem)
		return false;

	pItem->Reset();
	// 퀘스트 체크
	if( bCalQuest == true )
	{
		g_pDataMng->GetQuest()->CalProcess();
		g_pDataMng->GetQuest()->RevQuestCheck();
	}	
	cData_QuickSlot::CheckItemCount_AllUser();
	return true;
}

void cData_Digivice::ChipsetToInven( int nSrcIndex, int nInvenIndex )
{
	// 비여있는 인벤토리인가
	cData_Inven* pInven = g_pDataMng->GetInven();
	if( pInven->GetData( nInvenIndex )->IsEnable() == false )
	{
		assert_cs( GetChipset( nSrcIndex )->IsEnable() );

		// 인벤토리에 넣어주고
		CopyItemInfoFields( *pInven->GetData( nInvenIndex ), *GetChipset( nSrcIndex ) );

		// 칩셋에서 빼주자
		m_Chipset[ nSrcIndex ].Reset();
	}
	// 차 있는 인벤 인가
	else
	{
		// 임시보관
		cItemInfo Temp;
		CopyItemInfoFields( Temp, *pInven->GetData( nInvenIndex ) );
		// 인벤토리에 넣어주고
		CopyItemInfoFields( *pInven->GetData( nInvenIndex ), *GetChipset( nSrcIndex ) );		

		// 귀속 처리
		if( nsCsFileTable::g_pItemMng->GetItem( Temp.m_nType )->GetInfo()->s_nBelonging == 1 )
			Temp.m_nLimited = 1;

		// 칩셋에 넣어주자
		CopyItemInfoFields( m_Chipset[ nSrcIndex ], Temp );
	}

#ifdef UI_INVENTORY_RENEWAL
	std::pair< int, bool > pairInfo( nInvenIndex, false );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo );
#endif

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

void cData_Digivice::ChipsetToChipset( int nSrcIndex, int nDestIndex )
{
	cItemInfo Temp;
	CopyItemInfoFields( Temp, *GetChipset( nSrcIndex ) );
	CopyItemInfoFields( *GetChipset( nSrcIndex ), *GetChipset( nDestIndex ) );
	CopyItemInfoFields( *GetChipset( nDestIndex ), Temp );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

int cData_Digivice::GetEmptyChipsetSlot()
{
	for( int i=0; i<m_nChipsetCount; ++i )
	{
		if( m_Chipset[ i ].IsEnable() == false )
			return i;
	}
	return -1;
}
