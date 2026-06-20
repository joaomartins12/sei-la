#pragma once

class cStrButton
{
public:
	cStrButton(void);
	~cStrButton(void);

protected:
	CDialog*	m_pkParent;

	TCHAR		m_szStr[ 64 ];
	CPoint		m_ptPos;
	CPoint		m_ptSize;
	CRect		m_rc;
	COLORREF	m_nColor;
	COLORREF	m_nOverColor;

	CFont		m_font;
	eSTATE		m_eState;

public:
	void		Init( CDialog* pkParent, CPoint ptPos, TCHAR* szStr, bool bUndetline, COLORREF nColor, COLORREF nOverColor, int nFontSize = 14 );

	void		Update( eSTATE eMouseState );
	void		Render( CDC* pDC );	
	void		ResetArea( CRect rc ) { m_rc = rc; }

	eSTATE		GetState() { return m_eState; }
};
