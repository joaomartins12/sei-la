
#include "stdafx.h"
#include "Item.h"

CsItemCoolTime* CsItem::sINFO::GetCoolTime()
{
	return s_nUseTimeGroup ? nsCsFileTable::g_pItemMng->GetCoolTime( s_nUseTimeGroup ) : NULL;
}

CsCoolTimeSeq* CsItem::sINFO::GetCoolTimeSeq()
{
	return s_nUseTimeGroup ? nsCsFileTable::g_pItemMng->GetCoolTime( s_nUseTimeGroup )->GetSeq() : NULL;
}

bool CsItem::sINFO::IsCashType() const 
{ 
	return ( s_nClass == 10 )||( s_nClass == 13 ); 
}

bool CsItem::sINFO::IsPCBangType() const 
{ 
	return s_nClass == 12; 
}

bool CsItem::sINFO::IsNoticeItem() const
{
	switch( s_nClass )
	{
	case 4:
	case 5:
	case 6:
	case 7:
		return true;
	}

	return false;
}

void CsItem::TypeS_to_TypeLS( int nTypeS, int& nResTypeL, int& nResTypeS )
{ 
	nResTypeL = nTypeS/100; 
	nResTypeS = nTypeS%100; 
}

void CsItem::Delete()
{

}

void CsItem::Init( sINFO* pSrcInfo )
{ 
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); 
}

CsItem::sINFO* CsItem::GetInfo()
{ 
	return &m_Info; 
}

std::wstring CsItem::GetName() const
{
	return m_Info.s_szName;
}

CsItem::eEnableType CsItem::IsEnableTamerLv( int const& nTamerLv, int& nNeedLv) const
{
	return m_Info.IsEnableTamerLv( nTamerLv, nNeedLv );
}

CsItem::eEnableType CsItem::IsEnableDigimonLv(int const& nDigimonLv, int& nNeedLv) const
{
	return m_Info.IsEnableDigimonLv( nDigimonLv, nNeedLv );
}

bool CsItem::IsEnableTamerLv(const int& nTamerLv) const
{
	return m_Info.IsEnableTamerLv( nTamerLv );
}

bool CsItem::IsEnableDigimonLv(const int& nDigimonLv) const
{
	return m_Info.IsEnableDigimonLv( nDigimonLv );
}

void CsItem::GetTamerReqLv( int& nMinLv, int& nMaxLv ) const
{
	m_Info.GetTamerReqLv( nMinLv, nMaxLv );
}

void CsItem::GetDigimonReqLv( int& nMinLv, int& nMaxLv ) const
{
	m_Info.GetDigimonReqLv( nMinLv, nMaxLv );
}

bool CsItem::sINFO::IsAccessory() const
{ 
	if( nsCsItemTable::OPTIONCODE != s_nSkillCodeType )
		return false;

	switch( s_nType_L )
	{
	case 28://nItem::Necklace
	case 29://nItem::Ring
	case 30://nItem::Earring
	case 33://nItem::Bracelet
	case 53://nItem::Digivice
		return true;
	}
	return false;
}

bool CsItem::sINFO::IsAccStone() const
{ 
	return ( s_nType_L == 110/*nItem::AccStone*/ );
}

CsItem::eEnableType CsItem::sINFO::IsEnableTamerLv(int const& nTamerLv, int & nNeedLv) const
{
	// 제한 레벨이 같은 경우 수치 이상이면 사용 가능 
	if( s_nTamerReqMinLevel == s_nTamerReqMaxLevel )
	{
		if( s_nTamerReqMinLevel <= nTamerLv )
			return CsItem::eEnableUse;
	}

	if( nTamerLv < s_nTamerReqMinLevel )
	{
		nNeedLv = s_nTamerReqMinLevel;
		return CsItem::eNeedMinLv;
	}
	
	if( 1 >= s_nTamerReqMaxLevel )// Max Lv 제한 없음.
		return CsItem::eEnableUse;

	if( nTamerLv > s_nTamerReqMaxLevel )
	{
		nNeedLv = s_nTamerReqMaxLevel;
		return CsItem::eOverMaxLv;
	}

	return CsItem::eEnableUse;
}

CsItem::eEnableType CsItem::sINFO::IsEnableDigimonLv(int const& nDigimonLv, int & nNeedLv) const
{
	// 제한 레벨이 같은 경우 수치 이상이면 사용 가능 
	if( s_nDigimonReqMinLevel == s_nDigimonReqMaxLevel )
	{
		if( s_nDigimonReqMaxLevel <= nDigimonLv )
			return CsItem::eEnableUse;
	}

	if( nDigimonLv < s_nDigimonReqMinLevel )
	{
		nNeedLv = s_nDigimonReqMinLevel;
		return CsItem::eNeedMinLv;
	}

	if( 1 >= s_nDigimonReqMaxLevel )// Max Lv 제한 없음.
		return CsItem::eEnableUse;

	if( nDigimonLv > s_nDigimonReqMaxLevel )
	{
		nNeedLv = s_nDigimonReqMaxLevel;
		return CsItem::eOverMaxLv;
	}

	return CsItem::eEnableUse;
}


bool CsItem::sINFO::IsEnableTamerLv(int const& nTamerLv) const
{
	int nReqLv = 0;
	eEnableType eResult = IsEnableTamerLv(nTamerLv, nReqLv);
	if( CsItem::eEnableUse != eResult )
		return false;

	return true;
}

bool CsItem::sINFO::IsEnableDigimonLv(int const& nDigimonLv) const
{
	int nReqLv = 0;
	eEnableType eResult = IsEnableDigimonLv(nDigimonLv, nReqLv);
	if( CsItem::eEnableUse != eResult )
		return false;

	return true;
}


void CsItem::sINFO::GetTamerReqLv( int& nMinLv, int & nMaxLv ) const
{
	nMinLv = s_nTamerReqMinLevel;
	nMaxLv = s_nTamerReqMaxLevel;
}

void CsItem::sINFO::GetDigimonReqLv( int& nMinLv, int & nMaxLv ) const
{
	nMinLv = s_nDigimonReqMinLevel;
	nMaxLv = s_nDigimonReqMaxLevel;	
}