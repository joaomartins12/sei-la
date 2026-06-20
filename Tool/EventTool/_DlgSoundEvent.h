#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgSoundEvent 대화 상자입니다.

class _DlgSoundEvent : public CDialog
{
	DECLARE_DYNAMIC(_DlgSoundEvent)

public:
	_DlgSoundEvent(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgSoundEvent();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SOUNDLIST };

protected:
	void _FindSubDirSound( CString strDir, CStringArray &FileArray );

protected:
	CStringArray	m_SoundFile;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListCtrl m_ListSound;

public:
	afx_msg void OnLvnItemchangedListsound(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListsound(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListsound(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioMusic();
	afx_msg void OnBnClickedRadioSound();
	CButton m_RadioMusic;
};
