
#pragma once

//class cMoneyIF;
class cString : public NiMemObject
{
public:
	struct sELEMENT : public NiMemObject
	{
		sELEMENT() : s_bEnable(true),s_AutoPointerDelete(false)
		{};
		virtual ~sELEMENT()
		{};

		enum eTYPE{ TEXT, SPRITE, ICON, IMAGE, CHECK_BOX, MONEYIF, DATA_STR, BUTTON, PROGRESS, TIMERPROGRESS, ANIBUTTON };

		virtual eTYPE	GetType() = 0;
		virtual void	Delete() = 0;
		virtual void	Render( CsPoint pos ) = 0;
		virtual void	RenderLimit( CsPoint pos );
		virtual void	SetAlpha( float fAlpha ) = 0;
		virtual void	Update( float const& fDeltaTime ) {};
		virtual void	SetColor( NiColor color );
		virtual void	ResetDevice();
		//virtual void	SetEnable( bool bValue, int nValue ) = 0;
		virtual void	SetEnable( bool bEnable ) = 0;
		virtual void    OnMouseOver(){};
		virtual void    OnMouseLeave(){};
		virtual void    OnMouseDown(){};
		virtual void    OnMouseUp(){};
		virtual void    onMousePressed(){};
		virtual void    SetVisible(bool bValue){};
		virtual void	SetOnMode(bool bValue) {};
		virtual	CsPoint GetWorldPos() const { return CsPoint::ZERO; }
		virtual	CsRect GetWorldRect() const { return CsRect::ZERO; }

		virtual	bool	GetEnable() const ;
		virtual void	SetAutoPointerDelete(bool bValue){ s_AutoPointerDelete = bValue; }

		bool			s_bEnable;

		CsPoint			s_ptSize;
		
		bool			s_AutoPointerDelete;
	};

	struct sTEXT : public sELEMENT
	{
		CSGBMEMPOOL_H( cString::sTEXT );
		sTEXT();
		~sTEXT();

		virtual eTYPE	GetType();
		void			Delete();
		void			Render( CsPoint pos );
		void			RenderLimit( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			SetFontSize( CFont::eFACE_SIZE FontSize );
		virtual void	SetColor( NiColor color );
		virtual void	ResetDevice();
		void			SetText( TCHAR const* sz );
		void			SetText( TCHAR const* sz, int nWidth );
		//virtual void	SetEnable(bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual void	SetVisible(bool bValue);

		
		cText		s_Text;
		CsPoint		s_ptDeltaPos;
		NiColor		s_backupColor;
	};
	struct sDATA_STR : public sELEMENT
	{
		CSGBMEMPOOL_H( cString::sDATA_STR );
		sDATA_STR();
		~sDATA_STR();

		virtual eTYPE	GetType();
		void			Delete();
		void			Render( CsPoint pos );
		void			RenderLimit( CsPoint pos );
		void			SetAlpha( float fAlpha );
		virtual void	SetColor( NiColor color );
		virtual void	ResetDevice();
		//virtual void	SetEnable(bool bValue, int nValue);
		virtual void	SetEnable( bool bEnable );

		int				s_nValue;
		TCHAR			s_szStr[ 64 ];
	};
	struct sSPRITE : public sELEMENT
	{
		// 스프라이트는 포인터만 연결
		CSGBMEMPOOL_H( cString::sSPRITE );
		
		sSPRITE();
		~sSPRITE();
		//virtual void	SetEnable( bool bValue , int nValue);
		virtual void	SetEnable( bool bEnable );
		
		virtual eTYPE	GetType();
		virtual void	SetColor( NiColor color );
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			ResetDevice();
		virtual void	SetVisible(bool bValue);
		void			ChangeTexture( char const * pTextureFileName, bool bUseWorkingFolder = true );

		virtual	CsPoint GetWorldPos() const
		{ 
			return s_RenderPos;
// 			if( s_pSprite )
// 				return s_pSprite->GetClient() + s_ptDeltaPos;
// 			return CsPoint::ZERO; 
		}
		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pSprite )
				return s_pSprite->GetRect();

			return CsRect::ZERO; 
		}

		bool		s_bChangeSize;
		CsPoint		s_ptDeltaPos;
		CsPoint		s_RenderPos;
		cSprite*	s_pSprite;	
		NiColor		s_backupColor;
	};

	struct sICON : public sELEMENT
	{
		CSGBMEMPOOL_H( cString::sICON );

		sICON();
		virtual ~sICON();

		//virtual void	SetEnable( bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		virtual void	SetColor( NiColor color );
		void			ChangeIcon( int nIconType, int nIndex1, int nIndex2 = 0, int nIndex3 = 0);
		void			ChangeSkillIcon( int nIconType, int nIndex1, int nIndex2 = 0, int nIndex3 = 0);
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		virtual void	SetVisible(bool bValue);
		void			SetCoolTimeSeq( CsCoolTimeSeq* pCoolTimeSeq );
		void			SetCoolTimeMode( bool bSetCoolTime );

		virtual void    OnMouseOver();
		virtual void    OnMouseLeave();

		virtual	CsPoint GetWorldPos() const
		{ 
			return s_ptLastPos; 
		}
		virtual	CsRect GetWorldRect() const 
		{ 
			CsRect rect;
			rect.left = s_ptLastPos.x;
			rect.top = s_ptLastPos.y;
			rect.right = s_ptLastPos.x + s_ptIconSize.x;
			rect.bottom = s_ptLastPos.y + s_ptIconSize.y;
			return rect;
		}

		bool			s_bVisible;
		bool			s_bCoolTimeMode;
		int				s_nIconType;
		int				s_nIndex_1;
		int				s_nIndex_2;
		int				s_nIndex_3;
		float			s_fAlpha;
		CsPoint			s_ptDeltaPos;
		CsPoint			s_ptIconSize;
		CsPoint			s_ptLastPos;
		CsRect			m_MouseOverCheckRect;
		CsCoolTimeSeq*	s_pCoolTimeSeq;

	};	
	struct sIMAGE : public sELEMENT
	{
		// 이미지는 포인터만 연결
		CSGBMEMPOOL_H( cString::sIMAGE );

		sIMAGE();
		~sIMAGE();
		//virtual void	SetEnable( bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		virtual void	SetColor( NiColor color );

		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			ResetDevice();
		void			SetImangeIndex( const int nIndex );
		virtual void	SetVisible(bool bValue);

		virtual	CsPoint GetWorldPos() const
		{ 
			if( s_pImage )
				return s_pImage->GetClient() + s_ptDeltaPos;
			return CsPoint::ZERO; 
		}

		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pImage )
				return s_pImage->GetRect();

			return CsRect::ZERO; 
		}

		CsPoint			s_ptImageSize;
		CsPoint			s_ptDeltaPos;
		cImage*			s_pImage;
		int				s_nImageIndex;
		NiColor			s_backupColor;
	};
	struct sCHECKBOX : public sELEMENT
	{
		// 이미지는 포인터만 연결
		CSGBMEMPOOL_H( cString::sCHECKBOX );

		sCHECKBOX();
		~sCHECKBOX();

		//virtual void	SetEnable( bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		virtual void	SetColor( NiColor color );

		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			ResetDevice();
		virtual void	SetVisible(bool bValue);

		virtual void    OnMouseOver();
		virtual void    OnMouseLeave();
		virtual void    OnMouseDown();
		virtual void    OnMouseUp();
		virtual void    onMousePressed();

		virtual	CsPoint GetWorldPos() const
		{ 
			if( s_pCheckBox )
				return s_pCheckBox->GetClient();
			return CsPoint::ZERO; 
		}

		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pCheckBox )
				return s_pCheckBox->GetRect();

			return CsRect::ZERO; 
		}

		CsPoint			s_ptDeltaPos;
		cCheckBox*		s_pCheckBox;
		int				s_nCheckBoxIndex;
		NiColor			s_backupColor;
	};

	struct sMONEYIF : public sELEMENT
	{
		sMONEYIF();
		~sMONEYIF();
		// 돈은 메모리 할당 및 삭제
		//virtual void	SetEnable( bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			SetMoney( u8 nMoney );
		void			SetColor( NiColor color );
		virtual void	SetVisible(bool bValue);
		virtual void	ResetDevice();

		virtual	CsPoint GetWorldPos() const
		{ 
			if( s_pMoney )
				return s_pMoney->GetClient();
			return CsPoint::ZERO; 
		}

		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pMoney )
				return s_pMoney->GetRect();

			return CsRect::ZERO; 
		}

		CsPoint			s_ptDeltaPos;
		cWindow*		s_pMoney;
		NiColor			s_backupColor;
	};

	struct sBUTTON: public sELEMENT
	{
		//버튼은 포인터만 연결
		CSGBMEMPOOL_H( cString::sBUTTON );
		sBUTTON();
		~sBUTTON();

		//virtual void	SetEnable( bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			SetColor( NiColor color );	
		virtual void	SetVisible(bool bValue);
		virtual void	SetOnMode(bool bValue);	
		virtual void	ResetDevice();

		virtual	CsPoint GetWorldPos() const
		{ 
			if( s_pButton )
				return s_pButton->GetClient();
			return CsPoint::ZERO; 
		}

		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pButton )
				return s_pButton->GetRect();

			return CsRect::ZERO; 
		}

		int				GetValue() const;
		void			SetValue( int nValue);

		virtual void    OnMouseOver();
		virtual void    OnMouseLeave();
		virtual void    OnMouseDown();
		virtual void    OnMouseUp();
		virtual void    onMousePressed();
		
		CsPoint			s_ptDeltaPos;
		CsPoint			s_ptButtonSize;
		cButton*		s_pButton;
		int				s_nValue1;
		CsPoint			m_ptTextPos;
	};

	struct sANIBUTTON: public sELEMENT
	{
		//버튼은 포인터만 연결
		CSGBMEMPOOL_H( cString::sANIBUTTON );
		sANIBUTTON();
		~sANIBUTTON();

		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		virtual void	Update( float const& fDeltaTime );
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			SetColor( NiColor color );	
		virtual void	SetVisible(bool bValue);
		virtual void	ResetDevice();

		virtual	CsPoint GetWorldPos() const
		{ 
			if( s_pButton )
				return s_pButton->GetClient();
			return CsPoint::ZERO; 
		}

		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pButton )
				return s_pButton->GetRect();

			return CsRect::ZERO; 
		}

		int				GetValue() const;
		void			SetValue( int nValue);

		virtual void    OnMouseOver();
		virtual void    OnMouseLeave();
		virtual void    OnMouseDown();
		virtual void    OnMouseUp();
		virtual void    onMousePressed();

		CsPoint			s_ptDeltaPos;
		CsPoint			s_ptButtonSize;
		cAniButton*		s_pButton;
		int				s_nValue1;
	};

	struct sPROGRESS: public sELEMENT
	{
		//버튼은 포인터만 연결
		CSGBMEMPOOL_H( cString::sPROGRESS );
		sPROGRESS();
		~sPROGRESS();

		//virtual void	SetEnable( bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			SetColor( NiColor color );		
		virtual void	SetVisible(bool bValue);
		virtual void	ResetDevice();
		virtual void    OnMouseOver();
		virtual void    OnMouseLeave();
		virtual void    OnMouseDown();
		virtual void    OnMouseUp();
		virtual void    onMousePressed();

		virtual	CsPoint GetWorldPos() const
		{ 
			if( s_pProgressbar )
				return s_pProgressbar->GetClient();
			return CsPoint::ZERO; 
		}

		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pProgressbar )
				return s_pProgressbar->GetRect();

			return CsRect::ZERO; 
		}

		bool			s_bChangeSize;
		CsPoint			s_ptDeltaPos;
		cProgressBar*	s_pProgressbar;
	};

	struct sTIMERPROGRESS: public sELEMENT
	{
		//버튼은 포인터만 연결
		CSGBMEMPOOL_H( cString::sTIMERPROGRESS );
		sTIMERPROGRESS();
		~sTIMERPROGRESS();

		//virtual void	SetEnable( bool bValue, int nValue );
		virtual void	SetEnable( bool bEnable );
		virtual eTYPE	GetType();
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			SetColor( NiColor color );		
		virtual void	SetVisible(bool bValue);
		virtual void	Update( float const& fDeltaTime );
		virtual void	ResetDevice();
		virtual void    OnMouseOver();
		virtual void    OnMouseLeave();
		virtual void    OnMouseDown();
		virtual void    OnMouseUp();
		virtual void    onMousePressed();

		virtual	CsPoint GetWorldPos() const
		{ 
			if( s_pTimerProgress )
				return s_pTimerProgress->GetClient();
			return CsPoint::ZERO; 
		}

		virtual	CsRect GetWorldRect() const 
		{ 
			if( s_pTimerProgress )
				return s_pTimerProgress->GetRect();

			return CsRect::ZERO; 
		}

		void			Start();
		void			Stop();
		void			Reset();
		void			SetTimer( float fCurTime, float fTotalTime );
		bool			IsPlay() const;

		bool			s_bChangeSize;
		CsPoint			s_ptDeltaPos;
		cUI_TimerProgressBar*	s_pTimerProgress;
	};

public:
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	enum { INVALIDE_STRING_VALUE = INT_MAX, };
	cString(){ _Reset(); }
	~cString(){	Delete(); }

	void		Delete();
	void		DeleteElement( int nIndex );
	void		ResetDevice();
	sTEXT*		AddText( cText::sTEXTINFO* pTextInfo, CsPoint deltaPos = CsPoint::ZERO );
	sTEXT*		AddText( NiStencilProperty* pPropStencil, cText::sTEXTINFO* pTextInfo, CsPoint deltaPos = CsPoint::ZERO );	
	void		AddText_Head( cText::sTEXTINFO* pTextInfo );

	void		SetText( int nElementIndex, TCHAR const* sz );

	sSPRITE*	AddSprite( cSprite* pSprite, CsPoint deltaPos = CsPoint::ZERO, CsPoint ptSize = CsPoint::ZERO );
	sPROGRESS*	AddProgress( cProgressBar* pProgress, CsPoint deltaPos = CsPoint::ZERO, CsPoint ptSize = CsPoint::ZERO );
	sTIMERPROGRESS*	AddTimerProgress( cUI_TimerProgressBar* pTimerProgress, CsPoint deltaPos = CsPoint::ZERO );
	sSPRITE*	AddSprite_Head( cSprite* pSprite, CsPoint deltaPos = CsPoint::ZERO, CsPoint ptSize = CsPoint::ZERO );
	sICON*		AddIcon( CsPoint size, int nIconType, int nIndex1, int nIndex2=0, CsPoint deltaPos = CsPoint::ZERO );
	sICON*		AddSkillIcon( CsPoint size, int nIconType, int nIndex1, int nIndex2=0, CsPoint deltaPos = CsPoint::ZERO );
	sICON*		AddIcon_Head( CsPoint size, int nIconType, int nIndex1, int nIndex2=0, CsPoint deltaPos = CsPoint::ZERO );
	sIMAGE*		AddImage( cImage* pImage, int nImageIndex, CsPoint deltaPos = CsPoint::ZERO, CsPoint imageSize = CsPoint::ZERO );
	sIMAGE*		AddImage_Head( cImage* pImage, int nImageIndex, CsPoint deltaPos = CsPoint::ZERO, CsPoint imageSize = CsPoint::ZERO );
	sCHECKBOX*	AddCheckBox( char* cCheckBoxPath, bool bCheck, CsPoint TexToken, CsPoint CheckBoxSize, CsPoint deltaPos = CsPoint::ZERO );
	sMONEYIF*	AddMoneyIF( int nDisplayType, u8 nMoney, NiColor color, CFont::eFACE_SIZE eFontSize = CFont::FS_9, int eAllign = DT_RIGHT, CsPoint deltaPos = CsPoint::ZERO, bool bShowBG = false, DWORD dwItemID = 0 );
	void		AddData_Str( int nValue, TCHAR* strBuffer, int nBufferSize );
	sBUTTON*	AddButton( cButton* pButton, int nButtonValue, CsPoint deltaPos = CsPoint::ZERO, CsPoint ptSize = CsPoint::ZERO, CsPoint textPos = CsPoint::ZERO);
	sBUTTON*	AddButton_Head( cButton* pButton, int nButtonValue, CsPoint deltaPos = CsPoint::ZERO, CsPoint ptSize = CsPoint::ZERO );
	sANIBUTTON*	AddAniButton( cAniButton* pAniButton, int nButtonValue, CsPoint deltaPos = CsPoint::ZERO, CsPoint ptSize = CsPoint::ZERO );

	void		AddPropertyPrice( DWORD dwNpcID, std::vector<int>& vecItemID, std::vector<int>& vecItemPrice );
	sELEMENT*	Update_ForMouse_Control( CsPoint pos, cString::sELEMENT** ppActiveElement = NULL );	
	sELEMENT*	Update_ForMouseOver_Control( CsPoint pos );

	int		Render_Overlab( CsPoint pos );
	int		Render_Overlab( CsPoint pos, UINT Align );

	int		Render( CsPoint pos );
	int		RenderLimit( CsPoint pos );
	int		Render( CsPoint pos, UINT Align );
	int		RenderLimit( CsPoint pos, UINT Align );
	void	SetAlpha( float fAlpha /*부하가적다*/ );
	void	SetColor( NiColor color );
	CsPoint	GetWorldPos() const;
	virtual void	Update( float const& fDeltaTime );

	virtual void    OnMouseOver();
	virtual void    OnMouseLeave();
	virtual void    OnMouseDown();
	virtual void    OnMouseUp();
	virtual void    onMousePressed();

	virtual void    SetControlOnMode(bool bValue);

protected:
	void	_Reset(){ m_nMaxSize = CsPoint::ZERO; m_nValue1 = m_nValue2 = m_nValue3 = m_nValue4 = INVALIDE_STRING_VALUE; m_pValuePointer1 = NULL; }

protected:
	std::list< sELEMENT* >	m_listText;
	CsPoint					m_nMaxSize;
	int						m_nValue1;
	int						m_nValue2;
	int						m_nValue3;
	int						m_nValue4;
	void*					m_pValuePointer1;

public:
	void		CalMaxSize();
	void		CalMaxVSize();
	CsPoint		GetMaxSize();
	void		SetMaxSize( CsPoint ptMaxSize );

	void		SetValue1( int nValue1 );
	int			GetValue1();
	void		SetValue2( int nValue2 );
	int			GetValue2();
	void		SetValue3( int nValue3 );
	int			GetValue3();
	void		SetValue4( int nValue4 );
	int			GetValue4();

	void		SetValuePointer1( void* pValuePointer );
	void*		GetValuePointer1();

	void		AddSizeY( int nSizeY );
	void		SetSizeY( int nSizeY );

	void		TailAddSizeX( int nSizeX );
	void		HeadAddSizeX( int nSizeX );

	int			GetListSize();
	//void		SetEnable(bool bValue, int nValue);
	void		SetEnable(bool bValue);
	bool		GetEnabled() const;

	std::list< sELEMENT* >::iterator	Get_sTextList() { return m_listText.begin(); }

	// Find
public:
	sELEMENT*	GetElement( int nIndex );
	int			GetListIndex( void* pPointer );
	bool		IsText( TCHAR* szText );
	bool		IsIncludeText( std::wstring const& findText, bool btolower );
	sELEMENT*	FindElement( int nElementType );
	void		FindElement( int nElementType, std::list<sELEMENT*>& rList );
};
 
class cStringList : public NiMemObject
{
public:
	enum{ INVALIDE_FIND = INT_MAX, };	

public:
	cStringList();
	~cStringList();

public:
	void		Delete( bool bDeleteUnderLine = true );
	void		ResetDevice();
	//void		SetEnable( bool bValue );

	int					Update_ForMouse( cString** ppSelect, int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, CsSIZE CheckSize );
	int					Update_ForMouse( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, CsSIZE CheckSize );
	cString::sELEMENT*	Update_ForMouse_Control( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, cString::sELEMENT** ppActiveElement = NULL );

	void		Render_Overlab( CsPoint pos, UINT Align = DT_LEFT );
	void		Render_Overlab_RenderText( CsPoint pos, UINT Align = DT_LEFT );

	void		Render( CsPoint pos, int nDeltaY, UINT Align = DT_LEFT );
	void		RenderEnd( CsPoint pos, int nDeltaY, UINT Align = DT_LEFT );
	void		Render_LineHeight( CsPoint pos );
	void		RenderLimit( CsPoint pos, int nDeltaY, UINT Align = DT_LEFT );
	void		SetAlpha( float fAlpha /*부하가적다*/ );	

public:
	int			GetSize(){ return (int)m_list.size(); }
	void		AddHead( cString* pString );
	void		AddTail( cString* pString );
	void		AddTailEmpty( CFont::eFACE_SIZE eFontSize);
	void		AddIndexAfter( cString* pString, int nIndex );

	cString*	GetString( int nIndex );
	int			GetStringPosY( int nStartY, int nIndex, int nDeltaY );
	CsPoint		GetLastPos( int nDeltaY );
	cString*	GetTail();
	void		RemoveHead();
	void		RemoveTail();
	void		RemoveIndex( int nIndex );
	void		DisableIndex( int nIndex );

	CsPoint		CalMaxSize( int nDeltaY );
	void		SetSameMaxWidth( int nWidthX );
	void		SetVisible( bool bVisible );

	
	std::list< cString* >*	GetList(){ return &m_list; }

	//====================================================================================================
	//
	//		Find
	//
	//====================================================================================================
public:
	int			FindText( TCHAR* szText );
	bool		IsIncludeText( std::wstring const& findText, bool btolower );

public:
	void		SetStrInfo( sSTR_INFO* pInfo ){ m_StrInfo = *pInfo; }
	sSTR_INFO*	GetStrInfo(){ return &m_StrInfo; }

protected:
	sSTR_INFO				m_StrInfo;
	std::list< cString* >	m_list;

	bool					m_bIsVisible;

	//====================================================================================================
	//
	//		Buddy
	//
	//====================================================================================================
protected:
	cScrollBar*		m_pScrollBar;

public:
	void		SetBuddy( cScrollBar* pScrollBar );

	//====================================================================================================
	//
	//		UnderLineCursor
	//
	//====================================================================================================
protected:
	cText*		m_pUnderLineCursor;
	float		m_fCursorTime;

public:
	void		SetUnderLineCursor( NiColor color = NiColor::WHITE );
	void		ReleaseCursor() { SAFE_NIDELETE( m_pUnderLineCursor ); }

	std::list< cString* >::iterator Get_StringList() { return m_list.begin(); }
};

