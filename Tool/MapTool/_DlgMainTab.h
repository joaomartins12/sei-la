#pragma once
#include "afxcmn.h"


// _DlgMainTab 대화 상자입니다.

class _DlgMainTab : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMainTab)

public:
	_DlgMainTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMainTab();

public:
	virtual void	_Delete();	
	virtual void	_ViewInit();

	virtual void	_PreViewReset();
	virtual void	_PostViewReset();

	virtual void	_UpdateView();
	virtual void	_PreRenderView();
	virtual void	_PostRenderView();

	virtual void	_PreSave( const char* cFile, bool bAutoSave );

	virtual void	_OnSlide_SLeft();
	virtual void	_OnSlide_SRight();
	virtual void	_OnSlide_BLeft();
	virtual void	_OnSlide_BRight();

	virtual bool	_PreCheckKey( DWORD vKeyCode, bool bDown );
	virtual void	OnKeyUp(const DWORD msg);

public:
	enum eTAP_TYPE{ TT_MAIN, TT_MAP, TT_HEIGHT, TT_MAPPING, TT_OBJECT, TT_CELLOBJECT, TT_MONSTOR, TT_MODEL, TT_SHADOW, TT_LIGHT, TT_PLAYER, 
					TT_PATH, TT_WEATHER, TT_ETC, TT_COUNT };

protected:
	_DlgMTMain		m_DlgMTMain;
	_DlgMTMap		m_DlgMTMap;
	_DlgMTHeight	m_DlgMTHeight;
	_DlgMTMapping	m_DlgMTMapping;
	_DlgMTObject	m_DlgMTObject;
	_DlgMTCellObject m_DlgMTCellObject;
	_DlgMTMonster	m_DlgMTMonster;
	_DlgMTModel		m_DlgMTModel;
	_DlgMTShadow	m_DlgMTShadow;
	_DlgMTLight		m_DlgMTLight;
	_DlgMTPlayer	m_DlgMTPlayer;
	_DlgMTPath		m_DlgMTPath;
	_DlgMTWeather	m_DlgMTWeather;
	_DlgMTEtc		m_DlgMTEtc;

	std::map< DWORD, _DlgBase* >	m_mapDlg;

public:
	void		_Reset();
	void		_Resist();	
	CTabCtrl*	_GetTabCtrl(){ return &m_tabMain; }

public:
	int				_GetSelTabIndex(){ return m_tabMain.GetCurSel(); }
	_DlgMTMain*		_GetDlgMain(){ return &m_DlgMTMain; }
	_DlgMTMap*		_GetDlgMap(){ return &m_DlgMTMap; }
	_DlgMTHeight*	_GetDlgHeight(){ return &m_DlgMTHeight; }
	_DlgMTMapping*	_GetDlgMapping(){ return &m_DlgMTMapping; }
	_DlgMTObject*	_GetDlgObejct(){ return &m_DlgMTObject; }
	_DlgMTCellObject*	_GetDlgCellObject(){ return &m_DlgMTCellObject; }
	_DlgMTMonster*	_GetDlgMonster(){ return &m_DlgMTMonster; }
	_DlgMTModel*	_GetDlgModel(){ return &m_DlgMTModel; }
	_DlgMTShadow*	_GetDlgShadow(){ return &m_DlgMTShadow; }
	_DlgMTLight*	_GetDlgLight(){ return &m_DlgMTLight; }
	_DlgMTPlayer*	_GetDlgPlayer(){ return &m_DlgMTPlayer; }
	_DlgMTEtc*		_GetDlgEtc(){ return &m_DlgMTEtc; }
	_DlgMTPath*		_GetDlgPath(){ return &m_DlgMTPath; }

	void		_OnLButtonDown( CPoint pt );
	void		_OnLButtonUp( CPoint pt );
	void		_OnMouseMove( CPoint pt );
	void		_OnMouseWheel( short zDelta );
	void		_OnLButtonDbClick( CPoint pt );

public:
	void		_KeyDownEscape();
	

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MAINTAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CTabCtrl m_tabMain;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
};
