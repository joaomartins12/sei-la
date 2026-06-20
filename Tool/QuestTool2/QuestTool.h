// QuestTool2.h : QuestTool2 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif




// CQuestTool2App:
// 이 클래스의 구현에 대해서는 QuestTool2.cpp을 참조하십시오.
//

class CQuestTool2App : public CWinApp
{
public:
	CQuestTool2App();


// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CQuestTool2App theApp;