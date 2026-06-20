#pragma once


// _DlgMTHeight_Polygon 대화 상자입니다.

class _DlgMTHeight_Polygon : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTHeight_Polygon)














public:
	_DlgMTHeight_Polygon(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTHeight_Polygon();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_HEIGHT_POLYGON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
