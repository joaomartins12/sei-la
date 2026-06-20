
#pragma once 

#define IF_OP_SEL_COUNT					15
#define IF_OP_COMBO_STR_MAX_COUNT		30
#define IF_OP_COMBO_STR_MAX_LEN			64
#define IF_OP_CHECKBOX_MAX_COUNT		3

class cOptionBase : public cBaseWindow
{
public:
	cOptionBase(){}

public:
	virtual eWINDOW_TYPE	GetWindowType() = 0;

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual bool			CreateBase( TCHAR const* szWinName, int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			OnMoveWindow();

protected:
	void					_ResizeScrollBar(){ m_pScrollBar->SetRange( CsPoint( 0, (int)m_listControl.size() ) ); }

	void					_AddControl_Title( TCHAR const* sz, int nValue = 0 );
	void					_AddControl_CheckRadio( TCHAR const* szName, TCHAR const* szOver, int nRadioID, int nValue = 0 );
	void					_AddControl_Combo( TCHAR const* szName, int nArrayCount, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], NiColor* pColorArray, int* pDataArray, int nSelData, int nValue = 0 );
	void					_AddControl_Slider( TCHAR const* szName, CsPoint ptRange, int nBtnMoveRange, int nStartPos, TCHAR const* szLow, TCHAR const* szHigh, int nValue = 0 );
	void					_AddControl_CheckBox( TCHAR const* szName, int nCheckBoxCnt, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], int* pDataArray, bool bBattle, bool bOnly = true, int nValue = 0 );

	void					_DeleteControl();
	virtual void			_OnOk();
	virtual void			_OnCancel();

public:
	struct sOP_BASE : public NiMemObject
	{
		sOP_BASE( cOptionBase* pParent ){ s_pParent = pParent; s_pOptionName = NULL; }
		enum eTYPE{ TITLE, CHECK_RADIO, COMBO, SLIDER, CHECK_BOX };

		static sOP_BASE*	NewInstance( cOptionBase* pParent, eTYPE type );
		static void			DeleteInstance( sOP_BASE* pInstance );

		virtual eTYPE	GetType() = 0;

		virtual void	Delete(){ SAFE_NIDELETE( s_pOptionName ); }
		virtual void	Update(float const& fDeltaTime){}
		virtual void	Update_ForMouse(){}		
		virtual void	Render( CsPoint pos ){}
		virtual void	ResetDevice(){ if( s_pOptionName ){ s_pOptionName->ResetDevice(); } }

		cOptionBase*	s_pParent;
		int				s_nValue;		
		cText*			s_pOptionName;
	};

	struct sOP_TITLE : public sOP_BASE
	{
		sOP_TITLE( cOptionBase* pParent ):sOP_BASE( pParent ){
		}
		virtual eTYPE	GetType(){ return TITLE; }

		virtual void	Delete();
				void	Init( TCHAR const* szTitle, int nValue );		
		virtual void	Render( CsPoint pos );		

		virtual void	ResetDevice(){ sOP_BASE::ResetDevice(); }
	};

	struct sOP_CHECK_RADIO : public sOP_BASE
	{
		sOP_CHECK_RADIO( cOptionBase* pParent ):sOP_BASE( pParent ){ s_pCheck = NULL; s_pOverText = NULL; }
		virtual eTYPE	GetType(){ return CHECK_RADIO; }

		virtual void	Delete(){ sOP_BASE::Delete(); SAFE_NIDELETE( s_pCheck ); SAFE_NIDELETE( s_pOverText ); }
				void	Init( TCHAR const* szName, TCHAR const* szOver, int nRadioID, int nValue );
		virtual void	Update_ForMouse();
		virtual void	Render( CsPoint pos );

		virtual void	ResetDevice(){ sOP_BASE::ResetDevice(); if( s_pCheck ){ s_pCheck->ResetDevice(); } if( s_pOverText ){ s_pOverText->ResetDevice(); } }


		int				s_nRadioID;
		cCheckBox*		s_pCheck;
		cText*			s_pOverText;
	};

	struct sOP_COMBO : public sOP_BASE
	{
		sOP_COMBO( cOptionBase* pParent ):sOP_BASE( pParent ){ s_pCombo = NULL; }
		virtual eTYPE	GetType(){ return COMBO; }

		virtual void	Delete(){ sOP_BASE::Delete(); SAFE_NIDELETE( s_pCombo ); }
				void	Init( TCHAR const* szName, int nArrayCount, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], NiColor* pColorArray, int* pDataArray, int nSelData, int nValue );
		virtual void	Update(float const& fDeltaTime);
		virtual void	Update_ForMouse();
		virtual void	Render( CsPoint pos );
		virtual void	ResetDevice(){ sOP_BASE::ResetDevice(); if( s_pCombo ){ s_pCombo->ResetDevice(); } }
				bool	IsChange();

		cComboBox*		s_pCombo;
		int				s_nBackupData;
	};

	struct sOP_SLIDER : public sOP_BASE
	{
		sOP_SLIDER( cOptionBase* pParent ):sOP_BASE( pParent ){ s_pSlider = NULL; s_pBG = NULL; s_pLowText = NULL; s_pHighText = NULL; }
		virtual eTYPE	GetType(){ return SLIDER; }

		virtual void	Delete(){ sOP_BASE::Delete(); SAFE_NIDELETE( s_pSlider ); SAFE_NIDELETE( s_pBG ); SAFE_NIDELETE( s_pLowText ); SAFE_NIDELETE( s_pHighText ); }
				void	Init( TCHAR const* szName, CsPoint ptRange, int nBtnMoveRange, int nStartPos, int nValue, TCHAR const* szLow, TCHAR const* szHigh );
		virtual void	Update(float const& fDeltaTime);
		virtual void	Update_ForMouse();
		virtual void	Render( CsPoint pos );
		virtual void	ResetDevice(){ sOP_BASE::ResetDevice(); if( s_pSlider ){ s_pSlider->ResetDevice(); } if( s_pBG ){ s_pBG->ResetDevice(); } }

		cSlider*		s_pSlider;
		cSprite*		s_pBG;
		int				s_nBackupPos;
		cText*			s_pLowText;
		cText*			s_pHighText;
		CsPoint			s_BarPos;
		TCHAR			s_szNum[ 8 ];
	};

	struct sOP_CHECK_BOX : public sOP_BASE
	{
		sOP_CHECK_BOX( cOptionBase* pParent ):sOP_BASE( pParent ){ for( int i = 0; i < IF_OP_CHECKBOX_MAX_COUNT ; i++ ) { s_pCheck[i] = NULL; s_pCheckText[i]; } }
		virtual eTYPE	GetType(){ return CHECK_BOX; }

		virtual void	Delete();
		void			Init( TCHAR const* szName, int nCheckBoxCnt, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], int* pDataArray, bool bBattle, bool bOnly, int nValue );
		virtual void	Update_ForMouse();
		virtual void	Render( CsPoint pos );

		virtual void	ResetDevice();

		int				s_nCheckBoxCnt;
		bool			s_bOnly;	// 유일하게 선택되어야 하면 true, 중복 선택 가능하면 false ( 중복 선택 가능한가? )
		bool			s_bBattle;	// 배틀(PvP) 맵에서 옵션 변경 가능하게 할건지 ( true면 변경 가능 )
		cCheckBox*		s_pCheck[IF_OP_CHECKBOX_MAX_COUNT];
		cText*			s_pCheckText[IF_OP_CHECKBOX_MAX_COUNT];
	};

	// 전체 리스트 관련
public:
	std::list< sOP_BASE* >*	GetListControl(){ return &m_listControl; }

	// 체크 라디오 관련
public:
	sOP_CHECK_RADIO*	GetCheckRadio(){ return m_pCheckRadio; }
	void				SetCheckRadio( sOP_CHECK_RADIO* pCR ){ m_pCheckRadio = pCR; }	
	void				ReleaseCheckRadio(){ if( m_pCheckRadio ){ m_pCheckRadio->s_pCheck->SetCheck( false ); m_pCheckRadio = NULL; } }

	// 콤보 박스 관련
public:
	sOP_COMBO*			GetFocusCombo(){ return m_pFocusCombo; }
	void				ReleaseFocusCombo(){ m_pNextFocusCombo = NULL; }	
	void				SetFocusCombo( sOP_COMBO* pCR ){ m_pNextFocusCombo = m_pFocusCombo = pCR; }	
	void				SetFocusComboPos( CsPoint ptPos ){ m_ptFocusComboPos = ptPos; }
	sOP_COMBO*			FindCombo( int nComboValue );

	virtual void		Combo_SelChange( sOP_COMBO* pCR ){}

	// 슬라이더 관련
public:
	sOP_SLIDER*			FindSlider( int nSliderValue );

	// 체크 박스 관련
public:
	sOP_CHECK_BOX*		FindCheckBox( int nCheckBoxValue );

	// 필요 변수
protected:
	std::list< sOP_BASE* >	m_listControl;
	sOP_CHECK_RADIO*		m_pCheckRadio;
	
	sOP_COMBO*				m_pNextFocusCombo;
	sOP_COMBO*				m_pFocusCombo;
	CsPoint					m_ptFocusComboPos;

	// 메모리잡힌 컨트롤
protected:

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;
	cButton*		m_pOkBtn;
	cScrollBar*		m_pScrollBar;
	cText*			m_pWinName;
};

