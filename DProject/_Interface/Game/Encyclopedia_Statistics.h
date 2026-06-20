#pragma once
#include "EncyclopediaContents.h"

class cEncyclopedia_Statistics : public cBaseWindow, public EncyclopediaContents::ObserverType
{
public:
	cEncyclopedia_Statistics();
	virtual ~cEncyclopedia_Statistics();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream){}
	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_ENCYCLOPEDIA_STATS; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			OnMoveWindow();

	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey(){ Close( false ); return true; }

	virtual void			UserDie(){ Close( false ); }

	/************************************************************************/
	/*                           도감 통계 UI			                               */
	/************************************************************************/
protected:
	cText*					m_szDigimon_Cnt;		// 디지몬(계열체) 보유 수 ( 달성률 )
	cText*					m_szAvrage_Lv;			// 디지몬 평균 레벨
	cText*					m_szSum_Lv;				// 디지몬 레벨 총 합
	cText*					m_szAvrage_Scale;		// 디지몬 평균 크기
	cText*					m_szStar_Cnt;			// 획득 별 수
	cText*					m_szComp_Rate;			// 도감 달성률
public:
	void					_SetText_Stat();		// 통계 텍스트 셋팅
};