#pragma once

class DataTradeDigimonScale : public DataTradeBase

{
public:
	DataTradeDigimonScale(void);
	virtual ~DataTradeDigimonScale(void);

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
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DATA_TRADE_DIGIMON_SCALE; }

////////////						UNCOMMON				//////////////////

protected:
	virtual	void			CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);	// 좌우 데이터 교환 슬롯 정보 생성.
	virtual void			DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo);
	virtual void			ResetPosition();							// 해상도 재설정 시에 위치 재설정
	virtual	void			ResetData();								// 데이터 교환 후 교환 된 데이터로 리셋해준다.

private:
	//cSprite*				m_pBGLeft;
	//cSprite*				m_pBGRight;

	cText*					m_pDigimonScaleLeft;	// 스케일 크기
	cText*					m_pDigimonScaleRight;	// 스케일 크기	
private:
	void					SetDigimonScaleLeft(int cmSize, int percentSize, int hatchlv);	// ?cm, ?% 표시
	void					SetDigimonScaleRight(int cmSize, int percentSize, int hatchlv);	// ?cm, ?% 표시
	int						GetPercentCmSize(stLoadData*		pLoadData, stModelInfo*	pModelInfo);	// 쎈티미터 사이즈
};