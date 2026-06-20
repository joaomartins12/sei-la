#pragma once

class DataTradeDigimonUpgrade : public DataTradeBase

{
public:
	DataTradeDigimonUpgrade(void);
	virtual ~DataTradeDigimonUpgrade(void);

////////////							COMMON				//////////////////
protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

protected:
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();
	virtual bool			Close( bool bSound /* = true  */ );
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DATA_TRADE_DIGIMON_UPGRADE; }

////////////						UNCOMMON				//////////////////
protected:
	virtual	void			CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);	// 좌우 데이터 교환 슬롯 정보 생성.
	virtual void			DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);
	virtual void			ResetPosition();							// 해상도 재설정 시에 위치 재설정
	virtual	void			ResetData();								// 데이터 교환 후 교환 된 데이터로 리셋해준다.
	virtual void			OnFlicker();

private:
	struct sEnchantLvInfo
	{
		sEnchantLvInfo() : s_pLvIcon(NULL) {}
		~sEnchantLvInfo() {}

		void				ChangeLvIcon( int const& nLv );
		void				SetVisible( bool const& bVisible );

		cString::sICON*		s_pLvIcon;
	};
	typedef std::map< int, sEnchantLvInfo >	MAP_ENCHANT_INFO;
	typedef MAP_ENCHANT_INFO::iterator		MAP_ENCHANT_INFO_IT;
	typedef MAP_ENCHANT_INFO::const_pointer MAP_ENCHANT_INFO_CIT;

	MAP_ENCHANT_INFO		m_mapLeftInfo;
	MAP_ENCHANT_INFO		m_mapRightInfo;

	cGridListBox*			m_pLeftGrid;
	cGridListBox*			m_pRightGrid;

private:
	void					_MakeEnchantLvInfo();
	void					_AddEnchantLvInfo( cGridListBox* pGrid, MAP_ENCHANT_INFO& mapEnchant, int const& nState );
	void					_UpdateChantLvInfo( eDIGIMONSLOT const& eSlot );
};