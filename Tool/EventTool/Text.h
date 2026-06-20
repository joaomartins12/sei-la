
#pragma once 

class cText : public NiMemObject
{
public:
	struct sTEXTINFO
	{
		enum eBOLD_LEVEL{ BL_NONE, BL_1, BL_2, BL_3, BL_4, BL_5, };

		// 초기화
		void	Init( CFreeFont* pFont, int nFontSize = CFreeFont::FS_10, NiColor color = NiColor::WHITE );

		bool			s_bOutLine;

		NiColor			s_Color;
		UINT			s_eTextAlign;

		// 폰트
	public:
		CFreeFont*			s_pFont;
		int				s_nFontSize;
	public:
		FT_Face			GetFace(){ return s_pFont->GetFace( s_nFontSize ); }
		int				GetHeight(){ return s_pFont->GetHeight( s_nFontSize ); }

		// 텍스트
	protected:
		TCHAR			s_szText[ MAX_PATH ];
	public:
		void			SetText( TCHAR* szText ){ _tcscpy_s( s_szText, MAX_PATH, szText ); }
		void			SetText( int nText ){ _stprintf_s( s_szText, MAX_PATH, L"%d", nText ); }
		TCHAR*			GetText(){ return s_szText; }

		// 볼드
	protected:
		eBOLD_LEVEL		s_eBoldLevel;
	public:
		void			SetBoldLevel( eBOLD_LEVEL Level ){ s_eBoldLevel = Level; }
		eBOLD_LEVEL		GetBoldLevel(){ return s_eBoldLevel; }
		int				GetBoldSize();
	};

	//=======================================================================================
	//
	//		생성자, 소멸자
	//
	//=======================================================================================
public:
	cText();	
	virtual ~cText(){ Delete(); }

	//=======================================================================================
	//
	//		베이스
	//
	//=======================================================================================
public:
	void		Delete();

	void		Init( cWindow* pParent, CsPoint pos, sTEXTINFO* pText, bool bApplyWindowSize );
	void		Render( CsPoint pos, UINT Align );
	void		Render( CsPoint pos );
	void		Render();
	void		RenderLimit( CsPoint pos, UINT Align );
	void		RenderLimit( CsPoint pos );

	//=======================================================================================
	//
	//		포인터
	//
	//=======================================================================================
public:
	sTEXTINFO*			GetTextInfo(){ return &m_TextInfo; }

	//=======================================================================================
	//
	//		스트링
	//
	//=======================================================================================
protected:
	bool				m_bUseMark;
	TCHAR				m_szMark;	

	CsPoint				m_ptStrSize;

	int					m_nFTSize_HBase;
	cSprite				m_Sprite;
	sTEXTINFO			m_TextInfo;


public:
	bool				GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr );
	CsPoint				GetStringSize(){ return m_ptStrSize; }

	void				SetColorA( NiColorA FontColor /*부하가적다*/ ){ m_Sprite.SetColorA( FontColor ); }
	virtual void		SetColor( NiColor FontColor /*부하가적다*/ ){ m_Sprite.SetColor( FontColor ); }
	NiColor				GetColor(){ return m_Sprite.GetColor(); }
	virtual void		SetAlpha( float fAlpha ){ m_Sprite.SetAlpha( fAlpha ); }

	bool				SetText( TCHAR* szStr /*부하가크다*/ );	
	bool				SetText( int nStr );
	void				SetMark( TCHAR szMark ){ m_szMark = szMark; m_bUseMark = true; }

	//int					GetFontSize(){ return m_nFontSize; }
	//bool				IsOutLine(){ return m_bOutLine; }

	//void				SetTextAlign( UINT nCode ){ m_TextAlign = nCode; }
	//UINT				GetTextAlign(){ return m_TextAlign; }

	cSprite*			GetSprite(){ return &m_Sprite; }
	bool				IsEnableTexture(){ return ( m_pTexture != NULL ); }

public:
	static int			GetCharWidth( FT_Face face, FT_ULong charcode );
	

	//=======================================================================================
	//
	//		텍스쳐
	//
	//=======================================================================================
protected:
	NiSourceTexturePtr	m_pTexture;
	NiPixelDataPtr		m_pPixelData;
	BYTE*				m_pFTData;

protected:
	bool				_AllocData();	
	void				_FTBmpToFTData();
	void				_ReadFTBmp( FT_Bitmap* bitmap, int x, int y, int sx, int sy );

	void				_CreateTexture();	
	void				_StringToData_OutLine();
	void				_StringToData();

public:
	virtual void		ResetDevice(){ m_Sprite.ResetDevice(); }
};

/*
class cText3D : public cText
{
public:
	cText3D();
	virtual ~cText3D();

protected:
	CBillboard*	m_pBillBoardText;

public:
	bool		Init3D( cText::sTEXTINFO* pTextInfo );

	void		SetAlpha( float fAlpha ){ if( m_pBillBoardText ){ m_pBillBoardText->SetAlpha( fAlpha ); } }
	void		SetColor( NiColor FontColor ){ if( m_pBillBoardText ){ m_pBillBoardText->SetColor( FontColor ); } }

	void		Render( NiPoint3 vPos, float fX, float fY );
	void		Render( NiPoint3 vPos, float fX, float fY, float fScale );
};
*/