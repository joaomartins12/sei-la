#pragma once

class DataTradeDigimonEvolution : public DataTradeBase
{
public:
	DataTradeDigimonEvolution(void);
	virtual ~DataTradeDigimonEvolution(void);

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
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DATA_TRADE_DIGIMON_EVOLUTION; }

////////////						UNCOMMON				//////////////////

private:
	struct ST_EVOL_INFO
	{
		ST_EVOL_INFO();
		virtual	~ST_EVOL_INFO();
		void Delete();
		void ResetDevice();

		sIFIcon					s_IFIcon;
		cImage*					s_pImage;

		CDigimonUser::eEvoState	s_eSlotState;

		bool					s_bActiveRide;
	};

private:
	void					CreateEvolutionInfoLeft();
	void					CreateEvolutionInfoRight();

	void					DeleteEvolutionInfoLeft();
	void					DeleteEvolutionInfoRight();

protected:

	virtual	void			CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);	// 좌우 데이터 교환 슬롯 정보 생성.
	virtual void			DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);
	virtual void			ResetPosition();							// 해상도 재설정 시에 위치 재설정
	virtual	void			ResetData();								// 데이터 교환 후 교환 된 데이터로 리셋해준다.
private:
	void					_RenderEvolLeft();
	void					_RenderLineLeft();

	void					_RenderEvolRight();
	void					_RenderLineRight();
	void					_SlotTooltip(ST_EVOL_INFO*	pInfo, eDIGIMONSLOT	eSlot, CsPoint	pos);
private:
	ST_EVOL_INFO			m_EvolInfoLeft[ nLimit::EvoUnit ];
	ST_EVOL_INFO			m_EvolInfoRight[ nLimit::EvoUnit ];

private:
	cSprite*	m_pEvolBG;
	cSprite*	m_pLineLR;
	cSprite*	m_pLineTB;
	cSprite*	m_pIconBG;
	cSprite*	m_pEvolBlock;
	cSprite*	m_pRideMask;
	//cButton*	m_pEvolLockBtn;	// 자물쇠 표시 안함.
};