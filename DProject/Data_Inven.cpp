
#include "stdafx.h"
#include "Data_Inven.h"

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

void SwapItemInfoFields( cItemInfo& left, cItemInfo& right )
{
	cItemInfo backup;
	CopyItemInfoFields( backup, left );
	CopyItemInfoFields( left, right );
	CopyItemInfoFields( right, backup );
}
}


#include "_Interface/Adapt/AdaptWareHouse.h"

sMONEY::sMONEY() : s_nTera(0),s_nMega(0),s_nBit(0)
				  ,s_nProperty_A(0),s_nProperty_B(0),s_nProperty_C(0),s_nProperty_D(0)
{

}

sMONEY::~sMONEY()
{ 
	memset( this, 0, sizeof(sMONEY) );
}

sMONEY::sMONEY(u8 nMoney): s_nTera(0),s_nMega(0),s_nBit(0)
,s_nProperty_A(0),s_nProperty_B(0),s_nProperty_C(0),s_nProperty_D(0)
{
	*this = nMoney;
}

sMONEY::sMONEY( int nTera, int nMega, int nBit ): s_nTera(nTera),s_nMega(nMega),s_nBit(nBit)
,s_nProperty_A(0),s_nProperty_B(0),s_nProperty_C(0),s_nProperty_D(0)
{ 
}

sMONEY::sMONEY( int nPrPA, int nPrPB, int nPrPC, int nPrPD ): s_nTera(0),s_nMega(0),s_nBit(0)
,s_nProperty_A(nPrPA),s_nProperty_B(nPrPB),s_nProperty_C(nPrPC),s_nProperty_D(nPrPD)
{ 
}

sMONEY::sMONEY( sMONEY* src )
{ 
	*this = *src; 
}

void sMONEY::SetInvalideData()
{ 
	s_nTera = s_nMega = s_nBit = -1; 
}

void sMONEY::Init()
{ 
	s_nTera = s_nMega = s_nBit = 0; 
}

void sMONEY::IncreaseBit( int nBit )
{ 
	assert_cs(nBit<1000); 
	if( (s_nBit += nBit)>999 )
	{ 
		IncreaseMega( 1 ); 
		s_nBit -= 1000; 
	} 
	assert_cs( s_nBit >= 0 );  
}

void sMONEY::IncreaseMega( int nMega )
{ 
	assert_cs(nMega<1000); 
	if( (s_nMega += nMega)>999 )
	{ 
		IncreaseTera( 1 ); 
		s_nMega -= 1000; 
	} 
	assert_cs( s_nMega >= 0 );  
}

void sMONEY::IncreaseTera( int nTera )
{ 
	s_nTera += nTera; 
}

void sMONEY::DecreaseBit( int nBit )
{ 
	assert_cs(nBit>=0);	
	assert_cs(nBit<1000); 
	if( (s_nBit -= nBit)<0 )
	{ 
		DecreaseMega( 1 ); 
		s_nBit += 1000; 
	} 
	assert_cs( s_nBit >= 0 );  
}

void sMONEY::DecreaseMega( int nMega )
{ 
	assert_cs(nMega>=0); 
	assert_cs(nMega<1000); 
	if( (s_nMega -= nMega)<0 )
	{ 
		DecreaseTera( 1 ); 
		s_nMega += 1000; 
	} 
	assert_cs( s_nMega >= 0 );  
}

void sMONEY::DecreaseTera( int nTera )
{ 
	assert_cs(nTera>=0); 
	s_nTera -= nTera; 
	assert_cs( s_nTera >=0 ); 
}

void sMONEY::SetInvalideDataProperty()
{ 
	s_nProperty_A = s_nProperty_B = s_nProperty_C = s_nProperty_D = -1; 
}

void sMONEY::InitProperty()
{ 
	s_nProperty_A = s_nProperty_B = s_nProperty_C = s_nProperty_D = 0; 
}

void sMONEY::IncreaseProperty_A( int nProperty_A )
{ 
	s_nProperty_A += nProperty_A; 
}

void sMONEY::IncreaseProperty_B( int nProperty_B )
{ 
	s_nProperty_B += nProperty_B; 
}

void sMONEY::IncreaseProperty_C( int nProperty_C )
{ 
	s_nProperty_C += nProperty_C; 
}

void sMONEY::IncreaseProperty_D( int nProperty_D )
{ 
	s_nProperty_D += nProperty_D; 
}

void sMONEY::DecreaseProperty_A( int nProperty_A )
{ 
	assert_cs(nProperty_A>=0); s_nProperty_A -= nProperty_A; assert_cs( nProperty_A >=0 ); 
}

void sMONEY::DecreaseProperty_B( int nProperty_B )
{ 
	assert_cs(nProperty_B>=0); 
	nProperty_B -= nProperty_B; 
	assert_cs( nProperty_B >=0 ); 
}

void sMONEY::DecreaseProperty_C( int nProperty_C )
{ 
	assert_cs(nProperty_C>=0); 
	nProperty_C -= nProperty_C; 
	assert_cs( nProperty_C >=0 ); 
}

void sMONEY::DecreaseProperty_D( int nProperty_D )
{ 
	assert_cs(nProperty_D>=0); 
	nProperty_D -= nProperty_D; 
	assert_cs( nProperty_D >=0 ); 
}

int	 sMONEY::GetProperty_A()
{ 
	return s_nProperty_A; 
}

int	 sMONEY::GetProperty_B()
{ 
	return s_nProperty_B; 
}

int	 sMONEY::GetProperty_C()
{ 
	return s_nProperty_C; 
}

int	 sMONEY::GetProperty_D()
{ 
	return s_nProperty_D; 
}

void sMONEY::SetProperty_A(int nMoney)
{ 
	s_nProperty_A = nMoney; 
}

void sMONEY::SetProperty_B(int nMoney)
{ 
	s_nProperty_B = nMoney; 
}

void sMONEY::SetProperty_C(int nMoney)
{ 
	s_nProperty_C = nMoney; 
}

void sMONEY::SetProperty_D(int nMoney)
{ 
	s_nProperty_D = nMoney; 
}

int	 sMONEY::GetProperty()
{ 
	return s_nProperty_A + s_nProperty_B + s_nProperty_C + s_nProperty_D; 
}

int	 sMONEY::GetTera()
{ 
	return s_nTera; 
}

int	 sMONEY::GetMega()
{ 
	return s_nMega; 
}

int	 sMONEY::GetBit()
{ 
	return s_nBit; 
}

u8	 sMONEY::GetInt()
{ 
	return (u8)(s_nTera)*1000000 + s_nMega*1000 + s_nBit; 
}

//////////////////////////////////////////////////////////////////////////

cData_Inven::cData_Inven() : m_nInvenSlotCount(0), m_nInvenGroupCount(0), m_nReserveCapsuleInvenIndex(0)
{
	ReleaseCapsuleItem();

	m_pWarehouseAdapt = new AdaptWareHouse;

	if( m_pWarehouseAdapt )
	{
		m_pWarehouseAdapt->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_WAREHOUSE ) );
	}
}



void cData_Inven::InitNetOff()
{
	m_Money = 5000000000;
	//m_nInvenSlotCount = IF_INVENTORY_COUNT;
	//m_nInvenSlotCount = IF_INVENTORY_COUNT*3;
	m_nInvenSlotCount = 21;
	m_nInvenGroupCount = 1;

	cProfile x("./dmo.ini");

	char key[ 32 ];
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		sprintf_s( key, 32, "%d_IDX", i );
		m_pInven[ i ].m_nType = x.GetInt( "ITEM", key );
		sprintf_s( key, 32, "%d_CNT", i );
		m_pInven[ i ].m_nCount = x.GetInt( "ITEM", key );

		if( m_pInven[ i ].m_nType != 0 )
		{
			m_pInven[ i ].m_nRate = 50;
			m_pInven[ i ].m_nEndTime = 600 + _TIME_TS;
		}		
	}	
}

void cData_Inven::Reset()
{
	m_Money = 0;
	memset( m_pInven, 0, sizeof(cItemInfo)*nLimit::Inven );	

	m_listLock_ItemID.clear();
	m_listLock_ItemType.clear();
}

void cData_Inven::ResetAllItem()
{
	memset( m_pInven, 0, sizeof(cItemInfo)*nLimit::Inven );
	m_listLock_ItemID.clear();
	m_listLock_ItemType.clear();
}

void cData_Inven::LoadApply()
{
	assert_cs( m_nInvenSlotCount > 0 );
#ifndef UI_INVENTORY_RENEWAL
	m_nInvenGroupCount = ( m_nInvenSlotCount + IF_INVENTORY_COUNT - 1 )/IF_INVENTORY_COUNT;	
#endif
}

void cData_Inven::Backup_Save()
{
	memcpy( m_pInvenBackup, m_pInven, sizeof( cItemInfo )*nLimit::Inven );
}

void cData_Inven::Backup_Load()
{
	memcpy( m_pInven, m_pInvenBackup, sizeof( cItemInfo )*nLimit::Inven );
}

u4	 cData_Inven::GetTypeToIndex(nItem::eType eType)
{

	for(int i=0; i<nLimit::Inven; ++i)
	{
		//if( m_pInven[i] != NULL)
		{
			CsItem*  pkItem = nsCsFileTable::g_pItemMng->GetItem( m_pInven[i].m_nType );
			if(pkItem != NULL)
			{
				CsItem::sINFO* pFTItemInfo = pkItem->GetInfo();
				if(pFTItemInfo != NULL)
				{
					if( pFTItemInfo->s_nType_L == eType)
					{
						return i;
					}
				}
			}
		}
	}
	return	INVALIDE_INVEN_INDEX;
}
u4	 cData_Inven::GetTypeToIndex(int Type)
{
	for(int i=0; i<nLimit::Inven; ++i)
	{
		if( m_pInven[i].m_nType == Type )
			return i;
	}
	return	INVALIDE_INVEN_INDEX;
}
u4	 cData_Inven::GetTypeToConut(int Type)
{
	u4	uCnt = 0;
	for(int i=0; i<nLimit::Inven; ++i)
	{
		if( m_pInven[i].m_nType == Type )
			uCnt += m_pInven[i].GetCount();
	}
	return uCnt;
}
cItemInfo*	cData_Inven::GetTypeToItem(int Type)
{
	for(int i=0; i<nLimit::Inven; ++i)
	{
		if( m_pInven[i].m_nType == Type )
			return &m_pInven[i];
	}
	return NULL;
}

void cData_Inven::SetMoney( n8 nMoney, bool bCheckAchieve )
{
	m_Money = nMoney;
	
	if( bCheckAchieve )
	{
		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_Money;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_HAVE_MONEY, NULL );
}

void cData_Inven::SetInvenSlotCount(int nCount)
{
	m_nInvenSlotCount = nCount;
#ifdef UI_INVENTORY_RENEWAL
	m_nInvenGroupCount = 1;
#endif
}

sMONEY cData_Inven::IncreaseMoney( u8 nValue, bool bCheckAchieve )
{
	m_Money += nValue;

	if( bCheckAchieve )
	{
		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_Money;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_HAVE_MONEY, NULL );
	return m_Money;
}

sMONEY cData_Inven::IncreaseMoney( sMONEY nValue, bool bCheckAchieve )
{
	m_Money += nValue;
	if( bCheckAchieve )
	{
		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_Money;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_HAVE_MONEY, NULL );
	return m_Money;
}

sMONEY cData_Inven::DecreaseMoney( u8 nValue )
{ 
	assert_cs( m_Money >= nValue ); 
	
	m_Money -= nValue; 

	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_HAVE_MONEY, NULL );
	return m_Money;
}

sMONEY cData_Inven::DecreaseMoney( sMONEY nValue )
{ 
	assert_cs( m_Money >= nValue ); 
	
	m_Money -= nValue; 

	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_HAVE_MONEY, NULL );

	return m_Money; 
}

cItemInfo* cData_Inven::GetData( int nIndex )
{ 
	assert_cs( nIndex < nLimit::Inven );
	if( nIndex < 0 || nIndex >= nLimit::Inven )
		return NULL;
	return &m_pInven[ nIndex ]; 
}

CsItem::sINFO* cData_Inven::GetFTData( int nIndex )
{ 
	cItemInfo* pItemInfo = GetData( nIndex );
	SAFE_POINTER_RETVAL( pItemInfo, NULL );

	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
	SAFE_POINTER_RETVAL( pItem, NULL );

	return pItem->GetInfo();
}

bool cData_Inven::IsExistItem( int nIndex )
{ 
	assert_cs( nIndex < nLimit::Inven );
	if( nIndex < 0 || nIndex >= nLimit::Inven )
		return false;
	return m_pInven[ nIndex ].IsEnable(); 
}

bool cData_Inven::IsSlot()
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			return true;
	}
	return false;
}


//===============================================================================================================================
//
//	ItemID
//
//===============================================================================================================================
bool cData_Inven::IsSlot( int nItemID, int nCount )
{
	uint nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo()->s_nOverlap;

	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		// 비여 있는게 있다면 리턴 트루
		if( !m_pInven[ i ].IsEnable() )
			return true;

		// 같은 타입이 존재 한다면
		if( m_pInven[ i ].m_nType != nItemID )
			continue;

		// 7일 거래 제한이 걸려 있는 아이템인 경우
		if( cData_Inven::IsTradeLimitedItem( &m_pInven[ i ] ) )
			continue;

		// 최대 갯수인 경우 패스
		if( m_pInven[ i ].GetCount() >= nMaxCnt )
			continue;

		if( m_pInven[ i ].GetCount() + nCount <= nMaxCnt )// 합쳐져도 될때는 리턴 트루
			return true;
		else
			nCount -= ( nMaxCnt - m_pInven[ i ].GetCount() );// 합쳐 지면 오바할때는 해당 갯수 弧斂?계속 계산하자
	}
	return false;

// 	uint nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo()->s_nOverlap;
// 
// 	for( uint i=0; i<m_nInvenSlotCount; ++i )
// 	{
// 		// 비여 있는게 있다면 리턴 트루
// 		if( m_pInven[ i ].IsEnable() == false )
// 			return true;
// 
// 		// 같은 타입이 존재 한다면
// 		if( m_pInven[ i ].m_nType == nItemID )
// 		{
// 			// 꽉차 있지 않는 같은 타입이라면
// 			if( m_pInven[ i ].GetCount() < nMaxCnt )
// 			{				
// 				if( m_pInven[ i ].GetCount() + nCount <= nMaxCnt )
// 				{
// 					// 합쳐져도 될때는 리턴 트루
// 					return true;
// 				}				
// 				else
// 				{
// 					// 합쳐 지면 오바할때는 해당 갯수 弧斂?계속 계산하자
// 					nCount -= ( nMaxCnt - m_pInven[ i ].GetCount() );
// 				}
// 			}
// 		}
// 	}
// 	return false;
}

int cData_Inven::FindEmptySlot( int nSearchIndex )
{
	if( nSearchIndex + 1 >= nLimit::Inven )
		return INVALIDE_INVEN_INDEX;

	for( uint i=nSearchIndex+1; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			return i;
	}
	return INVALIDE_INVEN_INDEX;
}


int cData_Inven::FindEmptySlot( int nSearchIndex, int nExceptSlot /*=-1*/ )
{
	if( nSearchIndex + 1 >= nLimit::Inven )
		return INVALIDE_INVEN_INDEX;

	for( uint i=nSearchIndex+1; i<m_nInvenSlotCount; ++i )
	{
		if( i == nExceptSlot )
			continue;

		if( m_pInven[ i ].IsEnable() == false )
			return i;
	}
	return INVALIDE_INVEN_INDEX;
}

int cData_Inven::GetEmptySlotCount()
{
	int nReturn = 0;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			++nReturn;
	}
	return nReturn;
}

int cData_Inven::GetFirstSlot_Item_ID( int nItemID )
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( m_pInven[ i ].m_nType == nItemID )
			return i;
	}
	return INVALIDE_INVEN_INDEX;
}

int cData_Inven::GetSlot_Item_ID_ReverseToSlot( int nSlot, int nItemID )
{
	if( nSlot < 0 || nSlot >= nLimit::Inven )
		return INVALIDE_INVEN_INDEX;

	for( int i=(int)m_nInvenSlotCount - 1; i>nSlot; --i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( m_pInven[ i ].m_nType == nItemID )
			return i;
	}
	return INVALIDE_INVEN_INDEX;
}

int cData_Inven::GetSlot_Item_ID_Count(uint const& nItemID, uint const& nItemCount)
{
	for( int i = 0; i < m_nInvenSlotCount; ++i )
	{
		if( !m_pInven[ i ].IsEnable() )
			continue;
		if( nItemID != m_pInven[ i ].m_nType )
			continue;
		if( nItemCount > m_pInven[ i ].GetCount() )
			continue;

		return i;
	}

	return INVALIDE_INVEN_INDEX;
}

int	 cData_Inven::GetSlot_Item_ID_ReverseToSlot_G(int nSlot, int nItemID )
{
	if( nSlot < 0 || nSlot >= nLimit::Inven )
		return INVALIDE_INVEN_INDEX;

	for( int i=nSlot ; i>=0; --i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( m_pInven[ i ].m_nType == nItemID )
			return i;
	}
	return INVALIDE_INVEN_INDEX;

}

int cData_Inven::GetFirstSlot_Item_ID_Rate( int nItemID, int nRate, bool bPassLimited )
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( m_pInven[ i ].m_nType != nItemID )
			continue;
		if( m_pInven[ i ].m_nRate != nRate )
			continue;

		if( bPassLimited == true )
		{
			if( m_pInven[ i ].m_nLimited != 0 )
				continue;
		}

		return i;
	}
	return INVALIDE_INVEN_INDEX;
}

int cData_Inven::GetFirstSlot_Item_TypeLS(int nTypeL, int nTypeS )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, INVALIDE_INVEN_INDEX );
	for( unsigned int i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		unsigned int nItemType = m_pInven[ i ].m_nType;

		CsItem* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( nItemType );
		SAFE_POINTER_CON( pItemInfo );

		CsItem::sINFO* pInfo = pItemInfo->GetInfo();
		SAFE_POINTER_CON( pInfo );

		if( ( pInfo->s_nType_L == nTypeL )&&( pInfo->s_nType_S == nTypeS ) )
			return i;
	}
	return INVALIDE_INVEN_INDEX;
}

int cData_Inven::GetFirstSlot_ItemID( int nItemID1, int nItemID2 )
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( ( m_pInven[ i ].m_nType == nItemID1 )||( m_pInven[ i ].m_nType == nItemID2 ) )
			return i;
	}
	return INVALIDE_INVEN_INDEX;
}

DWORD cData_Inven::GetCount_Item_ID( uint const& dwItemID, bool const& bCheckLimitedTime ) const
{
	DWORD nReturn = 0;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		
		if( m_pInven[ i ].m_nType != dwItemID )
			continue;

		if( bCheckLimitedTime )
		{
			if( IsTradeLimitedItem( &m_pInven[ i ] ) )
				continue;
		}

		nReturn += m_pInven[ i ].GetCount();
	}
	return nReturn;
}

void cData_Inven::GetItemDatas_ItemID( uint const& dwItemID, DWORD const& nTotalCount, std::map<int,int>& items, bool const& bCheckLimitedTime )
{
	DWORD dwRemainCount = nTotalCount;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( m_pInven[ i ].m_nType != dwItemID )
			continue;

		if( bCheckLimitedTime )
		{
			if( IsTradeLimitedItem( &m_pInven[ i ] ) )
				continue;
		}
		uint nCount = m_pInven[ i ].GetCount();

		if( dwRemainCount > nCount )
		{
			items.insert( std::make_pair( i, nCount ) );
			dwRemainCount -= nCount;
		}
		else
		{
			items.insert( std::make_pair( i, dwRemainCount ) );
			dwRemainCount = 0;
		}
		
		if( 0 == dwRemainCount )
			break;
	}
}

void cData_Inven::GetItemDatas_ItemType( uint const& dwItemType, DWORD const& nTotalCount, std::map<int,cItemData>& items, bool const& bCheckLimitedTime )
{
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	int nTypeL = dwItemType / 100;
	int nTypeS = dwItemType % 100;
	DWORD dwRemainCount = nTotalCount;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( bCheckLimitedTime )
		{
			if( IsTradeLimitedItem( &m_pInven[ i ] ) )
				continue;
		}

		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType );
		SAFE_POINTER_CON( pItem );
		CsItem::sINFO* pInfo = pItem->GetInfo();
		SAFE_POINTER_CON( pInfo );

		if( pInfo->s_nType_L != nTypeL || pInfo->s_nType_S != nTypeS )
			continue;

		uint nCount = m_pInven[ i ].GetCount();
		if( dwRemainCount > nCount )
		{
			items.insert( std::make_pair( i, m_pInven[ i ] ) );
			dwRemainCount -= nCount;
		}
		else
		{
			cItemData addItem = m_pInven[ i ];
			addItem.SetCount( dwRemainCount );
			items.insert( std::make_pair( i, addItem ) );
			dwRemainCount = 0;
		}

		if( 0 == dwRemainCount )
			break;		
	}
}

int cData_Inven::GetCount_Item_TypeL( int nType_L )
{
	CsItem::sINFO* pInfo = NULL;
	int nReturn = 0;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType ) == NULL )
			continue;

		pInfo = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType )->GetInfo();
		SAFE_POINTER_CON( pInfo );
		if( pInfo->s_nType_L != nType_L )
			continue;
		nReturn += m_pInven[ i ].GetCount();
	}
	return nReturn;
}

int cData_Inven::GetCount_Item_TypeLS(int nType_L, int nType_S )
{
	CsItem::sINFO* pInfo = NULL;
	int nReturn = 0;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType ) == NULL )
		{
			//DUMPLOG( " C004 - %s, %d, %d ", g_pCharMng->GetTamerUser()->GetName(), m_nInvenSlotCount, i );
			continue;
		}

		pInfo = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType )->GetInfo();
		if( ( pInfo->s_nType_L == nType_L )&&( pInfo->s_nType_S == nType_S ) )
			nReturn += m_pInven[ i ].GetCount();
	}
	return nReturn;
}

int cData_Inven::GetCount_Item_ID_Rate( int nItemID, int nRate )
{
	int nReturn = 0;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( m_pInven[ i ].m_nRate != nRate )
			continue;

		if( m_pInven[ i ].m_nType != nItemID )
			continue;

		nReturn += m_pInven[ i ].GetCount();
	}
	return nReturn;
}

int cData_Inven::GetCount_Item_EnableCrop( int nItemID )
{
	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo();
	int nReturn = 0;
	cItemInfo* pItem = NULL;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		pItem = GetData( i );

		// 아이템이 있다면
		if( pItem->IsEnable() )
		{
			// 같은 타입이라면
			if( pItem->GetType() != nItemID )
				continue;

			if( IsTradeLimitedItem( pItem ) )
				continue;

			// 남은 겹침 갯수 만큼 추가
			assert_cs( pFTItem->s_nOverlap >= pItem->GetCount() );
			nReturn += ( pFTItem->s_nOverlap - pItem->GetCount() );
		}
		// 비여 있다면
		else
		{
			nReturn += pFTItem->s_nOverlap;
		}
	}
	return nReturn;
}

// int cData_Inven::GetSlotNumber_Crop( const cItemData* pItemData )
// {
// 	SAFE_POINTER_RETVAL( pItemData, -1 );
// 	if( cData_Inven::IsTradeLimitedItem(pItemData) )
// 		return FindEmptySlot(-1);
// 
// 	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, -1 );
// 
// 	CsItem* pTbItem = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() );
// 	SAFE_POINTER_RETVAL( pTbItem, -1 );
// 	CsItem::sINFO* pFTItem = pTbItem->GetInfo();
// 	SAFE_POINTER_RETVAL( pFTItem, -1 );
// 
// 	if( pFTItem->s_nOverlap == 1 )	// 수량처리
// 		return FindEmptySlot(-1);
// 
// 	for( uint i=0; i<m_nInvenSlotCount; ++i )
// 	{
// 		cItemInfo* pItem = GetData( i );
// 		SAFE_POINTER_CON( pItem );
// 
// 		if( !pItem->IsEnable() )
// 			continue;
// 		
// 		// 같은 타입이라면
// 		if( pItem->GetType() != pItemData->GetType() )
// 			continue;
// 
// 		if( IsTradeLimitedItem( pItem ) )
// 			continue;
// 
// 		// 갯수가 맥스인가		
// 		if( pItem->m_nCount == pFTItem->s_nOverlap )
// 			continue;
// 
// 		if( pItem->m_nCount + pItemData->GetCount() > pFTItem->s_nOverlap )
// 			break;
// 
// 		return i;
// 	}
// 	
// 	return FindEmptySlot(-1);
// }

int cData_Inven::GetCount_Item_EnableCrop( const cItemData* pItemData )
{
	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() )->GetInfo();

	int nReturn = 0;
	cItemInfo* pItem = NULL;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( (int)pItemData->GetCount() <= nReturn )
			return nReturn;
		pItem = GetData( i );
		// 아이템이 있다면
		if( pItem->IsEnable() == true )
		{
			// 같은 타입이라면
			if( pItem->GetType() != pItemData->GetType() )
				continue;

			if( IsTradeLimitedItem( pItem ) )
				continue;

			// 남은 겹침 갯수 만큼 추가
			nReturn += ( pFTItem->s_nOverlap - pItem->GetCount() );
		}
		// 비여 있다면
		else
		{
			nReturn = pItemData->GetCount();
			break;
		}
	}
	return nReturn;

}

void cData_Inven::ItemLock_ItemID( int nItemID )
{
	_InsertLock_ItemID( nItemID );

	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( m_pInven[ i ].m_nType == nItemID )
			g_pDataMng->ItemLock( TO_INVEN_SID( i ) );
	}
}


void cData_Inven::ItemLock_ItemID_ELSE( int nItemID )
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( m_pInven[ i ].m_nType == nItemID )
			continue;

		_InsertLock_ItemID(m_pInven[ i ].m_nType);
		g_pDataMng->ItemLock( TO_INVEN_SID( i ) );
	}
}

void cData_Inven::ItemUnlock_AllItem()
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		_ReleaseLock_ItemID( m_pInven[ i ].m_nType );
		g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
	}
}

void cData_Inven::ItemUnlock_ItemID( int nItemID )
{
	_ReleaseLock_ItemID( nItemID );

	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if( m_pInven[ i ].m_nType == nItemID )
			g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
	}
}

void cData_Inven::Itemlock_InvenSlot( int nIndex )
{
	if( nIndex < 0 || nIndex >= nLimit::Inven )
		return;

	if( !m_pInven[nIndex].IsEnable() )
		return;

	_InsertLock_ItemID( m_pInven[nIndex].m_nType );
	g_pDataMng->ItemLock( nIndex );
}

void cData_Inven::ItemUnlock_InvenSlot( int nIndex )
{
	if( nIndex < 0 || nIndex >= nLimit::Inven )
		return;

	if( !m_pInven[nIndex].IsEnable() )
		return;

	_ReleaseLock_ItemID( m_pInven[nIndex].m_nType );
	g_pDataMng->ItemUnlock( nIndex );
}

void cData_Inven::SetCoolTime( int const& nItemID )
{
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );

	CsItem* pItemTable = nsCsFileTable::g_pItemMng->GetItem( nItemID );
	SAFE_POINTER_RET( pItemTable );
	CsItem::sINFO* pFTInfo = pItemTable->GetInfo();
	SAFE_POINTER_RET( pFTInfo );
	if( pFTInfo->s_nUseTimeGroup == 0 )
		return;

	CsCoolTimeSeq* pCool = pFTInfo->GetCoolTimeSeq();
	SAFE_POINTER_RET( pCool );
	pCool->Start();// 쿨타임 적용
}

bool cData_Inven::DecreaseItem( int const& nInvenPos, DWORD const& nItemID, int const& nCount, bool bCoolDown )
{
	cItemInfo* pItemInfo = GetData( nInvenPos );
	SAFE_POINTER_RETVAL( pItemInfo, false );

	DWORD dwTargetIDX = pItemInfo->GetType();
	if( dwTargetIDX != nItemID )
		return false;

	if( bCoolDown == true )
		SetCoolTime( nItemID );

	pItemInfo->DecreaseCount( nCount );

	// 아이템 슬롯이 잠겨 있고 아이템이 삭제 瑛?때 unlock 시킴
	if( !pItemInfo->IsEnable() )
	{
		if( g_pDataMng->IsItemLock( TO_INVEN_SID( nInvenPos ) ) )
			g_pDataMng->ItemUnlock( TO_INVEN_SID( nInvenPos ) );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_ITEM_COUNT, &dwTargetIDX );	// 아이템 삭제시
	return true;
}

// 인벤토리 슬롯의 아이템을 갱신 시킨다.
bool cData_Inven::UpdateInvenItem( int const& nInvenPos, cItemData const& cItemData, bool bCoolDown )
{
	cItemInfo* pItemInfo = GetData( nInvenPos );
	SAFE_POINTER_RETVAL( pItemInfo, false );

	DWORD dwItemCode = cItemData.m_nType;
	if( 0 == dwItemCode )// 인벤토리 슬롯 위치의 아이템이 삭제 榮?
	{
		dwItemCode = pItemInfo->GetType();
		pItemInfo->Clear();
	}
	else
	{
		if( pItemInfo->GetType() != dwItemCode )
			return false;

		*pItemInfo = cItemData;
	}

	// 아이템 슬롯이 잠겨 있고 아이템이 삭제 瑛?때 unlock 시킴
	if( !pItemInfo->IsEnable() )
	{
		if( g_pDataMng->IsItemLock( TO_INVEN_SID( nInvenPos ) ) )
			g_pDataMng->ItemUnlock( TO_INVEN_SID( nInvenPos ) );
	}

	if( bCoolDown == true )
		SetCoolTime( dwItemCode );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_ITEM_COUNT, &dwItemCode );	// 아이템 삭제시
	return true;
}

void cData_Inven::DecreaseItem( int const& nItemID, int nCount, bool bCoolDown, bool bPassLimited )
{
	//=============================================================================================================
	//	쿨타임
	//=============================================================================================================
	if( bCoolDown == true )
		SetCoolTime( nItemID );

	//=============================================================================================================
	//	갯수 감소
	//=============================================================================================================	
	assert_cs( GetCount_Item_ID( nItemID ) >= nCount );
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( bPassLimited == true )
		{
			if( m_pInven[ i ].m_nLimited != 0 )
				continue;
		}

		if( m_pInven[ i ].m_nType == nItemID )
		{
			if( (int)m_pInven[ i ].GetCount() < nCount )
			{
				nCount -= m_pInven[ i ].GetCount();
				assert_cs( nCount > 0 );

				m_pInven[ i ].Reset();

				if( g_pDataMng->IsItemLock( TO_INVEN_SID( i ) ) == true )
					g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
			}
			else if( (int)m_pInven[ i ].GetCount() == nCount )
			{
				m_pInven[ i ].Reset();
				if( g_pDataMng->IsItemLock( TO_INVEN_SID( i ) ) == true )
					g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );

				return;
			}
			else
			{
				// 아이템 갯수가 더 많다면
				m_pInven[ i ].DecreaseCount( nCount );
				return;
			}
		}	
	}
}

void cData_Inven::DecreaseItem( int const& nItemID, int nRate, int nCount, bool bCoolDown, bool bPassLimited )
{
	//=============================================================================================================
	//	쿨타임
	//=============================================================================================================
	if( bCoolDown == true )
		SetCoolTime( nItemID );

	//=============================================================================================================
	//	갯수 감소
	//=============================================================================================================
	assert_cs( GetCount_Item_ID( nItemID ) >= nCount );
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( bPassLimited == true )
		{
			if( m_pInven[ i ].m_nLimited != 0 )
				continue;
		}

		if( m_pInven[ i ].m_nType != nItemID )
			continue;

		if( m_pInven[ i ].m_nRate != nRate )
			continue;

		if( (int)m_pInven[ i ].GetCount() < nCount )
		{
			nCount -= m_pInven[ i ].GetCount();
			assert_cs( nCount > 0 );

			m_pInven[ i ].Reset();

			if( g_pDataMng->IsItemLock( TO_INVEN_SID( i ) ) == true )
				g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
		}
		else if( (int)m_pInven[ i ].GetCount() == nCount )
		{
			m_pInven[ i ].Reset();
			if( g_pDataMng->IsItemLock( TO_INVEN_SID( i ) ) == true )
				g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );

			return;
		}
		else
		{
			// 아이템 갯수가 더 많다면
			m_pInven[ i ].DecreaseCount( nCount );
			return;
		}
	}
}


void cData_Inven::DecreaseItem_TypeLS( int nTypeL, int nTypeS, int nCount, bool bCoolDown, bool bPassLimited )
{
	//=============================================================================================================
	//	쿨타임
	//=============================================================================================================
	if( bCoolDown == true )
	{
		int ItemTypeS = GetFirstSlot_Item_TypeLS( nTypeL, nTypeS );
		if(ItemTypeS == INVALIDE_INVEN_INDEX)
			return;

		SAFE_POINTER_RET(GetData( ItemTypeS ));
		int nCoolDownType = GetData( ItemTypeS )->GetType();

		SetCoolTime( nCoolDownType );
	}

	//=============================================================================================================
	//	갯수 감소
	//=============================================================================================================
	assert_cs( GetCount_Item_TypeLS( nTypeL, nTypeS ) >= nCount );
	CsItem::sINFO* pInfo = NULL;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( bPassLimited == true )
		{
			if( m_pInven[ i ].m_nLimited != 0 )
				continue;
		}

		if( nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType ) == NULL )
			continue;

		pInfo = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType )->GetInfo();
		SAFE_POINTER_CON( pInfo );
		if( ( pInfo->s_nType_L != nTypeL )||( pInfo->s_nType_S != nTypeS ) )
			continue;

		if( (int)m_pInven[ i ].GetCount() < nCount )
		{
			nCount -= m_pInven[ i ].GetCount();
			assert_cs( nCount > 0 );

			m_pInven[ i ].Reset();

			if( g_pDataMng->IsItemLock( TO_INVEN_SID( i ) ) == true )
				g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
		}
		else if( (int)m_pInven[ i ].GetCount() == nCount )
		{
			m_pInven[ i ].Reset();
			if( g_pDataMng->IsItemLock( TO_INVEN_SID( i ) ) == true )
				g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );

			return;
		}
		else
		{
			// 아이템 갯수가 더 많다면
			m_pInven[ i ].DecreaseCount( nCount );
			return;
		}
	}
}

void cData_Inven::_InsertLock_ItemID( int nItemID )
{
	std::list< int >::iterator it = m_listLock_ItemID.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemID.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nItemID )
			return;
	}
	m_listLock_ItemID.push_back( nItemID );
}

void cData_Inven::_ReleaseLock_ItemID( int nItemID )
{
	std::list< int >::iterator it = m_listLock_ItemID.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemID.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nItemID )
		{
			m_listLock_ItemID.erase( it );
			return;
		}
	}
// 	assert_cs( false );
}

bool cData_Inven::_IsLock_ItemID( int nItemID )
{
	std::list< int >::iterator it = m_listLock_ItemID.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemID.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nItemID )
			return true;
	}
	return false;
}

//===============================================================================================================================
//
//	ItemType
//
//===============================================================================================================================

int cData_Inven::GetFirstSlot_ItemType( int nType_L, int nType_S )
{
	CsItem::sINFO* pFTItem = NULL;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType ) == NULL )
			continue;

		pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType )->GetInfo();
		SAFE_POINTER_RETVAL( pFTItem, INVALIDE_INVEN_INDEX );
		if( ( pFTItem->s_nType_L == nType_L )&&( pFTItem->s_nType_S == nType_S ) )
			return i;
	}
	return INVALIDE_INVEN_INDEX;
}


void cData_Inven::ItemLock_ItemType( int nType_L, int nType_S )
{
	_InsertLock_ItemType( nType_L, nType_S );

	CsItem::sINFO* pFTItem = NULL;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		// Same guard pattern as GetCount_Item_TypeL — outer GetItem() can return NULL
		// when an inventory slot's m_nType isn't in ItemList.bin (server/client bin
		// mismatch, slot left stale post-consume).  Original code only checked pFTItem
		// (the inner GetInfo() result), so the GetItem()->GetInfo() chain AV'd before
		// SAFE_POINTER_CON could trip.
		if( nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType ) == NULL )
			continue;

		pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType )->GetInfo();
		SAFE_POINTER_CON( pFTItem );
		if( ( pFTItem->s_nType_L == nType_L )&&( pFTItem->s_nType_S == nType_S ) )
			g_pDataMng->ItemLock( TO_INVEN_SID( i ) );
	}
}

void cData_Inven::ItemUnlock_ItemType( int nType_L, int nType_S )
{
	_ReleaseLock_ItemType( nType_L, nType_S );

	CsItem::sINFO* pFTItem = NULL;
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;

		if( nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType ) == NULL )
			continue;

		pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType )->GetInfo();
		SAFE_POINTER_CON( pFTItem );
		if( ( pFTItem->s_nType_L == nType_L )&&( pFTItem->s_nType_S == nType_S ) )
			g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
	}
}


void cData_Inven::_InsertLock_ItemType( int nType_L, int nType_S )
{
	int nTotalType = nType_L*100 + nType_S;
	std::list< int >::iterator it = m_listLock_ItemType.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemType.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nTotalType )
			return;
	}
	m_listLock_ItemType.push_back( nTotalType );
}

void cData_Inven::_ReleaseLock_ItemType( int nType_L, int nType_S )
{
	int nTotalType = nType_L*100 + nType_S;
	std::list< int >::iterator it = m_listLock_ItemType.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemType.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nTotalType )
		{
			m_listLock_ItemType.erase( it );
			return;
		}
	}
}

bool cData_Inven::_IsLock_ItemType( int nType_L, int nType_S )
{
	int nTotalType = nType_L*100 + nType_S;
	std::list< int >::iterator it = m_listLock_ItemType.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemType.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nTotalType )
			return true;
	}
	return false;
}

//===============================================================================================
//
//	아이템
//
//===============================================================================================
bool cData_Inven::ItemMove( int nSrcIndex, int nDestIndex, int nSrcCount )
{
	if( nSrcIndex < 0 || nSrcIndex >= nLimit::Inven )
		return false;

	if( nDestIndex < 0 || nDestIndex >= nLimit::Inven )
		return false;

	assert_cs( m_pInven[ nSrcIndex ].IsEnable() == true );
	if( m_pInven[ nSrcIndex ].GetCount() == nSrcCount )
		_ItemMoveAllCount( nSrcIndex, nDestIndex );
	else
		return _ItemMoveCount( nSrcIndex, nDestIndex, nSrcCount );
	return true;
}

void cData_Inven::_ItemMoveAllCount( int nSrcIndex, int nDestIndex )
{
	// 대상쪽에 아이템이 존재 한다면
	if( GetData( nDestIndex )->IsEnable() )
	{
		cItemInfo* pSrc = GetData( nSrcIndex );
		cItemInfo* pDest = GetData( nDestIndex );

#ifdef ITEM_USE_TIME_PASS
		CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();

		if(pFTInfo->s_bUseTimeType == true)
		{
			if(pSrc->m_nEndTime != pDest->m_nEndTime)	//2017-04-25-nova 종료시간이 다르면 자리만 교체i
			{
				SwapItemInfoFields( m_pInven[ nSrcIndex ], m_pInven[ nDestIndex ] );

				return;
			}
		}		
#endif

		// 같은 타입이라면 합쳐 주자
		if( (pSrc->GetType() == pDest->GetType()) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			&&	(!cData_Inven::IsTradeLimitedItem( pSrc ) && !cData_Inven::IsTradeLimitedItem( pDest ) )
#endif
			)
		{
			int nMaxCount = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo()->s_nOverlap;

			// 이미 맥스 카운트 라면 교체
			if( pDest->GetCount() == nMaxCount )
			{
				SwapItemInfoFields( m_pInven[ nSrcIndex ], m_pInven[ nDestIndex ] );
			}			
			// 합쳐서 맥스치를 넘어 선다면 맥스치까지만 올려 주고 남은거 자리 복귀
			else if( pSrc->GetCount() + pDest->GetCount() > (uint)nMaxCount )
			{
				assert_cs( pSrc->m_nLimited == pDest->m_nLimited );
				assert_cs( pSrc->m_nRate == pDest->m_nRate );

				assert_cs( pSrc->GetCount() > ( nMaxCount - pDest->GetCount() ) );
				pSrc->m_nCount -= ( nMaxCount - pDest->GetCount() );
				pDest->m_nCount = nMaxCount;
			}
			// 맥스치를 안넘는다면 온전히 합체
			else
			{
				assert_cs( pSrc->m_nLimited == pDest->m_nLimited );
				assert_cs( pSrc->m_nRate == pDest->m_nRate );

				pDest->m_nCount += pSrc->GetCount();
				pSrc->Reset();
			}
		}
		// 다른 타입이라면 교체
		else
		{
			SwapItemInfoFields( *pSrc, *pDest );
		}
	}
	// 대상쪽이 비여 있다면
	else
	{
		// 집어 넣어주고 비워주자
		CopyItemInfoFields( m_pInven[ nDestIndex ], m_pInven[ nSrcIndex ] );
		m_pInven[ nSrcIndex ].Reset();
	}
}

bool cData_Inven::_ItemMoveCount( int nSrcIndex, int nDestIndex, int nSrcCount )
{
	if( nSrcIndex < 0 || nSrcIndex >= nLimit::Inven )
		return false;

	if( nDestIndex < 0 || nDestIndex >= nLimit::Inven )
		return false;

	assert_cs( nSrcCount > 0 );
	// 대상쪽에 아이템이 존재 한다면
	if( GetData( nDestIndex )->IsEnable() )
	{
		cItemInfo* pSrc = GetData( nSrcIndex );
		cItemInfo* pDest = GetData( nDestIndex );

		assert_cs( pSrc->GetType() == pDest->GetType() );
		assert_cs( pSrc->m_nLimited == pDest->m_nLimited );
		assert_cs( pSrc->m_nRate == pDest->m_nRate );
		uint nMaxCount = (uint)nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo()->s_nOverlap;

		// 합쳐서 맥스치를 넘어 선다면 실패
		assert_cs( pDest->GetCount() + nSrcCount <= nMaxCount );

		pDest->m_nCount += nSrcCount;
		pSrc->DecreaseCount( nSrcCount );
	}
	// 대상쪽이 비여 있다면
	else
	{
		// 집어 넣어주고 비워주자
		CopyItemInfoFields( m_pInven[ nDestIndex ], m_pInven[ nSrcIndex ] );
		m_pInven[ nDestIndex ].m_nCount = nSrcCount;
		m_pInven[ nSrcIndex ].DecreaseCount( nSrcCount );
	}
	return true;
}

#ifdef NEW_SHARESTASH
int cData_Inven::CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount )
{
	cItemInfo* pSrc = NULL;
	cItemInfo* pDest = NULL;

	switch( TO_CONSTANT( nDestSrvID ) )
	{
	case SERVER_DATA_INVEN_CONSTANT:
		{			
			pDest = GetData( TO_ID(nDestSrvID) );			
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		{			//기륜 창고
			pDest = m_pWarehouseAdapt->GetData( TO_ID(nDestSrvID) ) ;//GetData( TO_ID(nDestSrvID) ); 
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_SHARESTASH_CONSTANT:
		{			
			pDest = m_pWarehouseAdapt->GetShareData( TO_ID(nDestSrvID) ) ;//GetData( TO_ID(nDestSrvID) ); 
			assert_cs( pDest );			
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		{
			pDest = g_pDataMng->GetInvenCross()->GetData( TO_ID(nDestSrvID) );			
			assert_cs( pDest );
		}
		break;
#endif
	default:
		assert_cs( false );
	}

	pSrc = GetData( TO_ID(nSrcSrvID) );
	assert_cs( pSrc );

	assert_cs( pSrc->IsEnable() == true );
	// 대상쪽에 아이템이 존재 한다면
	if( pDest->IsEnable() )
	{
		// 다른타입이라면 나누지 않겠다
		if( pSrc->GetType() != pDest->GetType() )
			return 0;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		// 캐쉬 아이템 거래 제한 시간이 남아있으면 분해 금지
		if( cData_Inven::IsTradeLimitedItem( pSrc ) || cData_Inven::IsTradeLimitedItem( pDest ) )
			return 0;
#endif
		uint nMaxCount = (uint)nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo()->s_nOverlap;

		// 이미 맥스 카운트 라면 실패
		if( pDest->GetCount() == nMaxCount )
			return 0;

		// 합쳐서 맥스치를 넘어 선다면 맥스치까지만 올려 주고 남은거 자리 복귀
		if( nSrcCount + pDest->GetCount() > nMaxCount )
		{
			assert_cs( (uint)nSrcCount > ( nMaxCount - pDest->GetCount() ) );
			return nMaxCount - pDest->GetCount();
		}
		// 맥스치를 안넘는다면 온전히 합체
		return nSrcCount;
	}
	// 대상쪽이 비여 있다면
	return nSrcCount;
}

#else

int cData_Inven::CalItemSplit( int nSrcIndex, int nDestIndex, int nSrcCount )
{
	assert_cs( GetData( nSrcIndex )->IsEnable() == true );
	// 대상쪽에 아이템이 존재 한다면
	if( GetData( nDestIndex )->IsEnable() )
	{
		cItemInfo* pSrc = GetData( nSrcIndex );
		cItemInfo* pDest = GetData( nDestIndex );

		// 다른타입이라면 나누지 않겠다
		if( pSrc->GetType() != pDest->GetType() )
			return 0;

		uint nMaxCount = (uint)nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo()->s_nOverlap;

		// 이미 맥스 카운트 라면 실패
		if( pDest->GetCount() == nMaxCount )
			return 0;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		// 캐쉬 아이템 거래 제한 시간이 남아있으면 분해 금지
		if( cData_Inven::IsTradeLimitedItem( pSrc ) || cData_Inven::IsTradeLimitedItem( pDest ) )
			return 0;
#endif

		// 합쳐서 맥스치를 넘어 선다면 맥스치까지만 올려 주고 남은거 자리 복귀
		if( nSrcCount + pDest->GetCount() > nMaxCount )
		{
			assert_cs( (uint)nSrcCount > ( nMaxCount - pDest->GetCount() ) );
			return nMaxCount - pDest->GetCount();
		}
		// 맥스치를 안넘는다면 온전히 합체
		return nSrcCount;
	}
	// 대상쪽이 비여 있다면
	return nSrcCount;
}

#endif
//===============================================================================================
//
//	아이템 습득
//
//===============================================================================================
int cData_Inven::ItemCrop( int nItemID, int nCount, int nTradeLimitTime )
{
	cItemData item;
	item.Set( nItemID, 1 );
	item.m_nRemainTradeLimitTime = nTradeLimitTime;

	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo();
	SAFE_POINTER_RETVAL(pFTInfo, 0);
	if( pFTInfo->s_btApplyRateMin == pFTInfo->s_btApplyRateMax )
		item.m_nRate = pFTInfo->s_btApplyRateMax;

	if( ( ( pFTInfo->s_nType_L ) >= nItem::EItem_All )&&
		( ( pFTInfo->s_nType_L ) <= nItem::EItem_Glass ) )
	{
		item.m_nRate = 0;
	}
	if( pFTInfo->IsAccessory() )
		item.m_nRate = 0;
	return ItemCrop( &item, nCount );
}

// 캐쉬 아이템 거래 제한 기간이 남아 있는경우 아이템 겹치기가 안됨.
bool cData_Inven::IsTradeLimitedItem( cItemData const* pData )
{
	if( pData->m_nRemainTradeLimitTime == 0 || pData->m_nRemainTradeLimitTime == ITEMINFO_INVALIDE_TIME )
		return false;

	if((pData->m_nRemainTradeLimitTime + TRADE_LIMITE_TIME_DALAY) - _TIME_TS > 0)
		return true;
	return false;
}

int cData_Inven::ItemCrop( cItemData* pItemData, int nCount )
{
	SAFE_POINTER_RETVAL(pItemData, 0);

	assert_cs( pItemData->GetType() != 0 );
	assert_cs( pItemData->GetCount() != 0 );

	CsItem*	pItem = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() );
	SAFE_POINTER_RETVAL(pItem, 0);

	CsItem::sINFO* pFTInfo = pItem->GetInfo();
	SAFE_POINTER_RETVAL(pFTInfo, 0);

#ifdef ITEM_USE_TIME_PASS
	if(pFTInfo->s_bUseTimeType)
	{
		pItemData->m_nEndTime = 0xFFFFFFF0;
	}
#endif

	if( g_pGameIF->GetTrade()==NULL && /*g_pGameIF->GetPersonStore()==NULL &&*/ g_pGameIF->GetUnionStore()==NULL )//기간제아이템 거래 시 시간 그대로 넘기기 chu8820
	{
#ifdef ITEM_USE_TIME_PASS
		if(pItemData->m_nEndTime != 0xFFFFFFF0)
			pItemData->m_nEndTime = pFTInfo->s_nUseTime_Min*60;
#else
		pItemData->m_nEndTime = pFTInfo->s_nUseTime_Min*60;
#endif		
	}
	
	switch( pFTInfo->s_btUseTimeType )// 사용시간적용타입
	{
	case 1:
	case 2:
	case 3:
		if( g_pGameIF->GetTrade()==NULL && /*g_pGameIF->GetPersonStore()==NULL &&*/ g_pGameIF->GetUnionStore()==NULL )
		{
#ifdef ITEM_USE_TIME_PASS
			if(pItemData->m_nEndTime != 0xFFFFFFF0)
			{
				if( pItemData->m_nEndTime != 0 )
				{
					pItemData->m_nEndTime += _TIME_TS;	
				}
			}
#else
			if( pItemData->m_nEndTime != 0 )
			{
				pItemData->m_nEndTime += _TIME_TS;	
			}
#endif
		}
		break;
	case 0:
	case 4:
		if( g_pGameIF->GetTrade()==NULL && /*g_pGameIF->GetPersonStore()==NULL &&*/ g_pGameIF->GetUnionStore()==NULL )//기간제아이템 거래 시 시간 그대로 넘기기 chu8820
		{
			pItemData->m_nEndTime = ITEMINFO_INVALIDE_TIME;
		}
		break;
	default:
		assert_cs( false );
	}
	int nRemain = 0;
	if( 0 == nCount )
		nRemain = pItemData->GetCount();
	else
		nRemain = nCount;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	if( cData_Inven::IsTradeLimitedItem(pItemData) )
	{
		// 캐쉬 아이템 거래 제한 기간이 남아 있는경우 아이템 겹치기가 안됨.
	}
	else
#endif
	if( pFTInfo->s_nOverlap > 1 )	// 수량처리
	{
		// 기존 수량에 추가
		for( uint i=0; i<m_nInvenSlotCount; ++i )
		{
			assert_cs( nRemain >= 0 );

			// 수량이 0이 되면 중지
			if( nRemain == 0 )
				break;

			// 같은 타입인가
			if( m_pInven[ i ].m_nType != pItemData->GetType() )
				continue;

			// 갯수가 맥스인가		
			if( m_pInven[ i ].m_nCount == pFTInfo->s_nOverlap )
				continue;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			// 겹처질 아이템이 거래 제한이 없고
			// 겹처질 대상의 아이템이 거래 제한인 경우
			// 합치기 안됨.
			if( cData_Inven::IsTradeLimitedItem(&m_pInven[ i ]) )
				continue;
#endif

			assert_cs( m_pInven[ i ].m_nCount < pFTInfo->s_nOverlap );
			// 채우고 넘치는가
			if( m_pInven[ i ].m_nCount + nRemain > pFTInfo->s_nOverlap )
			{
				nRemain -= ( pFTInfo->s_nOverlap - m_pInven[ i ].m_nCount );
				m_pInven[ i ].m_nCount = pFTInfo->s_nOverlap;
				std::pair< int, bool > pairInfo( i, true );
				GAME_EVENT_ST.OnEvent( EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo );
			}
			// 넘치지 않거나 딱 맞으면
			else
			{
				m_pInven[ i ].m_nCount += nRemain;
				nRemain = 0;
				std::pair< int, bool > pairInfo( i, true );
				GAME_EVENT_ST.OnEvent( EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo );
			}
		}
	}
	
	assert_cs( nRemain >= 0 );
	// 합치고 나서 남아 있는 아이템이 존재 하는가
	while( nRemain > 0 )
	{
		int nEmptySlot = FindEmptySlot( -1 );
		if( nEmptySlot == cData_Inven::INVALIDE_INVEN_INDEX )
			break;

		m_pInven[ nEmptySlot ] = *pItemData;
		std::pair< int, bool > pairInfo( nEmptySlot, true );
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo );

		if( nRemain <= pFTInfo->s_nOverlap )
		{
			m_pInven[ nEmptySlot ].m_nCount = nRemain;
			nRemain = 0;
		}
		else
		{
			m_pInven[ nEmptySlot ].m_nCount = pFTInfo->s_nOverlap;
			nRemain -= pFTInfo->s_nOverlap;
		}


		// 획득시 귀속이라면
		//assert_cs( m_pInven[ nEmptySlot ].m_nLimited == 0 );
		if( pFTInfo->s_nBelonging == 2 )
			m_pInven[ nEmptySlot ].m_nLimited = 1;			

		// 이미 아이템 ID 闡려像繭窄?闡?
		if( _IsLock_ItemID( m_pInven[ nEmptySlot ].m_nType ) == true )
		{
			g_pDataMng->ItemLock( TO_INVEN_SID( nEmptySlot ) );
		}

		// 이미 아이템 Type 闡려像繭窄?闡?
		if( _IsLock_ItemType( pFTInfo->s_nType_L, pFTInfo->s_nType_S ) == true )
		{

			g_pDataMng->ItemLock( TO_INVEN_SID( nEmptySlot ) );
		}
	}
	
	// 퀘스트 체크
	// 진행 체크
	g_pDataMng->GetQuest()->CalProcess();
	// 받을수 있는지 체크
	g_pDataMng->GetQuest()->RevQuestCheck();
	// 퀵슬롯 체크
	cData_QuickSlot::CheckItemCount_AllUser();

	DWORD dwItemIdx = pItemData->GetType();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_ITEM_COUNT, &dwItemIdx );	// 획득시

	return nRemain;
}
#ifdef REPURCHASE
int cData_Inven::ItemCrop_Repurchase( cItemData* pItemData )
{
	SAFE_POINTER_RETVAL(pItemData, 0);

	assert_cs( pItemData->GetType() != 0 );
	assert_cs( pItemData->GetCount() != 0 );

	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() ), 0);

	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() )->GetInfo();

	SAFE_POINTER_RETVAL(pFTInfo, 0);

	int nRemain = pItemData->GetCount();

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	if( cData_Inven::IsTradeLimitedItem(pItemData) )
	{
		// 캐쉬 아이템 거래 제한 기간이 남아 있는경우 아이템 겹치기가 안됨.
	}
	else
#endif
	if( pFTInfo->s_nOverlap > 1 )	// 수량처리
	{
		// 기존 수량에 추가
		for( uint i=0; i<m_nInvenSlotCount; ++i )
		{
			assert_cs( nRemain >= 0 );

			// 수량이 0이 되면 중지
			if( nRemain == 0 )
				break;

			// 같은 타입인가
			if( m_pInven[ i ].m_nType != pItemData->GetType() )
				continue;

			// 갯수가 맥스인가		
			if( m_pInven[ i ].m_nCount == pFTInfo->s_nOverlap )
				continue;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			// 겹처질 아이템이 거래 제한이 없고
			// 겹처질 대상의 아이템이 거래 제한인 경우
			// 합치기 안됨.
			if( cData_Inven::IsTradeLimitedItem(&m_pInven[ i ]) )
				continue;
#endif
			assert_cs( m_pInven[ i ].m_nCount < pFTInfo->s_nOverlap );
			// 채우고 넘치는가
			if( m_pInven[ i ].m_nCount + nRemain > pFTInfo->s_nOverlap )
			{
				nRemain -= ( pFTInfo->s_nOverlap - m_pInven[ i ].m_nCount );
				m_pInven[ i ].m_nCount = pFTInfo->s_nOverlap;
			}
			// 넘치지 않거나 딱 맞으면
			else
			{
				m_pInven[ i ].m_nCount += nRemain;
				nRemain = 0;
			}
		}
	}

	assert_cs( nRemain >= 0 );
	// 합치고 나서 남아 있는 아이템이 존재 하는가
	while( nRemain > 0 )
	{
		int nEmptySlot = FindEmptySlot( -1 );
		if( nEmptySlot == cData_Inven::INVALIDE_INVEN_INDEX )
			break;

		m_pInven[ nEmptySlot ] = *pItemData;

		if( nRemain <= pFTInfo->s_nOverlap )
		{
			m_pInven[ nEmptySlot ].m_nCount = nRemain;
			nRemain = 0;
		}
		else
		{
			m_pInven[ nEmptySlot ].m_nCount = pFTInfo->s_nOverlap;
			nRemain -= pFTInfo->s_nOverlap;
		}


		// 획득시 귀속이라면
// 		assert_cs( m_pInven[ nEmptySlot ].m_nLimited == 0 );
// 		if( pFTInfo->s_nBelonging == 2 )
// 			m_pInven[ nEmptySlot ].m_nLimited = 1;			

		// 이미 아이템 ID 闡려像繭窄?闡?
		if( _IsLock_ItemID( m_pInven[ nEmptySlot ].m_nType ) == true )
		{
			g_pDataMng->ItemLock( TO_INVEN_SID( nEmptySlot ) );
		}

		// 이미 아이템 Type 闡려像繭窄?闡?
		if( _IsLock_ItemType( pFTInfo->s_nType_L, pFTInfo->s_nType_S ) == true )
		{
			g_pDataMng->ItemLock( TO_INVEN_SID( nEmptySlot ) );
		}
	}

	// 퀘스트 체크
	// 진행 체크
	g_pDataMng->GetQuest()->CalProcess();
	// 받을수 있는지 체크
	g_pDataMng->GetQuest()->RevQuestCheck();
	// 퀵슬롯 체크
	cData_QuickSlot::CheckItemCount_AllUser();

	DWORD dwItemIdx = pItemData->GetType();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_ITEM_COUNT, &dwItemIdx );	// 재구매시

	return nRemain;

}
#endif
#ifdef TRICK_ORTREAT
float cData_Inven::GetEventTime()
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{		
		if( m_pInven[ i ].IsEnable() )
		{
			SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType ), 0.0f);

			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( m_pInven[ i ].m_nType )->GetInfo();
			SAFE_POINTER_RETVAL(pFTInfo, 0.0f);
			if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) )//&& g_pDataMng->GetQuest()->IsProcess( pFTInfo->s_nQuest1 ))
			{
				float ftime = (float) m_pInven[ i ].m_nEndTime - _TIME_TS;

				return ftime;	// 아이템의 남은시간
				//return (float)pFTInfo->s_nUseTime_Min*60;	// 테이블값의 리턴값
			}
				
		}			
	}
	return -1.0f;
}
#endif
void cData_Inven::ItemDrop( int nIndex, int nCount )
{
	if( g_pDataMng->IsItemLock( TO_INVEN_SID( nIndex ) ) )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	cItemInfo* pItemInfo = GetData( nIndex );
	SAFE_POINTER_RET( pItemInfo );

	if( !g_pDataMng->IsDropItem(pItemInfo) )
		return;

	// 버릴수 있는 아이템인지 체크
	switch( nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() )->GetInfo()->s_nDrop )
	{
	case 0:
		assert_cs( false );
		return;
	case 1:
		{
			DWORD dwType = pItemInfo->GetType();			
			cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_DROP, nsCsFileTable::g_pItemMng->GetItem( dwType )->GetInfo()->s_szName );

			if( net::game )
			{
				nSync::Pos pos( (int)g_pCharMng->GetTamerUser()->GetPos().x, (int)g_pCharMng->GetTamerUser()->GetPos().y );
				net::game->SendItemDrop( nIndex + SERVER_DATA_INVEN_CONSTANT, pos, nCount );
			}

			pItemInfo->DecreaseCount( nCount );

			// 퀵슬롯 체크
			cData_QuickSlot::CheckItemCount_AllUser();
			// 퀘스트 체크
			g_pDataMng->GetQuest()->CalProcess();
			g_pDataMng->GetQuest()->RevQuestCheck();
			cData_QuickSlot::CheckItemCount_AllUser();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_ITEM_COUNT, &dwType );
		}
		break;
	case 2:
		{
			cPrintMsg::PrintMsg( 11012 );
			cDigimonTalk::Print( 11012 );
		}
		break;
	}	
}

void cData_Inven::ItemRemove_NoServer_Type( int nType_L, int nType_S, int nCount, bool bCalQuest )
{	
	int nInven = 0;
	while( ( nInven = GetFirstSlot_Item_TypeLS( nType_L, nType_S ) ) != INVALIDE_INVEN_INDEX )
	{
		cItemInfo*	pInfo = GetData( nInven );
		SAFE_POINTER_BEK(pInfo);
		int nItemCount = pInfo->GetCount();
		if( nItemCount < (uint)nCount )
		{
			ItemUnlock_ItemType( nType_L, nType_S );
			nCount -= pInfo->GetCount();
			pInfo->Reset();
		}
		else
		{
			if( 0 == (nItemCount - nCount) )
				ItemUnlock_ItemType( nType_L, nType_S );

			pInfo->DecreaseCount( nCount );
			break;
		}
	}

	// 퀘스트 체크
	if( bCalQuest == true )
	{
		g_pDataMng->GetQuest()->CalProcess();
		g_pDataMng->GetQuest()->RevQuestCheck();
	}	
	cData_QuickSlot::CheckItemCount_AllUser();
}
#ifdef TRICK_ORTREAT
void cData_Inven::ItemRemove_NoServer( int nItemID,  bool bCalQuest )
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			continue;
		if(m_pInven[ i ].m_nType == nItemID)
		{
			ItemUnlock_ItemID( nItemID );
			m_pInven[ i ].Reset();
		}
	}
	// 퀘스트 체크
	if( bCalQuest == true )
	{
		g_pDataMng->GetQuest()->CalProcess();
		g_pDataMng->GetQuest()->RevQuestCheck();
	}	

	cData_QuickSlot::CheckItemCount_AllUser();
}

#endif

void cData_Inven::ItemRemove_NoServer( int nItemID, int nCount, bool bCalQuest )
{	
	int nInven = 0;
	while( ( nInven = GetFirstSlot_Item_ID( nItemID ) ) != INVALIDE_INVEN_INDEX )
	{
		cItemInfo*	pInfo = GetData( nInven );
		SAFE_POINTER_BEK(pInfo);
		if( pInfo->GetCount() < (uint)nCount )
		{
			ItemUnlock_ItemID( nItemID );
			nCount -= pInfo->GetCount();
			pInfo->Reset();
		}
		else
		{
			pInfo->DecreaseCount( nCount );
			break;
		}
	}

	// 퀘스트 체크
	if( bCalQuest == true )
	{
		g_pDataMng->GetQuest()->CalProcess();
		g_pDataMng->GetQuest()->RevQuestCheck();
	}	
	cData_QuickSlot::CheckItemCount_AllUser();
}

bool cData_Inven::ItemRemove_NoServer( int const& nInvenSlotNum, int nItemID, int nCount, bool bCalQuest )
{	
	cItemInfo* pItem = GetData( nInvenSlotNum );
	SAFE_POINTER_RETVAL( pItem, false );

	if( !pItem->IsEnable() )
		return false;
	
	if( pItem->GetType() != nItemID )
		return false;

	// 지우려는 아이템의 갯수 보다 많이 지운다??
	if( pItem->GetCount() < (uint)nCount )
		return false;

	if( pItem->GetCount() == (uint)nCount )
	{
		ItemUnlock_ItemID( nItemID );
		pItem->Reset();
	}
	else
	{
		pItem->DecreaseCount( nCount );
	}

	// 퀘스트 체크
	if( bCalQuest == true )
	{
		g_pDataMng->GetQuest()->CalProcess();
		g_pDataMng->GetQuest()->RevQuestCheck();
	}	
	cData_QuickSlot::CheckItemCount_AllUser();
	return true;
}

void cData_Inven::ItemRemove_NoServer( int nItemID, int nRate, int nCount, bool bCalQuest, bool bRemoveLimited )
{	
	int nInven = 0;
	while( ( nInven = GetFirstSlot_Item_ID_Rate( nItemID, nRate, bRemoveLimited == false ) ) != INVALIDE_INVEN_INDEX )
	{
		cItemInfo*	pInfo = GetData( nInven );
		SAFE_POINTER_BEK(pInfo);
		if( pInfo->GetCount() < (uint)nCount )
		{
			ItemUnlock_ItemID( nItemID );
			nCount -= pInfo->GetCount();
			pInfo->Reset();
		}
		else
		{
			pInfo->DecreaseCount( nCount );
			break;
		}
	}

	// 퀘스트 체크
	if( bCalQuest == true )
	{
		g_pDataMng->GetQuest()->CalProcess();
		g_pDataMng->GetQuest()->RevQuestCheck();
	}	
	cData_QuickSlot::CheckItemCount_AllUser();
}

void cData_Inven::OutputCropMsg( cItemData* const pItemData, int nCount )
{
	SAFE_POINTER_RET(pItemData);
	
	assert_csm( pItemData->GetType() != 0, _T( "cData_Inven::OutputCropMsg" ) );
	assert_csm( nsCsFileTable::g_pItemMng->IsItem( pItemData->GetType() ), _T("cData_Inven::OutputCropMsg Error : not Resist ItemID"));

	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() )->GetInfo();	

	int GetItemCount = 0;
	if( 0 == nCount )
		GetItemCount = pItemData->GetCount();
	else
		GetItemCount = nCount;

	if(GetItemCount == 1)
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_CROP, pFTItem->s_szName, &g_pDataMng->GetItemColor(pFTItem) );

	else if(GetItemCount > 1)
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_MANYCROP, pFTItem->s_szName, &GetItemCount, &g_pDataMng->GetItemColor(pFTItem) );

	if( pFTItem->IsNoticeItem() )
		cDigimonTalk::Print( 50008 );
}

void cData_Inven::SetInvenItemData( int nIndex ,const cItemData& rInfo )
{
	assert_cs( nIndex < nLimit::Inven );
	if( nIndex < 0 || nIndex >= nLimit::Inven )
		return;

	assert_cs( 0 <= rInfo.GetCount() );

	if( rInfo.GetCount() == 0 )
		m_pInven[nIndex].Reset();

	else
	{
		m_pInven[nIndex] = rInfo;
		std::pair< int, bool > pairInfo( nIndex, true );
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo );
	}

	// 퀘스트 체크
	// 진행 체크
	g_pDataMng->GetQuest()->CalProcess();
	// 받을수 있는지 체크
	g_pDataMng->GetQuest()->RevQuestCheck();
}

void cData_Inven::SetInvenItemEndTime( int nIndex ,int endtime )
{
	assert_cs( nIndex < nLimit::Inven );
	if( nIndex < 0 || nIndex >= nLimit::Inven )
		return;

	m_pInven[nIndex].m_nEndTime = endtime;	//종료시간입력
	m_pInven[nIndex].m_nLimited = 1;		//귀속아이템으로 변경
}
