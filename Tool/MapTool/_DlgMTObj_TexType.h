#pragma once


// _DlgMTObj_TexType 대화 상자입니다.

class _DlgMTObj_TexType : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTObj_TexType)


protected:
	CsGBObject*		m_pCsGBObject;
protected:
	void		__ResetTextureType();








public:
	_DlgMTObj_TexType(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTObj_TexType();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_OBJECT_TEXTYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CComboBox m_combo_TextureType[ 8 ];	
public:
	afx_msg void OnCbnSelchangeComboTextureType0();
	afx_msg void OnCbnSelchangeComboTextureType1();
	afx_msg void OnCbnSelchangeComboTextureType2();
	afx_msg void OnCbnSelchangeComboTextureType3();
	afx_msg void OnCbnSelchangeComboTextureType4();
	afx_msg void OnCbnSelchangeComboTextureType5();
	afx_msg void OnCbnSelchangeComboTextureType6();
	afx_msg void OnCbnSelchangeComboTextureType7();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
