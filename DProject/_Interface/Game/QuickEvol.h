
#pragma once 

class cQuickEvol : public cBaseWindow
{
public:
	cQuickEvol();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_QUICKEVOL; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );

	virtual void			ResetDevice();
	virtual void			ResetMap(){}

	virtual bool			OnMacroKey(const MSG& p_kMsg);	//2016-01-25-nova 윈도우MSG처리

protected:
	struct EvolveIcon
	{
		EvolveIcon(){ s_pImage = NULL; s_pEvolObjInfo = NULL; 
#ifdef SDM_TAMER_XGUAGE_20180628
		s_isXAnti = false; 
#endif
		}
		void Delete(){ SAFE_NIDELETE( s_pImage ); s_pEvolObjInfo = NULL; s_IFIcon.ReleaseState( true ); s_bRender = false; }

		cImage*								s_pImage;		
		CDigimonEvolveObj::SEvolutionInfo*	s_pEvolObjInfo;
		sIFIcon								s_IFIcon;
		bool								s_bRender;
#ifdef SDM_TAMER_XGUAGE_20180628
		bool								s_isXAnti;
#endif	
	};
	EvolveIcon			EvolveIcons[ nLimit::EvoStep ];	

protected:
	CsCoolTimeSeq		m_EvolCoolTime;
	CsC_AvObject*		m_pTargetObject;	

	int					m_nQuickEvolIndex;
	DWORD				m_dwDestDigimonID;

public:
	void			SetTargetObject( CsC_AvObject* pTarget );
	void			Req_ActivateEvolve( int nIndex, bool bAutoEvo = false );
	void			Send_Evolvution(int EvolveIdx);

private:
//Checking Evolution's Conditions
	bool			isXAntiDigimon(DWORD BaseDigimonID);
	bool			isCoolTimeNow(void);
	bool			CheckCommonCondition(int iEvolutionIdx);
    // Normal Digimon의 경우
	void			CheckingEvolutionConditions(const int nIndex, bool bAutoEvo);
	bool			CheckingDs(int nIndex);
	bool			CheckHasEvolItem(int nIndex, bool bAutoEvo);
	bool			CheckEvolAbleArea(int nIndex, bool bAutoEvo);
	bool			CheckEvolAbleAnimState(void);
	bool			CheckPacketSync(void);
	bool			CheckingEvolAboutJogress(int nIndex, bool bAutoEvo);
	bool			CheckTamerForEvolution();
	bool			CheckDigiviceChipset(CDigimonEvolveObj* pFTEvolObj);

	// X항체 디지몬의 경우;
	bool			isSettedXAISystem(void);
	bool			hasEnoughXGage(int nIndex);
	bool			isXEvolutionOfJogress(int nIndex);
	bool			CheckingXEvolutionOfJogress(int nIndex);

public:
	CsCoolTimeSeq*	GetEvolCoolTimeSeq(){ return &m_EvolCoolTime; }

	CsC_AvObject*	GetTargetObject(){return m_pTargetObject;}
	void			SetQuickEvolIndex(int nQuickEvolIndex){m_nQuickEvolIndex = nQuickEvolIndex;}
	int				GetQuickEvolIndex(){return m_nQuickEvolIndex;}

	void			SetDestDigimonID( DWORD dwDestDigimonID ){ m_dwDestDigimonID = dwDestDigimonID;}
	DWORD				GetDestDigimonID(){return m_dwDestDigimonID;}

	//=====================================================================================================================
	//
	//		라이딩
	//
	//=====================================================================================================================
protected:
	CsCoolTimeSeq	m_RideCoolTime;
	cButton*		m_pRideBtn;
	bool			m_bEnableRide;

public:
	void			SetRideToggle();
	void			SetRide( bool bRide );

	//퀵슬롯 오픈여부에 따라 위치 옮겨야됨
	CsPoint			m_ptRootPos;
	cSprite*		m_pSlotBG;		//위치 안옮겨져서 그냥 따로 만듬

#ifdef SDM_TAMER_XGUAGE_20180628
	cSprite*		m_pXSlotBG;		//위치 안옮겨져서 그냥 따로 만듬
#endif
};