
#pragma once

#include "MakeTacticsContents.h"

#ifdef MINIGAME
#define MAX_BAR_LEN			152
#define MAX_BAR_BUTTON_COUNT		7 //바, 버튼 갯수

class cMinigame : public cBaseWindow, public MakeTacticsContents::ObserverType
{
	std::string BarSpriteName[MAX_BAR_BUTTON_COUNT];// = {"D", "I","G","I_2","M","O","N"};
	struct sBAR_N {
		sBAR_N()
			:m_pGaugeStencil(NULL)
		{ m_pGaugeStencil = NULL; m_pGaugeBar = NULL; m_nStartTick = 0; m_nBarTime = 1; m_nState = cButton::READY; }
		void		Delete(){ /*SAFE_NIDELETE( m_pBtn );*/ SAFE_NIDELETE( m_pGaugeStencil ); SAFE_NIDELETE( m_pGaugeBar ); }

		cButton*	m_pBtn;				// D,I,G,I,M,O,N 버튼

		cSprite*	m_pGaugeStencil;
		cSprite*	m_pGaugeBar;
		uint		m_nStartTick;		//버튼 눌렀을 때 시간
		uint		m_nBarTime;		//바 Charging Time
		u1			m_nState;			//바 성공, 실패, 대기 상태 
	};


public:
	enum GameKind { HATCH=1, INCHANT, NONE };
protected:
	//-------------------------------
	//Event, Key Event 
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	void					OnMiniGameStart(void);
	void					OnRecvClickResult(void);
	void					OnRecvClickTimeOut(void);
	void					OnMiniGameEnd(void);

	void					OnClickStopKey(void* pSender, void* pData);
	void					OnClickCloseKey(void* pSender, void* pData);
	//-------------------------------
public:
	//-------------------------------
	//Virtual
	cMinigame(void);
	virtual eWINDOW_TYPE	GetWindowType(void);
	virtual eMU_TYPE	Update_ForMouse(void);
	virtual	bool			Construct(void);
	virtual bool			OnEscapeKey(void);
	virtual void			Destroy(void);
	virtual void			DeleteResource(void);
	bool					Close( bool bSound = true );

	virtual void			Create( int nValue = 0 );	
	virtual void			Update(float const& fDeltaTime);
	virtual void			Render(void);

	virtual void			PlayOpenSound(void);
	virtual void			PlayCloseSound(void);
	virtual void			ResetDevice(void);
	virtual void			PreResetMap(void);
	//-------------------------------
private:
	//-------------------------------
	//Create, Update, Render
	void					CreateBars(void);
	void					CreateButtons(void);
	void					UpdateReadyCountdown(const float& fDeltaTime);
	float					UpdateCurrReadyCount(void);
	void					UpdateGauges(const float& fDeltaTime);
	void					RenderGauge(void);
	//-------------------------------

	//-------------------------------
	//Getter, Setter
	sBAR_N*				GetGauge( ushort iIdx );
	ushort					GetCurrGaugeIdx(void);
	void					SetGaugeChagingTime(  const ushort nBarIdx,  const ushort nBarTime );
	void					SetGaugeState(  const ushort nBarIdx,  const cButton::eSTATE eType );
	void					SetGaugeStartTime(void);
	void					SetStartGauge( const bool bCountDown );
	void					SetCurrGaugeIdx(  const ushort nBarIdx );
	CsPoint				CaclGaugeChargingRate( uint nCurGauge, uint nMaxGauge, CsPoint Size );

	ushort					GetSuccessCnt(void);
	void					SetSuccessCnt(  const ushort nCnt );
	void					SetServRecv(  const bool bRecv );	// 서버 수신 여부 Set
	//-------------------------------

private:
	sBAR_N		m_sBar[ MAX_BAR_BUTTON_COUNT ];		// Gauge

	cButton*		m_pOutBtn;		// 나가기 버튼
	cButton*		m_pStopBtn;		// 멈추기 버튼
	cButton*		m_pLetters;		// 전광판 : Letters
	cButton*		m_pUp;			// 전광판 : Up
	ushort			m_CurrGuageIndex;	
	ushort			m_nSuccessCnt;	//성공 카운트
	bool			m_bStartGaugeUpdate;	
	bool			m_bStartReadyCountingDown;
	bool			m_bServerRecv;
};

#include "Minigame.inl"
#endif