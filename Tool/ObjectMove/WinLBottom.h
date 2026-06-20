#pragma once
#include "afxcmn.h"
#include "afxwin.h"



// WinLBottom 폼 뷰입니다.

class WinLBottom : public CFormView
{
	DECLARE_DYNCREATE(WinLBottom)

public:
	void				_Delete(){}
	void				_Init();

	void				_Update();
	void				_Render(){}


public:
	void				_ResetMap( DWORD dwMapID );
	void				_SetTarget_Npc( CsC_AvObject* pObject );

	bool				_IsAllActEnableMove();

protected:
	void				__ResetActList( bool bKeepSelect );
	void				__EditModeActList();

public:
	void				_SM_Enable_All( bool bEnable );
	void				_SM_Enable_Type( bool bEnable );

	void				_SM_Enable_Pos( bool bEnable );
	bool				_SM_Set_Pos( CsPoint pos );	
	int					_SM_Get_PosX(){ return nsCsMfcFunc::Control2Int( &m_edit_SM_PosX ); }	
	int					_SM_Get_PosY(){ return nsCsMfcFunc::Control2Int( &m_edit_SM_PosY ); }

	void				_SM_Enable_Ani( bool bEnable );
	bool				_SM_Set_Ani( DWORD dwAniID );
	DWORD				_SM_Get_Ani(){ return (DWORD)m_cmb_SM_Ani.GetItemData( m_cmb_SM_Ani.GetCurSel() ); }

	void				_SM_Enable_Time( bool bEnable );
	bool				_SM_Set_Time( int nSec );
	int					_SM_Get_Time(){ return nsCsMfcFunc::Control2Int( &m_edit_SM_Time ); }

	CsMoveObject::sACT_BASE*		GetCurEditAct(){ return m_pEditAct; }

protected:
	CsMoveObject::sACT_BASE*		m_pEditAct;




public:
	bool				_IsPlay();
	void				_PreViewPlay( bool bPlay );
	float				_GetPreViewSpeed(){ return nsCsMfcFunc::Control2Float( &m_cmb_PV_Speed ); }


protected:
	WinLBottom();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinLBottom();

public:
	enum { IDD = IDD_LBottom };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	CComboBox m_cmb_SM;
	CComboBox m_cmb_SM_Type;
	CEdit m_edit_SM_PosX;
	CEdit m_edit_SM_PosY;
	CComboBox m_cmb_SM_Ani;
	CEdit m_edit_SM_Time;
public:
	afx_msg void OnBnClickedButtonSmAdd();
protected:
	CButton m_btn_SM_Add;
public:
	afx_msg void OnCbnSelchangeComboSetMode();
	afx_msg void OnCbnSelchangeComboSmType();
protected:
	CListCtrl m_list_Act;
	CStatic m_static_SMTime;
public:
	afx_msg void OnLvnItemchangedListAct(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListAct(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboSmAni();
	afx_msg void OnEnChangeEditSmTime();
	afx_msg void OnBnClickedButtonDelAllact();
	afx_msg void OnBnClickedButtonPvPlay();
	afx_msg void OnBnClickedButtonPvRewind();
protected:
	CButton m_btn_PV_Play;
	CComboBox m_cmb_PV_Speed;
	CEdit m_edit_PV_Time;
};


extern WinLBottom*		g_pWinLBottom;