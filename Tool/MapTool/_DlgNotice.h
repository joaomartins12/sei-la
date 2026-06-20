#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgNotice 대화 상자입니다.

class _DlgNotice : public CDialog
{
	DECLARE_DYNAMIC(_DlgNotice)

public:
	enum eNOTICE_TYPE
	{
		NT_SHADOWMAP,
		NT_SAVE,
		NT_LOAD,
		NT_PLAY,
		NT_HEIGHT_FIELD,
		NT_SAVE_MINIMAP,
		NT_SAVE_EMRFILE,
		NT_SM_BLUR,
	};
public:
	_DlgNotice( eNOTICE_TYPE ent, void* pConstant1, void* pConstant2 = NULL, void* pConstant3 = NULL, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgNotice();


protected:
	eNOTICE_TYPE	m_eNoticeType;
	int				m_nValue1;
	int				m_nValue2;

	void*			m_pConstant1;
	void*			m_pConstant2;
	void*			m_pConstant3;

	bool			m_bInit;

	NiSingleShaderMaterial* m_pShader_ObjectShadowColor;

protected:
	void			__ShadowMap_Update();
	void			__Save_Update();
	void			__Load_Update();
	void			__Play_Update();
	void			__HeightField_Update();

	void			__SaveMinimap_Update();
	void			__SaveEMRFile_Update();

	void			__ShadowMapBlur_Update();

public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_NOTICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	CStatic m_static_Notice;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void OnOK();
	virtual void OnCancel(){}
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	CProgressCtrl m_pgBar;
};
