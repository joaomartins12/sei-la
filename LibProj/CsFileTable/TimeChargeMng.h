#pragma once

#include "TimeCharge.h"

class CsTimeChargeMng
{
public:
	CsTimeChargeMng();
	~CsTimeChargeMng();

	bool			Init( char* cPath );
	void			Delete();

private:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	void			SaveBin( char* cPath );

private:
	MAP_CHARGEITEM		m_mapChargeItem;		// 충전 아이템 목록
	MAP_BITCODE			m_mapBitCode;			// 비트 사용 충전 정보
	MAP_CASHCODE		m_mapCashCode;			// 아이템 사용 충전 정보

public:
	sChargeItemInfo const*	GetChargeItemInfo( DWORD dwItemCode ) const;
	sBitCodeInfo const*		GetBitCodeInfo( int nBitCode ) const;
	sBitCodeInfo const*		GetBitCodeInfo( DWORD dwItemCode ) const;
	int						GetCashCodeCount( DWORD dwItemCode ) const;
	int						GetCashNeedCount( int nCashCode, int nTimeIndex ) const;
	DWORD					GetUnlockMoney( int nSlotIndex ) const;
	bool					IsEnableChargeItem( DWORD dwItemCode ) const;
};