#pragma once

class cBmpButton 
{
public:
	cBmpButton(void);
	~cBmpButton(void);	

public:
	void		Init( CDialog* pkParent, CPoint ptPos, CPoint ptSize, int idDefault, int idUp, int idDown, int idDisable );

	void		Update( eSTATE eMouseState );
	void		Render( CDC* pDc, CDC* pTempDC );		

	void		SetButton( eSTATE eState );
	eSTATE		GetState() { return m_eState; }		
	CPoint		GetPos() { return m_ptPos; }
	
	//CBitmap*	GetCurBmpDefault() { return &m_bmpDefault; }
	CBitmap*	GetCurBmpUp() { return &m_bmpUp; }

protected:
	CDialog*		m_pkParent;

	eSTATE			m_eState;
	CBitmap*		m_pCurBitmap;

	CBitmap			m_bmpDefault;
	CBitmap			m_bmpUp;
	CBitmap			m_bmpDown;
	CBitmap			m_bmpDisable;

	CPoint			m_ptPos;
	CPoint			m_ptSize;
};
