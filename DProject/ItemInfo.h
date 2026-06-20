
#pragma once

#define _TIME_TS			( net::game ? net::game->GetTimeTS() : 0 )

class cItemInfo : public cItemData
{
#define ITEMINFO_INVALIDE_TIME		(u4)( -1 )

public:
	void		Reset(){ Clear(); }
	bool		IsEnable() const { return ( m_nAll != 0 ); }	
	bool		IsRemainSocket() const;
	void		DecreaseCount( uint nCount );
	bool		IsEnableTamerLv( int const& nTLv ) const;
	bool		IsEnableDigimonLv( int const& nDLv ) const;

	void		GetTamerReqLevel(int& nMinLv, int& nMaxLv ) const;
	void		GetDigimonReqLevel(int& nMinLv, int& nMaxLv ) const;

	int			GetSkillRate() const;

	static int	GetSocketRate( int nType );
	int			GetSocketRate();

	bool		IsCashEndTime( CsItem::sINFO const* pFTItem ) const;
	static bool	IsUseItemTime( CsItem::sINFO const* pFTItem );	

	void		SetTradeLimitedTimeEnd();

	void operator= ( const cItemData& x)
	{
		cItemData::operator=( x );
	}
	void operator= (const cItemInfo& x)
	{
		cItemData::operator=( x );
	}
};
