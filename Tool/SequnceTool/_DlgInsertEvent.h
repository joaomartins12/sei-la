#pragma once
#include "afxwin.h"


// _DlgInsertEvent 대화 상자입니다.

class _DlgInsertEvent : public CDialog
{
	DECLARE_DYNAMIC(_DlgInsertEvent)

protected:
	int*		m_nType;
	char*		m_cTextKey;
	int*		m_nStaticIndex;
	int			m_nSelectList;

	struct sFE : public NiMemObject
	{
		char		s_cFile[ MAX_TEXTKEY_LEN ];
	};
	std::list< sFE* >	m_listFileEffect;

protected:
	void		_SetDyEffect();
	void		_SetDySound();

	bool		_GetEffectFileOpen( char* cFile );
	bool		_GetSoundFileOpen( char* cFile );
	bool		_GetLanguageSoundFileOpen( char* cFile, TCHAR* szLanguage );

	void		_ReadListFileEffect();
	void		_DeleteListFileEffect();
	void		_SaveListFileEffect();



public:
	virtual void		OnOK();
	virtual void		OnCancel();

public:
	_DlgInsertEvent( int* nType, char* cTextKey, int* nStaticIndex, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgInsertEvent();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_INSERT_EVENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonEffectFile();
	afx_msg void OnBnClickedButtonSoundFile();
protected:
	CListBox m_list_Effect;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnDblclkListDyEffect();
	afx_msg void OnLbnDblclkListDySound();
protected:
	CListBox m_list_Sound;
protected:
	CListBox m_list_FE;
public:
	afx_msg void OnBnClickedButtonFeDel();
	afx_msg void OnBnClickedButtonFeLoad();
	afx_msg void OnLbnDblclkListFeList();
};
