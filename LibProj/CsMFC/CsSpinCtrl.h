
#pragma once 


typedef void(*CsSpinCtrlMoveCallBack)( float fDelta );
typedef void(*CsSpinCtrlDownCallBack)( float fDelta );
typedef void(*CsSpinCtrlUpCallBack)( float fDelta );
class CsSpinCtrl : public CSpinButtonCtrl
{
public:
	CsSpinCtrl();

public:
	static int		m_nDispHeight;
	int				m_nDeltaPixel;

	// ====== Base
protected:
	bool			m_bDown;
	bool			m_bRange;
	CPoint			m_ptMouse;
	CsEdit*			m_pCsEdit;
	int				m_nDispPage;

public:
	void			SetCsDeltaPixel( int nDeltaPixel ){ m_nDeltaPixel = nDeltaPixel; }

	// 함수 포인터
protected:
	void		(CsSpinCtrl::*fPushButton)( POINT pt );
	void		(CsSpinCtrl::*fPushMove)( POINT pt );

	// 콜백 함수
	CsSpinCtrlMoveCallBack		m_CallBackMove;	
	CsSpinCtrlDownCallBack		m_CallbackMouseDown;
	CsSpinCtrlUpCallBack		m_CallbackMouseUp;
public:
	void			ResistCsMoveCallBack( void* func ){ m_CallBackMove = (CsSpinCtrlMoveCallBack)func; }
	void			ResistCsMouseDownCallback( void* func ){ m_CallbackMouseDown = (CsSpinCtrlDownCallBack)func; }
	void			ResistCsMouseUpCallback( void* func ){ m_CallbackMouseUp = (CsSpinCtrlUpCallBack)func; }

	// ===== int
protected:
	int			m_nAccel;
	int			m_nMin;
	int			m_nMax;
	int			m_nCurValue;
public:
	void		SetCsBuddy( CsEdit* pBuddyEdit, int nAccel );
	void		SetCsRange( int nMin, int nMax );
	void		SetCsAccel( int nAccel ){
		fPushButton			=	&CsSpinCtrl::_PushButtonInt;
		fPushMove			=	&CsSpinCtrl::_PushMoveInt;
		m_nAccel = nAccel;
	}

protected:
	// 버튼이 눌려 졌다면
	void			_PushButtonInt( POINT pt );
	// 눌린채로 움직인다면
	virtual void	_PushMoveInt( POINT pt );	

	// ===== float
protected:
	int			m_nDecimalPlace;
	float		m_fAccel;
	float		m_fMin;
	float		m_fMax;
	float		m_fCurValue;
public:
	void		SetCsBuddy( CsEdit* pBuddyEdit, float fAccel, int nDecimalPlace );
	void		SetCsRange( float fMin, float fMax );
	void		SetCsAccel( float fAccel ){
		m_fAccel = fAccel;
		fPushButton			=	&CsSpinCtrl::_PushButtonFloat;
		fPushMove			=	&CsSpinCtrl::_PushMoveFloat;
	}

protected:
	// 버튼이 눌려 졌다면
	void		_PushButtonFloat( POINT pt );
	// 눌린채로 움직인다면
	virtual void	_PushMoveFloat( POINT pt );

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};