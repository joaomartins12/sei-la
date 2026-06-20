#pragma once
#include "afxwin.h"
#include "afxcmn.h"



// _DlgEtc 폼 뷰입니다.

class _DlgEtc : public CFormView
{
	DECLARE_DYNCREATE(_DlgEtc)

protected:
	_DlgEtc();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~_DlgEtc();

public:
	void		Init();
	void		Update( float fEffTime );
	void		Reset();		
	void		SoundListReset() { m_List_Sound.DeleteAllItems(); }
		
	bool		IsPlay(){ return ( m_Check_Play.GetCheck() == BST_CHECKED ); }
	bool		IsModel(){ return ( m_Check_Model.GetCheck() == BST_CHECKED ); }
	bool		IsLoop(){ return ( m_Check_Loop.GetCheck() == BST_CHECKED ); }
	
	void		SetSldRange( float fEndTime ); 
	void		SetPlay( bool bSet ) { (bSet == true ) ? m_Check_Play.SetCheck( BST_CHECKED ) : m_Check_Play.SetCheck( BST_UNCHECKED ); }
	void		SetOption( CsEffectSound* Option );
	void		SetSoundList( CString strSound, float fTime );
	void		SetTime( LPWSTR szTime ){ m_Edit_Time.SetWindowText( szTime ); }
	void		SetCurEffect( CString str ){ m_Edit_CurEffect.SetWindowText( str ); }
	void		SetControltoData();

	void		GetSoundListData( CsEffectSound::sSound_Info* pData, int nIndex );
	float		GetScale() { return nsCsMfcFunc::Control2Float( &m_Edit_Scale ); }
	int			GetSldPos() { return m_sld_Time.GetPos(); }
	float		GetCurTime() { return m_sld_Time.GetPos()*0.001f; }
	int			GetListCount() { return m_List_Sound.GetItemCount(); }
	
	

public:
	enum { IDD = IDD_DIALOG_ETC };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CButton m_Check_Play;
	CButton m_Check_Loop;
	CButton m_Check_Model;

	CEdit	m_Edit_Time;
	CsEdit	m_Edit_Scale;
	CEdit	m_Edit_CurEffect;
	
	CsSpinCtrl m_spin_Scale;
	CSliderCtrl m_sld_Time;

	CComboBox m_Combo_Live;
	CComboBox m_Combo_Pos;
	CComboBox m_Combo_StartPos;
	CComboBox m_Combo_Dir;

	CsEdit	m_Edit_X;
	CsEdit	m_Edit_Y;
	CsEdit	m_Edit_Z;
	CsSpinCtrl m_spin_X;
	CsSpinCtrl m_spin_Y;
	CsSpinCtrl m_spin_Z;
	
	CButton m_Check_Offset;
	CListCtrl m_List_Sound;	

	bool m_bSort;
	int m_nSortColumn;

public:	
	afx_msg void OnEnChangeEditTime();
	afx_msg void OnBnClickedCheckLoop();	
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedButtonReoption();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLvnItemchangedListSoundlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListSoundlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelsound();	
	afx_msg void OnBnClickedCheckPlay();		
};


