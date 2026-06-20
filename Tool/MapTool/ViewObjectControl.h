
#pragma once 

class CViewObjectControl : public CViewBase
{
protected:
	_DlgObjectControl	m_DlgObjectControl;
public:
	CViewObjectControl();
	~CViewObjectControl();

public:	
	virtual void		_Init();
	virtual _DlgBase*	_GetDlg(){ return &m_DlgObjectControl; }
};