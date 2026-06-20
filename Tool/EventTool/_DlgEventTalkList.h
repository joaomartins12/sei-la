#pragma once
#include "afxwin.h"


// _DlgEventTalkList 대화 상자입니다.

class _DlgEventTalkList : public CDialog
{
	DECLARE_DYNAMIC(_DlgEventTalkList)

public:
	_DlgEventTalkList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgEventTalkList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TALKLIST };

public:
	void	Init();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListBox m_EventTalkList;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnDblclkListTalklist();
};
