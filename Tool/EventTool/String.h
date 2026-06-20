
#pragma once

class cMoneyIF;
class cString : public NiMemObject
{
public:
	struct sELEMENT : public NiMemObject
	{
		enum eTYPE{ TEXT, SPRITE, ICON, IMAGE, CHECK_BOX, MONEYIF, DATA_STR, };

		virtual eTYPE	GetType() = 0;
		virtual void	Delete() = 0;
		virtual void	Render( CsPoint pos ) = 0;
		virtual void	RenderLimit( CsPoint pos ){ Render( pos ); }
		virtual void	SetAlpha( float fAlpha ) = 0;
		virtual void	SetColor( NiColor color ){}
		virtual void	ResetDevice(){}

		CsPoint			s_ptSize;
	};
	struct sTEXT : public sELEMENT
	{
		CSGBMEMPOOL_H( cString::sTEXT );

		virtual eTYPE	GetType(){ return TEXT; }
		void			Delete(){ s_Text.Delete(); }
		void			Render( CsPoint pos ){ s_Text.Render( pos ); }
		void			RenderLimit( CsPoint pos ){ s_Text.RenderLimit( pos ); }
		void			SetAlpha( float fAlpha ){ s_Text.SetAlpha( fAlpha ); }
		virtual void	SetColor( NiColor color ){ s_Text.SetColor( color ); }
		virtual void	ResetDevice(){ s_Text.ResetDevice(); }
		
		cText		s_Text;
	};
	struct sDATA_STR : public sELEMENT
	{
		CSGBMEMPOOL_H( cString::sDATA_STR );

		virtual eTYPE	GetType(){ return DATA_STR; }
		void			Delete(){}
		void			Render( CsPoint pos ){}
		void			RenderLimit( CsPoint pos ){}
		void			SetAlpha( float fAlpha ){}
		virtual void	SetColor( NiColor color ){}
		virtual void	ResetDevice(){}

		int				s_nValue;
		TCHAR			s_szStr[ 64 ];
	};
	struct sSPRITE : public sELEMENT
	{
		// 스프라이트는 포인터만 연결
		CSGBMEMPOOL_H( cString::sSPRITE );

		virtual eTYPE	GetType(){ return SPRITE; }
		void			Delete(){ s_pSprite = NULL; }
		void			Render( CsPoint pos ){ (s_bChangeSize == false) ?  s_pSprite->Render( pos + s_ptDeltaPos ) : s_pSprite->Render( pos + s_ptDeltaPos, s_ptSize ); }
		void			SetAlpha( float fAlpha ){ s_pSprite->SetAlpha( fAlpha ); }
		void			ResetDevice(){ s_pSprite->ResetDevice(); }

		bool		s_bChangeSize;
		CsPoint		s_ptDeltaPos;
		cSprite*	s_pSprite;
	};
	/*
	struct sICON : public sELEMENT
	{
		CSGBMEMPOOL_H( cString::sICON );

		virtual eTYPE	GetType(){ return ICON; }
		void			Delete(){}
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha ){ s_fAlpha = fAlpha; }

		int				s_nIconType;
		int				s_nIndex_1;
		int				s_nIndex_2;
		int				s_nIndex_3;
		float			s_fAlpha;
		CsPoint			s_ptDeltaPos;
		CsPoint			s_ptIconSize;
	};		
	struct sIMAGE : public sELEMENT
	{
		// 이미지는 포인터만 연결
		CSGBMEMPOOL_H( cString::sIMAGE );

		virtual eTYPE	GetType(){ return IMAGE; }
		void			Delete(){  }
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha ){ s_pImage->SetAlpha( fAlpha ); }
		void			ResetDevice(){ s_pImage->ResetDevice(); }

		CsPoint			s_ptImageSize;
		CsPoint			s_ptDeltaPos;
		cImage*			s_pImage;
		int				s_nImageIndex;
	};
	struct sCHECKBOX : public sELEMENT
	{
		// 이미지는 포인터만 연결
		CSGBMEMPOOL_H( cString::sCHECKBOX );

		sCHECKBOX::sCHECKBOX(){ s_pCheckBox = NULL; }

		virtual eTYPE	GetType(){ return CHECK_BOX; }

		void			Delete(){ SAFE_NIDELETE( s_pCheckBox ); }
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha ){ s_pCheckBox->SetAlpha( fAlpha ); }
		void			ResetDevice(){ s_pCheckBox->ResetDevice(); }

		CsPoint			s_ptDeltaPos;
		cCheckBox*		s_pCheckBox;
		int				s_nCheckBoxIndex;
	};
	struct sMONEYIF : public sELEMENT
	{
		// 돈은 메모리 할당 및 삭제
		virtual eTYPE	GetType(){ return MONEYIF; }
		void			Delete();
		void			Render( CsPoint pos );
		void			SetAlpha( float fAlpha );
		void			SetColor( NiColor color );
		virtual void	ResetDevice(){ s_pMoney->ResetDevice(); }

		CsPoint			s_ptDeltaPos;
		cWindow*		s_pMoney;
	};
	*/

public:
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	enum { INVALIDE_STRING_VALUE = INT_MAX, };
	cString(){ _Reset(); }
	~cString(){	Delete(); }

	void		Delete();
	void		ResetDevice();
	sTEXT*		AddText( cText::sTEXTINFO* pTextInfo );
	void		AddText_Head( cText::sTEXTINFO* pTextInfo );	
	sSPRITE*	AddSprite( cSprite* pSprite, CsPoint deltaPos = CsPoint::ZERO, CsPoint ptSize = CsPoint::ZERO );
/*
	sICON*		AddIcon( CsPoint size, int nIconType, int nIndex1, int nIndex2=0, CsPoint deltaPos = CsPoint::ZERO, CsPoint IconSize = CsPoint::ZERO );
	sIMAGE*		AddImage( cImage* pImage, int nImageIndex, CsPoint deltaPos = CsPoint::ZERO, CsPoint imageSize = CsPoint::ZERO );
	void		AddImage_Head( cImage* pImage, int nImageIndex, CsPoint deltaPos = CsPoint::ZERO );
	sCHECKBOX*	AddCheckBox( char* cCheckBoxPath, bool bCheck, CsPoint TexToken, CsPoint CheckBoxSize, CsPoint deltaPos = CsPoint::ZERO );
	void		AddMoneyIF( int nDisplayType, u8 nMoney, NiColor color, int nFontSize = CFont::FS_9, int eAllign = DT_RIGHT, CsPoint deltaPos = CsPoint::ZERO );
*/
	void		AddData_Str( int nValue, TCHAR* strBuffer, int nBufferSize );

	sELEMENT*	Update_ForMouse_Control( CsPoint pos );

	int		Render( CsPoint pos );
	int		RenderLimit( CsPoint pos );
	int		Render( CsPoint pos, UINT Align );
	int		RenderLimit( CsPoint pos, UINT Align );
	void	SetAlpha( float fAlpha /*부하가적다*/ );
	void	SetColor( NiColor color );

protected:
	void	_Reset(){ m_nMaxSize = CsPoint::ZERO; m_nValue1 = m_nValue2 = INVALIDE_STRING_VALUE; }

protected:
	std::list< sELEMENT* >	m_listText;
	CsPoint					m_nMaxSize;
	int						m_nValue1;
	int						m_nValue2;
	int						m_nValue3;	

public:
	void		CalMaxSize();
	CsPoint		GetMaxSize(){ return m_nMaxSize; }	

	void		SetValue1( int nValue1 ){ m_nValue1 = nValue1; }
	int			GetValue1(){ return m_nValue1; }
	void		SetValue2( int nValue2 ){ m_nValue2 = nValue2; }
	int			GetValue2(){ return m_nValue2; }
	void		SetValue3( int nValue3 ){ m_nValue3 = nValue3; }
	int			GetValue3(){ return m_nValue3; }

	void		AddSizeY( int nSizeY ){ m_nMaxSize.y += nSizeY; }
	void		SetSizeY( int nSizeY ){ m_nMaxSize.y = nSizeY; }

	void		TailAddSizeX( int nSizeX );

	int			GetListSize(){ return (int)m_listText.size(); }

	// Find
public:
	sELEMENT*	GetElement( int nIndex );
	bool		IsText( TCHAR* szText );
	sELEMENT*	FindElement( int nElementType );
};

class cStringList : public NiMemObject
{
public:
	enum{ INVALIDE_FIND = INT_MAX, };

public:
	cStringList(){ /*m_pScrollBar = NULL;*/ }
	~cStringList(){	Delete(); }

public:
	void		Delete();
	void		ResetDevice();

	int					Update_ForMouse( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, CsSIZE CheckSize );
	cString::sELEMENT*	Update_ForMouse_Control( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY );

	void		Render( CsPoint pos, int nDeltaY, UINT Align = DT_LEFT );
	void		RenderLimit( CsPoint pos, int nDeltaY, UINT Align = DT_LEFT );
	void		SetAlpha( float fAlpha /*부하가적다*/ );

public:
	int			GetSize(){ return (int)m_list.size(); }
	void		AddHead( cString* pString );
	void		AddTail( cString* pString );
	void		AddTailEmpty( int nFontSize);
	void		AddIndexAfter( cString* pString, int nIndex );

	cString*	GetString( int nIndex );
	int			GetStringPosY( int nStartY, int nIndex, int nDeltaY );
	cString*	GetTail();
	void		RemoveHead();
	void		RemoveTail();

	CsPoint		CalMaxSize( int nDeltaY );

	
	std::list< cString* >*	GetList(){ return &m_list; }


	//====================================================================================================
	//
	//		Find
	//
	//====================================================================================================
public:
	int			FindText( TCHAR* szText );


public:
	std::list< cString* >	m_list;


/*
	//====================================================================================================
	//
	//		Buddy
	//
	//====================================================================================================
protected:
	cScrollBar*		m_pScrollBar;

public:
	void		SetBuddy( cScrollBar* pScrollBar );
	*/
};

