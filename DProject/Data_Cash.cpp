
#include "stdafx.h"
#include "Data_Cash.h"

cData_Cash::cData_Cash()
{

}


cData_Cash::~cData_Cash()
{


}

void cData_Cash::Init()
{
	m_RefreshTime.SetDeltaTime( 11000 );
	m_RefreshTime.SetEnable();
}

void cData_Cash::InitNetOff()
{
	m_pCashData[ 0 ].m_nType = 5001;
	m_pCashData[ 0 ].m_nCount = 1;
	m_pCashData[ 1 ].m_nType = 5002;
	m_pCashData[ 1 ].m_nCount = 1;
	m_pCashData[ 2 ].m_nType = 5003;
	m_pCashData[ 2 ].m_nCount = 1;
	m_pCashData[ 3 ].m_nType = 5004;
	m_pCashData[ 3 ].m_nCount = 1;
	m_pCashData[ 4 ].m_nType = 6001;
	m_pCashData[ 4 ].m_nCount = 1;
	m_pCashData[ 5 ].m_nType = 6002;
	m_pCashData[ 5 ].m_nCount = 1;
	m_pCashData[ 6 ].m_nType = 6003;
	m_pCashData[ 6 ].m_nCount = 1;

	m_nCashCount = 7;
}

void cData_Cash::Reset()
{
	ResetCash();
}

void cData_Cash::ResetCash()
{
	m_nCashCount = 0;
	memset( m_pCashData, 0, sizeof( cItemInfo )*CASH_COUNT );
}
void cData_Cash::Update()
{	
}

//=============================================================================================================
//		케쉬
//=============================================================================================================
void cData_Cash::SetItemCashCount( int nCount )
{
	m_nCashCount = CsMin( nCount, CASH_COUNT ); 
}
int	 cData_Cash::GetItemCashCount()
{
	return m_nCashCount;
}	
cItemInfo*	cData_Cash::GetCashDataArray()
{
	return m_pCashData;
}
cItemInfo*	cData_Cash::GetCashData( int nIndex )
{
	assert_cs( nIndex < CASH_COUNT );
	return &m_pCashData[ nIndex ];
}
CsItem::sINFO*	cData_Cash::GetCashFTData( int nIndex )
{ 
	return nsCsFileTable::g_pItemMng->GetItem( GetCashData( nIndex )->GetType() )->GetInfo();
}
bool cData_Cash::IsExistItemCash( int nIndex )
{ 
	return m_pCashData[ nIndex ].IsEnable();
}

bool cData_Cash::RemoveCashItem( int nResetSlotNum )
{ 
	if( nResetSlotNum < 0 || nResetSlotNum > CASH_COUNT )
		return false;

	if( !m_pCashData[ nResetSlotNum ].IsEnable() )
		return false;

	m_pCashData[ nResetSlotNum ].Reset();
	return false;
}

//=============================================================================================================
//		갱신 시간
//=============================================================================================================
void cData_Cash::SetRefreshDisable()
{
	m_RefreshTime.SetDisable();
}
bool cData_Cash::IsRefreshEnable()
{ 
	return m_RefreshTime.IsEnable();
}

// 거래 제한 시간이 만료된 아이템을 서버에서 받을 때 
// 시간이 갱신이 안되어서 클라에서 구매 시간을 체크해
// 거래 가능으로 바꿔 준다.
void cData_Cash::UpdateTradeLimiteTime()
{
	for( int n = 0; n < CASH_COUNT; ++n )
	{
		if( !m_pCashData[n].IsEnable() )
			continue;

		if( !cData_Inven::IsTradeLimitedItem( &m_pCashData[n] ) )
			continue;
		
		int nRemain = m_pCashData[n].m_nRemainTradeLimitTime - _TIME_TS;
		if( nRemain <= 0 )
			m_pCashData[n].SetTradeLimitedTimeEnd();
	}
}