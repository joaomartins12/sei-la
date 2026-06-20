
#pragma once 

#define IF_TARGET_ELEMENT
#define ElementCount		7


class cTargetWindow : public cBaseWindow
{
public:
	cTargetWindow();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TARGET_WINDOW; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual bool			Close( bool bSound /* = true  */ );

	virtual void			PlayOpenSound(){}
	virtual void			PlayCloseSound(){}

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnRButtonUp( CsPoint pos );

	virtual void			ResetDevice();

	virtual bool			OnEscapeKey();

protected:
	CsC_AvObject*				m_pTargetObject;
// 	cData_Party::sINFO*			m_pPartyInfo;
// 	cData_Party::sINFO::sSTAT*	m_pPartyInfoStat;
	bool						m_bPartyStatTamer;

	bool						m_bOwner;
	
	// 메모리잡힌 컨트롤
protected:
	cSprite*				m_pWindowBG;
	cSprite*				m_pWindowBG_Mask;
	
#ifdef TARGET_WIN_MULTYLINE
	cSprite*				m_pWindowBG_2;
	cSprite*				m_pWindowBG_Mask_2;
	cText*					m_pPenNameText;
#endif

	cString					m_AttributeIcon;
	cString					m_NatureIcon;
	cString					m_MonsterAI;

	cSprite*				m_pLevelBG;
	cText*					m_pLevelText;

	cSprite*				m_pCMonsterMark;
	cSprite*				m_pCMonsterLevel;
	cSprite*				m_pBMonsterMark;
	cSprite*				m_pBMonsterLevel;
	int						m_nMonsterType;

	enum eCOMPARE{ UP, DOWN, EQUAL, BEST, WORST, COMPARE_MAX, };
#ifdef IF_TARGET_ELEMENT	
	cSprite*		m_pCompareSprite[ COMPARE_MAX ];
#else
	cSprite*		m_pUnknown;
#endif
	
	cSprite*		m_pNatureOutLineSprite[3];
	
	CsRect			m_rcAI;
	CsRect			m_rcAttribute;
	CsRect			m_rcNature;
	int			m_bNatureTooltip[3];	
	int				m_nOutLineCount;
	DWORD			m_dwAngles[3];

	cSprite*		m_pAttributeRateSprite[3];
	int				m_nAttributeRateIndex;
	CsPoint			m_cpAttributeStatus[3];

	// 포인터만 있는 컨트롤
protected:
	cText*			m_pNameText;	
	cProgressBar*	m_pHPBar;

public:
	//bool			IsPartyTarget();
	bool			IsTarget( uint nUniqID );

	void			SetTargetObject( CsC_AvObject* pTarget );
// 	void			SetTargetObject_Party( cData_Party::sINFO* pPartyInfo, bool bTamer );
// 
// 	cData_Party::sINFO*	GetPartyInfo(){ return m_pPartyInfo; }

	CsC_AvObject*	GetTargetWindow_Object() { return m_pTargetObject; }

protected:
	eCOMPARE		_GetAttributeCompare();
	eCOMPARE		_GetNatureCompare();

	int				_GetAttributeCompare2();
	DWORD			_GetNatureCompare2(int i);


	bool			_CheckOwner();

	//===============================================================================================
	//
	//		시간
	//
	//===============================================================================================
protected:
	bool			m_bEnableTimeBar;
	cSprite*		m_pTimeBarBG;
	cSprite*		m_pTimeBarBGMask;
	cProgressBar*	m_pTimeBar;
	cSpriteAni*		m_pTimeDeco;
};