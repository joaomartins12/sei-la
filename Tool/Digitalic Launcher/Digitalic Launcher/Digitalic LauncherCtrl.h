#pragma once

// Digitalic LauncherCtrl.h : CDigitalicLauncherCtrl ActiveX 컨트롤 클래스의 선언입니다.


// CDigitalicLauncherCtrl : 구현을 보려면 Digitalic LauncherCtrl.cpp을(를) 참조하십시오.


#include "WinInet.h"

class CDigitalicLauncherCtrl : public COleControl
{
	DECLARE_DYNCREATE(CDigitalicLauncherCtrl)

// 생성자입니다.
public:
	CDigitalicLauncherCtrl();

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// 구현입니다.
protected:
	~CDigitalicLauncherCtrl();

	DECLARE_OLECREATE_EX(CDigitalicLauncherCtrl)    // 클래스 팩터리와 GUID입니다.
	DECLARE_OLETYPELIB(CDigitalicLauncherCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CDigitalicLauncherCtrl)     // 속성 페이지 ID입니다.
	DECLARE_OLECTLTYPE(CDigitalicLauncherCtrl)		// 형식 이름과 기타 상태입니다.

// 메시지 맵입니다.
	DECLARE_MESSAGE_MAP()

// 디스패치 맵입니다.
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// 이벤트 맵입니다.
	DECLARE_EVENT_MAP()

// 디스패치와 이벤트 ID입니다.
public:
	enum {
		dispidGetFileVersion = 3L,
		dispidIsGameLaunching = 2L,
		dispidGameLaunching = 1L
	};

protected:	
	HINTERNET							m_hSession;	

protected:
	VARIANT_BOOL GameLaunching(LPCTSTR szParam);
	VARIANT_BOOL IsGameLaunching(void);
	BSTR GetFileVersion(void);

	void		Setup();
	
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
};

