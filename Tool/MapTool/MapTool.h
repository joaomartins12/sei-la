// MapTool.h : MapTool 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.


namespace nsMAPTOOL
{
	#define VIEWER_WIDTH		800
	#define VIEWER_HEIGHT		600
	enum eVIEW
	{				
		MV_MAINTAB	=	1,
		MV_NODE,
		MV_OBJECT_CONTROL,
		MV_OBJECT_LIST,
		MV_SETUP,
		MV_VIEWER,				// 항상 가장 끝에 와야한다
	};
}

// CMapToolApp:
// 이 클래스의 구현에 대해서는 MapTool.cpp을 참조하십시오.

typedef	std::map< DWORD, CFrameBase* >					MAP_CHILDFRM;
typedef	std::map< DWORD, CFrameBase* >::iterator		MAP_CHILDFRM_IT;

class CMapToolApp : public CWinApp
{
public:
	CMapToolApp();

protected:
	MAP_CHILDFRM		m_mapFrame;
	std::list< DWORD >	m_listWindow;
public:
	void			_Init();
	CFrameBase*		_NewFrameWnd( DWORD ChildID, LPCTSTR Caption, CRect rcView, bool bStatic, int nCmd = SW_SHOW );
	void			_SortWindow( DWORD ChildID );

	MAP_CHILDFRM*	_GetMapFrame(){ return &m_mapFrame; }	


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCameraReset();
	afx_msg void OnViewGridlinetoggle();
	afx_msg void ShowPathObjectToggle(void);
	afx_msg void ShowMonsterRegionToggle(void);
public:
	afx_msg void OnConfig();
public:
	afx_msg void OnFileLoadas();
public:
	afx_msg void OnFileSaveas();
	bool GetShowObj(void);
	bool GetShowMonsterRegion(void);
	void SetShowPathObj(bool TOF);
	void SetShowMonsterRegion(bool TOF);

	bool ShowPathObj;
	bool ShowMosterRegion;
};

extern CMapToolApp theApp;
