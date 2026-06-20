
#pragma once 

class CViewObjectList : public CViewBase
{
protected:
	_DlgObjectList	m_DlgObjectList;
public:
	CViewObjectList();
	~CViewObjectList();

public:	
	virtual void		_Init();
	virtual _DlgBase*	_GetDlg(){ return &m_DlgObjectList; }
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};