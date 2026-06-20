#pragma once
#include "IntegrityTool.h"

// _DlgResourceView 대화 상자입니다.

class _DlgResourceView : public CDialog
{
	DECLARE_DYNAMIC(_DlgResourceView)

public:
	_DlgResourceView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgResourceView();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_HASHLIST };

	void			SetHashData( MAP_HASH_CONTANER const & RecvHashData );

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	BOOL			OnInitDialog();

	DECLARE_MESSAGE_MAP()
	void			_UpdateItemCount();
public:
	afx_msg void	OnBnClickedCancel();
	afx_msg void	OnListKeyDown( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnDelListKeyDown( NMHDR* pNMHDR, LRESULT* pResult );

private:

private:
	CListCtrl			m_list;
	CListCtrl			m_listDel;
	CStatic				m_OrgCount;
	CStatic				m_DelCount;
	MAP_HASH_CONTANER	recvHash;
public:
	afx_msg void OnSelectedItemDelete();
	afx_msg void OnDelListSelectedItemDelete();
	afx_msg void OnBnClickedOrgBtn();
	afx_msg void OnBnClickedHashUpdate();
};
