
#pragma once

class _DlgBase : public CDialog
{
public:
	struct sCOLORDLG_INFO
	{
		sCOLORDLG_INFO(){ s_bInit = false; }

		void		Delete(){ s_Brush.DeleteObject(); }
		void		Create( CWnd* pWnd, NiColor color ){
			if( s_bInit == true )
				Delete();
			s_Color = color;
			s_Brush.CreateSolidBrush( RGB( CsF2B( s_Color.r ), CsF2B( s_Color.g ), CsF2B( s_Color.b ) ) );
			pWnd->InvalidateRect( NULL );
			s_bInit = true;
		}
		bool		OpenDlg( CWnd* pWnd ){
			CColorDialog colorDlg( RGB( CsF2B( s_Color.r ), CsF2B( s_Color.g ), CsF2B( s_Color.b ) ), CC_FULLOPEN | CC_SOLIDCOLOR );
			if( colorDlg.DoModal() == IDCANCEL )
				return false;
			COLORREF color = colorDlg.GetColor();
			s_Color = NiColor( CsB2F( GetRValue( color ) ), CsB2F( GetGValue( color ) ), CsB2F( GetBValue( color ) ) );
			Delete();
			Create( pWnd, s_Color );
			return true;
		}
		bool		s_bInit;
		NiColor		s_Color;
		CBrush		s_Brush;
	};

public:
	_DlgBase( UINT nIDTemplate, CWnd* pParentWnd = NULL ):CDialog( nIDTemplate, pParentWnd ){ m_nDlgID = nIDTemplate; }

public:
	virtual void		_Delete(){}
	virtual void		_ViewInit(){}

	virtual void		_PreViewReset(){}
	virtual void		_PostViewReset(){}

	virtual void		_UpdateView(){}
	virtual void		_PreRenderView(){}
	virtual void		_PostRenderView(){}

	virtual void		_PreSave( const char* cFile, bool bAutoSave ){}

	virtual void		_KeyDownEscape(){}
	virtual void		_OnLButtonDbClick( CPoint pt ) {};
	virtual void		_OnLButtonDown( CPoint pt ){}
	virtual void		_OnLButtonUp( CPoint pt ){}
	virtual void		_OnMouseMove( CPoint pt ){}
	virtual void		_OnMouseWheel( short zDelta ){}
	virtual void		OnKeyUp(const DWORD msg){}

	virtual void		_OnSlide_SLeft(){}
	virtual void		_OnSlide_SRight(){}
	virtual void		_OnSlide_BLeft(){}
	virtual void		_OnSlide_BRight(){}

	virtual bool		_PreCheckKey( DWORD vKeyCode, bool bDown ){ return false; }

protected:
	UINT				m_nDlgID;
public:
	UINT				_GetDlgID(){ return m_nDlgID; }
};

