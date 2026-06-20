#pragma once

class cProgressbar
{
public:
	cProgressbar(void);
	~cProgressbar(void);

public:
	void	Delete();
	void	Init( UINT nIDresource, CPoint ptPos, CPoint ptSize );

	void	Update();
	void	Render( CDC* pMemDc, CDC* pTempDC );

	void	SetPersent( float fPer );

protected:
public:
protected:
	CBitmap		m_bmpDefault;	

	CPoint		m_ptPos;
	CPoint		m_ptMaxSize;
	CPoint		m_ptSize;

};
