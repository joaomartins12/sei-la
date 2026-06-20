
#pragma once 


class AdaptTutorialQuest;

class cQuickSlot : public cBaseWindow
{
public:
	cQuickSlot();
	~cQuickSlot();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_QUICKSLOT; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	virtual void			ResetDevice();
	
	virtual bool			OnMacroKey(const MSG& p_kMsg);	//2016-01-18-nova 윈도우Msg로 처리

	virtual void			ChangeMacroKey();

protected:

	// 메모리잡힌 컨트롤
protected:
	cButton*			m_pInfoBtn;

	// 포인터만 있는 컨트롤
protected:
	cText*				m_pMacroText[ nLimit::QuickSlot ];
	cSprite*			m_pBox;
	cImage*				m_pBox2;

protected:
	CsC_AvObject*		m_pTargetObject;
	int					m_nQuickSlotIndex;
	sIFIcon				m_IFIcon[ nLimit::QuickSlot ];

public:
	void			SetTargetObject( CsC_AvObject* pTarget );
	int				BoxImage_FromDist( int nSlotIndex, float fDist );

protected:
	void			_RenderTamer();
	void			_RenderDigimon();
	void			_RenderSlot( cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot, cSkill* pSkillMng );

	cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* _CalQuickSlotConstant( int& nQuickSlotType, int& nID, int nIndex );

	/************************************************************************/
	/*								 퀵슬롯 타입						              */
	/************************************************************************/
public:
	enum eQUICK_TYPE{
		Q_TAMER = 0,
		Q_DIGIMON = 1,
		Q_ADDSLOT = 2,
		QUICKSLOT_MAXCNT = 3,
		NONE = 999,
	};
	eQUICK_TYPE	m_eQ_Type;

	/************************************************************************/
	/*                    센터바, 퀵슬롯 열기/닫기 버튼                       */
	/************************************************************************/
protected:
	//	센터바 버튼은 퀵슬롯 가장 윗줄에 붙여야함, 퀵슬롯 한줄 더 추가 시 마지막 추가 퀵슬롯에 붙임
	cButton*		m_pCenterBarOpenBtn;	// 센터바 열기 버튼
	cButton*		m_pCenterBarCloseBtn;	// 센터바 닫기 버튼

	cButton*		m_pSlotOpenBtn;			// 센터바 열기 버튼
	cButton*		m_pSlotCloseBtn;		// 센터바 닫기 버튼

	CsPoint			m_ptRootPos;			// 디지몬 퀵슬롯 기본 위치

	/************************************************************************/
	/*                           추가 퀵슬롯 정보                           */
	/************************************************************************/
public:
	cResist::sCHAR_QUICKSLOT*		m_pAddQuickSlot;		// 추가 퀵슬롯 단축키 정보, QuickSlotChar 는 0만 씀

	cResist::sCHAR_QUICKSLOT::sCHAR*	GetQuickSlot(){ return &m_pAddQuickSlot->s_QuickSlotChar[Q_ADDSLOT]; }
	void			_InitSlot_AddQuick( cResist::sCHAR_QUICKSLOT* pQuickSlot );		// 추가 퀵슬롯 초기화

	void			_DeleteSkill_AddSlot( int nType );		// 디지몬/테이머 교체 시 추가 퀵슬롯에 등록된 스킬 제거( Type : RTTI_테이머 or RTTI_디지몬 )

	/************************************************************************/
	/*                       추가 퀵슬롯 랜더/업데이트                      */
	/************************************************************************/
protected:
	void					_Render_AddSlot();					// 추가 퀵슬롯 랜더
	cBaseWindow::eMU_TYPE	_Update_ForMouse_AddSlot();			// 추가 퀵슬롯 업데이트
	/************************************************************************/
	/*								기타 추가 리소스		                */
	/************************************************************************/
protected:
	cImage*			m_pAddImage;			// 초상화위치 추가퀵슬롯 이미지


	// Tutorial Quick Slot Control
private:
	std::list< int >		m_lLockSlot;

public:
	void					LockSlot( int const& nSlotIndex );
	void					UnlockSlot( int const& nSlotIndex );
	void					LockSlotAll();
	void					UnlockSlotAll();
	bool					IsLockSlot( int const& nSlotIndex ) const;
	int						GetSlotIndex( DWORD const& dwSlotType, DWORD const& dwSlotValue );
	CsPoint					GetSlotPos( int const& nSlotIndex );

private:
	TOwnershipGuard< AdaptTutorialQuest >		m_pAdaptTutorial;
};

