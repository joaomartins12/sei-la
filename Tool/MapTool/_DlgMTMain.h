#pragma once
#include "afxwin.h"

#pragma pack( push )
#pragma pack( 4 )

// _DlgMTMain 대화 상자입니다.

class _DlgMTMain : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMain)

public:
	_DlgMTMain(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMain();

public:
	struct sCMI_HEADER
	{
		int		s_nRootInfo_Size;
	};

	struct sCMI_ROOTINFO 
	{
		int		s_nXVertCount;
		int		s_nYVertCount;
		int		s_nGridSize;
		int		s_nTotalVertCount;
	};

	struct sCMI_OBJECTINFO
	{
		void Reset(){
			s_ptMinVert = NiPoint3( FLT_MAX, FLT_MAX, FLT_MAX );
			s_ptMaxVert = NiPoint3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
			s_cObjName[ 0 ] = NULL;
		}
		NiPoint3	s_ptPos;
		NiPoint3	s_ptRot;
		NiPoint3	s_ptMinVert;
		NiPoint3	s_ptMaxVert;
		char		s_cObjName[ MAX_FILENAME ];
	};

protected:
	void		__ReadMapInfo();

protected:	
	void		__InsertList( LPCTSTR str );
	void		__InsertList( char* c );
	void		__InsertList( LPCTSTR str, int val );
	void		__InsertList( LPCTSTR str, int val, int val2 );
	void		__InsertList( LPCTSTR str, float val );
	void		__InsertList( LPCTSTR str, float val, float val2 );
	void		__InsertList( LPCTSTR str, LPCTSTR str2 );
	void		__InsertList( char* c1, char* c2 );

public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CListBox m_list_MapInfo;
public:
	afx_msg void OnBnClickedBtnNew();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnPatchNote();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnMakeCifile();
	afx_msg void OnBnClickedBtnSaveas();
	afx_msg void OnBnClickedBtnTrMerge();
	afx_msg void OnBnClickedBtnReload();
};

#pragma pack( pop )
