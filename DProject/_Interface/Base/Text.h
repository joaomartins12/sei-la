
#pragma once 

struct sSTR_INFO
{
	enum 
	{
		SF_NONE				=	0x00000000,

		SF_LINE_HEIGHT		=	0x00000001,		
		SF_LINE_DELTA		=	0x00000002,
		_SF_LINE_MASK		=	0x0000000f,

		SF_BASE_HEIGHT		=	0x00000010,
		_SF_BASE_MASK		=	0x000000f0,

		SF_STENCIL_USE_1	=	0x00000100,
		_SF_STENCIL_MASK	=	0x00000f00,

		SF_NORMAL			=	0x00000020,
	};

	sSTR_INFO(){ s_dwPlag = SF_NONE; }

	int			s_nLineHeight;
	DWORD		s_dwPlag;


	DWORD		GetBasePlag(){ return ( s_dwPlag&_SF_BASE_MASK ); }
	DWORD		GetLinePlag(){ return ( s_dwPlag&_SF_LINE_MASK ); }
	DWORD		GetStencilPlag(){ return ( s_dwPlag&_SF_STENCIL_MASK ); }
};


class cText : public NiMemObject
{
public:
	struct sTEXTINFO
	{
	public:
		sTEXTINFO();
		~sTEXTINFO();

		enum eBOLD_LEVEL{ BL_NONE, BL_1, BL_2, BL_3, BL_4, BL_5, };

		// 초기화
		void	Init( CFont* pFont, CFont::eFACE_SIZE eFontSize = CFont::FS_10, NiColor color = FONT_WHITE );
		void	Init( CFont::eFACE_SIZE eFontSize = CFont::FS_10, NiColor color = FONT_WHITE );
		void	Init( CsHelp::sTEXT* pHelpText );

		FT_Face						GetFace() const;
		int							GetHeight() const;
		int							GetHBase() const;
		int							GetGabHeight() const;
		void						SetLineGabHeight(int const& nHeight);

		void						SetText( TCHAR const* szText );
		cText::sTEXTINFO&			AddString( TCHAR const* szText );
		cText::sTEXTINFO&			SetString( TCHAR const* szText );
		cText::sTEXTINFO&			AddSpace();
		cText::sTEXTINFO&			AddNewLine();

		void						SetText( int nText );
		void						SetText_Reduce( TCHAR const* szText, int nLen, int nDotNum = 2 );
		const TCHAR*				GetText() const;
		int							GetLen() const;
		void						AddText( TCHAR const* szTex);

		void						SetBoldLevel( eBOLD_LEVEL Level );
		eBOLD_LEVEL					GetBoldLevel() const; 
		int							GetBoldSize() const;

		int							GetTextWidth();

		NiColor						s_Color;
		wstring						s_strText;
		bool						s_bOutLine;
		UINT						s_eTextAlign;
		CFont*						s_pFont;
		CFont::eFACE_SIZE			s_eFontSize;
		eBOLD_LEVEL					s_eBoldLevel;	// 볼드
		int							s_nLineGabHeight;
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
	void		InitStencil( cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize, NiStencilProperty* pPropStencil );

	void		SetPos( CsPoint ptPos );
	CsPoint		GetPos();

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
	const TCHAR*		GetText(){ return m_TextInfo.GetText(); }
	int					GetTextLen() { return m_TextInfo.GetLen(); }

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
	void				SetVisible( bool bValue );
	bool				GetVisible() const;

	static	void		GetStringSize( sTEXTINFO* TextInfo, CsPoint& size, int& hBase, const TCHAR* szStr );
	bool				GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr );
	bool				GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr, std::wstring& wsResult, int nWidth );
	CsPoint				GetStringSize(){ return m_ptStrSize; }

	void				SetColorA( NiColorA FontColor /*부하가적다*/ ){ m_TextInfo.s_Color = NiColor( FontColor.r, FontColor.g, FontColor.b ); m_Sprite.SetColorA( FontColor ); }
	virtual void		SetColor( NiColor FontColor /*부하가적다*/ ){ m_TextInfo.s_Color = FontColor; m_Sprite.SetColor( FontColor ); }
	NiColor				GetColor(){ return m_TextInfo.s_Color; }
	virtual void		SetAlpha( float fAlpha ){ m_Sprite.SetAlpha( fAlpha ); }

	virtual bool		SetText( TCHAR const* szStr /*부하가크다*/ );	
	virtual bool		SetText( TCHAR const* szStr /*부하가크다*/, int nWidth );	
	
	bool				SetText( int nStr );
	void				SetMark( TCHAR szMark ){ m_szMark = szMark; m_bUseMark = true; }

	void				SetFontSize(CFont::eFACE_SIZE FontSize);	//2017-02-16-nova 폰트사이즈 변경

	cSprite*			GetSprite(){ return &m_Sprite; }
	bool				IsEnableTexture(){ return ( m_pTexture != NULL ); }

	CsPoint				GetSpritePos(){ return m_Sprite.GetTempPos(); }

	sTEXTINFO			Get_TextInfo() { return m_TextInfo; }

public:
	static int			GetCharWidth( FT_Face face, FT_ULong charcode );
	static int			GetStringWidth( FT_Face face, const std::wstring& str); // [4/21/2016 hyun] 추가. 문자열의 넓이를 가져온다
	

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
	bool				_AllocData(int nWidth);	
	void				_FTBmpToFTData();
	void				_FTBmpToFTData_MultiLine();
	void				_ReadFTBmp( FT_Bitmap* bitmap, int x, int y, int sx, int sy );

	void				_CreateTexture();	
	void				_StringToData_OutLine();
	void				_StringToData();

public:
	virtual void		ResetDevice(){ m_Sprite.ResetDevice(); }
};


//=======================================================================================
//
//
//
//		타이머 텍스트
//
//
//
//=======================================================================================
// class cTimerText : public cText
// {
// public:
// 	cTimerText();
// 	~cTimerText();
// 
// 	enum eSHOW_TIME_TYPE{ FULL_TIME, SIMPLE_TIME, SYMBOL_TIME};
// 
// 	void Update( float fEp );
// 
// private:
// 	unsigned int	m_CurrentTime;
// 	unsigned int	m_StartTime;
// 	unsigned int	m_EndTime;
// 
// 	bool			m_bTimerStop;
// 	bool			m_bEndTimer;
// 	eSHOW_TIME_TYPE m_TimerType;
// 
// 	int				m_nCurrent_nSec;
// 	int				m_nCurrent_nMin;
// 	int				m_nCurrent_nHour;
// 	int				m_nCurrent_nDay;
// };


//=======================================================================================
//
//
//
//		3D 텍스트
//
//
//
//=======================================================================================

class cText3D : public cText
{
public:
	cText3D();
	virtual ~cText3D();

	struct sBillboard : public NiMemObject
	{
	public:
		sBillboard();
		~sBillboard();

		void		Delete(){ SAFE_NIDELETE( s_pBillboard ); }
		void		Init( char const* pFileName, NiPoint2 DeltaPos, NiPoint2 ptSize );
		void		Init( char const* pFileName, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiPoint2 DeltaPos, NiPoint2 ptSize );
		void		Render( NiPoint3 vPos, float& fX, float& fY );
		void		Render( NiPoint3 vPos, float& fX, float& fY, float fScale );
		
		NiPoint2				s_Size;
		NiPoint2				s_DeltaPos;
		CBillboard*				s_pBillboard;
	};

protected:
	CBillboard*	m_pBillBoardText;
	CsVectorPB< sBillboard* > m_pbBillBoard;	

public:
	virtual	bool		SetText( int nStr );
	virtual bool		SetText( TCHAR const* szStr /*부하가크다*/ );	

public:
	void				DeleteBillboard();
	bool				Init3D( cText::sTEXTINFO* pTextInfo );

	void				SetAlpha( float fAlpha );
	void				SetColor( NiColor FontColor /*부하가적다*/ );

	void				Render( NiPoint3 vPos, float fX, float fY );
	void				Render( NiPoint3 vPos, float fX, float fY, float fScale );

	void				AddBillBoard( char const* pFileName, NiPoint2 DeltaPos, NiPoint2 ptSize );
	void				AddBillBoard( char const* pFileName, NiPoint2 DeltaPos, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiPoint2 ptSize );
	int					GetBillBoardCount() { return m_pbBillBoard.Size(); }

public:
	virtual void		ResetDevice() {}
};


class cMyText : public NiMemObject
{
public:
	struct sTEXTINFO
	{
		sTEXTINFO(){ s_pFont = NULL; }
		enum eBOLD_LEVEL{ BL_NONE, BL_1, BL_2, BL_3, BL_4, BL_5, };

		// 초기화
		void	Init( CFont* pFont, CFont::eFACE_SIZE eFontSize = CFont::FS_10, NiColor color = FONT_WHITE );
		void	Init( CFont::eFACE_SIZE eFontSize = CFont::FS_10, NiColor color = FONT_WHITE );
		void	Init( CsHelp::sTEXT* pHelpText );

		bool			s_bOutLine;

		NiColor			s_Color;
		UINT			s_eTextAlign;

		// 폰트
	public:
		CFont*				s_pFont;
		CFont::eFACE_SIZE	s_eFontSize;
	public:
		FT_Face			GetFace(){ return s_pFont->GetFace( s_eFontSize ); }
		int				GetHeight(){ return s_pFont->GetHeight( s_eFontSize ); }
		int				GetHBase(){ return s_pFont->GetHBase( s_eFontSize ); }

		// 텍스트
	protected:
		std::wstring	s_strText;
	public:		
		void			SetText( TCHAR* szText ) { s_strText = szText; }
		void			SetText( int nText ){ TCHAR sz[ 16 ] = {0, }; _stprintf_s( sz, 16, _T( "%d" ), nText ); s_strText = sz; }
		void			SetText_Reduce( TCHAR* szText, int nLen, int nDotNum = 2 );
		const TCHAR*	GetText(){ return s_strText.data(); }
		int				GetLen() { return (int)s_strText.length(); }

		void			AddText( TCHAR* szTex) { s_strText.append( szTex ); }

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
	cMyText();	
	virtual ~cMyText(){ Delete(); }

	//=======================================================================================
	//
	//		베이스
	//
	//=======================================================================================
public:
	void		Delete();

	void		Init( cWindow* pParent, CsPoint pos, CsPoint pixel, sTEXTINFO* pText, bool bApplyWindowSize );


	//=======================================================================================
	//
	//		포인터
	//
	//=======================================================================================
public:
	sTEXTINFO*			GetTextInfo(){ return &m_TextInfo; }
	const TCHAR*		GetText(){ return m_TextInfo.GetText(); }
	int					GetTextLen() { return m_TextInfo.GetLen(); }

	//=======================================================================================
	//
	//		스트링
	//
	//=======================================================================================
protected:
	bool				m_bUseMark;
	TCHAR				m_szMark;	

	CsPoint				m_ptStrSize;
	CsPoint				m_ptPixelSize;

	int					m_nFTSize_HBase;
	sTEXTINFO			m_TextInfo;
	bool				m_bSet;	// 두줄지원


public:	
	bool				GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr );
	CsPoint				GetStringSize(){ return m_ptStrSize; }
	void				SetStringSize( CsPoint pos ){ return m_ptStrSize = pos; }
	CsPoint				GetPixelSize(){ return m_ptPixelSize; }
	void				SetPixelSize( CsPoint pos ){ return m_ptPixelSize = pos; }

	void				SetColorA( NiColorA FontColor /*부하가적다*/ ){ m_TextInfo.s_Color = NiColor( FontColor.r, FontColor.g, FontColor.b ); }
	virtual void		SetColor( NiColor FontColor /*부하가적다*/ ){ m_TextInfo.s_Color = FontColor; }
	NiColor				GetColor(){ return m_TextInfo.s_Color; }

	virtual bool		SetText( TCHAR* szStr /*부하가크다*/,  bool bSet = false );	
	bool				SetText( int nStr );
	void				SetMark( TCHAR szMark ){ m_szMark = szMark; m_bUseMark = true; }



	bool				IsEnableTexture(){ return ( m_pTexture != NULL ); }

#ifdef CONSIGNMENT_SALE
	sTEXTINFO Get_TextInfo() { return m_TextInfo; }
#endif

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
	NiPixelData*		GetPixelData(){return m_pPixelData;}
};