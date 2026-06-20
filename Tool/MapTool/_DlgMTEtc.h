#pragma once
#include "afxwin.h"


// _DlgMTEtc 대화 상자입니다.

class _DlgMTEtc : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTEtc)

	// ============ 전체 관리
public:
	virtual void			_Delete();

	// ============ 미니맵

	// ============ 필드 이동 가능 지역 ( Field Enable Move Region )
public:
	struct sEMR_INFO
	{
		int		s_nWidth;
		int		s_nHeight;
		int		s_nGridSize;
		int		s_nXFieldCount;
	};
	enum eEMR_TYPE
	{
		ET_NOMOVE = 0,
		ET_ENABLE_MOVE,
	};

public:
	_DlgMTEtc(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTEtc();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_ETC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMakeMinimap();
protected:
	CsEditFosus m_edit_FEMRDist;
public:
	afx_msg void OnBnClickedButtonMakeFemr();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonOpenDialog();
	afx_msg void OnBnClickedButtonOpenDialog2();
	afx_msg void OnBnClickedButtonTgaToEmr();
	afx_msg void OnBnClickedButtonEmrToTga();
	CEdit m_desttgaName;
	CEdit m_destemrName;
};
