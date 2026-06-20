

#pragma once 

#define CASH_COUNT				49
#define POSTAL_COUNT			7

class cData_Cash
{
public:
	cData_Cash();
	virtual	~cData_Cash();

public:
	void	Delete(){}
	void	Init();

	void	Reset();
	void	ResetCash();
	void	Update();

	void	InitNetOff();

protected:
	uint			m_nCashCount;
	cItemInfo		m_pCashData[ CASH_COUNT ];
	CsEnableTimeSeq	m_RefreshTime;

	//=============================================================================================================
	//		케쉬
	//=============================================================================================================
public:
	void			SetItemCashCount( int nCount );
	int				GetItemCashCount();
	cItemInfo*		GetCashDataArray();
	cItemInfo*		GetCashData( int nIndex );
	CsItem::sINFO*	GetCashFTData( int nIndex );
	bool			IsExistItemCash( int nIndex );
	bool			RemoveCashItem( int nResetSlotNum );
	
	//=============================================================================================================
	//		갱신 시간
	//=============================================================================================================
public:	
	void			SetRefreshDisable();
	bool			IsRefreshEnable();
	void			UpdateTradeLimiteTime();
};