#pragma once

struct sStrInfo
{	
	sStrInfo() { s_bOutline = s_bBold = s_bUnderline = false; s_nFontSize = 14; s_nColor = FONT_WHITE; memset( s_sz, 0, sizeof( TCHAR ) * 128 ); }

	void	Init( TCHAR* sz, CPoint ptPos ) { s_ptPos = ptPos;	_tcscpy_s( s_sz, 128, sz ); }
	void	ChangeStr( TCHAR* sz );

	CPoint		s_ptPos;
	int			s_nFontSize;
	bool		s_bOutline;	
	bool		s_bBold;
	bool		s_bUnderline;
	COLORREF	s_nColor;		
	TCHAR		s_sz[ 128 ];
};

class cStrMng
{
public:
	cStrMng(void);
	~cStrMng(void);

public:
	void	Delete();
	void	Init();

	void	Update();
	void	Render( CDC* pDC );
	
	void	SetText( sStrInfo* pStrInfo ) { m_strList.push_back( pStrInfo ); }

protected:
	std::list< sStrInfo* >		m_strList;	
};

extern cStrMng* g_pStrMng;
