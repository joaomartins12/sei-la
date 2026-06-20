#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgMTMonster 대화 상자입니다.

class _DlgMTMonster : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMonster)

public:
	_DlgMTMonster(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMonster();


	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual	void			_Delete();
	virtual void			_ViewInit();

	virtual void			_PostViewReset();

	virtual void			_UpdateView();
	virtual void			_PostRenderView();

	virtual void			_PreSave( const char* cFile, bool bAutoSave );

	virtual void			_KeyDownEscape();
	virtual void			_OnLButtonDown( CPoint pt );
	virtual void			_OnMouseMove( CPoint pt );

	// =================================================================================================
	//
	//		Monster Table
	//
	// =================================================================================================
public:
	void				_ResetMapID();

protected:
	void				__WriteList_MonTable();
	void				__InsertMonTable( CsMonster* pMonster );
	void				__Sort_MonTable( int nColume, bool bIncrease );

	// =================================================================================================
	//
	//		Monster Info
	//
	// =================================================================================================
public:
	struct sMON_INFO
	{
		sMON_INFO(){ s_pModelRes = NULL; }
		void			Init();
		void			Delete();
		sMON_INFO*		Clone();

		CsMapMonster::sINFO	s_TableInfo;
		cModel_Res*			s_pModelRes;
	};
	CsMemPool< sMON_INFO >							m_mempoolMonInfo;
	typedef std::list< sMON_INFO* >					LIST_MONINFO;
	typedef std::list< sMON_INFO* >::iterator		LIST_MONINFO_IT;

protected:
	void				__SetMonInfo( sMON_INFO* pMonInfo );

	// =================================================================================================
	//
	//		PreView
	//
	// =================================================================================================
protected:
	bool				m_bRenderPreview;
	sMON_INFO			m_PreView;

protected:
	void				__UpdatePreview();
	void				__RenderPreview();
	bool				__IsPreview(){ return ( m_bRenderPreview && ( m_PreView.s_pModelRes != NULL ) ); }

	void				__SetPreview( DWORD dwMonstorTableID );	
	void				__ReleasePreview();

	// =================================================================================================
	//
	//		Resist
	//
	// =================================================================================================
protected:
	LIST_MONINFO		m_listMonInfo;
	sMON_INFO*			m_pSelectResist;

protected:
	void				__DeleteResist();
	void				__InitResist();

	void				__UpdateResist();


	bool				__AddResist( CsMapMonster::sINFO* pInfo );
	void				__DeleteSelectResist();
	void				__ReleaseSelectResist();
	
	sMON_INFO*			__SelectResist( DWORD dwIndex );
	void				__SelectResist( sMON_INFO* pSelectMon );	
	int					__GetSelectResistIndex();	

	void				__PreviewToResist();
	void				__ResetResistMonstorList();

	void				__FileTableToResist();
public:
	void				__RenderResist(bool RenderMonster = true);
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MONSTER };

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListCtrl m_list_Monster;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	CListBox m_list_ResistMonstor;
	CSliderCtrl m_sld_RegionSize;
	CSliderCtrl m_sld_Def_RegionSize;
	CSliderCtrl m_sld_MonScale;
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CsEditFosus m_edit_RegionSize;
	CsEditFosus m_edit_Def_RegionSize;
	CsEditFosus m_edit_MonScale;
public:
	afx_msg void OnBnClickedButtonResistdelete();
	afx_msg void OnLbnSelchangeListResistMonstor();
protected:
	CsEditFosus m_edit_MonCount;
	CsEditFosus m_edit_MonResponTime;
	CsEditFosus m_edit_Def_MonCount;
	CsEditFosus m_edit_Def_MonResponTime;
	CsEditFosus m_edit_Move;
	CsEditFosus m_edit_Def_Move;	
	CButton		m_check_InsRegen;//인던에서 몬스터 리젠 가능하지 여부
	CToolTipCtrl  *m_pCtrlTT;
public:
	afx_msg void OnEnChangeEditMoncount();
	afx_msg void OnEnChangeEditMonrespontime();
	afx_msg void OnEnChangeEditDefMoncount();
	afx_msg void OnEnChangeEditDefMonrespontime();
	afx_msg void OnNMClickListMonster(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListMonster(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonResistalldelete();
	afx_msg void OnEnChangeEditMove();	
	afx_msg void OnEnChangeEditDefMove();
protected:
	CsEditFosus m_edit_Killgen_MonID;
	CsEditFosus m_edit_Killgen_Count;
	CsEditFosus m_edit_Killgen_ViewCnt;
	CsEditFosus m_edit_Killgen_Disp;
public:
	afx_msg void OnEnChangeEditKillgenId();
	afx_msg void OnEnChangeEditKillgenCount();
	afx_msg void OnEnChangeEditKillgenViewcnt();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedInsRegenType();
};
