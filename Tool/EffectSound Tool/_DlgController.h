#pragma once
#include "afxcmn.h"
#include "afxwin.h"



// _DlgController 폼 뷰입니다.

class _DlgController : public CFormView
{
	DECLARE_DYNCREATE(_DlgController)

protected:
	_DlgController();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~_DlgController();

public:
	void		Init();	
	void		FindSerch();
	void		FindSubDirEffect( CString strDir, CStringArray &FileArray );
	void		FindSubDirSound( CString strDir, CStringArray &FileArray );
	

public:
	CStringArray	m_EffectFile;
	CStringArray	m_SoundFile;

public:
	enum { IDD = IDD_DIALOG_EFFECT };
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
	CListCtrl m_List_EffectList;		
	CListCtrl m_List_Sound;
	
	CEdit m_EditSerch;

	int m_nSortColumn;
	bool m_bSort;

	int m_nSortColumn1;
	bool m_bSort1;
	
public:		
	afx_msg void OnLvnItemchangedListEffect(NMHDR *pNMHDR, LRESULT *pResult);		
	afx_msg void OnLvnItemchangedListSoundtlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonFileeffect();
	afx_msg void OnBnClickedButtonFilesound();
	afx_msg void OnBnClickedButtonInsertsound();
	afx_msg void OnBnClickedCheckInput();
	afx_msg void OnEnChangeEditTime();
	afx_msg void OnLvnColumnclickListSoundtlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListEffectlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListSoundtlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListEffectlist(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnBnClickedButtonSerch();
};


