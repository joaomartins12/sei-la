#pragma once
#include "afxcmn.h"



// WinDiscript 폼 뷰입니다.

class WinDiscript : public CFormView
{
	DECLARE_DYNCREATE(WinDiscript)

public:
	enum eTAB_TYPE{ TT_SETTING, TT_TEXT, TT_EVENT, };


public:
	void				_Init();

	void				_SetSelQuest( cQuestObj* pQuestObj );
	cQuestObj*			_GetSelQuest(){ return m_pSelQuest; }


protected:
	cQuestObj*			m_pSelQuest;

	_DlgTab_Setting		m_DlgSetting;
	_DlgTab_Text		m_DlgText;
	_DlgTab_Event		m_DlgEvent;



protected:
	WinDiscript();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinDiscript();

public:
	enum { IDD = IDD_DIALOG_DISCRIPT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CTabCtrl m_MainTab;
public:
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
};


extern WinDiscript*		g_pDiscript;